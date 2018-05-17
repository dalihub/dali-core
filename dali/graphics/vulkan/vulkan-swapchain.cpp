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
#include <dali/graphics/vulkan/vulkan-queue.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-swapchain.h>
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
  CommandBufferRef masterCmdBuffer;

  /*
   * Each buffer has a command pool to allocate from
   */
  CommandPoolRef masterCommandPool;

  /*
   * Framebuffer object associated with the buffer
   */
  FramebufferRef framebuffer;

  /*
   * Sync primitives
   */
  FenceRef endOfFrameFence;

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
        Graphics&  graphics,
        Queue&     presentationQueue,
        SurfaceRef surface,
        uint32_t   bufferCount,
        uint32_t   flags )
  : mOwner( owner ),
    mGraphics( graphics ),
    mQueue( presentationQueue ),
    mSurface( surface ),
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

    PrepareFramebuffers();

    mFirstPresent = true;
    return true;
  }

  bool InitialiseSwapchainBuffers()
  {
    mSwapchainBuffer.clear();
    for( auto&& fb : mFramebuffers )
    {
      auto cmdPool   = CommandPool::New( mGraphics, vk::CommandPoolCreateInfo{}.setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer ) );
      auto masterCmd = cmdPool->NewCommandBuffer( true );

      auto swapBuffer              = SwapchainBuffer{};
      swapBuffer.framebuffer       = fb;
      swapBuffer.index             = 0;
      swapBuffer.masterCmdBuffer   = masterCmd;
      swapBuffer.masterCommandPool = cmdPool;
      swapBuffer.endOfFrameFence   = Fence::New( mGraphics );
      swapBuffer.firstUse          = true;
      mSwapchainBuffer.emplace_back( swapBuffer );
    }

    return true;
  }

  void PrepareFramebuffers()
  {
    /*
     * After creating the new swapchain we need to make sure
     * the layout of images is correct to start with. To do so
     * we will wait till the whole device is idle ( there should
     * be a mechanism preventing from using the GPU past that point )
     * and submit pipeline barriers setting up the layouts of
     * all framebuffer images in one go. There will be no fancy
     * synchronisation here as it's not really needed at this point.
     * Waiting for queue or device idle should be enough.
     */

    const auto& device    = mGraphics.GetDevice();

    device.waitIdle();

    /*
     * Create temporary command pool
     */
    auto commandPool = CommandPool::New( mGraphics );
    auto cmdBuffer   = commandPool->NewCommandBuffer();

    std::vector<vk::ImageMemoryBarrier> barriers;
    ImageViewRef                        depthStencilImage{};

    for( auto&& buffer : mSwapchainBuffer )
    {
      auto colorImages = buffer.framebuffer->GetAttachments( Framebuffer::AttachmentType::COLOR );

      // expecting to use one depth stencil image for all swapbuffers
      if( !depthStencilImage )
      {
        depthStencilImage = buffer.framebuffer->GetAttachment( Framebuffer::AttachmentType::DEPTH_STENCIL, 0u );
      }

      /*
       * Add barriers for color images
       */
      for( auto&& colorImageView : colorImages )
      {
        auto image   = colorImageView->GetImage();
        auto vkImage = image->GetVkHandle();

        vk::ImageSubresourceRange range;
        range.setLayerCount( image->GetLayerCount() )
          .setLevelCount( image->GetLevelCount() )
          .setBaseMipLevel( 0 )
          .setBaseArrayLayer( 0 )
          .setAspectMask( vk::ImageAspectFlagBits::eColor );
        auto colorBarrier = vk::ImageMemoryBarrier{}
                              .setImage( vkImage )
                              .setSubresourceRange( range )
                              .setSrcAccessMask( vk::AccessFlags{} )
                              .setDstAccessMask( vk::AccessFlagBits::eColorAttachmentWrite )
                              .setOldLayout( vk::ImageLayout::eUndefined )
                              .setNewLayout( vk::ImageLayout::eColorAttachmentOptimal );

        barriers.emplace_back( colorBarrier );
      }
    }

    /*
     * Add barrier for depth stencil image
     */
    if( depthStencilImage )
    {
      auto image   = depthStencilImage->GetImage();
      auto vkImage = image->GetVkHandle();

      vk::ImageSubresourceRange range;
      range.setLayerCount( image->GetLayerCount() )
        .setLevelCount( image->GetLevelCount() )
        .setBaseMipLevel( 0 )
        .setBaseArrayLayer( 0 )
        .setAspectMask( vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil );
      auto depthStencilBarrier = vk::ImageMemoryBarrier{}
                                   .setImage( vkImage )
                                   .setSubresourceRange( range )
                                   .setSrcAccessMask( vk::AccessFlags{} )
                                   .setDstAccessMask( vk::AccessFlagBits::eDepthStencilAttachmentWrite )
                                   .setOldLayout( vk::ImageLayout::eUndefined )
                                   .setNewLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
      barriers.emplace_back( depthStencilBarrier );
    }

    /*
     * Record command buffer with pipeline barrier
     */
    cmdBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );
    cmdBuffer->PipelineBarrier( vk::PipelineStageFlagBits::eTopOfPipe,
                                vk::PipelineStageFlagBits::eTopOfPipe,
                                vk::DependencyFlags{},
                                std::vector<vk::MemoryBarrier>{},
                                std::vector<vk::BufferMemoryBarrier>{},
                                barriers );
    cmdBuffer->End();

    // use presentation queue to submit the call
    mQueue.Submit( cmdBuffer, FenceRef{} );
    mQueue.WaitIdle();
  }

  bool SetImageFormat()
  {
    // obtain supported image format
    auto formats          = VkAssert( mGraphics.GetPhysicalDevice().getSurfaceFormatsKHR( mSurface->GetSurfaceKHR() ) );
    mSwapchainImageFormat = vk::Format::eUndefined;

    for( auto&& format : formats )
    {
      if( format.format != vk::Format::eUndefined )
      {
        mSwapchainColorSpace  = format.colorSpace;
        mSwapchainImageFormat = format.format;
        break;
      }
    }

    if( vk::Format::eUndefined == mSwapchainImageFormat )
    {
      return false;
    }

    return true;
  }

  /**
   * Creates swapchain immediately
   * @return
   */
  bool Create()
  {
    const auto& device    = mGraphics.GetDevice();
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
  ImageViewRef CreateDepthStencil()
  {
    // create depth stencil image
    auto dsImageRef = Image::New( mGraphics,
                                  vk::ImageCreateInfo{}
                                    .setFormat( vk::Format::eD24UnormS8Uint )
                                    .setMipLevels( 1 )
                                    .setTiling( vk::ImageTiling::eOptimal )
                                    .setImageType( vk::ImageType::e2D )
                                    .setArrayLayers( 1 )
                                    .setExtent( {mSwapchainExtent.width, mSwapchainExtent.height, 1} )
                                    .setUsage( vk::ImageUsageFlagBits::eDepthStencilAttachment )
                                    .setSharingMode( vk::SharingMode::eExclusive )
                                    .setInitialLayout( vk::ImageLayout::eUndefined )
                                    .setSamples( vk::SampleCountFlagBits::e1 ) );

    auto memory = mGraphics.GetDeviceMemoryManager().GetDefaultAllocator().Allocate(
      dsImageRef, vk::MemoryPropertyFlagBits::eDeviceLocal );

    dsImageRef->BindMemory( memory );

    // create imageview to be used within framebuffer
    auto dsImageViewRef = ImageView::New( mGraphics, dsImageRef );
    return dsImageViewRef;
  }

  /**
   * Creates a Framebuffer and compatible RenderPass
   * @param image
   * @return
   */
  FramebufferRef CreateFramebuffer( vk::Image& image )
  {
    auto fbRef = Framebuffer::New( mGraphics, mSwapchainExtent.width, mSwapchainExtent.height );

    // Create external Image reference
    // Note that despite we don't create VkImage, we still fill the createinfo structure
    // as this data will be used later
    ImageRef imageRef = Image::New( mGraphics,
                                    vk::ImageCreateInfo{}
                                      .setFormat( mSwapchainImageFormat )
                                      .setSamples( vk::SampleCountFlagBits::e1 )
                                      .setInitialLayout( vk::ImageLayout::eUndefined )
                                      .setSharingMode( vk::SharingMode::eExclusive )
                                      .setUsage( vk::ImageUsageFlagBits::eColorAttachment )
                                      .setExtent( {mSwapchainExtent.width, mSwapchainExtent.height, 1} )
                                      .setArrayLayers( 1 )
                                      .setImageType( vk::ImageType::e2D )
                                      .setTiling( vk::ImageTiling::eOptimal )
                                      .setMipLevels( 1 ),
                                    image );

    // Create basic imageview ( all mipmaps, all layers )
    ImageViewRef iv = ImageView::New( mGraphics, imageRef );

    fbRef->SetAttachment( iv, Framebuffer::AttachmentType::COLOR, 0u );

    return fbRef;
  }
#pragma GCC diagnostic pop

  /**
   * This function acquires next framebuffer
   * @todo we should rather use roundrobin method
   * @return
   */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  FramebufferRef AcquireNextFramebuffer()
  {
    const auto& device    = mGraphics.GetDevice();

    if( !mFrameFence )
    {
      mFrameFence = Fence::New( mGraphics );
    }

    mCurrentBufferIndex =
      VkAssert( device.acquireNextImageKHR( mSwapchainKHR, 1000000, nullptr, mFrameFence->GetFence() ) );

    mFrameFence->Wait();
    mFrameFence->Reset();

    auto& swapBuffer = mSwapchainBuffer[mCurrentBufferIndex];

    // start recording
    auto inheritanceInfo = vk::CommandBufferInheritanceInfo{}
      .setFramebuffer(swapBuffer.framebuffer->GetVkHandle() )
      .setRenderPass(swapBuffer.framebuffer->GetRenderPassVkHandle())
      .setSubpass( 0 );
    swapBuffer.masterCmdBuffer->Reset();
    swapBuffer.masterCmdBuffer->Begin( vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inheritanceInfo );

    // change layout from present to color attachment if not done yet
    // ( swapchain must track that )
    if(!swapBuffer.firstUse)
    {
      UpdateLayoutPresentToColorAttachment(swapBuffer);
    }
    swapBuffer.firstUse = false;

    return swapBuffer.framebuffer;
  }

  void BeginPrimaryRenderPass()
  {
    auto& currentBuffer = mSwapchainBuffer[mCurrentBufferIndex];

    vk::RenderPassBeginInfo rpInfo{};
    rpInfo.setRenderPass(currentBuffer.framebuffer->GetRenderPassVkHandle() )
      .setFramebuffer(currentBuffer.framebuffer->GetVkHandle() )
      .setPClearValues( currentBuffer.framebuffer->GetDefaultClearValues().data() )
      .setClearValueCount( U32( currentBuffer.framebuffer->GetDefaultClearValues().size() ) )
      .setRenderArea( vk::Rect2D( {0, 0}, mSurface->GetSize() ) );

    currentBuffer.masterCmdBuffer->BeginRenderPass( rpInfo, vk::SubpassContents::eSecondaryCommandBuffers );
  }

  void BeginPrimaryRenderPass( std::vector<std::array<float,4>> colors )
  {
    auto& currentBuffer = mSwapchainBuffer[mCurrentBufferIndex];

    vk::RenderPassBeginInfo rpInfo{};

    auto newColors = currentBuffer.framebuffer->GetDefaultClearValues();
    newColors[0].color.setFloat32( { colors[0][0],
                                     colors[0][1],
                                     colors[0][2],
                                     colors[0][3]
                                   } );

    rpInfo.setRenderArea( vk::Rect2D( {0, 0}, mSurface->GetSize() ) )
          .setRenderPass( currentBuffer.framebuffer->GetVkRenderPass() )
          .setFramebuffer( currentBuffer.framebuffer->GetVkFramebuffer() )
          .setPClearValues( newColors.data() )
          .setClearValueCount( U32( currentBuffer.framebuffer->GetDefaultClearValues().size() ) );

    currentBuffer.masterCmdBuffer->BeginRenderPass( rpInfo, vk::SubpassContents::eSecondaryCommandBuffers );
  }

  void EndPrimaryRenderPass( SwapchainBuffer& currentBuffer )
  {
    currentBuffer.masterCmdBuffer->EndRenderPass();
  }

  /**
   * Performs layout transition for all the color attachments
   * in the current framebuffer.
   * The master command buffer must be in the recording state
   * @param swapBuffer
   */
  void UpdateLayoutPresentToColorAttachment( SwapchainBuffer& swapBuffer )
  {
    auto& cmdBuf = swapBuffer.masterCmdBuffer;

    //todo: test the state of th ebuffer, must be recording

    auto attachments = swapBuffer.framebuffer->GetAttachments( Framebuffer::AttachmentType::COLOR );

    std::vector<vk::ImageMemoryBarrier> barriers;
    vk::ImageMemoryBarrier              barrier;
    barrier.setSrcAccessMask( vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eColorAttachmentRead )
      .setDstAccessMask( vk::AccessFlagBits::eColorAttachmentWrite )
      .setOldLayout( vk::ImageLayout::ePresentSrcKHR )
      .setNewLayout( vk::ImageLayout::eColorAttachmentOptimal )
      .setSubresourceRange( vk::ImageSubresourceRange{}.setAspectMask( vk::ImageAspectFlagBits::eColor ).setBaseArrayLayer(0)
                                                       .setBaseMipLevel(0)
                                                       .setLevelCount(1)
                                                       .setLayerCount(1));

    for( auto&& imageView : attachments )
    {
      barriers.emplace_back( barrier.setImage( *imageView->GetImage() ) );
    }

    cmdBuf->PipelineBarrier( vk::PipelineStageFlagBits::eBottomOfPipe,
                             vk::PipelineStageFlagBits::eColorAttachmentOutput,
                             vk::DependencyFlags{},
                             std::vector<vk::MemoryBarrier>{},
                             std::vector<vk::BufferMemoryBarrier>{},
                             barriers );
  }

  CommandBufferRef GetPrimaryCommandBuffer() const
  {
    return mSwapchainBuffer[mCurrentBufferIndex].masterCmdBuffer;
  }

  bool Present()
  {
    auto& swapBuffer = mSwapchainBuffer[mCurrentBufferIndex];

    // end render pass
    EndPrimaryRenderPass( swapBuffer );

    // end command buffer
    swapBuffer.masterCmdBuffer->End();

    // submit
    swapBuffer.endOfFrameFence->Reset();
    mQueue.Submit( swapBuffer.masterCmdBuffer, swapBuffer.endOfFrameFence );
    swapBuffer.endOfFrameFence->Wait( 0u );

    // fixme: use semaphores to synchronize all previously submitted command buffers!
    vk::PresentInfoKHR presentInfo{};
    vk::Result result;
    presentInfo.setPImageIndices( &mCurrentBufferIndex )
      .setPResults( &result )
      .setPSwapchains( &mSwapchainKHR )
      .setSwapchainCount( 1 )
      .setPWaitSemaphores( nullptr )
      .setWaitSemaphoreCount( 0 );
    mQueue.Present( presentInfo );

    // just to speed things up :P
    mQueue.WaitIdle();

    return true;
  }

  // same as present but additionally waits for semaphores
  // needed when present queue is different from graphics queue
  bool Present( std::vector<vk::Semaphore> semaphores )
  {
    vk::PresentInfoKHR presentInfo{};
    vk::Result         result{};
    presentInfo.setPImageIndices( &mCurrentBufferIndex )
      .setPResults( &result )
      .setPSwapchains( &mSwapchainKHR )
      .setSwapchainCount( 1 )
      .setPWaitSemaphores( nullptr )
      .setWaitSemaphoreCount( 0 );

    mQueue.Present( presentInfo );

    return true;
  }

  Swapchain& mOwner;
  Graphics&  mGraphics;
  Queue&     mQueue;
  SurfaceRef mSurface;

  uint32_t mBufferCount;
  uint32_t mFlags;
  uint32_t mCurrentBufferIndex;

  FenceRef mFrameFence;

  // swapchain framebuffers
  std::vector<FramebufferRef> mFramebuffers;

  vk::SwapchainKHR           mSwapchainKHR;
  vk::SwapchainCreateInfoKHR mSwapchainCreateInfoKHR;

  vk::Format        mSwapchainImageFormat;
  vk::ColorSpaceKHR mSwapchainColorSpace;
  vk::Extent2D      mSwapchainExtent;

  std::vector<SwapchainBuffer> mSwapchainBuffer;

  bool mFirstPresent;
};

/**
 * Swapchain API
 */
SwapchainRef Swapchain::New(
  Graphics& graphics, Queue& presentationQueue, SurfaceRef surface, uint8_t bufferCount, uint32_t flags )
{
  auto retval = SwapchainRef( new Swapchain( graphics, presentationQueue, surface, bufferCount, flags ) );

  if( retval->mImpl->Initialise() )
  {
    return retval;
  }

  return SwapchainRef();
}

Swapchain::Swapchain(
  Graphics& graphics, Queue& presentationQueue, SurfaceRef surface, uint8_t bufferCount, uint32_t flags )
{
  mImpl = std::make_unique<Impl>( *this, graphics, presentationQueue, surface, bufferCount, flags );
}

Swapchain::Swapchain()  = default;
Swapchain::~Swapchain() = default;

FramebufferRef Swapchain::GetCurrentFramebuffer() const
{
  return GetFramebuffer( mImpl->mCurrentBufferIndex );
}

FramebufferRef Swapchain::GetFramebuffer( uint32_t index ) const
{
  return mImpl->mSwapchainBuffer[index].framebuffer;
}

FramebufferRef Swapchain::AcquireNextFramebuffer()
{
  return mImpl->AcquireNextFramebuffer();
}

void Swapchain::Present()
{
  mImpl->Present();
}

void Swapchain::Present( std::vector<vk::Semaphore> waitSemaphores )
{
  mImpl->Present( waitSemaphores );
}

CommandBufferRef Swapchain::GetPrimaryCommandBuffer() const
{
  return mImpl->GetPrimaryCommandBuffer();
}

void Swapchain::BeginPrimaryRenderPass()
{
  mImpl->BeginPrimaryRenderPass( );
}

void Swapchain::BeginPrimaryRenderPass( std::vector<std::array<float,4>> colors )
{
  mImpl->BeginPrimaryRenderPass( colors );
}


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
