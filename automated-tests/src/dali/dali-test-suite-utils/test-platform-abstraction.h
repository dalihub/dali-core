#ifndef __DALI_TEST_PLATFORM_ABSTRACTION_H__
#define __DALI_TEST_PLATFORM_ABSTRACTION_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include "test-trace-call-stack.h"

namespace Dali
{

/**
 * Concrete implementation of the platform abstraction class.
 */
class DALI_IMPORT_API TestPlatformAbstraction : public Dali::Integration::PlatformAbstraction
{

public:

  /**
   * Constructor
   */
  TestPlatformAbstraction();

  /**
   * Destructor
   */
  virtual ~TestPlatformAbstraction();

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual ImageDimensions GetClosestImageSize( const std::string& filename,
                                                 ImageDimensions size,
                                                 FittingMode::Type fittingMode,
                                                 SamplingMode::Type samplingMode,
                                                 bool orientationCorrection );

  /**
   * @copydoc PlatformAbstraction::GetClosestImageSize()
   */
  virtual ImageDimensions GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                               ImageDimensions size,
                                               FittingMode::Type fittingMode,
                                               SamplingMode::Type samplingMode,
                                               bool orientationCorrection );

  /**
   * @copydoc PlatformAbstraction::LoadResourceSynchronously()
   */
  virtual Integration::ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath );

  /**
   * @copydoc PlatformAbstraction::DecodeBuffer()
   */
  virtual Integration::BitmapPtr DecodeBuffer( const Dali::Integration::ResourceType& resourceType, uint8_t * buffer, size_t size );

  /**
   * @copydoc PlatformAbstraction::LoadShaderBinaryFile()
   */
  virtual bool LoadShaderBinaryFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::SaveShaderBinaryFile()
   */
  virtual bool SaveShaderBinaryFile( const std::string& filename, const unsigned char * buffer, unsigned int numBytes ) const { return true; }


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

  inline void EnableTrace(bool enable) { mTrace.Enable(enable); }
  inline void ResetTrace() { mTrace.Reset(); }
  inline TraceCallStack& GetTrace() { return mTrace; }

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
  void SetLoadFileResult( bool result, Dali::Vector< unsigned char >& buffer );

  /**
   * @brief Sets the SaveFile result
   * @param[in] result The value that SaveFile should return
   */
  void SetSaveFileResult( bool result );

  /**
   * @brief Sets the resource loaded by LoadResourceSynchronously
   * @param[in] resource The loaded resource
   */
  void SetSynchronouslyLoadedResource( Integration::ResourcePointer resource );

  /**
   * @brief Sets the bitmap returned by DecodeBuffer()
   * @param[in] bitmap The decoded bitmap
   */
  void SetDecodedBitmap( Integration::BitmapPtr bitmap );

private:

  TestPlatformAbstraction( const TestPlatformAbstraction& ); ///< Undefined
  TestPlatformAbstraction& operator=( const TestPlatformAbstraction& ); ///< Undefined

private:

  struct LoadFileResult
  {
    inline LoadFileResult()
    : loadResult(false)
    {

    }

    bool loadResult;
    Dali::Vector< unsigned char> buffer;
  };

  mutable TraceCallStack        mTrace;
  bool                          mIsLoadingResult;
  Vector2                       mSize;
  Vector2                       mClosestSize;

  LoadFileResult                mLoadFileResult;
  bool                          mSaveFileResult;

  Integration::ResourcePointer  mSynchronouslyLoadedResource;
  Integration::BitmapPtr        mDecodedBitmap;
};

} // Dali

#endif /* __DALI_TET_PLATFORM_ABSTRACTION_H__ */
