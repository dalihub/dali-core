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
void LoadBitmapResource(TestPlatformAbstraction& platform)
{
  Integration::ResourceRequest* request = platform.GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
}

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

int UtcDaliImageGetReleasePolicy(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetReleasePolicy");

  Image image = ResourceImage::New(gTestImageFilename, ResourceImage::IMMEDIATE, Image::UNUSED);

  DALI_TEST_CHECK( image );

  DALI_TEST_CHECK( Image::UNUSED == image.GetReleasePolicy() );

  END_TEST;
}

int UtcDaliImageGetWidthHeight(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetWidthHeight - Image::GetWidth() & Image::GetHeight");

  Vector2 testSize(8.0f, 16.0f);
  application.GetPlatform().SetClosestImageSize(testSize);
  Image image1 = ResourceImage::New(gTestImageFilename);
  DALI_TEST_EQUALS( image1.GetWidth(), testSize.width, TEST_LOCATION );
  DALI_TEST_EQUALS( image1.GetHeight(), testSize.height, TEST_LOCATION );

  Image image2 = ResourceImage::New( gTestImageFilename, ImageDimensions(128, 256), FittingMode::SCALE_TO_FILL, SamplingMode::DEFAULT );
  DALI_TEST_EQUALS( image2.GetWidth(), 128u, TEST_LOCATION );
  DALI_TEST_EQUALS( image2.GetHeight(), 256u, TEST_LOCATION );

  Image image3 = FrameBufferImage::New(16, 32);
  DALI_TEST_EQUALS(image3.GetWidth(), 16u, TEST_LOCATION);
  DALI_TEST_EQUALS(image3.GetHeight(), 32u, TEST_LOCATION);

  TestNativeImagePointer nativeImage = TestNativeImage::New(32, 64);
  Image image4 = NativeImage::New(*(nativeImage.Get()));
  DALI_TEST_EQUALS(image4.GetWidth(), 32u, TEST_LOCATION);
  DALI_TEST_EQUALS(image4.GetHeight(), 64u, TEST_LOCATION);

  END_TEST;
}

static bool SignalUploadedFlag = false;

static void SignalUploadedHandler(Image image)
{
  tet_infoline("Received image uploaded signal");

  SignalUploadedFlag = true;
}

int UtcDaliImageSignalUploaded(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();
  tet_infoline("UtcDaliImageSignalUploaded - Image::SignalUploaded()");

  // set up image in fake platform abstraction
  Vector2 testSize(80.0f, 80.0f);
  platform.SetClosestImageSize(testSize);

  ResourceImage image = ResourceImage::New(gTestImageFilename);

  // Load image
  application.SendNotification();
  application.Render(16);
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  Integration::ResourceRequest* request = platform.GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
  application.Render(16);
  application.SendNotification();

  image.UploadedSignal().Connect( SignalUploadedHandler );

  Dali::ImageActor imageActor = ImageActor::New(image);
  Stage::GetCurrent().Add(imageActor);
  imageActor.SetSize(80, 80);
  imageActor.SetVisible(true);

  application.SendNotification();
  application.Render(0);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( SignalUploadedFlag == true );
  SignalUploadedFlag = false;

  image.Reload();
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 160, 160, 160, 160);

  // image loading
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  //upload
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK( SignalUploadedFlag == true );
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
    TestPlatformAbstraction& platform = application.GetPlatform();
    LoadBitmapResource( platform );
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

int UtcDaliImageDiscard02(void)
{
  TestApplication application;
  application.GetGlAbstraction().EnableTextureCallTrace( true );
  tet_infoline("UtcDaliImageDiscard02 - one actor, tests TextureCache::DiscardTexture");

  {
    {
      ImageActor actor;
      {
        Image image = ResourceImage::New(gTestImageFilename, ImageDimensions( 40, 30 ) );
        actor = ImageActor::New(image);
        Stage::GetCurrent().Add(actor);

        application.SendNotification();
        application.Render(16);

        std::vector<GLuint> ids;
        ids.push_back( 23 );
        application.GetGlAbstraction().SetNextTextureIds( ids );

        TestPlatformAbstraction& platform = application.GetPlatform();
        LoadBitmapResource( platform );
        application.Render(16);
        application.SendNotification();
        DALI_TEST_CHECK( application.GetGlAbstraction().GetTextureTrace().FindMethod("BindTexture") );
      } // lose image handle, actor should still keep one
      application.SendNotification();
      application.Render(16);

      Stage::GetCurrent().Remove(actor);
      application.SendNotification();
      application.Render(16);
    } // lose actor
    application.SendNotification();
    application.Render(16);
  }

  // Cleanup
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  // texture should have been removed:
  DALI_TEST_CHECK( application.GetGlAbstraction().CheckTextureDeleted( 23 ));
  END_TEST;
}

int UtcDaliImageDiscard03(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageDiscard03 - one actor, tests TextureCache::RemoveObserver");

  const Vector2 closestImageSize( 1, 1);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  Image image = ResourceImage::New(gTestImageFilename);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  TestPlatformAbstraction& platform = application.GetPlatform();
  LoadBitmapResource( platform );
  application.Render(16);
  application.SendNotification();
  application.SendNotification();
  application.Render(16);

  const std::vector<GLuint>& texIds = application.GetGlAbstraction().GetNextTextureIds();
  DALI_TEST_CHECK( texIds.size() == 0 );
  const std::vector<GLuint>& boundTexIds = application.GetGlAbstraction().GetBoundTextures();
  DALI_TEST_CHECK( boundTexIds[0] == 23 );

  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16); // Should remove image renderer

  END_TEST;
}

int UtcDaliImageContextLoss(void)
{
  TestApplication application; // Default config: DALI_DISCARDS_ALL_DATA

  const Vector2 closestImageSize( 80, 80 );
  TestPlatformAbstraction& platform = application.GetPlatform();
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  platform.SetClosestImageSize(closestImageSize);

  tet_infoline("UtcDaliImageContextLoss - Load image with LoadPolicy::Immediate, ReleasePolicy::Never, bitmap discard. Check that the image is re-requested on context regain\n");

  Image image = ResourceImage::New("image.png", ResourceImage::IMMEDIATE, Image::NEVER);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading immediately

  DALI_TEST_CHECK( platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  textureTrace.Enable(true);

  std::vector<GLuint> ids;
  ids.push_back( 23 );
  glAbstraction.SetNextTextureIds( ids );

  LoadBitmapResource(platform);

  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( textureTrace.FindMethod("GenTextures") );

  textureTrace.Reset();
  textureTrace.Enable(true);
  platform.ResetTrace();
  platform.EnableTrace(true);

  // Lose & regain context (in render 'thread')
  application.ResetContext();

  application.GetCore().RecoverFromContextLoss(); // start the recovery process
  application.SendNotification();

  // Run update/render loop
  application.Render(16);
  application.SendNotification();

  // Expect new load request
  DALI_TEST_CHECK( platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  // Finish loading image
  LoadBitmapResource(platform);
  ids.clear();
  ids.push_back( 57 );
  glAbstraction.SetNextTextureIds(ids);

  // Run update/render loop
  application.Render(16);
  application.SendNotification();

  // Expect new GenTextures
  DALI_TEST_CHECK( textureTrace.FindMethod("GenTextures") );

  END_TEST;
}
