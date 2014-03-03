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
#include <test-native-image.h>

// Internal headers are allowed here

#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/resources/bitmap-metadata.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/resources/image-ticket.h>
#include <dali/internal/event/resources/resource-ticket-observer.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/modeling/model-data-impl.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/internal/render/gl-resources/texture-declarations.h>
#include <dali/internal/render/shaders/shader.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/public-api/shader-effects/shader-effect.h>


using namespace Dali;
#include <mesh-builder.h>

static void Startup();
static void Cleanup();

extern "C"
{
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliInternalRequestResourceBitmapRequests01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalRequestResourceBitmapRequests02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalRequestResourceBitmapRequests03, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalRequestReloadBitmapRequests01,   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalRequestReloadBitmapRequests02,   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalRequestReloadBitmapRequests03,   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalSaveResource01,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalSaveResource02,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalSaveResource03,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalSaveResource04,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalSaveResource05,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalRequestResourceTicket01,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalRequestResourceTicket02,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalLoadShaderRequest01,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalLoadShaderRequest02,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAllocateBitmapImage01,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAddBitmapImage01,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAddBitmapImage02,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAddBitmapImage03,                NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalGetBitmapImage01,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalGetBitmapImage02,                NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalGetBitmapImage03,                NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAllocateTexture01,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAddNativeImage,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAddFrameBufferImage,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliInternalAllocateMesh01,                  POSITIVE_TC_IDX );


class TestTicketObserver : public Internal::ResourceTicketObserver
{
public:
  TestTicketObserver()
  : mLoadingFailedCalled(0), mLoadingSucceededCalled(0),
    mSavingFailedCalled(0), mSavingSucceededCalled(0),
    mUploadedCount(0)
  {}

  int LoadFailedCalled() {
    tet_printf("TicketObserver: LoadingFailed() called %d times", mLoadingFailedCalled);
    return mLoadingFailedCalled;
  }
  int LoadSucceededCalled() {
    tet_printf("TicketObserver: LoadingSucceeded()  called %d times", mLoadingSucceededCalled);
    return mLoadingSucceededCalled;
  }
  int SaveFailedCalled() {
    tet_printf("TicketObserver: SavingFailed() called %d times", mSavingFailedCalled);
    return mSavingFailedCalled;
  }
  int SaveSucceededCalled() {
    tet_printf("TicketObserver: SavingSucceeded() called %d times", mSavingSucceededCalled);
    return mSavingSucceededCalled;
  }
  int  UploadCalled() {
    tet_printf("TicketObserver: Uploaded() called %d times", mUploadedCount);
    return mUploadedCount;
  }
  void Reset() {
    mLoadingFailedCalled    = 0;
    mLoadingSucceededCalled = 0;
    mSavingFailedCalled     = 0;
    mSavingSucceededCalled  = 0;
    mUploadedCount           = 0;
  }

public: // From ResourceTicketObserver
  virtual void ResourceLoadingFailed(const Internal::ResourceTicket& ticket) {mLoadingFailedCalled++;}
  virtual void ResourceLoadingSucceeded(const Internal::ResourceTicket& ticket) {mLoadingSucceededCalled++;}
  virtual void ResourceSavingFailed(const Internal::ResourceTicket& ticket) {mSavingFailedCalled++;}
  virtual void ResourceSavingSucceeded(const Internal::ResourceTicket& ticket) {mSavingSucceededCalled++;}
  virtual void ResourceUploaded(const Internal::ResourceTicket& ticket) {mUploadedCount++;}

private:
  int mLoadingFailedCalled;
  int mLoadingSucceededCalled;
  int mSavingFailedCalled;
  int mSavingSucceededCalled;
  int mUploadedCount;
};

class TestTicketLifetimeObserver : public Internal::ResourceTicketLifetimeObserver
{
public:
  TestTicketLifetimeObserver() : resourceTicketDiscarded(false) {}

  virtual void ResourceTicketDiscarded( const Internal::ResourceTicket& ticket )
  { resourceTicketDiscarded = true; }

  void Reset() { resourceTicketDiscarded = false; }
  bool resourceTicketDiscarded;
};

static TestTicketObserver testTicketObserver;
static TestTicketLifetimeObserver testTicketLifetimeObserver;

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

Internal::ImagePtr LoadImage(TestApplication& application, char* name)
{
  Internal::ImagePtr image = Internal::Image::New(name);
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 80,80,80,80 );
  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  req=NULL;
  application.GetPlatform().ResetTrace();
  return image;
}


Internal::ResourceTicketPtr CheckLoadBitmap(TestApplication& application, char* name, int w, int h)
{
  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest(attr);
  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, name );
  ticket->AddObserver(testTicketObserver);
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest*   req = application.GetPlatform().GetRequest();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, w, h, w, h );
  Integration::ResourcePointer resourcePtr(bitmap); // reference it
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoadingSucceeded );
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  req=NULL;
  application.GetPlatform().ResetTrace();

  return ticket;
}

Internal::ResourceTicketPtr CheckLoadModel(TestApplication& application, char* name)
{
  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( Integration::ModelResourceType(), name );
  ticket->AddObserver(testTicketObserver);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Dali::ModelData modelData = BuildTreeModel();
  Internal::ModelData& modelDataImpl = GetImplementation(modelData);
  Integration::ResourcePointer resourcePtr(&modelDataImpl);
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  DALI_TEST_CHECK(ticket->GetLoadingState() == ResourceLoadingSucceeded);
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded
  req=NULL;
  application.GetPlatform().ResetTrace();

  return ticket;
}


// Load a bitmap resource successfully, then discard it's ticket
static void UtcDaliInternalRequestResourceBitmapRequests01()
{
  TestApplication application; // Reset all test adapter return codes

  tet_infoline("Testing bitmap requests");

  Internal::ResourceManager& resourceManager = Internal::ThreadLocalStorage::Get().GetResourceManager();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest (attr);
  Internal::ResourceId id(0);

  testTicketObserver.Reset();

  {
    Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
    /************************************************************
     * FUNCTION UNDER TEST
     ***********************************************************/
    Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, "image.png" );
    ticket->AddObserver(testTicketObserver);

    // Update thread will request the bitmap resource:
    // Sets application.GetPlatform().mRequest
    application.SendNotification(); // Run flush update queue
    application.Render(1);          // Process update messages
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );

    application.SendNotification(); // Send any event messages
    DALI_TEST_CHECK( testTicketObserver.LoadFailedCalled() == 0 );
    DALI_TEST_CHECK( testTicketObserver.LoadSucceededCalled() == 0 );
    DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoading );

    Internal::ImageTicketPtr imageTicket(dynamic_cast<Internal::ImageTicket*>(ticket.Get()));
    DALI_TEST_CHECK( imageTicket );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );

    // Create a resource
    Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
    Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
    bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 80, 80, 80, 80 );
    Integration::ResourcePointer resourcePtr(bitmap); // reference it

    // Set up platform abstraction to load it
    id=req->GetId();
    application.GetPlatform().SetResourceLoaded( id, Integration::ResourceBitmap, resourcePtr );

    DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(req->GetId()) );

    // load the cache, which will immediately have the loaded resource
    application.Render(0);

    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetResourcesFunc ) );

    DALI_TEST_CHECK( resourceManager.IsResourceLoaded(req->GetId()) );
    Internal::BitmapMetadata bitmapData = resourceManager.GetBitmapMetadata(req->GetId());
    DALI_TEST_CHECK( bitmapData.GetWidth() == 80 );
    DALI_TEST_CHECK( bitmapData.GetHeight() == 80 );

    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );

    // Trigger the event thread to process notify messages. Should then trigger the signals
    // in the ticket observer
    application.SendNotification();

    DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoadingSucceeded );
    DALI_TEST_EQUALS(testTicketObserver.LoadSucceededCalled(), 1, TEST_LOCATION );

    // Check that the image ticket was updated with the image attributes
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 80, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 80, TEST_LOCATION );
  } // Discard ticket

  application.SendNotification(); // Flush update queue (with ticket discarded message
  application.Render(1);          // Process update messages
  application.SendNotification(); // Send event notifications
  application.Render(1);          // Process update messages

  // Resource should have been discarded.
  DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::CancelLoadFunc ) );
  DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));

  DALI_TEST_EQUALS(testTicketObserver.LoadSucceededCalled(), 1, TEST_LOCATION );
  DALI_TEST_EQUALS(testTicketObserver.LoadFailedCalled(), 0, TEST_LOCATION );
}

// Starting Loading a bitmap resource, then discard it's ticket before loading complete.
static void UtcDaliInternalRequestResourceBitmapRequests02()
{
  TestApplication application; // Reset all test adapter return codes

  tet_infoline("Testing bitmap request ticket discard before load complete");

  Internal::ResourceManager& resourceManager = Internal::ThreadLocalStorage::Get().GetResourceManager();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest (attr);
  Internal::ResourceId id(0);

  testTicketObserver.Reset();

  {
    Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
    /************************************************************
     * FUNCTION UNDER TEST
     ***********************************************************/
    Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, "image.png" );
    ticket->AddObserver(testTicketObserver);
    Internal::ImageTicketPtr imageTicket(dynamic_cast<Internal::ImageTicket*>(ticket.Get()));
    DALI_TEST_CHECK( imageTicket );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );

    // Update thread will request the bitmap resource:
    // Sets application.GetPlatform().mRequest
    application.SendNotification(); // Run flush update queue
    application.Render(1);
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );
    Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
    id=req->GetId();

    application.SendNotification(); // Should produce no messages
    DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.LoadFailedCalled(), 0, TEST_LOCATION );

    DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));

    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );
    DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoading );

  } // Discard ticket

  // Ensure ticket discarded message is sent to update thread
  application.SendNotification(); // Flush update queue
  application.Render(0);          // Process update messages

  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::CancelLoadFunc ) );
  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetResourcesFunc ) );

  DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));

  // Trigger the event thread to process notify messages. Should then trigger the signals
  // in the ticket observer
  application.SendNotification();

  DALI_TEST_EQUALS(testTicketObserver.LoadSucceededCalled(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS(testTicketObserver.LoadFailedCalled(), 0, TEST_LOCATION );
}

// start loading a bitmap resource that doesn't exist, then discard it's ticket after failure
static void UtcDaliInternalRequestResourceBitmapRequests03()
{
  TestApplication application; // Reset all test adapter return codes

  tet_infoline("Load bitmap that doesn't exist, followed by ticket discard. Expect LoadingFailed");

  Internal::ResourceManager& resourceManager = Internal::ThreadLocalStorage::Get().GetResourceManager();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest (attr);
  Internal::ResourceId id(0);

  testTicketObserver.Reset();
  { // Scope lifetime of ticket
    Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

    /************************************************************
     * FUNCTION UNDER TEST
     ***********************************************************/
    Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, "image.png" );
    ticket->AddObserver(testTicketObserver);
    Internal::ImageTicketPtr imageTicket(dynamic_cast<Internal::ImageTicket*>(ticket.Get()));
    DALI_TEST_CHECK( imageTicket );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );

    // Update thread will request the bitmap resource:
    // Sets application.GetPlatform().mRequest
    application.SendNotification(); // Run flush update queue
    application.Render(1);          // process update messages
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );
    Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
    id=req->GetId();
    application.SendNotification(); // Should produce no messages
    DALI_TEST_CHECK( testTicketObserver.LoadFailedCalled() == 0 );
    DALI_TEST_CHECK( testTicketObserver.LoadSucceededCalled() == 0 );

    DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));

    application.GetPlatform().SetResourceLoadFailed(id, Integration::FailureFileNotFound );

    application.Render(0); // Get failed result
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetResourcesFunc ) );
    application.SendNotification(); // send failed
    DALI_TEST_CHECK( testTicketObserver.LoadFailedCalled() != 0 );
    DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoadingFailed );

    DALI_TEST_CHECK( testTicketObserver.LoadSucceededCalled() == 0 );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );

    DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));
  } // Discard ticket

  application.Render(0); // Send DiscardTicket
  application.SendNotification();

  DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));
}



// Load a bitmap resource successfully, then reload it
static void UtcDaliInternalRequestReloadBitmapRequests01()
{
  TestApplication application; // Reset all test adapter return codes

  tet_infoline("Testing bitmap reload after successful load");

  Internal::ResourceManager& resourceManager = Internal::ThreadLocalStorage::Get().GetResourceManager();
  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  Internal::ResourceId id(0);
  testTicketObserver.Reset();

  {
    Internal::ResourceTicketPtr ticket = CheckLoadBitmap( application, "image.png", 80, 80 );
    Internal::ImageTicketPtr imageTicket(dynamic_cast<Internal::ImageTicket*>(ticket.Get()));
    id = ticket->GetId();

    // Reset call statistics - test that resource is reloaded
    application.GetPlatform().ResetTrace();

    /************************************************************
     * FUNCTION UNDER TEST
     ***********************************************************/
    resourceClient.ReloadResource( ticket->GetId() );

    DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );
    application.SendNotification(); // Flush update messages
    application.Render(0);  // Process update messages
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );
    application.SendNotification(); // Process event messages

    DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoading, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.LoadFailedCalled(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 1, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.UploadCalled(), 0, TEST_LOCATION );

    // Create a new resource - the image size could have changed in the meantime
    Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
    Integration::Bitmap* bitmap2 = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
    bitmap2->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 120, 120, 120, 120 );
    Integration::ResourcePointer resourcePtr2(bitmap2); // reference it
    DALI_TEST_CHECK( req->GetId() == ticket->GetId() );
    application.GetPlatform().SetResourceLoaded(id, Integration::ResourceBitmap, resourcePtr2);

    application.Render(0);  // Process update messages / UpdateCache
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetResourcesFunc ) );

    DALI_TEST_CHECK( resourceManager.IsResourceLoaded(id));
    Internal::BitmapMetadata bitmapData = resourceManager.GetBitmapMetadata(id);
    DALI_TEST_CHECK( bitmapData.GetWidth() == 120 );
    DALI_TEST_CHECK( bitmapData.GetHeight() == 120 );

    // Ticket can't have been updated yet - it should still have old values
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 80, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 80, TEST_LOCATION );

    application.SendNotification(); // Process event messages
    application.Render(0);          // Process update messages / UpdateCache
    application.SendNotification(); // Process event messages

    DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 2, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.UploadCalled(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 120, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 120, TEST_LOCATION );

  } // Discard ticket

  application.SendNotification(); // Flush update queue (with ticket discarded message
  application.Render(1);          // Process update messages
  application.SendNotification(); // Send event notifications
  application.Render(1);          // Process update messages

  // Resource should have been discarded.
  DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::CancelLoadFunc ) );
  DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));

  DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 2, TEST_LOCATION );
  DALI_TEST_EQUALS( testTicketObserver.LoadFailedCalled(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( testTicketObserver.UploadCalled(), 0, TEST_LOCATION );
}


static void UtcDaliInternalRequestReloadBitmapRequests02()
{
  TestApplication application; // Reset all test adapter return codes

  tet_infoline("Testing bitmap reload during first load");

  Internal::ResourceManager& resourceManager = Internal::ThreadLocalStorage::Get().GetResourceManager();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest (attr);
  Internal::ResourceId id(0);

  testTicketObserver.Reset();

  {
    Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
    Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, "image.png" );
    ticket->AddObserver(testTicketObserver);

    // Update thread will request the bitmap resource:
    // Sets application.GetPlatform().mRequest
    application.SendNotification(); // Run flush update queue
    application.Render(1);          // Process update messages
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );

    application.SendNotification(); // Send any event messages
    DALI_TEST_CHECK( testTicketObserver.LoadFailedCalled() == 0 );
    DALI_TEST_CHECK( testTicketObserver.LoadSucceededCalled() == 0 );

    Internal::ImageTicketPtr imageTicket(dynamic_cast<Internal::ImageTicket*>(ticket.Get()));
    DALI_TEST_CHECK( imageTicket );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );


    /************************************************************
     * FUNCTION UNDER TEST
     ***********************************************************/
    resourceClient.ReloadResource( ticket->GetId() );
    /************************************************************
     * Expected result - current load completes as usual, no reload requested
     ************************************************************/

    application.SendNotification(); // Flush update methods

    // Reset call statistics - test that resource is not reloaded
    application.GetPlatform().ResetTrace();

    application.Render(0); // Process reload message (nothing for UpdateCache yet)

    DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetResourcesFunc ) );
    // Create a resource
    Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
    Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
    bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 80, 80, 80, 80 );
    Integration::ResourcePointer resourcePtr(bitmap); // reference it

    // Set up platform abstraction to load it
    id=req->GetId();

    application.GetPlatform().SetResourceLoaded(id, Integration::ResourceBitmap, resourcePtr);

    DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));

    application.GetPlatform().ResetTrace();
    // load the cache, which will immediately have the loaded resource
    application.Render(0);
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetResourcesFunc ) );
    DALI_TEST_CHECK( resourceManager.IsResourceLoaded(id));
    Internal::BitmapMetadata bitmapData = resourceManager.GetBitmapMetadata(id);
    DALI_TEST_CHECK( bitmapData.GetWidth() == 80 );
    DALI_TEST_CHECK( bitmapData.GetHeight() == 80 );

    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );

    // Trigger the event thread to process notify messages. Should then trigger the signals
    // in the ticket observer
    application.SendNotification();

    DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 1, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.UploadCalled(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );

    // Check that the image ticket was updated with the image attributes
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 80, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 80, TEST_LOCATION );

    DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );

    application.SendNotification(); // Flush update messages
    application.Render(0);  // Process update messages

    // There should be no reload
    DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );
    application.SendNotification(); // Process event messages

    DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.LoadFailedCalled(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 1, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.UploadCalled(), 0, TEST_LOCATION );

    DALI_TEST_CHECK( resourceManager.IsResourceLoaded(id));
    bitmapData = resourceManager.GetBitmapMetadata(id);
    DALI_TEST_CHECK( bitmapData.GetWidth() == 80 );
    DALI_TEST_CHECK( bitmapData.GetHeight() == 80 );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 80, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 80, TEST_LOCATION );

  } // Discard ticket
}


static void UtcDaliInternalRequestReloadBitmapRequests03()
{
  TestApplication application; // Reset all test adapter return codes

  tet_infoline("Testing bitmap reload at end of first load");

  Internal::ResourceManager& resourceManager = Internal::ThreadLocalStorage::Get().GetResourceManager();
  ImageAttributes attr;
  Integration::BitmapResourceType bitmapRequest (attr);
  Internal::ResourceId id(0);

  testTicketObserver.Reset();

  {
    Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();
    Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( bitmapRequest, "image.png" );
    ticket->AddObserver(testTicketObserver);

    // Update thread will request the bitmap resource:
    // Sets application.GetPlatform().mRequest
    application.SendNotification(); // Run flush update queue
    application.Render(1);          // Process update messages
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );

    application.SendNotification(); // Send any event messages
    DALI_TEST_CHECK( testTicketObserver.LoadFailedCalled() == 0 );
    DALI_TEST_CHECK( testTicketObserver.LoadSucceededCalled() == 0 );

    Internal::ImageTicketPtr imageTicket(dynamic_cast<Internal::ImageTicket*>(ticket.Get()));
    DALI_TEST_CHECK( imageTicket );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );


    /************************************************************
     * FUNCTION UNDER TEST
     ***********************************************************/
    resourceClient.ReloadResource( ticket->GetId() );
    /************************************************************
     * Expected result - current load completes as usual, no reload requested
     ************************************************************/

    application.SendNotification(); // Flush update methods

    // Reset call statistics - test that resource is not reloaded
    application.GetPlatform().ResetTrace();

    // Create a resource
    Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
    Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
    bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 80, 80, 80, 80 );
    Integration::ResourcePointer resourcePtr(bitmap); // reference it

    // Set up platform abstraction to load it
    id=req->GetId();

    application.GetPlatform().SetResourceLoaded(id, Integration::ResourceBitmap, resourcePtr);

    DALI_TEST_CHECK( ! resourceManager.IsResourceLoaded(id));

    application.GetPlatform().ResetTrace();
    // load the cache, which will immediately have the loaded resource
    application.Render(0);

    // UpdateCache runs before ProcessMessages, so the loading resource completes before
    // the reload request is handled.
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc ) );
    DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::GetResourcesFunc ) );

    DALI_TEST_CHECK( resourceManager.IsResourceLoaded(id));
    Internal::BitmapMetadata bitmapData = resourceManager.GetBitmapMetadata(id);
    DALI_TEST_CHECK( bitmapData.GetWidth() == 80 );
    DALI_TEST_CHECK( bitmapData.GetHeight() == 80 );

    DALI_TEST_EQUALS( imageTicket->GetWidth(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 0, TEST_LOCATION );

    // Trigger the event thread to process notify messages. Should then trigger the signals
    // in the ticket observer
    application.SendNotification();

    DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 1, TEST_LOCATION );
    DALI_TEST_EQUALS( testTicketObserver.UploadCalled(), 0, TEST_LOCATION );
    DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoading, TEST_LOCATION );

    // Check that the image ticket was updated with the image attributes
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 80, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 80, TEST_LOCATION );

    DALI_TEST_EQUALS( resourceManager.ResourcesToProcess(), true, TEST_LOCATION );

    // Create a new resource - the image size could have changed in the meantime
    req = application.GetPlatform().GetRequest();
    Integration::Bitmap* bitmap2 = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, false );
    bitmap2->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 120, 120, 120, 120 );
    Integration::ResourcePointer resourcePtr2(bitmap2); // reference it
    DALI_TEST_CHECK( req->GetId() == id );
    application.GetPlatform().SetResourceLoaded(id, Integration::ResourceBitmap, resourcePtr2);

    application.Render(0);  // Process update messages / UpdateCache

    DALI_TEST_CHECK( resourceManager.IsResourceLoaded(id));
    bitmapData = resourceManager.GetBitmapMetadata(id);
    DALI_TEST_CHECK( bitmapData.GetWidth() == 120 );
    DALI_TEST_CHECK( bitmapData.GetHeight() == 120 );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 80, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 80, TEST_LOCATION );

    application.SendNotification(); // Process event messages

    DALI_TEST_EQUALS( testTicketObserver.LoadSucceededCalled(), 2, TEST_LOCATION );

    // Not staged - no GL upload
    DALI_TEST_EQUALS( testTicketObserver.UploadCalled(), 0, TEST_LOCATION );

    DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetWidth(), 120, TEST_LOCATION );
    DALI_TEST_EQUALS( imageTicket->GetHeight(), 120, TEST_LOCATION );
    DALI_TEST_EQUALS( resourceManager.ResourcesToProcess(), false, TEST_LOCATION );
  }
}


static void UtcDaliInternalSaveResource01()
{
  TestApplication application;
  tet_infoline("Testing SaveResource() with valid id, and valid filename");

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  Dali::ModelData modelData = BuildTreeModel();
  testTicketObserver.Reset();

  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( Integration::ModelResourceType(), "model.dae" );
  ticket->AddObserver(testTicketObserver);

  // First, load a model resource
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Internal::ModelData& modelDataImpl = GetImplementation(modelData);
  Integration::ResourcePointer resourcePtr(&modelDataImpl);

  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  DALI_TEST_CHECK( ticket->GetLoadingState() == ResourceLoadingSucceeded );

  // Try saving it
  resourceClient.SaveResource( ticket, "model.dali-bin" );
  application.SendNotification(); // Flush update messages
  application.Render();           // Process save resource request

  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::SaveResourceFunc ) );
  req = application.GetPlatform().GetRequest();
  DALI_TEST_CHECK( req->GetType()->id == Integration::ResourceModel );
  DALI_TEST_CHECK( req->GetPath().compare("model.dali-bin") == 0 );

  // Set up success response
  application.GetPlatform().SetResourceSaved(req->GetId(), req->GetType()->id);
  application.Render();           // GetResources --> SaveComplete
  application.SendNotification(); // Send event messages

  DALI_TEST_CHECK( testTicketObserver.SaveSucceededCalled() == 1 );
  DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION )
;
}


static void UtcDaliInternalSaveResource02()
{
  TestApplication application;
  tet_infoline("Testing SaveResource() with invalid id");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  Dali::ModelData modelData = BuildTreeModel();

  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( Integration::ModelResourceType(), "model.dae" );
  ticket->AddObserver(testTicketObserver);

  // First, load a model resource
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Internal::ModelData& modelDataImpl = GetImplementation(modelData);
  Integration::ResourcePointer resourcePtr(&modelDataImpl);
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  DALI_TEST_CHECK(ticket->GetLoadingState() == ResourceLoadingSucceeded);
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded

  Internal::ResourceTypePath typePath(Integration::ModelResourceType(), "");
  Internal::ResourceTicketPtr aTicket = new Internal::ResourceTicket( testTicketLifetimeObserver, 2000,  typePath );
  try
  {
    resourceClient.SaveResource( aTicket, "model.dali-bin" ); // Should be outside range of valid resources!
  } catch (DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());

    DALI_TEST_ASSERT(e, "mImpl->mTickets.find(ticket->GetId()) != mImpl->mTickets.end()", TEST_LOCATION );
  }

  application.SendNotification(); // Flush update messages
  application.Render();           // Process save resource request

  // Ensure no request sent to platform abstraction
  DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::SaveResourceFunc ) );
  req = application.GetPlatform().GetRequest();
  DALI_TEST_CHECK ( req == NULL );
}

static void UtcDaliInternalSaveResource03()
{
  TestApplication application;
  tet_infoline("Testing SaveResource() with invalid id");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  Dali::ModelData modelData = BuildTreeModel();

  Internal::ResourceTicketPtr ticket = resourceClient.RequestResource( Integration::ModelResourceType(), "model.dae" );
  ticket->AddObserver(testTicketObserver);

  // First, load a model resource
  application.SendNotification(); // Flush update messages
  application.Render();           // Process resource request
  Integration::ResourceRequest* req = application.GetPlatform().GetRequest();
  Internal::ModelData& modelDataImpl = GetImplementation(modelData);
  Integration::ResourcePointer resourcePtr(&modelDataImpl);
  application.GetPlatform().SetResourceLoaded(req->GetId(), req->GetType()->id, resourcePtr);
  application.Render();           // Process LoadComplete
  application.SendNotification(); // Process event messages
  DALI_TEST_CHECK(ticket->GetLoadingState() == ResourceLoadingSucceeded);
  application.GetPlatform().DiscardRequest(); // Ensure load request is discarded

  Internal::ResourceTicketPtr aTicket;
  try
  {
    resourceClient.SaveResource( aTicket, "model.dali-bin" ); // Should be outside range of valid resources!
  } catch (DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());

    DALI_TEST_EQUALS(e.mCondition, "ticket", TEST_LOCATION);
  }

  application.SendNotification(); // Flush update messages
  application.Render();           // Process save resource request

  // Ensure no request sent to platform abstraction
  DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::SaveResourceFunc ) );
  req = application.GetPlatform().GetRequest();
  DALI_TEST_CHECK ( req == NULL );
}


static void UtcDaliInternalSaveResource04()
{
  TestApplication application;
  tet_infoline("Testing SaveResource() with valid id, but invalid filename");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  // First, load a model resource
  Internal::ResourceTicketPtr ticket = CheckLoadModel(application, "model.dae");

  resourceClient.SaveResource( ticket, "model.dali-bin" );
  application.SendNotification(); // Flush update messages
  application.Render();           // Process save resource request

  // Set up fail response
  application.GetPlatform().SetResourceSaveFailed(ticket->GetId(), Integration::FailureInvalidPath );
  application.Render();           // GetResources --> SaveFailed
  application.SendNotification(); // Send event messages

  DALI_TEST_CHECK( testTicketObserver.SaveSucceededCalled() == 0 );
  DALI_TEST_CHECK( testTicketObserver.SaveFailedCalled() == 1 );
  DALI_TEST_EQUALS( ticket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION )
;
}

static void UtcDaliInternalSaveResource05()
{
  TestApplication application;
  tet_infoline("Testing SaveResource() with valid id, but invalid resource type");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  // First, load a bitmap resource
  Internal::ResourceTicketPtr ticket = CheckLoadBitmap(application, "bitmap.jpg", 80, 80);

  // Try saving it
  resourceClient.SaveResource( ticket, "bitmap.png" );
  application.SendNotification(); // Flush update messages
  application.Render();           // Process save resource request

  DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::SaveResourceFunc ) );
}

static void UtcDaliInternalRequestResourceTicket01()
{
  TestApplication application;
  tet_infoline("Testing RequestResourceTicket() with valid id");

  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  // First, load a bitmap resource
  Internal::ResourceTicketPtr ticket = CheckLoadBitmap(application, "bitmap.jpg", 80, 80);

  Internal::ResourceTicketPtr newTicket = resourceClient.RequestResourceTicket( ticket->GetId() );
  DALI_TEST_CHECK( newTicket );
  DALI_TEST_CHECK( newTicket->GetId() == ticket->GetId() );
  DALI_TEST_CHECK( newTicket->GetTypePath().type->id == ticket->GetTypePath().type->id );
}

static void UtcDaliInternalRequestResourceTicket02()
{
  TestApplication application;
  tet_infoline("Testing RequestResourceTicket() with invalid id");

  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  // First, load a bitmap resource
  Internal::ResourceTicketPtr ticket = CheckLoadBitmap(application, "bitmap.jpg", 80, 80);

  Internal::ResourceTicketPtr newTicket = resourceClient.RequestResourceTicket( ticket->GetId() + 2000 );
  DALI_TEST_CHECK( ! newTicket );
}

static void UtcDaliInternalLoadShaderRequest01()
{
  TestApplication application;
  tet_infoline("Testing LoadShader() success");
  testTicketObserver.Reset();

  // Clear through all of the outstanding shader load requests from the default shader effect
  std::vector< unsigned char > buffer;
  for( int i=0; i<10; i++ )
  {
    buffer.push_back((unsigned char)i);
  }
  application.GetPlatform().SetLoadFileResult( true, buffer );
  application.GetGlAbstraction().SetLinkStatus(1);
  application.SendNotification(); // Flush update messages
  application.Render();           // Process load shader request (immediately)
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  Internal::ResourceClient& resourceClient = Internal::ThreadLocalStorage::Get().GetResourceClient();

  Integration::ShaderResourceType shaderRequest(123, "vertex src", "frag src");
  std::string shaderBinaryFile("shader.bin");
  Internal::ResourceTicketPtr ticket = resourceClient.LoadShader(shaderRequest, shaderBinaryFile);
  DALI_TEST_CHECK( ticket );

  application.GetPlatform().SetLoadFileResult( true, buffer );
  application.GetGlAbstraction().EnableShaderCallTrace( true );
  application.GetGlAbstraction().SetLinkStatus(1);

  application.SendNotification(); // Flush update messages
  application.Render();           // Process load shader request (immediately)

  application.SendNotification();
  application.Render();

  application.SendNotification();
  application.Render();

  // If shader program loads OK, we shouldn't see any calls to CompileShader or SaveResource
  TraceCallStack& shaderTrace = application.GetGlAbstraction().GetShaderTrace();
  DALI_TEST_CHECK( ! shaderTrace.FindMethod("CompileShader") );

  // Ensure no request sent to platform abstraction
  DALI_TEST_CHECK( ! application.GetPlatform().WasCalled(TestPlatformAbstraction::SaveResourceFunc ) );
}

static void UtcDaliInternalLoadShaderRequest02()
{
  TestApplication application;
  tet_infoline("Testing LoadShader() failure");
  testTicketObserver.Reset();

  // Clear through all of the outstanding shader load requests from the default shader effect
  std::vector< unsigned char > buffer;
  for( int i=0; i<10; i++ )
  {
    buffer.push_back((unsigned char)i);
  }
  application.GetPlatform().SetLoadFileResult( true, buffer );
  application.GetGlAbstraction().SetLinkStatus(1);
  application.SendNotification(); // Flush update messages
  application.Render();           // Process load shader request (immediately)
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::SceneGraph::UpdateManager& updateManager = Internal::ThreadLocalStorage::Get().GetUpdateManager();

  Integration::ShaderResourceType shaderRequest(123, "vertex src", "frag src");
  std::string shaderBinaryFile("shader.bin");
  Internal::ResourceTicketPtr ticket = resourceClient.LoadShader(shaderRequest, shaderBinaryFile);
  ticket->AddObserver(testTicketObserver);

  ShaderEffect::GeometryHints hints = ShaderEffect::HINT_NONE;
  Internal::SceneGraph::Shader* sceneObject = new Internal::SceneGraph::Shader( hints );
  AddShaderMessage( updateManager, *sceneObject );

  size_t shaderHash=0;
  SetShaderProgramMessage( updateManager, *sceneObject, GEOMETRY_TYPE_IMAGE, Internal::SHADER_DEFAULT, ticket->GetId(), shaderHash );

  DALI_TEST_CHECK( ticket );

  buffer.clear();
  DALI_TEST_CHECK(buffer.size() == 0);

  application.GetPlatform().SetLoadFileResult( true, buffer );
  application.GetGlAbstraction().ResetShaderCallStack();
  application.GetGlAbstraction().EnableShaderCallTrace( true );
  application.GetGlAbstraction().SetLinkStatus(1);
  application.GetGlAbstraction().SetProgramBinaryLength(20);
  application.GetPlatform().SetResourceSaved (ticket->GetId(), Integration::ResourceShader );

  application.SendNotification(); // Flush update messages
  application.Render();           // Process load shader request (immediately), add responses
                                  // to post process q
  application.Render();           // this update will process old post-process-q
  application.Render();           // this update will process new post-process-q
  application.SendNotification(); // Send save request to event thread
  application.Render();           // this update will process save request
  application.Render();           // this update will get SaveComplete
  application.SendNotification(); // Send save request response

  TraceCallStack& shaderTrace = application.GetGlAbstraction().GetShaderTrace();
  DALI_TEST_CHECK( shaderTrace.FindMethod("CompileShader") );

  DALI_TEST_CHECK( application.GetPlatform().WasCalled(TestPlatformAbstraction::SaveResourceFunc ) );
  DALI_TEST_CHECK( testTicketObserver.SaveSucceededCalled() );
}

static void UtcDaliInternalAllocateBitmapImage01()
{
  TestApplication application;
  tet_infoline("Testing AllocateBitmap()");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ImageTicketPtr imageTicket = resourceClient.AllocateBitmapImage(80, 80, 80, 80, Pixel::RGB565);
  imageTicket->AddObserver( testTicketObserver );

  DALI_TEST_CHECK( imageTicket );
  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoading, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  application.SendNotification(); // Flush update queue
  application.Render(0); // Process message
  application.SendNotification(); // Send message to tickets

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(), 80, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 80, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::RGB565, TEST_LOCATION );
  DALI_TEST_CHECK( testTicketObserver.LoadSucceededCalled() == 1 );

  Integration::Bitmap* bitmap = resourceClient.GetBitmap(imageTicket);
  DALI_TEST_CHECK ( bitmap != NULL );
  DALI_TEST_EQUALS ( bitmap->GetImageWidth(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetImageHeight(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferWidth(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferHeight(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPixelFormat(), Pixel::RGB565, TEST_LOCATION );
}

static void UtcDaliInternalAddBitmapImage01()
{
  TestApplication application;
  tet_infoline("Testing AddBitmap with reserved buffer()");
  testTicketObserver.Reset();
  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,  false  );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGB565, 80, 80, 80, 80 );

  Internal::ImageTicketPtr imageTicket = resourceClient.AddBitmapImage( bitmap );
  DALI_TEST_CHECK( imageTicket );
  imageTicket->AddObserver( testTicketObserver );

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoading, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  application.SendNotification(); // Flush update queue
  application.Render(0); // Process message
  application.SendNotification(); // Send message to tickets

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(), 80, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 80, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::RGB565, TEST_LOCATION );

  Integration::Bitmap* theBitmap = resourceClient.GetBitmap(imageTicket);
  DALI_TEST_CHECK ( theBitmap != NULL );
  DALI_TEST_CHECK ( bitmap == theBitmap );
  DALI_TEST_EQUALS ( bitmap->GetImageWidth(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetImageHeight(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferWidth(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferHeight(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPixelFormat(), Pixel::RGB565, TEST_LOCATION );
}

static void UtcDaliInternalAddBitmapImage02()
{
  TestApplication application;
  tet_infoline("Testing AddBitmap without reserved buffer()");
  testTicketObserver.Reset();
  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,  false  );

  Internal::ImageTicketPtr imageTicket = resourceClient.AddBitmapImage( bitmap );
  DALI_TEST_CHECK( imageTicket );
  imageTicket->AddObserver( testTicketObserver );

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoading, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  application.SendNotification(); // Flush update queue
  application.Render(0); // Process message
  application.SendNotification(); // Send message to tickets

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  Integration::Bitmap* theBitmap = resourceClient.GetBitmap(imageTicket);
  DALI_TEST_CHECK ( theBitmap != NULL );
  DALI_TEST_CHECK ( bitmap == theBitmap );
  DALI_TEST_EQUALS ( bitmap->GetImageWidth(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetImageHeight(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferWidth(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferHeight(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  // There is no way for the ticket's image attributes to be updated if the bitmap
  // reserves a buffer after ticket generation.
  // Probably not an issue - there is no public API in BufferImage to change the image size.
}


static void UtcDaliInternalAddBitmapImage03()
{
  TestApplication application;
  tet_infoline("Testing AddBitmap() with invalid bitmap");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ImageTicketPtr imageTicket;
  bool exceptionRaised=false;
  try
  {
    imageTicket = resourceClient.AddBitmapImage( NULL );
  }
  catch( DaliException& e )
  {
    exceptionRaised = true;
  }
  DALI_TEST_CHECK( exceptionRaised );
  DALI_TEST_CHECK( ! imageTicket );
}

static void UtcDaliInternalGetBitmapImage01()
{
  TestApplication application;
  tet_infoline("Testing GetBitmap() with valid ticket");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS,  false  );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 20, 20, 80, 80 );
  Internal::ImageTicketPtr imageTicket = resourceClient.AddBitmapImage( bitmap );

  Integration::Bitmap* theBitmap = resourceClient.GetBitmap(imageTicket);
  DALI_TEST_CHECK ( theBitmap != NULL );
  DALI_TEST_CHECK ( bitmap == theBitmap );
  DALI_TEST_EQUALS ( bitmap->GetImageWidth(),   20u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetImageHeight(),  20u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferWidth(),  80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferHeight(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  imageTicket->AddObserver( testTicketObserver );
  application.SendNotification(); // Flush update queue
  application.Render(0);          // Process message
  application.SendNotification(); // Send message to tickets

  theBitmap = resourceClient.GetBitmap(imageTicket);
  DALI_TEST_CHECK ( theBitmap != NULL );
  DALI_TEST_CHECK ( bitmap == theBitmap );
  DALI_TEST_EQUALS ( bitmap->GetImageWidth(),   20u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetImageHeight(),  20u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferWidth(),  80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPackedPixelsProfile()->GetBufferHeight(), 80u, TEST_LOCATION );
  DALI_TEST_EQUALS ( bitmap->GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );
}

static void UtcDaliInternalGetBitmapImage02()
{
  TestApplication application;
  tet_infoline("Testing GetBitmap() with invalid ticket");

  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ImageTicketPtr imageTicket;
  Integration::Bitmap* theBitmap = NULL;
  bool exceptionRaised = false;
  try
  {
    theBitmap = resourceClient.GetBitmap(imageTicket);
  } catch (DaliException& e)
  {
    exceptionRaised = true;
  }
  DALI_TEST_CHECK( exceptionRaised );
  DALI_TEST_CHECK( ! theBitmap );
}

static void UtcDaliInternalGetBitmapImage03()
{
  TestApplication application;
  tet_infoline("Testing GetBitmap() with valid ticket for incorrect type");

  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();  Internal::ResourceTicketPtr imageTicket = CheckLoadBitmap( application, "Stuff.png", 100, 100 );
  Internal::ResourceTicketPtr modelTicket = CheckLoadModel( application, "Stuff.dae");

  Integration::Bitmap* theBitmap = NULL;
  theBitmap = resourceClient.GetBitmap(imageTicket);
  DALI_TEST_CHECK( ! theBitmap );

  theBitmap = resourceClient.GetBitmap(modelTicket);
  DALI_TEST_CHECK( ! theBitmap );
}

static void UtcDaliInternalAllocateTexture01()
{
  TestApplication application;
  tet_infoline("Testing AllocateTexture()");
  testTicketObserver.Reset();

  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ResourceTicketPtr resourceTicket = resourceClient.AllocateTexture(80, 80, Pixel::L8 );
  resourceTicket->AddObserver( testTicketObserver );

  DALI_TEST_CHECK( resourceTicket );
  DALI_TEST_EQUALS ( resourceTicket->GetLoadingState(), ResourceLoading, TEST_LOCATION );

  application.SendNotification(); // Flush update queue
  application.Render(0); // Process message
  application.SendNotification(); // Send message to tickets

  DALI_TEST_EQUALS ( resourceTicket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
  DALI_TEST_CHECK( testTicketObserver.LoadSucceededCalled() == 1 );

}

static void UtcDaliInternalAddNativeImage()
{
  TestApplication application;
  tet_infoline("Testing AddNativeImage()");

  testTicketObserver.Reset();
  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ImageTicketPtr imageTicket;
  { // Test image going out of scope after ticket creation (message to Update thread holds a ref)
    TestNativeImagePointer nativeImage = TestNativeImage::New( 80, 80 );
    Internal::ResourceTicketPtr ticket = resourceClient.AddNativeImage( *nativeImage );
    imageTicket = dynamic_cast<Internal::ImageTicket*>(ticket.Get());
    DALI_TEST_CHECK( imageTicket );
    imageTicket->AddObserver( testTicketObserver );
  }

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoading, TEST_LOCATION );

  application.SendNotification(); // Flush update queue
  application.Render(0); // Process message
  application.SendNotification(); // Send message to tickets

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(),  80, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 80, TEST_LOCATION );

  Integration::Bitmap* theBitmap = NULL;
  theBitmap = resourceClient.GetBitmap(imageTicket);

  DALI_TEST_CHECK ( theBitmap == NULL );
}

static void UtcDaliInternalAddFrameBufferImage()
{
  TestApplication application;
  tet_infoline("Testing AddFrameBufferImage()");

  testTicketObserver.Reset();
  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::ImageTicketPtr imageTicket = resourceClient.AddFrameBufferImage(80, 80, Pixel::A8 );
  DALI_TEST_CHECK( imageTicket );
  imageTicket->AddObserver( testTicketObserver );

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoading, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(),  0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 0, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION );

  application.SendNotification(); // Flush update queue
  application.Render(0); // Process message
  application.SendNotification(); // Send message to tickets

  DALI_TEST_EQUALS ( imageTicket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetWidth(), 80, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetHeight(), 80, TEST_LOCATION );
  DALI_TEST_EQUALS ( imageTicket->GetAttributes().GetPixelFormat(), Pixel::A8, TEST_LOCATION );

  Integration::Bitmap* theBitmap = NULL;
  theBitmap = resourceClient.GetBitmap(imageTicket);
  DALI_TEST_CHECK ( theBitmap == NULL );
}

static void UtcDaliInternalAllocateMesh01()
{
  TestApplication application;
  tet_infoline("Testing AllocateMesh() with vald mesh data");

  MeshData publicMeshData;
  MeshData::VertexContainer    vertices;
  MeshData::FaceIndices        faces;
  BoneContainer                bones;
  ConstructVertices(vertices, 60);
  ConstructFaces(vertices, faces);
  Material customMaterial = ConstructMaterial();
  publicMeshData.SetData(vertices, faces, bones, customMaterial);
  publicMeshData.SetHasNormals(true);
  publicMeshData.SetHasTextureCoords(true);

  testTicketObserver.Reset();
  Internal::ResourceClient& resourceClient  = Internal::ThreadLocalStorage::Get().GetResourceClient();
  Internal::OwnerPointer<Internal::MeshData> meshDataPtr( new Internal::MeshData( publicMeshData, true, true ) );
  Internal::ResourceTicketPtr meshTicket = resourceClient.AllocateMesh(meshDataPtr);
  DALI_TEST_CHECK( meshTicket );
  meshTicket->AddObserver( testTicketObserver );

  DALI_TEST_EQUALS ( meshTicket->GetLoadingState(), ResourceLoading, TEST_LOCATION );

  application.SendNotification(); // Flush update queue
  application.Render(0); // Process message
  application.SendNotification(); // Send message to tickets

  DALI_TEST_EQUALS ( meshTicket->GetLoadingState(), ResourceLoadingSucceeded, TEST_LOCATION );
}
