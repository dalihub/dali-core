/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

using namespace Dali;

void utc_dali_resource_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_resource_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* gTestImageFilename = "icon_wrt.png";
} // unnamed namespace


// 1.1
int UtcDaliResourceImageNew01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliResourceImageNew01 - ResourceImage::New(const std::string&)");

  // invoke default handle constructor
  ResourceImage image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = ResourceImage::New(gTestImageFilename);

  DALI_TEST_CHECK( image );
  END_TEST;
}

// 1.2
int UtcDaliResourceImageNew02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliREsourceImageNew02 - ResourceImage New( const std::string& url, ImageDimensions size, FittingMode scalingMode, SamplingMode samplingMode, bool orientationCorrection = true )");

  // invoke default handle constructor
  ResourceImage image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = ResourceImage::New(gTestImageFilename, ImageDimensions( 128, 256 ), FittingMode::FIT_HEIGHT );

  DALI_TEST_CHECK( image );
  END_TEST;
}

// 1.7
int UtcDaliResourceImageDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::ResourceImage::DownCast()");

  ResourceImage image = ResourceImage::New(gTestImageFilename);

  BaseHandle object(image);

  ResourceImage image2 = ResourceImage::DownCast(object);
  DALI_TEST_CHECK(image2);

  ResourceImage image3 = DownCast< ResourceImage >(object);
  DALI_TEST_CHECK(image3);

  BaseHandle unInitializedObject;
  ResourceImage image4 = ResourceImage::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!image4);

  ResourceImage image5 = DownCast< ResourceImage >(unInitializedObject);
  DALI_TEST_CHECK(!image5);

  Image image6 = ResourceImage::New(gTestImageFilename);
  ResourceImage image7 = ResourceImage::DownCast(image6);
  DALI_TEST_CHECK(image7);
  END_TEST;
}

// 1.8
int UtcDaliResourceImageGetImageSize(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("UtcDaliResourceImageGetImageSize - ResourceImage::GetImageSize()");

  Vector2 testSize(8.0f, 16.0f);
  platform.SetClosestImageSize(testSize);

  const ImageDimensions size = ResourceImage::GetImageSize(gTestImageFilename);

  DALI_TEST_CHECK( application.GetPlatform().GetTrace().FindMethod("GetClosestImageSize"));
  DALI_TEST_EQUALS( Vector2( size.GetX(), size.GetY() ), testSize, TEST_LOCATION);
  END_TEST;
}

// 1.9
int UtcDaliResourceImageGetUrl(void)
{
  TestApplication application;

  tet_infoline("UtcDaliResourceImageGetFilename - ResourceImage::GetUrl()");

  // invoke default handle constructor
  ResourceImage image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = ResourceImage::New(gTestImageFilename);

  DALI_TEST_EQUALS( image.GetUrl(), gTestImageFilename, TEST_LOCATION);
  END_TEST;
}

// 1.10
int UtcDaliResourceImageGetLoadingState01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliResourceImageGetLoadingState01");

  ResourceImage image = ResourceImage::New(gTestImageFilename);
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoadingFailed);

  // simulate load success
  PrepareResourceImage( application, 100u, 100u, Pixel::RGBA8888 );
  image.Reload();

  // Test state == ResourceLoadingSucceeded
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoadingSucceeded);
  END_TEST;
}

// 1.11
int UtcDaliResourceImageGetLoadingState02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliResourceImageGetLoadingState02");

  // invoke default handle constructor
  ResourceImage image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = ResourceImage::New(gTestImageFilename);

  // Test state == ResourceLoadingFailed
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoadingFailed);

  PrepareResourceImage( application, 100u, 100u, Pixel::RGBA8888 );
  image.Reload();

  // Test state == ResourceLoadingFailed
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoadingSucceeded);
  END_TEST;
}

static bool SignalLoadFlag = false;

static void SignalLoadHandler(ResourceImage image)
{
  tet_infoline("Received image load finished signal");

  SignalLoadFlag = true;
}

// 1.13
int UtcDaliResourceImageSignalLoadingFinished(void)
{
  TestApplication application;

  tet_infoline("UtcDaliResourceImageSignalLoadingFinished");

  SignalLoadFlag = false;

  PrepareResourceImage( application, 100u, 100u, Pixel::RGBA8888 );
  ResourceImage image = ResourceImage::New(gTestImageFilename);

  image.LoadingFinishedSignal().Connect( SignalLoadHandler );
  image.Reload();
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( SignalLoadFlag == true );
  END_TEST;
}
