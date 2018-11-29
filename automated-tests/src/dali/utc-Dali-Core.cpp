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

#include <iostream>
#include <sstream>
#include <cmath> // isfinite

#include <stdlib.h>
#include <dali/integration-api/core.h>
#include <dali-test-suite-utils.h>

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

  DALI_TEST_EQUALS( 2, application.GetCore().GetMaximumUpdateCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliCoreSetStereoBase(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::GetMaximumUpdateCount");

  application.GetCore().SetViewMode( STEREO_HORIZONTAL );
  DALI_TEST_EQUALS( application.GetCore().GetViewMode(), STEREO_HORIZONTAL, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  application.GetCore().SetViewMode( STEREO_VERTICAL );
  DALI_TEST_EQUALS( application.GetCore().GetViewMode(), STEREO_VERTICAL, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  application.GetCore().SetViewMode( MONO );
  DALI_TEST_EQUALS( application.GetCore().GetViewMode(), MONO, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  END_TEST;
}

