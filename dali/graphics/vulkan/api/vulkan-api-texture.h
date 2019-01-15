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

  void CopyBuffer(const API::Buffer& buffer,
                  uint32_t bufferOffset,
                  API::Extent2D extent2D,
                  API::Offset2D textureOffset2D,
                  uint32_t layer,
                  uint32_t level,
                  API::TextureUpdateFlags flags ) override;

  API::MemoryRequirements GetMemoryRequirements() const override;

  /**
   * Returns structure with texture properties
   * @return The reference to immutable TextureProperties object
   */
  const API::TextureProperties& GetProperties() override;

  /**
   * Initialises resources like memory, image view and samplers for previously
   * initialised image object. Used when lazy allocation is needed.
   */
  void InitialiseResources();

  /**
   * Tries to convert pixel data to the compatible format. As result it returns new buffer.
   * @param pData source data
   * @param sizeInBytes size of source data in bytes
   * @param width width in pixels
   * @param height height in pixels
   * @param outputBuffer reference to an output buffer
   * @return True if conversion was successful
   */
  bool TryConvertPixelData( const void* pData, uint32_t sizeInBytes, uint32_t width, uint32_t height, std::vector<uint8_t>& outputBuffer );

  /**
   * Tries to convert pixel data to the compatible format. The result is written into the specified memory area.
   * The memory must be allocated and large enough to accomodate output data.
   * @param pData  source data
   * @param sizeInBytes  size of source data in bytes
   * @param width width in pixels
   * @param height height in pixels
   * @param pOutputBuffer pointer to a valid output buffer
   * @param outputBufferSize size of output buffer. Must not be 0.
   * @return True if conversion was successful
   */
  bool TryConvertPixelData( const void* pData, uint32_t sizeInBytes, uint32_t width, uint32_t height, void* pOutputBuffer, uint32_t outputBufferSize );

private:

  void CreateSampler();
  void CreateImageView();
  bool InitialiseTexture();

  /**
   * Validates initial format
   * @return if valid, returns existing format
   *         if possible conversion, returns new converted format
   *         if not supported returns vk::Format::eUndefined
   */
  vk::Format ValidateFormat( vk::Format sourceFormat );


private:

  std::unique_ptr<VulkanAPI::TextureFactory> mTextureFactory;
  VulkanAPI::Controller& mController;
  Vulkan::Graphics& mGraphics;

  Vulkan::RefCountedImage       mImage;
  Vulkan::RefCountedImageView   mImageView;
  Vulkan::RefCountedSampler     mSampler;

  uint32_t    mWidth;
  uint32_t    mHeight;
  vk::Format  mFormat;
  vk::Format  mConvertFromFormat { vk::Format::eUndefined };
  vk::ImageUsageFlags mUsage;
  vk::ImageLayout mLayout;
  vk::ComponentMapping mComponentMapping{};

  bool mDisableStagingBuffer { false };
  std::unique_ptr<API::TextureProperties> mProperties;
};

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
#endif // DALI_GRAPHICS_VULKAN_API_TEXTURE_H
