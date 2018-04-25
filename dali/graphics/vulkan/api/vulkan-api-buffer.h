#ifndef DALI_GRAPHICS_VULKAN_API_BUFFER_H
#define DALI_GRAPHICS_VULKAN_API_BUFFER_H

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

#include <dali/graphics-api/graphics-api-buffer.h>
#include <dali/graphics/vulkan/vulkan-types.h>

#include <memory>



namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Controller;
}
namespace VulkanAPI
{


/**
 * Buffer represents a Buffer-type resource
 */
class Buffer : public Graphics::API::Buffer
{
public:

  explicit Buffer( Vulkan::Controller& controller, vk::BufferUsageFlagBits usage, API::Buffer::UsageHint usageHints, uint32_t size );

  bool Initialise();

  /**
   * Maps buffer only when host visible or nullptr when device local
   * @return
   */
  void* Map() override;

  /**
   * Unmaps previously accessed buffer
   */
  void Unmap() override;

  /**
   * Writes into the buffer
   * @param src
   * @param srcSize
   * @param dstOffset
   */
  void Write( void* src, uint32_t srcSize, uint32_t dstOffset ) override;

  Vulkan::BufferRef GetBufferRef() const;

private:

  Vulkan::Controller& mController;
  Vulkan::Graphics& mGraphics;
  vk::BufferUsageFlagBits mUsage{ vk::BufferUsageFlagBits::eVertexBuffer};
  API::Buffer::UsageHint mUsageHints;
  uint32_t mSize;

  Vulkan::BufferRef mBufferRef;
};
}
} // namespace Graphics
} // namespace Dali
#endif // DALI_GRAPHICS_VULKAN_API_BUFFER_H
