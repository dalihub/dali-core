#ifndef __DALI_TEST_PLATFORM_ABSTRACTION_H__
#define __DALI_TEST_PLATFORM_ABSTRACTION_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES
#include <set>
#include <stdint.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali/integration-api/platform-abstraction.h>

#include <dali/integration-api/glyph-set.h>
#include <dali/integration-api/resource-cache.h>
#include <dali/integration-api/resource-types.h>
#include <test-trace-call-stack.h>

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
    LoadFileResult()
    : loadResult(false)
    {

    }

    bool loadResult;
    std::vector< unsigned char> buffer;
  };


  /**
   * Constructor
   */
  TestPlatformAbstraction() : mRequest(0)
  {
    Initialize();
  }

  /**
   * Destructor
   */
  virtual ~TestPlatformAbstraction()
  {
  }

  /**
   * @copydoc PlatformAbstraction::GetTimeMicroseconds()
   */
  virtual void GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds)
  {
    seconds = mSeconds;
    microSeconds = mMicroSeconds;
    mTrace.PushCall("GetTimeMicroseconds", "");
  }

  /**
   * @copydoc PlatformAbstraction::Suspend()
   */
  virtual void Suspend()
  {
    mTrace.PushCall("Suspend", "");
  }

  /**
   * @copydoc PlatformAbstraction::Resume()
   */
  virtual void Resume()
  {
    mTrace.PushCall("Resume", "");
  }

  virtual void GetClosestImageSize( const std::string& filename,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize)
  {
    closestSize = mClosestSize;
    mTrace.PushCall("GetClosestImageSize", "");
  }

  virtual void GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                    const ImageAttributes& attributes,
                                    Vector2& closestSize)
  {
    closestSize = mClosestSize;
    mTrace.PushCall("GetClosestImageSize", "");
  }


  /**
   * @copydoc PlatformAbstraction::LoadResource()
   */
  virtual void LoadResource(const Integration::ResourceRequest& request)
  {
    mTrace.PushCall("LoadResource", "");
    if(mRequest != NULL)
    {
      delete mRequest;
      tet_infoline ("Warning: multiple resource requests not handled by Test Suite. You may see unexpected errors");
    }
    mRequest = new Integration::ResourceRequest(request);
  }

  virtual Integration::ResourcePointer LoadResourceSynchronously( const Integration::ResourceType& resourceType, const std::string& resourcePath)
  {
    mTrace.PushCall("LoadResourceSynchronously", "");
    return mResources.loadedResource;
  }

  /**
   * @copydoc PlatformAbstraction::SaveResource()
   */
  virtual void SaveResource(const Integration::ResourceRequest& request)
  {
    mTrace.PushCall("SaveResource", "");
    if(mRequest != NULL)
    {
      delete mRequest;
      tet_infoline ("Warning: multiple resource requests not handled by Test Suite. You may see unexpected errors");
    }
    mRequest = new Integration::ResourceRequest(request);
  }

  /**
   * @copydoc PlatformAbstraction::CancelLoad()
   */
  virtual void CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
  {
    mTrace.PushCall("CancelLoad", "");
  }

  /**
   * @copydoc PlatformAbstraction::GetResources()
   */
  virtual void GetResources(Integration::ResourceCache& cache)
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
    if(mResources.saved)
    {
      cache.SaveComplete( mResources.savedId, mResources.savedType );
    }
    if(mResources.saveFailed)
    {
      cache.SaveFailed( mResources.saveFailedId, mResources.saveFailure );
    }
  }

  /**
   * @copydoc PlatformAbstraction::IsLoading()
   */
  virtual bool IsLoading()
  {
    mTrace.PushCall("IsLoading", "");
    return mIsLoadingResult;
  }

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontFamily()
   */
  virtual std::string GetDefaultFontFamily() const
  {
    mTrace.PushCall("GetDefaultFontFamily", "");
    return mGetDefaultFontFamilyResult;
  }

  /**
   * @copydoc PlatformAbstraction::GetDefaultFontSize()
   */
  virtual const float GetDefaultFontSize() const
  {
    mTrace.PushCall("GetDefaultFontSize", "");
    return mGetDefaultFontSizeResult;
  }

  virtual const PixelSize GetFontLineHeightFromCapsHeight(const std::string fontFamily, const std::string& fontStyle, const CapsHeight& capsHeight) const
  {
    mTrace.PushCall("GetFontLineHeightFromCapsHeight", "");
    // LineHeight will be bigger than CapsHeight, so return capsHeight + 1
    return PixelSize(capsHeight + 1);
  }

  /**
   * @copydoc PlatformAbstraction::GetGlyphData()
   */

  virtual Integration::GlyphSet* GetGlyphData ( const Integration::TextResourceType& textRequest,
                                                const std::string& fontFamily,
                                                bool getBitmap) const
  {
    if( getBitmap )
    {
      mTrace.PushCall("GetGlyphData", "getBitmap:true");
    }
    else
    {
      mTrace.PushCall("GetGlyphData", "getBitmap:false");
    }

    // It creates fake metrics for the received characters.

    Integration::GlyphSet* set = new Dali::Integration::GlyphSet();
    Integration::BitmapPtr bitmapData;

    std::set<uint32_t> characters;

    for( Integration::TextResourceType::CharacterList::const_iterator it = textRequest.mCharacterList.begin(), endIt = textRequest.mCharacterList.end(); it != endIt; ++it )
    {
      if( characters.find( it->character ) == characters.end() )
      {
        characters.insert( it->character );
        Integration::GlyphMetrics character = {it->character, Integration::GlyphMetrics::LOW_QUALITY,  10.0f,  10.0f, 9.0f, 1.0f, 10.0f, it->xPosition, it->yPosition };

        if( getBitmap )
        {
          bitmapData = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true);
          bitmapData->GetPackedPixelsProfile()->ReserveBuffer(Pixel::A8, 64, 64);
          Integration::PixelBuffer* pixelBuffer = bitmapData->GetBuffer();
          memset( pixelBuffer, it->character, 64*64 );
        }

        set->AddCharacter(bitmapData, character);
      }
    }

    set->mLineHeight = 10.0f;
    set->mAscender = 9.0f;
    set->mUnitsPerEM = 2048.0f/64.0f;
    set->SetAtlasResourceId( textRequest.mTextureAtlasId );
    set->mFontHash = textRequest.mFontHash;

    return set;
  }

  /**
   * @copydoc PlatformAbstraction::GetCachedGlyphData()
   */

  virtual Integration::GlyphSet* GetCachedGlyphData( const Integration::TextResourceType& textRequest,
                                                     const std::string& fontFamily ) const
  {
    mTrace.PushCall("GetCachedGlyphData", "");

    // It creates fake metrics and bitmap for received numeric characters '0' through '9'.
    Integration::GlyphSet* set = new Dali::Integration::GlyphSet();
    Integration::BitmapPtr bitmapData;

    std::set<uint32_t> characters;

    for( Integration::TextResourceType::CharacterList::const_iterator it = textRequest.mCharacterList.begin(), endIt = textRequest.mCharacterList.end(); it != endIt; ++it )
    {
      if( it->character >= '0' && it->character <= '9' && characters.find( it->character ) == characters.end() )
      {
        characters.insert( it->character );
        Integration::GlyphMetrics character = {it->character, Integration::GlyphMetrics::HIGH_QUALITY,  10.0f,  10.0f, 9.0f, 1.0f, 10.0f, it->xPosition, it->yPosition };

        bitmapData = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true);
        bitmapData->GetPackedPixelsProfile()->ReserveBuffer(Pixel::A8, 64, 64);
        Integration::PixelBuffer* pixelBuffer = bitmapData->GetBuffer();
        memset( pixelBuffer, it->character, 64*64 );
        set->AddCharacter(bitmapData, character);
      }
    }

    set->mLineHeight = 10.0f;
    set->mAscender = 9.0f;
    set->mUnitsPerEM = 2048.0f/64.0f;
    set->SetAtlasResourceId( textRequest.mTextureAtlasId );
    set->mFontHash = textRequest.mFontHash;

    return set;
  }


  /**
   * @copydoc PlatformAbstraction::GetGlobalMetrics()
   */
  virtual void GetGlobalMetrics( const std::string& fontFamily,
                                 const std::string& fontStyle,
                                 Integration::GlobalMetrics& globalMetrics ) const
  {
    globalMetrics.lineHeight = 10.0f;
    globalMetrics.ascender = 9.0f;
    globalMetrics.unitsPerEM = 2048.0f/64.0f;
    globalMetrics.underlineThickness = 2.f;
    globalMetrics.underlinePosition = 1.f;
  }

  /**
   * @copydoc PlatformAbstraction::GetFontPath()
   */
  virtual std::string GetFontPath(const std::string& family, bool bold, bool italic) const
  {
    mTrace.PushCall("GetFontPath", "");
    return mGetFontPathResult;

    // Do nothing with arguments
  }

  /**
   * @copydoc PlatformAbstraction::SetDpi()
   */
  virtual void SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical)
  {
    mTrace.PushCall("SetDpi", "");
  }

  /**
   * @copydoc PlatformAbstraction::GetFontFamilyForChars()
   */
  virtual std::string GetFontFamilyForChars(const TextArray& charsRequested) const
  {
    mTrace.PushCall("GetFontFamilyForChars", "");
    return mGetDefaultFontFamilyResult;
  }

  /**
   * @copydoc PlatformAbstraction::AllGlyphsSupported()
   */
  virtual bool AllGlyphsSupported(const std::string& name, const std::string& fontStyle, const TextArray& text) const
  {
    mTrace.PushCall("AllGlyphsSupported", "");
    return true;
  }

  /**
   * @copydoc PlatformAbstraction::ValidateFontFamilyName()
   */
  virtual bool ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestMatch, std::string& closestStyleMatch) const
  {
    mTrace.PushCall("ValidateFontFamilyName", "");
    return true;
  }

  /**
   * @copydoc PlatformAbstraction::GetFontList()
   */
  virtual std::vector<std::string> GetFontList( FontListMode mode ) const
  {
    mFontListMode = mode;
    mTrace.PushCall("ValidateGetFontList", "");
    std::vector<std::string> fontList;
    return fontList;
  }

  /**
   * @copydoc PlatformAbstraction::LoadFile()
   */
  virtual bool LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const
  {
    mTrace.PushCall("LoadFile", "");
    if( mLoadFileResult.loadResult )
    {
      buffer = mLoadFileResult.buffer;
    }

    return mLoadFileResult.loadResult;
  }

  /**
   * @copydoc PlatformAbstraction::SaveFile()
   */
  virtual bool SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const
  {
    mTrace.PushCall("SaveFile", "");
    return false;
  }

  virtual void JoinLoaderThreads()
  {
    mTrace.PushCall("JoinLoaderThreads", "");
  }

  virtual void UpdateDefaultsFromDevice()
  {
    mTrace.PushCall("UpdateDefaultsFromDevice", "");
    mGetDefaultFontFamilyResult+=1.0f;
  }

  virtual Integration::DynamicsFactory* GetDynamicsFactory()
  {
    mTrace.PushCall("GetDynamicsFactory", "");
    return NULL;
  }

  virtual bool ReadGlobalMetricsFromCache( const std::string& fontFamily,
                                           const std::string& fontStyle,
                                           Integration::GlobalMetrics& globalMetrics )
  {
    mTrace.PushCall("ReadGlobalMetricsFromCacheFile", "");
    globalMetrics = mReadGlobalMetrics; // Want to copy contents...
    return mReadGlobalMetricsResult; // Default false (will be set to true on subsequent write)
  }

  virtual void WriteGlobalMetricsToCache( const std::string& fontFamily,
                                          const std::string& fontStyle,
                                          const Integration::GlobalMetrics& globalMetrics )
  {
    // Copy so next read uses written values. TODO: Could add method
    // to turn this behaviour off for more extensive testing.
    mReadGlobalMetrics = globalMetrics;
    mReadGlobalMetricsResult = true;

    mTrace.PushCall("WriteGlobalMetricsToCacheFile", "");
  }

  virtual bool ReadMetricsFromCache( const std::string& fontFamily,
                                     const std::string& fontStyle,
                                     std::vector<Integration::GlyphMetrics>& glyphMetricsContainer )
  {
    mTrace.PushCall("ReadMetricsFromCacheFile", "");
    glyphMetricsContainer = mReadMetrics;
    return mReadMetricsResult; // Default false (will be set to true on subsequent write)
  }

  virtual void WriteMetricsToCache( const std::string& fontFamily,
                                    const std::string& fontStyle,
                                    const Integration::GlyphSet& glyphSet )
  {
    // Copy so next read uses written values. TODO: Could add method
    // to turn this behaviour off for more extensive testing.
    const Integration::GlyphSet::CharacterList& charList =  glyphSet.GetCharacterList();
    mReadMetrics.clear();
    for(std::size_t i=0, end=charList.size(); i<end; ++i)
    {
      mReadMetrics.push_back(charList[i].second);
    }
    mReadMetricsResult = true;

    mTrace.PushCall("WriteMetricsToCacheFile", "");
  }

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
  void Initialize()
  {
    mTrace.Reset();
    mTrace.Enable(true);
    memset(&mResources, 0, sizeof(Resources));
    memset(&mReadGlobalMetrics, 0, sizeof(Integration::GlobalMetrics));
    mSeconds=0;
    mMicroSeconds=0;
    mIsLoadingResult=false;
    mGetDefaultFontFamilyResult = "HelveticaNeue";
    mGetDefaultFontSizeResult=12.0f;
    mGetFontPathResult="helvetica-12";
    mReadMetricsResult=false;
    mReadGlobalMetricsResult=false;

    if(mRequest)
    {
      delete mRequest;
      mRequest = 0;
    }
  }

  void EnableTrace(bool enable) { mTrace.Enable(enable); }
  void ResetTrace() { mTrace.Reset(); }
  TraceCallStack& GetTrace() { return mTrace; }

  bool WasCalled(TestFuncEnum func)
  {
    switch(func)
    {
      case GetTimeMicrosecondsFunc:             return mTrace.FindMethod("GetTimeMicroseconds");
      case SuspendFunc:                         return mTrace.FindMethod("Suspend");
      case ResumeFunc:                          return mTrace.FindMethod("Resume");
      case LoadResourceFunc:                    return mTrace.FindMethod("LoadResource");
      case SaveResourceFunc:                    return mTrace.FindMethod("SaveResource");
      case LoadFileFunc:                        return mTrace.FindMethod("LoadFile");
      case SaveFileFunc:                        return mTrace.FindMethod("SaveFile");
      case CancelLoadFunc:                      return mTrace.FindMethod("CancelLoad");
      case GetResourcesFunc:                    return mTrace.FindMethod("GetResources");
      case IsLoadingFunc:                       return mTrace.FindMethod("IsLoading");
      case GetDefaultFontFamilyFunc:            return mTrace.FindMethod("GetDefaultFontFamily");
      case GetDefaultFontSizeFunc:              return mTrace.FindMethod("GetDefaultFontSize");
      case GetFontLineHeightFromCapsHeightFunc: return mTrace.FindMethod("GetFontLineHeightFromCapsHeight");
      case GetGlyphDataFunc:                    return mTrace.FindMethod("GetGlyphData");
      case GetCachedGlyphDataFunc:              return mTrace.FindMethod("GetCachedGlyphData");
      case GetFontPathFunc:                     return mTrace.FindMethod("GetFontPath");
      case SetDpiFunc:                          return mTrace.FindMethod("SetDpi");
      case JoinLoaderThreadsFunc:               return mTrace.FindMethod("JoinLoaderThreads");
      case GetFontFamilyForCharsFunc:           return mTrace.FindMethod("GetFontFamilyForChars");
      case AllGlyphsSupportedFunc:              return mTrace.FindMethod("AllGlyphsSupported");
      case ValidateFontFamilyNameFunc:          return mTrace.FindMethod("ValidateFontFamilyName");
      case UpdateDefaultsFromDeviceFunc:        return mTrace.FindMethod("UpdateDefaultsFromDevice");
      case GetDynamicsFactoryFunc:              return mTrace.FindMethod("GetDynamicsFactory");
      case ValidateGetFontListFunc:             return mTrace.FindMethod("ValidateGetFontList");
      case ReadGlobalMetricsFromCacheFileFunc:  return mTrace.FindMethod("ReadGlobalMetricsFromCacheFile");
      case WriteGlobalMetricsToCacheFileFunc:   return mTrace.FindMethod("WriteGlobalMetricsToCacheFile");
      case ReadMetricsFromCacheFileFunc:        return mTrace.FindMethod("ReadMetricsFromCacheFile");
      case WriteMetricsToCacheFileFunc:         return mTrace.FindMethod("WriteMetricsToCacheFile");
    }
    return false;
  }

  void SetGetTimeMicrosecondsResult(size_t sec, size_t usec)
  {
    mSeconds = sec;
    mMicroSeconds = usec;
  }

  void IncrementGetTimeResult(size_t milliseconds)
  {
    mMicroSeconds += milliseconds * 1000u;
    unsigned int additionalSeconds = mMicroSeconds / 1000000u;

    mSeconds += additionalSeconds;
    mMicroSeconds -= additionalSeconds * 1000000u;
  }

  void SetIsLoadingResult(bool result)
  {
    mIsLoadingResult = result;
  }

  void SetGetDefaultFontFamilyResult(std::string result)
  {
    mGetDefaultFontFamilyResult = result;
  }

  void SetGetDefaultFontSizeResult(float result)
  {
    mGetDefaultFontSizeResult = result;
  }

  void SetGetFontPathResult(std::string& result)
  {
    mGetFontPathResult = result;
  }

  void ClearReadyResources()
  {
    memset(&mResources, 0, sizeof(Resources));
  }

  void SetResourceLoaded(Integration::ResourceId  loadedId,
                         Integration::ResourceTypeId  loadedType,
                         Integration::ResourcePointer loadedResource)
  {
    mResources.loaded = true;
    mResources.loadedId = loadedId;
    mResources.loadedType = loadedType;
    mResources.loadedResource = loadedResource;
  }

  void SetResourceLoadFailed(Integration::ResourceId  id,
                             Integration::ResourceFailure failure)
  {
    mResources.loadFailed = true;
    mResources.loadFailedId = id;
    mResources.loadFailure = failure;
  }

  void SetResourceSaved(Integration::ResourceId      savedId,
                        Integration::ResourceTypeId  savedType)
  {
    mResources.saved = true;
    mResources.savedId = savedId;
    mResources.savedType = savedType;
  }

  void SetResourceSaveFailed(Integration::ResourceId  id,
                             Integration::ResourceFailure failure)
  {
    mResources.saveFailed = true;
    mResources.saveFailedId = id;
    mResources.saveFailure = failure;
  }

  Integration::ResourceRequest* GetRequest()
  {
    return mRequest;
  }

  void DiscardRequest()
  {
    delete mRequest;
    mRequest = NULL;
  }

  void SetClosestImageSize(const Vector2& size)
  {
    mClosestSize = size;
  }

  void SetLoadFileResult( bool result, std::vector< unsigned char >& buffer )
  {
    mLoadFileResult.loadResult = result;
    if( result )
    {
      mLoadFileResult.buffer = buffer;
    }
  }

  void SetSaveFileResult( bool result )
  {
    mSaveFileResult = result;
  }

  FontListMode GetLastFontListMode( )
  {
    return mFontListMode;
  }

  void SetReadGlobalMetricsResult( bool success, Integration::GlobalMetrics& globalMetrics )
  {
    mReadGlobalMetricsResult = success;
    mReadGlobalMetrics = globalMetrics;
  }

  void SetReadMetricsResult( bool success, std::vector<Integration::GlyphMetrics>& glyphMetricsContainer )
  {
    mReadMetricsResult = success;
    mReadMetrics = glyphMetricsContainer; // copy
  }

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
