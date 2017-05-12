/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 *
 */

#include <dali-test-suite-utils.h>
#include <dali/graphics/utility/utility-queue.h>

using namespace Dali::Graphics::Utility;

void utc_dali_graphics_utility_queue_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_graphics_utility_queue_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
struct Tracker
{
  Tracker() = delete;

  Tracker(size_t& ctrCount, size_t& dtrCount)
  : mCtrCount(ctrCount), mDtrCount(dtrCount)
  {
    ++mCtrCount;
  }

  Tracker(const Tracker&) = delete;
  Tracker& operator=(const Tracker&) = delete;

  Tracker(Tracker&& t) : Tracker(t.mCtrCount, t.mDtrCount)
  {
  }
  Tracker& operator=(Tracker&&) = delete;

  ~Tracker()
  {
    ++mDtrCount;
  }
  size_t& mCtrCount;
  size_t& mDtrCount;

};

} // namespace

int UtcDaliGraphicsUtilityQueueSimple(void)
{
  auto queue = Queue<int>{};

  queue.Enqueue(42);
  auto value = queue.Dequeue();

  DALI_TEST_EQUALS(value, 42, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGraphicsUtilityQueueMultiple(void)
{
  auto queue = Queue<int>{};

  queue.Enqueue(42);
  for(int i=0; i<42; ++i)
  {
    queue.Enqueue(i);
  }

  auto value = queue.Dequeue();

  DALI_TEST_EQUALS(value, 42, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGraphicsUtilityQueueLongRun(void)
{
  auto queue = Queue<int>{};

  queue.Enqueue(0);

  for(int i=0; i<1000; ++i)
  {
    queue.Enqueue(i+1);
    auto value = queue.Dequeue();
    DALI_TEST_EQUALS(value, i, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(queue.Count(), 1, TEST_LOCATION);
  queue.Dequeue();

  END_TEST;
}

int UtcDaliGraphicsUtilityQueueLongRunTracker(void)
{
  auto ctrCounter = size_t{};
  auto dtrCounter = size_t{};

  auto queue = Queue< Tracker >{};

  queue.EnqueueEmplace(ctrCounter, dtrCounter);

  for(int i = 0; i < 1000; ++i)
  {
    {
      queue.EnqueueEmplace(ctrCounter, dtrCounter);
      queue.Dequeue();
    }
    DALI_TEST_EQUALS(ctrCounter, dtrCounter + 1, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(queue.Count(), 1, TEST_LOCATION);
  queue.Dequeue();

  DALI_TEST_EQUALS(ctrCounter, dtrCounter, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGraphicsUtilityQueueCapacityCheck(void)
{
  auto queue = Queue< int >{};

  DALI_TEST_EQUALS(queue.GetCapacity(), 0, TEST_LOCATION);

  queue.EnqueueEmplace(1);
  DALI_TEST_EQUALS(queue.GetCapacity(), 255, TEST_LOCATION);

  queue.Dequeue();
  DALI_TEST_EQUALS(queue.GetCapacity(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGraphicsUtilityQueueTrackerDeletion(void)
{
  auto ctrCounter = size_t{};
  auto dtrCounter = size_t{};

  {
    auto queue = Queue< Tracker >{};

    DALI_TEST_EQUALS(queue.GetCapacity(), 0, TEST_LOCATION);
    queue.EnqueueEmplace(ctrCounter, dtrCounter);
    for(int i = 0; i < 10; ++i)
    {
      queue.EnqueueEmplace(ctrCounter, dtrCounter);
    }
    DALI_TEST_EQUALS(queue.GetCapacity(), 63, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(ctrCounter, dtrCounter, TEST_LOCATION);

  END_TEST;
}


int UtcDaliGraphicsUtilityQueueReduceCapacity(void)
{
  auto ctrCounter = size_t{};
  auto dtrCounter = size_t{};

  {
    auto queue = Queue< Tracker >{};

    DALI_TEST_EQUALS(queue.GetCapacity(), 0, TEST_LOCATION);
    queue.EnqueueEmplace(ctrCounter, dtrCounter);
    for(int i = 0; i < 1000; ++i)
    {
      queue.EnqueueEmplace(ctrCounter, dtrCounter);
    }
    DALI_TEST_EQUALS(queue.GetCapacity(), 1023, TEST_LOCATION);
    DALI_TEST_EQUALS(queue.Count(), 1001, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(ctrCounter, dtrCounter, TEST_LOCATION);

  END_TEST;
}
