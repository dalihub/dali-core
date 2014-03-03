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
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliFrameBufferImageNew01();
static void UtcDaliFrameBufferImageDownCast();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

static const float ROTATION_EPSILON = 0.0001f;

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliFrameBufferImageNew01, POSITIVE_TC_IDX },
    { UtcDaliFrameBufferImageDownCast, POSITIVE_TC_IDX },
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

static void UtcDaliFrameBufferImageNew01()
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageNew01 - FrameBufferImage::New(unsigned int, unsigned int, Pixel::Format)");

  // invoke default handle constructor
  FrameBufferImage image;
  Dali::ImageAttributes attributes;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // initialise handle
  image = FrameBufferImage::New();            // create framebuffer with the same dimensions as the stage
  ImageActor actor=ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  attributes = image.GetAttributes();

  DALI_TEST_CHECK( image );
  DALI_TEST_EQUALS((float)attributes.GetWidth(), stageSize.width, TEST_LOCATION);
  DALI_TEST_EQUALS((float)attributes.GetHeight(), stageSize.height, TEST_LOCATION);

  image = FrameBufferImage::New(16, 16);      // create framebuffer with dimensions of 16x16
  actor.SetImage(image);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  attributes = image.GetAttributes();

  DALI_TEST_CHECK( image );
  DALI_TEST_EQUALS(attributes.GetWidth(), 16u, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetHeight(), 16u, TEST_LOCATION);
}

static void UtcDaliFrameBufferImageDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::FrameBufferImage::DownCast()");

  FrameBufferImage image = FrameBufferImage::New();

  BaseHandle object(image);

  FrameBufferImage image2 = FrameBufferImage::DownCast(object);
  DALI_TEST_CHECK(image2);

  FrameBufferImage image3 = DownCast< FrameBufferImage >(object);
  DALI_TEST_CHECK(image3);

  BaseHandle unInitializedObject;
  FrameBufferImage image4 = FrameBufferImage::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!image4);

  FrameBufferImage image5 = DownCast< FrameBufferImage >(unInitializedObject);
  DALI_TEST_CHECK(!image5);

  Image image6 = FrameBufferImage::New();
  FrameBufferImage image7 = FrameBufferImage::DownCast(image6);
  DALI_TEST_CHECK(image7);
}
