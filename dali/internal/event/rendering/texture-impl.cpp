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

#if defined(ENABLE_GPU_MEMORY_PROFILE)
#include <unordered_set>

struct Dali::Internal::Texture::TextureMemoryInfo
{
  TextureMemoryInfo(const int32_t textureId, const std::string url, const int32_t width, const int32_t height, const uint32_t bytesPerPixel);

  ~TextureMemoryInfo();

  void Destroy(bool printLog);

  int32_t     mTextureId; ///< The TextureId associated with this ExternalTexture
  std::string mUrl;
  int32_t     mWidth;
  int32_t     mHeight;
  uint32_t    mMemorySize; //< The memory size of texture
};

namespace
{
std::unordered_set<Dali::Internal::Texture::TextureMemoryInfo*> gTextureMemoryInfoQueue;
uint32_t                                                        gTotalTextureMemory; ///< Total memory used by textures

inline void PrintTotalMemory()
{
  DALI_LOG_ERROR_NOFN("\n\n"); // For beauty
  DALI_LOG_ERROR_NOFN("## DALI_TEXTURE_MEMORY_PROFILER\n");
  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
  DALI_LOG_ERROR_NOFN("| Image Url                                                                                            |    size     | memory(MB) +\n");
  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");

  for(auto iter = gTextureMemoryInfoQueue.begin(); iter != gTextureMemoryInfoQueue.end(); iter++)
  {
    auto url = (*iter)->mUrl;
    if(url.length() > 100)
    {
      url = std::string("...") + url.substr(url.length() - 97, 97);
    }
    DALI_LOG_ERROR_NOFN("| %100s | %4d x %4d |%8.3f \n", url.c_str(), (*iter)->mWidth, (*iter)->mHeight, static_cast<double>((*iter)->mMemorySize) / (1024.0 * 1024.0));
  }

  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
  DALI_LOG_ERROR_NOFN(" >> textureCount    : %8d                 \n", gTextureMemoryInfoQueue.size());
  DALI_LOG_ERROR_NOFN(" >> Total Memory(MB): %8.3f            \n", static_cast<double>(gTotalTextureMemory) / (1024.0 * 1024.0));
  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
  DALI_LOG_ERROR_NOFN("\n\n");
}
} // namespace

Dali::Internal::Texture::TextureMemoryInfo::TextureMemoryInfo(const int32_t textureId, const std::string url, const int32_t width, const int32_t height, const uint32_t bytesPerPixel)
: mTextureId(textureId),
  mUrl(url),
  mWidth(width),
  mHeight(height),
  mMemorySize(static_cast<uint32_t>(width) * static_cast<uint32_t>(height) * bytesPerPixel)
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
#if defined(ENABLE_GPU_MEMORY_PROFILE)
  ,
  mUrl(""),
  mTextureId(-1),
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
#if defined(ENABLE_GPU_MEMORY_PROFILE)
  ,
  mUrl("(NativeImage)"),
  mTextureId(-1),
  mMemoryInfo(new TextureMemoryInfo(-1, "(NativeImage)", mSize.GetWidth(), mSize.GetHeight(), 4)) // Most of NativeImage texture use 4 byte per pixels. Let we assume the data size maximize.
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
#if defined(ENABLE_GPU_MEMORY_PROFILE)
  ,
  mUrl("(FastTrack)"),
  mTextureId(-1),
  mMemoryInfo(new TextureMemoryInfo(-1, "(FastTrack)", 0, 0, 0)) // We cannot know the size of reouceId using case. But just insert info to the queue.
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

#if defined(ENABLE_GPU_MEMORY_PROFILE)
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

#if defined(ENABLE_GPU_MEMORY_PROFILE)
bool Texture::Upload(PixelDataPtr pixelData, std::string url, int32_t textureId)
{
  if(mNativeImage || mResourceId != 0u)
  {
    // Change stored memory data and memory info here, To change newest data of native / resouces.
    // PixelData only be used for get hint of pixel format.
    if(mMemoryInfo)
    {
      mMemoryInfo->Destroy(false);
    }
    mMemoryInfo.reset(new TextureMemoryInfo(textureId, url, mSize.GetWidth(), mSize.GetHeight(), Pixel::GetBytesPerPixel(pixelData->GetPixelFormat())));
    return false;
  }
  mUrl       = (url == "") ? "UNDEFINED_URL" : url;
  mTextureId = textureId;
  return Upload(pixelData);
}
#endif

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

#if defined(ENABLE_GPU_MEMORY_PROFILE)
            if(mMemoryInfo)
            {
              mMemoryInfo->Destroy(false);
            }
            // Set size of this texture as mSize. (For partial upload cases.)
            mMemoryInfo.reset(new TextureMemoryInfo(mTextureId, mUrl, mSize.GetWidth(), mSize.GetHeight(), Pixel::GetBytesPerPixel(mFormat)));
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

#if defined(ENABLE_GPU_MEMORY_PROFILE)
#endif

} // namespace Internal
} // namespace Dali
