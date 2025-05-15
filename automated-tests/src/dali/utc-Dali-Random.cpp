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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

int UtcDaliRandomRangeMethod(void)
{
  TestApplication application; // Reset all test adapter return codes

  float a = 0, b = 1;
  for(size_t i = 0; i < 100; i++)
  {
    float r = Dali::Random::Range(a, b);
    DALI_TEST_CHECK(r >= a && r <= b);
  }

  a = 100;
  b = -100;
  for(size_t i = 0; i < 100; i++)
  {
    float r = Dali::Random::Range(a, b);
    DALI_TEST_CHECK(r >= b && r <= a);
  }
  END_TEST;
}

int UtcDaliRandomAxisMethod(void)
{
  TestApplication application; // Reset all test adapter return codes

  for(size_t i = 0; i < 100; i++)
  {
    Vector4 axis = Dali::Random::Axis();
    DALI_TEST_EQUALS(axis.Length3(), 1.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(axis.Length(), 1.0f, 0.0001f, TEST_LOCATION); // Check w is zero.
  }
  END_TEST;
}
