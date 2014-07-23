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

/**
 * Constructor
 */
TestPlatformAbstraction::TestPlatformAbstraction()
: mRequest(0)
{
  Initialize();
}

/**
 * Destructor
 */
TestPlatformAbstraction::~TestPlatformAbstraction()
{
}

/**
 * @copydoc PlatformAbstraction::GetTimeMicroseconds()
 */
void TestPlatformAbstraction::GetTimeMicroseconds(unsigned int &seconds, unsigned int &microSeconds)
{
  seconds = mSeconds;
  microSeconds = mMicroSeconds;
  mTrace.PushCall("GetTimeMicroseconds", "");
}

/**
 * @copydoc PlatformAbstraction::Suspend()
 */
void TestPlatformAbstraction::Suspend()
{
  mTrace.PushCall("Suspend", "");
}

/**
 * @copydoc PlatformAbstraction::Resume()
 */
void TestPlatformAbstraction::Resume()
{
  mTrace.PushCall("Resume", "");
}

void TestPlatformAbstraction::GetClosestImageSize( const std::string& filename,
                                                   const ImageAttributes& attributes,
                                                   Vector2& closestSize)
{
  closestSize = mClosestSize;
  mTrace.PushCall("GetClosestImageSize", "");
}

void TestPlatformAbstraction::GetClosestImageSize( Integration::ResourcePointer resourceBuffer,
                                                   const ImageAttributes& attributes,
                                                   Vector2& closestSize)
{
  closestSize = mClosestSize;
  mTrace.PushCall("GetClosestImageSize", "");
}


/**
 * @copydoc PlatformAbstraction::LoadResource()
 */
void TestPlatformAbstraction::LoadResource(const Integration::ResourceRequest& request)
{
  std::ostringstream out;
  out << "Type:";
  if( request.GetType()->id == Integration::ResourceText )
  {
    out << "Text";
  }
  else
  {
    out << request.GetType()->id;
  }
  out << ", Path: " << request.GetPath() << std::endl ;

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

/**
 * @copydoc PlatformAbstraction::SaveResource()
 */
void TestPlatformAbstraction::SaveResource(const Integration::ResourceRequest& request)
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
void TestPlatformAbstraction::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  mTrace.PushCall("CancelLoad", "");
}

/**
 * @copydoc PlatformAbstraction::GetResources()
 */
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
bool TestPlatformAbstraction::IsLoading()
{
  mTrace.PushCall("IsLoading", "");
  return mIsLoadingResult;
}

/**
 * @copydoc PlatformAbstraction::GetDefaultFontFamily()
 */
const std::string& TestPlatformAbstraction::GetDefaultFontFamily() const
{
  mTrace.PushCall("GetDefaultFontFamily", "");
  return mGetDefaultFontFamilyResult;
}

/**
 * @copydoc PlatformAbstraction::GetDefaultFontSize()
 */
float TestPlatformAbstraction::GetDefaultFontSize() const
{
  mTrace.PushCall("GetDefaultFontSize", "");
  return mGetDefaultFontSizeResult;
}

PixelSize TestPlatformAbstraction::GetFontLineHeightFromCapsHeight(const std::string& fontFamily, const std::string& fontStyle, CapsHeight capsHeight) const
{
  mTrace.PushCall("GetFontLineHeightFromCapsHeight", "");
  // LineHeight will be bigger than CapsHeight, so return capsHeight + 1
  return PixelSize(capsHeight + 1);
}

/**
 * @copydoc PlatformAbstraction::GetGlyphData()
 */

Integration::GlyphSet* TestPlatformAbstraction::GetGlyphData ( const Integration::TextResourceType& textRequest,
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
        PixelBuffer* pixelBuffer = bitmapData->GetBuffer();
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

Integration::GlyphSet* TestPlatformAbstraction::GetCachedGlyphData( const Integration::TextResourceType& textRequest,
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
      PixelBuffer* pixelBuffer = bitmapData->GetBuffer();
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
void TestPlatformAbstraction::GetGlobalMetrics( const std::string& fontFamily,
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
std::string TestPlatformAbstraction::GetFontPath(const std::string& family, bool bold, bool italic) const
{
  mTrace.PushCall("GetFontPath", "");
  return mGetFontPathResult;

  // Do nothing with arguments
}

/**
 * @copydoc PlatformAbstraction::SetDpi()
 */
void TestPlatformAbstraction::SetDpi (unsigned int dpiHorizontal, unsigned int dpiVertical)
{
  mTrace.PushCall("SetDpi", "");
}

/**
 * @copydoc PlatformAbstraction::GetFontFamilyForChars()
 */
const std::string& TestPlatformAbstraction::GetFontFamilyForChars(const TextArray& charsRequested) const
{
  mTrace.PushCall("GetFontFamilyForChars", "");
  return mGetDefaultFontFamilyResult;
}

/**
 * @copydoc PlatformAbstraction::AllGlyphsSupported()
 */
bool TestPlatformAbstraction::AllGlyphsSupported(const std::string& name, const std::string& fontStyle, const TextArray& text) const
{
  mTrace.PushCall("AllGlyphsSupported", "");
  return true;
}

/**
 * @copydoc PlatformAbstraction::ValidateFontFamilyName()
 */
bool TestPlatformAbstraction::ValidateFontFamilyName(const std::string& fontFamily, const std::string& fontStyle, bool& isDefaultSystemFont, std::string& closestMatch, std::string& closestStyleMatch) const
{
  mTrace.PushCall("ValidateFontFamilyName", "");
  return true;
}

/**
 * @copydoc PlatformAbstraction::GetFontList()
 */
void TestPlatformAbstraction::GetFontList( PlatformAbstraction::FontListMode mode, std::vector<std::string>& fonstList ) const
{
  mFontListMode = mode;
  mTrace.PushCall("ValidateGetFontList", "");
}

/**
 * @copydoc PlatformAbstraction::LoadFile()
 */
bool TestPlatformAbstraction::LoadFile( const std::string& filename, std::vector< unsigned char >& buffer ) const
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
bool TestPlatformAbstraction::SaveFile(const std::string& filename, std::vector< unsigned char >& buffer) const
{
  mTrace.PushCall("SaveFile", "");
  return false;
}

void TestPlatformAbstraction::JoinLoaderThreads()
{
  mTrace.PushCall("JoinLoaderThreads", "");
}

void TestPlatformAbstraction::UpdateDefaultsFromDevice()
{
  mTrace.PushCall("UpdateDefaultsFromDevice", "");
  mGetDefaultFontFamilyResult+=1.0f;
}

Integration::DynamicsFactory* TestPlatformAbstraction::GetDynamicsFactory()
{
  mTrace.PushCall("GetDynamicsFactory", "");
  return NULL;
}

bool TestPlatformAbstraction::ReadGlobalMetricsFromCache( const std::string& fontFamily,
                                                          const std::string& fontStyle,
                                                          Integration::GlobalMetrics& globalMetrics )
{
  mTrace.PushCall("ReadGlobalMetricsFromCacheFile", "");
  globalMetrics = mReadGlobalMetrics; // Want to copy contents...
  return mReadGlobalMetricsResult; // Default false (will be set to true on subsequent write)
}

void TestPlatformAbstraction::WriteGlobalMetricsToCache( const std::string& fontFamily,
                                                         const std::string& fontStyle,
                                                         const Integration::GlobalMetrics& globalMetrics )
{
  // Copy so next read uses written values. TODO: Could add method
  // to turn this behaviour off for more extensive testing.
  mReadGlobalMetrics = globalMetrics;
  mReadGlobalMetricsResult = true;

  mTrace.PushCall("WriteGlobalMetricsToCacheFile", "");
}

bool TestPlatformAbstraction::ReadMetricsFromCache( const std::string& fontFamily,
                                                    const std::string& fontStyle,
                                                    std::vector<Integration::GlyphMetrics>& glyphMetricsContainer )
{
  mTrace.PushCall("ReadMetricsFromCacheFile", "");
  glyphMetricsContainer = mReadMetrics;
  return mReadMetricsResult; // Default false (will be set to true on subsequent write)
}

void TestPlatformAbstraction::WriteMetricsToCache( const std::string& fontFamily,
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


void TestPlatformAbstraction::GetFileNamesFromDirectory( const std::string& directoryName,
                                                         std::vector<std::string>& fileNames )
{
  fileNames.push_back( std::string( "u1f004.png" ) );
  fileNames.push_back( std::string( "u1f0cf.png" ) );
  fileNames.push_back( std::string( "u1f170.png" ) );
  fileNames.push_back( std::string( "u1f601.png" ) );
}


Integration::BitmapPtr TestPlatformAbstraction::GetGlyphImage( const std::string& fontFamily, const std::string& fontStyle, float fontSize, uint32_t character ) const
{
  Integration::BitmapPtr image = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, true );
  image->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 1, 1 );

  mTrace.PushCall("GetGlyphImage", "");

  return image;
}


/** Call this every test */
void TestPlatformAbstraction::Initialize()
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


bool TestPlatformAbstraction::WasCalled(TestFuncEnum func)
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

void TestPlatformAbstraction::SetGetTimeMicrosecondsResult(size_t sec, size_t usec)
{
  mSeconds = sec;
  mMicroSeconds = usec;
}

void TestPlatformAbstraction::IncrementGetTimeResult(size_t milliseconds)
{
  mMicroSeconds += milliseconds * 1000u;
  unsigned int additionalSeconds = mMicroSeconds / 1000000u;

  mSeconds += additionalSeconds;
  mMicroSeconds -= additionalSeconds * 1000000u;
}

void TestPlatformAbstraction::SetIsLoadingResult(bool result)
{
  mIsLoadingResult = result;
}

void TestPlatformAbstraction::SetGetDefaultFontFamilyResult(std::string result)
{
  mGetDefaultFontFamilyResult = result;
}

void TestPlatformAbstraction::SetGetDefaultFontSizeResult(float result)
{
  mGetDefaultFontSizeResult = result;
}

void TestPlatformAbstraction::SetGetFontPathResult(std::string& result)
{
  mGetFontPathResult = result;
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

void TestPlatformAbstraction::SetResourceSaved(Integration::ResourceId      savedId,
                                               Integration::ResourceTypeId  savedType)
{
  mResources.saved = true;
  mResources.savedId = savedId;
  mResources.savedType = savedType;
}

void TestPlatformAbstraction::SetResourceSaveFailed(Integration::ResourceId  id,
                                                    Integration::ResourceFailure failure)
{
  mResources.saveFailed = true;
  mResources.saveFailedId = id;
  mResources.saveFailure = failure;
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

void TestPlatformAbstraction::SetLoadFileResult( bool result, std::vector< unsigned char >& buffer )
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

Integration::PlatformAbstraction::FontListMode TestPlatformAbstraction::GetLastFontListMode( )
{
  return mFontListMode;
}

void TestPlatformAbstraction::SetReadGlobalMetricsResult( bool success, Integration::GlobalMetrics& globalMetrics )
{
  mReadGlobalMetricsResult = success;
  mReadGlobalMetrics = globalMetrics;
}

void TestPlatformAbstraction::SetReadMetricsResult( bool success, std::vector<Integration::GlyphMetrics>& glyphMetricsContainer )
{
  mReadMetricsResult = success;
  mReadMetrics = glyphMetricsContainer; // copy
}

} // namespace Dali
