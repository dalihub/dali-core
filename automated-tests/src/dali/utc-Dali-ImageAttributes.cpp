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
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

void utc_dali_image_attributes_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_attributes_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliImageAttributesConstructor(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageAttributesConstructor");

  // invoke default handle constructor
  ImageAttributes imageAttributes;

  DALI_TEST_CHECK( imageAttributes.GetWidth() == 0);
  END_TEST;
}

int UtcDaliImageAttributesLessThan(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageAttributesLessThan");

  // invoke default handle constructor
  ImageAttributes imageAttributes;

  ImageAttributes imageAttributesWidth;
  imageAttributesWidth.SetSize(2,1);
  DALI_TEST_CHECK(imageAttributes < imageAttributesWidth);

  ImageAttributes imageAttributesHeight;
  imageAttributesHeight.SetSize(1,2);
  DALI_TEST_CHECK(imageAttributes < imageAttributesHeight);

  imageAttributesWidth.SetSize(Size(2,1));
  DALI_TEST_CHECK(imageAttributes < imageAttributesWidth);

  imageAttributesHeight.SetSize(Size(1,2));
  DALI_TEST_CHECK(imageAttributes < imageAttributesHeight);

  ImageAttributes imageAttributesFormat;
  imageAttributesFormat.SetPixelFormat(Pixel::BGRA8888);
  DALI_TEST_CHECK(imageAttributes < imageAttributesFormat);

  ImageAttributes imageAttributesScaling;
  imageAttributesScaling.SetScalingMode(ImageAttributes::FitHeight);
  DALI_TEST_CHECK(imageAttributes < imageAttributesScaling);
  END_TEST;
}

int UtcDaliImageAttributesEquality(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageAttributesEquality");

  // invoke default handle constructor
  ImageAttributes imageAttributes01;
  ImageAttributes imageAttributes02;

  DALI_TEST_CHECK(imageAttributes02 == imageAttributes01);
  END_TEST;
}

int UtcDaliImageAttributesInEquality(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageAttributesInEquality");

  // invoke default handle constructor
  ImageAttributes imageAttributes01;
  ImageAttributes imageAttributes02;

  DALI_TEST_CHECK((imageAttributes02 != imageAttributes01) == false);
  END_TEST;
}
