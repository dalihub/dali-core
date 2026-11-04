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

// Trace::Filter and the DALI_TRACE_* macros are compiled only when the library
// is built with TRACE_ENABLED, so only the always-available MarkContext API is
// tested here.
#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <string>
#include <vector>

// Internal headers are allowed here
#include <dali/integration-api/trace.h>

using namespace Dali;

namespace
{
struct LogContextCall
{
  bool        start;
  std::string tag;
  bool        hasMessage;
  std::string message;
};

std::vector<LogContextCall> gLogContextCalls;

void LogContextCapture(bool start, const char* tag, const char* message)
{
  gLogContextCalls.push_back({start, tag ? tag : "", message != nullptr, message ? message : ""});
}

struct MarkContextCall
{
  std::string tag;
  bool        hasMessage;
  std::string message;
};

std::vector<MarkContextCall> gMarkContextCalls;

void MarkContextCapture(const char* tag, const char* message)
{
  gMarkContextCalls.push_back({tag ? tag : "", message != nullptr, message ? message : ""});
}

void InstallCaptureFunctions()
{
  gLogContextCalls.clear();
  gMarkContextCalls.clear();
  Dali::Integration::Trace::InstallLogContextFunction(LogContextCapture);
  Dali::Integration::Trace::InstallMarkContextFunction(MarkContextCapture);
}

void UninstallCaptureFunctions()
{
  Dali::Integration::Trace::InstallLogContextFunction(nullptr);
  Dali::Integration::Trace::InstallMarkContextFunction(nullptr);
}
} // namespace

int UtcDaliTraceMarkContextWithMarkFunctionP(void)
{
  tet_infoline("Testing MarkContext routes to the installed MarkContextFunction");

  InstallCaptureFunctions();

  Dali::Integration::Trace::MarkContext("MARK_TAG", "mark message");
  Dali::Integration::Trace::MarkContext("MARK_TAG_NO_MESSAGE");

  DALI_TEST_CHECK(gMarkContextCalls.size() == 2u);
  DALI_TEST_CHECK(gMarkContextCalls[0].tag == "MARK_TAG");
  DALI_TEST_CHECK(gMarkContextCalls[0].hasMessage);
  DALI_TEST_CHECK(gMarkContextCalls[0].message == "mark message");
  DALI_TEST_CHECK(gMarkContextCalls[1].tag == "MARK_TAG_NO_MESSAGE");
  DALI_TEST_CHECK(!gMarkContextCalls[1].hasMessage);

  // The installed mark function consumes the marker : no begin/end fallback.
  DALI_TEST_CHECK(gLogContextCalls.empty());

  UninstallCaptureFunctions();
  END_TEST;
}

int UtcDaliTraceMarkContextFallbackP(void)
{
  tet_infoline("Testing MarkContext falls back to an adjacent begin/end pair without a MarkContextFunction");

  InstallCaptureFunctions();
  Dali::Integration::Trace::InstallMarkContextFunction(nullptr);

  Dali::Integration::Trace::MarkContext("FALLBACK_TAG", "fallback message");

  DALI_TEST_CHECK(gMarkContextCalls.empty());
  DALI_TEST_CHECK(gLogContextCalls.size() == 2u);
  DALI_TEST_CHECK(gLogContextCalls[0].start);
  DALI_TEST_CHECK(gLogContextCalls[0].tag == "FALLBACK_TAG");
  DALI_TEST_CHECK(gLogContextCalls[0].hasMessage);
  DALI_TEST_CHECK(gLogContextCalls[0].message == "fallback message");
  DALI_TEST_CHECK(!gLogContextCalls[1].start);
  DALI_TEST_CHECK(gLogContextCalls[1].tag == "FALLBACK_TAG");
  DALI_TEST_CHECK(!gLogContextCalls[1].hasMessage);

  UninstallCaptureFunctions();
  END_TEST;
}

int UtcDaliTraceMarkContextNoFunctionP(void)
{
  tet_infoline("Testing MarkContext is safely ignored without any installed function");

  UninstallCaptureFunctions();

  Dali::Integration::Trace::MarkContext("IGNORED_TAG", "ignored message");
  Dali::Integration::Trace::MarkContext("IGNORED_TAG");

  DALI_TEST_CHECK(true); // Should not crash
  END_TEST;
}

int UtcDaliTraceMarkContextReinstallP(void)
{
  tet_infoline("Testing MarkContext routing follows the lastly installed functions");

  InstallCaptureFunctions();

  Dali::Integration::Trace::MarkContext("FIRST_TAG");
  DALI_TEST_CHECK(gMarkContextCalls.size() == 1u);

  // Removing the mark function switches new markers to the fallback pair.
  Dali::Integration::Trace::InstallMarkContextFunction(nullptr);
  Dali::Integration::Trace::MarkContext("SECOND_TAG");
  DALI_TEST_CHECK(gMarkContextCalls.size() == 1u);
  DALI_TEST_CHECK(gLogContextCalls.size() == 2u);

  // Reinstalling the mark function restores direct routing.
  Dali::Integration::Trace::InstallMarkContextFunction(MarkContextCapture);
  Dali::Integration::Trace::MarkContext("THIRD_TAG");
  DALI_TEST_CHECK(gMarkContextCalls.size() == 2u);
  DALI_TEST_CHECK(gMarkContextCalls[1].tag == "THIRD_TAG");
  DALI_TEST_CHECK(gLogContextCalls.size() == 2u);

  UninstallCaptureFunctions();
  END_TEST;
}
