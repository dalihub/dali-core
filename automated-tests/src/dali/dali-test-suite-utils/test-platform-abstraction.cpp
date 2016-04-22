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
  mResources(),
  mRequest( NULL ),
  mSize(),
  mClosestSize(),
  mLoadFileResult(),
  mSaveFileResult( false )
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
  if(mRequest != NULL)
  {
    delete mRequest;
    tet_infoline ("Warning: multiple resource requests not handled by Test Suite. You may see unexpected errors");
  }
  mRequest = new Integration::ResourceRequest(request);
}

Integration::ResourcePointer TestPlatformAbstraction::LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath )
{
  mTrace.PushCall("LoadResourceSynchronously", "");
  return mResources.loadedResource;
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

  if(mResources.loaded)
  {
    cache.LoadResponse( mResources.loadedId, mResources.loadedType, mResources.loadedResource, Integration::RESOURCE_COMPLETELY_LOADED );
  }
  if(mResources.loadFailed)
  {
    cache.LoadFailed( mResources.loadFailedId, mResources.loadFailure );
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
  memset(&mResources, 0, sizeof(Resources));
  mIsLoadingResult=false;

  if(mRequest)
  {
    delete mRequest;
    mRequest = 0;
  }
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
  memset(&mResources, 0, sizeof(Resources));
}

void TestPlatformAbstraction::SetResourceLoaded(Integration::ResourceId  loadedId,
                                                Integration::ResourceTypeId  loadedType,
                                                Integration::ResourcePointer loadedResource)
{
  mResources.loaded = true;
  mResources.loadedId = loadedId;
  mResources.loadedType = loadedType;
  mResources.loadedResource = loadedResource;
}

void TestPlatformAbstraction::SetResourceLoadFailed(Integration::ResourceId  id,
                                                    Integration::ResourceFailure failure)
{
  mResources.loadFailed = true;
  mResources.loadFailedId = id;
  mResources.loadFailure = failure;
}

Integration::ResourceRequest* TestPlatformAbstraction::GetRequest()
{
  return mRequest;
}

void TestPlatformAbstraction::DiscardRequest()
{
  delete mRequest;
  mRequest = NULL;
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

} // namespace Dali
