/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use *this file except in compliance with the License.
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
#include <dali/public-api/rendering/texture.h> // Dali::Texture

// INTERNAL INCLUDES
#include <dali/internal/event/images/pixel-data-impl.h> // Dali::Internal::PixelData
#include <dali/internal/event/rendering/texture-impl.h> // Dali::Internal::Texture

namespace Dali
{
Texture Texture::New(TextureType::Type type, Pixel::Format format, uint32_t width, uint32_t height)
{
  Internal::TexturePtr texture = Internal::Texture::New(type, format, width, height);
  return Texture(texture.Get());
}

Texture Texture::New(TextureType::Type type)
{
  Internal::TexturePtr texture = Internal::Texture::New(type, Pixel::INVALID, 0u, 0u);
  return Texture(texture.Get());
}

Texture Texture::New(NativeImageInterface& nativeImageInterface)
{
  Internal::TexturePtr texture = Internal::Texture::New(nativeImageInterface);
  return Texture(texture.Get());
}

Texture::Texture() = default;

Texture::~Texture() = default;

Texture::Texture(const Texture& handle) = default;

Texture Texture::DownCast(BaseHandle handle)
{
  return Texture(dynamic_cast<Dali::Internal::Texture*>(handle.GetObjectPtr()));
}

Texture& Texture::operator=(const Texture& handle) = default;

Texture::Texture(Texture&& rhs) noexcept = default;

Texture& Texture::operator=(Texture&& rhs) noexcept = default;

bool Texture::Upload(PixelData pixelData, std::string url, int32_t textureId)
{
#if defined(ENABLE_GPU_MEMORY_PROFILE)
  Internal::PixelData& internalPixelData = GetImplementation(pixelData);
  return GetImplementation(*this).Upload(&internalPixelData, url, textureId);
#endif
  return Upload(pixelData);
}

bool Texture::Upload(PixelData pixelData)
{
  Internal::PixelData& internalPixelData = GetImplementation(pixelData);
  return GetImplementation(*this).Upload(&internalPixelData);
}

bool Texture::Upload(PixelData pixelData,
                     uint32_t  layer,
                     uint32_t  mipmap,
                     uint32_t  xOffset,
                     uint32_t  yOffset,
                     uint32_t  width,
                     uint32_t  height)
{
  Internal::PixelData& internalPixelData = GetImplementation(pixelData);
  return GetImplementation(*this).Upload(&internalPixelData, layer, mipmap, xOffset, yOffset, width, height);
}

void Texture::GenerateMipmaps()
{
  return GetImplementation(*this).GenerateMipmaps();
}

uint32_t Texture::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

uint32_t Texture::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

Pixel::Format Texture::GetPixelFormat() const
{
  return GetImplementation(*this).GetPixelFormat();
}

Texture::Texture(Internal::Texture* pointer)
: BaseHandle(pointer)
{
}

} //namespace Dali
