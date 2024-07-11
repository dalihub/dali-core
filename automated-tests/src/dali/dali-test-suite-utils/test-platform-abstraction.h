#ifndef DALI_TEST_PLATFORM_ABSTRACTION_H
#define DALI_TEST_PLATFORM_ABSTRACTION_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <stdint.h>

#include <cstring>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/math/vector2.h>

#include "test-trace-call-stack.h"

namespace Dali
{
/**
 * Concrete implementation of the platform abstraction class.
 */
class DALI_CORE_API TestPlatformAbstraction : public Dali::Integration::PlatformAbstraction
{
public:
  /**
   * Constructor
   */
  TestPlatformAbstraction();

  /**
   * Destructor
   */
  ~TestPlatformAbstraction() override;

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  ImageDimensions GetClosestImageSize(const std::string& filename,
                                      ImageDimensions    size,
                                      FittingMode::Type  fittingMode,
                                      SamplingMode::Type samplingMode,
                                      bool               orientationCorrection) override;

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  ImageDimensions GetClosestImageSize(Integration::ResourcePointer resourceBuffer,
                                      ImageDimensions              size,
                                      FittingMode::Type            fittingMode,
                                      SamplingMode::Type           samplingMode,
                                      bool                         orientationCorrection) override;

  /**
   * @copydoc PlatformAbstraction::LoadResourceSynchronously()
   */
  Integration::ResourcePointer LoadImageSynchronously(const Integration::BitmapResourceType& resourceType, const std::string& resourcePath) override;

  /**
   * @copydoc PlatformAbstraction::DecodeBuffer()
   */
  Integration::BitmapPtr DecodeBuffer(const Dali::Integration::BitmapResourceType& resourceType, uint8_t* buffer, size_t size) override;

  /**
   * @copydoc PlatformAbstraction::LoadShaderBinaryFile()
   */
  bool LoadShaderBinaryFile(const std::string& filename, Dali::Vector<unsigned char>& buffer) const override;

  /**
   * @copydoc PlatformAbstraction::SaveShaderBinaryFile()
   */
  bool SaveShaderBinaryFile(const std::string& filename, const unsigned char* buffer, unsigned int numBytes) const override;

  /**
   * @copydoc PlatformAbstraction::StartTimer()
   */
  uint32_t StartTimer(uint32_t milliseconds, CallbackBase* callback) override;

  /*
   * @copydoc PlatformAbstraction::CancelTimer()
   */
  void CancelTimer(uint32_t timerId) override;

public: // TEST FUNCTIONS
  // Enumeration of Platform Abstraction methods
  typedef enum
  {
    LoadResourceSynchronouslyFunc,
    LoadShaderBinaryFileFunc,
    SaveShaderBinaryFileFunc
  } TestFuncEnum;

  /** Call this every test */
  void Initialize();

  inline void EnableTrace(bool enable)
  {
    mTrace.Enable(enable);
  }
  inline void ResetTrace()
  {
    mTrace.Reset();
  }
  inline TraceCallStack& GetTrace()
  {
    return mTrace;
  }

  /**
   * @brief Checks if a platform function was called
   * @param[in] func The function to check
   * @return true if the function was called
   */
  bool WasCalled(TestFuncEnum func);

  /**
   * @brief Sets the result to return when IsLoading is called by Core.
   * @param[in] result The result to set.
   */
  void SetIsLoadingResult(bool result);

  /**
   * @brief Clears all resource queues
   */
  void ClearReadyResources();

  /**
   * @brief Sets the value returned by GetClosestImageSize.
   * @param[in] size The size that should be returned.
   */
  void SetClosestImageSize(const Vector2& size);

  /**
   * @brief Sets the result return by LoadFile.
   * @param[in] result The value that LoadFile should return.
   * @param[in] buffer The buffer of the loaded file.
   */
  void SetLoadFileResult(bool result, Dali::Vector<unsigned char>& buffer);

  /**
   * @brief Sets the SaveFile result
   * @param[in] result The value that SaveFile should return
   */
  void SetSaveFileResult(bool result);

  /**
   * @brief Sets the resource loaded by LoadResourceSynchronously
   * @param[in] resource The loaded resource
   */
  void SetSynchronouslyLoadedResource(Integration::ResourcePointer resource);

  /**
   * @brief Sets the bitmap returned by DecodeBuffer()
   * @param[in] bitmap The decoded bitmap
   */
  void SetDecodedBitmap(Integration::BitmapPtr bitmap);

  /**
   * @brief Triggers the previously stored callback function
   */
  void TriggerTimer();

private:
  TestPlatformAbstraction(const TestPlatformAbstraction&);            ///< Undefined
  TestPlatformAbstraction& operator=(const TestPlatformAbstraction&); ///< Undefined

private:
  struct LoadFileResult
  {
    inline LoadFileResult()
    : loadResult(false)
    {
    }

    bool                        loadResult;
    Dali::Vector<unsigned char> buffer;
  };

  mutable TraceCallStack mTrace{true, "PlatformAbstraction::"};
  bool                   mIsLoadingResult;
  ImageDimensions        mClosestSize;

  LoadFileResult mLoadFileResult;
  bool           mSaveFileResult;

  Integration::ResourcePointer mSynchronouslyLoadedResource;
  Integration::BitmapPtr       mDecodedBitmap;

  uint32_t      mTimerId;
  CallbackBase* mCallbackFunction;
};

} // namespace Dali

#endif /* DALI_TEST_PLATFORM_ABSTRACTION_H */
