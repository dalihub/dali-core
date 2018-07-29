#ifndef DALI_GRAPHICS_VULKAN_API_TEXTURE_H
#define DALI_GRAPHICS_VULKAN_API_TEXTURE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-texture-factory.h>
#include <dali/graphics-api/graphics-api-texture.h>
#include <dali/graphics/vulkan/internal/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
}
namespace VulkanAPI
{
class TextureFactory;
class Controller;

/**
 * This is temporary implementation. It should be using graphics-texture as base
 * interface.
 */
class Texture : public Dali::Graphics::API::Texture
{
public:

  explicit Texture( Dali::Graphics::API::TextureFactory& factory );

  ~Texture() override;

  bool Initialise();

  Vulkan::RefCountedImage GetImageRef() const;

  Vulkan::RefCountedImageView GetImageViewRef() const;

  Vulkan::RefCountedSampler GetSamplerRef() const;

  void CopyMemory( const void *srcMemory, uint32_t maxDataSize, API::Extent2D srcExtent, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode ) override;

  void CopyTexture( const API::Texture &srcTexture, API::Rect2D srcRegion, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode ) override;

  void CopyBuffer( const API::Buffer &srcBuffer, API::Extent2D srcExtent, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode) override;

private:

  void CreateSampler();

  void CreateImageView();

  bool InitialiseTexture();

  void InitialiseNativeTexture();

private:

  VulkanAPI::TextureFactory& mTextureFactory;
  VulkanAPI::Controller& mController;
  Vulkan::Graphics& mGraphics;

  Vulkan::RefCountedImage       mImage;
  Vulkan::RefCountedImageView   mImageView;
  Vulkan::RefCountedSampler     mSampler;

  uint32_t    mWidth;
  uint32_t    mHeight;
  vk::Format  mFormat;
  vk::ImageUsageFlags mUsage;
  vk::ImageLayout mLayout;
  vk::ComponentMapping mComponentMapping{};

  Any                   mTbmSurface;
};

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
#endif // DALI_GRAPHICS_VULKAN_API_TEXTURE_H
