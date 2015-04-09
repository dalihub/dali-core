/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

#include <mesh-builder.h>

void geometry_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void geometry_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
}


int UtcDaliGeometryNew01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  DALI_TEST_EQUALS( (bool)geometry, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryNew02(void)
{
  TestApplication application;
  Geometry geometry;
  DALI_TEST_EQUALS( (bool)geometry, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryDownCast01(void)
{
  TestApplication application;

  Geometry geometry = Geometry::New();

  BaseHandle handle(geometry);
  Geometry geometry2 = Geometry::DownCast(handle);
  DALI_TEST_EQUALS( (bool)geometry2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliGeometryDownCast02(void)
{
  TestApplication application;

  Handle handle = Handle::New(); // Create a custom object
  Geometry geometry = Geometry::DownCast(handle);
  DALI_TEST_EQUALS( (bool)geometry, false, TEST_LOCATION );
  END_TEST;
}
