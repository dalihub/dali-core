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

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <test-native-image.h>

using namespace Dali;

void utc_dali_native_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_native_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliNativeImageNew(void)
{
  TestApplication application;

  tet_infoline("UtcDaliNativeImageNew - NativeImage::New(NativeImageInterface&)");

  // invoke default handle constructor
  NativeImage image;
  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = NativeImage::New(*(nativeImage.Get()));

  DALI_TEST_CHECK( image );
  END_TEST;
}

int UtcDaliNativeImageDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Image::DownCast()");

  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);
  NativeImage image = NativeImage::New(*(nativeImage.Get()));

  BaseHandle object(image);

  NativeImage image2 = NativeImage::DownCast(object);
  DALI_TEST_CHECK(image2);

  NativeImage image3 = DownCast< NativeImage >(object);
  DALI_TEST_CHECK(image3);

  BaseHandle unInitializedObject;
  NativeImage image4 = NativeImage::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!image4);

  NativeImage image5 = DownCast< NativeImage >(unInitializedObject);
  DALI_TEST_CHECK(!image5);

  Image image6 = NativeImage::New(*(nativeImage.Get()));
  NativeImage image7 = NativeImage::DownCast(image6);
  DALI_TEST_CHECK(image7);
  END_TEST;
}
