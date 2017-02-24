/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/bitmap.h>
#include <dali-test-suite-utils.h>
#include <test-native-image.h>

using namespace Dali;

void utc_dali_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

static const char* gTestImageFilename = "icon_wrt.png";

namespace
{

}

int UtcDaliImageDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Image::DownCast()");

  Image image = ResourceImage::New(gTestImageFilename);

  BaseHandle object(image);

  Image image2 = Image::DownCast(object);
  DALI_TEST_CHECK(image2);

  Image image3 = DownCast< Image >(object);
  DALI_TEST_CHECK(image3);

  BaseHandle unInitializedObject;
  Image image4 = Image::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!image4);

  Image image5 = DownCast< Image >(unInitializedObject);
  DALI_TEST_CHECK(!image5);
  END_TEST;
}

int UtcDaliImageGetWidthHeight(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetWidthHeight - Image::GetWidth() & Image::GetHeight");

  Vector2 testSize(8.0f, 16.0f);
  application.GetPlatform().SetClosestImageSize(testSize);
  PrepareResourceImage( application, testSize.x, testSize.y, Pixel::RGBA8888 );
  Image image1 = ResourceImage::New(gTestImageFilename);
  DALI_TEST_EQUALS( image1.GetWidth(), testSize.x, TEST_LOCATION );
  DALI_TEST_EQUALS( image1.GetHeight(), testSize.y, TEST_LOCATION );

  testSize = Vector2(128.0f, 256.0f);
  PrepareResourceImage( application, testSize.x, testSize.y, Pixel::RGBA8888 );
  Image image2 = ResourceImage::New( gTestImageFilename, ImageDimensions(testSize.x, testSize.y), FittingMode::SCALE_TO_FILL, SamplingMode::DEFAULT );
  DALI_TEST_EQUALS( image2.GetWidth(), testSize.x, TEST_LOCATION );
  DALI_TEST_EQUALS( image2.GetHeight(), testSize.y, TEST_LOCATION );

  testSize = Vector2(16.0f, 32.0f);
  Image image3 = FrameBufferImage::New(testSize.x, testSize.y);
  DALI_TEST_EQUALS(image3.GetWidth(), testSize.x, TEST_LOCATION);
  DALI_TEST_EQUALS(image3.GetHeight(), testSize.y, TEST_LOCATION);

  testSize = Vector2(32.0f, 64.0f);
  PrepareResourceImage( application, testSize.x, testSize.y, Pixel::RGBA8888 );
  TestNativeImagePointer nativeImage = TestNativeImage::New(testSize.x, testSize.y);
  Image image4 = NativeImage::New(*(nativeImage.Get()));
  DALI_TEST_EQUALS(image4.GetWidth(), testSize.x, TEST_LOCATION);
  DALI_TEST_EQUALS(image4.GetHeight(), testSize.y, TEST_LOCATION);

  END_TEST;
}

int UtcDaliImageDiscard01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageDiscard01 - no actors");

  {
    Image image = ResourceImage::New(gTestImageFilename);

    // Load image
    application.SendNotification();
    application.Render(16);
    std::vector<GLuint> ids;
    ids.push_back( 23 );
    application.GetGlAbstraction().SetNextTextureIds( ids );
    application.Render(16);
    application.SendNotification();
  } // Drop image handle

  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  // Shouldn't have been sent to GL...
  const std::vector<GLuint>& texIds = application.GetGlAbstraction().GetNextTextureIds();
  DALI_TEST_CHECK( texIds.size() == 1 );
  DALI_TEST_CHECK( texIds[0] == 23 );
  END_TEST;
}
