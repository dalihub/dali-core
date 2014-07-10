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
  image = Image::New(gTestImageFilename);

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
  image = Image::New(gTestImageFilename, imageAttributes);

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
  Image image = Image::New(gTestImageFilename, Image::OnDemand, Image::Never);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

  // testing resource deletion when taken off stage
  tet_infoline("UtcDaliImageNewWithPolicies02 - Load image with LoadPolicy::OnDemand, ReleasePolicy::Unused");

  Image image = Image::New(gTestImageFilename, Image::OnDemand, Image::Unused);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

  // load immediately -> resource deletion when taken off stage -> put actor back on stage -> load resource again
  tet_infoline("UtcDaliImageNewWithPolicies03 - Load image with LoadPolicy::Immediate, ReleasePolicy::Unused");

  Image image = Image::New(gTestImageFilename, Image::Immediate, Image::Unused);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

  Image image = Image::New(gTestImageFilename, Image::Immediate, Image::Never);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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
int UtcDaliImageNewDistanceField(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageNewDistanceField - Image::NewDistanceField(const std::string&)");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::NewDistanceField(gTestImageFilename);

  DALI_TEST_CHECK( image );
  END_TEST;
}

// 1.9
int UtcDaliImageNewDistanceFieldWithPolicies01(void)
{
  TestApplication application;

  // testing delayed loading
  tet_infoline("UtcDaliImageNewDistanceFieldWithPolicies01 - Load image with LoadPolicy::OnDemand, ReleasePolicy::Never");
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc) );
  Image image = Image::NewDistanceField(gTestImageFilename, Image::OnDemand, Image::Never);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

// 1.10
int UtcDaliImageNewDistanceFieldWithPolicies02(void)
{
  TestApplication application;

  // testing resource deletion when taken off stage
  tet_infoline("UtcDaliImageNewDistanceFieldWithPolicies02 - Load image with LoadPolicy::OnDemand, ReleasePolicy::Unused");

  Image image = Image::NewDistanceField(gTestImageFilename, Image::OnDemand, Image::Unused);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

// 1.11
int UtcDaliImageNewDistanceFieldWithPolicies03(void)
{
  TestApplication application;

  // load immediately -> resource deletion when taken off stage -> put actor back on stage -> load resource again
  tet_infoline("UtcDaliImageNewDistanceFieldWithPolicies03 - Load image with LoadPolicy::Immediate, ReleasePolicy::Unused");

  Image image = Image::NewDistanceField(gTestImageFilename, Image::Immediate, Image::Unused);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

// 1.12
int UtcDaliImageNewDistanceFieldWithPolicies04(void)
{
  TestApplication application;

  // load immediately, don't release texture when off stage
  tet_infoline("UtcDaliImageNewDistanceFieldWithPolicies04 - Load image with LoadPolicy::Immediate, ReleasePolicy::Never");

  Image image = Image::NewDistanceField(gTestImageFilename, Image::Immediate, Image::Never);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

// 1.13
int UtcDaliImageNewDistanceFieldWithAttributes(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageNewDistanceFieldWithAttributes - Image::NewDistanceField(const std::string&, const ImageAttributes& attributes)");

  // invoke default handle constructor
  Image image;
  Dali::ImageAttributes imageAttributes = Dali::ImageAttributes::NewDistanceField(6.0f, 12);

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::NewDistanceField(gTestImageFilename, imageAttributes);

  DALI_TEST_CHECK( image );
  END_TEST;
}

// 1.14
int UtcDaliImageNewDistanceFieldWithAttrandPol(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageNewDistanceFieldWithAttrandPol - Load image with LoadPolicy::OnDemand, ReleasePolicy::Unused");

  Dali::ImageAttributes imageAttributes = Dali::ImageAttributes::NewDistanceField(6.0f, 12);

  Image image = Image::NewDistanceField(gTestImageFilename, imageAttributes, Image::OnDemand, Image::Unused);

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
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

// 1.15
int UtcDaliImageDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Image::DownCast()");

  Image image = Image::New(gTestImageFilename);

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

// 1.16
int UtcDaliImageGetImageSize(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("UtcDaliImageGetImageSize - Image::GetImageSize()");

  Vector2 testSize(8.0f, 16.0f);
  platform.SetClosestImageSize(testSize);

  Vector2 size = Image::GetImageSize(gTestImageFilename);

  DALI_TEST_CHECK( application.GetPlatform().GetTrace().FindMethod("GetClosestImageSize"));
  DALI_TEST_EQUALS( size, testSize, TEST_LOCATION);
  END_TEST;
}

// 1.17
int UtcDaliImageGetFilename(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetFilename");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::New(gTestImageFilename);

  DALI_TEST_EQUALS( image.GetFilename(), gTestImageFilename, TEST_LOCATION);
  END_TEST;
}

// 1.18
int UtcDaliImageGetLoadingState01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageGetLoadingState01");

  Image image = Image::New(gTestImageFilename);
  DALI_TEST_CHECK(image.GetLoadingState() == ResourceLoading);
  application.SendNotification();
  application.Render(16);

  // simulate load success
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

// 1.19
int UtcDaliImageGetLoadingState02(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetLoadingState02");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::New(gTestImageFilename);

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

// 1.20
int UtcDaliImageGetReleasePolicy(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetReleasePolicy");

  Image image = Image::New(gTestImageFilename, Image::Immediate, Image::Unused);

  DALI_TEST_CHECK( image );

  DALI_TEST_CHECK( Image::Unused == image.GetReleasePolicy() );

  END_TEST;
}

// 1.21
int UtcDaliImageGetLoadPolicy(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetLoadPolicy");

  Image image = Image::New(gTestImageFilename, Image::OnDemand, Image::Never);

  DALI_TEST_CHECK( image );

  DALI_TEST_CHECK( Image::OnDemand == image.GetLoadPolicy());
  END_TEST;
}

static bool SignalLoadFlag = false;

static void SignalLoadHandler(Image image)
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

// 1.22
int UtcDaliImageSignalLoadingFinished(void)
{
  TestApplication application;

  tet_infoline("UtcDaliImageSignalLoadingFinished");

  SignalLoadFlag = false;

  Image image = Image::New(gTestImageFilename);

  image.LoadingFinishedSignal().Connect( SignalLoadHandler );
  application.SendNotification();
  application.Render(16);

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true)));
  }

  application.Render(16);
  application.SendNotification();

  DALI_TEST_CHECK( SignalLoadFlag == true );
  END_TEST;
}

// 1.23
int UtcDaliImageSignalUploaded(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageSignalUploaded - Image::SignalUploaded()");

  // set up image in fake platform abstraction
  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  Image image = Image::New(gTestImageFilename);
  image.LoadingFinishedSignal().Connect( SignalLoadHandler );

  // Load image
  application.SendNotification();
  application.Render(16);
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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


// 1.24
int UtcDaliImageDiscard01(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageDiscard01 - no actors");

  {
    Image image = Image::New(gTestImageFilename);

    // Load image
    application.SendNotification();
    application.Render(16);
    std::vector<GLuint> ids;
    ids.push_back( 23 );
    application.GetGlAbstraction().SetNextTextureIds( ids );
    Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
    Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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

// 1.25
int UtcDaliImageDiscard02(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageDiscard02 - one actor, tests TextureCache::DiscardTexture");

  {
    {
      ImageActor actor;
      {
        Image image = Image::New(gTestImageFilename);
        actor = ImageActor::New(image);
        Stage::GetCurrent().Add(actor);

        application.SendNotification();
        application.Render(16);

        std::vector<GLuint> ids;
        ids.push_back( 23 );
        application.GetGlAbstraction().SetNextTextureIds( ids );

        Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
        Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
        Integration::ResourcePointer resource(bitmap);
        bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
        if(request)
        {
          application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resource);
        }
        application.Render(16);
        application.SendNotification();
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

// 1.26
int UtcDaliImageDiscard03(void)
{
  TestApplication application;
  tet_infoline("UtcDaliImageDiscard03 - one actor, tests TextureCache::RemoveObserver");

  Image image = Image::New(gTestImageFilename);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render(16);

  std::vector<GLuint> ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
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
