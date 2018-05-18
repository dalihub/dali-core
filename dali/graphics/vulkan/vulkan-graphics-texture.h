#ifndef DALI_GRAPHICS_VULKAN_GRAPHICS_TEXTURE_H
#define DALI_GRAPHICS_VULKAN_GRAPHICS_TEXTURE_H

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
#include <dali/graphics/vulkan/vulkan-types.h>
#include <dali/graphics/graphics-controller.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 *
 */
enum class TextureUploadMode
{
  eLazy,
  eImmediate
};

/**
 * This is temporary implementation. It should be using graphics-texture as base
 * interface.
 */
class Texture : public VkManaged
{
public:

  /**
   * Creates new texture
   * @param width
   * @param height
   * @param format
   * @return
   */
  static Handle<Texture> New( Graphics& graphics, uint32_t width, uint32_t height, vk::Format format );

  /**
   * Schedules data upload from CPU
   * @param data
   * @param size
   * @param mode
   */
  void UploadData( const void* data, size_t size, TextureUploadMode mode );

  /**
   *
   * @param buffer
   * @param mode
   */
  void UploadFromBuffer( RefCountedBuffer buffer, TextureUploadMode mode );

  /**
   * Returns Image object
   * @return
   */
  RefCountedImage GetImage() const;

  /**
   * Returns image view
   * @return
   */
  RefCountedImageView GetImageView() const;

  /**
   * Returns sampler
   * @return
   */
  RefCountedSampler GetSampler() const;

private:

  explicit Texture( Graphics& graphics, uint32_t width, uint32_t height, vk::Format format );


  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

}
} // namespace Graphics
} // namespace Dali
#endif // DALI_GRAPHICS_VULKAN_GRAPHICS_TEXTURE_H
