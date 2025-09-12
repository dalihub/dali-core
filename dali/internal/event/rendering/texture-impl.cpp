/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/manager/update-manager.h>

#if defined(ENABLE_GPU_MEMORY_PROFILE)
namespace
{
typedef int32_t TextureId;
struct TextureMemoryInfo
{
  TextureMemoryInfo(const TextureId textureId, const std::string url, const int32_t width, const int32_t height, const uint64_t memorySize)
  : mTextureId(textureId),
    mUrl(url),
    mWidth(width),
    mHeight(height),
    mMemorySize(memorySize)
  {
  }

  TextureId   mTextureId; ///< The TextureId associated with this ExternalTexture
  std::string mUrl;
  int32_t     mWidth;
  int32_t     mHeight;
  uint64_t    mMemorySize; //< The memory size of texture
};

std::vector<TextureMemoryInfo> gTextureMemoryInfoQueue;
uint64_t                       gTotalTextureMemory; ///< Total memory used by textures
} //namespace
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
  mTextureId(-1),
  mDataSize(0u),
  mUrl("")
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
  mTextureId(-1),
  mDataSize(0u),
  mUrl("")
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
  mTextureId(-1),
  mDataSize(0u),
  mUrl("")
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

    AddTextureMessage(GetEventThreadServices().GetUpdateManager(), mTextureKey);
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
#if defined(ENABLE_GPU_MEMORY_PROFILE)
    for(auto iter = gTextureMemoryInfoQueue.begin(); iter != gTextureMemoryInfoQueue.end(); iter++)
    {
      if(iter->mTextureId == mTextureId)
      {
        if(iter->mMemorySize == mDataSize)
        {
          gTotalTextureMemory -= iter->mMemorySize;
          gTextureMemoryInfoQueue.erase(iter);
          PrintTotalMemory();
          break;
        }
        DALI_LOG_WARNING("Can't find proper texture memory info");
      }
    }
#endif

    RemoveTextureMessage(GetEventThreadServices().GetUpdateManager(), mTextureKey);
  }
}

#if defined(ENABLE_GPU_MEMORY_PROFILE)
bool Texture::Upload(PixelDataPtr pixelData, std::string url, int32_t textureId)
{
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
            auto pixelSize = Pixel::GetBytesPerPixel(mFormat);
            mDataSize      = dataWidth * dataHeight * pixelSize;
            gTotalTextureMemory += mDataSize;
            gTextureMemoryInfoQueue.push_back(TextureMemoryInfo(mTextureId, mUrl, dataWidth, dataHeight, mDataSize));
            PrintTotalMemory();
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
            UploadTextureMessage(GetEventThreadServices().GetUpdateManager(), mTextureKey, pixelData, params);

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
    GenerateMipmapsMessage(GetEventThreadServices().GetUpdateManager(), mTextureKey);
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
    SetTextureSizeMessage(GetEventThreadServices().GetUpdateManager(), mTextureKey, mSize);
  }
}

void Texture::SetPixelFormat(Pixel::Format format)
{
  mFormat = format;
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mTextureKey))
  {
    SetTextureFormatMessage(GetEventThreadServices().GetUpdateManager(), mTextureKey, mFormat);
  }
}

bool Texture::IsNative() const
{
  return static_cast<bool>(mNativeImage);
}

bool Texture::ApplyNativeFragmentShader(std::string& shader, int count)
{
  bool modified = false;
  if(mNativeImage && !shader.empty())
  {
    modified = mNativeImage->ApplyNativeFragmentShader(shader, count);
  }

  return modified;
}

#if defined(ENABLE_GPU_MEMORY_PROFILE)
void Texture::PrintTotalMemory()
{
  DALI_LOG_ERROR_NOFN("\n\n"); // For beauty
  DALI_LOG_ERROR_NOFN("## DALI_TEXTURE_MEMORY_PROFILER\n");
  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
  DALI_LOG_ERROR_NOFN("| Image Url                                                                                            |    size     | memory(MB) +\n");
  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");

  for(auto iter = gTextureMemoryInfoQueue.begin(); iter != gTextureMemoryInfoQueue.end(); iter++)
  {
    auto url = iter->mUrl;
    if(url.length() > 100)
    {
      url = url.substr(0, 97) + "...";
    }
    DALI_LOG_ERROR_NOFN("| %-100s | %4d x %4d |%8.2f \n", url.c_str(), iter->mWidth, iter->mHeight, static_cast<double>(iter->mMemorySize) / (1024.0 * 1024.0));
  }

  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
  DALI_LOG_ERROR_NOFN(" >> textureCount    : %8d                 \n", gTextureMemoryInfoQueue.size());
  DALI_LOG_ERROR_NOFN(" >> Total Memory(MB): %8.2f            \n", static_cast<double>(gTotalTextureMemory) / (1024.0 * 1024.0));
  DALI_LOG_ERROR_NOFN("+---------------------------------------------------------------------------------------------------------------------------------+\n");
  DALI_LOG_ERROR_NOFN("\n\n");
}
#endif

} // namespace Internal
} // namespace Dali
