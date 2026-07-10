#ifndef DALI_INTERNAL_QUEUE_BENCHMARK_INSTRUMENTATION_H
#define DALI_INTERNAL_QUEUE_BENCHMARK_INSTRUMENTATION_H

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
 *
 */

// Lightweight latency/counter instrumentation for the Update/Event
// message-queue call sites. Timings recorded here happen on the actual
// Update and Event threads, with real message traffic, real thread
// priorities, and real OS scheduling decisions.
//
// Enable with environment variable: DALI_QUEUE_BENCHMARK=1
// By default this is disabled.
//
// Usage note:
//
// This benchmark instrumentation is for both the mutex-only and the
// lockless-ring implementations of MessageQueue/NotificationManager,
// using the exact same channel names in both, so the resulting numbers
// are directly comparable.
//
// MQ_FLUSH_QUEUE and MQ_PROCESS_MESSAGES (and their NM_* counterparts,
// NM_UPDATE_COMPLETED / NM_PROCESS_MESSAGES) bracket the entire hand-off
// end to end. MQ_RESERVE_MESSAGE_SLOT, MQ_COMMIT, and NM_COMMIT are
// finer-grained sub-measurements, useful for understanding where time
// goes.
//
// MQ_MUTEX_OVERFLOW_SECTION and MQ_EVENT_THREAD_BLOCKED_WAIT (plus the
// MQ_EVENT_THREAD_WAIT_COUNT / MQ_EVENT_THREAD_BLOCKED_TOTAL_NS counters)
// are the headline "how much did contention actually cost the Event
// thread" numbers - a different question from the end-to-end channels
// above (responsiveness under load, not per-call baseline cost).

// EXTERNAL INCLUDES
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <string_view>
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#define DALI_QB_CONCAT_INNER(a, b) a##b
#define DALI_QB_CONCAT(a, b) DALI_QB_CONCAT_INNER(a, b)

/**
 * @brief Times the enclosing scope and records the elapsed duration into a timing channel.
 *
 * Declare at the top of the scope to be measured; the recording happens
 * automatically when the scope ends (normal return, early return, or an
 * exception unwinding through it), via the destructor of the underlying
 * ScopedTimer. A no-op (branch not taken) unless DALI_QUEUE_BENCHMARK=1.
 *
 * @param[in] channel An unqualified Channel enumerator, e.g. MQ_FLUSH_QUEUE.
 *
 * @code
 * bool MessageQueue::FlushQueue()
 * {
 *   DALI_QB_SCOPE_TIMER(MQ_FLUSH_QUEUE); // times the whole function body
 *   ...
 * }
 * @endcode
 */
#define DALI_QB_SCOPE_TIMER(channel)                            \
  if(::Dali::Internal::QueueBenchmark::IsEnabled())             \
  ::Dali::Internal::QueueBenchmark::ScopedTimer DALI_QB_CONCAT( \
    qbTimer_, __LINE__)(::Dali::Internal::QueueBenchmark::Channel::channel)

/**
 * @brief Increments a discrete-event counter.
 *
 * For counting how many times something happened, as opposed to timing how
 * long something took. A no-op (branch not taken) unless
 * DALI_QUEUE_BENCHMARK=1.
 *
 * @param[in] counter An unqualified Counter enumerator, e.g. MQ_BUFFER_FULL_EVENTS.
 * @param[in] ... Optional amount to increment by (defaults to 1 if omitted).
 *
 * @code
 * DALI_QB_COUNT(MQ_BUFFER_FULL_EVENTS);                          // increment by 1
 * DALI_QB_COUNT(MQ_EVENT_THREAD_BLOCKED_TOTAL_NS, blockedNs);    // increment by an explicit amount
 * @endcode
 */
#define DALI_QB_COUNT(counter, ...)                 \
  if(::Dali::Internal::QueueBenchmark::IsEnabled()) \
  ::Dali::Internal::QueueBenchmark::IncrementCounter(::Dali::Internal::QueueBenchmark::Counter::counter, ##__VA_ARGS__)

/**
 * @brief Records one non-timing sample (e.g. a queue depth) into a value channel.
 *
 * Uses the same distribution machinery (mean/p50/p95/p99/p99.9) as a timing
 * channel, but for a quantity that isn't a duration. A no-op (branch not
 * taken) unless DALI_QUEUE_BENCHMARK=1.
 *
 * @param[in] channel An unqualified ValueChannel enumerator, e.g. MQ_BACKLOG_BYTES.
 * @param[in] value The value to record; cast to std::int64_t.
 *
 * @code
 * DALI_QB_RECORD_VALUE(MQ_BACKLOG_BYTES, mImpl->bufferRing.GetApproximateDepth());
 * @endcode
 */
#define DALI_QB_RECORD_VALUE(channel, value)        \
  if(::Dali::Internal::QueueBenchmark::IsEnabled()) \
  ::Dali::Internal::QueueBenchmark::RecordValue(::Dali::Internal::QueueBenchmark::ValueChannel::channel, static_cast<std::int64_t>(value))

/**
 * @brief Writes all recorded channels, counters, and value channels to a CSV file.
 *
 * Call once per run - e.g. on application quit, or from a debug key combo -
 * never from inside a hot path itself. A no-op (branch not taken) unless
 * DALI_QUEUE_BENCHMARK=1. See DumpToFile() for the exact file format.
 *
 * @param[in] path Filesystem path to write the CSV to.
 */
#define DALI_QB_DUMP(path)                          \
  if(::Dali::Internal::QueueBenchmark::IsEnabled()) \
  ::Dali::Internal::QueueBenchmark::DumpToFile(path)

namespace Dali::Internal::QueueBenchmark
{
/**
 * @brief Check if queue benchmark is enabled via environment variable.
 *
 * Reads the environment variable on each call. This allows test cases to
 * change the environment variable between tests to verify both enabled
 * and disabled behavior.
 *
 * @return true if DALI_QUEUE_BENCHMARK environment variable is set to non-zero.
 */
inline bool IsEnabled()
{
  const char* env = std::getenv("DALI_QUEUE_BENCHMARK");
  return env && (std::atoi(env) != 0);
}

/**
 * @brief Identifies a timing channel: a named call site whose duration is recorded via DALI_QB_SCOPE_TIMER.
 *
 * Each enumerator maps to one ChannelLog (see GetLog()), holding up to
 * MAX_SAMPLES_PER_CHANNEL nanosecond-resolution samples for that call site
 * across the run.
 */
enum class Channel : std::size_t
{
  MQ_RESERVE_MESSAGE_SLOT,      ///< MessageQueue::ReserveMessageSlot() — Event thread, lock-free triple buffer (baseline)
  MQ_FLUSH_QUEUE,               ///< MessageQueue::FlushQueue() end-to-end — Event thread hand-off; fair comparison point vs old mutex swap
  MQ_COMMIT,                    ///< Just the processBuffer.Commit() call inside FlushQueue
  MQ_PROCESS_MESSAGES,          ///< MessageQueue::ProcessMessages() end-to-end — Update thread consume; fair comparison point vs old mutex swap
  MQ_MUTEX_OVERFLOW_SECTION,    ///< Time spent inside the queueMutex-guarded overflow fallback specifically
  MQ_EVENT_THREAD_BLOCKED_WAIT, ///< Time spent inside localFuture.wait_for() specifically — only recorded for calls that actually entered the wait (not every overflow hits this). Direct evidence of Event-thread stalls, rather than inferring them from MQ_MUTEX_OVERFLOW_SECTION duration alone.
  NM_UPDATE_COMPLETED,          ///< NotificationManager::UpdateCompleted() end-to-end — Update thread producer
  NM_COMMIT,                    ///< Just the notificationBuffer.Commit() call
  NM_PROCESS_MESSAGES,          ///< NotificationManager::ProcessMessages() end-to-end — Event thread consumer
  COUNT
};

/**
 * @brief Identifies a discrete-event counter incremented via DALI_QB_COUNT.
 *
 * Each enumerator maps to one running total (see GetCounters()), for
 * counting how often something happened rather than how long it took.
 */
enum class Counter : std::size_t
{
  MQ_BUFFER_FULL_EVENTS,            ///< Times the lockless processBuffer was full and the overflow path was taken
  NM_BUFFER_FULL_EVENTS,            ///< Times the lockless notificationBuffer was full and a notification was rolled back
  MQ_EVENT_THREAD_WAIT_COUNT,       ///< Times the Event thread actually entered localFuture.wait_for() (a subset of MQ_BUFFER_FULL_EVENTS — only when the overflow queue was ALSO deep enough to trigger waitForConsumer)
  MQ_EVENT_THREAD_BLOCKED_TOTAL_NS, ///< Cumulative nanoseconds the Event thread spent blocked in wait_for() across the whole run — a single number for "how much total responsiveness did contention cost", complementing the MQ_EVENT_THREAD_BLOCKED_WAIT distribution
  COUNT
};

/**
 * @brief Identifies a value channel: a non-timing quantity recorded via DALI_QB_RECORD_VALUE.
 *
 * A raw quantity sampled at a call site, such as a queue depth, rather than
 * a duration. Uses the same distribution machinery (mean/p50/p95/p99/p99.9)
 * as a timing Channel, but is dumped as its own CSV section, in its own
 * units, so it is never misread as microseconds.
 */
enum class ValueChannel : std::size_t
{
  MQ_BACKLOG_BYTES,                ///< Depth of the lockless ring (in whole buffers/batches, NOT bytes - see LocklessPointerRing), sampled at the TOP of each FlushQueue() call. Renamed in spirit but not in symbol to avoid unnecessary churn: after the pointer-handoff optimization, "backlog" is measured in pending buffer count rather than serialized bytes, since messages are no longer serialized into the ring at all.
  MQ_PROCESS_QUEUE_DEPTH_ON_FLUSH, ///< Number of buffers sitting in the mutex-guarded overflow processQueue, sampled right after FlushQueue() pushes onto it (producer's view of backlog). Under sustained overload the lockless ring can read empty (nothing is being written to it once overflowPending latches) while all the real backlog piles up here instead — this channel is what makes that backlog visible.
  MQ_PROCESS_QUEUE_DEPTH_ON_DRAIN, ///< Number of buffers sitting in processQueue, sampled at the start of ProcessMessages()'s overflow section, right before it is swapped out and drained (consumer's view of how deep the backlog had grown by the time it got processed).
  COUNT
};

/**
 * @brief Maximum number of samples retained per channel.
 *
 * Preallocated up front so the recorder itself never allocates on the hot
 * path - an allocation there would pollute the very timings it's trying to
 * measure. ~16MB per channel. Samples recorded past this count are dropped
 * (writeIndex keeps counting, but ChannelLog::Record() stops writing into
 * samplesNs once idx reaches this bound).
 */
constexpr std::size_t MAX_SAMPLES_PER_CHANNEL = 2'000'000;

/**
 * @brief Fixed-capacity, lock-free sample recorder for one channel.
 *
 * Backs both Channel (timing) and ValueChannel (non-timing) entries - the
 * two enums share this exact storage/distribution logic and are only kept
 * in separate arrays (see GetLogs()/GetValueLogs()) so a timing sample and
 * a value sample can never be confused with each other in the dump output.
 *
 * Not thread-safe against multiple concurrent WRITERS - see the note on
 * Record() - but safe to read from a different thread than the one writing
 * to it (as DumpToFile() does), since writeIndex is atomic and stale reads
 * only ever see a slightly-behind sample count, never a torn one.
 */
struct ChannelLog
{
  std::vector<std::int64_t> samplesNs;
  std::atomic<std::size_t>  writeIndex{0};

  /**
   * @brief Allocates the sample buffer up front.
   *
   * Called once per channel via GetLogs()/GetValueLogs()'s std::call_once,
   * before any Record() call can race with it.
   */
  void Init()
  {
    samplesNs.assign(MAX_SAMPLES_PER_CHANNEL, 0);
  }

  /**
   * @brief Records one sample.
   *
   * @param[in] ns The sample value, in nanoseconds for a timing Channel or
   * in the channel's own unit for a ValueChannel (still stored as
   * std::int64_t either way).
   *
   * @note Each channel is only ever written by one real thread (Update xor
   * Event exclusively, never both), so a plain relaxed fetch_add is safe
   * here even without a mutex - there is no concurrent writer to race with.
   * Samples past MAX_SAMPLES_PER_CHANNEL are silently dropped rather than
   * growing the buffer.
   */
  void Record(std::int64_t ns)
  {
    std::size_t idx = writeIndex.fetch_add(1, std::memory_order_relaxed);
    if(idx < MAX_SAMPLES_PER_CHANNEL)
    {
      samplesNs[idx] = ns;
    }
  }
};

/**
 * @brief Returns the array of timing ChannelLogs, one per Channel enumerator.
 *
 * Lazily allocates each log's sample buffer on first call (thread-safe via
 * std::call_once), so the cost of that allocation is paid once, off the hot
 * path, rather than on the first Record().
 *
 * @return Pointer to the first element of a Channel::COUNT-sized array.
 */
inline ChannelLog* GetLogs()
{
  static ChannelLog     logs[static_cast<std::size_t>(Channel::COUNT)];
  static std::once_flag initFlag;
  std::call_once(initFlag, []
                 {
    for(auto& log : logs)
    {
      log.Init();
    } });
  return logs;
}

/**
 * @brief Returns the ChannelLog for one timing channel.
 *
 * @param[in] channel Which channel's log to return.
 * @return Reference to that channel's ChannelLog.
 */
inline ChannelLog& GetLog(Channel channel)
{
  return GetLogs()[static_cast<std::size_t>(channel)];
}

// Value channels reuse the identical ChannelLog storage/distribution logic
// as timing channels, just addressed by a different enum and kept in a
// separate array so the two are never confused in the dump output.

/**
 * @brief Returns the array of value-channel ChannelLogs, one per ValueChannel enumerator.
 *
 * See GetLogs() - identical lazy-init behaviour, just for ValueChannel
 * instead of Channel.
 *
 * @return Pointer to the first element of a ValueChannel::COUNT-sized array.
 */
inline ChannelLog* GetValueLogs()
{
  static ChannelLog     logs[static_cast<std::size_t>(ValueChannel::COUNT)];
  static std::once_flag initFlag;
  std::call_once(initFlag, []
                 {
    for(auto& log : logs)
    {
      log.Init();
    } });
  return logs;
}

/**
 * @brief Returns the ChannelLog for one value channel.
 *
 * @param[in] channel Which value channel's log to return.
 * @return Reference to that value channel's ChannelLog.
 */
inline ChannelLog& GetValueLog(ValueChannel channel)
{
  return GetValueLogs()[static_cast<std::size_t>(channel)];
}

/**
 * @brief Records one sample into a value channel. Prefer DALI_QB_RECORD_VALUE over calling this directly.
 *
 * Re-checks IsEnabled() itself so it is also safe to call directly (not just
 * via the macro) without paying for a Record() call when disabled.
 *
 * @param[in] channel Which value channel to record into.
 * @param[in] value The value to record.
 */
inline void RecordValue(ValueChannel channel, std::int64_t value)
{
  if(IsEnabled())
  {
    GetValueLog(channel).Record(value);
  }
}

/**
 * @brief Returns the array of counter totals, one atomic per Counter enumerator.
 *
 * @return Pointer to the first element of a Counter::COUNT-sized array.
 */
inline std::atomic<std::uint64_t>* GetCounters()
{
  static std::atomic<std::uint64_t> counters[static_cast<std::size_t>(Counter::COUNT)]{};
  return counters;
}

/**
 * @brief Increments a counter. Prefer DALI_QB_COUNT over calling this directly.
 *
 * Re-checks IsEnabled() itself so it is also safe to call directly (not just
 * via the macro) without paying for the atomic increment when disabled.
 *
 * @param[in] counter Which counter to increment.
 * @param[in] by Amount to increment by. Defaults to 1.
 */
inline void IncrementCounter(Counter counter, std::uint64_t by = 1)
{
  if(IsEnabled())
  {
    GetCounters()[static_cast<std::size_t>(counter)].fetch_add(by, std::memory_order_relaxed);
  }
}

/**
 * @brief RAII timer that records its own lifetime into a timing channel. Prefer DALI_QB_SCOPE_TIMER over constructing this directly.
 *
 * Starts the clock on construction; on destruction (normal scope exit, early
 * return, or exception unwinding through it), records the elapsed duration
 * into the given channel's ChannelLog.
 */
class ScopedTimer
{
public:
  /**
   * @brief Constructor. Starts the clock immediately.
   *
   * @param[in] channel Which timing channel this instance's duration will be recorded into.
   */
  explicit ScopedTimer(Channel channel)
  : mChannel(channel),
    mStart(std::chrono::steady_clock::now())
  {
  }

  /**
   * @brief Destructor. Records the elapsed time since construction, if benchmarking is enabled.
   */
  ~ScopedTimer()
  {
    if(IsEnabled())
    {
      auto elapsed = std::chrono::steady_clock::now() - mStart;
      GetLog(mChannel).Record(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());
    }
  }

  ScopedTimer(const ScopedTimer&)            = delete;
  ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
  Channel                               mChannel;
  std::chrono::steady_clock::time_point mStart;
};

/**
 * @brief Returns the display name for a timing channel, e.g. "MQ_FlushQueue".
 *
 * @param[in] channel The channel to name. An out-of-range value returns "Unknown".
 * @return Pointer to a static, null-terminated string. Never null.
 */
DALI_CORE_API const char* ChannelName(Channel channel);

/**
 * @brief Returns the display name for a counter, e.g. "MQ_BufferFullEvents".
 *
 * @param[in] counter The counter to name. An out-of-range value returns "Unknown".
 * @return Pointer to a static, null-terminated string. Never null.
 */
DALI_CORE_API const char* CounterName(Counter counter);

/**
 * @brief Returns the display name for a value channel, e.g. "MQ_BacklogBytes".
 *
 * @param[in] channel The value channel to name. An out-of-range value returns "Unknown".
 * @return Pointer to a static, null-terminated string. Never null.
 */
DALI_CORE_API const char* ValueChannelName(ValueChannel channel);

/**
 * @brief Writes all recorded samples and counters to a CSV file: one row per
 * channel with count/mean/p50/p95/p99/p99.9 in microseconds, plus counter
 * totals.
 *
 * Call this ONCE - e.g. wired to a debug key combo, or from
 * Adaptor/Application shutdown - never from inside the hot path itself.
 *
 * @param[in] path Filesystem path to write the CSV to.
 */
DALI_CORE_API void DumpToFile(std::string_view path);

// Helper functions for testing (exposed for unit tests)

/**
 * @brief Converts a nanosecond duration to microseconds.
 *
 * @param[in] ns Duration in nanoseconds.
 * @return The same duration in microseconds.
 */
DALI_CORE_API double NanosToMicros(std::int64_t ns);

/**
 * @brief Computes a percentile of a timing channel's samples, in microseconds.
 *
 * @param[in,out] sorted Samples in nanoseconds; sorted in place by this call if not already sorted.
 * @param[in] p Percentile to compute, in the range [0, 1] (e.g. 0.999 for p99.9).
 * @return The requested percentile, converted to microseconds. 0.0 if sorted is empty.
 */
DALI_CORE_API double Percentile(std::vector<std::int64_t>& sorted, double p);

/**
 * @brief Computes a percentile of a value channel's samples, in the channel's own (non-time) unit.
 *
 * Identical to Percentile() except the result is NOT converted to
 * microseconds, since a value channel's samples aren't a duration.
 *
 * @param[in,out] sorted Samples; sorted in place by this call if not already sorted.
 * @param[in] p Percentile to compute, in the range [0, 1] (e.g. 0.999 for p99.9).
 * @return The requested percentile, in the channel's own unit. 0.0 if sorted is empty.
 */
DALI_CORE_API double RawPercentile(std::vector<std::int64_t>& sorted, double p);

} // namespace Dali::Internal::QueueBenchmark

#endif // DALI_INTERNAL_QUEUE_BENCHMARK_INSTRUMENTATION_H
