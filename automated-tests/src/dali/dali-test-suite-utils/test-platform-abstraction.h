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
#include <dali/public-api/common/set-wrapper.h>
#include <dali/integration-api/platform-abstraction.h>

#include "test-trace-call-stack.h"
#include "test-dynamics.h"

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

    bool                         saved;
    Integration::ResourceId      savedId;
    Integration::ResourceTypeId  savedType;

    bool                         saveFailed;
    Integration::ResourceId      saveFailedId;
    Integration::ResourceFailure saveFailure;
  };

  struct LoadFileResult
  {
    inline LoadFileResult()
    : loadResult(false)
    {

    }

    bool loadResult;
    std::vector< unsigned char> buffer;
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
   * @copydoc PlatformAbstraction::GetTimeMicroseconds()
   */
  virtual void GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds);

  /**
   * @copydoc PlatformAbstraction::Suspend()
   */
  virtual void Suspend();

  /**
   * @copydoc PlatformAbstraction::Resume()
   */
  virtual void Resume();

  virtual ImageDimensions GetClosestImageSize( const std::string& filename,
                                                 ImageDimensions size,
                                                 FittingMode::Type fittingMode,
                                                 SamplingMode::Type samplingMode,
                                                 bool orientationCorrection );

  virtual ImageDimensions GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                               ImageDimensions size,
                                               FittingMode::Type fittingMode,
                                               SamplingMode::Type samplingMode,
                                               bool orientationCorrection );

  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  virtual void LoadResource(const Integration::ResourceRequest& request);

  virtual Integration::ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath );

  /**
   * @copydoc PlatformAbstraction::SaveResource()
   */
  virtual void SaveResource(const Integration::ResourceRequest& request);

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
   * @copydoc PlatformAbstraction::GetDefaultFontFamily()
   */
  virtual void GetDefaultFontDescription( std::string& family, std::string& style ) const;

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
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::LoadShaderBinFile()
   */
  virtual bool LoadShaderBinFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::SaveFile()
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const;

  virtual void JoinLoaderThreads();

  virtual Integration::DynamicsFactory* GetDynamicsFactory();

public: // TEST FUNCTIONS

  // Enumeration of Platform Abstraction methods
  typedef enum
  {
    GetTimeMicrosecondsFunc,
    SuspendFunc,
    ResumeFunc,
    LoadResourceFunc,
    SaveResourceFunc,
    SaveFileFunc,
    LoadFileFunc,
    CancelLoadFunc,
    GetResourcesFunc,
    IsLoadingFunc,
    SetDpiFunc,
    JoinLoaderThreadsFunc,
    GetDynamicsFactoryFunc,
  } TestFuncEnum;

  /** Call this every test */
  void Initialize();

  inline void EnableTrace(bool enable) { mTrace.Enable(enable); }
  inline void ResetTrace() { mTrace.Reset(); }
  inline TraceCallStack& GetTrace() { return mTrace; }

  bool WasCalled(TestFuncEnum func);

  void SetGetTimeMicrosecondsResult(size_t sec, size_t usec);

  void IncrementGetTimeResult(size_t milliseconds);

  void SetIsLoadingResult(bool result);

  void ClearReadyResources();

  void SetResourceLoaded(Integration::ResourceId  loadedId,
                         Integration::ResourceTypeId  loadedType,
                         Integration::ResourcePointer loadedResource);

  void SetResourceLoadFailed(Integration::ResourceId  id,
                             Integration::ResourceFailure failure);

  void SetResourceSaved(Integration::ResourceId      savedId,
                        Integration::ResourceTypeId  savedType);

  void SetResourceSaveFailed(Integration::ResourceId  id,
                             Integration::ResourceFailure failure);

  Integration::ResourceRequest* GetRequest();

  void DiscardRequest();

  void SetClosestImageSize(const Vector2& size);

  void SetLoadFileResult( bool result, std::vector< unsigned char >& buffer );

  void SetSaveFileResult( bool result );

private:
  mutable TraceCallStack        mTrace;
  size_t                        mSeconds;
  size_t                        mMicroSeconds;
  bool                          mIsLoadingResult;
  std::string                   mGetDefaultFontFamilyResult;
  std::string                   mGetDefaultFontStyleResult;
  int                           mGetDefaultFontSizeResult;
  Resources                     mResources;
  Integration::ResourceRequest* mRequest;
  Vector2                       mSize;
  Vector2                       mClosestSize;

  LoadFileResult                mLoadFileResult;
  bool                          mSaveFileResult;
  TestDynamicsFactory*          mDynamicsFactory;
};

} // Dali

#endif /* __DALI_TET_PLATFORM_ABSTRACTION_H__ */
