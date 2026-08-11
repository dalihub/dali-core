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

#include <dali/integration-api/queue/queue-benchmark-instrumentation.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace Dali;
using namespace Dali::Internal;

namespace
{
// Helper to check if a file exists
bool FileExists(const char* path)
{
  std::ifstream file(path);
  return file.good();
}

// Helper to read file contents
std::string ReadFileContents(const char* path)
{
  std::ifstream file(path);
  if(!file.is_open())
  {
    return "";
  }
  return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Helper to delete a file
void DeleteFile(const char* path)
{
  std::remove(path);
}

void SetEnvironmentVariable(const char* name, const char* value)
{
#if defined(_WIN32)
  _putenv_s(name, value);
#else
  setenv(name, value, 1);
#endif
}

void UnsetEnvironmentVariable(const char* name)
{
#if defined(_WIN32)
  _putenv_s(name, "");
#else
  unsetenv(name);
#endif
}

} // namespace

void utc_dali_internal_queuebenchmark_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_queuebenchmark_cleanup()
{
  test_return_value = TET_PASS;
}

int UtcDaliInternalQueueBenchmarkDisabledP(void)
{
  tet_infoline("UtcDaliInternalQueueBenchmarkDisabledP - Benchmark disabled");

  // Ensure env var is not set (default state)
  UnsetEnvironmentVariable("DALI_QUEUE_BENCHMARK");

  // Verify IsEnabled returns false
  DALI_TEST_CHECK(QueueBenchmark::IsEnabled() == false);

  // Verify macros are no-ops when disabled
  // These should not crash and should not record anything

  // Test ScopedTimer - should not record timing
  {
    QueueBenchmark::ScopedTimer timer(QueueBenchmark::Channel::MQ_FLUSH_QUEUE);
    // Timer destruction should not record anything when disabled
  }

  // Test IncrementCounter - should not increment
  QueueBenchmark::IncrementCounter(QueueBenchmark::Counter::MQ_BUFFER_FULL_EVENTS, 5);

  // Test RecordValue - should not record
  QueueBenchmark::RecordValue(QueueBenchmark::ValueChannel::MQ_BACKLOG_BYTES, 1024);

  // Test DumpToFile - should not create file when nothing recorded
  const char* testPath = "/tmp/test_queue_benchmark_disabled.csv";
  DeleteFile(testPath); // Clean up first
  QueueBenchmark::DumpToFile(testPath);

  // File should not be created
  DALI_TEST_CHECK(!FileExists(testPath));

  END_TEST;
}

int UtcDaliInternalQueueBenchmarkEnabledP(void)
{
  tet_infoline("UtcDaliInternalQueueBenchmarkEnabledP - Benchmark enabled");

  // Set env var to enable benchmark
  SetEnvironmentVariable("DALI_QUEUE_BENCHMARK", "1");

  // Verify IsEnabled returns true
  DALI_TEST_CHECK(QueueBenchmark::IsEnabled() == true);

  // Test ChannelLog recording
  auto& flushLog     = QueueBenchmark::GetLog(QueueBenchmark::Channel::MQ_FLUSH_QUEUE);
  auto  initialIndex = flushLog.writeIndex.load();

  flushLog.Record(1000); // 1000ns
  flushLog.Record(2000); // 2000ns
  flushLog.Record(3000); // 3000ns

  DALI_TEST_CHECK(flushLog.writeIndex.load() == initialIndex + 3);

  // Test Counter increment
  auto* counters            = QueueBenchmark::GetCounters();
  auto  initialCounterValue = counters[static_cast<std::size_t>(QueueBenchmark::Counter::MQ_BUFFER_FULL_EVENTS)].load();

  QueueBenchmark::IncrementCounter(QueueBenchmark::Counter::MQ_BUFFER_FULL_EVENTS, 5);

  DALI_TEST_CHECK(counters[static_cast<std::size_t>(QueueBenchmark::Counter::MQ_BUFFER_FULL_EVENTS)].load() == initialCounterValue + 5);

  // Test ValueChannel recording
  auto& backlogLog        = QueueBenchmark::GetValueLog(QueueBenchmark::ValueChannel::MQ_BACKLOG_BYTES);
  auto  initialValueIndex = backlogLog.writeIndex.load();

  QueueBenchmark::RecordValue(QueueBenchmark::ValueChannel::MQ_BACKLOG_BYTES, 1024);
  QueueBenchmark::RecordValue(QueueBenchmark::ValueChannel::MQ_BACKLOG_BYTES, 2048);

  DALI_TEST_CHECK(backlogLog.writeIndex.load() == initialValueIndex + 2);

  // Test ScopedTimer - verify timing is recorded
  auto& commitLog          = QueueBenchmark::GetLog(QueueBenchmark::Channel::MQ_COMMIT);
  auto  initialCommitIndex = commitLog.writeIndex.load();

  {
    QueueBenchmark::ScopedTimer timer(QueueBenchmark::Channel::MQ_COMMIT);
    // Do some work (small delay)
    volatile int dummy = 0;
    for(int i = 0; i < 1000; ++i)
    {
      dummy += i;
    }
  }

  DALI_TEST_CHECK(commitLog.writeIndex.load() > initialCommitIndex);

  // Test DumpToFile - verify CSV format
  const char* testPath = "/tmp/test_queue_benchmark_enabled.csv";
  DeleteFile(testPath); // Clean up first

  QueueBenchmark::DumpToFile(testPath);

  DALI_TEST_CHECK(FileExists(testPath));

  std::string contents = ReadFileContents(testPath);

  // Verify CSV has expected headers
  DALI_TEST_CHECK(contents.find("channel,samples,mean_us,p50_us,p95_us,p99_us,p999_us") != std::string::npos);
  DALI_TEST_CHECK(contents.find("counter,total") != std::string::npos);
  DALI_TEST_CHECK(contents.find("value_channel,samples,mean,p50,p95,p99,p999") != std::string::npos);

  // Verify our recorded data is present
  DALI_TEST_CHECK(contents.find("MQ_FlushQueue") != std::string::npos);
  DALI_TEST_CHECK(contents.find("MQ_BufferFullEvents") != std::string::npos);
  DALI_TEST_CHECK(contents.find("MQ_BacklogBytes") != std::string::npos);

  // Clean up
  DeleteFile(testPath);

  END_TEST;
}

/**
 * @brief Test ChannelLog overflow handling
 *
 * Verifies that when the sample buffer is full, additional samples are dropped gracefully.
 */
int UtcDaliInternalQueueBenchmarkOverflowP(void)
{
  tet_infoline("UtcDaliInternalQueueBenchmarkOverflowP - Test overflow handling");

  // Enable benchmark
  SetEnvironmentVariable("DALI_QUEUE_BENCHMARK", "1");

  auto& log = QueueBenchmark::GetLog(QueueBenchmark::Channel::MQ_RESERVE_MESSAGE_SLOT);

  // Record up to MAX_SAMPLES_PER_CHANNEL
  constexpr std::size_t maxSamples = QueueBenchmark::MAX_SAMPLES_PER_CHANNEL;

  for(std::size_t i = 0; i < maxSamples + 100; ++i)
  {
    log.Record(static_cast<std::int64_t>(i * 100));
  }

  // writeIndex should exceed maxSamples, but actual stored samples should be capped
  DALI_TEST_CHECK(log.writeIndex.load() == maxSamples + 100);

  // Verify we can still access the log without crashing
  DALI_TEST_CHECK(log.samplesNs.size() == maxSamples);

  END_TEST;
}

/**
 * @brief Test Percentile and RawPercentile calculations
 */
int UtcDaliInternalQueueBenchmarkPercentileP(void)
{
  tet_infoline("UtcDaliInternalQueueBenchmarkPercentileP - Test percentile calculations");

  // Enable benchmark
  SetEnvironmentVariable("DALI_QUEUE_BENCHMARK", "1");

  // Create sorted test data
  std::vector<std::int64_t> samples = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

  // Test empty vector
  std::vector<std::int64_t> empty;
  DALI_TEST_CHECK(QueueBenchmark::Percentile(empty, 0.50) == 0.0);
  DALI_TEST_CHECK(QueueBenchmark::RawPercentile(empty, 0.50) == 0.0);

  // Test non-empty vector (sorted)
  double p50 = QueueBenchmark::Percentile(samples, 0.50);
  double p95 = QueueBenchmark::Percentile(samples, 0.95);
  double p99 = QueueBenchmark::Percentile(samples, 0.99);
  double p999 = QueueBenchmark::Percentile(samples, 0.999);

  // p50 should be around 500-600ns (0.5-0.6us)
  DALI_TEST_CHECK(p50 > 0.0 && p50 < 1.0); // In microseconds

  // p95, p99 and p999 should be monotonically non-decreasing
  DALI_TEST_CHECK(p95 >= p50);
  DALI_TEST_CHECK(p99 >= p95);
  DALI_TEST_CHECK(p999 >= p99);

  // p999 must never exceed the maximum sample (it is a percentile, not an
  // extrapolation) - guards the new p99.9 dump column against ever reporting
  // a value larger than any observed sample.
  double maxMicros = QueueBenchmark::NanosToMicros(samples.back());
  DALI_TEST_CHECK(p999 <= maxMicros);

  // RawPercentile at 0.999 (value-channel path) must behave the same way.
  double rawP999 = QueueBenchmark::RawPercentile(samples, 0.999);
  DALI_TEST_CHECK(rawP999 <= static_cast<double>(samples.back()));

  // Max should be 1000ns = 1.0us
  double max = QueueBenchmark::NanosToMicros(samples.back());
  DALI_TEST_CHECK(max == 1.0);

  END_TEST;
}

/**
 * @brief Test ChannelName, CounterName, ValueChannelName functions
 */
int UtcDaliInternalQueueBenchmarkNamesP(void)
{
  tet_infoline("UtcDaliInternalQueueBenchmarkNamesP - Test name functions");

  // Enable benchmark
  SetEnvironmentVariable("DALI_QUEUE_BENCHMARK", "1");

  // Test ChannelName
  DALI_TEST_CHECK(std::string(QueueBenchmark::ChannelName(QueueBenchmark::Channel::MQ_FLUSH_QUEUE)) == "MQ_FlushQueue");
  DALI_TEST_CHECK(std::string(QueueBenchmark::ChannelName(QueueBenchmark::Channel::MQ_COMMIT)) == "MQ_Commit");
  DALI_TEST_CHECK(std::string(QueueBenchmark::ChannelName(QueueBenchmark::Channel::MQ_PROCESS_MESSAGES)) == "MQ_ProcessMessages");

  // Test CounterName
  DALI_TEST_CHECK(std::string(QueueBenchmark::CounterName(QueueBenchmark::Counter::MQ_BUFFER_FULL_EVENTS)) == "MQ_BufferFullEvents");
  DALI_TEST_CHECK(std::string(QueueBenchmark::CounterName(QueueBenchmark::Counter::NM_BUFFER_FULL_EVENTS)) == "NM_BufferFullEvents");

  // Test ValueChannelName
  DALI_TEST_CHECK(std::string(QueueBenchmark::ValueChannelName(QueueBenchmark::ValueChannel::MQ_BACKLOG_BYTES)) == "MQ_BacklogBytes");
  DALI_TEST_CHECK(std::string(QueueBenchmark::ValueChannelName(QueueBenchmark::ValueChannel::MQ_PROCESS_QUEUE_DEPTH_ON_FLUSH)) == "MQ_ProcessQueueDepthOnFlush");

  // Test unknown/invalid values return "Unknown"
  DALI_TEST_CHECK(std::string(QueueBenchmark::ChannelName(static_cast<QueueBenchmark::Channel>(999))) == "Unknown");
  DALI_TEST_CHECK(std::string(QueueBenchmark::CounterName(static_cast<QueueBenchmark::Counter>(999))) == "Unknown");
  DALI_TEST_CHECK(std::string(QueueBenchmark::ValueChannelName(static_cast<QueueBenchmark::ValueChannel>(999))) == "Unknown");

  END_TEST;
}
