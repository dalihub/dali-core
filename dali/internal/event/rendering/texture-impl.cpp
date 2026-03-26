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

// CLASS HEADER
#include <dali/internal/event/rendering/texture-impl.h> // Dali::Internal::Texture

// INTERNAL INCLUDES
#include <dali/integration-api/render-controller.h>
#include <dali/integration-api/string-utils.h>
#include <dali/internal/common/owner-key-type.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/render/renderers/render-texture-messages.h>
#include <dali/internal/update/manager/update-manager.h>

#if defined(GPU_MEMORY_PROFILE_ENABLED)
#include <dali/public-api/common/vector-wrapper.h>
#include <unordered_set>

struct Dali::Internal::Texture::TextureMemoryInfo
{
  TextureMemoryInfo(const Dali::Integration::TextureContextTypeHint::Type typeHint, const std::string context, const int32_t width, const int32_t height, const uint32_t bytesPerPixel, const uint32_t memorySize = 0);

  ~TextureMemoryInfo();

  void Destroy(bool printLog);

  Dali::Integration::TextureContextTypeHint::Type mTypeHint;

  std::string mContext;
  uint32_t    mBytesPerPixel;
  int32_t     mWidth;
  int32_t     mHeight;
  uint32_t    mMemorySize; //< The memory size of texture
};

namespace
{
std::unordered_set<Dali::Internal::Texture::TextureMemoryInfo*> gTextureMemoryInfoQueue;
uint32_t                                                        gTotalTextureMemory; ///< Total memory used by textures

struct MemoryInfoCollector
{
  MemoryInfoCollector()
  : mTypeName("Unknown")
  {
  }

  MemoryInfoCollector(const char* typeName)
  : mTypeName(typeName)
  {
  }

  void Insert(const Dali::Internal::Texture::TextureMemoryInfo* data)
  {
    mData.push_back(data);
  }

  void PrintAndClear()
  {
    if(mData.empty())
    {
      return;
    }

    // Reorder data
    std::sort(mData.begin(), mData.end(), [](const Dali::Internal::Texture::TextureMemoryInfo*& lhs, const Dali::Internal::Texture::TextureMemoryInfo*& rhs)
    {
      if(lhs->mMemorySize != rhs->mMemorySize)
      {
        return lhs->mMemorySize > rhs->mMemorySize;
      }
      if(lhs->mTypeHint != rhs->mTypeHint)
      {
        return lhs->mTypeHint < rhs->mTypeHint;
      }
      return lhs->mContext < rhs->mContext;
    });

    uint32_t totalTextureMemory = 0u;

    DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
    DALI_LOG_ERROR_NOFN("| Type hint | Content of [ %-20s ]                                                |    size     | bpp | memory(MB) +\n", mTypeName.c_str());
    DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
    for(auto iter = mData.begin(); iter != mData.end(); iter++)
    {
      auto context = (*iter)->mContext;
      if(context.length() > 82)
      {
        context = std::string("...") + context.substr(context.length() - 79, 79);
      }
      DALI_LOG_ERROR_NOFN("| %9d | %82s | %4d x %4d | %3d | %10.3f | \n", static_cast<int32_t>((*iter)->mTypeHint), context.c_str(), (*iter)->mWidth, (*iter)->mHeight, (*iter)->mBytesPerPixel, static_cast<double>((*iter)->mMemorySize) / (1024.0 * 1024.0));

      totalTextureMemory += (*iter)->mMemorySize;
    }
    DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
    DALI_LOG_ERROR_NOFN(" >> Count            : %8d                 \n", mData.size());
    DALI_LOG_ERROR_NOFN(" >> Memory(MB)       : %8.3f            \n", static_cast<double>(totalTextureMemory) / (1024.0 * 1024.0));

    Clear();
  }

  void Clear()
  {
    mData.clear();
  }

private:
  const std::string mTypeName;

  // List of memory info pointer collected for print.
  std::vector<const Dali::Internal::Texture::TextureMemoryInfo*> mData;
};

inline void PrintTotalMemory()
{
  // Collect the result first.
  static MemoryInfoCollector sImageCollector("Image");
  static MemoryInfoCollector sTextCollector("Text");
  static MemoryInfoCollector sDaliEtcCollector("ETC (DALi)");
  static MemoryInfoCollector sOuterEtcCollector("ETC");
  static MemoryInfoCollector sUnknownCollector{};

  for(auto iter = gTextureMemoryInfoQueue.begin(); iter != gTextureMemoryInfoQueue.end(); iter++)
  {
    int32_t collectorType = static_cast<int32_t>((*iter)->mTypeHint) / 1000;
    switch(collectorType)
    {
      case 0: // Unknown/Error (0xxx)
      {
        sUnknownCollector.Insert(*iter);
        break;
      }
      case 1: // Image resources (1xxx)
      {
        sImageCollector.Insert(*iter);
        break;
      }
      case 2: // Text (2xxx)
      {
        sTextCollector.Insert(*iter);
        break;
      }
      case 3: // ETC (3xxx) ~ End of dali(9999)
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      {
        sDaliEtcCollector.Insert(*iter);
        break;
      }
      // Some value inserted. Respect as it is not unknown type.
      default:
      {
        sOuterEtcCollector.Insert(*iter);
        break;
      }
    }
  }
  DALI_LOG_ERROR_NOFN("\n\n"); // For beauty
  DALI_LOG_ERROR_NOFN("## DALI_TEXTURE_MEMORY_PROFILER\n");

  sImageCollector.PrintAndClear();
  sTextCollector.PrintAndClear();
  sDaliEtcCollector.PrintAndClear();
  sOuterEtcCollector.PrintAndClear();
  sUnknownCollector.PrintAndClear();

  DALI_LOG_ERROR_NOFN("+=================================================================================================================================+\n");
  DALI_LOG_ERROR_NOFN(" >> textureCount     : %8d                 \n", gTextureMemoryInfoQueue.size());
  DALI_LOG_ERROR_NOFN(" >> Total Memory(MB) : %8.3f            \n", static_cast<double>(gTotalTextureMemory) / (1024.0 * 1024.0));
  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
  DALI_LOG_ERROR_NOFN("\n\n");
}
} // namespace

Dali::Internal::Texture::TextureMemoryInfo::TextureMemoryInfo(const Dali::Integration::TextureContextTypeHint::Type typeHint, const std::string context, const int32_t width, const int32_t height, const uint32_t bytesPerPixel, const uint32_t memorySize)
: mTypeHint(typeHint),
  mContext(context),
  mBytesPerPixel(bytesPerPixel),
  mWidth(width),
  mHeight(height),
  mMemorySize(memorySize ? memorySize : static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * bytesPerPixel)
{
  gTotalTextureMemory += mMemorySize;
  gTextureMemoryInfoQueue.insert(this);
  PrintTotalMemory();
}

Dali::Internal::Texture::TextureMemoryInfo::~TextureMemoryInfo()
{
  // Should not call Destroy() here, to avoid touch gTextureMemoryInfoQueue at static terminate time.
}

void Dali::Internal::Texture::TextureMemoryInfo::Destroy(bool printLog)
{
  gTotalTextureMemory -= mMemorySize;
  gTextureMemoryInfoQueue.erase(this);
  if(printLog)
  {
    PrintTotalMemory();
  }
}
#endif

namespace Dali
{
namespace Internal
{

TexturePtr Texture::New(TextureType::Type type, Pixel::Format format, uint32_t width, uint32_t height)
{
  constexpr auto max_value = std::numeric_limits<uint16_t>::max();
  DALI_ASSERT_ALWAYS((width < max_value) && (height < max_value) && "Size out of range");
  TexturePtr texture(new Texture(type, format, ImageDimensions(width, height)));
  texture->Initialize();
  return texture;
}

TexturePtr Texture::New(NativeImageInterface& nativeImageInterface)
{
  TexturePtr texture(new Texture(&nativeImageInterface));
  texture->Initialize();
  return texture;
}

TexturePtr Texture::New(TextureType::Type type, uint32_t resourceId)
{
  TexturePtr texture(new Texture(type, resourceId));
  texture->Initialize();
  return texture;
}

Render::TextureKey Texture::GetRenderTextureKey() const
{
  return mTextureKey;
}

Texture::Texture(TextureType::Type type, Pixel::Format format, ImageDimensions size)
: EventThreadServicesHolder(EventThreadServices::Get()),
  mTextureKey{},
  mNativeImage(),
  mSize(size),
  mType(type),
  mFormat(format),
  mResourceId(0u),
  mUseUploadedParameter(mSize.GetWidth() == 0u && mSize.GetHeight() == 0u && mFormat == Pixel::INVALID)
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  ,
  mMemoryInfo()
#endif
{
}

Texture::Texture(NativeImageInterfacePtr nativeImageInterface)
: EventThreadServicesHolder(EventThreadServices::Get()),
  mTextureKey{},
  mNativeImage(nativeImageInterface),
  mSize(nativeImageInterface->GetWidth(), nativeImageInterface->GetHeight()),
  mType(TextureType::TEXTURE_2D),
  mFormat(Pixel::RGB888),
  mResourceId(0u),
  mUseUploadedParameter(false)
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  ,
  mMemoryInfo(new TextureMemoryInfo(static_cast<Dali::Integration::TextureContextTypeHint::Type>(Dali::Integration::TextureContextTypeHint::UNKNOWN + 1), "(NativeImage. Size could be changed)", mSize.GetWidth(), mSize.GetHeight(), 4)) // Most of NativeImage texture use 4 byte per pixels. Let we assume the data size maximize.
#endif
{
}

Texture::Texture(TextureType::Type type, uint32_t resourceId)
: EventThreadServicesHolder(EventThreadServices::Get()),
  mTextureKey{},
  mNativeImage(),
  mSize(),
  mType(type),
  mFormat(Pixel::INVALID),
  mResourceId(resourceId),
  mUseUploadedParameter(true)
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  ,
  mMemoryInfo(new TextureMemoryInfo(static_cast<Dali::Integration::TextureContextTypeHint::Type>(Dali::Integration::TextureContextTypeHint::UNKNOWN + 2), "(FastTrack)", 0, 0, 0)) // We cannot know the size of reouceId using case. But just insert info to the queue.
#endif
{
}

void Texture::Initialize()
{
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning()))
  {
    if(mNativeImage)
    {
      mTextureKey = Render::Texture::NewKey(mNativeImage);
    }
    else
    {
      if(mResourceId != 0u)
      {
        mTextureKey = Render::Texture::NewKey(mType, mResourceId);
      }
      else
      {
        mTextureKey = Render::Texture::NewKey(mType, mFormat, mSize);
      }
    }
    OwnerKeyType<Render::Texture> transferKeyOwnership(mTextureKey);

    SceneGraph::AddTextureMessage(GetEventThreadServices().GetUpdateManager(), transferKeyOwnership);
  }
}

Texture::~Texture()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~Texture[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mTextureKey))
  {
    SceneGraph::RemoveTextureMessage(GetEventThreadServices().GetUpdateManager(), mTextureKey);
  }

#if defined(GPU_MEMORY_PROFILE_ENABLED)
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning()))
  {
    if(mMemoryInfo)
    {
      mMemoryInfo->Destroy(true);
    }
    mMemoryInfo.reset();
  }
#endif
}

#if defined(GPU_MEMORY_PROFILE_ENABLED)
namespace
{
// Temporal data to be used at UploadSubPixelData().
// Note that we can assume that Upload API called only for event thread. So don't be afraid of race condition.
Dali::Integration::TextureContextTypeHint::Type gTypeHint = Dali::Integration::TextureContextTypeHint::UNKNOWN;
std::string                                     gContext{};
} //namespace
#endif

bool Texture::Upload(PixelDataPtr pixelData, std::string context, Dali::Integration::TextureContextTypeHint::Type typeHint)
{
  if(mNativeImage || mResourceId != 0u)
  {
#if defined(GPU_MEMORY_PROFILE_ENABLED)
    // Change stored memory data and memory info here, To change newest data of native / resouces.
    // PixelData only be used for get hint of pixel format.
    if(mMemoryInfo)
    {
      mMemoryInfo->Destroy(false);
    }
    mMemoryInfo.reset(new TextureMemoryInfo(typeHint, context, mSize.GetWidth(), mSize.GetHeight(), Pixel::GetBytesPerPixel(pixelData->GetPixelFormat())));
#endif
    return false;
  }

#if defined(GPU_MEMORY_PROFILE_ENABLED)
  gTypeHint   = typeHint;
  gContext    = std::move(context);
  bool result = Upload(pixelData);
  gTypeHint   = Dali::Integration::TextureContextTypeHint::UNKNOWN;
  gContext.clear();
  return result;
#else
  // When GPU_MEMORY_PROFILE_ENABLED is not defined, fall back to basic Upload
  return Upload(pixelData);
#endif
}

bool Texture::Upload(PixelDataPtr pixelData)
{
  return UploadSubPixelData(pixelData, 0u, 0u, pixelData->GetWidth(), pixelData->GetHeight(), 0u, 0u, 0u, 0u, pixelData->GetWidth(), pixelData->GetHeight());
}

bool Texture::Upload(PixelDataPtr pixelData,
                     uint32_t     layer,
                     uint32_t     mipmap,
                     uint32_t     xOffset,
                     uint32_t     yOffset,
                     uint32_t     width,
                     uint32_t     height)
{
  return UploadSubPixelData(pixelData, 0u, 0u, pixelData->GetWidth(), pixelData->GetHeight(), layer, mipmap, xOffset, yOffset, width, height);
}

bool Texture::UploadSubPixelData(PixelDataPtr pixelData,
                                 uint32_t     dataXOffset,
                                 uint32_t     dataYOffset,
                                 uint32_t     dataWidth,
                                 uint32_t     dataHeight)
{
  return UploadSubPixelData(pixelData, dataXOffset, dataYOffset, dataWidth, dataHeight, 0u, 0u, 0u, 0u, dataWidth, dataHeight);
}

bool Texture::UploadSubPixelData(PixelDataPtr pixelData,
                                 uint32_t     dataXOffset,
                                 uint32_t     dataYOffset,
                                 uint32_t     dataWidth,
                                 uint32_t     dataHeight,
                                 uint32_t     layer,
                                 uint32_t     mipmap,
                                 uint32_t     xOffset,
                                 uint32_t     yOffset,
                                 uint32_t     width,
                                 uint32_t     height)
{
  constexpr auto max_value = std::numeric_limits<uint16_t>::max();
  DALI_ASSERT_ALWAYS(layer < max_value &&
                     mipmap < max_value &&
                     xOffset + width < max_value &&
                     yOffset + height < max_value &&
                     dataWidth < max_value &&
                     dataHeight < max_value &&
                     "Parameter value out of range");

  bool result(false);
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mTextureKey))
  {
    if(mNativeImage)
    {
      DALI_LOG_ERROR("OpenGL ES does not support uploading data to native texture\n");
    }
    else if(mResourceId != 0u)
    {
      DALI_LOG_ERROR("ResourceId using case does not support uploading data\n");
    }
    else
    {
      uint32_t pixelDataSize = dataWidth * dataHeight;
      if(pixelData->GetBuffer() == nullptr || pixelDataSize == 0)
      {
        DALI_LOG_ERROR("PixelData is empty\n");
      }
      else
      {
        Pixel::Format pixelDataFormat = pixelData->GetPixelFormat();
        if((mUseUploadedParameter || pixelDataFormat == mFormat) || ((pixelDataFormat == Pixel::RGB888) && (mFormat == Pixel::RGBA8888)))
        {
          if(pixelDataSize < width * height)
          {
            DALI_LOG_ERROR("PixelData of an incorrect size when trying to update texture\n");
          }
          else if(Pixel::IsCompressed(mFormat) &&
                  ((dataXOffset != 0) ||
                   (dataYOffset != 0) ||
                   (dataWidth != pixelData->GetWidth()) ||
                   (dataHeight != pixelData->GetHeight())))
          {
            DALI_LOG_ERROR("Compressed pixel format don't support SubPixelData upload\n");
          }
          else if((dataXOffset + dataWidth > pixelData->GetWidth()) ||
                  (dataYOffset + dataHeight > pixelData->GetHeight()))
          {
            DALI_LOG_ERROR("PixelData of an incorrect subsize when trying to update texture\n");
          }
          else if(!mUseUploadedParameter &&
                  ((xOffset + width > (static_cast<uint32_t>(mSize.GetWidth()) >> mipmap)) ||
                   (yOffset + height > (static_cast<uint32_t>(mSize.GetHeight()) >> mipmap))))
          {
            DALI_LOG_ERROR("Texture update area out of bounds\n");
          }
          else
          {
            if(mUseUploadedParameter)
            {
              mSize   = ImageDimensions(xOffset + width, yOffset + height);
              mFormat = pixelDataFormat;
            }

#if defined(GPU_MEMORY_PROFILE_ENABLED)
            if(mMemoryInfo)
            {
              mMemoryInfo->Destroy(false);
            }
            {
              auto bpp = Pixel::GetBytesPerPixel(mFormat);

              // Set size of this texture as mSize. (For partial upload cases.)
              // Send size of buffer only if bpp is zero, mean compressed type, or something error.
              mMemoryInfo.reset(new TextureMemoryInfo(gTypeHint, gContext, mSize.GetWidth(), mSize.GetHeight(), bpp, bpp ? 0 : pixelData.Get()->GetBufferSize()));
            }
#endif

            //Parameters are correct. Send message to upload data to the texture
            Graphics::UploadParams params = {static_cast<uint32_t>(dataXOffset),
                                             static_cast<uint32_t>(dataYOffset),
                                             static_cast<uint16_t>(dataWidth),
                                             static_cast<uint16_t>(dataHeight),
                                             static_cast<uint16_t>(layer),
                                             static_cast<uint16_t>(mipmap),
                                             static_cast<uint16_t>(xOffset),
                                             static_cast<uint16_t>(yOffset),
                                             static_cast<uint16_t>(width),
                                             static_cast<uint16_t>(height)};
            Render::UploadTextureMessage(GetEventThreadServices(), mTextureKey, pixelData, params);

            result = true;
          }
        }
        else
        {
          DALI_LOG_ERROR("Bad format\n");
        }
      }
    }
  }
  else
  {
    DALI_LOG_ERROR("Texture[%p] called Upload API from non-UI thread!\n", this);
  }

  return result;
}

void Texture::GenerateMipmaps()
{
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mTextureKey))
  {
    Render::GenerateMipmapsMessage(GetEventThreadServices(), mTextureKey);
  }
  else
  {
    DALI_LOG_ERROR("Texture[%p] called GenerateMipmaps API from non-UI thread!\n", this);
  }
}

uint32_t Texture::GetWidth() const
{
  return mSize.GetWidth();
}

uint32_t Texture::GetHeight() const
{
  return mSize.GetHeight();
}

Pixel::Format Texture::GetPixelFormat() const
{
  return mFormat;
}

uint32_t Texture::GetResourceId() const
{
  return mResourceId;
}

Dali::TextureType::Type Texture::GetTextureType() const
{
  return mType;
}

void Texture::SetSize(const ImageDimensions& size)
{
  mSize = size;
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mTextureKey))
  {
    Render::SetTextureSizeMessage(GetEventThreadServices(), mTextureKey, mSize);
  }
}

void Texture::SetPixelFormat(Pixel::Format format)
{
  mFormat = format;
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mTextureKey))
  {
    Render::SetTextureFormatMessage(GetEventThreadServices(), mTextureKey, mFormat);
  }
}

bool Texture::IsNative() const
{
  return static_cast<bool>(mNativeImage);
}

bool Texture::ApplyNativeFragmentShader(std::string& shader, int mask)
{
  bool modified = false;
  if(mNativeImage && !shader.empty())
  {
    Dali::String daliShader = Dali::Integration::ToDaliString(shader);
    modified                = mNativeImage->ApplyNativeFragmentShader(daliShader, mask);
    shader                  = Dali::Integration::ToStdString(daliShader);
  }

  return modified;
}

} // namespace Internal
} // namespace Dali
