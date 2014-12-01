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

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

// Internal headers are allowed here
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/image-factory.h>
#include <dali/internal/event/resources/resource-ticket.h>

using namespace Dali;

using Internal::ResourceTicketPtr;
using Internal::ImageFactory;
using Internal::ImageFactoryCache::RequestPtr;


namespace
{

static const char* gTestImageFilename = "icon_wrt.png";

static void EmulateImageLoaded( TestApplication& application, unsigned int width, unsigned int height )
{
  // emulate load success
  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,  ResourcePolicy::DISCARD );
  Integration::ResourcePointer resource( bitmap );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, width, height, width, height );
  if( request )
  {
    application.GetPlatform().SetResourceLoaded( request->GetId(), request->GetType()->id, resource );
  }

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();
}

} //anonymous namespace


// High-level test for image factory request cache
int UtcDaliImageFactoryUseCachedRequest01(void)
{
  TestApplication application;

  tet_infoline( "UtcDaliImageFactoryCachedRequest01 - Request same image more than once" );

  Image image = Image::New( gTestImageFilename );

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  Image image2 = Image::New( gTestImageFilename );

  application.SendNotification();
  application.Render();

  // check resource is not loaded twice
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  Image image3 = Image::New( gTestImageFilename );

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  END_TEST;
}

// High-level test for image factory request cache
int UtcDaliImageFactoryUseCachedRequest02(void)
{
  TestApplication application;

  // testing resource deletion when taken off stage
  tet_infoline( "UtcDaliImageFactoryCachedRequest02 - Discard previously requested resource" );

  Image image = Image::New( gTestImageFilename, Image::Immediate, Image::Unused );
  ImageActor actor = ImageActor::New( image );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  // Add actor to stage
  Stage::GetCurrent().Add( actor );

  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  // Release the resource, request is still cached
  Stage::GetCurrent().Remove( actor );
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  // Should find stale request in cache, so load image from filesystem
  Image image2 = Image::New( gTestImageFilename );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  // Resource is reloaded
  Image image3 = Image::New( gTestImageFilename );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  END_TEST;
}

// Low-level test for image factory request cache
int UtcDaliImageFactoryUseCachedRequest03(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryCachedRequest03 - Request same image more than once - Request Ids" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  RequestPtr req2 = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket2 = imageFactory.Load( *req2.Get() );
  DALI_TEST_EQUALS( req, req2, TEST_LOCATION );
  DALI_TEST_EQUALS( ticket, ticket2, TEST_LOCATION );

  req2 = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket3 = imageFactory.Load( *req2.Get() );
  DALI_TEST_EQUALS( req, req2, TEST_LOCATION );
  DALI_TEST_EQUALS( ticket, ticket3, TEST_LOCATION );

  // request differs in scaled size - not default size
  ImageAttributes attr = ImageAttributes::New( 80, 160, Pixel::BGR8888 );
  req2 = imageFactory.RegisterRequest( gTestImageFilename, &attr );
  ResourceTicketPtr ticket4 = imageFactory.Load( *req2.Get() );
  DALI_TEST_CHECK( req != req2 );
  END_TEST;
}

// Low-level test for image factory request cache
int UtcDaliImageFactoryUseCachedRequest04(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryCachedRequest04 - Request same image with different Image objects - Request Ids" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  ImageAttributes attr = ImageAttributes::New( 80, 160, Pixel::BGR8888 );
  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, &attr );

  ImageAttributes attr2 = ImageAttributes::New( 80, 160, Pixel::BGR8888 );
  RequestPtr req2 = imageFactory.RegisterRequest( gTestImageFilename, &attr2 );
  DALI_TEST_EQUALS( req, req2, TEST_LOCATION );
  END_TEST;
}

// Different requests, compatible resource
int UtcDaliImageFactoryCompatibleResource01(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryCompatibleResource01 - Two requests mapping to same resource" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  // request with default attributes ( size is 0,0 )
  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  ImageAttributes attr = ImageAttributes::New();
  attr.SetSize( 80, 80 );
  RequestPtr req2 = imageFactory.RegisterRequest( gTestImageFilename, &attr );
  ResourceTicketPtr ticket2 = imageFactory.Load( *req2.Get() );

  DALI_TEST_CHECK( req != req2 ); // different requests
  DALI_TEST_EQUALS( ticket->GetId(), ticket2->GetId(), TEST_LOCATION ); // same resource
  END_TEST;
}

// Different requests, compatible resource
int UtcDaliImageFactoryCompatibleResource02(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryCompatibleResource02 - Two requests mapping to same resource." );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  // request with default attributes ( size is 0,0 )
  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  // Request bigger size than actual image.
  // This will load the same resource.
  // However if image size changes later on to eg. 512*512 (file is overwritten),
  // reissuing these two requests will load different resources.
  // See UtcDaliImageFactoryReload06
  ImageAttributes attr = ImageAttributes::New();
  attr.SetSize( 92, 92 );
  RequestPtr req2 = imageFactory.RegisterRequest( gTestImageFilename, &attr );
  ResourceTicketPtr ticket2 = imageFactory.Load( *req2.Get() );

  DALI_TEST_CHECK( req != req2 ); // different requests
  DALI_TEST_EQUALS( ticket->GetId(), ticket2->GetId(), TEST_LOCATION ); // same resource
  END_TEST;
}

// Different requests, compatible resource
int UtcDaliImageFactoryCompatibleResource03(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryCompatibleResource03 - Two requests mapping to same resource" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  // this time use defined attributes, nut just NULL
  ImageAttributes attr = ImageAttributes::New();
  attr.SetSize( 120, 120 );

  // request with default attributes ( size is 0,0 )
  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, &attr );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  ImageAttributes attr2 = ImageAttributes::New();
  attr2.SetSize( 80, 80 );
  RequestPtr req2 = imageFactory.RegisterRequest( gTestImageFilename, &attr2 );
  ResourceTicketPtr ticket2 = imageFactory.Load( *req2.Get() );

  DALI_TEST_CHECK( req != req2 ); // different requests
  DALI_TEST_EQUALS( ticket->GetId(), ticket2->GetId(), TEST_LOCATION ); // same resource
  END_TEST;
}

// Test for reloading image
int UtcDaliImageFactoryReload01(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryReload01 - Reload unchanged image" );

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  ResourceTicketPtr ticket2 = imageFactory.Reload( *req.Get() );
  DALI_TEST_EQUALS( ticket, ticket2, TEST_LOCATION );

  ResourceTicketPtr ticket3 = imageFactory.Reload( *req.Get() );
  DALI_TEST_EQUALS( ticket, ticket3, TEST_LOCATION );
  END_TEST;
}

// Testing file system access when reloading image
int UtcDaliImageFactoryReload02(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryReload02 - Reload unchanged image" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  ResourceTicketPtr ticket2 = imageFactory.Reload( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( ticket, ticket2, TEST_LOCATION );
  // resource is still loading, do not issue another request
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  ResourceTicketPtr ticket3 = imageFactory.Reload( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( ticket, ticket3, TEST_LOCATION );
  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  ticket3 = imageFactory.Reload( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  END_TEST;
}

// Test for reloading changed image
int UtcDaliImageFactoryReload03(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryReload03 - Reload changed image" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize( 80.0f, 80.0f );
  application.GetPlatform().SetClosestImageSize( testSize );

  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  Vector2 newSize( 192.0f, 192.0f );
  application.GetPlatform().SetClosestImageSize( newSize );

  // Image file changed size, new resource request should be issued
  ResourceTicketPtr ticket2 = imageFactory.Reload( *req.Get() );
  DALI_TEST_CHECK( ticket != ticket2 );

  ResourceTicketPtr ticket3 = imageFactory.Reload( *req.Get() );
  DALI_TEST_EQUALS( ticket2, ticket3, TEST_LOCATION );
  END_TEST;
}

// Testing file system access when reloading image
int UtcDaliImageFactoryReload04(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryReload04 - Reload unchanged image" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  ResourceTicketPtr ticket2 = imageFactory.Reload( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( ticket, ticket2, TEST_LOCATION );
  // resource is still loading, do not issue another request
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  ResourceTicketPtr ticket3 = imageFactory.Reload( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // size didn't change, using same ticket
  DALI_TEST_EQUALS( ticket, ticket3, TEST_LOCATION );
  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  application.GetPlatform().ResetTrace();

  // still loading
  ticket3 = imageFactory.Reload( *req.Get() );
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  END_TEST;
}

// Testing OnDemand + Reload
// Reload should have no effect if OnDemand Image is not loaded yet, as stated in the API documentation
int UtcDaliImageFactoryReload05(void)
{
  TestApplication application;

  tet_infoline( "UtcDaliImageFactoryReload05 - Reload OnDemand image" );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  RequestPtr req;
  ImageAttributes attr = ImageAttributes::New();
  attr.SetSize( 80, 80 );

  // this happens first when loading Image OnDemand
  req = imageFactory.RegisterRequest( gTestImageFilename, &attr );

  application.SendNotification();
  application.Render();

  ResourceTicketPtr ticket = imageFactory.Reload( *req.Get() );

  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  DALI_TEST_CHECK( !ticket );

  // this happens when Image is put on stage
  ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  DALI_TEST_CHECK( ticket );
  application.GetPlatform().ResetTrace();

  ticket = imageFactory.Reload( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // still loading, no new request
  DALI_TEST_CHECK( !application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  ticket = imageFactory.Reload( *req.Get() );

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();


  DALI_TEST_CHECK( application.GetPlatform().WasCalled( TestPlatformAbstraction::LoadResourceFunc ) );
  END_TEST;
}

// Initally two different requests map to same resource.
// After overwriting the file, they load different image resources.
int UtcDaliImageFactoryReload06(void)
{
  TestApplication application;
  tet_infoline( "UtcDaliImageFactoryReload06 - Two requests first mapping to same resource, then different resources." );

  ImageFactory& imageFactory  = Internal::ThreadLocalStorage::Get().GetImageFactory();

  Vector2 testSize(80.0f, 80.0f);
  application.GetPlatform().SetClosestImageSize(testSize);

  // request with default attributes ( size is 0,0 )
  RequestPtr req = imageFactory.RegisterRequest( gTestImageFilename, NULL );
  ResourceTicketPtr ticket = imageFactory.Load( *req.Get() );

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // emulate load success
  EmulateImageLoaded( application, 80, 80 );

  // Request bigger size than actual image.
  // This will load the same resource.
  // However if image size changes later on to eg. 512*512 (file is overwritten),
  // reissuing these two requests will load different resources.
  ImageAttributes attr = ImageAttributes::New();
  attr.SetSize( 92, 92 );
  RequestPtr req2 = imageFactory.RegisterRequest( gTestImageFilename, &attr );
  ResourceTicketPtr ticket2 = imageFactory.Load( *req2.Get() );

  DALI_TEST_CHECK( req != req2 ); // different requests
  DALI_TEST_EQUALS( ticket->GetId(), ticket2->GetId(), TEST_LOCATION ); // same resource

  Vector2 newSize(512.0f, 512.0f);
  application.GetPlatform().SetClosestImageSize(newSize);

  // reload fixed size (192,192) request
  ticket2 = imageFactory.Reload( *req2.Get() );

  // emulate load success
  // note: this is the only way to emulate what size is loaded by platform abstraction
  EmulateImageLoaded( application, 92, 92 );

  // reload default size request
  ticket = imageFactory.Reload( *req.Get() );

  DALI_TEST_CHECK( ticket->GetId() != ticket2->GetId() ); // different resources
  END_TEST;
}
