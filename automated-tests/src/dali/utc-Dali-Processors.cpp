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
 *
 */

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;


class TestProcessor : public Integration::Processor
{
public:
  TestProcessor()
  : processRun(false)
  {
  }

  virtual void Process()
  {
    processRun = true;
  }

  bool processRun;
};


int UtcDaliCoreProcessorP(void)
{
  TestApplication application;

  TestProcessor testProcessor;
  Integration::Core& core = application.GetCore();
  core.RegisterProcessor( testProcessor );

  tet_infoline("Test that the processor has not been executed yet:");
  DALI_TEST_CHECK( testProcessor.processRun == false );

  application.SendNotification();

  tet_infoline("Test that the processor has been executed:");
  DALI_TEST_CHECK( testProcessor.processRun );

  // Clear down for next part of test
  testProcessor.processRun = false;

  core.UnregisterProcessor( testProcessor );
  application.SendNotification();
  tet_infoline("Test that the processor has not been executed again:");
  DALI_TEST_CHECK( testProcessor.processRun == false );

  END_TEST;
}
