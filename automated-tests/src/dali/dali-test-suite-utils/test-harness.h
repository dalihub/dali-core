#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <cstdint>

namespace TestHarness
{

enum ExitStatus
{
  EXIT_STATUS_TESTCASE_SUCCEEDED,   // 0
  EXIT_STATUS_TESTCASE_FAILED,      // 1
  EXIT_STATUS_TESTCASE_ABORTED,     // 2
  EXIT_STATUS_FORK_FAILED,          // 3
  EXIT_STATUS_WAITPID_FAILED,       // 4
  EXIT_STATUS_BAD_ARGUMENT,         // 5
  EXIT_STATUS_TESTCASE_NOT_FOUND    // 6
};

const int32_t MAX_NUM_CHILDREN(16);

struct TestCase
{
  int32_t testCase;
  const char* testCaseName;

  TestCase()
  : testCase(0),
    testCaseName(NULL)
  {
  }

  TestCase(int32_t tc, const char* name)
  : testCase(tc),
    testCaseName(name)
  {
  }
  TestCase(const TestCase& rhs)
  : testCase(rhs.testCase),
    testCaseName(rhs.testCaseName)
  {
  }
  TestCase& operator=(const TestCase& rhs)
  {
    testCase = rhs.testCase;
    testCaseName = rhs.testCaseName;
    return *this;

  }
};

/**
 * Run a test case
 * @param[in] testCase The Testkit-lite test case to run
 */
int32_t RunTestCase( struct testcase_s& testCase );

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
int32_t RunAll( const char* processName, testcase tc_array[] );

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

} // namespace TestHarness

#endif
