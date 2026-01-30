/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/bitmap.h>
#include <algorithm>

#include "dali-test-suite-utils.h"

namespace Dali
{
TestPlatformAbstraction::TestPlatformAbstraction()
: mIsLoadingResult(false),
  mClosestSize(),
  mLoadFileResult(),
  mSaveFileResult(false),
  mSynchronouslyLoadedResource(),
  mTimerId(0),
  mTimerPairsWaiting()
{
  Initialize();
}

TestPlatformAbstraction::~TestPlatformAbstraction()
{
}

ImageDimensions TestPlatformAbstraction::GetClosestImageSize(const std::string& filename,
                                                             ImageDimensions    size,
                                                             FittingMode::Type  fittingMode,
                                                             SamplingMode::Type samplingMode,
                                                             bool               orientationCorrection)
{
  ImageDimensions closestSize = ImageDimensions(mClosestSize);
  mTrace.PushCall("GetClosestImageSize", "");
  return closestSize;
}

ImageDimensions TestPlatformAbstraction::GetClosestImageSize(Integration::ResourcePointer resourceBuffer,
                                                             ImageDimensions              size,
                                                             FittingMode::Type            fittingMode,
                                                             SamplingMode::Type           samplingMode,
                                                             bool                         orientationCorrection)
{
  ImageDimensions closestSize = ImageDimensions(mClosestSize);
  mTrace.PushCall("GetClosestImageSize", "");
  return closestSize;
}

Integration::ResourcePointer TestPlatformAbstraction::LoadImageSynchronously(const Integration::BitmapResourceType& resourceType, const std::string& resourcePath)
{
  mTrace.PushCall("LoadResourceSynchronously", "");
  return mSynchronouslyLoadedResource;
}

Integration::BitmapPtr TestPlatformAbstraction::DecodeBuffer(const Integration::BitmapResourceType& resourceType, uint8_t* buffer, size_t size)
{
  mTrace.PushCall("DecodeBuffer", "");
  return mDecodedBitmap;
}

bool TestPlatformAbstraction::LoadShaderBinaryFile(const std::string& filename, Dali::Vector<unsigned char>& buffer) const
{
  mTrace.PushCall("LoadShaderBinaryFile", "");
  if(mLoadFileResult.loadResult)
  {
    buffer = mLoadFileResult.buffer;
  }

  return mLoadFileResult.loadResult;
}

bool TestPlatformAbstraction::SaveShaderBinaryFile(const std::string& filename, const unsigned char* buffer, unsigned int numBytes) const
{
  mTrace.PushCall("SaveShaderBinaryFile", "");

  return mSaveFileResult;
}

/** Call this every test */
void TestPlatformAbstraction::Initialize()
{
  mTrace.Reset();
  mTrace.Enable(true);
  mIsLoadingResult = false;
  mSynchronouslyLoadedResource.Reset();
  mDecodedBitmap.Reset();

  mTimerId = 0;
  mTimerPairsWaiting.clear();
}

bool TestPlatformAbstraction::WasCalled(TestFuncEnum func)
{
  switch(func)
  {
    case LoadResourceSynchronouslyFunc:
      return mTrace.FindMethod("LoadResourceSynchronously");
    case LoadShaderBinaryFileFunc:
      return mTrace.FindMethod("LoadShaderBinaryFile");
    case SaveShaderBinaryFileFunc:
      return mTrace.FindMethod("SaveShaderBinaryFile");
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

void TestPlatformAbstraction::SetClosestImageSize(const Vector2& size)
{
  mClosestSize = ImageDimensions(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
}

void TestPlatformAbstraction::SetLoadFileResult(bool result, Dali::Vector<unsigned char>& buffer)
{
  mLoadFileResult.loadResult = result;
  if(result)
  {
    mLoadFileResult.buffer = buffer;
  }
}

void TestPlatformAbstraction::SetSaveFileResult(bool result)
{
  mSaveFileResult = result;
}

void TestPlatformAbstraction::SetSynchronouslyLoadedResource(Integration::ResourcePointer resource)
{
  mSynchronouslyLoadedResource = resource;
}

void TestPlatformAbstraction::SetDecodedBitmap(Integration::BitmapPtr bitmap)
{
  mDecodedBitmap = bitmap;
}

uint32_t TestPlatformAbstraction::StartTimer(uint32_t milliseconds, CallbackBase* callback)
{
  mTimerId++;
  mTimerPairsWaiting.push_back(std::make_pair(mTimerId, std::unique_ptr<CallbackBase>(callback)));
  return mTimerId;
}

void TestPlatformAbstraction::TriggerTimer()
{
  // Copy the callbacks first, to allow to remove timer during execution
  std::vector<std::pair<uint32_t, CallbackBase*>> timerPairs;
  for(auto& pair : mTimerPairsWaiting)
  {
    timerPairs.push_back(std::make_pair(pair.first, pair.second.get()));
  }

  for(auto& pair : timerPairs)
  {
    bool valid = false;
    for(auto& originalPair : mTimerPairsWaiting)
    {
      if(originalPair.first == pair.first)
      {
        valid = true;
        break;
      }
    }

    if(valid)
    {
      if(pair.second != nullptr)
      {
        CallbackBase::Execute(*pair.second);
      }

      // Remove from the original list
      mTimerPairsWaiting.erase(std::remove_if(mTimerPairsWaiting.begin(), mTimerPairsWaiting.end(),
                                              [&](const std::pair<uint32_t, std::unique_ptr<CallbackBase>>& originalPair)
      { return originalPair.first == pair.first; }),
                               mTimerPairsWaiting.end());
    }
  }
}

void TestPlatformAbstraction::CancelTimer(uint32_t timerId)
{
  mTimerPairsWaiting.erase(std::remove_if(mTimerPairsWaiting.begin(), mTimerPairsWaiting.end(),
                                          [&](const std::pair<uint32_t, std::unique_ptr<CallbackBase>>& originalPair)
  { return originalPair.first == timerId; }),
                           mTimerPairsWaiting.end());
}

} // namespace Dali
