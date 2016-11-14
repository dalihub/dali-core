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

using namespace Dali;

void utc_dali_resource_image_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_resource_image_cleanup(void)
{
  test_return_value = TET_PASS;
}

static const char* gTestImageFilename = "icon_wrt.png";

namespace
{

void LoadBitmapResource(TestPlatformAbstraction& platform)
{
  Integration::ResourceRequest* request = platform.GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD );
  Integration::ResourcePointer resource(bitmap);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);

  if(request)
  {
    platform.SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
  }
}

} // namespace


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

// 1.3
int UtcDaliResourceImageNewWithPolicies01(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  // testing delayed loading
  tet_infoline("UtcDaliResourceImageNewWithPolicies01 - Load image with LoadPolicy::OnDemand, ReleasePolicy::Never");
  DALI_TEST_CHECK( !platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  ResourceImage image = ResourceImage::New(gTestImageFilename, ResourceImage::ON_DEMAND, Image::NEVER);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading only when actor added to stage
  DALI_TEST_CHECK( !platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  Actor actor = CreateRenderableActor(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  // testing ReleasePolicy::Never
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  LoadBitmapResource( platform );

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

// 1.4
int UtcDaliResourceImageNewWithPolicies02(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();
  const Vector2 closestImageSize( 80, 45);
  platform.SetClosestImageSize(closestImageSize);

  // testing resource deletion when taken off stage
  tet_infoline("UtcDaliResourceImageNewWithPolicies02 - Load image with LoadPolicy::OnDemand, ReleasePolicy::Unused");

  ResourceImage image = ResourceImage::New(gTestImageFilename, ResourceImage::ON_DEMAND, Image::UNUSED);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading only when actor added to stage
  DALI_TEST_CHECK( !platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  Actor actor = CreateRenderableActor(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK( platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  // testing ReleasePolicy::Unused
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  LoadBitmapResource( platform );

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

// 1.5
int UtcDaliResourceImageNewWithPolicies03(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();
  const Vector2 closestImageSize( 80, 45);
  platform.SetClosestImageSize(closestImageSize);

  // load immediately -> resource deletion when taken off stage -> put actor back on stage -> load resource again
  tet_infoline("UtcDaliResourceImageNewWithPolicies03 - Load image with LoadPolicy::Immediate, ReleasePolicy::Unused");

  ResourceImage image = ResourceImage::New(gTestImageFilename, ResourceImage::IMMEDIATE, Image::UNUSED);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading immediately
  DALI_TEST_CHECK( platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  Actor actor = CreateRenderableActor(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  // testing ReleasePolicy::Unused
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  LoadBitmapResource( platform );

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

  DALI_TEST_CHECK( platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  END_TEST;
}

// 1.6
int UtcDaliResourceImageNewWithPolicies04(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  // load immediately, don't release texture when off stage
  tet_infoline("UtcDaliResourceImageNewWithPolicies03 - Load image with LoadPolicy::Immediate, ReleasePolicy::Never");

  ResourceImage image = ResourceImage::New(gTestImageFilename, ResourceImage::IMMEDIATE, Image::NEVER);

  DALI_TEST_CHECK( image );

  application.SendNotification();
  application.Render(16);

  // request file loading immediately
  DALI_TEST_CHECK( platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );

  Actor actor = CreateRenderableActor(image);

  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  // testing ReleasePolicy::Never
  // fake loading image
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  LoadBitmapResource(platform);

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

  DALI_TEST_CHECK( !platform.WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
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
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoading);
  application.SendNotification();
  application.Render(16);

  // simulate load success
  TestPlatformAbstraction& platform = application.GetPlatform();
  LoadBitmapResource( platform );
  application.Render(16);
  application.SendNotification();

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

// 1.12
int UtcDaliResourceImageGetLoadPolicy(void)
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

// 1.13
int UtcDaliResourceImageSignalLoadingFinished(void)
{
  TestApplication application;

  tet_infoline("UtcDaliResourceImageSignalLoadingFinished");

  SignalLoadFlag = false;

  ResourceImage image = ResourceImage::New(gTestImageFilename);

  image.LoadingFinishedSignal().Connect( SignalLoadHandler );
  application.SendNotification();
  application.Render(16);

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)));
  }

  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( SignalLoadFlag == true );
  END_TEST;
}
