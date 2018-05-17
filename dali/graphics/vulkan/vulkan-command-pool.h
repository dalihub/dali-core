#ifndef DALI_GRAPHICS_VULKAN_COMMANDPOOL
#define DALI_GRAPHICS_VULKAN_COMMANDPOOL

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

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Graphics;
class CommandBuffer;

using CommandPoolHandle = Handle<class CommandPool>;

class CommandPool : public VkManaged
{

public:

  /**
   *
   * @param graphics
   * @param createInfo
   * @return
   */
  static CommandPoolHandle New( Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo );

  /**
   *
   * @param graphics
   * @return
   */
  static CommandPoolHandle New( Graphics& graphics );

  ~CommandPool() override;

  /**
   *
   * @return
   */
  vk::CommandPool GetVkHandle() const;

  /**
   *
   * @return
   */
  Graphics& GetGraphics() const;

  /**
   * Resets command pool
   */
  void Reset( bool releaseResources );

  /**
   *
   * @param allocateInfo
   * @return
   */
  CommandBufferRef NewCommandBuffer( const vk::CommandBufferAllocateInfo& allocateInfo );

  /**
   *
   * @param isPrimary
   * @return
   */
  CommandBufferRef NewCommandBuffer( bool isPrimary = true );

  /**
   * Releases command buffer
   * @param buffer
   * @return
   */
  bool ReleaseCommandBuffer( CommandBuffer& buffer, bool forceRelease );

  /**
   * Returns current pool capacity ( 0 if nothing allocated )
   * @return
   */
  uint32_t GetCapacity() const;

  /**
   * Returns number of allocated command buffers
   * @return
   */
  uint32_t GetAllocationCount() const;

  /**
   * Returns number of allocated command buffers by level
   * @param level
   * @return
   */
  uint32_t GetAllocationCount( vk::CommandBufferLevel level ) const;

public:

  bool OnDestroy() override;

private:

  CommandPool();

  CommandPool( Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo );

  struct Impl;
  std::unique_ptr<Impl> mImpl;

};


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_COMMANDPOOL
