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
class Image;
class Graphics;
class Buffer;
class Pipeline;
class DescriptorSet;
class CommandBuffer : public VkManaged
{
  friend class CommandPool;
  friend class CommandBufferPool;

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

  /**
   * Tests if the command buffer is primary
   * @return Returns true if the command buffer is primary
   */
  bool IsPrimary() const;

  /**
   * Binds an array of vertex buffers
   * @param firstBinding
   * @param bindingCount
   * @param buffers
   * @param pOffsets
   */
  void BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount,
                         std::vector<Dali::Graphics::Vulkan::Handle<Buffer>> buffers,
                         const vk::DeviceSize *pOffsets);

  /**
   * Binds an index buffer
   * @param buffer
   * @param offset
   * @param indexType
   */
  void BindIndexBuffer( BufferRef buffer, uint32_t offset, vk::IndexType indexType);

  /**
   * Binds single vertex buffer
   * @param binding
   * @param buffer
   * @param offset
   */
  void BindVertexBuffer(uint32_t binding, Dali::Graphics::Vulkan::Handle<Buffer> buffer, vk::DeviceSize offset );

  /**
   * Binds graphics pipeline
   * @param pipeline
   */
  void BindGraphicsPipeline( Handle<Pipeline> pipeline );

  /**
   *
   * @param descriptorSets
   * @param pipeline
   * @param firstSet
   * @param descriptorSetCount
   */
  void BindDescriptorSets( std::vector<Dali::Graphics::Vulkan::Handle<DescriptorSet>> descriptorSets,
                           Handle<Pipeline> pipeline, uint32_t firstSet, uint32_t descriptorSetCount );

  /**
   * Binds descriptor sets to the most recently bound Pipeline
   * @param descriptorSets
   * @param firstSet
   */
  void BindDescriptorSets( std::vector<Dali::Graphics::Vulkan::Handle<DescriptorSet>> descriptorSets, uint32_t firstSet );

  /**
   * Issues draw command
   * @param vertexCount
   * @param instanceCount
   * @param firstVertex
   * @param firstInstance
   */
  void Draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance );

  /**
   * Issues draw indexed primiteve command
   * @param indexCount
   * @param instanceCount
   * @param firstIndex
   * @param vertexOffset
   * @param firstInstance
   */
  void DrawIndexed( uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance );

  /**
   * Begins render pass using VkRenderPass and VkFramebuffer associated with FBID
   * @todo should be replaced with proper implementation and use the framebuffer
   * @param framebufferId
   * @param bufferIndex
   */
  void BeginRenderPass( FBID framebufferId, uint32_t bufferIndex );

  /**
   * Allows to issue custom VkRenderPassBeginInfo structure
   * @param renderPassBeginInfo
   * @param subpassContents
   */
  void BeginRenderPass( vk::RenderPassBeginInfo renderPassBeginInfo, vk::SubpassContents subpassContents );

  /**
   * Ends current render pass
   */
  void EndRenderPass();

  /**
   * Records pipeline barrier
   * @param srcStageMask
   * @param dstStageMask
   * @param dependencyFlags
   * @param memoryBarriers
   * @param bufferBarriers
   * @param imageBarriers
   */
  void PipelineBarrier( vk::PipelineStageFlags srcStageMask,
                        vk::PipelineStageFlags dstStageMask,
                        vk::DependencyFlags dependencyFlags,
                        std::vector<vk::MemoryBarrier> memoryBarriers,
                        std::vector<vk::BufferMemoryBarrier> bufferBarriers,
                        std::vector<vk::ImageMemoryBarrier> imageBarriers );

  /**
   * Executes secondary command buffers within primary command buffer
   * @param commandBuffers
   */
  void ExecuteCommands( std::vector<Dali::Graphics::Vulkan::Handle<CommandBuffer>> commandBuffers );

  /**
   * Copies buffer into the specified image
   * @param srcBuffer
   * @param dstImage
   * @param dtsLayout
   * @param regions
   */
  void CopyBufferToImage( BufferRef srcBuffer, ImageRef dstImage, vk::ImageLayout dstLayout,
                          std::vector<vk::BufferImageCopy> regions );

  /**
   * Creates layout transition barrier
   * @return
   */
  vk::ImageMemoryBarrier ImageLayoutTransitionBarrier( ImageRef image,
                                                  vk::AccessFlags        srcAccessMask,
                                                  vk::AccessFlags        dstAccessMask,
                                                  vk::ImageLayout        oldLayout,
                                                  vk::ImageLayout        newLayout,
                                                  vk::ImageAspectFlags   aspectMask
  ) const;

  /**
   * Simplified version of memory barrier generation based on data stored inside the Image
   * @param image
   * @param newLayout
   * @param aspectMask
   * @return
   */
  vk::ImageMemoryBarrier ImageLayoutTransitionBarrier( ImageRef image,
                                                       vk::ImageLayout        olsLayout,
                                                       vk::ImageLayout        newLayout,
                                                       vk::ImageAspectFlags   aspectMask
  ) const;

private:

  /**
   * Returns allocation index
   * @return
   */
  uint32_t GetPoolAllocationIndex() const;

private:

  // Constructor called by the CommandPool only
  CommandBuffer( CommandPool& commandPool, uint32_t poolIndex, const vk::CommandBufferAllocateInfo& allocateInfo, vk::CommandBuffer vkCommandBuffer );
  class Impl;
  std::unique_ptr<Impl> mImpl;

};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H
