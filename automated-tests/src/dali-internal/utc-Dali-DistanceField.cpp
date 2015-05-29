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
#include <algorithm>

#include <stdlib.h>

#include <dali/public-api/dali-core.h>
#include <dali/devel-api/images/distance-field.h>
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

namespace
{

static const float ROTATION_EPSILON = 0.0001f;

static unsigned char sourceImage[] =
{
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

} // anonymous namespace



int UtcDaliGenerateDistanceField(void)
{
  unsigned char distanceField[4*4];

  GenerateDistanceFieldMap(sourceImage, Size(8.0f, 8.0f), distanceField, Size(4.0f, 4.0f), 0, Size(4.0f, 4.0f));

  if(distanceField[0]  <= distanceField[5] &&
     distanceField[5]  <= distanceField[10] &&
     distanceField[10] <= distanceField[15])
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }
  END_TEST;
}
