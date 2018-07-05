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
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/internal/vulkan-pipeline.h>
#include <dali/graphics/vulkan/internal/vulkan-surface.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
/**
 *
 * Class: CommandBuffer
 *
 */

CommandBuffer::CommandBuffer( CommandPool& commandPool,
                              uint32_t poolIndex,
                              const vk::CommandBufferAllocateInfo& allocateInfo,
                              vk::CommandBuffer vulkanHandle )
        : mOwnerCommandPool( &commandPool ),
          mGraphics( &mOwnerCommandPool->GetGraphics() ),
          mPoolAllocationIndex( poolIndex ),
          mAllocateInfo( allocateInfo ),
          mCommandBuffer( vulkanHandle )
{
}

CommandBuffer::~CommandBuffer() = default;

/** Begin recording */
void CommandBuffer::Begin( vk::CommandBufferUsageFlags usageFlags,
                           vk::CommandBufferInheritanceInfo* inheritanceInfo )
{
  assert( !mRecording && "CommandBuffer already is in the recording state" );
  auto info = vk::CommandBufferBeginInfo{};
  info.setPInheritanceInfo( inheritanceInfo );
  info.setFlags( usageFlags );

  // set the inheritance option
  auto inheritance = vk::CommandBufferInheritanceInfo{}.setSubpass( 0 );

  if( mAllocateInfo.level == vk::CommandBufferLevel::eSecondary )
  {
    // Render pass is obtained from the default framebuffer
    // it's a legacy but little nicer
    auto swapchain = mGraphics->GetSwapchainForFBID( 0u );
    inheritance.setRenderPass( swapchain->GetCurrentFramebuffer()->GetRenderPass() );
    info.setPInheritanceInfo( &inheritance );
  }

  VkAssert( mCommandBuffer.begin( info ) );

  mRecording = true;
}

/** Finish recording */
void CommandBuffer::End()
{
  assert( mRecording && "CommandBuffer is not in the recording state!" );
  VkAssert( mCommandBuffer.end() );
  mRecording = false;
}

/** Reset command buffer */
void CommandBuffer::Reset()
{
  assert( !mRecording && "Can't reset command buffer during recording!" );
  assert( mCommandBuffer && "Invalid command buffer!" );
  mCommandBuffer.reset( vk::CommandBufferResetFlagBits::eReleaseResources );
}

/** Free command buffer */
void CommandBuffer::Free()
{
  assert( mCommandBuffer && "Invalid command buffer!" );
  mGraphics->GetDevice().freeCommandBuffers( mOwnerCommandPool->GetVkHandle(), mCommandBuffer );
}

vk::CommandBuffer CommandBuffer::GetVkHandle() const
{
  return mCommandBuffer;
}

bool CommandBuffer::IsPrimary() const
{
  return mAllocateInfo.level == vk::CommandBufferLevel::ePrimary;
}

void CommandBuffer::BindVertexBuffers( uint32_t firstBinding,
                                       uint32_t bindingCount,
                                       std::vector< Handle< Buffer>> buffers,
                                       const vk::DeviceSize* pOffsets )
{
  // update list of used resources and create an array of VkBuffers
  std::vector< vk::Buffer > vkBuffers;
  vkBuffers.reserve( buffers.size() );
  for( auto&& buffer : buffers )
  {
    vkBuffers.emplace_back( buffer->GetVkHandle() );
  }

  mCommandBuffer.bindVertexBuffers( firstBinding, bindingCount, vkBuffers.data(), pOffsets );
}

void CommandBuffer::BindIndexBuffer( RefCountedBuffer buffer, uint32_t offset, vk::IndexType indexType )
{
  // validate
  assert( ( buffer->GetUsage() & vk::BufferUsageFlagBits::eIndexBuffer ) &&
          "The buffer used as index buffer has wrong usage flags!" );

  mCommandBuffer.bindIndexBuffer( buffer->GetVkHandle(), offset, indexType );
}

void CommandBuffer::BindVertexBuffer( uint32_t binding,
                                      const RefCountedBuffer& buffer,
                                      vk::DeviceSize offset )
{
  BindVertexBuffers( binding, 1, std::vector< Handle< Buffer>>( { buffer } ), &offset );
}

void CommandBuffer::BindGraphicsPipeline( Handle< Pipeline > pipeline )
{
  mCurrentPipeline = pipeline;
  mCommandBuffer.bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline->GetVkHandle() );
}

void CommandBuffer::BindDescriptorSets( std::vector< RefCountedDescriptorSet > descriptorSets,
                                        RefCountedPipeline pipeline,
                                        uint32_t firstSet,
                                        uint32_t descriptorSetCount )
{
  // update resources
  std::vector< vk::DescriptorSet > vkSets;
  vkSets.reserve( descriptorSets.size() );
  for( auto&& set : descriptorSets )
  {
    vkSets.emplace_back( set->GetVkDescriptorSet() );
  }

  // TODO: support dynamic offsets
  mCommandBuffer.bindDescriptorSets( vk::PipelineBindPoint::eGraphics,
                                     pipeline->GetVkPipelineLayout(),
                                     firstSet,
                                     descriptorSetCount,
                                     vkSets.data(),
                                     0,
                                     nullptr );
}

void CommandBuffer::BindDescriptorSets( std::vector< RefCountedDescriptorSet > descriptorSets,
                                        uint32_t firstSet )
{
  BindDescriptorSets(
          descriptorSets, mCurrentPipeline, firstSet, static_cast<uint32_t>( descriptorSets.size()) );
}

void CommandBuffer::Draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance )
{
  mCommandBuffer.draw( vertexCount, instanceCount, firstVertex, firstInstance );
}

void CommandBuffer::DrawIndexed( uint32_t indexCount,
                                 uint32_t instanceCount,
                                 uint32_t firstIndex,
                                 uint32_t vertexOffset,
                                 uint32_t firstInstance )
{
  mCommandBuffer.drawIndexed( indexCount,
                              instanceCount,
                              firstIndex,
                              static_cast<int32_t>( vertexOffset ),
                              firstInstance );
}

void CommandBuffer::BeginRenderPass( FBID framebufferId, uint32_t bufferIndex )
{
  auto swapchain = mGraphics->GetSwapchainForFBID( 0u );
  auto surface = mGraphics->GetSurface( 0u );
  auto frameBuffer = swapchain->GetCurrentFramebuffer();
  auto renderPass = frameBuffer->GetRenderPass();
  auto clearValues = frameBuffer->GetClearValues();

  auto info = vk::RenderPassBeginInfo{};
  info.setFramebuffer( frameBuffer->GetVkHandle() );
  info.setRenderPass( renderPass );
  info.setClearValueCount( U32( clearValues.size() ) );
  info.setPClearValues( clearValues.data() );
  info.setRenderArea( vk::Rect2D( { 0, 0 }, surface->GetCapabilities().currentExtent ) );

  mCurrentRenderPass = renderPass;
  mCommandBuffer.beginRenderPass( info, vk::SubpassContents::eInline );
}

void CommandBuffer::BeginRenderPass( vk::RenderPassBeginInfo renderPassBeginInfo, vk::SubpassContents subpassContents )
{
  mCurrentRenderPass = renderPassBeginInfo.renderPass;
  mCommandBuffer.beginRenderPass( renderPassBeginInfo, subpassContents );
}

void CommandBuffer::EndRenderPass()
{
  mCurrentRenderPass = nullptr;
  mCommandBuffer.endRenderPass();
}

void CommandBuffer::ExecuteCommands( const std::vector< Dali::Graphics::Vulkan::Handle< CommandBuffer>>& commandBuffers,
                                     uint32_t offset, uint32_t count )
{
  assert( mAllocateInfo.level == vk::CommandBufferLevel::ePrimary
          &&
          "Cannot record command: ExecuteCommands\tReason: The command buffer recording this command is not primary" );

  auto vkBuffers = std::vector< vk::CommandBuffer >{};
  vkBuffers.reserve( commandBuffers.size() );

  auto iter = commandBuffers.begin();
  std::advance( iter, offset );

  for( uint32_t i = 0; i < count; ++i )
  {
    const auto& buf = *iter;
    assert( buf->mAllocateInfo.level == vk::CommandBufferLevel::eSecondary &&
            "Cannot record command: Execute Commands\tReason: A command buffer provided for execution is not secondary" );

    vkBuffers.emplace_back( buf->GetVkHandle() );
    ++iter;
  }

  mCommandBuffer.executeCommands( vkBuffers );
}

void
CommandBuffer::ExecuteCommands( const std::vector< Dali::Graphics::Vulkan::Handle< CommandBuffer>>& commandBuffers )
{
  ExecuteCommands( commandBuffers, 0, uint32_t( commandBuffers.size() ) );
}

void CommandBuffer::PipelineBarrier( vk::PipelineStageFlags srcStageMask,
                                     vk::PipelineStageFlags dstStageMask,
                                     vk::DependencyFlags dependencyFlags,
                                     std::vector< vk::MemoryBarrier > memoryBarriers,
                                     std::vector< vk::BufferMemoryBarrier > bufferBarriers,
                                     std::vector< vk::ImageMemoryBarrier > imageBarriers )
{
  mCommandBuffer.pipelineBarrier( srcStageMask,
                                  dstStageMask,
                                  dependencyFlags,
                                  memoryBarriers,
                                  bufferBarriers,
                                  imageBarriers );
}

void CommandBuffer::CopyBufferToImage( RefCountedBuffer srcBuffer,
                                       RefCountedImage dstImage,
                                       vk::ImageLayout dstLayout,
                                       std::vector< vk::BufferImageCopy > regions )
{
  mCommandBuffer.copyBufferToImage( srcBuffer->GetVkHandle(),
                                    dstImage->GetVkHandle(),
                                    dstLayout,
                                    regions );
}

void CommandBuffer::SetScissor( uint32_t firstScissor, uint32_t scissorCount, const vk::Rect2D* pScissors )
{
  mCommandBuffer.setScissor( firstScissor, scissorCount, pScissors );
}

uint32_t CommandBuffer::GetPoolAllocationIndex() const
{
  return mPoolAllocationIndex;
}

bool CommandBuffer::OnDestroy()
{
  mOwnerCommandPool->ReleaseCommandBuffer( *this );
  return true;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
