/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include <dali/integration-api/render-controller.h>
#include <dali/internal/common/lockless-pointer-ring.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/update/queue/update-message-queue.h>

using namespace Dali;
using namespace Dali::Internal;

namespace
{
/**
 * A watchdog that sets an std::atomic<bool> flag to true after a timeout, from
 * a separate thread, so that producer/consumer loops which only check the flag
 * as a secondary exit condition can actually be interrupted while still running
 * (rather than the flag only ever being set after both worker threads have
 * already been joined, which makes it dead code).
 *
 * Without this, a genuine regression that stalls the producer or consumer turns
 * into a test binary that hangs forever, rather than a clean, fast test failure.
 *
 * (Same shape as the watchdog in the LocklessMessageBuffer UTCs - kept identical
 * so the two suites read the same way.)
 */
class TestWatchdog
{
public:
  explicit TestWatchdog(std::atomic<bool>& stopFlag, std::chrono::milliseconds timeout)
  : mStopFlag(stopFlag),
    mCancelled(false),
    mThread([this, timeout]()
            {
      std::unique_lock<std::mutex> lock(mMutex);
      if(!mCondition.wait_for(lock, timeout, [this] { return mCancelled; }))
      {
        mStopFlag.store(true);
      } })
  {
  }

  ~TestWatchdog()
  {
    {
      std::lock_guard<std::mutex> lock(mMutex);
      mCancelled = true;
    }
    mCondition.notify_all();
    mThread.join();
  }

private:
  std::atomic<bool>&      mStopFlag;
  std::mutex              mMutex;
  std::condition_variable mCondition;
  bool                    mCancelled;
  std::thread             mThread;
};

/**
 * Minimal RenderController stub for MessageQueue tests. Counts calls so tests
 * can verify MessageQueue requests a render/process-on-idle when expected.
 */
class MyTestRenderController : public Integration::RenderController
{
public:
  void RequestUpdate()
  {
    ++updateRequestCount;
  }

  void RequestProcessEventsOnIdle()
  {
    ++processOnIdleRequestCount;
  }

  int updateRequestCount{0};
  int processOnIdleRequestCount{0};
};

// A trivial heap object the ring can carry pointers to. Not a MessageBase - the
// ring is a pure pointer container and knows nothing about message semantics, so
// the direct ring tests deliberately use the simplest possible payload.
struct Payload
{
  explicit Payload(int v)
  : value(v)
  {
  }
  int value;
};

// Placement-constructible message for MessageQueue::ReserveMessageSlot tests.
struct QueueTestMessage : public MessageBase
{
  explicit QueueTestMessage(int v)
  : value(v)
  {
  }

  void Process() override
  {
    if(processedCounter)
    {
      ++(*processedCounter);
    }
  }

  ~QueueTestMessage() override = default;

  int               value;
  std::atomic<int>* processedCounter = nullptr;
};

// Message that records the order in which it is Process()'d, under a mutex since
// Process() runs on the consumer thread but the result is checked from the main
// thread after draining. Used to detect cross-channel reordering between the
// lockless ring and the mutex-protected overflow queue.
struct OrderedTestMessage : public MessageBase
{
  OrderedTestMessage(int id, std::vector<int>* order, std::mutex* orderMutex)
  : id(id),
    order(order),
    orderMutex(orderMutex)
  {
  }

  void Process() override
  {
    std::lock_guard<std::mutex> lock(*orderMutex);
    order->push_back(id);
  }

  ~OrderedTestMessage() override = default;

  int               id;
  std::vector<int>* order;
  std::mutex*       orderMutex;
};

} // namespace

// These mirror the constants in update-message-queue.cpp. so the tests can
// deterministically drive the ring to full and the overflow queue past its
// backpressure threshold using only normal-sized messages - rather than
// relying on an oversized single message, which trips MessageBuffer's own
// internal single-slot capacity assertion (a MessageBuffer-internal limit
// unrelated to the ring/overflow logic under test).
// If the production constants change, update these to match.
namespace
{
constexpr int TEST_LOCKLESS_RING_CAPACITY                = 32; ///< LOCKLESS_RING_CAPACITY
constexpr int TEST_MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE = 24; ///< MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE

// Reserve one small message that requires no scene update, place it, and flush.
// Returns FlushQueue()'s result. Each call produces exactly one buffer handed to
// the queue (ring on the fast path, or overflow once the ring is full / overflow
// is already active).
bool ReserveAndFlushOne(Internal::Update::MessageQueue& queue, int id, std::vector<int>* order, std::mutex* orderMutex, bool updateScene = false)
{
  constexpr uint32_t SMALL_SIZE = static_cast<uint32_t>(sizeof(OrderedTestMessage));
  uint32_t*          slot       = queue.ReserveMessageSlot(SMALL_SIZE, updateScene);
  if(!slot)
  {
    return false;
  }
  new(slot) OrderedTestMessage(id, order, orderMutex);
  return queue.FlushQueue();
}
} // namespace

void utc_dali_internal_lockless_pointer_ring_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_lockless_pointer_ring_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
// LocklessPointerRing - direct unit tests
///////////////////////////////////////////////////////////////////////////////

int UtcDaliInternalLocklessPointerRingConstructorEmpty(void)
{
  tet_infoline("Test LocklessPointerRing is empty on construction and reports its compile-time capacity");

  TestApplication application;

  LocklessPointerRing<Payload, 8> ring;

  DALI_TEST_EQUALS(ring.GetCapacity(), static_cast<std::size_t>(8), TEST_LOCATION);
  DALI_TEST_EQUALS(ring.GetApproximateDepth(), static_cast<std::size_t>(0), TEST_LOCATION);

  // Draining an empty ring yields nothing and does not touch the output array.
  Payload*    out[8] = {nullptr};
  std::size_t count  = ring.Drain(out, 8);
  DALI_TEST_EQUALS(count, static_cast<std::size_t>(0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingPushDrainSingle(void)
{
  tet_infoline("Test a single Push() followed by Drain() returns the same pointer and empties the ring");

  TestApplication application;

  LocklessPointerRing<Payload, 8> ring;
  Payload                         payload(42);

  DALI_TEST_CHECK(ring.Push(&payload));
  DALI_TEST_EQUALS(ring.GetApproximateDepth(), static_cast<std::size_t>(1), TEST_LOCATION);

  Payload*    out[8] = {nullptr};
  std::size_t count  = ring.Drain(out, 8);
  DALI_TEST_EQUALS(count, static_cast<std::size_t>(1), TEST_LOCATION);
  DALI_TEST_CHECK(out[0] == &payload);
  DALI_TEST_EQUALS(out[0]->value, 42, TEST_LOCATION);

  // Ring is empty again after draining.
  DALI_TEST_EQUALS(ring.GetApproximateDepth(), static_cast<std::size_t>(0), TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingFifoOrder(void)
{
  tet_infoline("Test Drain() returns pushed pointers in FIFO order");

  TestApplication application;

  LocklessPointerRing<Payload, 8> ring;

  std::vector<Payload> payloads;
  payloads.reserve(6);
  for(int i = 0; i < 6; ++i)
  {
    payloads.emplace_back(i);
  }
  for(int i = 0; i < 6; ++i)
  {
    DALI_TEST_CHECK(ring.Push(&payloads[i]));
  }

  Payload*    out[8] = {nullptr};
  std::size_t count  = ring.Drain(out, 8);
  DALI_TEST_EQUALS(count, static_cast<std::size_t>(6), TEST_LOCATION);
  for(int i = 0; i < 6; ++i)
  {
    DALI_TEST_EQUALS(out[i]->value, i, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingPushFull(void)
{
  tet_infoline("Test Push() returns false once the ring is full and the caller keeps ownership");

  TestApplication application;

  constexpr std::size_t                  CAPACITY = 4;
  LocklessPointerRing<Payload, CAPACITY> ring;

  std::vector<Payload> payloads;
  payloads.reserve(CAPACITY + 1);
  for(std::size_t i = 0; i < CAPACITY + 1; ++i)
  {
    payloads.emplace_back(static_cast<int>(i));
  }

  // First CAPACITY pushes succeed.
  for(std::size_t i = 0; i < CAPACITY; ++i)
  {
    DALI_TEST_CHECK(ring.Push(&payloads[i]));
  }
  DALI_TEST_EQUALS(ring.GetApproximateDepth(), CAPACITY, TEST_LOCATION);

  // The next push must fail (ring full) - the caller still owns payloads[CAPACITY].
  DALI_TEST_CHECK(!ring.Push(&payloads[CAPACITY]));
  DALI_TEST_EQUALS(ring.GetApproximateDepth(), CAPACITY, TEST_LOCATION);

  // After draining one slot, a push succeeds again.
  Payload*    out[CAPACITY] = {nullptr};
  std::size_t drained       = ring.Drain(out, 1); // drain just one
  DALI_TEST_EQUALS(drained, static_cast<std::size_t>(1), TEST_LOCATION);
  DALI_TEST_CHECK(ring.Push(&payloads[CAPACITY]));

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingPartialDrain(void)
{
  tet_infoline("Test Drain() honours maxOut and leaves the remaining pointers pending in FIFO order");

  TestApplication application;

  LocklessPointerRing<Payload, 8> ring;

  std::vector<Payload> payloads;
  payloads.reserve(5);
  for(int i = 0; i < 5; ++i)
  {
    payloads.emplace_back(i);
  }
  for(int i = 0; i < 5; ++i)
  {
    DALI_TEST_CHECK(ring.Push(&payloads[i]));
  }

  // Drain only 2 of the 5 pending.
  Payload*    out[2] = {nullptr};
  std::size_t count  = ring.Drain(out, 2);
  DALI_TEST_EQUALS(count, static_cast<std::size_t>(2), TEST_LOCATION);
  DALI_TEST_EQUALS(out[0]->value, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(out[1]->value, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(ring.GetApproximateDepth(), static_cast<std::size_t>(3), TEST_LOCATION);

  // The remaining 3 come out next, still in order.
  Payload*    rest[8]   = {nullptr};
  std::size_t restCount = ring.Drain(rest, 8);
  DALI_TEST_EQUALS(restCount, static_cast<std::size_t>(3), TEST_LOCATION);
  DALI_TEST_EQUALS(rest[0]->value, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(rest[1]->value, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(rest[2]->value, 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingWraparound(void)
{
  tet_infoline("Test many push/drain cycles wrap the internal indices around the capacity mask without loss or reordering");

  TestApplication application;

  constexpr std::size_t                  CAPACITY = 4;
  LocklessPointerRing<Payload, CAPACITY> ring;

  // Run far more cycles than the capacity so the write/read indices wrap the
  // (index & (Capacity-1)) mask many times.
  Payload payload(0);
  for(int cycle = 0; cycle < 100; ++cycle)
  {
    // Fill to capacity.
    for(std::size_t i = 0; i < CAPACITY; ++i)
    {
      payload.value = cycle * 10 + static_cast<int>(i);
      DALI_TEST_CHECK(ring.Push(&payload));
    }
    // Drain fully.
    Payload*    out[CAPACITY] = {nullptr};
    std::size_t count         = ring.Drain(out, CAPACITY);
    DALI_TEST_EQUALS(count, CAPACITY, TEST_LOCATION);
    DALI_TEST_EQUALS(ring.GetApproximateDepth(), static_cast<std::size_t>(0), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingDrainMoreThanAvailable(void)
{
  tet_infoline("Test Drain() with maxOut larger than the pending count returns only what is available");

  TestApplication application;

  LocklessPointerRing<Payload, 8> ring;
  Payload                         a(1), b(2);

  DALI_TEST_CHECK(ring.Push(&a));
  DALI_TEST_CHECK(ring.Push(&b));

  Payload*    out[8] = {nullptr};
  std::size_t count  = ring.Drain(out, 8); // ask for 8, only 2 available
  DALI_TEST_EQUALS(count, static_cast<std::size_t>(2), TEST_LOCATION);
  DALI_TEST_EQUALS(out[0]->value, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(out[1]->value, 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingConcurrentSpsc(void)
{
  tet_infoline("Test concurrent single-producer/single-consumer Push()/Drain() delivers every pointer exactly once, in order, with no loss");

  TestApplication application;

  static constexpr std::size_t           CAPACITY = 16;
  LocklessPointerRing<Payload, CAPACITY> ring;

  constexpr int TARGET_COUNT = 20000;

  // Pre-allocate all payloads so pointers stay valid for the whole run and the
  // producer thread does no heap work in its hot loop.
  std::vector<Payload> payloads;
  payloads.reserve(TARGET_COUNT);
  for(int i = 0; i < TARGET_COUNT; ++i)
  {
    payloads.emplace_back(i);
  }

  std::atomic<bool> stop{false};
  std::atomic<int>  produced{0};
  std::vector<int>  consumedValues;
  consumedValues.reserve(TARGET_COUNT);

  TestWatchdog watchdog(stop, std::chrono::seconds(30));

  std::thread producer([&]()
                       {
    for(int i = 0; i < TARGET_COUNT && !stop; ++i)
    {
      // Retry until there is room - this is the ring-full backpressure path,
      // exercised naturally because CAPACITY (16) is far smaller than
      // TARGET_COUNT.
      while(!ring.Push(&payloads[i]) && !stop)
      {
        std::this_thread::yield();
      }
      ++produced;
    } });

  std::thread consumer([&]()
                       {
    while(static_cast<int>(consumedValues.size()) < TARGET_COUNT && !stop)
    {
      Payload*    out[CAPACITY] = {nullptr};
      std::size_t count         = ring.Drain(out, CAPACITY);
      if(count == 0)
      {
        std::this_thread::yield();
        continue;
      }
      for(std::size_t i = 0; i < count; ++i)
      {
        consumedValues.push_back(out[i]->value);
      }
    } });

  producer.join();
  consumer.join();

  // A watchdog trip means a stall regression - fail clearly rather than on the
  // truncated vector below.
  DALI_TEST_CHECK(!stop.load());

  DALI_TEST_EQUALS(produced.load(), TARGET_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(consumedValues.size()), TARGET_COUNT, TEST_LOCATION);

  // SPSC ordering guarantee: values must arrive strictly in the order produced.
  bool ordered = true;
  for(int i = 0; i < TARGET_COUNT; ++i)
  {
    if(consumedValues[i] != i)
    {
      ordered = false;
      break;
    }
  }
  DALI_TEST_CHECK(ordered);

  END_TEST;
}

int UtcDaliInternalLocklessPointerRingApproximateDepthFromBothSides(void)
{
  tet_infoline("Test GetApproximateDepth() is readable and consistent when the ring is filled and partially drained");

  TestApplication application;

  LocklessPointerRing<Payload, 8> ring;
  std::vector<Payload>            payloads;
  payloads.reserve(5);
  for(int i = 0; i < 5; ++i)
  {
    payloads.emplace_back(i);
  }

  for(int i = 0; i < 5; ++i)
  {
    DALI_TEST_CHECK(ring.Push(&payloads[i]));
    DALI_TEST_EQUALS(ring.GetApproximateDepth(), static_cast<std::size_t>(i + 1), TEST_LOCATION);
  }

  Payload*    out[3] = {nullptr};
  std::size_t count  = ring.Drain(out, 3);
  DALI_TEST_EQUALS(count, static_cast<std::size_t>(3), TEST_LOCATION);
  DALI_TEST_EQUALS(ring.GetApproximateDepth(), static_cast<std::size_t>(2), TEST_LOCATION);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////
// NotificationManager - lockless ring integration
///////////////////////////////////////////////////////////////////////////////

int UtcDaliInternalLocklessRingNotificationMessageRoundTrip(void)
{
  tet_infoline("Test a message queued on the Update side is delivered and Process()'d on the Event side via the notification ring");

  TestApplication application;

  NotificationManager notificationManager;

  // Nothing queued yet.
  DALI_TEST_CHECK(!notificationManager.MessagesToProcess());

  QueueTestMessage* message = new QueueTestMessage(7);
  notificationManager.QueueMessage(message);

  // Queued but not yet handed over - the ring is still empty until UpdateCompleted().
  DALI_TEST_CHECK(!notificationManager.MessagesToProcess());

  notificationManager.UpdateCompleted();
  DALI_TEST_CHECK(notificationManager.MessagesToProcess());

  notificationManager.ProcessMessages();
  DALI_TEST_CHECK(!notificationManager.MessagesToProcess());

  END_TEST;
}

int UtcDaliInternalLocklessRingNotificationInterfaceRoundTrip(void)
{
  tet_infoline("Test a queued complete-notification interface travels through the ring and is drained on the Event side");

  TestApplication application;

  NotificationManager notificationManager;

  // Pass a nullptr interface (guarded by the manager's own `if(iface)` check),
  // matching the established LocklessMessageBuffer UTC pattern. Delivery is
  // asserted via the ring draining to empty, not via a callback.
  NotificationManager::NotificationParameterList params;
  notificationManager.QueueNotification(nullptr, std::move(params));
  notificationManager.UpdateCompleted();

  DALI_TEST_CHECK(notificationManager.MessagesToProcess());
  notificationManager.ProcessMessages();

  DALI_TEST_CHECK(!notificationManager.MessagesToProcess());

  END_TEST;
}

int UtcDaliInternalLocklessRingNotificationEmptyUpdateCompletedIsNoOp(void)
{
  tet_infoline("Test UpdateCompleted() with nothing accumulated produces no batch and leaves the ring empty");

  TestApplication application;

  NotificationManager notificationManager;

  // Call UpdateCompleted() repeatedly with nothing queued: each call must early-out
  // without pushing an (empty) batch onto the ring.
  for(int i = 0; i < 20; ++i)
  {
    notificationManager.UpdateCompleted();
    DALI_TEST_CHECK(!notificationManager.MessagesToProcess());
  }

  // ProcessMessages() on the resulting empty ring must also be inert.
  notificationManager.ProcessMessages();
  DALI_TEST_CHECK(!notificationManager.MessagesToProcess());

  END_TEST;
}

int UtcDaliInternalLocklessRingNotificationMessagesAndInterfacesTogether(void)
{
  tet_infoline("Test a single batch carrying both messages and interfaces delivers all of them in one ProcessMessages()");

  TestApplication application;

  NotificationManager notificationManager;

  std::atomic<int> processedCount{0};

  constexpr int MESSAGE_COUNT   = 6;
  constexpr int INTERFACE_COUNT = 3;

  for(int i = 0; i < MESSAGE_COUNT; ++i)
  {
    auto* message             = new QueueTestMessage(i);
    message->processedCounter = &processedCount;
    notificationManager.QueueMessage(message);
  }
  // nullptr interfaces (null-guarded by the manager), per the LMB UTC pattern.
  for(int i = 0; i < INTERFACE_COUNT; ++i)
  {
    NotificationManager::NotificationParameterList params;
    notificationManager.QueueNotification(nullptr, std::move(params));
  }

  notificationManager.UpdateCompleted();
  DALI_TEST_CHECK(notificationManager.MessagesToProcess());

  notificationManager.ProcessMessages();

  // All messages Process()'d, and the batch (messages + interfaces) fully drained.
  DALI_TEST_EQUALS(processedCount.load(), MESSAGE_COUNT, TEST_LOCATION);
  DALI_TEST_CHECK(!notificationManager.MessagesToProcess());

  END_TEST;
}

int UtcDaliInternalLocklessRingNotificationBatchRecycling(void)
{
  tet_infoline("Test NotificationBatch objects are recycled across many frames rather than accumulating unbounded");

  TestApplication application;

  NotificationManager notificationManager;

  // Run many produce/consume frames. If batch recycling works, the pool
  // stabilises; the observable proxy here is simply that thousands of cycles
  // complete without crashing, leaking (checked by ASAN in CI), or leaving
  // work stuck pending.
  constexpr int FRAMES = 2000;
  for(int frame = 0; frame < FRAMES; ++frame)
  {
    notificationManager.QueueMessage(new QueueTestMessage(frame));
    notificationManager.UpdateCompleted();
    DALI_TEST_CHECK(notificationManager.MessagesToProcess());
    notificationManager.ProcessMessages();
    DALI_TEST_CHECK(!notificationManager.MessagesToProcess());
  }

  END_TEST;
}

int UtcDaliInternalLocklessRingNotificationRingFullRollback(void)
{
  tet_infoline("Test UpdateCompleted() rolls back into the working containers (nothing lost) when the notification ring is full, and later frames drain successfully");

  TestApplication application;

  NotificationManager notificationManager;

  std::atomic<int> processedCount{0};

  // The notification ring capacity is small (a handful of batches). Produce many
  // batches back-to-back WITHOUT draining, so the ring fills and UpdateCompleted()
  // is forced onto its rollback path (Push() returns false, data is moved back into
  // the working containers, batch recycled, retried next frame).
  constexpr int BATCHES = 64; // deliberately far more than the ring capacity

  for(int b = 0; b < BATCHES; ++b)
  {
    auto* message             = new QueueTestMessage(b);
    message->processedCounter = &processedCount;
    notificationManager.QueueMessage(message);
    notificationManager.UpdateCompleted(); // some of these hit the ring-full rollback path
  }

  // Now drain repeatedly. Every message must eventually be processed exactly once:
  // ones that fit the ring directly, plus ones rolled back into the working
  // containers and re-pushed on a later frame, are all still owned by the manager,
  // so nothing is lost.
  //
  // Rolled-back batches are retried on the NEXT UpdateCompleted() (not
  // spontaneously), so we interleave a drain with a re-flush until quiescent. The
  // bound is deliberately generous: under a tiny ring capacity many batches can
  // pile up in the working containers, needing several drain/reflush rounds to
  // fully clear. A large ceiling still guards against a genuine stuck-forever
  // regression without risking a false failure from needing "a few more" rounds.
  constexpr int MAX_DRAIN_ROUNDS = BATCHES * 4;
  int           guard            = 0;
  while(notificationManager.MessagesToProcess() && guard < MAX_DRAIN_ROUNDS)
  {
    notificationManager.ProcessMessages();
    notificationManager.UpdateCompleted(); // gives any rolled-back batch another chance
    ++guard;
  }
  notificationManager.ProcessMessages();

  // Nothing should still be pending, and we must not have spun the guard out (which
  // would indicate work was stuck and never drained).
  DALI_TEST_CHECK(!notificationManager.MessagesToProcess());
  DALI_TEST_CHECK(guard < MAX_DRAIN_ROUNDS);

  // Every queued message must have been processed exactly once - no loss, no
  // double-processing - regardless of how many hit the rollback path.
  DALI_TEST_EQUALS(processedCount.load(), BATCHES, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessRingNotificationTeardownWithUndrainedBatches(void)
{
  tet_infoline("Test NotificationManager destruction with batches still queued in the ring releases them cleanly (no leak, no crash)");

  TestApplication application;

  {
    NotificationManager notificationManager;

    // Queue several frames' worth of work and hand each over, but NEVER call
    // ProcessMessages() - so batches remain sitting in the ring, owned by the
    // manager, at destruction time. The destructor must drain and free them
    // (this is the teardown path the Impl destructor documents).
    for(int b = 0; b < 4; ++b)
    {
      notificationManager.QueueMessage(new QueueTestMessage(b));
      notificationManager.UpdateCompleted();
    }

    DALI_TEST_CHECK(notificationManager.MessagesToProcess());
    // notificationManager goes out of scope here with undrained batches.
  }

  // Reaching here without a crash/leak (leak caught by ASAN in CI) is the pass
  // condition.
  DALI_TEST_CHECK(true);

  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////
// Update::MessageQueue - lockless ring integration
///////////////////////////////////////////////////////////////////////////////

int UtcDaliInternalLocklessRingMessageQueueRoundTrip(void)
{
  tet_infoline("Test MessageQueue reserve/flush/process round trip over the lockless ring fast path");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  std::atomic<int>   processedCount{0};
  constexpr uint32_t MESSAGE_SIZE  = static_cast<uint32_t>(sizeof(QueueTestMessage));
  constexpr int      MESSAGE_COUNT = 10;

  // Not inside Core::ProcessEvents(), so ReserveMessageSlot() should request a
  // process-on-idle at least once.
  for(int i = 0; i < MESSAGE_COUNT; ++i)
  {
    uint32_t* slot = messageQueue.ReserveMessageSlot(MESSAGE_SIZE, /*updateScene*/ false);
    DALI_TEST_CHECK(slot != nullptr);
    auto* message             = new(slot) QueueTestMessage(i);
    message->processedCounter = &processedCount;
  }
  DALI_TEST_CHECK(renderController.processOnIdleRequestCount > 0);

  DALI_TEST_CHECK(messageQueue.FlushQueue());
  bool sceneUpdated = messageQueue.ProcessMessages();
  (void)sceneUpdated;

  DALI_TEST_EQUALS(processedCount.load(), MESSAGE_COUNT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueFlushEmptyReturnsFalse(void)
{
  tet_infoline("Test FlushQueue() returns false when nothing has been reserved");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  // No ReserveMessageSlot() call -> currentMessageBuffer is null -> nothing to flush.
  DALI_TEST_CHECK(!messageQueue.FlushQueue());

  // ProcessMessages() on an empty queue reports no scene update and marks empty.
  DALI_TEST_CHECK(!messageQueue.ProcessMessages());
  DALI_TEST_CHECK(messageQueue.WasEmpty());

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueSceneUpdateFlagFastPath(void)
{
  tet_infoline("Test the scene-update flag is set after a scene-updating flush over the lockless ring and clears once processed");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  std::atomic<int>   processedCount{0};
  constexpr uint32_t MESSAGE_SIZE = static_cast<uint32_t>(sizeof(QueueTestMessage));

  DALI_TEST_CHECK(!messageQueue.IsSceneUpdateRequired());

  {
    uint32_t* slot = messageQueue.ReserveMessageSlot(MESSAGE_SIZE, /*updateScene*/ true);
    DALI_TEST_CHECK(slot != nullptr);
    auto* message             = new(slot) QueueTestMessage(0);
    message->processedCounter = &processedCount;
  }
  DALI_TEST_CHECK(messageQueue.FlushQueue());

  // Visible before ProcessMessages() runs.
  DALI_TEST_CHECK(messageQueue.IsSceneUpdateRequired());

  bool sceneUpdated = messageQueue.ProcessMessages();
  DALI_TEST_CHECK(sceneUpdated);
  DALI_TEST_EQUALS(processedCount.load(), 1, TEST_LOCATION);

  // With nothing further queued, subsequent idle cycles must not keep reporting
  // a pending update.
  for(int i = 0; i < 10; ++i)
  {
    DALI_TEST_CHECK(!messageQueue.IsSceneUpdateRequired());
    DALI_TEST_CHECK(!messageQueue.ProcessMessages());
  }

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueSceneUpdateFlagNotRequested(void)
{
  tet_infoline("Test the scene-update flag stays clear when the flushed message did not request a scene update");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  std::atomic<int>   processedCount{0};
  constexpr uint32_t MESSAGE_SIZE = static_cast<uint32_t>(sizeof(QueueTestMessage));

  {
    uint32_t* slot = messageQueue.ReserveMessageSlot(MESSAGE_SIZE, /*updateScene*/ false);
    DALI_TEST_CHECK(slot != nullptr);
    auto* message             = new(slot) QueueTestMessage(0);
    message->processedCounter = &processedCount;
  }
  DALI_TEST_CHECK(messageQueue.FlushQueue());
  DALI_TEST_CHECK(!messageQueue.IsSceneUpdateRequired());

  bool sceneUpdated = messageQueue.ProcessMessages();
  DALI_TEST_CHECK(!sceneUpdated);
  DALI_TEST_EQUALS(processedCount.load(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueOverflowPath(void)
{
  tet_infoline("Test MessageQueue falls back to the mutex-protected overflow queue when a flush cannot take the lockless ring, and still processes every message");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  std::atomic<int> processedCount{0};

  std::vector<int> processedOrder;
  std::mutex       orderMutex;

  // Force the overflow path deterministically by FILLING the lockless ring: push
  // (capacity + a few) undrained buffers. The first RING_CAPACITY flushes take the
  // fast path (ring); every flush after that finds the ring full and falls back to
  // the mutex-protected overflow (processQueue). No oversized message is used -
  // that would trip MessageBuffer's internal single-slot capacity assert instead
  // of exercising this fallback.
  constexpr int OVERFLOW_SPILL = 4;
  constexpr int TOTAL          = TEST_LOCKLESS_RING_CAPACITY + OVERFLOW_SPILL;

  bool lastFlush = false;
  for(int i = 0; i < TOTAL; ++i)
  {
    // The very last message requests a scene update, so we can assert the flag is
    // set across whichever channel (ring or overflow) it landed in.
    bool updateScene = (i == TOTAL - 1);
    lastFlush        = ReserveAndFlushOne(messageQueue, i, &processedOrder, &orderMutex, updateScene);
    DALI_TEST_CHECK(lastFlush);
  }

  DALI_TEST_CHECK(messageQueue.IsSceneUpdateRequired());

  // A single ProcessMessages() drains the ring first, then the overflow queue -
  // every message must be processed exactly once, in order, regardless of channel.
  bool sceneUpdated = messageQueue.ProcessMessages();
  DALI_TEST_CHECK(sceneUpdated);

  DALI_TEST_EQUALS(static_cast<int>(processedOrder.size()), TOTAL, TEST_LOCATION);
  bool ordered = true;
  for(int i = 0; i < TOTAL; ++i)
  {
    if(processedOrder[i] != i)
    {
      ordered = false;
      break;
    }
  }
  DALI_TEST_CHECK(ordered);

  // Flag clears once processed; capacity query works.
  DALI_TEST_CHECK(!messageQueue.IsSceneUpdateRequired());
  (void)processedCount;

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueuePreservesOrderAcrossChannels(void)
{
  tet_infoline("Test global FIFO order is preserved when the ring fills and later flushes are forced onto the overflow queue");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  std::vector<int> processedOrder;
  std::mutex       orderMutex;

  // Fill the lockless ring exactly to capacity: ids 0 .. RING_CAPACITY-1 take the
  // fast path and sit in the ring, undrained.
  for(int i = 0; i < TEST_LOCKLESS_RING_CAPACITY; ++i)
  {
    DALI_TEST_CHECK(ReserveAndFlushOne(messageQueue, i, &processedOrder, &orderMutex));
  }

  // The next flushes cannot fit the ring (it is full) and are forced onto the
  // mutex-protected overflow queue. Once overflowActive latches, EVERY subsequent
  // flush must also go to overflow - even though draining would free ring space -
  // so that a single ProcessMessages() (which drains ring first, then overflow)
  // still yields strict global FIFO order across BOTH channels.
  constexpr int OVERFLOW_TAIL = 5;
  for(int i = 0; i < OVERFLOW_TAIL; ++i)
  {
    DALI_TEST_CHECK(ReserveAndFlushOne(messageQueue, TEST_LOCKLESS_RING_CAPACITY + i, &processedOrder, &orderMutex));
  }

  const int total = TEST_LOCKLESS_RING_CAPACITY + OVERFLOW_TAIL;

  // One drain: ring (ids 0..capacity-1) then overflow (ids capacity..total-1),
  // which must equal the original enqueue order 0,1,2,...,total-1.
  messageQueue.ProcessMessages();

  DALI_TEST_EQUALS(static_cast<int>(processedOrder.size()), total, TEST_LOCATION);
  std::vector<int> expected;
  for(int i = 0; i < total; ++i)
  {
    expected.push_back(i);
  }
  DALI_TEST_CHECK(processedOrder == expected);

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueBackpressureReleasedByProcess(void)
{
  tet_infoline("Test a concurrent ProcessMessages() releases a FlushQueue() blocked on overflow backpressure, well before the timeout");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  std::atomic<bool> stop{false};
  std::atomic<bool> producerFinished{false};
  TestWatchdog      watchdog(stop, std::chrono::seconds(30));

  std::vector<int> processedOrder;
  std::mutex       orderMutex;

  // To make FlushQueue() actually block on backpressure, the overflow queue
  // (processQueue) must reach MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE. Buffers only
  // land in processQueue once the ring is full, so the producer first fills the
  // ring (RING_CAPACITY flushes), then pushes enough further flushes to grow
  // processQueue past the backpressure threshold. All messages are normal-sized;
  // the ring-full condition (not an oversized message) is what routes them to
  // overflow.
  //
  // Generous headroom above the threshold guarantees at least one flush enters the
  // timed wait even if the consumer drains some in between.
  constexpr int TOTAL_FLUSHES = TEST_LOCKLESS_RING_CAPACITY + TEST_MAX_MESSAGES_ALLOWED_IN_PROCESS_QUEUE + 16;

  std::thread producer([&]()
                       {
    for(int i = 0; i < TOTAL_FLUSHES && !stop; ++i)
    {
      // FlushQueue() may block here (that is the backpressure mechanism under
      // test) - the watchdog guards against it blocking forever.
      ReserveAndFlushOne(messageQueue, i, &processedOrder, &orderMutex);
    }
    producerFinished = true; });

  // Consumer drains periodically, releasing the producer's backpressure wait.
  while(!producerFinished && !stop)
  {
    messageQueue.ProcessMessages();
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }

  producer.join();
  messageQueue.ProcessMessages(); // final drain

  DALI_TEST_CHECK(!stop.load()); // false if the watchdog had to intervene
  DALI_TEST_CHECK(producerFinished.load());

  // Every flushed message must have been processed exactly once across both
  // channels - backpressure must never drop or duplicate work.
  DALI_TEST_EQUALS(static_cast<int>(processedOrder.size()), TOTAL_FLUSHES, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueMoveMessageQueue(void)
{
  tet_infoline("Test MoveMessageQueue() transfers ownership of pending state to the destination and invalidates the source");

  TestApplication        application;
  MyTestRenderController renderController;

  Internal::Update::MessageQueue source(renderController);
  Internal::Update::MessageQueue destination(renderController);

  std::atomic<int>   processedCount{0};
  constexpr uint32_t MESSAGE_SIZE = static_cast<uint32_t>(sizeof(QueueTestMessage));

  // Queue and flush a message into `source`.
  {
    uint32_t* slot = source.ReserveMessageSlot(MESSAGE_SIZE, false);
    DALI_TEST_CHECK(slot != nullptr);
    auto* message             = new(slot) QueueTestMessage(0);
    message->processedCounter = &processedCount;
  }
  DALI_TEST_CHECK(source.FlushQueue());

  // Move source -> destination. After this, destination owns the pending buffer
  // and processing it there must run the queued message.
  Internal::Update::MessageQueue::MoveMessageQueue(destination, source);

  bool sceneUpdated = destination.ProcessMessages();
  (void)sceneUpdated;
  DALI_TEST_EQUALS(processedCount.load(), 1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueConcurrentProducerConsumer(void)
{
  tet_infoline("Test concurrent event-thread flush and update-thread process over the MessageQueue delivers every message exactly once");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  std::atomic<bool> stop{false};
  std::atomic<int>  processedCount{0};
  std::atomic<bool> producerFinished{false};

  TestWatchdog watchdog(stop, std::chrono::seconds(30));

  constexpr uint32_t MESSAGE_SIZE       = static_cast<uint32_t>(sizeof(QueueTestMessage));
  constexpr int      BATCHES            = 400;
  constexpr int      MESSAGES_PER_BATCH = 20;
  constexpr int      EXPECTED_TOTAL     = BATCHES * MESSAGES_PER_BATCH;

  // Event thread: reserve a batch of messages, flush, repeat.
  std::thread producer([&]()
                       {
    for(int b = 0; b < BATCHES && !stop; ++b)
    {
      for(int i = 0; i < MESSAGES_PER_BATCH; ++i)
      {
        uint32_t* slot = messageQueue.ReserveMessageSlot(MESSAGE_SIZE, false);
        if(!slot)
        {
          continue;
        }
        auto* message             = new(slot) QueueTestMessage(b * MESSAGES_PER_BATCH + i);
        message->processedCounter = &processedCount;
      }
      messageQueue.FlushQueue();
    }
    producerFinished = true; });

  // Update thread: drain continuously until the producer is done and the queue
  // is empty.
  while((!producerFinished || !messageQueue.WasEmpty()) && !stop)
  {
    messageQueue.ProcessMessages();
    std::this_thread::yield();
  }
  producer.join();

  // A few final drains to mop up anything flushed just before the producer set
  // its finished flag.
  for(int i = 0; i < 8; ++i)
  {
    messageQueue.ProcessMessages();
  }

  DALI_TEST_CHECK(!stop.load());
  DALI_TEST_EQUALS(processedCount.load(), EXPECTED_TOTAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInternalLocklessRingMessageQueueRecyclesBuffers(void)
{
  tet_infoline("Test MessageQueue recycles buffers across many flush/process cycles rather than growing capacity unbounded");

  TestApplication                application;
  MyTestRenderController         renderController;
  Internal::Update::MessageQueue messageQueue(renderController);

  constexpr uint32_t MESSAGE_SIZE    = static_cast<uint32_t>(sizeof(QueueTestMessage));
  constexpr int      WARMUP_CYCLES   = 5;
  constexpr int      MEASURED_CYCLES = 50;

  auto runCycle = [&](int id)
  {
    uint32_t* slot = messageQueue.ReserveMessageSlot(MESSAGE_SIZE, false);
    DALI_TEST_CHECK(slot != nullptr);
    new(slot) QueueTestMessage(id);
    DALI_TEST_CHECK(messageQueue.FlushQueue());
    messageQueue.ProcessMessages();
  };

  for(int i = 0; i < WARMUP_CYCLES; ++i)
  {
    runCycle(i);
  }
  std::size_t baseline = messageQueue.GetCapacity();

  for(int i = 0; i < MEASURED_CYCLES; ++i)
  {
    runCycle(WARMUP_CYCLES + i);
  }
  std::size_t afterMany = messageQueue.GetCapacity();

  // With recycling working, steady-state capacity must not scale with the number
  // of cycles run - it should stay close to the post-warmup baseline.
  DALI_TEST_CHECK(afterMany <= baseline * 2);

  END_TEST;
}
