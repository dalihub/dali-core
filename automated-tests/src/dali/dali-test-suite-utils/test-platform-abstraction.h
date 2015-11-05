#ifndef __DALI_TEST_PLATFORM_ABSTRACTION_H__
#define __DALI_TEST_PLATFORM_ABSTRACTION_H__

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

// EXTERNAL INCLUDES
#include <stdint.h>
#include <cstring>
#include <string>

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

  struct Resources
  {
    bool                         loaded;
    Integration::ResourceId      loadedId;
    Integration::ResourceTypeId  loadedType;
    Integration::ResourcePointer loadedResource;

    bool                         loadFailed;
    Integration::ResourceId      loadFailedId;
    Integration::ResourceFailure loadFailure;
  };

  struct LoadFileResult
  {
    inline LoadFileResult()
    : loadResult(false)
    {

    }

    bool loadResult;
    Dali::Vector< unsigned char> buffer;
  };

  /**
   * Constructor
   */
  TestPlatformAbstraction();

  /**
   * Destructor
   */
  virtual ~TestPlatformAbstraction();

  /**
   * @copydoc PlatformAbstraction::GetTimeNanoseconds()
   */
  virtual void GetTimeNanoseconds( uint64_t& seconds, uint64_t& nanoseconds );

  /**
   * @copydoc PlatformAbstraction::Suspend()
   */
  virtual void Suspend();

  /**
   * @copydoc PlatformAbstraction::Resume()
   */
  virtual void Resume();

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
   * @copydoc PlatformAbstraction::LoadResource()
   */
  virtual void LoadResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::LoadResourceSynchronously()
   */
  virtual Integration::ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath );

  /**
   * @copydoc PlatformAbstraction::DecodeBuffer()
   */
  virtual Integration::BitmapPtr DecodeBuffer( const Dali::Integration::ResourceType& resourceType, uint8_t * buffer, size_t size );

  /**
   * @copydoc PlatformAbstraction::CancelLoad()
   */
  virtual void CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId);

  /**
   * @copydoc PlatformAbstraction::GetResources()
   */
  virtual void GetResources(Integration::ResourceCache& cache);

  /**
   * @copydoc PlatformAbstraction::IsLoading()
   */
  virtual bool IsLoading();

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontSize()
   */
  virtual int GetDefaultFontSize() const;

  /**
   * @copydoc PlatformAbstraction::SetDpi()
   */
  virtual void SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical);

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  virtual bool LoadFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::LoadShaderBinaryFile()
   */
  virtual bool LoadShaderBinaryFile( const std::string& filename, Dali::Vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::SaveFile()
   */
  virtual bool SaveFile(const std::string& filename, const unsigned char * buffer, unsigned int numBytes ) const;

 /**
  * @copydoc PlatformAbstraction::SaveShaderBinaryFile()
  */
  virtual bool SaveShaderBinaryFile( const std::string& filename, const unsigned char * buffer, unsigned int numBytes ) const { return true; }

  virtual void JoinLoaderThreads();

public: // TEST FUNCTIONS

  // Enumeration of Platform Abstraction methods
  typedef enum
  {
    GetTimeNanosecondsFunc,
    SuspendFunc,
    ResumeFunc,
    LoadResourceFunc,
    SaveFileFunc,
    LoadFileFunc,
    LoadShaderBinaryFileFunc,
    SaveShaderBinaryFileFunc,
    CancelLoadFunc,
    GetResourcesFunc,
    IsLoadingFunc,
    SetDpiFunc,
    JoinLoaderThreadsFunc,
  } TestFuncEnum;

  /** Call this every test */
  void Initialize();

  inline void EnableTrace(bool enable) { mTrace.Enable(enable); }
  inline void ResetTrace() { mTrace.Reset(); }
  inline TraceCallStack& GetTrace() { return mTrace; }

  bool WasCalled(TestFuncEnum func);

  void SetGetTimeNanosecondsResult(size_t sec, size_t nsec);

  void IncrementGetTimeResult(size_t milliseconds);

  void SetIsLoadingResult(bool result);

  void SetGetDefaultFontSizeResult(float result);

  void ClearReadyResources();

  void SetResourceLoaded(Integration::ResourceId  loadedId,
                         Integration::ResourceTypeId  loadedType,
                         Integration::ResourcePointer loadedResource);

  void SetResourceLoadFailed(Integration::ResourceId  id,
                             Integration::ResourceFailure failure);

  Integration::ResourceRequest* GetRequest();

  void DiscardRequest();

  void SetClosestImageSize(const Vector2& size);

  void SetLoadFileResult( bool result, Dali::Vector< unsigned char >& buffer );

  void SetSaveFileResult( bool result );

private:
  mutable TraceCallStack        mTrace;
  uint64_t                      mSeconds;
  uint64_t                      mNanoSeconds;
  bool                          mIsLoadingResult;
  int                           mGetDefaultFontSizeResult;
  Resources                     mResources;
  Integration::ResourceRequest* mRequest;
  Vector2                       mSize;
  Vector2                       mClosestSize;

  LoadFileResult                mLoadFileResult;
  bool                          mSaveFileResult;
};

} // Dali

#endif /* __DALI_TET_PLATFORM_ABSTRACTION_H__ */
