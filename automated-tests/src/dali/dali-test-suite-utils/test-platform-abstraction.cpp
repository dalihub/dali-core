/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include "test-trace-call-stack.h"

namespace Dali
{

TestPlatformAbstraction::TestPlatformAbstraction()
: mTrace(),
  mIsLoadingResult( false ),
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

ImageDimensions TestPlatformAbstraction::GetClosestImageSize( const std::string& filename,
                                                              ImageDimensions size,
                                                              FittingMode::Type fittingMode,
                                                              SamplingMode::Type samplingMode,
                                                              bool orientationCorrection )
{
  ImageDimensions closestSize = ImageDimensions( mClosestSize );
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["filename"] = Dali::Any(filename);
  namedParams["size"] = Dali::Any(size);
  namedParams["fittingMode"] = Dali::Any(fittingMode);
  namedParams["samplingMode"] = Dali::Any(samplingMode);
  namedParams["orientationCorrection"] = Dali::Any(orientationCorrection);
  mTrace.PushCall("GetClosestImageSize", namedParams);
  return closestSize;
}

ImageDimensions TestPlatformAbstraction::GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                                   ImageDimensions size,
                                                   FittingMode::Type fittingMode,
                                                   SamplingMode::Type samplingMode,
                                                   bool orientationCorrection )
{
  ImageDimensions closestSize = ImageDimensions( mClosestSize );
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["size"] = Dali::Any(size);
  namedParams["fittingMode"] = Dali::Any(int(fittingMode));
  namedParams["samplingMode"] = Dali::Any(int(samplingMode));
  namedParams["orientationCorrection"] = Dali::Any(orientationCorrection);
  mTrace.PushCall("GetClosestImageSize", namedParams);
  return closestSize;
}

Integration::ResourcePointer TestPlatformAbstraction::LoadImageSynchronously( const Integration::BitmapResourceType& resourceType, const std::string& resourcePath )
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["resourcePath"] = Dali::Any(resourcePath);
  mTrace.PushCall("LoadResourceSynchronously", namedParams);
  return mSynchronouslyLoadedResource;
}

Integration::BitmapPtr TestPlatformAbstraction::DecodeBuffer( const Integration::BitmapResourceType& resourceType, uint8_t * buffer, size_t size )
{
  mTrace.PushCall("DecodeBuffer");
  return mDecodedBitmap;
}

bool TestPlatformAbstraction::LoadShaderBinaryFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["filename"]=Dali::Any(filename);
  mTrace.PushCall("LoadShaderBinaryFile", namedParams);
  if( mLoadFileResult.loadResult )
  {
    buffer = mLoadFileResult.buffer;
  }

  return mLoadFileResult.loadResult;
}


/** Call this every test */
void TestPlatformAbstraction::Initialize()
{
  mTrace.Reset();
  mTrace.Enable(true);
  mIsLoadingResult=false;
  mSynchronouslyLoadedResource.Reset();
  mDecodedBitmap.Reset();
}

bool TestPlatformAbstraction::WasCalled(TestFuncEnum func)
{
  switch(func)
  {
    case LoadResourceSynchronouslyFunc:       return mTrace.FindMethod("LoadResourceSynchronously");
    case LoadShaderBinaryFileFunc:            return mTrace.FindMethod("LoadShaderBinaryFile");
    case SaveShaderBinaryFileFunc:            return mTrace.FindMethod("SaveShaderBinaryFile");
  }
  return false;
}

void TestPlatformAbstraction::SetIsLoadingResult(bool result)
{
  mIsLoadingResult = result;
}

void TestPlatformAbstraction::ClearReadyResources()
{
  mSynchronouslyLoadedResource.Reset();
  mDecodedBitmap.Reset();
}

void TestPlatformAbstraction::SetClosestImageSize( const Vector2& size )
{
  mClosestSize = ImageDimensions( static_cast<uint32_t>( size.x ), static_cast<uint32_t>( size.y ) );
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

void TestPlatformAbstraction::SetDecodedBitmap( Integration::BitmapPtr bitmap )
{
  mDecodedBitmap = bitmap;
}

} // namespace Dali
