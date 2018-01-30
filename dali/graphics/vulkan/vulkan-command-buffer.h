#ifndef DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H
#define DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
class CommandBuffer : public VkManaged
{
  friend class CommandPool;

public:

  CommandBuffer() = delete;

  ~CommandBuffer() override;

  /** Begin recording */
  void Begin(vk::CommandBufferUsageFlags       usageFlags      = vk::CommandBufferUsageFlags{},
             vk::CommandBufferInheritanceInfo* inheritanceInfo = nullptr);

  /** Finish recording */
  void End();

  /** Reset command buffer */
  void Reset();

  /** Free command buffer */
  void Free();

  /** Records image layout transition barrier for one image */
  void ImageLayoutTransition(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectMask);

  /** Push wait semaphores */
  void PushWaitSemaphores(const std::vector< vk::Semaphore >&          semaphores,
                          const std::vector< vk::PipelineStageFlags >& stages);

  /** Push signal semaphores */
  void PushSignalSemaphores(const std::vector< vk::Semaphore >& semaphores);

  /**
   *
   * @return
   */
  const std::vector< vk::Semaphore >& GetSignalSemaphores() const;

  /**
   *
   * @return
   */
  const std::vector< vk::Semaphore >& GetSWaitSemaphores() const;

  /**
   *
   * @return
   */
  const std::vector< vk::PipelineStageFlags >& GetWaitSemaphoreStages() const;

  /** Returns Vulkan object associated with the buffer */
  vk::CommandBuffer GetVkCommandBuffer() const;

  operator vk::CommandBuffer() const
  {
    return GetVkCommandBuffer();
  }

private:

  /**
   *
   * @param image
   * @param srcAccessMask
   * @param dstAccessMask
   * @param srcStageMask
   * @param dstStageMask
   * @param oldLayout
   * @param newLayout
   * @param aspectMask
   */
  void RecordImageLayoutTransition(vk::Image             image,
                                   vk::AccessFlags        srcAccessMask,
                                   vk::AccessFlags        dstAccessMask,
                                   vk::PipelineStageFlags srcStageMask,
                                   vk::PipelineStageFlags dstStageMask,
                                   vk::ImageLayout        oldLayout,
                                   vk::ImageLayout        newLayout,
                                   vk::ImageAspectFlags   aspectMask);

private:

  // Constructor called by the CommandPool only
  CommandBuffer( CommandPool& commandPool, const vk::CommandBufferAllocateInfo& allocateInfo, vk::CommandBuffer vkCommandBuffer );

  class Impl;
  std::unique_ptr<Impl> mImpl;

};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H
