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
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-swapchain.h>
#include <dali/graphics/vulkan/vulkan-queue.h>
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
};

struct Swapchain::Impl
{
  Impl( Swapchain& owner,
        Graphics&  graphics,
        Queue&     presentationQueue,
        Surface&   surface,
        uint32_t   bufferCount,
        uint32_t   flags )
  : mOwner( owner ),
    mGraphics( graphics ),
    mQueue( presentationQueue ),
    mSurface( surface ),
    mBufferCount( bufferCount ),
    mFlags( flags )
  {
    mSwapchainCreateInfoKHR.setSurface( mSurface.GetSurfaceKHR() )
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
    mSwapchainExtent = mSurface.GetSize();

    mSwapchainCreateInfoKHR.setImageFormat( mSwapchainImageFormat );
    mSwapchainCreateInfoKHR.setImageExtent( mSwapchainExtent );
    mSwapchainCreateInfoKHR.setImageColorSpace( mSwapchainColorSpace );

    Create();

    InitialiseSwapchainBuffers();

    PrepareFramebuffers();

    return true;
  }

  bool InitialiseSwapchainBuffers()
  {
    mSwapchainBuffer.clear();
    for( auto&& fb : mFramebuffers )
    {
      auto cmdPool   = CommandPool::New( mGraphics );
      auto masterCmd = cmdPool->NewCommandBuffer( true );

      auto swapBuffer              = SwapchainBuffer{};
      swapBuffer.framebuffer       = fb;
      swapBuffer.index             = 0;
      swapBuffer.masterCmdBuffer   = masterCmd;
      swapBuffer.masterCommandPool = cmdPool;
      swapBuffer.endOfFrameFence   = Fence::New( mGraphics );
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
    const auto& allocator = mGraphics.GetAllocator();

    device.waitIdle();

    /*
     * Create temporary command pool
     */
    auto commandPool = CommandPool::New( mGraphics );
    auto cmdBuffer   = commandPool->NewCommandBuffer();

    std::vector<vk::ImageMemoryBarrier> barriers;
    std::vector<ImageViewRef>           colorImages;
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
        auto vkImage = image->GetVkImage();

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
      auto vkImage = image->GetVkImage();

      vk::ImageSubresourceRange range;
      range.setLayerCount( image->GetLayerCount() )
           .setLevelCount( image->GetLevelCount() )
           .setBaseMipLevel( 0 )
           .setBaseArrayLayer( 0 )
           .setAspectMask( vk::ImageAspectFlagBits::eDepth|vk::ImageAspectFlagBits::eStencil );
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
    cmdBuffer->PipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::DependencyFlags{},
      std::vector<vk::MemoryBarrier>{},
      std::vector<vk::BufferMemoryBarrier>{},
      barriers
    );
    cmdBuffer->End();

    // use presentation queue to submit the call
    mQueue.Submit( cmdBuffer, FenceRef{} );
    mQueue.WaitIdle();
  }

  bool SetImageFormat()
  {
    // obtain supported image format
    auto formats          = VkAssert( mGraphics.GetPhysicalDevice().getSurfaceFormatsKHR( mSurface.GetSurfaceKHR() ) );
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

  Swapchain& mOwner;
  Graphics&  mGraphics;
  Queue&     mQueue;
  Surface&   mSurface;

  uint32_t mBufferCount;
  uint32_t mFlags;

  // swapchain framebuffers
  std::vector<FramebufferRef> mFramebuffers;

  vk::SwapchainKHR           mSwapchainKHR;
  vk::SwapchainCreateInfoKHR mSwapchainCreateInfoKHR;

  vk::Format        mSwapchainImageFormat;
  vk::ColorSpaceKHR mSwapchainColorSpace;
  vk::Extent2D      mSwapchainExtent;

  std::vector<SwapchainBuffer> mSwapchainBuffer;
};

/**
 * Swapchain API
 */
SwapchainRef Swapchain::New(
  Graphics& graphics, Queue& presentationQueue, Surface& surface, uint8_t bufferCount, uint32_t flags )
{
  auto retval = SwapchainRef( new Swapchain( graphics, presentationQueue, surface, bufferCount, flags ) );

  if( retval->mImpl->Initialise() )
  {
    return retval;
  }

  return SwapchainRef();
}

Swapchain::Swapchain(
  Graphics& graphics, Queue& presentationQueue, Surface& surface, uint8_t bufferCount, uint32_t flags )
{
  mImpl = std::make_unique<Impl>( *this, graphics, presentationQueue, surface, bufferCount, flags );
}

Swapchain::Swapchain()  = default;
Swapchain::~Swapchain() = default;

FramebufferRef Swapchain::GetCurrentFramebuffer() const
{
}

FramebufferRef Swapchain::GetFramebuffer( uint32_t index ) const
{
}

FramebufferRef Swapchain::AcquireNextFramebuffer()
{
}

void Swapchain::Present( const QueueRef& queue )
{
}

void Swapchain::Present()
{
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
