/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/core.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

using namespace Dali;

class TestProcessor : public Integration::Processor
{
public:
  TestProcessor()
  : processRun(false)
  {
  }

  virtual void Process(bool postProcessor)
  {
    processRun = true;
  }

  bool processRun;
};

int UtcDaliCoreProcessorP(void)
{
  TestApplication application;

  TestProcessor      testProcessor;
  Integration::Core& core = application.GetCore();
  core.RegisterProcessor(testProcessor);

  tet_infoline("Test that the processor has not been executed yet:");
  DALI_TEST_CHECK(testProcessor.processRun == false);

  application.SendNotification();

  tet_infoline("Test that the processor has been executed:");
  DALI_TEST_CHECK(testProcessor.processRun);

  // Clear down for next part of test
  testProcessor.processRun = false;

  core.UnregisterProcessor(testProcessor);
  application.SendNotification();
  tet_infoline("Test that the processor has not been executed again:");
  DALI_TEST_CHECK(testProcessor.processRun == false);

  END_TEST;
}

int UtcDaliCoreProcessorMultipleP(void)
{
  TestApplication application;

  TestProcessor testProcessor1;
  TestProcessor testProcessor2;
  TestProcessor testProcessor3;

  Integration::Core& core = application.GetCore();
  core.RegisterProcessor(testProcessor1);

  tet_infoline("Test that the processor has not been executed yet:");
  DALI_TEST_CHECK(testProcessor1.processRun == false);

  application.SendNotification();

  tet_infoline("Test that the processor has been executed:");
  DALI_TEST_CHECK(testProcessor1.processRun);

  // Clear down for next part of test
  testProcessor1.processRun = false;

  core.RegisterProcessor(testProcessor2);
  core.RegisterProcessor(testProcessor3);

  tet_infoline("Test that the processors have not been executed yet:");
  DALI_TEST_CHECK(testProcessor1.processRun == false);
  DALI_TEST_CHECK(testProcessor2.processRun == false);
  DALI_TEST_CHECK(testProcessor3.processRun == false);

  application.SendNotification();

  tet_infoline("Test that the processors have been executed:");
  DALI_TEST_CHECK(testProcessor1.processRun);
  DALI_TEST_CHECK(testProcessor2.processRun);
  DALI_TEST_CHECK(testProcessor3.processRun);

  // Clear down for next part of test
  testProcessor2.processRun = false;

  core.UnregisterProcessor(testProcessor2);
  application.SendNotification();
  tet_infoline("Test that the unregistered processor has not been executed again but others have");
  DALI_TEST_CHECK(testProcessor1.processRun);
  DALI_TEST_CHECK(testProcessor2.processRun == false);
  DALI_TEST_CHECK(testProcessor3.processRun);

  END_TEST;
}

int UtcDaliCorePostProcessorP(void)
{
  TestApplication application;

  TestProcessor      testProcessor;
  Integration::Core& core = application.GetCore();
  core.RegisterProcessor(testProcessor, true);

  tet_infoline("Test that the processor has not been executed yet:");
  DALI_TEST_CHECK(testProcessor.processRun == false);

  application.SendNotification();

  tet_infoline("Test that the processor has been executed:");
  DALI_TEST_CHECK(testProcessor.processRun);

  // Clear down for next part of test
  testProcessor.processRun = false;

  core.UnregisterProcessor(testProcessor);
  application.SendNotification();
  tet_infoline("Test that the processor is still executed:");
  DALI_TEST_CHECK(testProcessor.processRun);

  // Clear down for next part of test
  testProcessor.processRun = false;

  core.UnregisterProcessor(testProcessor, true);
  application.SendNotification();
  tet_infoline("Test that the processor has not been executed again:");
  DALI_TEST_CHECK(testProcessor.processRun == false);

  END_TEST;
}