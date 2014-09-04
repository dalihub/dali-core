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

// INTERNAL INCLUDES
#include <dali/public-api/common/set-wrapper.h>
#include <dali/integration-api/platform-abstraction.h>

#include <dali/integration-api/glyph-set.h>
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

  virtual void GetClosestImageSize( const std::string& filename,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize);

  virtual void GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize);

  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  virtual void LoadResource(const Integration::ResourceRequest& request);

  /**
   * @copydoc PlatformAbstraction::LoadResourceSynchronously()
   */
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
  virtual const std::string& GetDefaultFontFamily() const;

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontSize()
   */
  virtual float GetDefaultFontSize() const;

  /**
   * @copydoc PlatformAbstraction::GetFontLineHeightFromCapsHeight()
   */
  virtual Dali::PixelSize GetFontLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, CapsHeight capsHeight) const;

  /**
   * @copydoc PlatformAbstraction::GetGlyphData()
   */
  virtual Integration::GlyphSet* GetGlyphData ( const Integration::TextResourceType& textRequest,
                                                const std::string& fontFamily,
                                                bool getBitmap) const;

  /**
   * @copydoc PlatformAbstraction::GetCachedGlyphData()
   */
  virtual Integration::GlyphSet* GetCachedGlyphData( const Integration::TextResourceType& textRequest,
                                                     const std::string& fontFamily ) const;


  /**
   * @copydoc PlatformAbstraction::GetGlobalMetrics()
   */
  virtual void GetGlobalMetrics( const std::string& fontFamily,
                                 const std::string& fontStyle,
                                 Integration::GlobalMetrics& globalMetrics ) const;

  /**
   * @copydoc PlatformAbstraction::GetFontPath()
   */
  virtual std::string GetFontPath(const std::string& family, bool bold, bool italic) const;

  /**
   * @copydoc PlatformAbstraction::SetDpi()
   */
  virtual void SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical);

  /**
   * @copydoc PlatformAbstraction::GetFontFamilyForChars()
   */
  virtual const std::string& GetFontFamilyForChars(const Integration::TextArray& charsRequested) const;

  /**
   * @copydoc PlatformAbstraction::AllGlyphsSupported()
   */
  virtual bool AllGlyphsSupported(const std::string& name, const std::string& fontStyle, const Integration::TextArray& text) const;

  /**
   * @copydoc PlatformAbstraction::ValidateFontFamilyName()
   */
  virtual bool ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestMatch, std::string& closestStyleMatch) const;

  /**
   * @copydoc PlatformAbstraction::GetFontList()
   */
  virtual void GetFontList( PlatformAbstraction::FontListMode mode, std::vector<std::string>& fontList ) const;

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const;

  /**
   * @copydoc PlatformAbstraction::SaveFile()
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const;

  virtual void JoinLoaderThreads();

  virtual void UpdateDefaultsFromDevice();

  virtual Integration::DynamicsFactory* GetDynamicsFactory();

  virtual bool ReadGlobalMetricsFromCache( const std::string& fontFamily,
                                           const std::string& fontStyle,
                                           Integration::GlobalMetrics& globalMetrics );

  virtual void WriteGlobalMetricsToCache( const std::string& fontFamily,
                                          const std::string& fontStyle,
                                          const Integration::GlobalMetrics& globalMetrics );

  virtual bool ReadMetricsFromCache( const std::string& fontFamily,
                                     const std::string& fontStyle,
                                     std::vector<Integration::GlyphMetrics>& glyphMetricsContainer );
  virtual void WriteMetricsToCache( const std::string& fontFamily,
                                    const std::string& fontStyle,
                                    const Integration::GlyphSet& glyphSet );


  virtual void GetFileNamesFromDirectory( const std::string& directoryName,
                                          std::vector<std::string>& fileNames );

  virtual Integration::BitmapPtr GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character ) const;

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
    GetDefaultFontFamilyFunc,
    GetDefaultFontSizeFunc,
    GetFontLineHeightFromCapsHeightFunc,
    GetGlyphDataFunc,
    GetCachedGlyphDataFunc,
    SetDpiFunc,
    GetFontPathFunc,
    JoinLoaderThreadsFunc,
    GetFontFamilyForCharsFunc,
    AllGlyphsSupportedFunc,
    ValidateFontFamilyNameFunc,
    UpdateDefaultsFromDeviceFunc,
    GetDynamicsFactoryFunc,
    ValidateGetFontListFunc,
    ReadGlobalMetricsFromCacheFileFunc,
    WriteGlobalMetricsToCacheFileFunc,
    ReadMetricsFromCacheFileFunc,
    WriteMetricsToCacheFileFunc,
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

  void SetGetDefaultFontFamilyResult(std::string result);

  void SetGetDefaultFontSizeResult(float result);

  void SetGetFontPathResult(std::string& result);

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

  PlatformAbstraction::FontListMode GetLastFontListMode( );

  void SetReadGlobalMetricsResult( bool success, Integration::GlobalMetrics& globalMetrics );

  void SetReadMetricsResult( bool success, std::vector<Integration::GlyphMetrics>& glyphMetricsContainer );


private:
  mutable TraceCallStack        mTrace;
  size_t                        mSeconds;
  size_t                        mMicroSeconds;
  bool                          mIsLoadingResult;
  std::string                   mGetDefaultFontFamilyResult;
  float                         mGetDefaultFontSizeResult;
  std::string                   mGetFontPathResult;
  Resources                     mResources;
  Integration::ResourceRequest* mRequest;
  Vector2                       mSize;
  Vector2                       mClosestSize;
  bool                          mReadGlobalMetricsResult;
  bool                          mReadMetricsResult;
  Integration::GlobalMetrics mReadGlobalMetrics;
  std::vector<Integration::GlyphMetrics> mReadMetrics;

  LoadFileResult                mLoadFileResult;
  bool                          mSaveFileResult;
  mutable FontListMode          mFontListMode;
};

} // Dali

#endif /* __DALI_TET_PLATFORM_ABSTRACTION_H__ */
