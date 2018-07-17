#ifndef DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H
#define DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H

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
#include <dali/graphics/vulkan/internal/vulkan-types.h>

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

  friend struct CommandBufferPool;

public:

  CommandBuffer() = delete;

  ~CommandBuffer() override;

  /** Begin recording */
  void Begin( vk::CommandBufferUsageFlags usageFlags = vk::CommandBufferUsageFlags{},
              vk::CommandBufferInheritanceInfo* inheritanceInfo = nullptr );

  /** Finish recording */
  void End();

  /** Reset command buffer */
  void Reset();

  /** Free command buffer */
  void Free();

  /** Returns Vulkan object associated with the buffer */
  vk::CommandBuffer GetVkHandle() const;

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
  void BindVertexBuffers( uint32_t firstBinding, uint32_t bindingCount,
                          std::vector< Dali::Graphics::Vulkan::Handle< Buffer>> buffers,
                          const vk::DeviceSize* pOffsets );

  /**
   * Binds an index buffer
   * @param buffer
   * @param offset
   * @param indexType
   */
  void BindIndexBuffer( RefCountedBuffer buffer, uint32_t offset, vk::IndexType indexType );

  /**
   * Binds single vertex buffer
   * @param binding
   * @param buffer
   * @param offset
   */
  void
  BindVertexBuffer( uint32_t binding, const Dali::Graphics::Vulkan::Handle< Buffer >& buffer, vk::DeviceSize offset );

  /**
   * Binds graphics pipeline
   * @param pipeline
   */
  void BindGraphicsPipeline( Handle <Pipeline> pipeline );

  /**
   *
   * @param descriptorSets
   * @param pipeline
   * @param firstSet
   * @param descriptorSetCount
   */
  void BindDescriptorSets( std::vector< Dali::Graphics::Vulkan::Handle< DescriptorSet>> descriptorSets,
                           Handle <Pipeline> pipeline, uint32_t firstSet, uint32_t descriptorSetCount );

  /**
   * Binds descriptor sets to the most recently bound Pipeline
   * @param descriptorSets
   * @param firstSet
   */
  void
  BindDescriptorSets( std::vector< Dali::Graphics::Vulkan::Handle< DescriptorSet>> descriptorSets, uint32_t firstSet );

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
  void DrawIndexed( uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset,
                    uint32_t firstInstance );

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
                        std::vector< vk::MemoryBarrier > memoryBarriers,
                        std::vector< vk::BufferMemoryBarrier > bufferBarriers,
                        std::vector< vk::ImageMemoryBarrier > imageBarriers );

  /**
   * Executes secondary command buffers within primary command buffer
   * @param commandBuffers
   */
  void ExecuteCommands( const std::vector< Dali::Graphics::Vulkan::Handle< CommandBuffer>>& commandBuffers );

  /**
   * Executes secondary command buffers within primary command buffer
   * @param commandBuffers
   */
  void ExecuteCommands( const std::vector< Dali::Graphics::Vulkan::Handle< CommandBuffer>>& commandBuffers, uint32_t offset,
                   uint32_t count );

  /**
   * Copies buffer into the specified image
   * @param srcBuffer
   * @param dstImage
   * @param dtsLayout
   * @param regions
   */
  void CopyBufferToImage( RefCountedBuffer srcBuffer, RefCountedImage dstImage, vk::ImageLayout dstLayout,
                          std::vector< vk::BufferImageCopy > regions );

  /**
   * Copies image to image
   * @param srcImage
   * @param srcLayout
   * @param dstImage
   * @param dstLAyout
   * @param regions
   */
  void CopyImage( RefCountedImage srcImage,
                  vk::ImageLayout srcLayout,
                  RefCountedImage dstImage,
                  vk::ImageLayout dstLayout,
                  const std::vector<vk::ImageCopy>& regions );

  /**
   * Sets scissors when VK_DYNAMIC_STATE_SCISSOR used
   * @param firstScissor
   * @param scissorCount
   * @param pScissors
   */
  void SetScissor( uint32_t firstScissor, uint32_t scissorCount, const vk::Rect2D* pScissors );

  /**
   * Sets viewport when VK_DYNAMIC_STATE_VIEWPORT used
   * @param firstViewport
   * @param viewportCount
   * @param pViewports
   */
  void SetViewport( uint32_t firstViewport, uint32_t viewportCount, const vk::Viewport* pViewports );

  /**
   * Implements VkManaged::OnDestroy
   * @return
   */
  bool OnDestroy() override;

private:

  /**
   * Returns allocation index
   * @return
   */
  uint32_t GetPoolAllocationIndex() const;

private:

  // Constructor called by the CommandPool only
  CommandBuffer( CommandPool& commandPool,
                 uint32_t poolIndex,
                 const vk::CommandBufferAllocateInfo& allocateInfo,
                 vk::CommandBuffer vulkanHandle );

private:

  CommandPool* mOwnerCommandPool;
  Graphics* mGraphics;
  uint32_t mPoolAllocationIndex;
  vk::CommandBufferAllocateInfo mAllocateInfo{};

  vk::CommandBuffer mCommandBuffer{};

  RefCountedPipeline mCurrentPipeline;

  vk::RenderPass mCurrentRenderPass;

  bool mRecording{ false };
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_COMMAND_BUFFER_H
