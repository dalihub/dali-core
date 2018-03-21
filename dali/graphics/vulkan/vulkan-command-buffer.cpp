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
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-fence.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
struct CommandBuffer::Impl
{
  Impl( CommandPool& commandPool, const vk::CommandBufferAllocateInfo& allocateInfo, vk::CommandBuffer commandBuffer )
  : mGraphics( commandPool.GetGraphics() ),
    mOwnerCommandPool( commandPool ),
    mAllocateInfo( allocateInfo ),
    mCommandBuffer( commandBuffer )
  {
  }

  ~Impl()
  {
    mResources.clear();
    mGraphics.GetDevice().freeCommandBuffers( mOwnerCommandPool.GetPool(),
                                              1, &mCommandBuffer );
  }

  bool Initialise()
  {
    return true;
  }

  template<class T>
  bool IsResourceAdded( Handle<T> resourceHandle )
  {
    for( auto&& res : mResources )
    {
      if( res == resourceHandle )
      {
        return true;
      }
    }
    return false;
  }

  template<class T>
  bool PushResource( Handle<T> resourceHandle )
  {
    if( !IsResourceAdded( resourceHandle ) )
    {
      mResources.push_back( VkTypeCast<VkManaged>( resourceHandle ) );
      return true;
    }
    return false;
  }

  /**
   *
   */
  void Begin( vk::CommandBufferUsageFlags usageFlags, vk::CommandBufferInheritanceInfo* inheritanceInfo )
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
      auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
      inheritance.setRenderPass( swapchain->GetCurrentFramebuffer()->GetVkRenderPass() );
      info.setPInheritanceInfo( &inheritance );
    }

    VkAssert( mCommandBuffer.begin( info ) );

    mResources.clear();

    mRecording = true;
  }

  void End()
  {
    assert( mRecording && "CommandBuffer is not in the recording state!" );
    VkAssert( mCommandBuffer.end() );
    mRecording = false;
  }

  void Reset()
  {
    assert( !mRecording && "Can't reset command buffer during recording!" );
    assert( mCommandBuffer && "Invalid command buffer!" );
    mCommandBuffer.reset( vk::CommandBufferResetFlagBits::eReleaseResources );

    mResources.clear();
  }

  void Free()
  {
    assert( mCommandBuffer && "Invalid command buffer!" );
    mGraphics.GetDevice().freeCommandBuffers( mOwnerCommandPool.GetPool(), mCommandBuffer );
  }

#pragma GCC diagnostic pop

  /** Push wait semaphores */
  void PushWaitSemaphores( const std::vector<vk::Semaphore>&          semaphores,
                           const std::vector<vk::PipelineStageFlags>& stages )
  {
    mWaitSemaphores = semaphores;
    mWaitStages     = stages;
  }

  /** Push signal semaphores */
  void PushSignalSemaphores( const std::vector<vk::Semaphore>& semaphores )
  {
    mSignalSemaphores = semaphores;
  }

  // TODO: handles should be synchronized
  void BindVertexBuffers( uint32_t                    firstBinding,
                          uint32_t                    bindingCount,
                          std::vector<Handle<Buffer>> buffers,
                          const vk::DeviceSize*       pOffsets )
  {
    // update list of used resources and create an array of VkBuffers
    std::vector<vk::Buffer> vkBuffers;
    vkBuffers.reserve( buffers.size() );
    for( auto&& buffer : buffers )
    {
      vkBuffers.emplace_back( buffer->GetVkBuffer() );
      PushResource( buffer );
    }

    mCommandBuffer.bindVertexBuffers( firstBinding, bindingCount, vkBuffers.data(), pOffsets );
  }

  void BindIndexBuffer( BufferRef buffer, uint32_t offset, vk::IndexType indexType )
  {
    // validate
    assert( ( buffer->GetUsage() & vk::BufferUsageFlagBits::eIndexBuffer ) &&
            "The buffer used as index buffer has wrong usage flags!" );

    PushResource( buffer );
    mCommandBuffer.bindIndexBuffer( buffer->GetVkBuffer(), offset, indexType );
  }

  void BindGraphicsPipeline( Handle<Pipeline> pipeline )
  {
    PushResource( pipeline );
    mCurrentPipeline = pipeline;
    mCommandBuffer.bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline->GetVkPipeline() );
  }

  void BindDescriptorSets( std::vector<Dali::Graphics::Vulkan::Handle<DescriptorSet>> descriptorSets,
                           Handle<Pipeline>                                           pipeline,
                           uint32_t                                                   firstSet,
                           uint32_t                                                   descriptorSetCount )
  {
    // update resources
    PushResource( pipeline );
    std::vector<vk::DescriptorSet> vkSets;
    vkSets.reserve( descriptorSets.size() );
    for( auto&& set : descriptorSets )
    {
      vkSets.emplace_back( set->GetVkDescriptorSet() );
      PushResource( set );
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

  void Draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance )
  {
    mCommandBuffer.draw( vertexCount, instanceCount, firstVertex, firstInstance );
  }

  void DrawIndexed(
    uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance )
  {
    mCommandBuffer.drawIndexed(
      indexCount, instanceCount, firstIndex, static_cast<int32_t>( vertexOffset ), firstInstance );
  }

  const std::vector<Handle<VkManaged>> GetResources() const
  {
    return mResources;
  }

  // RENDER PASS
  void BeginRenderPass( FBID framebufferId, uint32_t bufferIndex )
  {
    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
    auto surface = mGraphics.GetSurface( 0u );
    auto frameBuffer = swapchain->GetCurrentFramebuffer();
    auto renderPass = frameBuffer->GetVkRenderPass();
    auto clearValues = frameBuffer->GetDefaultClearValues();

    auto info = vk::RenderPassBeginInfo{};
    info.setFramebuffer( frameBuffer->GetVkFramebuffer() );
    info.setRenderPass( renderPass );
    info.setClearValueCount( U32( clearValues.size() ) );
    info.setPClearValues( clearValues.data() );
    info.setRenderArea( vk::Rect2D( {0, 0}, surface->GetSize() ) );

    mCurrentRenderPass = renderPass;
    mCommandBuffer.beginRenderPass( info, vk::SubpassContents::eInline );
  }

  void BeginRenderPass( vk::RenderPassBeginInfo renderPassBeginInfo, vk::SubpassContents subpassContents )
  {
    mCurrentRenderPass = renderPassBeginInfo.renderPass;
    mCommandBuffer.beginRenderPass( renderPassBeginInfo, subpassContents );
  }

  void EndRenderPass()
  {
    mCurrentRenderPass = nullptr;
    mCommandBuffer.endRenderPass();
  }

  void ExecuteCommands( std::vector<Dali::Graphics::Vulkan::Handle<CommandBuffer>> commandBuffers )
  {
    auto vkBuffers = std::vector<vk::CommandBuffer>{};
    vkBuffers.reserve( commandBuffers.size() );
    for( auto&& buf : commandBuffers )
    {
      vkBuffers.emplace_back( buf->GetVkCommandBuffer() );
      PushResource( buf );
    }

    mCommandBuffer.executeCommands( vkBuffers );
  }

  void PipelineBarrier( vk::PipelineStageFlags srcStageMask,
                        vk::PipelineStageFlags dstStageMask,
                        vk::DependencyFlags dependencyFlags,
                        std::vector<vk::MemoryBarrier> memoryBarriers,
                        std::vector<vk::BufferMemoryBarrier> bufferBarriers,
                        std::vector<vk::ImageMemoryBarrier> imageBarriers )
  {
    /*
     * Track resources
     */
    if( !imageBarriers.empty() )
    {
      for( auto&& imageBarrier : imageBarriers )
      {
        ImageRef imageResource{};
        if( imageResource = mGraphics.FindImage( imageBarrier.image ) )
        {
          PushResource( imageResource );
        }
      }
    }
    //@ todo other resource tracking

    mCommandBuffer.pipelineBarrier( srcStageMask, dstStageMask, dependencyFlags, memoryBarriers, bufferBarriers, imageBarriers );
  }

  void CopyBufferToImage( BufferRef srcBuffer, ImageRef dstImage, vk::ImageLayout dstLayout, std::vector<vk::BufferImageCopy> regions )
  {
    PushResource( srcBuffer );
    PushResource( dstImage );

    mCommandBuffer.copyBufferToImage( srcBuffer->GetVkBuffer(), dstImage->GetVkImage(), dstLayout, regions );
  }

  vk::ImageMemoryBarrier ImageLayoutTransitionBarrier( ImageRef image,
                                                      const vk::AccessFlags&        srcAccessMask,
                                                      const vk::AccessFlags&        dstAccessMask,
                                                      vk::ImageLayout        oldLayout,
                                                      vk::ImageLayout        newLayout,
                                                      const vk::ImageAspectFlags&   aspectMask
  ) const
  {
    return vk::ImageMemoryBarrier{}
         .setNewLayout( newLayout )
         .setImage( image->GetVkImage() )
         .setOldLayout( oldLayout )
         .setSrcAccessMask( srcAccessMask )
         .setDstAccessMask( dstAccessMask )
         .setSubresourceRange( vk::ImageSubresourceRange{ aspectMask, 0, image->GetLevelCount(), 0, image->GetLayerCount() } );
  }


  Graphics&                     mGraphics;
  CommandPool&                  mOwnerCommandPool;
  vk::CommandBufferAllocateInfo mAllocateInfo{};

  vk::CommandBuffer mCommandBuffer{};

  // semaphores per command buffer
  std::vector<vk::Semaphore>          mSignalSemaphores{};
  std::vector<vk::Semaphore>          mWaitSemaphores{};
  std::vector<vk::PipelineStageFlags> mWaitStages{};

  std::vector<Handle<VkManaged>> mResources; // used resources

  PipelineRef mCurrentPipeline;

  vk::RenderPass mCurrentRenderPass;

  FenceRef       mFinishedFence;

  bool mRecording{false};
};

/**
 *
 * Class: CommandBuffer
 *
 */

CommandBuffer::CommandBuffer( CommandPool&                         commandPool,
                              const vk::CommandBufferAllocateInfo& allocateInfo,
                              vk::CommandBuffer                    vkCommandBuffer )
{
  mImpl = MakeUnique<Impl>( commandPool, allocateInfo, vkCommandBuffer );
}

CommandBuffer::~CommandBuffer()
{
}

/** Begin recording */
void CommandBuffer::Begin( vk::CommandBufferUsageFlags usageFlags, vk::CommandBufferInheritanceInfo* inheritanceInfo )
{
  mImpl->Begin( usageFlags, inheritanceInfo );
}

/** Finish recording */
void CommandBuffer::End()
{
  mImpl->End();
}

/** Reset command buffer */
void CommandBuffer::Reset()
{
  mImpl->Reset();
}

/** Free command buffer */
void CommandBuffer::Free()
{
  mImpl->Free();
}

void CommandBuffer::OnRelease( uint32_t refcount )
{
  VkManaged::OnRelease( refcount );
}

/** Push wait semaphores */
void CommandBuffer::PushWaitSemaphores( const std::vector<vk::Semaphore>&          semaphores,
                                        const std::vector<vk::PipelineStageFlags>& stages )
{
  mImpl->PushWaitSemaphores( semaphores, stages );
}

/** Push signal semaphores */
void CommandBuffer::PushSignalSemaphores( const std::vector<vk::Semaphore>& semaphores )
{
  mImpl->PushSignalSemaphores( semaphores );
}

const std::vector<vk::Semaphore>& CommandBuffer::GetSignalSemaphores() const
{
  return mImpl->mSignalSemaphores;
}

const std::vector<vk::Semaphore>& CommandBuffer::GetSWaitSemaphores() const
{
  return mImpl->mWaitSemaphores;
}

const std::vector<vk::PipelineStageFlags>& CommandBuffer::GetWaitSemaphoreStages() const
{
  return mImpl->mWaitStages;
}

vk::CommandBuffer CommandBuffer::GetVkCommandBuffer() const
{
  return mImpl->mCommandBuffer;
}

bool CommandBuffer::IsPrimary() const
{
  return mImpl->mAllocateInfo.level == vk::CommandBufferLevel::ePrimary;
}

void CommandBuffer::BindVertexBuffers( uint32_t                    firstBinding,
                                       uint32_t                    bindingCount,
                                       std::vector<Handle<Buffer>> buffers,
                                       const vk::DeviceSize*       pOffsets )
{
  mImpl->BindVertexBuffers( firstBinding, bindingCount, buffers, pOffsets );
}

void CommandBuffer::BindIndexBuffer( BufferRef buffer, uint32_t offset, vk::IndexType indexType )
{
  mImpl->BindIndexBuffer( buffer, offset, indexType );
}

void CommandBuffer::BindVertexBuffer( uint32_t                               binding,
                                      Dali::Graphics::Vulkan::Handle<Buffer> buffer,
                                      vk::DeviceSize                         offset )
{
  mImpl->BindVertexBuffers( binding, 1, std::vector<Handle<Buffer>>( {buffer} ), &offset );
}

void CommandBuffer::BindGraphicsPipeline( Handle<Pipeline> pipeline )
{
  mImpl->BindGraphicsPipeline( pipeline );
}

void CommandBuffer::BindDescriptorSets( std::vector<Dali::Graphics::Vulkan::Handle<DescriptorSet>> descriptorSets,
                                        Handle<Pipeline>                                           pipeline,
                                        uint32_t                                                   firstSet,
                                        uint32_t                                                   descriptorSetCount )
{
  mImpl->BindDescriptorSets( descriptorSets, pipeline, firstSet, descriptorSetCount );
}

void CommandBuffer::BindDescriptorSets( std::vector<Dali::Graphics::Vulkan::Handle<DescriptorSet>> descriptorSets,
                                        uint32_t                                                   firstSet )
{
  mImpl->BindDescriptorSets(
    descriptorSets, mImpl->mCurrentPipeline, 0, static_cast<uint32_t>( descriptorSets.size() ) );
}

void CommandBuffer::Draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance )
{
  mImpl->Draw( vertexCount, instanceCount, firstVertex, firstInstance );
}

void CommandBuffer::DrawIndexed(
  uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance )
{
  mImpl->DrawIndexed( indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
}

void CommandBuffer::BeginRenderPass( FBID framebufferId, uint32_t bufferIndex )
{
  mImpl->BeginRenderPass( framebufferId, bufferIndex );
}

void CommandBuffer::BeginRenderPass( vk::RenderPassBeginInfo renderPassBeginInfo, vk::SubpassContents subpassContents )
{
  mImpl->BeginRenderPass( renderPassBeginInfo, subpassContents );
}

void CommandBuffer::EndRenderPass()
{
  mImpl->EndRenderPass();
}

void CommandBuffer::ExecuteCommands( std::vector<Dali::Graphics::Vulkan::Handle<CommandBuffer>> commandBuffers )
{
  mImpl->ExecuteCommands( commandBuffers );
}

void CommandBuffer::PipelineBarrier( vk::PipelineStageFlags srcStageMask,
                      vk::PipelineStageFlags dstStageMask,
                      vk::DependencyFlags dependencyFlags,
                      std::vector<vk::MemoryBarrier> memoryBarriers,
                      std::vector<vk::BufferMemoryBarrier> bufferBarriers,
                      std::vector<vk::ImageMemoryBarrier> imageBarriers )
{
  mImpl->PipelineBarrier( srcStageMask, dstStageMask, dependencyFlags, memoryBarriers, bufferBarriers, imageBarriers );
}

void CommandBuffer::CopyBufferToImage( BufferRef srcBuffer, ImageRef dstImage,
                                       vk::ImageLayout dstLayout, std::vector<vk::BufferImageCopy> regions )
{
  mImpl->CopyBufferToImage( srcBuffer, dstImage, dstLayout, regions );
}

vk::ImageMemoryBarrier CommandBuffer::ImageLayoutTransitionBarrier( ImageRef image,
                                                     vk::AccessFlags        srcAccessMask,
                                                     vk::AccessFlags        dstAccessMask,
                                                     vk::ImageLayout        oldLayout,
                                                     vk::ImageLayout        newLayout,
                                                     vk::ImageAspectFlags   aspectMask
) const
{
  return mImpl->ImageLayoutTransitionBarrier( image,
                                              srcAccessMask, dstAccessMask,
                                              oldLayout, newLayout,
                                              aspectMask  );
}

vk::ImageMemoryBarrier CommandBuffer::ImageLayoutTransitionBarrier( ImageRef image,
                                                     vk::ImageLayout        oldLayout,
                                                     vk::ImageLayout        newLayout,
                                                     vk::ImageAspectFlags   aspectMask
) const
{

  vk::AccessFlags  srcAccessMask, dstAccessMask;
  vk::PipelineStageFlags srcStageMask, dstStageMask;

  switch( oldLayout )
  {
    case vk::ImageLayout::ePreinitialized:
    case vk::ImageLayout::eUndefined:
    {
      srcAccessMask = {};
      srcStageMask  = vk::PipelineStageFlagBits::eTopOfPipe;
      break;
    }
    case vk::ImageLayout::ePresentSrcKHR:
    {
      srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead;
      srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
      break;
    }
    case vk::ImageLayout::eTransferSrcOptimal:
    {
      srcAccessMask = vk::AccessFlagBits::eMemoryRead;
      srcStageMask  = vk::PipelineStageFlagBits::eTransfer;
      break;
    }
    case vk::ImageLayout::eTransferDstOptimal:
    {
      srcAccessMask = vk::AccessFlagBits::eMemoryWrite;
      srcStageMask  = vk::PipelineStageFlagBits::eTransfer;
      break;
    }
    case vk::ImageLayout::eGeneral:
    case vk::ImageLayout::eColorAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    case vk::ImageLayout::eShaderReadOnlyOptimal:
    case vk::ImageLayout::eSharedPresentKHR:
    {
      break;
    }
  }

  switch( newLayout )
  {
    case vk::ImageLayout::ePresentSrcKHR:
    {
      dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
      dstStageMask  = vk::PipelineStageFlagBits::eBottomOfPipe;
      break;
    }
    case vk::ImageLayout::eTransferSrcOptimal:
    {
      dstAccessMask = vk::AccessFlagBits::eMemoryRead;
      dstStageMask  = vk::PipelineStageFlagBits::eTransfer;
      break;
    }
    case vk::ImageLayout::eTransferDstOptimal:
    {
      dstAccessMask = vk::AccessFlagBits::eMemoryWrite;
      dstStageMask  = vk::PipelineStageFlagBits::eTransfer;
      break;
    }
    case vk::ImageLayout::eShaderReadOnlyOptimal:
    {
      dstAccessMask = vk::AccessFlagBits::eMemoryRead;
      dstStageMask = vk::PipelineStageFlagBits::eVertexShader;
      break;
    }
    case vk::ImageLayout::eUndefined:
    case vk::ImageLayout::eGeneral:
    case vk::ImageLayout::eColorAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    case vk::ImageLayout::ePreinitialized:
    case vk::ImageLayout::eSharedPresentKHR:
    {
      break;
    }
  }

  return mImpl->ImageLayoutTransitionBarrier( image,
                                              srcAccessMask, dstAccessMask,
                                              oldLayout, newLayout,
                                              aspectMask  );
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
