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

#include "queue-benchmark-instrumentation.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <cstdio>
#include <string>

namespace Dali::Internal::QueueBenchmark
{
const char* ChannelName(Channel channel)
{
  switch(channel)
  {
    case Channel::MQ_RESERVE_MESSAGE_SLOT:
    {
      return "MQ_ReserveMessageSlot";
    }
    case Channel::MQ_FLUSH_QUEUE:
    {
      return "MQ_FlushQueue";
    }
    case Channel::MQ_COMMIT:
    {
      return "MQ_Commit";
    }
    case Channel::MQ_PROCESS_MESSAGES:
    {
      return "MQ_ProcessMessages";
    }
    case Channel::MQ_MUTEX_OVERFLOW_SECTION:
    {
      return "MQ_MutexOverflowSection";
    }
    case Channel::MQ_EVENT_THREAD_BLOCKED_WAIT:
    {
      return "MQ_EventThreadBlockedWait";
    }
    case Channel::NM_UPDATE_COMPLETED:
    {
      return "NM_UpdateCompleted";
    }
    case Channel::NM_COMMIT:
    {
      return "NM_Commit";
    }
    case Channel::NM_PROCESS_MESSAGES:
    {
      return "NM_ProcessMessages";
    }
    default:
    {
      return "Unknown";
    }
  }
}

const char* CounterName(Counter counter)
{
  switch(counter)
  {
    case Counter::MQ_BUFFER_FULL_EVENTS:
    {
      return "MQ_BufferFullEvents";
    }
    case Counter::NM_BUFFER_FULL_EVENTS:
    {
      return "NM_BufferFullEvents";
    }
    case Counter::MQ_EVENT_THREAD_WAIT_COUNT:
    {
      return "MQ_EventThreadWaitCount";
    }
    case Counter::MQ_EVENT_THREAD_BLOCKED_TOTAL_NS:
    {
      return "MQ_EventThreadBlockedTotalNs";
    }
    default:
      return "Unknown";
  }
}

const char* ValueChannelName(ValueChannel channel)
{
  switch(channel)
  {
    case ValueChannel::MQ_BACKLOG_BYTES:
    {
      return "MQ_BacklogBytes";
    }
    case ValueChannel::MQ_PROCESS_QUEUE_DEPTH_ON_FLUSH:
    {
      return "MQ_ProcessQueueDepthOnFlush";
    }
    case ValueChannel::MQ_PROCESS_QUEUE_DEPTH_ON_DRAIN:
    {
      return "MQ_ProcessQueueDepthOnDrain";
    }
    default:
    {
      return "Unknown";
    }
  }
}

double NanosToMicros(std::int64_t ns)
{
  return static_cast<double>(ns) / 1000.0;
}

double Percentile(std::vector<std::int64_t>& sorted, double p)
{
  if(sorted.empty())
  {
    return 0.0;
  }
  std::size_t index = static_cast<std::size_t>(p * static_cast<double>(sorted.size() - 1));
  return NanosToMicros(sorted[index]);
}

// Same as Percentile() above but returns the raw value with no unit
// conversion, for value channels (e.g. byte counts) rather than durations.
double RawPercentile(std::vector<std::int64_t>& sorted, double p)
{
  if(sorted.empty()) return 0.0;
  std::size_t index = static_cast<std::size_t>(p * static_cast<double>(sorted.size() - 1));
  return static_cast<double>(sorted[index]);
}

void DumpToFile(std::string_view path)
{
  // Double-check enabled before dumping
  if(!IsEnabled())
  {
    return;
  }

#if defined(_MSC_VER)
  FILE* file = nullptr;
  if(fopen_s(&file, std::string(path).c_str(), "w") != 0)
  {
    file = nullptr;
  }
#else
  FILE* file = std::fopen(std::string(path).c_str(), "w");
#endif
  if(!file)
  {
    return;
  }

  std::fprintf(file, "channel,samples,mean_us,p50_us,p95_us,p99_us,p999_us\n");

  ChannelLog* logs = GetLogs();
  for(std::size_t i = 0; i < static_cast<std::size_t>(Channel::COUNT); ++i)
  {
    ChannelLog& log = logs[i];

    // Snapshot only what was actually written; writeIndex may exceed the
    // preallocated capacity if a channel filled up, so clamp it.
    std::size_t count = std::min(log.writeIndex.load(std::memory_order_relaxed), MAX_SAMPLES_PER_CHANNEL);

    std::vector<std::int64_t> samples(log.samplesNs.begin(), log.samplesNs.begin() + static_cast<std::ptrdiff_t>(count));
    if(samples.empty())
    {
      std::fprintf(file, "%s,0,0,0,0,0,0\n", ChannelName(static_cast<Channel>(i)));
      continue;
    }

    double sum = 0.0;
    for(auto ns : samples) sum += NanosToMicros(ns);
    double mean = sum / static_cast<double>(samples.size());

    std::sort(samples.begin(), samples.end());
    double p50  = Percentile(samples, 0.50);
    double p95  = Percentile(samples, 0.95);
    double p99  = Percentile(samples, 0.99);
    double p999 = Percentile(samples, 0.999);

    std::fprintf(file, "%s,%zu,%.3f,%.3f,%.3f,%.3f,%.3f\n", ChannelName(static_cast<Channel>(i)), samples.size(), mean, p50, p95, p99, p999);
  }

  std::fprintf(file, "\ncounter,total\n");
  std::atomic<std::uint64_t>* counters = GetCounters();
  for(std::size_t i = 0; i < static_cast<std::size_t>(Counter::COUNT); ++i)
  {
    std::fprintf(file, "%s,%llu\n", CounterName(static_cast<Counter>(i)), static_cast<unsigned long long>(counters[i].load(std::memory_order_relaxed)));
  }

  // Value channels: raw sampled quantities (e.g. byte counts), not durations.
  // Kept in their own section/table so they're never misread against the
  // "_us" column headers above.
  std::fprintf(file, "\nvalue_channel,samples,mean,p50,p95,p99,p999\n");

  ChannelLog* valueLogs = GetValueLogs();
  for(std::size_t i = 0; i < static_cast<std::size_t>(ValueChannel::COUNT); ++i)
  {
    ChannelLog& log = valueLogs[i];

    std::size_t count = std::min(log.writeIndex.load(std::memory_order_relaxed), MAX_SAMPLES_PER_CHANNEL);

    std::vector<std::int64_t> samples(log.samplesNs.begin(), log.samplesNs.begin() + static_cast<std::ptrdiff_t>(count));
    if(samples.empty())
    {
      std::fprintf(file, "%s,0,0,0,0,0,0\n", ValueChannelName(static_cast<ValueChannel>(i)));
      continue;
    }

    double sum = 0.0;
    for(auto v : samples) sum += static_cast<double>(v);
    double mean = sum / static_cast<double>(samples.size());

    std::sort(samples.begin(), samples.end());
    double p50  = RawPercentile(samples, 0.50);
    double p95  = RawPercentile(samples, 0.95);
    double p99  = RawPercentile(samples, 0.99);
    double p999 = RawPercentile(samples, 0.999);

    std::fprintf(file, "%s,%zu,%.1f,%.1f,%.1f,%.1f,%.1f\n", ValueChannelName(static_cast<ValueChannel>(i)), samples.size(), mean, p50, p95, p99, p999);
  }

  std::fclose(file);
}

} // namespace Dali::Internal::QueueBenchmark
