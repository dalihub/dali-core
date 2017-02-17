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

  typedef Vector< Integration::ResourceRequest* > ResourceRequestContainer;

  /**
   * Constructor
   */
  TestPlatformAbstraction();

  /**
   * Destructor
   */
  virtual ~TestPlatformAbstraction();

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
    SuspendFunc,
    ResumeFunc,
    LoadResourceFunc,
    LoadResourceSynchronouslyFunc,
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
   * @brief Sets the value returned by GetDefaultFontSize
   * @param[in] result The value to return
   */
  void SetGetDefaultFontSizeResult(float result);

  /**
   * @brief Clears all resource queues
   */
  void ClearReadyResources();

  /**
   * @brief Sets a particular resource request as loaded.
   * @param[in] loadedId The ResourceID of the resource that has been loaded.
   * @param[in] loadedType The type of resource that has been loaded.
   * @param[in] loadedResource A pointer to the resource that has been loaded.
   */
  void SetResourceLoaded(Integration::ResourceId  loadedId,
                         Integration::ResourceTypeId  loadedType,
                         Integration::ResourcePointer loadedResource);

  /**
   * @brief Sets a particular resource request as load failure.
   * @param[in] id The ID of the failed resource request.
   * @param[in] failure The type of failure.
   */
  void SetResourceLoadFailed(Integration::ResourceId  id,
                             Integration::ResourceFailure failure);

  /**
   * @brief Retrieves the latest resource request
   * @return A pointer to the latest resource request.
   */
  Integration::ResourceRequest* GetRequest();

  /**
   * @brief Retrieves a reference to a container of all the resource requests.
   * @return A reference to a container of all the resource requests.
   */
  const ResourceRequestContainer& GetAllRequests() const;

  /**
   * @brief Sets all resource requests as loaded.
   */
  void SetAllResourceRequestsAsLoaded();

  /**
   * @brief Sets all resource requests as loaded.
   * @param[in] failure The failure type
   */
  void SetAllResourceRequestsAsFailed( Integration::ResourceFailure failure );

  /**
   * @brief Discards all current resource requests.
   */
  void DiscardRequest();

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

  struct LoadedResource
  {
    Integration::ResourceId      id;
    Integration::ResourceTypeId  type;
    Integration::ResourcePointer resource;
  };

  struct FailedLoad
  {
    Integration::ResourceId      id;
    Integration::ResourceFailure failure;
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

  typedef std::vector< LoadedResource > LoadedResourceContainer;
  typedef std::vector< FailedLoad > FailedLoadContainer;

  mutable TraceCallStack        mTrace;
  bool                          mIsLoadingResult;
  int                           mGetDefaultFontSizeResult;
  LoadedResourceContainer       mLoadedResourcesQueue;
  FailedLoadContainer           mFailedLoadQueue;
  ResourceRequestContainer      mResourceRequests;
  Vector2                       mSize;
  Vector2                       mClosestSize;

  LoadFileResult                mLoadFileResult;
  bool                          mSaveFileResult;

  Integration::ResourcePointer  mSynchronouslyLoadedResource;
  Integration::BitmapPtr        mDecodedBitmap;
};

} // Dali

#endif /* __DALI_TET_PLATFORM_ABSTRACTION_H__ */
