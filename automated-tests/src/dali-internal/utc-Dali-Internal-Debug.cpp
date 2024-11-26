/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

// Internal headers are allowed here
#include <dali/integration-api/debug.h>

using namespace Dali;

namespace
{
static Debug::DebugPriority gCapturedLogPriority;
static std::string          gCapturedLog;

void LogCaptureFunction(Debug::DebugPriority priority, std::string& string)
{
  gCapturedLogPriority = priority;
  gCapturedLog         = string;
}

bool CheckCapturedLogContainString(Debug::DebugPriority priority, std::string string)
{
  bool ret = false;
  if(gCapturedLogPriority == priority)
  {
    ret = (gCapturedLog.find(string) != std::string::npos);
  }
  return ret;
}

// Define static dummy filter, for line coverage.
DALI_INIT_TIME_CHECKER_FILTER(gDummyTimeCheckerFilter, SOME_ENVORINMENT);
} // namespace

void utc_dali_internal_debug_startup()
{
  test_return_value = TET_UNDEF;

  gCapturedLog.clear();
}

void utc_dali_internal_debug_cleanup()
{
  gCapturedLog.clear();

  test_return_value = TET_PASS;
}

int UtcDaliDebugLogPrintP(void)
{
  TestApplication application;

  tet_infoline("UtcDaliDebugLogPrintP() Test dali default log macros");

  Debug::InstallLogFunction(LogCaptureFunction);

  std::string expectLogString = "some expect logs";

  DALI_LOG_ERROR("%s", expectLogString.c_str());
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::ERROR, expectLogString), true, TEST_LOCATION);

  DALI_LOG_WARNING("%s", expectLogString.c_str());
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::WARNING, expectLogString), true, TEST_LOCATION);

  DALI_LOG_RELEASE_INFO("%s", expectLogString.c_str());
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, expectLogString), true, TEST_LOCATION);

  DALI_LOG_DEBUG_INFO("%s", expectLogString.c_str());
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::DEBUG, expectLogString), true, TEST_LOCATION);

  Debug::UninstallLogFunction();

  END_TEST;
}

int UtcDaliDebugTimeCheckerP(void)
{
  TestApplication application;

  tet_infoline("UtcDaliDebugTimeCheckerP() Test time checker without environment");

  Debug::InstallLogFunction(LogCaptureFunction);

  auto* enabledFilter  = Dali::Integration::TimeChecker::ThresholdFilter::New(0, "SOME_ENVORINMENT");
  auto* disabledFilter = Dali::Integration::TimeChecker::ThresholdFilter::New(std::numeric_limits<uint32_t>::max(), "SOME_ENVORINMENT");

  DALI_TEST_EQUALS(enabledFilter->IsEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(disabledFilter->IsEnabled(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(gDummyTimeCheckerFilter->IsEnabled(), false, TEST_LOCATION);

  std::string expectLogString = "some expect logs";

  tet_printf("Use function directly.\n");
  tet_printf("Check enabled filter print log\n");
  enabledFilter->BeginTimeCheck();
  enabledFilter->EndTimeCheck(expectLogString.c_str(), "", "", 0);
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, expectLogString), true, TEST_LOCATION);

  tet_printf("Check disabled filter don't print log\n");
  // Clean up captured log first
  gCapturedLog.clear();
  disabledFilter->BeginTimeCheck();
  disabledFilter->EndTimeCheck(expectLogString.c_str(), "", "", 0);
  DALI_TEST_EQUALS(gCapturedLog.empty(), true, TEST_LOCATION);

  tet_printf("Use function by macro.\n");
  tet_printf("Check enabled filter print log\n");
  DALI_TIME_CHECKER_BEGIN(enabledFilter);
  DALI_TIME_CHECKER_BEGIN(enabledFilter);
  DALI_TIME_CHECKER_BEGIN(enabledFilter);

  DALI_TIME_CHECKER_END(enabledFilter);
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, "ms"), true, TEST_LOCATION);

  DALI_TIME_CHECKER_END_WITH_MESSAGE(enabledFilter, "simple message");
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, "simple message"), true, TEST_LOCATION);

  bool messageGeneratorExcuted = false;

  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(enabledFilter, [&](std::ostringstream& oss) {
    messageGeneratorExcuted = true;
    oss << "complex message";
  });
  DALI_TEST_EQUALS(messageGeneratorExcuted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, "complex message"), true, TEST_LOCATION);

  tet_printf("Check disabled filter don't print log\n");
  // Clean up captured log first
  gCapturedLog.clear();
  DALI_TIME_CHECKER_BEGIN(gDummyTimeCheckerFilter);
  DALI_TIME_CHECKER_BEGIN(gDummyTimeCheckerFilter);
  DALI_TIME_CHECKER_BEGIN(gDummyTimeCheckerFilter);

  DALI_TIME_CHECKER_END(gDummyTimeCheckerFilter);
  DALI_TEST_EQUALS(gCapturedLog.empty(), true, TEST_LOCATION);

  DALI_TIME_CHECKER_END_WITH_MESSAGE(gDummyTimeCheckerFilter, "simple message");
  DALI_TEST_EQUALS(gCapturedLog.empty(), true, TEST_LOCATION);

  messageGeneratorExcuted = false;
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gDummyTimeCheckerFilter, [&](std::ostringstream& oss) {
    messageGeneratorExcuted = true;
    oss << "complex message";
  });
  DALI_TEST_EQUALS(messageGeneratorExcuted, false, TEST_LOCATION);
  DALI_TEST_EQUALS(gCapturedLog.empty(), true, TEST_LOCATION);

  Debug::UninstallLogFunction();

  END_TEST;
}

int UtcDaliDebugTimeCheckerScopeTracerP(void)
{
  TestApplication application;

  tet_infoline("UtcDaliDebugTimeCheckerScopeTracerP() Test time checker without environment");

  Debug::InstallLogFunction(LogCaptureFunction);

  auto* enabledFilter = Dali::Integration::TimeChecker::ThresholdFilter::New(0, "SOME_ENVORINMENT");

  DALI_TEST_EQUALS(enabledFilter->IsEnabled(), true, TEST_LOCATION);

  std::string expectLogString = "some expect logs";
  {
    DALI_TIME_CHECKER_SCOPE(enabledFilter, expectLogString.c_str());
    DALI_TEST_EQUALS(gCapturedLog.empty(), true, TEST_LOCATION);
    std::string expectLogString2 = "another expect logs";
    {
      DALI_TIME_CHECKER_SCOPE(enabledFilter, expectLogString2.c_str());
      DALI_TEST_EQUALS(gCapturedLog.empty(), true, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, expectLogString2), true, TEST_LOCATION);

    // Clean up captured log first
    gCapturedLog.clear();
    {
      DALI_TIME_CHECKER_SCOPE(enabledFilter, expectLogString2.c_str());
      DALI_TEST_EQUALS(gCapturedLog.empty(), true, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, expectLogString2), true, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(CheckCapturedLogContainString(Debug::DebugPriority::INFO, expectLogString), true, TEST_LOCATION);

  Debug::UninstallLogFunction();

  END_TEST;
}
