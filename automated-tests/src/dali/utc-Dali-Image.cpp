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


// 1.1
int UtcDaliImageNew01(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageNew01 - Image::New(const std::string&)");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = ResourceImage::New(gTestImageFilename);

  DALI_TEST_CHECK( image );
  END_TEST;
}

// 1.2
int UtcDaliImageNew02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageNew02 - Image::New(const std::string&, const ImageAttributes&)");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  Dali::ImageAttributes imageAttributes;
  imageAttributes.SetSize(128, 256);
  imageAttributes.SetScalingMode(Dali::ImageAttributes::FitHeight);
  image = ResourceImage::New(gTestImageFilename, imageAttributes);

  DALI_TEST_CHECK( image );
  END_TEST;
}

// 1.3
int UtcDaliImageNew03(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageNew03 - Image::New(NativeImage&)");

  // invoke default handle constructor
  Image image;
  TestNativeImagePointer nativeImage = TestNativeImage::New(16, 16);

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::New(*(nativeImage.Get()));

  DALI_TEST_CHECK( image );
  END_TEST;
}

// 1.4
int UtcDaliImageNewWithPolicies01(void)
{
  TestApplication application;

  // testing delayed loading
  tet_infoline("UtcDaliImageNewWithPolicies01 - Load image with LoadPolicy::OnDemand, ReleasePolicy::Never");
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  Image image = ResourceImage::New(gTestImageFilename, ResourceImage::ON_DEMAND, Image::NEVER);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading only when actor added to stage
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  ImageActor actor = ImageActor::New(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  // testing ReleasePolicy::Never
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );

  // never discard texture
  Stage::GetCurrent().Remove(actor);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );
  END_TEST;
}

// 1.5
int UtcDaliImageNewWithPolicies02(void)
{
  TestApplication application;
  const Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  // testing resource deletion when taken off stage
  tet_infoline("UtcDaliImageNewWithPolicies02 - Load image with LoadPolicy::OnDemand, ReleasePolicy::Unused");

  Image image = ResourceImage::New(gTestImageFilename, ResourceImage::ON_DEMAND, Image::UNUSED);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading only when actor added to stage
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  ImageActor actor = ImageActor::New(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  // testing ReleasePolicy::Unused
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );

  // discard texture when actor comes off stage
  Stage::GetCurrent().Remove(actor);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK ( application.GetGlAbstraction().CheckTextureDeleted(23) );
  END_TEST;
}

// 1.6
int UtcDaliImageNewWithPolicies03(void)
{
  TestApplication application;
  const Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  // load immediately -> resource deletion when taken off stage -> put actor back on stage -> load resource again
  tet_infoline("UtcDaliImageNewWithPolicies03 - Load image with LoadPolicy::Immediate, ReleasePolicy::Unused");

  Image image = ResourceImage::New(gTestImageFilename, ResourceImage::IMMEDIATE, Image::UNUSED);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading immediately
  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  ImageActor actor = ImageActor::New(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  // testing ReleasePolicy::Unused
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );

  // discard texture when actor comes off stage
  Stage::GetCurrent().Remove(actor);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK ( application.GetGlAbstraction().CheckTextureDeleted(23) );

  // check load request when actor added back to stage
  application.GetPlatform().ResetTrace();

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  END_TEST;
}

// 1.7
int UtcDaliImageNewWithPolicies04(void)
{
  TestApplication application;

  // load immediately, don't release texture when off stage
  tet_infoline("UtcDaliImageNewWithPolicies03 - Load image with LoadPolicy::Immediate, ReleasePolicy::Never");

  Image image = ResourceImage::New(gTestImageFilename, ResourceImage::IMMEDIATE, Image::NEVER);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading immediately
  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  ImageActor actor = ImageActor::New(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  // testing ReleasePolicy::Never
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );

  // texture is not discarded
  Stage::GetCurrent().Remove(actor);
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK ( !application.GetGlAbstraction().CheckTextureDeleted(23) );

  // no load request when actor added back to stage
  application.GetPlatform().ResetTrace();

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  END_TEST;
}

// 1.8
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

// 1.9
int UtcDaliImageGetImageSize(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("UtcDaliImageGetImageSize - Image::GetImageSize()");

  Vector2 testSize(8.0f, 16.0f);
  platform.SetClosestImageSize(testSize);

  Vector2 size = ResourceImage::GetImageSize(gTestImageFilename);

  DALI_TEST_CHECK( application.GetPlatform().GetTrace().FindMethod("GetClosestImageSize"));
  DALI_TEST_EQUALS( size, testSize, TEST_LOCATION);
  END_TEST;
}

// 1.10
int UtcDaliImageGetUrl(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetFilename");

  // invoke default handle constructor
  ResourceImage image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = ResourceImage::New(gTestImageFilename);

  DALI_TEST_EQUALS( image.GetUrl(), gTestImageFilename, TEST_LOCATION);
  END_TEST;
}

// 1.11
int UtcDaliImageGetLoadingState01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageGetLoadingState01");

  ResourceImage image = ResourceImage::New(gTestImageFilename);
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoading);
  application.SendNotification();
  application.Render(16);

  // simulate load success
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
  application.Render(16);
  application.SendNotification();

  // Test state == ResourceLoadingSucceeded
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoadingSucceeded);
  END_TEST;
}

// 1.12
int UtcDaliImageGetLoadingState02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetLoadingState02");

  // invoke default handle constructor
  ResourceImage image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = ResourceImage::New(gTestImageFilename);

  // Test state == ResourceLoading
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoading);
  application.SendNotification();
  application.Render(16);

  // simulate load failure
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoadFailed(request->GetId(), Integration::FailureUnknown);
  }
  application.Render(16);
  application.SendNotification();

  // Test state == ResourceLoadingFailed
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoadingFailed);
  END_TEST;
}

// 1.13
int UtcDaliImageGetReleasePolicy(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetReleasePolicy");

  Image image = ResourceImage::New(gTestImageFilename, ResourceImage::IMMEDIATE, Image::UNUSED);

  DALI_TEST_CHECK( image );

  DALI_TEST_CHECK( Image::UNUSED == image.GetReleasePolicy() );

  END_TEST;
}

// 1.14
int UtcDaliImageGetLoadPolicy(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetLoadPolicy");

  ResourceImage image = ResourceImage::New(gTestImageFilename, ResourceImage::ON_DEMAND, Image::NEVER);

  DALI_TEST_CHECK( image );

  DALI_TEST_CHECK( ResourceImage::ON_DEMAND == image.GetLoadPolicy());
  END_TEST;
}

static bool SignalLoadFlag = false;

static void SignalLoadHandler(ResourceImage image)
{
  tet_infoline("Received image load finished signal");

  SignalLoadFlag = true;
}

static bool SignalUploadedFlag = false;

static void SignalUploadedHandler(Image image)
{
  tet_infoline("Received image uploaded signal");

  SignalUploadedFlag = true;
}

// 1.15
int UtcDaliImageSignalLoadingFinished(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageSignalLoadingFinished");

  SignalLoadFlag = false;

  ResourceImage image = ResourceImage::New(gTestImageFilename);

  image.LoadingFinishedSignal().Connect( SignalLoadHandler );
  application.SendNotification();
  application.Render(16);

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD)));
  }

  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( SignalLoadFlag == true );
  END_TEST;
}

// 1.16
int UtcDaliImageSignalUploaded(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageSignalUploaded - Image::SignalUploaded()");

  // set up image in fake platform abstraction
  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  ResourceImage image = ResourceImage::New(gTestImageFilename);
  image.LoadingFinishedSignal().Connect( SignalLoadHandler );

  // Load image
  application.SendNotification();
  application.Render(16);
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
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

  DALI_TEST_CHECK( SignalLoadFlag == true );
  DALI_TEST_CHECK( SignalUploadedFlag == true );
  SignalLoadFlag = false;
  SignalUploadedFlag = false;

  image.Reload();
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 160, 160, 160, 160);

  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK( SignalLoadFlag == true );

  application.Render(16);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  DALI_TEST_CHECK( SignalUploadedFlag == true );
  END_TEST;
}

// 1.17
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
    Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
    Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
    Integration::ResourcePointer resource(bitmap);
    bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
    if(request)
    {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
    }
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

// 1.18
int UtcDaliImageDiscard02(void)
{
  TestApplication application;
  application.GetGlAbstraction().EnableTextureCallTrace( true );
  tet_infoline("UtcDaliImageDiscard02 - one actor, tests TextureCache::DiscardTexture");

  {
    {
      ImageActor actor;
      {
        ImageAttributes attrs;
        const Vector2 requestedSize( 40, 30 );
        attrs.SetSize( requestedSize.width, requestedSize.height );
        Image image = ResourceImage::New(gTestImageFilename, attrs);
        actor = ImageActor::New(image);
        Stage::GetCurrent().Add(actor);

        application.SendNotification();
        application.Render(16);

        std::vector<GLuint> ids;
        ids.push_back( 23 );
        application.GetGlAbstraction().SetNextTextureIds( ids );

        Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
        Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );
        Integration::ResourcePointer resource(bitmap);
        bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
        if(request)
        {
          application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
        }
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

// 1.19
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

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::DISCARD );

  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
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
