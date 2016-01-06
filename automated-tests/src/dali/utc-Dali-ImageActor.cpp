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
#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/bitmap.h>
#include "dali-test-suite-utils/dali-test-suite-utils.h"

using namespace Dali;

static const char* TestImageFilename = "icon_wrt.png";

void image_actor_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void image_actor_test_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliImageActorConstructorVoid(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::ImageActor()");

  ImageActor actor;

  DALI_TEST_CHECK(!actor);
  END_TEST;
}

int UtcDaliImageActorDestructor(void)
{
  TestApplication application;

  ImageActor* actor = new ImageActor();
  delete actor;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliImageActorNew01(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::New()");

  Image image = ResourceImage::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();
  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliImageActorNew02(void)
{
  TestApplication application;
  tet_infoline("Negative test for Dali::ImageActor::New()");

  Image image = ResourceImage::New("hopefullyThisImageFileDoesNotExist");
  ImageActor actor = ImageActor::New(image);

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliImageActorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::DownCast()");

  Image image = ResourceImage::New("IncorrectImageName");
  ImageActor actor1 = ImageActor::New(image);
  Actor anActor = Actor::New();
  anActor.Add(actor1);

  Actor child = anActor.GetChildAt(0);
  ImageActor imageActor = DownCast< ImageActor >(child);

  DALI_TEST_CHECK(imageActor);
  END_TEST;
}

int UtcDaliImageActorDownCast2(void)
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
  END_TEST;
}

int UtcDaliImageActorCopyConstructor(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor::ImageActor(const ImageActor& )");

  Image image = ResourceImage::New("IncorrectImageName");
  ImageActor actor1 = ImageActor::New(image);

  ImageActor actor2(actor1);
  DALI_TEST_CHECK(actor2);
  DALI_TEST_EQUALS( actor2, actor1, TEST_LOCATION );
  DALI_TEST_EQUALS( actor2.GetImage(), image, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageActor9Patch(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor:: 9 patch api");

  Image image = ResourceImage::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetStyle(ImageActor::STYLE_NINE_PATCH);
  DALI_TEST_EQUALS( ImageActor::STYLE_NINE_PATCH, actor.GetStyle(), TEST_LOCATION );

  Vector4 border(0.1,0.2,0.3,0.4);
  actor.SetNinePatchBorder(border);

  DALI_TEST_EQUALS( 0.1f, actor.GetNinePatchBorder().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 0.2f, actor.GetNinePatchBorder().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 0.3f, actor.GetNinePatchBorder().z, TEST_LOCATION );
  DALI_TEST_EQUALS( 0.4f, actor.GetNinePatchBorder().w, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorPixelArea(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::UtcDaliImageActorPixelArea");

  BufferImage img = BufferImage::New( 10, 10 );
  ImageActor actor = ImageActor::New( img );

  ImageActor::PixelArea area( 1, 2, 3, 4 );
  actor.SetPixelArea( area );

  DALI_TEST_EQUALS( 1, actor.GetPixelArea().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 2, actor.GetPixelArea().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 3, actor.GetPixelArea().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 4, actor.GetPixelArea().height, TEST_LOCATION );

  ImageActor actor2 = ImageActor::New( img, ImageActor::PixelArea( 5, 6, 7, 8 ) );

  DALI_TEST_EQUALS( 5, actor2.GetPixelArea().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 6, actor2.GetPixelArea().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 7, actor2.GetPixelArea().width, TEST_LOCATION );
  DALI_TEST_EQUALS( 8, actor2.GetPixelArea().height, TEST_LOCATION );
  END_TEST;
}

// Set a size that is too large on an Image with a shader that requires grid
int UtcDaliImageActorSetSize01(void)
{
  TestApplication application;

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );

  ShaderEffect effect = ShaderEffect::New( " ", " ", ShaderEffect::HINT_GRID );
  actor.SetShaderEffect( effect );

  const float INVALID_SIZE = float(1u<<31);
  Vector3 vector( INVALID_SIZE, INVALID_SIZE, 0.0f );

  DALI_TEST_CHECK(vector != actor.GetCurrentSize());

  actor.SetSize(vector);
  Stage::GetCurrent().Add(actor);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(vector, actor.GetCurrentSize(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorGetCurrentSize01(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentSize");

  Vector2 initialImageSize(100, 50);
  BufferImage image = BufferImage::New( initialImageSize.width, initialImageSize.height );
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), initialImageSize, TEST_LOCATION );

  Vector2 size(200.0f, 200.0f);
  actor.SetSize(size);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), size, TEST_LOCATION );

  size.x = 200.0f;
  size.y = 200.0f;
  actor.SetSize(size);
  application.Render(8);

  // Test when a pixel area is set
  ImageActor::PixelArea area(0, 0, 10, 10);
  actor.SetPixelArea(area);
  application.Render(9);
  // natural size is not used as setsize is called
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), size, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageActorGetCurrentSize02(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentSize - Test that using an image resource sets the actor size with it's natural size immediately rather than on load");

  Vector2 initialImageSize(100, 50);

  application.GetPlatform().SetClosestImageSize(initialImageSize);

  Image image = ResourceImage::New("image.jpg");
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), initialImageSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  initialImageSize.width,initialImageSize.height, initialImageSize.width,initialImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), initialImageSize, TEST_LOCATION );

  Vector2 size(200.0f, 200.0f);
  actor.SetSize(size);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), size, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageActorGetCurrentSize03(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentSize - Test that using an image resource with a requested size sets the actor size with it's nearest size immediately rather than on load");

  const Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  Vector2 requestedSize( 40, 30 );
  Image image = ResourceImage::New("image.jpg", ImageDimensions( requestedSize.x, requestedSize.y ), FittingMode::DEFAULT, SamplingMode::DEFAULT );
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageActorGetCurrentSize04(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentSize - check a new image doesn't change a set actor size");

  const Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  Vector2 requestedSize( 40, 30 );
  Image image = ResourceImage::New("image.jpg", ImageDimensions( requestedSize.x, requestedSize.y ), FittingMode::DEFAULT, SamplingMode::DEFAULT );
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  Vector2 size(200.0f, 200.0f);
  actor.SetSize(size);

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), size, TEST_LOCATION );

  // Load a different image

  Vector2 image2ClosestSize = Vector2(240, 150); // The actual size image loader will return for the request below
  application.GetPlatform().SetClosestImageSize(image2ClosestSize);

  const Vector2 request2Size( 100, 100 );
  Image image2 = ResourceImage::New("image.jpg", ImageDimensions( request2Size.x, request2Size.y ), FittingMode::DEFAULT, SamplingMode::DEFAULT );
  actor.SetImage(image2);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  // Ensure the actor size is kept
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), size, TEST_LOCATION );

  // Now complete the image load
  req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap2 = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  bitmap2->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  image2ClosestSize.width, image2ClosestSize.height, image2ClosestSize.width, image2ClosestSize.height );

  Integration::ResourcePointer resourcePtr2(bitmap2); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr2);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  // Ensure the actor size is kept
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), size, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageActorGetCurrentSize05(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentSize - check a new image doens't change actor size until load complete");

  Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  Vector2 requestedSize( 40, 30 );
  Image image = ResourceImage::New("image.jpg", ImageDimensions( requestedSize.x, requestedSize.y ), FittingMode::DEFAULT, SamplingMode::DEFAULT );
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Load a different image

  Vector2 image2ClosestSize = Vector2(240, 150);
  application.GetPlatform().SetClosestImageSize(image2ClosestSize);

  const Vector2 requestedSize2( 100, 100 );
  Image image2 = ResourceImage::New("image.jpg", ImageDimensions( requestedSize2.x, requestedSize2.y ), FittingMode::DEFAULT, SamplingMode::DEFAULT );
  actor.SetImage(image2);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  // Ensure the actor size is kept
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize2, TEST_LOCATION );

  // Now complete the image load
  req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap2 = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
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
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize2, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorNaturalPixelAreaSize01(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentSize - check a new image doens't change actor size until load complete");

//If an image is loaded without setting size, then the actor gets the natural size of the image
//Setting the pixel area will change the actor size to match the pixel area
//Setting the actor size will not change pixel area, and will cause the partial image to stretch
//to the new size.
//Clearing the pixel area will not change actor size, and the actor will show the whole image.


  Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  Vector2 requestedSize( 40, 30 );
  Image image = ResourceImage::New("image.jpg", ImageDimensions( requestedSize.x, requestedSize.y ), FittingMode::DEFAULT, SamplingMode::DEFAULT );
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Set a pixel area on a naturally sized actor - expect the actor to take the
  // pixel area as size
  actor.SetPixelArea(ImageActor::PixelArea(0, 0, 30, 30));
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(30, 30), TEST_LOCATION );

  // Set a size. Expect the partial image to stretch to fill the new size
  actor.SetSize(100, 100);
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );

  // Clear the pixel area. Expect the whole image to be shown, filling the set size.
  actor.SetPixelArea( ImageActor::PixelArea( 0, 0, image.GetWidth(), image.GetHeight()) );
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorNaturalPixelAreaSize02(void)
{
  TestApplication application;
  tet_infoline("Positive test for Dali::ImageActor::GetCurrentSize - check a new image doens't change actor size until load complete");

//If an image is loaded without setting size, then the actor gets the natural size of the image
//Setting the pixel area will change the actor size to match the pixel area
//Setting the actor size will not change pixel area, and will cause the partial image to stretch
//to the new size.
//Clearing the pixel area will not change actor size, and the actor will show the whole image.


  Vector2 closestImageSize( 80, 45);
  application.GetPlatform().SetClosestImageSize(closestImageSize);

  Vector2 requestedSize( 40, 30 );
  Image image = ResourceImage::New("image.jpg", ImageDimensions( requestedSize.x, requestedSize.y ), FittingMode::DEFAULT, SamplingMode::DEFAULT );
  ImageActor actor = ImageActor::New( image );
  Stage::GetCurrent().Add(actor);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Now complete the image load
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  closestImageSize.width, closestImageSize.height, closestImageSize.width, closestImageSize.height );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  application.GetPlatform().ClearReadyResources(); //

  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Set a pixel area on a naturally sized actor - expect the actor to take the
  // pixel area as size
  actor.SetPixelArea(ImageActor::PixelArea(0, 0, 30, 30));
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(30, 30), TEST_LOCATION );

  // Clear the pixel area. Expect the whole image to be shown, changing actor size
  actor.SetPixelArea( ImageActor::PixelArea( 0, 0, image.GetWidth(), image.GetHeight()) );
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );

  // Set a size. Expect the partial image to stretch to fill the new size
  actor.SetSize(100, 100);
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );

  // Set a pixel area, don't expect size to change
  actor.SetPixelArea(ImageActor::PixelArea(0, 0, 40, 40));
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), Vector2(100, 100), TEST_LOCATION );

  // Clearing pixel area should change actor size to image size
  actor.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
  actor.SetPixelArea( ImageActor::PixelArea( 0, 0, image.GetWidth(), image.GetHeight()) );
  application.SendNotification(); // Process event messages
  application.Render();           // Process LoadComplete
  DALI_TEST_EQUALS( Vector2(actor.GetCurrentSize()), requestedSize, TEST_LOCATION );
  END_TEST;
}



int UtcDaliImageActorDefaultProperties(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::ImageActor DefaultProperties");

  BufferImage img = BufferImage::New( 10, 10 );
  ImageActor actor = ImageActor::New( img );

  std::vector<Property::Index> indices;
  indices.push_back(ImageActor::Property::PIXEL_AREA      );
  indices.push_back(ImageActor::Property::STYLE           );
  indices.push_back(ImageActor::Property::BORDER          );
  indices.push_back(ImageActor::Property::IMAGE           );

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
  actor.SetProperty(ImageActor::Property::PIXEL_AREA, Property::Value(Rect<int>(area)));

  DALI_TEST_CHECK(Property::RECTANGLE == actor.GetPropertyType(ImageActor::Property::PIXEL_AREA));

  Property::Value v = actor.GetProperty(ImageActor::Property::PIXEL_AREA);

  DALI_TEST_CHECK(v.Get<Rect<int> >() == area);

  END_TEST;
}

int UtcDaliImageActorUseImageAlpha01(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BufferImage image = BufferImage::New( 100, 50 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::ON );
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableEnableDisableCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetEnableDisableTrace();
  DALI_TEST_EQUALS( BlendEnabled( callTrace), true, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorUseImageAlpha02(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BufferImage image = BufferImage::New( 100, 50 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::OFF );
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableEnableDisableCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetEnableDisableTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorUseImageAlpha03(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BufferImage image = BufferImage::New( 100, 50 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::AUTO );
  actor.SetColor(Vector4(1.0, 1.0, 1.0, 0.5));
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableEnableDisableCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetEnableDisableTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorUseImageAlpha04(void)
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
  application.GetGlAbstraction().EnableEnableDisableCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetEnableDisableTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorUseImageAlpha05(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::RenderableActor::SetUseImageAlpha()");

  BufferImage image = BufferImage::New( 100, 50, Pixel::RGB888 );
  ImageActor actor = ImageActor::New( image );
  actor.SetBlendMode( BlendingMode::AUTO );
  actor.SetColor(Vector4(1.0, 1.0, 1.0, 1.0));
  actor.SetSize(100, 50);
  application.GetGlAbstraction().EnableEnableDisableCallTrace(true); // For Enable(GL_BLEND)
  Stage::GetCurrent().Add(actor);

  application.SendNotification();
  application.Render();

  const TraceCallStack& callTrace = application.GetGlAbstraction().GetEnableDisableTrace();
  DALI_TEST_EQUALS( BlendDisabled( callTrace ), false, TEST_LOCATION );
  DALI_TEST_EQUALS( BlendEnabled( callTrace), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorGetStyle(void)
{
  TestApplication application;

  Image image = ResourceImage::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetStyle(ImageActor::STYLE_NINE_PATCH);

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( ImageActor::STYLE_NINE_PATCH, actor.GetStyle(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorSetNinePatchBorder(void)
{
  TestApplication application;

  Image image = ResourceImage::New(TestImageFilename);
  ImageActor actor = ImageActor::New(image);

  actor.SetStyle(ImageActor::STYLE_NINE_PATCH);
  DALI_TEST_EQUALS( ImageActor::STYLE_NINE_PATCH, actor.GetStyle(), TEST_LOCATION );

  actor.SetNinePatchBorder(Vector4( 1.0f, 2.0f, 3.0f, 4.0f));

  DALI_TEST_EQUALS( 1.0f, actor.GetNinePatchBorder().x, TEST_LOCATION );
  DALI_TEST_EQUALS( 2.0f, actor.GetNinePatchBorder().y, TEST_LOCATION );
  DALI_TEST_EQUALS( 3.0f, actor.GetNinePatchBorder().z, TEST_LOCATION );
  DALI_TEST_EQUALS( 4.0f, actor.GetNinePatchBorder().w, TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorNewNull(void)
{
  TestApplication application;

  ImageActor actor = ImageActor::New(Image());

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliImageActorNewNullWithArea(void)
{
  TestApplication application;

  ImageActor::PixelArea area( 1, 2, 3, 4 );

  ImageActor actor = ImageActor::New(Image(), area);

  DALI_TEST_CHECK(actor);
  END_TEST;
}

int UtcDaliImageActorSetImage(void)
{
  TestApplication application;

  ImageActor actor = ImageActor::New(Image());

  DALI_TEST_CHECK(actor);

  actor.SetImage( Image() );

  DALI_TEST_CHECK(!actor.GetImage());
  END_TEST;
}

int UtcDaliImageActorPropertyIndices(void)
{
  TestApplication application;
  Actor basicActor = Actor::New();
  ImageActor imageActor = ImageActor::New();

  Property::IndexContainer indices;
  imageActor.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.Size() > basicActor.GetPropertyCount() );
  DALI_TEST_EQUALS( indices.Size(), imageActor.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorImageProperty(void)
{
  TestApplication application;
  Image image = ResourceImage::New( "MY_PATH" );
  ImageActor imageActor = ImageActor::New( image );

  Stage::GetCurrent().Add( imageActor );
  application.SendNotification();
  application.Render();

  Property::Value imageProperty = imageActor.GetProperty( ImageActor::Property::IMAGE );
  Property::Map* imageMap = imageProperty.GetMap();
  DALI_TEST_CHECK( imageMap != NULL );
  DALI_TEST_CHECK( NULL != imageMap->Find( "filename" ) );
  DALI_TEST_EQUALS( (*imageMap)[ "filename" ].Get< std::string >(), "MY_PATH", TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorNinePatch01(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  tet_infoline("Test the successful loading of a nine-patch image\n");

  platform.SetClosestImageSize(Vector2(4, 4));
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  Integration::PixelBuffer* pixels = bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  4,4,4,4 );
  memset( pixels, 0, 64 );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );

  Image ninePatchImage = ResourceImage::New( "blah.#.png" );
  DALI_TEST_CHECK( ninePatchImage );

  ImageActor imageActor = ImageActor::New( ninePatchImage );
  DALI_TEST_CHECK( imageActor );
  Stage::GetCurrent().Add( imageActor );

  drawTrace.Reset();
  textureTrace.Reset();
  drawTrace.Enable(true);
  textureTrace.Enable(true);
  glAbstraction.ClearBoundTextures();
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  glAbstraction.SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );
  typedef std::vector<GLuint> TexVec;
  const TexVec& textures = glAbstraction.GetBoundTextures(GL_TEXTURE0);
  DALI_TEST_CHECK( textures.size() > 0 );
  if( textures.size() > 0 )
  {
    DALI_TEST_EQUALS( textures[0], 23u, TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliImageActorNinePatch02(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  tet_infoline("Test the failed loading of a nine-patch image\n");

  platform.SetClosestImageSize(Vector2(0, 0));
  Integration::ResourcePointer resourcePtr;
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );

  Image ninePatchImage = ResourceImage::New( "blah.#.png" );
  DALI_TEST_CHECK( ninePatchImage );

  ImageActor imageActor = ImageActor::New( ninePatchImage );
  DALI_TEST_CHECK( imageActor );
  Stage::GetCurrent().Add( imageActor );

  drawTrace.Reset();
  textureTrace.Reset();
  drawTrace.Enable(true);
  textureTrace.Enable(true);
  glAbstraction.ClearBoundTextures();
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  glAbstraction.SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  // Check that nothing was drawn.
  DALI_TEST_CHECK( ! drawTrace.FindMethod( "DrawElements" ) );
  typedef std::vector<GLuint> TexVec;
  const TexVec& textures = glAbstraction.GetBoundTextures(GL_TEXTURE0);
  DALI_TEST_CHECK( textures.size() == 0u );

  END_TEST;
}


int UtcDaliImageActorNinePatch03(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  tet_infoline("Test the successful loading of a nine-patch image added using ImageActor::SetImage()\n");

  platform.SetClosestImageSize(Vector2(4, 4));
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  Integration::PixelBuffer* pixels = bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888,  4,4,4,4 );
  memset( pixels, 0, 64 );

  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );

  Image ninePatchImage = ResourceImage::New( "blah.#.png" );
  DALI_TEST_CHECK( ninePatchImage );

  ImageActor imageActor = ImageActor::New();
  DALI_TEST_CHECK( imageActor );
  Stage::GetCurrent().Add( imageActor );

  imageActor.SetImage( ninePatchImage );

  drawTrace.Reset();
  textureTrace.Reset();
  drawTrace.Enable(true);
  textureTrace.Enable(true);
  glAbstraction.ClearBoundTextures();
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  glAbstraction.SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( drawTrace.FindMethod( "DrawElements" ) );
  typedef std::vector<GLuint> TexVec;
  const TexVec& textures = glAbstraction.GetBoundTextures(GL_TEXTURE0);
  DALI_TEST_CHECK( textures.size() > 0 );
  if( textures.size() > 0 )
  {
    DALI_TEST_EQUALS( textures[0], 23u, TEST_LOCATION );
  }

  END_TEST;
}


int UtcDaliImageActorNinePatch04(void)
{
  TestApplication application;
  TestPlatformAbstraction& platform = application.GetPlatform();
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack& textureTrace = glAbstraction.GetTextureTrace();
  TraceCallStack& drawTrace = glAbstraction.GetDrawTrace();

  tet_infoline("Test the failed loading of a nine-patch image using ImageActor::SetImage()\n");

  platform.SetClosestImageSize(Vector2(0, 0));
  Integration::ResourcePointer resourcePtr;
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );

  Image ninePatchImage = ResourceImage::New( "blah.#.png" );
  DALI_TEST_CHECK( ninePatchImage );

  ImageActor imageActor = ImageActor::New();
  DALI_TEST_CHECK( imageActor );
  Stage::GetCurrent().Add( imageActor );

  imageActor.SetImage( ninePatchImage );

  drawTrace.Reset();
  textureTrace.Reset();
  drawTrace.Enable(true);
  textureTrace.Enable(true);
  glAbstraction.ClearBoundTextures();
  std::vector<GLuint> ids;
  ids.push_back( 23 );
  glAbstraction.SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  // Check that nothing was drawn.
  DALI_TEST_CHECK( ! drawTrace.FindMethod( "DrawElements" ) );
  typedef std::vector<GLuint> TexVec;
  const TexVec& textures = glAbstraction.GetBoundTextures(GL_TEXTURE0);
  DALI_TEST_CHECK( textures.size() == 0u );

  END_TEST;
}

int UtcDaliImageActorGetNaturalSize(void)
{
  TestApplication application;

  // Standard image
  BufferImage img = BufferImage::New( 10, 10 );
  ImageActor actor = ImageActor::New( img );

  DALI_TEST_CHECK( actor.GetNaturalSize().GetVectorXY() == Vector2( 10, 10 ) );

  // Pixel area set
  ImageActor::PixelArea area( 1, 2, 3, 4 );
  actor.SetPixelArea( area );

  DALI_TEST_CHECK( actor.GetNaturalSize().GetVectorXY() == Vector2( 3, 4 ) );

  END_TEST;
}

int UtcDaliImageActorGetSortModifier(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::ImageActor::GetSortModifier()");

  ImageActor actor = ImageActor::New();
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS(actor.GetSortModifier(), 0.0f, TEST_LOCATION);

  Stage::GetCurrent().Remove(actor);
  END_TEST;
}

int UtcDaliImageActorSetGetBlendMode(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::ImageActor::SetBlendMode() / Dali::ImageActor::GetBlendMode()");

  ImageActor actor = ImageActor::New();

  actor.SetBlendMode( BlendingMode::OFF );
  DALI_TEST_CHECK( BlendingMode::OFF == actor.GetBlendMode() );

  actor.SetBlendMode( BlendingMode::AUTO );
  DALI_TEST_CHECK( BlendingMode::AUTO == actor.GetBlendMode() );

  actor.SetBlendMode( BlendingMode::ON );
  DALI_TEST_CHECK( BlendingMode::ON == actor.GetBlendMode() );
  END_TEST;
}

int UtcDaliImageActorSetGetBlendFunc(void)
{
  TestApplication application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  tet_infoline("Testing Dali::ImageActor::UtcDaliImageActorSetGetBlendFunc()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );
  Stage::GetCurrent().Add( actor );
  application.SendNotification();
  application.Render();

  // Test the defaults as documented int blending.h
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    actor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA,           srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_ALPHA, destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE,                 srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_ALPHA, destFactorAlpha, TEST_LOCATION );
  }

  // Set to non-default values
  actor.SetBlendFunc( BlendingFactor::ONE_MINUS_SRC_COLOR, BlendingFactor::SRC_ALPHA_SATURATE, BlendingFactor::ONE_MINUS_SRC_COLOR, BlendingFactor::SRC_ALPHA_SATURATE );

  // Test that Set was successful
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    actor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_COLOR, srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA_SATURATE,  destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_SRC_COLOR, srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::SRC_ALPHA_SATURATE,  destFactorAlpha, TEST_LOCATION );
  }

  // Render & check GL commands
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_COLOR, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA_SATURATE,  glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  // Set using separate alpha settings
  actor.SetBlendFunc( BlendingFactor::CONSTANT_COLOR, BlendingFactor::ONE_MINUS_CONSTANT_COLOR,
                      BlendingFactor::CONSTANT_ALPHA, BlendingFactor::ONE_MINUS_CONSTANT_ALPHA );

  // Test that Set was successful
  {
    BlendingFactor::Type srcFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorRgb( BlendingFactor::ZERO );
    BlendingFactor::Type srcFactorAlpha( BlendingFactor::ZERO );
    BlendingFactor::Type destFactorAlpha( BlendingFactor::ZERO );
    actor.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
    DALI_TEST_EQUALS( BlendingFactor::CONSTANT_COLOR,            srcFactorRgb,    TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_CONSTANT_COLOR,  destFactorRgb,   TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::CONSTANT_ALPHA,            srcFactorAlpha,  TEST_LOCATION );
    DALI_TEST_EQUALS( BlendingFactor::ONE_MINUS_CONSTANT_ALPHA,  destFactorAlpha, TEST_LOCATION );
  }

  // Render & check GL commands
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( (GLenum)GL_CONSTANT_COLOR,           glAbstraction.GetLastBlendFuncSrcRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_CONSTANT_COLOR, glAbstraction.GetLastBlendFuncDstRgb(),   TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_CONSTANT_ALPHA,           glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_CONSTANT_ALPHA, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorSetGetAlpha(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::ImageActor::SetGetAlpha()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );
  Stage::GetCurrent().Add( actor );
  application.SendNotification();
  application.Render();

  // use the image alpha on actor
  actor.SetBlendMode(BlendingMode::ON);

  // Test that Set was successful
  DALI_TEST_EQUALS( BlendingMode::ON, actor.GetBlendMode(), TEST_LOCATION );

  // Now test that it can be set to false
  actor.SetBlendMode(BlendingMode::OFF);
  DALI_TEST_EQUALS(BlendingMode::OFF, actor.GetBlendMode(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliImageActorSetGetFilterModes(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::ImageActor::SetFilterMode() / Dali::ImageActor::GetFilterMode()");

  ImageActor actor = ImageActor::New();

  FilterMode::Type minifyFilter = FilterMode::NEAREST;
  FilterMode::Type magnifyFilter = FilterMode::NEAREST;

  // Default test
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == minifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == magnifyFilter );

  // Default/Default
  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == minifyFilter );
  DALI_TEST_CHECK( FilterMode::DEFAULT == magnifyFilter );

  // Nearest/Nearest
  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == minifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == magnifyFilter );

  // Linear/Linear
  actor.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == minifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == magnifyFilter );

  // Nearest/Linear
  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::LINEAR );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == minifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == magnifyFilter );

  // Linear/Nearest
  actor.SetFilterMode( FilterMode::LINEAR, FilterMode::NEAREST );
  actor.GetFilterMode( minifyFilter, magnifyFilter );
  DALI_TEST_CHECK( FilterMode::LINEAR == minifyFilter );
  DALI_TEST_CHECK( FilterMode::NEAREST == magnifyFilter );

  END_TEST;
}

int UtcDaliImageActorSetFilterMode(void)
{
  TestApplication application;

  tet_infoline("Testing Dali::ImageActor::SetFilterMode()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );

  actor.SetSize(100.0f, 100.0f);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  Stage::GetCurrent().Add(actor);

  /**************************************************************/

  // Default/Default
  TraceCallStack& texParameterTrace = application.GetGlAbstraction().GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  std::stringstream out;

  // Verify actor gl state

  // There are two calls to TexParameteri when the texture is first created
  // Texture mag filter is not called as the first time set it uses the system default
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 3, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(2, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // Default/Default
  texParameterTrace = application.GetGlAbstraction().GetTexParameterTrace();
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state

  // Should not make any calls when settings are the same
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 0, TEST_LOCATION);

  /**************************************************************/

  // Nearest/Nearest
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::NEAREST );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 2, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(1, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // Linear/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::LINEAR, FilterMode::LINEAR );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 2, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MAG_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(1, "TexParameteri", out.str()), true, TEST_LOCATION);


  /**************************************************************/

  // Nearest/Linear
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::NEAREST, FilterMode::LINEAR );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // Default/Default
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::DEFAULT, FilterMode::DEFAULT );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  // None/None
  texParameterTrace.Reset();
  texParameterTrace.Enable( true );

  actor.SetFilterMode( FilterMode::NONE, FilterMode::NONE );

  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  texParameterTrace.Enable( false );

  // Verify actor gl state
  DALI_TEST_EQUALS( texParameterTrace.CountMethod( "TexParameteri" ), 1, TEST_LOCATION);

  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_MIN_FILTER << ", " << GL_NEAREST_MIPMAP_LINEAR;
  DALI_TEST_EQUALS( texParameterTrace.TestMethodAndParams(0, "TexParameteri", out.str()), true, TEST_LOCATION);

  /**************************************************************/

  Stage::GetCurrent().Remove(actor);

  END_TEST;
}

int UtcDaliImageActorSetShaderEffect(void)
{
  TestApplication application;
  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );
  Stage::GetCurrent().Add( actor );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  application.GetGlAbstraction().EnableShaderCallTrace( true );

  const std::string vertexShader = "UtcDaliImageActorSetShaderEffect-VertexSource";
  const std::string fragmentShader = "UtcDaliImageActorSetShaderEffect-FragmentSource";
  ShaderEffect effect = ShaderEffect::New(vertexShader, fragmentShader );
  DALI_TEST_CHECK( effect != actor.GetShaderEffect() );

  actor.SetShaderEffect( effect );
  DALI_TEST_CHECK( effect == actor.GetShaderEffect() );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();
  DALI_TEST_EQUALS( lastShaderCompiledAfter, lastShaderCompiledBefore + 2, TEST_LOCATION );

  std::string actualVertexShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 1 );
  DALI_TEST_EQUALS( vertexShader,
                    actualVertexShader.substr( actualVertexShader.length() - vertexShader.length() ), TEST_LOCATION );
  std::string actualFragmentShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 2 );
  DALI_TEST_EQUALS( fragmentShader,
                    actualFragmentShader.substr( actualFragmentShader.length() - fragmentShader.length() ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageActorGetShaderEffect(void)
{
  TestApplication application;
  ImageActor actor = ImageActor::New();

  ShaderEffect effect = ShaderEffect::New("UtcDaliImageActorGetShaderEffect-VertexSource", "UtcDaliImageActorGetShaderEffect-FragmentSource" );
  actor.SetShaderEffect(effect);

  DALI_TEST_CHECK(effect == actor.GetShaderEffect());
  END_TEST;
}

int UtcDaliImageActorRemoveShaderEffect01(void)
{
  TestApplication application;
  ImageActor actor = ImageActor::New();

  ShaderEffect defaultEffect = actor.GetShaderEffect();

  ShaderEffect effect = ShaderEffect::New("UtcDaliImageActorRemoveShaderEffect-VertexSource", "UtcDaliImageActorRemoveShaderEffect-FragmentSource" );
  actor.SetShaderEffect(effect);

  DALI_TEST_CHECK(effect == actor.GetShaderEffect());

  actor.RemoveShaderEffect();

  DALI_TEST_CHECK(defaultEffect == actor.GetShaderEffect());
  END_TEST;
}

int UtcDaliImageActorRemoveShaderEffect02(void)
{
  TestApplication application;
  ImageActor actor = ImageActor::New();

  ShaderEffect defaultEffect = actor.GetShaderEffect();

  actor.RemoveShaderEffect();

  DALI_TEST_CHECK(defaultEffect == actor.GetShaderEffect());
  END_TEST;
}

int UtcDaliSetShaderEffectRecursively(void)
{
  TestApplication application;
  /**
   * create a tree
   *                 actor1
   *           actor2       actor4
   *       actor3 imageactor1
   * imageactor2
   */
  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor1 = ImageActor::New( img );
  Actor actor2 = Actor::New();
  actor1.Add( actor2 );
  Actor actor3 = Actor::New();
  actor2.Add( actor3 );
  ImageActor imageactor1 = ImageActor::New( img );
  actor2.Add( imageactor1 );
  ImageActor imageactor2 = ImageActor::New( img );
  actor3.Add( imageactor2 );
  Actor actor4 = Actor::New();
  actor1.Add( actor4 );
  Stage::GetCurrent().Add( actor1 );

  // flush the queue and render once
  application.SendNotification();
  application.Render();
  GLuint lastShaderCompiledBefore = application.GetGlAbstraction().GetLastShaderCompiled();

  application.GetGlAbstraction().EnableShaderCallTrace( true );

  const std::string vertexShader = "UtcDaliImageActorSetShaderEffect-VertexSource";
  const std::string fragmentShader = "UtcDaliImageActorSetShaderEffect-FragmentSource";
  // test with empty effect
  ShaderEffect effect;
  SetShaderEffectRecursively( actor1, effect );

  effect = ShaderEffect::New(vertexShader, fragmentShader );

  DALI_TEST_CHECK( effect != actor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect != imageactor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect != imageactor2.GetShaderEffect() );

  SetShaderEffectRecursively( actor1, effect );
  DALI_TEST_CHECK( effect == imageactor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect == imageactor2.GetShaderEffect() );

  // flush the queue and render once
  application.SendNotification();
  application.Render();

  GLuint lastShaderCompiledAfter = application.GetGlAbstraction().GetLastShaderCompiled();
  DALI_TEST_EQUALS( lastShaderCompiledAfter, lastShaderCompiledBefore + 2, TEST_LOCATION );

  std::string actualVertexShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 1 );
  DALI_TEST_EQUALS( vertexShader,
                    actualVertexShader.substr( actualVertexShader.length() - vertexShader.length() ), TEST_LOCATION );
  std::string actualFragmentShader = application.GetGlAbstraction().GetShaderSource( lastShaderCompiledBefore + 2 );
  DALI_TEST_EQUALS( fragmentShader,
                    actualFragmentShader.substr( actualFragmentShader.length() - fragmentShader.length() ), TEST_LOCATION );

  // remove from one that does not have shader
  RemoveShaderEffectRecursively( actor4 );

  // remove partially
  RemoveShaderEffectRecursively( actor3 );
  DALI_TEST_CHECK( effect == imageactor1.GetShaderEffect() );
  DALI_TEST_CHECK( effect != imageactor2.GetShaderEffect() );

  // test with empty actor just to check it does not crash
  Actor empty;
  SetShaderEffectRecursively( empty, effect );
  RemoveShaderEffectRecursively( empty );

  // test with actor with no children just to check it does not crash
  Actor loner = Actor::New();
  Stage::GetCurrent().Add( loner );
  SetShaderEffectRecursively( loner, effect );
  RemoveShaderEffectRecursively( loner );

  END_TEST;
}

int UtcDaliImageActorTestClearCache(void)
{
  // Testing the framebuffer state caching in frame-buffer-state-caching.cpp
  TestApplication application;

  tet_infoline("Testing Dali::ImageActor::ClearCache()");

  BufferImage img = BufferImage::New( 1,1 );
  ImageActor actor = ImageActor::New( img );

  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetAnchorPoint(AnchorPoint::CENTER);

  Stage::GetCurrent().Add(actor);

  /**************************************************************/
  // Flush the queue and render once
  application.SendNotification();
  application.Render();

  // There should be a single call to Clear
  DALI_TEST_EQUALS( application.GetGlAbstraction().GetClearCountCalled(), 1u, TEST_LOCATION );

  // the last set clear mask should be COLOR, DEPTH & STENCIL which occurs at the start of each frame
  GLbitfield mask = application.GetGlAbstraction().GetLastClearMask();
  DALI_TEST_CHECK( mask & GL_DEPTH_BUFFER_BIT );
  DALI_TEST_CHECK( mask & GL_STENCIL_BUFFER_BIT );
  DALI_TEST_CHECK( mask & GL_COLOR_BUFFER_BIT );

  END_TEST;
}
