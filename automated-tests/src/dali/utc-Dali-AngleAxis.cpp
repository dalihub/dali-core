/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

void utc_dali_angle_axis_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_angle_axis_cleanup(void)
{
  test_return_value = TET_PASS;
}



int UtcDaliAngleAxisNew01(void)
{
  TestApplication application;

  AngleAxis a;
  DALI_TEST_EQUALS(float(a.angle), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.axis, Vector3(0.0f, 0.0f, 0.0f), 0.001f, TEST_LOCATION);
  END_TEST;
}



int UtcDaliAngleAxisNew02(void)
{
  TestApplication application;

  Degree d(75.0f);
  AngleAxis a(d, Vector3::XAXIS);

  DALI_TEST_EQUALS(a.angle, Radian(d), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.axis, Vector3::XAXIS, 0.001f, TEST_LOCATION);
  END_TEST;
}


int UtcDaliAngleAxisNew03(void)
{
  TestApplication application;

  Radian r(Math::PI_2);
  AngleAxis a(r, Vector3::ZAXIS);

  // AngleAxis stores its angle as a degree, so should only do degree comparison.
  DALI_TEST_EQUALS(a.angle, Radian(Math::PI_2), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.axis, Vector3::ZAXIS, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAngleAxisAssign(void)
{
  TestApplication application;

  Radian r(Math::PI_2);
  AngleAxis a(r, Vector3::ZAXIS);

  AngleAxis b = a;

  // AngleAxis stores its angle as a degree, so should only do degree comparison.
  DALI_TEST_EQUALS(b.angle, Radian(Math::PI_2), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.axis, Vector3::ZAXIS, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAngleAxisCopy(void)
{
  TestApplication application;

  Radian r(Math::PI_2);
  AngleAxis a(r, Vector3::ZAXIS);
  AngleAxis b(a);

  // AngleAxis stores its angle as a degree, so should only do degree comparison.
  DALI_TEST_EQUALS(b.angle, Radian(Math::PI_2), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(b.axis, Vector3::ZAXIS, 0.001f, TEST_LOCATION);
  END_TEST;
}
