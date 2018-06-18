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
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-fence.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-image-view.h>
#include <dali/graphics/vulkan/vulkan-queue.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-swapchain.h>
#include <dali/graphics/vulkan/vulkan-debug.h>

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
  /*
   * Each buffer has own master command buffer which executes
   * secondary buffers
   */
  RefCountedCommandBuffer masterCmdBuffer;

  /*
   * Framebuffer object associated with the buffer
   */
  RefCountedFramebuffer framebuffer;

  /*
   * Sync primitives
   */
  RefCountedFence endOfFrameFence;

  /*
   * Buffer index
   */
  uint32_t index;

  /*
   * First use before presenting
   */
  bool firstUse;
};

struct Swapchain::Impl
{
  Impl( Swapchain& owner,
        Graphics& graphics,
        Queue& presentationQueue,
        RefCountedSurface surface,
        uint32_t bufferCount,
        uint32_t flags )
          : mOwner( owner ),
            mGraphics( graphics ),
            mQueue( presentationQueue ),
            mSurface( std::move( surface ) ),
            mBufferCount( bufferCount ),
            mFlags( flags ),
            mCurrentBufferIndex( 0u )
  {
    mSwapchainCreateInfoKHR.setSurface( mSurface->GetSurfaceKHR() )
                           .setPreTransform( vk::SurfaceTransformFlagBitsKHR::eIdentity )
                           .setPresentMode( vk::PresentModeKHR::eFifo )
                           .setOldSwapchain( nullptr ) //@todo support surface replacement!
                           .setMinImageCount( mBufferCount )
                           .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )
                           .setImageSharingMode( vk::SharingMode::eExclusive )
                           .setImageArrayLayers( 1 )
                           .setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )
                           .setClipped( true )
                           .setQueueFamilyIndexCount( 0 )
                           .setPQueueFamilyIndices( nullptr );
  }

  ~Impl() = default;

  Impl( const Impl& ) = delete;

  Impl& operator=( const Impl& ) = delete;

  bool Initialise()
  {
    if( !SetImageFormat() )
    {
      return false;
    }

    // get extents
    mSwapchainExtent = mSurface->GetSize();

    mSwapchainCreateInfoKHR.setImageFormat( mSwapchainImageFormat );
    mSwapchainCreateInfoKHR.setImageExtent( mSwapchainExtent );
    mSwapchainCreateInfoKHR.setImageColorSpace( mSwapchainColorSpace );

    Create();

    InitialiseSwapchainBuffers();

    mFirstPresent = true;
    return true;
  }

  bool InitialiseSwapchainBuffers()
  {
    mSwapchainBuffer.clear();
    for( auto&& fb : mFramebuffers )
    {
      auto masterCmd = mGraphics.CreateCommandBuffer( true );

      auto swapBuffer = SwapchainBuffer{};
      swapBuffer.framebuffer = fb;
      swapBuffer.index = 0;
      swapBuffer.masterCmdBuffer = masterCmd;
      swapBuffer.endOfFrameFence = mGraphics.CreateFence( {} );
      swapBuffer.firstUse = true;
      mSwapchainBuffer.emplace_back( swapBuffer );
    }

    return true;
  }

  bool SetImageFormat()
  {
    // obtain supported image format
    auto formats = VkAssert( mGraphics.GetPhysicalDevice().getSurfaceFormatsKHR( mSurface->GetSurfaceKHR() ) );
    mSwapchainImageFormat = vk::Format::eUndefined;

    for( auto&& format : formats )
    {
      if( format.format != vk::Format::eUndefined )
      {
        mSwapchainColorSpace = format.colorSpace;
        mSwapchainImageFormat = format.format;
        break;
      }
    }

    return vk::Format::eUndefined != mSwapchainImageFormat;
  }

  /**
   * Creates swapchain immediately
   * @return
   */
  bool Create()
  {
    const auto& device = mGraphics.GetDevice();
    const auto& allocator = mGraphics.GetAllocator();

    //@todo validation
    mSwapchainKHR = VkAssert( device.createSwapchainKHR( mSwapchainCreateInfoKHR, allocator ) );

    if( !mSwapchainKHR )
    {
      return false;
    }

    // pull images and create Framebuffers
    auto images = VkAssert( device.getSwapchainImagesKHR( mSwapchainKHR ) );

    // number of images must match requested buffering mode
    if( images.size() != mBufferCount )
    {
      device.destroySwapchainKHR( mSwapchainKHR );
      mSwapchainKHR = nullptr;
      return false;
    }

    //@todo create depth-stencil image
    auto depthStencilImageView = CreateDepthStencil();

    /*
     * CREATE FRAMEBUFFERS
     */
    for( auto&& image : images )
    {
      mFramebuffers.emplace_back( CreateFramebuffer( image ) );

      // set depth/stencil if supported
      if( depthStencilImageView )
      {
        mFramebuffers.back()->SetAttachment( depthStencilImageView, Framebuffer::AttachmentType::DEPTH_STENCIL, 0u );
      }

      // create framebuffer and compatible render pass right now, no need to defer it
      mFramebuffers.back()->Commit();
    }

    return true;
  }

  /**
   * Creates depth stencil if necessary
   * @return
   */
  RefCountedImageView CreateDepthStencil()
  {
    auto imageCreateInfo = vk::ImageCreateInfo{}
            .setFormat( vk::Format::eD24UnormS8Uint )
            .setMipLevels( 1 )
            .setTiling( vk::ImageTiling::eOptimal )
            .setImageType( vk::ImageType::e2D )
            .setArrayLayers( 1 )
            .setExtent( { mSwapchainExtent.width, mSwapchainExtent.height, 1 } )
            .setUsage( vk::ImageUsageFlagBits::eDepthStencilAttachment )
            .setSharingMode( vk::SharingMode::eExclusive )
            .setInitialLayout( vk::ImageLayout::eUndefined )
            .setSamples( vk::SampleCountFlagBits::e1 );

    auto dsRefCountedImage = mGraphics.CreateImage( imageCreateInfo );

    auto memory = mGraphics
            .GetDeviceMemoryManager()
            .GetDefaultAllocator()
            .Allocate( dsRefCountedImage, vk::MemoryPropertyFlagBits::eDeviceLocal );

    mGraphics.BindImageMemory( dsRefCountedImage, memory, 0 );

    // create imageview to be used within framebuffer
    return mGraphics.CreateImageView( dsRefCountedImage );
  }

  /**
   * Creates a Framebuffer and compatible RenderPass
   * @param image
   * @return
   */
  RefCountedFramebuffer CreateFramebuffer( vk::Image& image )
  {
    //TODO: Create the Framebuffer using the graphics class
    auto fbRef = Framebuffer::New( mGraphics, mSwapchainExtent.width, mSwapchainExtent.height );

    auto imageCreateInfo = vk::ImageCreateInfo{}
            .setFormat( mSwapchainImageFormat )
            .setSamples( vk::SampleCountFlagBits::e1 )
            .setInitialLayout( vk::ImageLayout::eUndefined )
            .setSharingMode( vk::SharingMode::eExclusive )
            .setUsage( vk::ImageUsageFlagBits::eColorAttachment )
            .setExtent( { mSwapchainExtent.width, mSwapchainExtent.height, 1 } )
            .setArrayLayers( 1 )
            .setImageType( vk::ImageType::e2D )
            .setTiling( vk::ImageTiling::eOptimal )
            .setMipLevels( 1 );

    // Create external Image reference
    // Note that despite we don't create VkImage, we still fill the createinfo structure
    // as this data will be used later
    auto refCountedImage = Image::NewFromExternal( mGraphics, imageCreateInfo, image );

    // Create basic imageview ( all mipmaps, all layers )
    auto refCountedImageView = mGraphics.CreateImageView( refCountedImage );

    fbRef->SetAttachment( refCountedImageView, Framebuffer::AttachmentType::COLOR, 0u );

    return fbRef;
  }

  /**
   * This function acquires next framebuffer
   * @todo we should rather use round robin method
   * @return
   */
  RefCountedFramebuffer AcquireNextFramebuffer()
  {
    const auto& device = mGraphics.GetDevice();

    if( !mFrameFence )
    {
      mFrameFence = mGraphics.CreateFence( {} );
    }

    mCurrentBufferIndex =
            VkAssert( device.acquireNextImageKHR( mSwapchainKHR, 1000000, nullptr, mFrameFence->GetVkHandle() ) );

    mGraphics.WaitForFence( mFrameFence );
    mGraphics.ResetFence( mFrameFence );

    auto& swapBuffer = mSwapchainBuffer[mCurrentBufferIndex];

    // start recording
    auto inheritanceInfo = vk::CommandBufferInheritanceInfo{}
            .setFramebuffer( swapBuffer.framebuffer->GetVkHandle() )
            .setRenderPass( swapBuffer.framebuffer->GetRenderPassVkHandle() )
            .setSubpass( 0 );

    swapBuffer.masterCmdBuffer->Reset();
    swapBuffer.masterCmdBuffer->Begin( vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo );

    return swapBuffer.framebuffer;
  }

  RefCountedCommandBuffer GetPrimaryCommandBuffer() const
  {
    return mSwapchainBuffer[mCurrentBufferIndex].masterCmdBuffer;
  }

  bool Present()
  {
    auto& swapBuffer = mSwapchainBuffer[mCurrentBufferIndex];

    // end command buffer
    swapBuffer.masterCmdBuffer->End();

    // submit
    mGraphics.ResetFence( swapBuffer.endOfFrameFence );

    auto submissionData = SubmissionData{}.SetCommandBuffers( { swapBuffer.masterCmdBuffer } );

    mGraphics.Submit( mQueue, { std::move( submissionData ) }, swapBuffer.endOfFrameFence );

    mGraphics.WaitForFence( swapBuffer.endOfFrameFence );

    // fixme: use semaphores to synchronize all previously submitted command buffers!
    vk::PresentInfoKHR presentInfo{};
    vk::Result result;
    presentInfo.setPImageIndices( &mCurrentBufferIndex )
               .setPResults( &result )
               .setPSwapchains( &mSwapchainKHR )
               .setSwapchainCount( 1 )
               .setPWaitSemaphores( nullptr )
               .setWaitSemaphoreCount( 0 );

    mGraphics.Present( mQueue, presentInfo );

    mGraphics.CollectGarbage();

    return true;
  }

  // same as present but additionally waits for semaphores
  // needed when present queue is different from graphics queue
  bool Present( const std::vector< vk::Semaphore >& semaphores )
  {
    vk::PresentInfoKHR presentInfo{};
    vk::Result result{};
    presentInfo.setPImageIndices( &mCurrentBufferIndex )
               .setPResults( &result )
               .setPSwapchains( &mSwapchainKHR )
               .setSwapchainCount( 1 )
               .setPWaitSemaphores( nullptr )
               .setWaitSemaphoreCount( 0 );

    mGraphics.Present( mQueue, presentInfo );

    mGraphics.CollectGarbage();

    return true;
  }

  Swapchain& mOwner;
  Graphics& mGraphics;
  Queue& mQueue;
  RefCountedSurface mSurface;

  uint32_t mBufferCount;
  uint32_t mFlags;
  uint32_t mCurrentBufferIndex;

  RefCountedFence mFrameFence;

  // swapchain framebuffers
  std::vector< RefCountedFramebuffer > mFramebuffers;

  vk::SwapchainKHR mSwapchainKHR;
  vk::SwapchainCreateInfoKHR mSwapchainCreateInfoKHR;

  vk::Format mSwapchainImageFormat;
  vk::ColorSpaceKHR mSwapchainColorSpace;
  vk::Extent2D mSwapchainExtent;

  std::vector< SwapchainBuffer > mSwapchainBuffer;

  bool mFirstPresent;
};

/**
 * Swapchain API
 */
RefCountedSwapchain Swapchain::New(
        Graphics& graphics, Queue& presentationQueue, RefCountedSurface surface, uint8_t bufferCount, uint32_t flags )
{
  auto retval = RefCountedSwapchain( new Swapchain( graphics, presentationQueue, surface, bufferCount, flags ) );

  if( retval->mImpl->Initialise() )
  {
    return retval;
  }

  return RefCountedSwapchain();
}

Swapchain::Swapchain(
        Graphics& graphics, Queue& presentationQueue, RefCountedSurface surface, uint8_t bufferCount, uint32_t flags )
{
  mImpl = std::make_unique< Impl >( *this, graphics, presentationQueue, surface, bufferCount, flags );
}

Swapchain::Swapchain() = default;

Swapchain::~Swapchain() = default;

RefCountedFramebuffer Swapchain::GetCurrentFramebuffer() const
{
  return GetFramebuffer( mImpl->mCurrentBufferIndex );
}

RefCountedFramebuffer Swapchain::GetFramebuffer( uint32_t index ) const
{
  return mImpl->mSwapchainBuffer[index].framebuffer;
}

RefCountedFramebuffer Swapchain::AcquireNextFramebuffer()
{
  return mImpl->AcquireNextFramebuffer();
}

void Swapchain::Present()
{
  mImpl->Present();
}

void Swapchain::Present( std::vector< vk::Semaphore > waitSemaphores )
{
  mImpl->Present( waitSemaphores );
}

RefCountedCommandBuffer Swapchain::GetPrimaryCommandBuffer() const
{
  return mImpl->GetPrimaryCommandBuffer();
}

bool Swapchain::Destroy()
{
  auto& graphics = mImpl->mGraphics;
  auto device = graphics.GetDevice();
  auto swapchain = mImpl->mSwapchainKHR;
  auto allocator = &graphics.GetAllocator();

  graphics.DiscardResource( [ device, swapchain, allocator ]() {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: swap chain->%p\n",
                   static_cast< void* >(swapchain) )
    device.destroySwapchainKHR( swapchain, allocator );
  } );

  return false;
}


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
