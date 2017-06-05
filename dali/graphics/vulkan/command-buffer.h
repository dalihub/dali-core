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
#include <dali/graphics/vulkan/types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Graphics;
class CommandPool;
class CommandBuffer
{
public:
  CommandBuffer() = delete;

  CommandBuffer(Graphics& graphics, CommandPool& ownerPool, vk::CommandBuffer commandBuffer);
  CommandBuffer(Graphics& graphics, CommandPool& ownerPool, const vk::CommandBufferAllocateInfo& allocateInfo);
  CommandBuffer(Graphics& graphics, CommandPool& ownerPool);

  ~CommandBuffer();

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

  const std::vector< vk::Semaphore >& GetSignalSemaphores() const
  {
    return mSignalSemaphores;
  }

  const std::vector< vk::Semaphore >& GetSWaitSemaphores() const
  {
    return mWaitSemaphores;
  }

  const std::vector< vk::PipelineStageFlags >& GetWaitSemaphoreStages() const
  {
    return mWaitStages;
  }

  /** Returns Vulkan object associated with the buffer */
  inline const vk::CommandBuffer& Get() const
  {
    return mCommandBuffer;
  }

private:
  void RecordImageLayoutTransition(vk::Image             image,
                                   vk::AccessFlags        srcAccessMask,
                                   vk::AccessFlags        dstAccessMask,
                                   vk::PipelineStageFlags srcStageMask,
                                   vk::PipelineStageFlags dstStageMask,
                                   vk::ImageLayout        oldLayout,
                                   vk::ImageLayout        newLayout,
                                   vk::ImageAspectFlags   aspectMask);

private:
  Graphics& mGraphics;

  CommandPool& mCommandPool;

  // semaphores per command buffer
  std::vector< vk::Semaphore >          mSignalSemaphores;
  std::vector< vk::Semaphore >          mWaitSemaphores;
  std::vector< vk::PipelineStageFlags > mWaitStages;

  vk::CommandBuffer mCommandBuffer;

  bool mRecording;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H
