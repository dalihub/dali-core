/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

  // Request event processing and update forcely.
  texture->mEventThreadServices.GetRenderController().RequestProcessEventsOnIdle(true);
  texture->mEventThreadServices.ForceNextUpdate();
  return texture;
}

Render::Texture* Texture::GetRenderObject() const
{
  return mRenderObject;
}

Texture::Texture(TextureType::Type type, Pixel::Format format, ImageDimensions size)
: mEventThreadServices(EventThreadServices::Get()),
  mRenderObject(nullptr),
  mNativeImage(),
  mSize(size),
  mType(type),
  mFormat(format)
{
}

Texture::Texture(NativeImageInterfacePtr nativeImageInterface)
: mEventThreadServices(EventThreadServices::Get()),
  mRenderObject(nullptr),
  mNativeImage(nativeImageInterface),
  mSize(nativeImageInterface->GetWidth(), nativeImageInterface->GetHeight()),
  mType(TextureType::TEXTURE_2D),
  mFormat(Pixel::RGB888)
{
}

void Texture::Initialize()
{
  if(EventThreadServices::IsCoreRunning())
  {
    if(mNativeImage)
    {
      mRenderObject = new Render::Texture(mNativeImage);
    }
    else
    {
      mRenderObject = new Render::Texture(mType, mFormat, mSize);
    }

    OwnerPointer<Render::Texture> transferOwnership(mRenderObject);
    AddTexture(mEventThreadServices.GetUpdateManager(), transferOwnership);
  }
}

Texture::~Texture()
{
  if(EventThreadServices::IsCoreRunning() && mRenderObject)
  {
    RemoveTexture(mEventThreadServices.GetUpdateManager(), *mRenderObject);
  }
}

bool Texture::Upload(PixelDataPtr pixelData)
{
  return Upload(pixelData, 0u, 0u, 0u, 0u, pixelData->GetWidth(), pixelData->GetHeight());
}

bool Texture::Upload(PixelDataPtr pixelData,
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
                     xOffset < max_value &&
                     yOffset < max_value &&
                     width < max_value &&
                     height < max_value &&
                     "Parameter value out of range");

  bool result(false);
  if(EventThreadServices::IsCoreRunning() && mRenderObject)
  {
    if(mNativeImage)
    {
      DALI_LOG_ERROR("OpenGL ES does not support uploading data to native texture\n");
    }
    else
    {
      uint32_t pixelDataSize = pixelData->GetWidth() * pixelData->GetHeight();
      if(pixelData->GetBuffer() == nullptr || pixelDataSize == 0)
      {
        DALI_LOG_ERROR("PixelData is empty\n");
      }
      else
      {
        Pixel::Format pixelDataFormat = pixelData->GetPixelFormat();
        if((pixelDataFormat == mFormat) || ((pixelDataFormat == Pixel::RGB888) && (mFormat == Pixel::RGBA8888)))
        {
          if(pixelDataSize < width * height)
          {
            DALI_LOG_ERROR("PixelData of an incorrect size when trying to update texture\n");
          }
          else if((xOffset + width > (mSize.GetWidth() / (1u << mipmap))) ||
                  (yOffset + height > (mSize.GetHeight() / (1u << mipmap))))
          {
            DALI_LOG_ERROR("Texture update area out of bounds\n");
          }
          else
          {
            //Parameters are correct. Send message to upload data to the texture
            UploadParams params = {static_cast<uint16_t>(layer),
                                   static_cast<uint16_t>(mipmap),
                                   static_cast<uint16_t>(xOffset),
                                   static_cast<uint16_t>(yOffset),
                                   static_cast<uint16_t>(width),
                                   static_cast<uint16_t>(height)};
            UploadTextureMessage(mEventThreadServices.GetUpdateManager(), *mRenderObject, pixelData, params);

            // Request event processing and update forcely
            mEventThreadServices.GetRenderController().RequestProcessEventsOnIdle(true);
            mEventThreadServices.ForceNextUpdate();

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

  return result;
}

void Texture::GenerateMipmaps()
{
  if(EventThreadServices::IsCoreRunning() && mRenderObject)
  {
    GenerateMipmapsMessage(mEventThreadServices.GetUpdateManager(), *mRenderObject);
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

bool Texture::IsNative() const
{
  return static_cast<bool>(mNativeImage);
}

bool Texture::ApplyNativeFragmentShader(std::string& shader)
{
  bool modified = false;
  if(mNativeImage && !shader.empty())
  {
    modified = mNativeImage->ApplyNativeFragmentShader(shader);
  }

  return modified;
}

} // namespace Internal
} // namespace Dali
