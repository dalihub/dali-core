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

#include "test-platform-abstraction.h"
#include "dali-test-suite-utils.h"
#include <dali/integration-api/bitmap.h>

namespace Dali
{

TestPlatformAbstraction::TestPlatformAbstraction()
: mTrace(),
  mIsLoadingResult( false ),
  mGetDefaultFontSizeResult( 0 ),
  mLoadedResourcesQueue(),
  mFailedLoadQueue(),
  mResourceRequests(),
  mSize(),
  mClosestSize(),
  mLoadFileResult(),
  mSaveFileResult( false ),
  mSynchronouslyLoadedResource()
{
  Initialize();
}

TestPlatformAbstraction::~TestPlatformAbstraction()
{
}

void TestPlatformAbstraction::Suspend()
{
  mTrace.PushCall("Suspend", "");
}

void TestPlatformAbstraction::Resume()
{
  mTrace.PushCall("Resume", "");
}

ImageDimensions TestPlatformAbstraction::GetClosestImageSize( const std::string& filename,
                                                              ImageDimensions size,
                                                              FittingMode::Type fittingMode,
                                                              SamplingMode::Type samplingMode,
                                                              bool orientationCorrection )
{
  ImageDimensions closestSize = ImageDimensions( mClosestSize.x, mClosestSize.y );
  mTrace.PushCall("GetClosestImageSize", "");
  return closestSize;
}

ImageDimensions TestPlatformAbstraction::GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                                   ImageDimensions size,
                                                   FittingMode::Type fittingMode,
                                                   SamplingMode::Type samplingMode,
                                                   bool orientationCorrection )
{
  ImageDimensions closestSize = ImageDimensions( mClosestSize.x, mClosestSize.y );
  mTrace.PushCall("GetClosestImageSize", "");
  return closestSize;
}

void TestPlatformAbstraction::LoadResource(const Integration::ResourceRequest& request)
{
  std::ostringstream out;
  out << "Type:" << request.GetType()->id << ", Path: " << request.GetPath() << std::endl ;

  mTrace.PushCall("LoadResource", out.str());

  mResourceRequests.PushBack( new Integration::ResourceRequest(request) );
}

Integration::ResourcePointer TestPlatformAbstraction::LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath )
{
  mTrace.PushCall("LoadResourceSynchronously", "");
  return mSynchronouslyLoadedResource;
}

Integration::BitmapPtr TestPlatformAbstraction::DecodeBuffer( const Integration::ResourceType& resourceType, uint8_t * buffer, size_t size )
{
  mTrace.PushCall("DecodeBuffer", "");
  return Integration::BitmapPtr();
}

void TestPlatformAbstraction::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  mTrace.PushCall("CancelLoad", "");
}

void TestPlatformAbstraction::GetResources(Integration::ResourceCache& cache)
{
  mTrace.PushCall("GetResources", "");

  while( !mLoadedResourcesQueue.empty() )
  {
    LoadedResource loaded( *mLoadedResourcesQueue.begin() );
    mLoadedResourcesQueue.erase( mLoadedResourcesQueue.begin() );
    cache.LoadResponse( loaded.id, loaded.type, loaded.resource, Integration::RESOURCE_COMPLETELY_LOADED );
  }

  // iterate through the resources which failed to load
  while( !mFailedLoadQueue.empty() )
  {
    FailedLoad failed( *mFailedLoadQueue.begin() );
    mFailedLoadQueue.erase( mFailedLoadQueue.begin() );
    cache.LoadFailed( failed.id, failed.failure );
  }
}

bool TestPlatformAbstraction::IsLoading()
{
  mTrace.PushCall("IsLoading", "");
  return mIsLoadingResult;
}

int TestPlatformAbstraction::GetDefaultFontSize() const
{
  mTrace.PushCall("GetDefaultFontSize", "");
  return mGetDefaultFontSizeResult;
}

void TestPlatformAbstraction::SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical)
{
  mTrace.PushCall("SetDpi", "");
}

bool TestPlatformAbstraction::LoadFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const
{
  mTrace.PushCall("LoadFile", "");
  if( mLoadFileResult.loadResult )
  {
    buffer = mLoadFileResult.buffer;
  }

  return mLoadFileResult.loadResult;
}

bool TestPlatformAbstraction::LoadShaderBinaryFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const
{
  mTrace.PushCall("LoadShaderBinaryFile", "");
  if( mLoadFileResult.loadResult )
  {
    buffer = mLoadFileResult.buffer;
  }

  return mLoadFileResult.loadResult;
}

bool TestPlatformAbstraction::SaveFile(const std::string& filename, const unsigned char * buffer, unsigned int numBytes ) const
{
  mTrace.PushCall("SaveFile", "");
  return false;
}

void TestPlatformAbstraction::JoinLoaderThreads()
{
  mTrace.PushCall("JoinLoaderThreads", "");
}

/** Call this every test */
void TestPlatformAbstraction::Initialize()
{
  mTrace.Reset();
  mTrace.Enable(true);
  mLoadedResourcesQueue.clear();
  mFailedLoadQueue.clear();
  mResourceRequests.Clear();
  mIsLoadingResult=false;
  mSynchronouslyLoadedResource.Reset();
}

bool TestPlatformAbstraction::WasCalled(TestFuncEnum func)
{
  switch(func)
  {
    case SuspendFunc:                         return mTrace.FindMethod("Suspend");
    case ResumeFunc:                          return mTrace.FindMethod("Resume");
    case LoadResourceFunc:                    return mTrace.FindMethod("LoadResource");
    case LoadResourceSynchronouslyFunc:       return mTrace.FindMethod("LoadResourceSynchronously");
    case LoadFileFunc:                        return mTrace.FindMethod("LoadFile");
    case LoadShaderBinaryFileFunc:            return mTrace.FindMethod("LoadShaderBinaryFile");
    case SaveShaderBinaryFileFunc:            return mTrace.FindMethod("SaveShaderBinaryFile");
    case SaveFileFunc:                        return mTrace.FindMethod("SaveFile");
    case CancelLoadFunc:                      return mTrace.FindMethod("CancelLoad");
    case GetResourcesFunc:                    return mTrace.FindMethod("GetResources");
    case IsLoadingFunc:                       return mTrace.FindMethod("IsLoading");
    case SetDpiFunc:                          return mTrace.FindMethod("SetDpi");
    case JoinLoaderThreadsFunc:               return mTrace.FindMethod("JoinLoaderThreads");
  }
  return false;
}

void TestPlatformAbstraction::SetIsLoadingResult(bool result)
{
  mIsLoadingResult = result;
}

void TestPlatformAbstraction::ClearReadyResources()
{
  mLoadedResourcesQueue.clear();
  mFailedLoadQueue.clear();
  mSynchronouslyLoadedResource.Reset();
}

void TestPlatformAbstraction::SetResourceLoaded(Integration::ResourceId  loadedId,
                                                Integration::ResourceTypeId  loadedType,
                                                Integration::ResourcePointer loadedResource)
{
  LoadedResource loadedInfo;
  loadedInfo.id = loadedId;
  loadedInfo.type = loadedType;
  loadedInfo.resource = loadedResource;
  mLoadedResourcesQueue.push_back( loadedInfo );
}

void TestPlatformAbstraction::SetResourceLoadFailed(Integration::ResourceId  id,
                                                    Integration::ResourceFailure failure)
{
  FailedLoad failedInfo;
  failedInfo.id = id;
  failedInfo.failure = failure;
  mFailedLoadQueue.push_back( failedInfo );
}

Integration::ResourceRequest* TestPlatformAbstraction::GetRequest()
{
  Integration::ResourceRequest* request = NULL;

  // Return last request
  if( ! mResourceRequests.Empty() )
  {
    request = *( mResourceRequests.End() - 1 );
  }

  return request;
}

const TestPlatformAbstraction::ResourceRequestContainer& TestPlatformAbstraction::GetAllRequests() const
{
  return mResourceRequests;
}

void TestPlatformAbstraction::SetAllResourceRequestsAsLoaded()
{
  for( ResourceRequestContainer::Iterator iter = mResourceRequests.Begin(), endIter = mResourceRequests.End();
       iter != endIter; ++iter )
  {
    Integration::ResourceRequest* request = *iter;
    Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD );
    Integration::ResourcePointer resource(bitmap);
    bitmap->GetPackedPixelsProfile()->ReserveBuffer(Pixel::RGBA8888, 80, 80, 80, 80);
    SetResourceLoaded( request->GetId(), request->GetType()->id, resource );
  }
  mResourceRequests.Clear();
}

void TestPlatformAbstraction::SetAllResourceRequestsAsFailed( Integration::ResourceFailure failure )
{
  for( ResourceRequestContainer::Iterator iter = mResourceRequests.Begin(), endIter = mResourceRequests.End();
       iter != endIter; ++iter )
  {
    SetResourceLoadFailed( (*iter)->GetId(), failure);
  }
  mResourceRequests.Clear();
}

void TestPlatformAbstraction::DiscardRequest()
{
  mResourceRequests.Clear();
}

void TestPlatformAbstraction::SetClosestImageSize(const Vector2& size)
{
  mClosestSize = size;
}

void TestPlatformAbstraction::SetLoadFileResult( bool result, Dali::Vector< unsigned char >& buffer )
{
  mLoadFileResult.loadResult = result;
  if( result )
  {
    mLoadFileResult.buffer = buffer;
  }
}

void TestPlatformAbstraction::SetSaveFileResult( bool result )
{
  mSaveFileResult = result;
}

void TestPlatformAbstraction::SetSynchronouslyLoadedResource( Integration::ResourcePointer resource )
{
  mSynchronouslyLoadedResource = resource;
}

} // namespace Dali
