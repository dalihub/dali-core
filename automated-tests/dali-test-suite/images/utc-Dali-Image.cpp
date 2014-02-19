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
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/image-attributes.h>
#include <dali-test-suite-utils.h>
#include <test-native-image.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static const char* gTestImageFilename = "icon_wrt.png";

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliImageNew01,                          POSITIVE_TC_IDX ); // 01
TEST_FUNCTION( UtcDaliImageNew02,                          POSITIVE_TC_IDX ); // 02
TEST_FUNCTION( UtcDaliImageNew03,                          POSITIVE_TC_IDX ); // 03
TEST_FUNCTION( UtcDaliImageNewWithPolicies01,              POSITIVE_TC_IDX ); // 04
TEST_FUNCTION( UtcDaliImageNewWithPolicies02,              POSITIVE_TC_IDX ); // 05
TEST_FUNCTION( UtcDaliImageNewWithPolicies03,              POSITIVE_TC_IDX ); // 06
TEST_FUNCTION( UtcDaliImageNewWithPolicies04,              POSITIVE_TC_IDX ); // 07
TEST_FUNCTION( UtcDaliImageNewDistanceField,               POSITIVE_TC_IDX ); // 08
TEST_FUNCTION( UtcDaliImageNewDistanceFieldWithPolicies01, POSITIVE_TC_IDX ); // 09
TEST_FUNCTION( UtcDaliImageNewDistanceFieldWithPolicies02, POSITIVE_TC_IDX ); // 10
TEST_FUNCTION( UtcDaliImageNewDistanceFieldWithPolicies03, POSITIVE_TC_IDX ); // 11
TEST_FUNCTION( UtcDaliImageNewDistanceFieldWithPolicies04, POSITIVE_TC_IDX ); // 12
TEST_FUNCTION( UtcDaliImageNewDistanceFieldWithAttributes, POSITIVE_TC_IDX ); // 13
TEST_FUNCTION( UtcDaliImageNewDistanceFieldWithAttrandPol, POSITIVE_TC_IDX ); // 14
TEST_FUNCTION( UtcDaliImageDownCast,                       POSITIVE_TC_IDX ); // 15
TEST_FUNCTION( UtcDaliImageGetImageSize,                   POSITIVE_TC_IDX ); // 16
TEST_FUNCTION( UtcDaliImageGetFilename,                    POSITIVE_TC_IDX ); // 17
TEST_FUNCTION( UtcDaliImageGetLoadingState01,              POSITIVE_TC_IDX ); // 18
TEST_FUNCTION( UtcDaliImageGetLoadingState02,              POSITIVE_TC_IDX ); // 19
TEST_FUNCTION( UtcDaliImageGetReleasePolicy,               POSITIVE_TC_IDX ); // 20
TEST_FUNCTION( UtcDaliImageGetLoadPolicy,                  POSITIVE_TC_IDX ); // 21
TEST_FUNCTION( UtcDaliImageSignalLoadingFinished,          POSITIVE_TC_IDX ); // 22
TEST_FUNCTION( UtcDaliImageSignalUploaded,                 POSITIVE_TC_IDX ); // 23
TEST_FUNCTION( UtcDaliImageDiscard01,                      POSITIVE_TC_IDX ); // 24
TEST_FUNCTION( UtcDaliImageDiscard02,                      POSITIVE_TC_IDX ); // 25
TEST_FUNCTION( UtcDaliImageDiscard03,                      POSITIVE_TC_IDX ); // 26


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// 1.1
static void UtcDaliImageNew01()
{
  TestApplication application;

  tet_infoline("UtcDaliImageNew01 - Image::New(const std::string&)");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::New(gTestImageFilename);

  DALI_TEST_CHECK( image );
}

// 1.2
static void UtcDaliImageNew02()
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
}

// 1.3
static void UtcDaliImageNew03()
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
}

// 1.4
static void UtcDaliImageNewWithPolicies01()
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
}

// 1.5
static void UtcDaliImageNewWithPolicies02()
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
}

// 1.6
static void UtcDaliImageNewWithPolicies03()
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
}

// 1.7
static void UtcDaliImageNewWithPolicies04()
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
}

// 1.8
static void UtcDaliImageNewDistanceField()
{
  TestApplication application;

  tet_infoline("UtcDaliImageNewDistanceField - Image::NewDistanceField(const std::string&)");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::NewDistanceField(gTestImageFilename);

  DALI_TEST_CHECK( image );
}

// 1.9
static void UtcDaliImageNewDistanceFieldWithPolicies01()
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
}

// 1.10
static void UtcDaliImageNewDistanceFieldWithPolicies02()
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
}

// 1.11
static void UtcDaliImageNewDistanceFieldWithPolicies03()
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
}

// 1.12
static void UtcDaliImageNewDistanceFieldWithPolicies04()
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
}

// 1.13
static void UtcDaliImageNewDistanceFieldWithAttributes()
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
}

// 1.14
static void UtcDaliImageNewDistanceFieldWithAttrandPol()
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
}

// 1.15
static void UtcDaliImageDownCast()
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
}

// 1.16
static void UtcDaliImageGetImageSize()
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();

  tet_infoline("UtcDaliImageGetImageSize - Image::GetImageSize()");

  Vector2 testSize(8.0f, 16.0f);
  platform.SetImageMetaDataSize(testSize);

  Vector2 size = Image::GetImageSize(gTestImageFilename);

  DALI_TEST_CHECK( application.GetPlatform().GetTrace().FindMethod("GetClosestImageSize"));
  DALI_TEST_EQUALS( size, testSize, TEST_LOCATION);
}

// 1.17
static void UtcDaliImageGetFilename()
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetFilename");

  // invoke default handle constructor
  Image image;

  DALI_TEST_CHECK( !image );

  // initialise handle
  image = Image::New(gTestImageFilename);

  DALI_TEST_EQUALS( image.GetFilename(), gTestImageFilename, TEST_LOCATION);
}

// 1.18
static void UtcDaliImageGetLoadingState01()
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
}

// 1.19
static void UtcDaliImageGetLoadingState02()
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
}

// 1.20
static void UtcDaliImageGetReleasePolicy()
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetReleasePolicy");

  Image image = Image::New(gTestImageFilename, Image::Immediate, Image::Unused);

  DALI_TEST_CHECK( image );

  DALI_TEST_CHECK( Image::Unused == image.GetReleasePolicy() );

}

// 1.21
static void UtcDaliImageGetLoadPolicy()
{
  TestApplication application;

  tet_infoline("UtcDaliImageGetLoadPolicy");

  Image image = Image::New(gTestImageFilename, Image::OnDemand, Image::Never);

  DALI_TEST_CHECK( image );

  DALI_TEST_CHECK( Image::OnDemand == image.GetLoadPolicy());
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
static void UtcDaliImageSignalLoadingFinished()
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
}

// 1.23
static void UtcDaliImageSignalUploaded()
{
  TestApplication application;
  tet_infoline("UtcDaliImageSignalUploaded - Image::SignalUploaded()");

  // set up image in fake platform abstraction
  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetImageMetaDataSize(testSize);

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
}


// 1.24
static void UtcDaliImageDiscard01()
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
}

// 1.25
static void UtcDaliImageDiscard02()
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
}

// 1.26
static void UtcDaliImageDiscard03()
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

  // Test what?!
}
