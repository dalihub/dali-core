/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/internal/render/renderers/render-texture.h>

// EXTERNAL INCLUDES
#include <math.h> //floor, log2

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gTextureFilter = Debug::Filter::New(Debug::Concise, false, "LOG_TEXTURE");
#endif

/**
 * Converts DALi pixel format to Graphics::Format
 * @param format
 * @return
 */
constexpr Graphics::Format ConvertPixelFormat(Pixel::Format format)
{
  switch(format)
  {
    case Pixel::INVALID:
      return Graphics::Format::UNDEFINED;
    case Pixel::A8:
      return Graphics::Format::R8_UNORM;

    case Pixel::L8:
      return Graphics::Format::L8;
    case Pixel::LA88:
      return Graphics::Format::L8A8;
    case Pixel::RGB565:
      return Graphics::Format::R5G6B5_UNORM_PACK16;
    case Pixel::BGR565:
      return Graphics::Format::B5G6R5_UNORM_PACK16;
    case Pixel::RGBA4444:
      return Graphics::Format::R4G4B4A4_UNORM_PACK16;

    case Pixel::BGRA4444:
      return Graphics::Format::B4G4R4A4_UNORM_PACK16;
    case Pixel::RGBA5551:
      return Graphics::Format::R5G5B5A1_UNORM_PACK16;
    case Pixel::BGRA5551:
      return Graphics::Format::B5G5R5A1_UNORM_PACK16;
    case Pixel::RGB888:
      return Graphics::Format::R8G8B8_UNORM;
    case Pixel::RGB8888:
      return Graphics::Format::R8G8B8A8_UNORM;
    case Pixel::BGR8888:
      return Graphics::Format::B8G8R8A8_UNORM;
    case Pixel::RGBA8888:
      return Graphics::Format::R8G8B8A8_UNORM;
    case Pixel::BGRA8888:
      return Graphics::Format::B8G8R8A8_UNORM;

    case Pixel::DEPTH_UNSIGNED_INT:
      return Graphics::Format::D16_UNORM;
    case Pixel::DEPTH_FLOAT:
      return Graphics::Format::D32_SFLOAT;
    case Pixel::DEPTH_STENCIL:
      return Graphics::Format::D24_UNORM_S8_UINT;

    // EAC
    case Pixel::COMPRESSED_R11_EAC:
      return Graphics::Format::EAC_R11_UNORM_BLOCK;
    case Pixel::COMPRESSED_SIGNED_R11_EAC:
      return Graphics::Format::EAC_R11_SNORM_BLOCK;
    case Pixel::COMPRESSED_RG11_EAC:
      return Graphics::Format::EAC_R11G11_UNORM_BLOCK;
    case Pixel::COMPRESSED_SIGNED_RG11_EAC:
      return Graphics::Format::EAC_R11G11_SNORM_BLOCK;

    // ETC
    case Pixel::COMPRESSED_RGB8_ETC2:
      return Graphics::Format::ETC2_R8G8B8_UNORM_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ETC2:
      return Graphics::Format::ETC2_R8G8B8_SRGB_BLOCK;

    case Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return Graphics::Format::ETC2_R8G8B8A1_UNORM_BLOCK; // no 'punchthrough' format

    case Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return Graphics::Format::ETC2_R8G8B8A1_SRGB_BLOCK; // no 'punchthrough' format

    case Pixel::COMPRESSED_RGBA8_ETC2_EAC:
      return Graphics::Format::ETC2_R8G8B8A8_UNORM_BLOCK;

    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
      return Graphics::Format::ETC2_R8G8B8A8_SRGB_BLOCK;

    case Pixel::COMPRESSED_RGB8_ETC1:
      return Graphics::Format::ETC2_R8G8B8_UNORM_BLOCK; // doesn't seem to be supported at all

    case Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
      return Graphics::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG; // or SRGB?

    // ASTC
    case Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
      return Graphics::Format::ASTC_4x4_UNORM_BLOCK; // or SRGB?
    case Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
      return Graphics::Format::ASTC_5x4_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
      return Graphics::Format::ASTC_5x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
      return Graphics::Format::ASTC_6x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
      return Graphics::Format::ASTC_6x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
      return Graphics::Format::ASTC_8x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
      return Graphics::Format::ASTC_8x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
      return Graphics::Format::ASTC_8x8_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
      return Graphics::Format::ASTC_10x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
      return Graphics::Format::ASTC_10x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
      return Graphics::Format::ASTC_10x8_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
      return Graphics::Format::ASTC_10x10_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
      return Graphics::Format::ASTC_12x10_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
      return Graphics::Format::ASTC_12x12_UNORM_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:

      return Graphics::Format::ASTC_4x4_SRGB_BLOCK; // not type with alpha, but likely to use SRGB
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
      return Graphics::Format::ASTC_5x4_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
      return Graphics::Format::ASTC_5x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
      return Graphics::Format::ASTC_6x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
      return Graphics::Format::ASTC_6x6_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
      return Graphics::Format::ASTC_8x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
      return Graphics::Format::ASTC_8x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
      return Graphics::Format::ASTC_8x8_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
      return Graphics::Format::ASTC_10x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
      return Graphics::Format::ASTC_10x6_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
      return Graphics::Format::ASTC_10x8_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
      return Graphics::Format::ASTC_10x10_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
      return Graphics::Format::ASTC_12x10_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
      return Graphics::Format::ASTC_12x12_SRGB_BLOCK;
    case Pixel::RGB16F:
      return Graphics::Format::R16G16B16_SFLOAT;
    case Pixel::RGB32F:
      return Graphics::Format::R32G32B32_SFLOAT;
    case Pixel::R11G11B10F:
      return Graphics::Format::R11G11B10_UFLOAT_PACK32;
  }
  return Graphics::Format::UNDEFINED;
}

constexpr Graphics::TextureType ConvertType(Texture::Type type)
{
  switch(type)
  {
    case TextureType::TEXTURE_2D:
      return Graphics::TextureType::TEXTURE_2D;
    case TextureType::TEXTURE_CUBE:
      return Graphics::TextureType::TEXTURE_CUBEMAP;
  }
  return Graphics::TextureType::TEXTURE_2D;
}

} //Unnamed namespace

Texture::Texture(Type type, Pixel::Format format, ImageDimensions size)
: mGraphicsController(nullptr),
  mGraphicsTexture(nullptr),
  mNativeImage(),
  mSampler(),
  mPixelFormat(format),
  mWidth(size.GetWidth()),
  mHeight(size.GetHeight()),
  mType(type),
  mHasAlpha(HasAlpha(format))
{
}

Texture::Texture(NativeImageInterfacePtr nativeImageInterface)
: mGraphicsController(nullptr),
  mGraphicsTexture(nullptr),
  mNativeImage(nativeImageInterface),
  mSampler(),
  mPixelFormat(Pixel::RGBA8888),
  mWidth(static_cast<uint16_t>(nativeImageInterface->GetWidth())),   // ignoring overflow, not happening in practice
  mHeight(static_cast<uint16_t>(nativeImageInterface->GetHeight())), // ignoring overflow, not happening in practice
  mType(TextureType::TEXTURE_2D),
  mHasAlpha(nativeImageInterface->RequiresBlending())
{
}

Texture::~Texture() = default;

void Texture::Initialize(Graphics::Controller& graphicsController)
{
  mGraphicsController = &graphicsController;
  if(mNativeImage)
  {
    Create(static_cast<uint32_t>(Graphics::TextureUsageFlagBits::SAMPLE));
  }
}

void Texture::Destroy()
{
  mGraphicsTexture.reset();
}

Graphics::Texture* Texture::GetGraphicsObject() const
{
  DALI_LOG_INFO(gTextureFilter, Debug::General, "SC::Texture(%p)::GetGraphicsObject() = %p\n", this, mGraphicsTexture.get());

  return mGraphicsTexture.get();
}

void Texture::Create(Graphics::TextureUsageFlags usage)
{
  CreateWithData(usage, nullptr, 0u);
}

void Texture::CreateWithData(Graphics::TextureUsageFlags usage, uint8_t* data, uint32_t size)
{
  auto createInfo = Graphics::TextureCreateInfo();
  createInfo
    .SetTextureType(ConvertType(mType))
    .SetUsageFlags(usage)
    .SetFormat(ConvertPixelFormat(mPixelFormat))
    .SetSize({mWidth, mHeight})
    .SetLayout(Graphics::TextureLayout::LINEAR)
    .SetData(data)
    .SetDataSize(size)
    .SetNativeImage(mNativeImage)
    .SetMipMapFlag(Graphics::TextureMipMapFlag::DISABLED);

  mGraphicsTexture = mGraphicsController->CreateTexture(createInfo, std::move(mGraphicsTexture));
}

void Texture::Upload(PixelDataPtr pixelData, const Internal::Texture::UploadParams& params)
{
  DALI_ASSERT_ALWAYS(!mNativeImage);

  if(!mGraphicsTexture)
  {
    Create(static_cast<Graphics::TextureUsageFlags>(Graphics::TextureUsageFlagBits::SAMPLE));
  }

  Graphics::TextureUpdateInfo info{};
  info.dstTexture   = mGraphicsTexture.get();
  info.dstOffset2D  = {params.xOffset, params.yOffset};
  info.layer        = params.layer;
  info.level        = params.mipmap;
  info.srcReference = 0;
  info.srcExtent2D  = {params.width, params.height};
  info.srcOffset    = 0;
  info.srcSize      = pixelData->GetBufferSize();
  info.srcFormat    = ConvertPixelFormat(pixelData->GetPixelFormat());

  Graphics::TextureUpdateSourceInfo updateSourceInfo{};
  updateSourceInfo.sourceType          = Graphics::TextureUpdateSourceInfo::Type::MEMORY;
  updateSourceInfo.memorySource.memory = pixelData->GetBuffer();

  mGraphicsController->UpdateTextures({info}, {updateSourceInfo});
}

bool Texture::HasAlphaChannel() const
{
  bool alpha = mHasAlpha;
  if(mNativeImage)
  {
    alpha = mNativeImage->RequiresBlending();
  }
  return alpha;
}

void Texture::GenerateMipmaps()
{
  if(!mGraphicsTexture)
  {
    Create(static_cast<Graphics::TextureUsageFlags>(Graphics::TextureUsageFlagBits::SAMPLE));
  }

  mGraphicsController->GenerateTextureMipmaps(*mGraphicsTexture.get());
}

} // namespace Render

} // namespace Internal

} // namespace Dali
