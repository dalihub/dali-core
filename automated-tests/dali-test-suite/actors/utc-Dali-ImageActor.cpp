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

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static const char* TestImageFilename = "icon_wrt.png";

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliImageActorConstructorVoid,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorDestructor,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorConstructorRefObject, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorNew01,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorNew02,                NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorDownCast,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorDownCast2,            NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActor9Patch,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorPixelArea,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorGetCurrentImageSize01,POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorGetCurrentImageSize02,POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorGetCurrentImageSize03,POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorGetCurrentImageSize04,POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorGetCurrentImageSize05,POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorNaturalPixelAreaSize01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorNaturalPixelAreaSize02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorDefaultProperties,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorUseImageAlpha01,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorUseImageAlpha02,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorUseImageAlpha03,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorUseImageAlpha04,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorUseImageAlpha05,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorClearPixelArea,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageGetStyle,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageSetNinePatchBorder,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageSetFadeIn,                 POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageSetFadeInDuration,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorNewNull,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorNewNullWithArea,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliImageActorSetImage,             POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliImageActorConstructorVoid()
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::ImageActor()");

  ImageActor actor;

  DALI_TEST_CHECK(!actor);
}

static void UtcDaliImageActorDestructor()
{
  TestApplication application;

  ImageActor* actor = new ImageActor();
  delete actor;

  DALI_TEST_CHECK( true );
}

static void UtcDaliImageActorConstructorRefObject()
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::ImageActor(Internal::ImageActor*)");

  ImageActor actor(NULL);

  DALI_TEST_CHECK(!actor);
}

static void UtcDaliImageActorNew01()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::New()");

  Image image = Image::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));

  DALI_TEST_CHECK(actor);
}

static void UtcDaliImageActorNew02()
{
  TestApplication application;
  tet_infoline("Negative test for Dali::ImageActor::New()");

  Image image = Image::New("hopefully-this-image-file-does-not-exist");
  ImageActor actor = ImageActor::New(image);

  DALI_TEST_CHECK(actor);
}

static void UtcDaliImageActorDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::DownCast()");

  Image image = Image::New("IncorrectImageName");
  ImageActor actor1 = ImageActor::New(image);
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  ImageActor imageActor = DownCast< ImageActor >(child);

  DALI_TEST_CHECK(imageActor);
}

static void UtcDaliImageActorDownCast2()
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::DownCast()");

  Actor actor1 = Actor::New();
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  ImageActor imageActor = ImageActor::DownCast(child);
  DALI_TEST_CHECK(!imageActor);

  Actor unInitialzedActor;
  imageActor = ImageActor::DownCast( unInitialzedActor );
  DALI_TEST_CHECK(!imageActor);
}

static void UtcDaliImageActor9Patch()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor:: 9 patch api");

  Image image = Image::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetStyle(ImageActor::STYLE_NINE_PATCH);
  Vector4 border(0.1,0.2,0.3,0.4);
  actor.SetNinePatchBorder(border);

  DALI_TEST_EQUALS( 0.1f, actor.GetNinePatchBorder().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 0.2f, actor.GetNinePatchBorder().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 0.3f, actor.GetNinePatchBorder().z, TEST_LOCATION );
  DALI_TEST_EQUALS( 0.4f, actor.GetNinePatchBorder().w, TEST_LOCATION );
}

static void UtcDaliImageActorPixelArea()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::UtcDaliImageActorPixelArea");

  BitmapImage img = BitmapImage::New( 10, 10 );
  ImageActor actor = ImageActor::New( img );

  DALI_TEST_CHECK( actor.IsPixelAreaSet() == false );

  ImageActor::PixelArea area( 1, 2, 3, 4 );
  actor.SetPixelArea( area );

  DALI_TEST_CHECK( actor.IsPixelAreaSet() == true );

  DALI_TEST_EQUALS( 1, actor.GetPixelArea().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 2, actor.GetPixelArea().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 3, actor.GetPixelArea().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 4, actor.GetPixelArea().height, TEST_LOCATION );

  ImageActor actor2 = ImageActor::New( img, ImageActor::PixelArea( 5, 6, 7, 8 ) );
  DALI_TEST_CHECK( actor2.IsPixelAreaSet() == true );

  DALI_TEST_EQUALS( 5, actor2.GetPixelArea().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 6, actor2.GetPixelArea().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 7, actor2.GetPixelArea().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 8, actor2.GetPixelArea().height, TEST_LOCATION );
}

static void UtcDaliImageActorGetCurrentImageSize01()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentImageSize");

  Vector2 initialImageSize(100, 50);
  BitmapImage image = BitmapImage::New( initialImageSize.width, initialImageSize.height );
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), initialImageSize, TEST_LOCATION );

  Vector2 size(200.0f, 200.0f);
  actor.SetSize(size);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), size, TEST_LOCATION );

  size.x = 200.0f;
  size.y = 200.0f;
  actor.SetSize(size);
  application.Render(8);

  // Test when a pixel area is set
  ImageActor::PixelArea area(0, 0, 10, 10);
  actor.SetPixelArea(area);
  application.Render(9);
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2( area.width, area.height ), TEST_LOCATION );
}


static void UtcDaliImageActorGetCurrentImageSize02()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentImageSize - Test that using an image resource sets the actor size with it's natural size immediately rather than on load");

  Vector2 initialImageSize(100, 50);

  application.GetPlatform().SetClosestImageSize(initialImageSize);

  Image image = Image::New("image.jpg");
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), initialImageSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  initialImageSize.width,initialImageSize.height, initialImageSize.width,initialImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), initialImageSize, TEST_LOCATION );

  Vector2 size(200.0f, 200.0f);
  actor.SetSize(size);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), size, TEST_LOCATION );

  actor.SetToNaturalSize();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), initialImageSize, TEST_LOCATION );
}


static void UtcDaliImageActorGetCurrentImageSize03()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentImageSize - Test that using an image resource with a requested size sets the actor size with it's nearest size immediately rather than on load");

  Vector2 closestImageSize( 80, 45);

  application.GetPlatform().SetClosestImageSize(closestImageSize);

  ImageAttributes attrs;
  attrs.SetSize(40, 30);
  Image image = Image::New("image.jpg", attrs);
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );

  // Test that setting a size on the actor can be 'undone' with SetNaturalSize()
  Vector2 size(200.0f, 200.0f);
  actor.SetSize(size);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), size, TEST_LOCATION );

  actor.SetToNaturalSize();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );
}


static void UtcDaliImageActorGetCurrentImageSize04()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentImageSize - check a new image doesn't change a set actor size");

  Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  ImageAttributes attrs;
  attrs.SetSize(40, 30); // Request a really small size we won't get.
  Image image = Image::New("image.jpg", attrs);
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );

  Vector2 size(200.0f, 200.0f);
  actor.SetSize(size);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), size, TEST_LOCATION );

  // Load a different image

  Vector2 image2ClosestSize = Vector2(240, 150); // The actual size image loader will return for the request below
  application.GetPlatform().SetClosestImageSize(image2ClosestSize);

  attrs.SetSize(100, 100);
  Image image2 = Image::New("image2.jpg", attrs);
  actor.SetImage(image2);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  // Ensure the actor size is kept
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), size, TEST_LOCATION );

  // Now complete the image load
  req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap2 = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap2->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  image2ClosestSize.width, image2ClosestSize.height, image2ClosestSize.width, image2ClosestSize.height );

  Integration::ResourcePointer resourcePtr2(bitmap2); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr2);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  // Ensure the actor size is kept
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), size, TEST_LOCATION );

  actor.SetToNaturalSize();
  application.SendNotification();
  application.Render();
  // Ensure the actor size gets the new image's natural size
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), image2ClosestSize, TEST_LOCATION );
}


static void UtcDaliImageActorGetCurrentImageSize05()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentImageSize - check a new image doens't change actor size until load complete");

  Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  ImageAttributes attrs;
  attrs.SetSize(40, 30); // Request a really small size we won't get.
  Image image = Image::New("image.jpg", attrs);
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );

  // Load a different image

  Vector2 image2ClosestSize = Vector2(240, 150);
  application.GetPlatform().SetClosestImageSize(image2ClosestSize);

  attrs.SetSize(100, 100);
  Image image2 = Image::New("image2.jpg", attrs);
  actor.SetImage(image2);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  // Ensure the actor size is kept
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );

  // Now complete the image load
  req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap2 = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap2->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  image2ClosestSize.width, image2ClosestSize.height, image2ClosestSize.width, image2ClosestSize.height );

  Integration::ResourcePointer resourcePtr2(bitmap2); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr2);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete

  // Ensure the actor size gets the new image's natural size
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), image2ClosestSize, TEST_LOCATION );
}

static void UtcDaliImageActorNaturalPixelAreaSize01()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentImageSize - check a new image doens't change actor size until load complete");

//If an image is loaded without setting size, then the actor gets the natural size of the image
//Setting the pixel area will change the actor size to match the pixel area
//Setting the actor size will not change pixel area, and will cause the partial image to stretch
//to the new size.
//Clearing the pixel area will not change actor size, and the actor will show the whole image.


  Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  ImageAttributes attrs;
  attrs.SetSize(40, 30); // Request a really small size we won't get.
  Image image = Image::New("image.jpg", attrs);
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), closestImageSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), closestImageSize, TEST_LOCATION );

  // Set a pixel area on a naturally sized actor - expect the actor to take the
  // pixel area as size
  actor.SetPixelArea(ImageActor::PixelArea(0, 0, 30, 30));
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2(30, 30), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(30, 30), TEST_LOCATION );

  // Set a size. Expect the partial image to stretch to fill the new size
  actor.SetSize(100, 100);
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2(30, 30), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );

  // Clear the pixel area. Expect the whole image to be shown, filling the set size.
  actor.ClearPixelArea();
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2(100, 100), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );
}

static void UtcDaliImageActorNaturalPixelAreaSize02()
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentImageSize - check a new image doens't change actor size until load complete");

//If an image is loaded without setting size, then the actor gets the natural size of the image
//Setting the pixel area will change the actor size to match the pixel area
//Setting the actor size will not change pixel area, and will cause the partial image to stretch
//to the new size.
//Clearing the pixel area will not change actor size, and the actor will show the whole image.


  Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  ImageAttributes attrs;
  attrs.SetSize(40, 30); // Request a really small size we won't get.
  Image image = Image::New("image.jpg", attrs);
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), closestImageSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), closestImageSize, TEST_LOCATION );

  // Set a pixel area on a naturally sized actor - expect the actor to take the
  // pixel area as size
  actor.SetPixelArea(ImageActor::PixelArea(0, 0, 30, 30));
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2(30, 30), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(30, 30), TEST_LOCATION );

  // Clear the pixel area. Expect the whole image to be shown, changing actor size
  actor.ClearPixelArea();
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), closestImageSize, TEST_LOCATION );

  // Set a size. Expect the partial image to stretch to fill the new size
  actor.SetSize(100, 100);
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2(100, 100), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );

  // Set a pixel area, don't expect size to change
  actor.SetPixelArea(ImageActor::PixelArea(0, 0, 40, 40));
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2(40, 40), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );

  // Use natural size - expect actor to change to pixel area
  actor.SetToNaturalSize();
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), Vector2(40, 40), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(40, 40), TEST_LOCATION );

  // Clearing pixel area should change actor size to image size
  actor.ClearPixelArea();
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( actor.GetCurrentImageSize(), closestImageSize, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), closestImageSize, TEST_LOCATION );
}



static void UtcDaliImageActorDefaultProperties()
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor DefaultProperties");

  BitmapImage img = BitmapImage::New( 10, 10 );
  ImageActor actor = ImageActor::New( img );

  std::vector<Property::Index> indices;
  indices.push_back(ImageActor::PIXEL_AREA      );
  indices.push_back(ImageActor::FADE_IN         );
  indices.push_back(ImageActor::FADE_IN_DURATION);
  indices.push_back(ImageActor::STYLE           );
  indices.push_back(ImageActor::BORDER          );
  indices.push_back(ImageActor::IMAGE           );

  DALI_TEST_CHECK(actor.GetPropertyCount() == ( Actor::New().GetPropertyCount() + indices.size() ) );

  for(std::vector<Property::Index>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
  {
    DALI_TEST_CHECK( *iter == actor.GetPropertyIndex(actor.GetPropertyName(*iter)) );
    DALI_TEST_CHECK( actor.IsPropertyWritable(*iter) );
    DALI_TEST_CHECK( !actor.IsPropertyAnimatable(*iter) );
    DALI_TEST_CHECK( actor.GetPropertyType(*iter) == actor.GetPropertyType(*iter) );  // just checking call succeeds
  }

  // set/get one of them
  actor.SetPixelArea(ImageActor::PixelArea( 0, 0, 0, 0 ));

  ImageActor::PixelArea area( 1, 2, 3, 4 );
  actor.SetProperty(ImageActor::PIXEL_AREA, Property::Value(Rect<int>(area)));

  DALI_TEST_CHECK(Property::RECTANGLE == actor.GetPropertyType(ImageActor::PIXEL_AREA));

  Property::Value v = actor.GetProperty(ImageActor::PIXEL_AREA);

  DALI_TEST_CHECK(v.Get<Rect<int> >() == area);

}

static void UtcDaliImageActorUseImageAlpha01()
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BitmapImage image = BitmapImage::New( 100, 50 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::ON );
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableCullFaceCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetCullFaceTrace();
  DALI_TEST_EQUALS( BlendEnabled( callTrace), true, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
}

static void UtcDaliImageActorUseImageAlpha02()
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BitmapImage image = BitmapImage::New( 100, 50 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::OFF );
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableCullFaceCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetCullFaceTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), false, TEST_LOCATION );
}

static void UtcDaliImageActorUseImageAlpha03()
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BitmapImage image = BitmapImage::New( 100, 50 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::AUTO );
  actor.SetColor(Vector4(1.0, 1.0, 1.0, 0.5));
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableCullFaceCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetCullFaceTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), true, TEST_LOCATION );
}

static void UtcDaliImageActorUseImageAlpha04()
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  FrameBufferImage image = FrameBufferImage::New( 100, 50, Pixel::RGBA8888 );
  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask task = taskList.GetTask( 0u );
  task.SetTargetFrameBuffer( image ); // To ensure frame buffer is connected
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE );
  application.SendNotification();
  application.Render(0);

  ImageActor actor = ImageActor::New( image );
  application.SendNotification();
  application.Render(0);

  actor.SetBlendMode( BlendingMode::ON );
  actor.SetColor(Vector4(1.0, 1.0, 1.0, 1.0));
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableCullFaceCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetCullFaceTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), true, TEST_LOCATION );
}

static void UtcDaliImageActorUseImageAlpha05()
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BitmapImage image = BitmapImage::New( 100, 50, Pixel::RGB888 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::AUTO );
  actor.SetColor(Vector4(1.0, 1.0, 1.0, 1.0));
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableCullFaceCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetCullFaceTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), false, TEST_LOCATION );
}

static void UtcDaliImageActorClearPixelArea()
{
  TestApplication application;

  BitmapImage img = BitmapImage::New( 10, 10 );
  ImageActor actor = ImageActor::New( img );

  DALI_TEST_CHECK( actor.IsPixelAreaSet() == false );

  ImageActor::PixelArea area( 1, 2, 3, 4 );
  actor.SetPixelArea( area );

  DALI_TEST_CHECK( actor.IsPixelAreaSet() == true );

  actor.ClearPixelArea();

  DALI_TEST_CHECK( actor.IsPixelAreaSet() == false );
}

static void UtcDaliImageGetStyle()
{
  TestApplication application;

  Image image = Image::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetStyle(ImageActor::STYLE_NINE_PATCH);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( ImageActor::STYLE_NINE_PATCH, actor.GetStyle(), TEST_LOCATION );
}

static void UtcDaliImageSetNinePatchBorder()
{
  TestApplication application;

  Image image = Image::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetStyle(ImageActor::STYLE_NINE_PATCH);
  actor.SetNinePatchBorder(Vector4( 1.0f, 2.0f, 3.0f, 4.0f));

  DALI_TEST_EQUALS( 1.0f, actor.GetNinePatchBorder().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 2.0f, actor.GetNinePatchBorder().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 3.0f, actor.GetNinePatchBorder().z, TEST_LOCATION );
  DALI_TEST_EQUALS( 4.0f, actor.GetNinePatchBorder().w, TEST_LOCATION );
}

static void UtcDaliImageSetFadeIn()
{
  TestApplication application;

  Image image = Image::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetFadeIn(true);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( true, actor.GetFadeIn(), TEST_LOCATION );

  actor.SetFadeIn(false);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( false, actor.GetFadeIn(), TEST_LOCATION );
}


static void UtcDaliImageSetFadeInDuration()
{
  TestApplication application;

  Image image = Image::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetFadeInDuration( 1.0f );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( 1.0f, actor.GetFadeInDuration(), TEST_LOCATION );

  actor.SetFadeInDuration( 3.0f );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( 3.0f, actor.GetFadeInDuration(), TEST_LOCATION );
}

static void UtcDaliImageActorNewNull()
{
  TestApplication application;

  ImageActor actor = ImageActor::New(Image());

  DALI_TEST_CHECK(actor);
}

static void UtcDaliImageActorNewNullWithArea()
{
  TestApplication application;

  ImageActor::PixelArea area( 1, 2, 3, 4 );

  ImageActor actor = ImageActor::New(Image(), area);

  DALI_TEST_CHECK(actor);
}

static void UtcDaliImageActorSetImage()
{
  TestApplication application;

  ImageActor actor = ImageActor::New(Image());

  DALI_TEST_CHECK(actor);

  actor.SetImage( Image() );

  DALI_TEST_CHECK(!actor.GetImage());
}
