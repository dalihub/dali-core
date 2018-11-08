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
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>

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

/**
 * SwapchainBuffer stores all per-buffer data
 */
struct SwapchainBuffer
{
  SwapchainBuffer( Graphics& _graphics );

  ~SwapchainBuffer();

  /**
   * Separate command buffer for each render pass (the final pass is for the swapchain fb)
   */
  std::vector<RefCountedCommandBuffer> commandBuffers;

  /**
   * Semaphore signalled on acquire next image
   */
  vk::Semaphore acquireNextImageSemaphore;

  /**
   * Semaphore signalled on complete commands submission
   */
  vk::Semaphore submitSemaphore;

  RefCountedFence betweenRenderPassFence;
  RefCountedFence endOfFrameFence;

  Graphics& graphics;
};

SwapchainBuffer::SwapchainBuffer(Graphics& _graphics)
: graphics( _graphics )
{
  acquireNextImageSemaphore = graphics.GetDevice().createSemaphore( {}, graphics.GetAllocator() ).value;
  submitSemaphore = graphics.GetDevice().createSemaphore( {}, graphics.GetAllocator() ).value;

  // Ensure there is at least 1 allocated primary command buffer
  commandBuffers.emplace_back( graphics.CreateCommandBuffer( true ) );

  betweenRenderPassFence = graphics.CreateFence({});
  endOfFrameFence = graphics.CreateFence({});
}

SwapchainBuffer::~SwapchainBuffer()
{
  // swapchain dies so make sure semaphore are not in use anymore
  graphics.GetDevice().waitIdle();
  graphics.GetDevice().destroySemaphore( acquireNextImageSemaphore, graphics.GetAllocator() );
  graphics.GetDevice().destroySemaphore( submitSemaphore, graphics.GetAllocator() );
}

Swapchain::Swapchain( Graphics& graphics, Queue& presentationQueue,
                      RefCountedSurface surface,
                      std::vector< RefCountedFramebuffer >&& framebuffers,
                      vk::SwapchainCreateInfoKHR createInfo,
                      vk::SwapchainKHR vkHandle )
: mGraphics( &graphics ),
  mQueue( &presentationQueue ),
  mSurface( std::move( surface ) ),
  mSwapchainImageIndex( 0u ),
  mSwapchainKHR( vkHandle ),
  mSwapchainCreateInfoKHR( std::move( createInfo ) ),
  mFramebuffers( std::move( framebuffers ) ),
  mIsValid( true )
{
}

Swapchain::~Swapchain() = default;

RefCountedFramebuffer Swapchain::GetCurrentFramebuffer() const
{
  return GetFramebuffer( mSwapchainImageIndex );
}

RefCountedFramebuffer Swapchain::GetFramebuffer( uint32_t index ) const
{
  return mFramebuffers[index];
}

RefCountedFramebuffer Swapchain::AcquireNextFramebuffer( bool shouldCollectGarbageNow  )
{
  // prevent from using invalid swapchain
  if( !mIsValid )
  {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Attempt to present invalid/expired swapchain: %p\n", static_cast< VkSwapchainKHR >(mSwapchainKHR) );
    return RefCountedFramebuffer();
  }

  const auto& device = mGraphics->GetDevice();

  // on swapchain first create sync primitives and master command buffer
  // if not created yet
  if( mSwapchainBuffers.empty() )
  {
    const auto MAX_SWAPCHAIN_BUFFERS { mFramebuffers.size() };

    mSwapchainBuffers.resize( MAX_SWAPCHAIN_BUFFERS );
    for( auto& buffer : mSwapchainBuffers )
    {
      buffer.reset( new SwapchainBuffer( *mGraphics ) );
    }
  }

  DALI_LOG_INFO( gVulkanFilter, Debug::General, "Swapchain Image Index ( BEFORE Acquire ) = %d", int(mSwapchainImageIndex) );
                 auto result = device.acquireNextImageKHR( mSwapchainKHR,
                                            std::numeric_limits<uint64_t>::max(),
                                            mSwapchainBuffers[mBufferIndex]->acquireNextImageSemaphore,
                                            nullptr, &mSwapchainImageIndex );

  DALI_LOG_INFO( gVulkanFilter, Debug::General, "Swapchain Image Index ( AFTER Acquire ) = %d", int(mSwapchainImageIndex) );

  // swapchain either not optimal or expired, returning nullptr and
  // setting validity to false
  if( result != vk::Result::eSuccess )
  {
    mIsValid = false;
    if ( result == vk::Result::eErrorOutOfDateKHR )
    {
      return RefCountedFramebuffer();
    }
    else
    {
      assert( mIsValid );
    }
  }

  auto& swapBuffer = mSwapchainBuffers[mBufferIndex];

  // First frames don't need waiting as they haven't been submitted
  // yet. Note, that waiting on the fence without resetting it may
  // cause a stall ( nvidia, ubuntu )
  if( mFrameCounter >= mSwapchainBuffers.size() )
  {
    mGraphics->WaitForFence( mSwapchainBuffers[mBufferIndex]->endOfFrameFence );
    mGraphics->ExecuteActions();
    mGraphics->CollectGarbage();
  }
  else
  {
    mGraphics->DeviceWaitIdle();
    mGraphics->ExecuteActions();
    mGraphics->CollectGarbage();
  }

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "Resetting " << swapBuffer->commandBuffers.size() << " command buffers" );

  for( auto& commandBuffer : swapBuffer->commandBuffers )
  {
    commandBuffer->Reset();
    commandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr );
  }

  return mFramebuffers[mSwapchainImageIndex];
}

void Swapchain::Present()
{
  // prevent from using invalid swapchain
  if( !mIsValid )
  {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Attempt to present invalid/expired swapchain: %p\n", static_cast< VkSwapchainKHR >(mSwapchainKHR) );
    return;
  }

  auto& swapBuffer = mSwapchainBuffers[mBufferIndex];

  // End any render pass command buffers
  size_t count = swapBuffer->commandBuffers.size();
  size_t index = 0;
  for( auto& commandBuffer : swapBuffer->commandBuffers )
  {
    commandBuffer->End();

    // @todo Add semaphores between each render pass
    if( index < count-1 )
    {
      auto submissionData = SubmissionData{};
      submissionData.SetCommandBuffers( { commandBuffer  } )
        .SetSignalSemaphores( { } )
        .SetWaitSemaphores( { } )
        .SetWaitDestinationStageMask( vk::PipelineStageFlagBits::eFragmentShader );
      mGraphics->Submit( *mQueue, { std::move( submissionData ) }, swapBuffer->betweenRenderPassFence );

      mGraphics->WaitForFence(swapBuffer->betweenRenderPassFence);
      mGraphics->ResetFence( swapBuffer->betweenRenderPassFence );
    }
    else // last frame
    {
      mGraphics->ResetFence( swapBuffer->endOfFrameFence );

      auto submissionData = SubmissionData{};

      submissionData.SetCommandBuffers( { commandBuffer  } )
        .SetSignalSemaphores( { swapBuffer->submitSemaphore } )
        .SetWaitSemaphores( { swapBuffer->acquireNextImageSemaphore } )
        .SetWaitDestinationStageMask( vk::PipelineStageFlagBits::eFragmentShader );
      mGraphics->Submit( *mQueue, { std::move( submissionData ) }, swapBuffer->endOfFrameFence );
    }

    ++index;
  }

  vk::PresentInfoKHR presentInfo{};
  vk::Result result;
  presentInfo.setPImageIndices( &mSwapchainImageIndex )
             .setPResults( &result )
             .setPSwapchains( &mSwapchainKHR )
             .setSwapchainCount( 1 )
             .setPWaitSemaphores( &swapBuffer->submitSemaphore )
             .setWaitSemaphoreCount( 1 );

  mGraphics->Present( *mQueue, presentInfo );

  // handle error
  if( presentInfo.pResults[0] != vk::Result::eSuccess )
  {
    // invalidate swapchain
    if ( result == vk::Result::eErrorOutOfDateKHR )
    {
      mIsValid = false;
    }
    else
    {
      mIsValid = false;
      assert( mIsValid );
    }
  }

  mFrameCounter++;
  mBufferIndex = uint32_t( (mBufferIndex+1) % mSwapchainBuffers.size() );
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
  presentInfo.setPImageIndices( &mSwapchainImageIndex )
             .setPResults( &result )
             .setPSwapchains( &mSwapchainKHR )
             .setSwapchainCount( 1 )
             .setPWaitSemaphores( nullptr )
             .setWaitSemaphoreCount( 0 );

  mGraphics->Present( *mQueue, presentInfo );
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

    mSwapchainKHR = nullptr;
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

void Swapchain::Invalidate()
{
  mIsValid = false;
}

void Swapchain::SetDepthStencil(vk::Format depthStencilFormat)
{
  RefCountedFramebufferAttachment depthAttachment;
  auto swapchainExtent = mSwapchainCreateInfoKHR.imageExtent;

  if( depthStencilFormat != vk::Format::eUndefined )
  {
    // Create depth/stencil image
    auto imageCreateInfo = vk::ImageCreateInfo{}
      .setFormat( depthStencilFormat )
      .setMipLevels( 1 )
      .setTiling( vk::ImageTiling::eOptimal )
      .setImageType( vk::ImageType::e2D )
      .setArrayLayers( 1 )
      .setExtent( { swapchainExtent.width, swapchainExtent.height, 1 } )
      .setUsage( vk::ImageUsageFlagBits::eDepthStencilAttachment )
      .setSharingMode( vk::SharingMode::eExclusive )
      .setInitialLayout( vk::ImageLayout::eUndefined )
      .setSamples( vk::SampleCountFlagBits::e1 );

    auto dsRefCountedImage = mGraphics->CreateImage( imageCreateInfo );

    auto memory = mGraphics->AllocateMemory( dsRefCountedImage, vk::MemoryPropertyFlagBits::eDeviceLocal );

    mGraphics->BindImageMemory( dsRefCountedImage, std::move(memory), 0 );

    // create the depth stencil ImageView to be used within framebuffer
    auto depthStencilImageView = mGraphics->CreateImageView( dsRefCountedImage );
    auto depthClearValue = vk::ClearDepthStencilValue{}.setDepth( 0.0 )
                                                       .setStencil( STENCIL_DEFAULT_CLEAR_VALUE );

    // A single depth attachment for the swapchain.
    depthAttachment = FramebufferAttachment::NewDepthAttachment( depthStencilImageView, depthClearValue );
  }

  // Get images
  auto images = VkAssert( mGraphics->GetDevice().getSwapchainImagesKHR( mSwapchainKHR ) );

  // allocate framebuffers
  auto framebuffers = std::vector< RefCountedFramebuffer >{};
  framebuffers.reserve( images.size() );

  auto clearColor = vk::ClearColorValue{}.setFloat32( { 0.0f, 0.0f, 0.0f, 1.0f } );

  for( auto&& image : images )
  {

    auto colorImageView = mGraphics->CreateImageView( mGraphics->CreateImageFromExternal( image, mSwapchainCreateInfoKHR.imageFormat, swapchainExtent ) );

    // A new color attachment for each framebuffer
    auto colorAttachment = FramebufferAttachment::NewColorAttachment( colorImageView,
                                                                      clearColor,
                                                                      true /* presentable */ );

    framebuffers.push_back( mGraphics->CreateFramebuffer( { colorAttachment },
                                               depthAttachment,
                                               swapchainExtent.width,
                                               swapchainExtent.height ) );
  }

  // Before replacing framebuffers in the swapchain, wait until all is done
  mGraphics->DeviceWaitIdle();

  mFramebuffers = std::move( framebuffers );
}


void Swapchain::AllocateCommandBuffers( size_t renderPassCount )
{
  size_t commandBuffersCount = mSwapchainBuffers[mBufferIndex]->commandBuffers.size();

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "AllocateCommandBuffers: cbCount:" << commandBuffersCount
                                                  << " renderPassCount: " << renderPassCount );

  if( commandBuffersCount < renderPassCount )
  {
    for( size_t index = commandBuffersCount; index < renderPassCount ; ++index )
    {
      // Create primary buffer for each render pass & begin recording
      auto commandBuffer = mGraphics->CreateCommandBuffer(true);
      commandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr );
      mSwapchainBuffers[mBufferIndex]->commandBuffers.emplace_back( commandBuffer );
    }
  }
}

RefCountedCommandBuffer Swapchain::GetLastCommandBuffer() const
{
  return mSwapchainBuffers[mBufferIndex]->commandBuffers.back();
}

std::vector<RefCountedCommandBuffer>& Swapchain::GetCommandBuffers() const
{
  return mSwapchainBuffers[mBufferIndex]->commandBuffers;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
