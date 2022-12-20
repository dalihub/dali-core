/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <stdlib.h>

#include <cmath> // isfinite
#include <iostream>
#include <sstream>

using namespace Dali;

void utc_dali_core_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_core_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCoreGetMaximumUpdateCount(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::GetMaximumUpdateCount");

  DALI_TEST_EQUALS(2, application.GetCore().GetMaximumUpdateCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCoreGetObjectRegistry(void)
{
  TestApplication application;
  DALI_TEST_CHECK(application.GetCore().GetObjectRegistry());
  END_TEST;
}

int UtcDaliCoreCheckMemoryPool(void)
{
  TestApplication application;

  // Calling LogMemoryPools should check capacity across the board.
  application.GetCore().LogMemoryPools();

  tet_result(TET_PASS);
  END_TEST;
}
