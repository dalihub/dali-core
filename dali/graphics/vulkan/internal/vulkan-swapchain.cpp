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
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-queue.h>
#include <dali/graphics/vulkan/internal/vulkan-surface.h>
#include <dali/graphics/vulkan/internal/vulkan-swapchain.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

Swapchain::Swapchain( Graphics& graphics, Queue& presentationQueue,
                      RefCountedSurface surface,
                      std::vector< SwapchainBuffer > framebuffers,
                      vk::SwapchainCreateInfoKHR createInfo,
                      vk::SwapchainKHR vkHandle )
        : mGraphics( &graphics ),
          mQueue( &presentationQueue ),
          mSurface( std::move( surface ) ),
          mCurrentBufferIndex( 0u ),
          mSwapchainKHR( vkHandle ),
          mSwapchainCreateInfoKHR( std::move( createInfo ) ),
          mSwapchainBuffer( std::move( framebuffers ) ),
          mIsValid( true )
{
}

Swapchain::~Swapchain() = default;

RefCountedFramebuffer Swapchain::GetCurrentFramebuffer() const
{
  return GetFramebuffer( mCurrentBufferIndex );
}

RefCountedFramebuffer Swapchain::GetFramebuffer( uint32_t index ) const
{
  return mSwapchainBuffer[index].framebuffer;
}

RefCountedFramebuffer Swapchain::AcquireNextFramebuffer()
{
  // prevent from using invalid swapchain
  if( !mIsValid )
  {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Attempt to present invalid/expired swapchain: %p\n", static_cast< VkSwapchainKHR >(mSwapchainKHR) );
    return RefCountedFramebuffer();
  }

  const auto& device = mGraphics->GetDevice();

  if( !mFrameFence )
  {
    mFrameFence = mGraphics->CreateFence( {} );
  }

  auto result = device.acquireNextImageKHR( mSwapchainKHR,
                                            std::numeric_limits<uint64_t>::max(),
                                            nullptr,
                                            mFrameFence->GetVkHandle(), &mCurrentBufferIndex );


  // swapchain either not optimal or expired, returning nullptr and
  // setting validity to false
  if( result != vk::Result::eSuccess )
  {
    mIsValid = false;
    return RefCountedFramebuffer();
  }

  mGraphics->WaitForFence( mFrameFence );
  mGraphics->ResetFence( mFrameFence );

  auto& swapBuffer = mSwapchainBuffer[mCurrentBufferIndex];

  // start recording
  auto inheritanceInfo = vk::CommandBufferInheritanceInfo{}
          .setFramebuffer( swapBuffer.framebuffer->GetVkHandle() )
          .setRenderPass( swapBuffer.framebuffer->GetRenderPass() )
          .setSubpass( 0 );

  swapBuffer.masterCmdBuffer->Reset();
  swapBuffer.masterCmdBuffer->Begin( vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo );

  return swapBuffer.framebuffer;
}

void Swapchain::Present()
{
  // prevent from using invalid swapchain
  if( !mIsValid )
  {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Attempt to present invalid/expired swapchain: %p\n", static_cast< VkSwapchainKHR >(mSwapchainKHR) );
    return;
  }

  auto& swapBuffer = mSwapchainBuffer[mCurrentBufferIndex];

  // end command buffer
  swapBuffer.masterCmdBuffer->End();

  // submit
  mGraphics->ResetFence( swapBuffer.endOfFrameFence );

  auto submissionData = SubmissionData{}.SetCommandBuffers( { swapBuffer.masterCmdBuffer } );

  mGraphics->Submit( *mQueue, { std::move( submissionData ) }, swapBuffer.endOfFrameFence );

  mGraphics->WaitForFence( swapBuffer.endOfFrameFence );

  // fixme: use semaphores to synchronize all previously submitted command buffers!
  vk::PresentInfoKHR presentInfo{};
  vk::Result result;
  presentInfo.setPImageIndices( &mCurrentBufferIndex )
             .setPResults( &result )
             .setPSwapchains( &mSwapchainKHR )
             .setSwapchainCount( 1 )
             .setPWaitSemaphores( nullptr )
             .setWaitSemaphoreCount( 0 );

  mGraphics->Present( *mQueue, presentInfo );

  mGraphics->ExecuteActions();
  mGraphics->CollectGarbage();

  // handle error
  if( presentInfo.pResults[0] != vk::Result::eSuccess )
  {
    // invalidate swapchain
    mIsValid = false;
  }

}

void Swapchain::Present( std::vector< vk::Semaphore > waitSemaphores )
{
  // prevent from using invalid swapchain
  if( !mIsValid )
  {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Attempt to present invalid/expired swapchain: %p\n", static_cast< VkSwapchainKHR >(mSwapchainKHR) );
    return;
  }

  vk::PresentInfoKHR presentInfo{};
  vk::Result result{};
  presentInfo.setPImageIndices( &mCurrentBufferIndex )
             .setPResults( &result )
             .setPSwapchains( &mSwapchainKHR )
             .setSwapchainCount( 1 )
             .setPWaitSemaphores( nullptr )
             .setWaitSemaphoreCount( 0 );

  mGraphics->Present( *mQueue, presentInfo );
}

RefCountedCommandBuffer Swapchain::GetCurrentCommandBuffer() const
{
  return mSwapchainBuffer[mCurrentBufferIndex].masterCmdBuffer;
}

bool Swapchain::OnDestroy()
{
  if( mSwapchainKHR )
  {
    auto graphics = mGraphics;
    auto device = graphics->GetDevice();
    auto swapchain = mSwapchainKHR;
    auto allocator = &graphics->GetAllocator();

    graphics->DiscardResource( [ device, swapchain, allocator ]() {
      DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: swap chain->%p\n",
                     static_cast< VkSwapchainKHR >(swapchain) )
      device.destroySwapchainKHR( swapchain, allocator );
    } );
  }
  return false;
}

vk::SwapchainKHR Swapchain::GetVkHandle()
{
  return mSwapchainKHR;
}

bool Swapchain::IsValid() const
{
  return mIsValid;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
