//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>
#include <algorithm>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliImageAttributesConstructor();
static void UtcDaliImageAttributesLessThan();
static void UtcDaliImageAttributesEquality();
static void UtcDaliImageAttributesInEquality();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

static const float ROTATION_EPSILON = 0.0001f;

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliImageAttributesConstructor,  POSITIVE_TC_IDX },
    { UtcDaliImageAttributesLessThan,     POSITIVE_TC_IDX },
    { UtcDaliImageAttributesEquality,     POSITIVE_TC_IDX },
    { UtcDaliImageAttributesInEquality,   POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliImageAttributesConstructor()
{
  TestApplication application;

  tet_infoline("UtcDaliImageAttributesConstructor");

  // invoke default handle constructor
  ImageAttributes imageAttributes;

  DALI_TEST_CHECK( imageAttributes.GetWidth() == 0);
}

static void UtcDaliImageAttributesLessThan()
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

  ImageAttributes imageAttributesCrop;
  imageAttributesCrop.SetCrop(Rect<float>(0.0f, 0.0f, 8.0f, 8.0f));
  DALI_TEST_CHECK(imageAttributes < imageAttributesCrop);

  ImageAttributes imageAttributesFormat;
  imageAttributesFormat.SetPixelFormat(Pixel::BGRA8888);
  DALI_TEST_CHECK(imageAttributes < imageAttributesFormat);

  ImageAttributes imageAttributesScaling;
  imageAttributesScaling.SetScalingMode(ImageAttributes::FitHeight);
  DALI_TEST_CHECK(imageAttributes < imageAttributesScaling);
}

static void UtcDaliImageAttributesEquality()
{
  TestApplication application;

  tet_infoline("UtcDaliImageAttributesEquality");

  // invoke default handle constructor
  ImageAttributes imageAttributes01;
  ImageAttributes imageAttributes02;

  DALI_TEST_CHECK(imageAttributes02 == imageAttributes01);
}

static void UtcDaliImageAttributesInEquality()
{
  TestApplication application;

  tet_infoline("UtcDaliImageAttributesInEquality");

  // invoke default handle constructor
  ImageAttributes imageAttributes01;
  ImageAttributes imageAttributes02;

  DALI_TEST_CHECK((imageAttributes02 != imageAttributes01) == false);
}


