#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <stdio.h>
#include <testcase.h>

#include <chrono>
#include <cstdint>

namespace TestHarness
{
enum ExitStatus
{
  EXIT_STATUS_TESTCASE_SUCCEEDED, // 0
  EXIT_STATUS_TESTCASE_FAILED,    // 1
  EXIT_STATUS_TESTCASE_ABORTED,   // 2
  EXIT_STATUS_FORK_FAILED,        // 3
  EXIT_STATUS_WAITPID_FAILED,     // 4
  EXIT_STATUS_BAD_ARGUMENT,       // 5
  EXIT_STATUS_TESTCASE_NOT_FOUND  // 6
};

const int32_t MAX_NUM_CHILDREN(16);

struct TestCase
{
  int32_t                               testCase;
  const char*                           name;
  std::chrono::steady_clock::time_point startTime;
  std::chrono::system_clock::time_point startSystemTime;
  int32_t                               result;
  pid_t                                 childPid{0};
  testcase*                             tctPtr;

  TestCase(int32_t index, testcase* testCase)
  : testCase(index),
    name(testCase->name),
    startTime(),
    startSystemTime(),
    result(0),
    childPid(0),
    tctPtr(testCase)
  {
  }
  TestCase()
  : testCase(0),
    name(NULL),
    startTime(),
    startSystemTime(),
    result(0),
    childPid(0),
    tctPtr(nullptr)
  {
  }

  TestCase(int32_t tc, const char* name)
  : testCase(tc),
    name(name),
    startTime(),
    startSystemTime(),
    result(0),
    childPid(0),
    tctPtr(nullptr)
  {
  }
  TestCase(const TestCase& rhs)
  : testCase(rhs.testCase),
    name(rhs.name),
    startTime(rhs.startTime),
    startSystemTime(rhs.startSystemTime),
    result(rhs.result),
    childPid(rhs.childPid),
    tctPtr(rhs.tctPtr)
  {
  }
  TestCase& operator=(const TestCase& rhs)
  {
    testCase        = rhs.testCase;
    name            = rhs.name;
    startTime       = rhs.startTime;
    startSystemTime = rhs.startSystemTime;
    result          = rhs.result;
    childPid        = rhs.childPid;
    tctPtr          = rhs.tctPtr;
    return *this;
  }
};

/**
 * Run a test case
 * @param[in] testCase The Testkit-lite test case to run
 */
int32_t RunTestCase(struct testcase_s& testCase);

/**
 * Run all test cases in parallel
 * @param[in] processName The name of this process
 * @param[in] tc_array The array of auto-generated testkit-lite test cases
 * @param[in] reRunFailed True if failed test cases should be re-run
 * @return 0 on success
 */
int32_t RunAllInParallel(const char* processName, testcase tc_array[], bool reRunFailed);

/**
 * Run all test cases in serial
 * @param[in] processName The name of this process
 * @param[in] tc_array The array of auto-generated testkit-lite test cases
 * @return 0 on success
 */
int32_t RunAll(const char* processName, testcase tc_array[]);

/**
 * Find the named test case in the given array, and run it
 * @param[in] tc_array The array of auto-generated testkit-lite test cases
 * @param[in] testCaseName the name of the test case to run
 * @return 0 on success
 */
int32_t FindAndRunTestCase(::testcase tc_array[], const char* testCaseName);

/**
 * Display usage instructions for this program
 * @param[in] program The name of this program
 */
void Usage(const char* program);

/**
 * Main function.
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 * @param[in] tc_array Array of test cases
 */
int RunTests(int argc, char* const argv[], ::testcase tc_array[]);

} // namespace TestHarness

#endif
