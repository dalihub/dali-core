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
#include <test-native-image.h>
#include <dali/integration-api/gl-abstraction.h>

using std::max;
using namespace Dali;

void utc_dali_framebuffer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_framebuffer_cleanup(void)
{
  test_return_value = TET_PASS;
}

static const float ROTATION_EPSILON = 0.0001f;


int UtcDaliFrameBufferImageNew01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageNew01 - FrameBufferImage::New(unsigned int, unsigned int, Pixel::Format)");

  // invoke default handle constructor
  FrameBufferImage image;
  Vector2 stageSize = Stage::GetCurrent().GetSize();

  // initialise handle
  image = FrameBufferImage::New();            // create framebuffer with the same dimensions as the stage
  ImageActor actor=ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( image );
  DALI_TEST_EQUALS((float)image.GetWidth(), stageSize.width, TEST_LOCATION);
  DALI_TEST_EQUALS((float)image.GetHeight(), stageSize.height, TEST_LOCATION);

  image = FrameBufferImage::New(16, 16);      // create framebuffer with dimensions of 16x16
  actor.SetImage(image);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK( image );
  DALI_TEST_EQUALS(image.GetWidth(), 16u, TEST_LOCATION);
  DALI_TEST_EQUALS(image.GetHeight(), 16u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFrameBufferImageNew02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageNew02 - FrameBufferImage::New(NativeImageInterface&)");

  // invoke default handle constructor
  FrameBufferImage image;
  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = FrameBufferImage::New(*(nativeImage.Get()));

  DALI_TEST_CHECK( image );
  END_TEST;
}

int UtcDaliFrameBufferImageNew03(void)
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageNew03 - FrameBufferImage::New(NativeImageInterface&, ReleasePolicy)");

  // invoke default handle constructor
  FrameBufferImage image;
  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);

  DALI_TEST_CHECK( !image );

  // initialise handle with UNUSED release policy
  image = FrameBufferImage::New(*(nativeImage.Get()), Image::UNUSED);

  DALI_TEST_CHECK( image );
  DALI_TEST_EQUALS( image.GetReleasePolicy(), Image::UNUSED, TEST_LOCATION );

  // initialise handle with NEVER release policy
  image.Reset();
  DALI_TEST_CHECK( !image );

  image = FrameBufferImage::New(*(nativeImage.Get()), Image::NEVER);

  DALI_TEST_CHECK( image );
  DALI_TEST_EQUALS( image.GetReleasePolicy(), Image::NEVER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFrameBufferImageAttachments01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageAttachments01 - FrameBufferImage::New(unsigned int, unsigned int, Pixel::Format, RenderBuffer::Format)");

  // invoke default handle constructor
  FrameBufferImage image;

  // initialise handle
  image = FrameBufferImage::New(64, 64, Pixel::RGBA8888, RenderBuffer::COLOR);       // create framebuffer with Color buffer
  ImageActor actor=ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachment(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferImageAttachments02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageAttachments02 - FrameBufferImage::New(unsigned int, unsigned int, Pixel::Format, RenderBuffer::Format)");

  // invoke default handle constructor
  FrameBufferImage image;

  // initialise handle
  image = FrameBufferImage::New(64, 64, Pixel::RGBA8888, RenderBuffer::COLOR_DEPTH); // create framebuffer with Color and Depth buffer
  ImageActor actor=ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachment(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferImageAttachments03(void)
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageAttachments03 - FrameBufferImage::New(unsigned int, unsigned int, Pixel::Format, RenderBuffer::Format)");

  // invoke default handle constructor
  FrameBufferImage image;

  // initialise handle
  image = FrameBufferImage::New(64, 64, Pixel::RGBA8888, RenderBuffer::COLOR_STENCIL);  // create framebuffer with Color and Stencil
  ImageActor actor=ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachment(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferImageAttachments04(void)
{
  TestApplication application;

  tet_infoline("UtcDaliFrameBufferImageAttachments04 - FrameBufferImage::New(unsigned int, unsigned int, Pixel::Format, RenderBuffer::Format)");

  // invoke default handle constructor
  FrameBufferImage image;

  // initialise handle
  image = FrameBufferImage::New(64, 64, Pixel::RGBA8888, RenderBuffer::COLOR_DEPTH_STENCIL);  // create framebuffer with Color, Depth and Stencil buffers
  ImageActor actor=ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferColorAttachment(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferDepthAttachment(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetGlAbstraction().CheckFramebufferStencilAttachment(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFrameBufferImageDownCast(void)
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
  END_TEST;
}
