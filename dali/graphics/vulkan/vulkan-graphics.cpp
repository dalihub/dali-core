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
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-queue.h>
#include <dali/graphics/vulkan/internal/vulkan-surface.h>
#include <dali/integration-api/graphics/vulkan/vk-surface-factory.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-manager.h>

#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-pipeline.h>
#include <dali/graphics/vulkan/internal/vulkan-shader.h>
#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-resource-register.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/internal/vulkan-swapchain.h>

#include <dali/graphics-api/graphics-api-controller.h>

#ifndef VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#define VK_KHR_XLIB_SURFACE_EXTENSION_NAME "VK_KHR_xlib_surface"
#endif

#ifndef VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#define VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME "VK_KHR_wayland_surface"
#endif

#ifndef VK_KHR_XCB_SURFACE_EXTENSION_NAME
#define VK_KHR_XCB_SURFACE_EXTENSION_NAME "VK_KHR_xcb_surface"
#endif

#include <iostream>
#include <utility>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

const auto VALIDATION_LAYERS = std::vector< const char* >{

        //"VK_LAYER_LUNARG_screenshot",           // screenshot
        //"VK_LAYER_RENDERDOC_Capture",
        "VK_LAYER_LUNARG_parameter_validation", // parameter
        //"VK_LAYER_LUNARG_vktrace",              // vktrace ( requires vktrace connection )
        //"VK_LAYER_LUNARG_monitor",             // monitor
        //"VK_LAYER_LUNARG_swapchain",           // swapchain
        //"VK_LAYER_GOOGLE_threading",           // threading
        //"VK_LAYER_LUNARG_api_dump",            // api
        //"VK_LAYER_LUNARG_object_tracker",      // objects
        "VK_LAYER_LUNARG_core_validation",     // core
        //"VK_LAYER_GOOGLE_unique_objects",      // unique objects
        "VK_LAYER_LUNARG_standard_validation", // standard
};

Graphics::Graphics() = default;

Graphics::~Graphics()
{
  mGfxController.reset( nullptr );

  // Wait for everything to finish on the GPU
  DeviceWaitIdle();

  // Manually resetting unique pointer here because we need to control the order of destruction.
  // This defeats the purpose of unique pointers and we might as well use raw pointers. But a unique ptr
  // communicates ownership more clearly (e.g by not allowing copies).
  mGfxController.reset( nullptr );
  mSurfaceFBIDMap.clear();

  mCommandPools.clear();

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "DESTROYING GRAPHICS CONTEXT--------------------------------\n" )
  mResourceRegister->PrintReferenceCountReport();

  // Clear the last references of resources in the cache.
  // This should ensure that all resources have been queued for garbage collection
  // This call assumes that the cash only holds the last reference of every resource in the program. (As it should)
  mResourceRegister->Clear();

  mDeviceMemoryManager.reset( nullptr );

  // Execute any outstanding actions...
  ExecuteActions();

  PrintAllocationReport( *mDescriptorAllocator );

  mDescriptorAllocator.reset( nullptr );

  // Collect the garbage! And shut down gracefully...
  CollectGarbage();

  // We are done with all resources (technically... . If not we will get a ton of validation layer errors)
  // Kill the Vulkan logical device
  mDevice.destroy( mAllocator.get() );

  // Kill the Vulkan instance
  DestroyInstance();

}

// Create methods -----------------------------------------------------------------------------------------------
void Graphics::Create()
{
  auto extensions = PrepareDefaultInstanceExtensions();

  auto layers = vk::enumerateInstanceLayerProperties();
  std::vector< const char* > validationLayers;
  for( auto&& reqLayer : VALIDATION_LAYERS )
  {
    for( auto&& prop : layers.value )
    {
      DALI_LOG_STREAM( gVulkanFilter, Debug::General, prop.layerName );
      if( std::string( prop.layerName ) == reqLayer )
      {
        validationLayers.push_back( reqLayer );
      }
    }
  }

  CreateInstance( extensions, validationLayers );
  PreparePhysicalDevice();
}

void Graphics::CreateDevice()
{
  auto queueInfos = GetQueueCreateInfos();
  {
    auto maxQueueCountPerFamily = 0u;
    for( auto&& info : queueInfos )
    {
      maxQueueCountPerFamily = std::max( info.queueCount, maxQueueCountPerFamily );
    }

    auto priorities = std::vector< float >( maxQueueCountPerFamily );
    std::fill( priorities.begin(), priorities.end(), 1.0f );

    for( auto& info : queueInfos )
    {
      info.setPQueuePriorities( priorities.data() );
    }

    std::vector< const char* > extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    auto info = vk::DeviceCreateInfo{};
    info.setEnabledExtensionCount( U32( extensions.size() ) )
        .setPpEnabledExtensionNames( extensions.data() )
        .setPEnabledFeatures( &( *mPhysicalDeviceFeatures ) )
        .setPQueueCreateInfos( queueInfos.data() )
        .setQueueCreateInfoCount( U32( queueInfos.size() ) );

    mDevice = VkAssert( mPhysicalDevice.createDevice( info, *mAllocator ) );
  }

  // create Queue objects
  for( auto& queueInfo : queueInfos )
  {
    for( auto i = 0u; i < queueInfo.queueCount; ++i )
    {
      auto queue = mDevice.getQueue( queueInfo.queueFamilyIndex, i );

      // based on family push queue instance into right array
      auto flags = mQueueFamilyProperties[queueInfo.queueFamilyIndex].queueFlags;
      if( flags & vk::QueueFlagBits::eGraphics )
      {
        mGraphicsQueues.emplace_back(
                std::unique_ptr< Queue >( new Queue( *this, queue, queueInfo.queueFamilyIndex, i, flags ) ) );
      }
      if( flags & vk::QueueFlagBits::eTransfer )
      {
        mTransferQueues.emplace_back(
                std::unique_ptr< Queue >( new Queue( *this, queue, queueInfo.queueFamilyIndex, i, flags ) ) );
      }
      if( flags & vk::QueueFlagBits::eCompute )
      {
        mComputeQueues.emplace_back(
                std::unique_ptr< Queue >( new Queue( *this, queue, queueInfo.queueFamilyIndex, i, flags ) ) );
      }

      // todo: present queue
    }
  }

  mResourceRegister = std::unique_ptr< ResourceRegister >( new ResourceRegister );
  mDescriptorAllocator = MakeUnique< DescriptorSetAllocator >( *this, 10u );
}

FBID Graphics::CreateSurface( std::unique_ptr< SurfaceFactory > surfaceFactory,
                              const Integration::Graphics::GraphicsCreateInfo& createInfo )
{
  // create surface from the factory
  auto surface = new Surface( *this, std::move( surfaceFactory ) );

  if( !surface->mVulkanSurfaceFactory )
  {
    return -1;
  }

  surface->mSurface = surface->mVulkanSurfaceFactory->Create( mInstance,
                                                              mAllocator.get(),
                                                              mPhysicalDevice );

  if( !surface->mSurface )
  {
    return -1;
  }

  surface->mCapabilities = VkAssert( mPhysicalDevice.getSurfaceCapabilitiesKHR( surface->mSurface ) );

   // If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
  if( surface->mCapabilities.currentExtent.width == std::numeric_limits< uint32_t >::max() )
  {
    surface->mCapabilities.currentExtent.width = std::max( surface->mCapabilities.minImageExtent.width,
                                   std::min( surface->mCapabilities.maxImageExtent.width, createInfo.surfaceWidth ) );

    surface->mCapabilities.currentExtent.height = std::max( surface->mCapabilities.minImageExtent.height,
                                    std::min( surface->mCapabilities.maxImageExtent.height, createInfo.surfaceHeight ) );

  }

  // map surface to FBID
  auto fbid = ++mBaseFBID;

  mSurfaceFBIDMap[ fbid ] = SwapchainSurfacePair{ RefCountedSwapchain{}, RefCountedSurface( surface ) };


  if( createInfo.depthStencilMode == Integration::Graphics::DepthStencilMode::DEPTH_OPTIMAL ||
      createInfo.depthStencilMode == Integration::Graphics::DepthStencilMode::DEPTH_STENCIL_OPTIMAL )
  {
    mHasDepth = true;
  }
  else
  {
    mHasDepth = false;
  }

  if( createInfo.depthStencilMode == Integration::Graphics::DepthStencilMode::DEPTH_STENCIL_OPTIMAL )
  {
    mHasStencil = true;
  }

  return fbid;
}

RefCountedSwapchain Graphics::CreateSwapchainForSurface( RefCountedSurface surface )
{

  auto surfaceCapabilities = surface->GetCapabilities();

  //TODO: propagate the format and presentation mode to higher layers to allow for more control?
  auto swapchain = CreateSwapchain( surface,
                                    vk::Format::eB8G8R8A8Unorm,
                                    vk::PresentModeKHR::eFifo,
                                    surfaceCapabilities.minImageCount+1, // Temp Fix to cope with incorrect image count returned by device
                                    Dali::Graphics::Vulkan::RefCountedSwapchain() );

  // store swapchain in the correct pair
  for( auto&& val : mSurfaceFBIDMap )
  {
    if( val.second.surface == surface )
    {
      val.second.swapchain = swapchain;
      break;
    }
  }

  return swapchain;
}

void Graphics::InitialiseController()
{
  mGfxController->Initialise();
}

RefCountedSwapchain Graphics::ReplaceSwapchainForSurface( RefCountedSurface surface, RefCountedSwapchain&& oldSwapchain )
{
  auto surfaceCapabilities = surface->GetCapabilities();

  auto swapchain = CreateSwapchain( surface,
                                    vk::Format::eB8G8R8A8Unorm,
                                    vk::PresentModeKHR::eFifo,
                                    surfaceCapabilities.minImageCount+1, // Temp Fix to cope with incorrect image count returned by device
                                    std::move(oldSwapchain) );


  // store swapchain in the correct pair
  for( auto&& val : mSurfaceFBIDMap )
  {
    if( val.second.surface == surface )
    {
      val.second.swapchain = swapchain;
      break;
    }
  }

  return swapchain;
}


RefCountedFence Graphics::CreateFence( const vk::FenceCreateInfo& fenceCreateInfo )
{
  auto fence = new Fence( *this );

  VkAssert( mDevice.createFence( &fenceCreateInfo, mAllocator.get(), &fence->mFence ) );

  auto refCountedFence = RefCountedFence( fence );

  return refCountedFence;
}

RefCountedBuffer Graphics::CreateBuffer( size_t size, BufferType type )
{
  auto usageFlags = vk::BufferUsageFlags{};

  switch( type )
  {
    case BufferType::VERTEX:
    {
      usageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
      break;
    };
    case BufferType::INDEX:
    {
      usageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;
      break;
    };
    case BufferType::UNIFORM:
    {
      usageFlags |= vk::BufferUsageFlagBits::eUniformBuffer;
      break;
    };
    case BufferType::SHADER_STORAGE:
    {
      usageFlags |= vk::BufferUsageFlagBits::eStorageBuffer;
      break;
    };
  }

  auto info = vk::BufferCreateInfo{};
  info.setSharingMode( vk::SharingMode::eExclusive );
  info.setSize( size );
  info.setUsage( usageFlags | vk::BufferUsageFlagBits::eTransferDst );

  auto buffer = new Buffer( *this, info );

  VkAssert( mDevice.createBuffer( &info, mAllocator.get(), &buffer->mBuffer ) );

  auto refCountedBuffer = RefCountedBuffer( buffer );
  AddBuffer( *buffer );

  return refCountedBuffer;
}

RefCountedBuffer Graphics::CreateBuffer( const vk::BufferCreateInfo& bufferCreateInfo )
{
  auto buffer = new Buffer( *this, bufferCreateInfo );

  VkAssert( mDevice.createBuffer( &bufferCreateInfo, mAllocator.get(), &buffer->mBuffer ) );

  auto refCountedBuffer = RefCountedBuffer( buffer );

  AddBuffer( *buffer );

  return refCountedBuffer;
}

RefCountedFramebuffer Graphics::CreateFramebuffer(
        const std::vector< RefCountedFramebufferAttachment >& colorAttachments,
        RefCountedFramebufferAttachment depthAttachment,
        uint32_t width,
        uint32_t height,
        vk::RenderPass externalRenderPass )
{
  assert( ( !colorAttachments.empty() || depthAttachment )
          && "Cannot create framebuffer. Please provide at least one attachment" );

  auto colorAttachmentsValid = true;
  for( auto& attachment : colorAttachments )
  {
    if( !attachment->IsValid() )
    {
      colorAttachmentsValid = false;
      break;
    }
  }

  assert( colorAttachmentsValid && "Invalid color attachment! The attachment has no ImageView" );

  // Flag that indicates if the framebuffer has a depth attachment
  auto hasDepth = false;
  if( depthAttachment )
  {
    hasDepth = depthAttachment->IsValid();
    assert( hasDepth && "Invalid depth attachment! The attachment has no ImageView" );
  }

  auto renderPass = externalRenderPass;

  // Flag that indicates if the render pass is externally provided
  auto isRenderPassExternal = externalRenderPass != vk::RenderPass{};

  // The total number of attachments
  auto totalAttachmentCount = hasDepth ? colorAttachments.size() + 1 : colorAttachments.size();

  // This vector stores the attachments (vk::ImageViews)
  auto attachments = std::vector< vk::ImageView >{};
  attachments.reserve( totalAttachmentCount );

  // If no external render pass was provided, create one internally
  if( !isRenderPassExternal )
  {
    // This vector stores the attachment references
    auto colorAttachmentReferences = std::vector< vk::AttachmentReference >{};
    colorAttachmentReferences.reserve( colorAttachments.size() );

    // This vector stores the attachment descriptions
    auto attachmentDescriptions = std::vector< vk::AttachmentDescription >{};
    attachmentDescriptions.reserve( totalAttachmentCount );

    // For each color attachment...
    for( auto i = 0u; i < colorAttachments.size(); ++i )
    {
      // Get the image layout
      auto imageLayout = colorAttachments[i]->GetImageView()->GetImage()->GetImageLayout();

      // If the layout is undefined...
      if( imageLayout == vk::ImageLayout::eUndefined )
      {
        // Set it to color attachment optimal
        imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
      }

      // Any other case should be invalid
      assert( imageLayout == vk::ImageLayout::eColorAttachmentOptimal );

      // Add a reference and a descriptions and image views to their respective vectors
      colorAttachmentReferences.push_back( vk::AttachmentReference{}.setLayout( imageLayout )
                                                                    .setAttachment( U32( i ) ) );

      attachmentDescriptions.push_back( colorAttachments[i]->GetDescription() );

      attachments.push_back( colorAttachments[i]->GetImageView()->GetVkHandle() );
    }


    // Follow the exact same procedure as color attachments
    auto depthAttachmentReference = vk::AttachmentReference{};
    if( hasDepth )
    {
      auto imageLayout = depthAttachment->GetImageView()->GetImage()->GetImageLayout();

      if( imageLayout == vk::ImageLayout::eUndefined )
      {
        imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
      }

      assert( imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal );

      depthAttachmentReference.setLayout( imageLayout );
      depthAttachmentReference.setAttachment( U32( colorAttachmentReferences.size() ) );

      attachmentDescriptions.push_back( depthAttachment->GetDescription() );

      attachments.push_back( depthAttachment->GetImageView()->GetVkHandle() );
    }

    // Creating a single subpass per framebuffer
    auto subpassDesc = vk::SubpassDescription{};
    subpassDesc.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics );
    subpassDesc.setColorAttachmentCount( U32( colorAttachments.size()));
    if( hasDepth )
    {
      subpassDesc.setPDepthStencilAttachment( &depthAttachmentReference );
    }
    subpassDesc.setPColorAttachments( colorAttachmentReferences.data() );

    // Creating 2 subpass dependencies using VK_SUBPASS_EXTERNAL to leverage the implicit image layout
    // transitions provided by the driver
    std::array< vk::SubpassDependency, 2 > subpassDependencies{

            vk::SubpassDependency{}.setSrcSubpass( VK_SUBPASS_EXTERNAL )
                                   .setDstSubpass( 0 )
                                   .setSrcStageMask( vk::PipelineStageFlagBits::eBottomOfPipe )
                                   .setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
                                   .setSrcAccessMask( vk::AccessFlagBits::eMemoryRead )
                                   .setDstAccessMask( vk::AccessFlagBits::eColorAttachmentRead |
                                                      vk::AccessFlagBits::eColorAttachmentWrite )
                                   .setDependencyFlags( vk::DependencyFlagBits::eByRegion ),

            vk::SubpassDependency{}.setSrcSubpass( 0 )
                                   .setDstSubpass( VK_SUBPASS_EXTERNAL )
                                   .setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
                                   .setDstStageMask( vk::PipelineStageFlagBits::eBottomOfPipe )
                                   .setSrcAccessMask( vk::AccessFlagBits::eColorAttachmentRead |
                                                      vk::AccessFlagBits::eColorAttachmentWrite )
                                   .setDstAccessMask( vk::AccessFlagBits::eMemoryRead )
                                   .setDependencyFlags( vk::DependencyFlagBits::eByRegion )

    };


    // Create the render pass
    auto renderPassCreateInfo = vk::RenderPassCreateInfo{}.setAttachmentCount( U32( attachmentDescriptions.size() ) )
                                                          .setPAttachments( attachmentDescriptions.data() )
                                                          .setPSubpasses( &subpassDesc )
                                                          .setSubpassCount( 1 )
                                                          .setPDependencies( subpassDependencies.data() );


    renderPass = VkAssert( mDevice.createRenderPass( renderPassCreateInfo, mAllocator.get() ) );
  }

  // Finally create the framebuffer
  auto framebufferCreateInfo = vk::FramebufferCreateInfo{}.setRenderPass( renderPass )
                                                          .setPAttachments( attachments.data() )
                                                          .setLayers( 1 )
                                                          .setWidth( width )
                                                          .setHeight( height )
                                                          .setAttachmentCount( U32( attachments.size() ) );

  auto framebuffer = VkAssert( mDevice.createFramebuffer( framebufferCreateInfo, mAllocator.get() ) );

  return RefCountedFramebuffer( new Framebuffer( *this,
                                                 colorAttachments,
                                                 depthAttachment,
                                                 framebuffer,
                                                 renderPass,
                                                 width,
                                                 height,
                                                 isRenderPassExternal ) );
}

RefCountedImage Graphics::CreateImage( const vk::ImageCreateInfo& imageCreateInfo )
{
  auto image = new Image( *this, imageCreateInfo );

  VkAssert( mDevice.createImage( &imageCreateInfo, mAllocator.get(), &image->mImage ) );

  auto refCountedImage = RefCountedImage( image );
  AddImage( *image );

  return refCountedImage;
}

RefCountedImage Graphics::CreateImageFromExternal( vk::Image externalImage,
                                                   vk::Format imageFormat,
                                                   vk::Extent2D extent )
{
  auto imageCreateInfo = vk::ImageCreateInfo{}
          .setFormat( imageFormat )
          .setSamples( vk::SampleCountFlagBits::e1 )
          .setInitialLayout( vk::ImageLayout::eUndefined )
          .setSharingMode( vk::SharingMode::eExclusive )
          .setUsage( vk::ImageUsageFlagBits::eColorAttachment )
          .setExtent( { extent.width, extent.height, 1 } )
          .setArrayLayers( 1 )
          .setImageType( vk::ImageType::e2D )
          .setTiling( vk::ImageTiling::eOptimal )
          .setMipLevels( 1 );

  return RefCountedImage(new Image( *this, imageCreateInfo, externalImage ) );
}

RefCountedImageView Graphics::CreateImageView( const vk::ImageViewCreateFlags& flags,
                                               const RefCountedImage& image,
                                               vk::ImageViewType viewType,
                                               vk::Format format,
                                               vk::ComponentMapping components,
                                               vk::ImageSubresourceRange subresourceRange )
{
  auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
          .setFlags( flags )
          .setImage( image->GetVkHandle() )
          .setViewType( viewType )
          .setFormat( format )
          .setComponents( components )
          .setSubresourceRange( std::move( subresourceRange ) );

  auto imageView = new ImageView( *this, image, imageViewCreateInfo );

  VkAssert( mDevice.createImageView( &imageViewCreateInfo, nullptr, &imageView->mImageView ) );

  auto refCountedImageView = RefCountedImageView( imageView );

  AddImageView( *imageView );

  return refCountedImageView;
}

RefCountedImageView Graphics::CreateImageView( RefCountedImage image )
{
  vk::ComponentMapping componentsMapping = { vk::ComponentSwizzle::eR,
                                             vk::ComponentSwizzle::eG,
                                             vk::ComponentSwizzle::eB,
                                             vk::ComponentSwizzle::eA };


  auto subresourceRange = vk::ImageSubresourceRange{}
          .setAspectMask( image->GetAspectFlags() )
          .setBaseArrayLayer( 0 )
          .setBaseMipLevel( 0 )
          .setLevelCount( image->GetMipLevelCount() )
          .setLayerCount( image->GetLayerCount() );

  auto refCountedImageView = CreateImageView( {},
                                              image,
                                              vk::ImageViewType::e2D,
                                              image->GetFormat(),
                                              componentsMapping,
                                              subresourceRange );

  return refCountedImageView;
}

RefCountedSampler Graphics::CreateSampler( const vk::SamplerCreateInfo& samplerCreateInfo )
{
  auto sampler = new Sampler( *this, samplerCreateInfo );

  VkAssert( mDevice.createSampler( &samplerCreateInfo, mAllocator.get(), &sampler->mSampler ) );

  auto refCountedSampler = RefCountedSampler( sampler );

  AddSampler( *sampler );

  return refCountedSampler;

}

RefCountedCommandBuffer Graphics::CreateCommandBuffer( bool primary )
{
  auto commandPool = GetCommandPool( std::this_thread::get_id() );

  return commandPool->NewCommandBuffer( primary );
}

vk::ImageMemoryBarrier Graphics::CreateImageMemoryBarrier( RefCountedImage image,
                                                           vk::ImageLayout oldLayout,
                                                           vk::ImageLayout newLayout )
{
  // This function assumes that all images have 1 mip level and 1 layer
  auto barrier = vk::ImageMemoryBarrier{}
          .setOldLayout( oldLayout )
          .setNewLayout( newLayout )
          .setSrcQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
          .setDstQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
          .setImage( image->GetVkHandle() )
          .setSubresourceRange( vk::ImageSubresourceRange{}.setBaseMipLevel( 0 )
                                                           .setLevelCount( 1 )
                                                           .setBaseArrayLayer( 0 )
                                                           .setLayerCount( 1 ) );

  barrier.subresourceRange.aspectMask = image->GetAspectFlags();

  // The srcAccessMask of the image memory barrier shows which operation
  // must be completed using the old layout, before the transition to the
  // new one happens.
  switch( oldLayout )
  {
    case vk::ImageLayout::eUndefined:
      barrier.srcAccessMask = vk::AccessFlags{};
      break;
    case vk::ImageLayout::ePreinitialized:
      barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
      break;
    case vk::ImageLayout::eColorAttachmentOptimal:
      barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
      break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      barrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
      break;
    case vk::ImageLayout::eTransferSrcOptimal:
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
      break;
    case vk::ImageLayout::eTransferDstOptimal:
      barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
      break;
    default:
      assert( false && "Image layout transition failed: Initial layout not supported." );
  }

  // Destination access mask controls the dependency for the new image layout
  switch( newLayout )
  {
    case vk::ImageLayout::eTransferDstOptimal:
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
      break;
    case vk::ImageLayout::eTransferSrcOptimal:
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
      break;
    case vk::ImageLayout::eColorAttachmentOptimal:
      barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
      break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
      break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      if( barrier.srcAccessMask == vk::AccessFlags{} )
      {
        barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
      }

      barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      break;
    default:
      assert( false && "Image layout transition failed: Target layout not supported." );
  }

  return barrier;
}

RefCountedSwapchain Graphics::CreateSwapchain( RefCountedSurface surface,
                                               vk::Format requestedFormat,
                                               vk::PresentModeKHR presentMode,
                                               uint32_t bufferCount,
                                               RefCountedSwapchain&& oldSwapchain )
{
  // obtain supported image format
  auto supportedFormats = VkAssert( mPhysicalDevice.getSurfaceFormatsKHR( surface->GetVkHandle() ) );

  vk::Format swapchainImageFormat{};
  vk::ColorSpaceKHR swapchainColorSpace{};

  // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
  // there is no preferred format, so we assume vk::Format::eB8G8R8A8Unorm
  if( supportedFormats.size() == 1 && supportedFormats[0].format == vk::Format::eUndefined )
  {
    swapchainColorSpace = supportedFormats[0].colorSpace;
    swapchainImageFormat = vk::Format::eB8G8R8A8Unorm;
  }
  else // Try to find the requested format in the list
  {
    auto found = std::find_if( supportedFormats.begin(),
                               supportedFormats.end(),
                               [ & ]( vk::SurfaceFormatKHR supportedFormat ) {
                                 return requestedFormat == supportedFormat.format;
                               } );

    // If found assign it.
    if( found != supportedFormats.end() )
    {
      auto surfaceFormat = *found;
      swapchainColorSpace = surfaceFormat.colorSpace;
      swapchainImageFormat = surfaceFormat.format;
    }
    else // Requested format not found...attempt to use the first one on the list
    {
      auto surfaceFormat = supportedFormats[0];
      swapchainColorSpace = surfaceFormat.colorSpace;
      swapchainImageFormat = surfaceFormat.format;
    }

  }

  assert( swapchainImageFormat != vk::Format::eUndefined && "Could not find a supported swap chain image format." );

  // Get the surface capabilities to determine some settings of the swap chain
  auto surfaceCapabilities = surface->GetCapabilities();

  // Determine the swap chain extent
  auto swapchainExtent = surfaceCapabilities.currentExtent;

  // Find a supported composite alpha format (not all devices support alpha opaque)
  auto compositeAlpha = vk::CompositeAlphaFlagBitsKHR{};

  // Simply select the first composite alpha format available
  auto compositeAlphaFlags = std::vector< vk::CompositeAlphaFlagBitsKHR >{
          vk::CompositeAlphaFlagBitsKHR::eOpaque,
          vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
          vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
          vk::CompositeAlphaFlagBitsKHR::eInherit
  };


  for( const auto& compositeAlphaFlag : compositeAlphaFlags )
  {

    if( surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag )
    {
      compositeAlpha = compositeAlphaFlag;
      break;
    }

  }

  // Determine the number of images
  if (surfaceCapabilities.minImageCount > 0 &&
      bufferCount > surfaceCapabilities.minImageCount )
  {
      bufferCount = surfaceCapabilities.minImageCount;
  }

  // Find the transformation of the surface
  vk::SurfaceTransformFlagBitsKHR preTransform;
  if( surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity )
  {
    // We prefer a non-rotated transform
    preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
  }
  else
  {
    preTransform = surfaceCapabilities.currentTransform;
  }


  // Check if the requested present mode is supported
  auto presentModes = mPhysicalDevice.getSurfacePresentModesKHR( surface->GetVkHandle() ).value;

  auto found = std::find_if( presentModes.begin(),
                             presentModes.end(),
                             [ & ]( vk::PresentModeKHR mode ) {
                               return presentMode == mode;
                             } );

  if( found == presentModes.end() )
  {
    // Requested present mode not supported. Default to FIFO. FIFO is always supported as per spec.
    presentMode = vk::PresentModeKHR::eFifo;
  }

  // Creation settings have been determined. Fill in the create info struct.
  auto swapChainCreateInfo = vk::SwapchainCreateInfoKHR{}.setSurface( surface->GetVkHandle() )
                                                         .setPreTransform( preTransform )
                                                         .setPresentMode( presentMode )
                                                         .setOldSwapchain( oldSwapchain ? oldSwapchain->GetVkHandle()
                                                                                        : vk::SwapchainKHR{} )
                                                         .setMinImageCount( bufferCount )
                                                         .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )
                                                         .setImageSharingMode( vk::SharingMode::eExclusive )
                                                         .setImageArrayLayers( 1 )
                                                         .setImageColorSpace( swapchainColorSpace )
                                                         .setImageFormat( swapchainImageFormat )
                                                         .setImageExtent( swapchainExtent )
                                                         .setCompositeAlpha( compositeAlpha )
                                                         .setClipped( static_cast<vk::Bool32>(true) )
                                                         .setQueueFamilyIndexCount( 0 )
                                                         .setPQueueFamilyIndices( nullptr );


  // Create the swap chain
  auto swapChainVkHandle = VkAssert( mDevice.createSwapchainKHR( swapChainCreateInfo, mAllocator.get() ) );

  if( oldSwapchain )
  {
    for( auto&& i : mSurfaceFBIDMap )
    {
      if( i.second.swapchain == oldSwapchain )
      {
        i.second.swapchain = RefCountedSwapchain();
        break;
      }
    }
  }

  if( oldSwapchain )
  {
    // prevent destroying the swapchain as it is handled automatically
    // during replacing the swapchain
    auto khr = oldSwapchain->mSwapchainKHR;
    oldSwapchain->mSwapchainKHR = nullptr;
    oldSwapchain.Reset();

    mDevice.destroySwapchainKHR( khr, *mAllocator );
  }

  // pull images and create Framebuffers
  auto images = VkAssert( mDevice.getSwapchainImagesKHR( swapChainVkHandle ) );

  // number of images must match requested buffering mode
  if( images.size() < surfaceCapabilities.minImageCount )
  {
    DALI_LOG_STREAM( gVulkanFilter,
                     Debug::General,
                     "Swapchain creation failed: Swapchain images are less than the requested amount" );
    mDevice.destroySwapchainKHR( swapChainVkHandle );
    return RefCountedSwapchain();
  }

  // Create a Depth/Stencil ( optional )
  auto depthAttachment = RefCountedFramebufferAttachment{};

  if( mHasDepth ) //@ todo fix for stencil
  {
    auto imageCreateInfo = vk::ImageCreateInfo{}
            .setFormat( vk::Format::eD24UnormS8Uint )
            .setMipLevels( 1 )
            .setTiling( vk::ImageTiling::eOptimal )
            .setImageType( vk::ImageType::e2D )
            .setArrayLayers( 1 )
            .setExtent( { swapchainExtent.width, swapchainExtent.height, 1 } )
            .setUsage( vk::ImageUsageFlagBits::eDepthStencilAttachment )
            .setSharingMode( vk::SharingMode::eExclusive )
            .setInitialLayout( vk::ImageLayout::eUndefined )
            .setSamples( vk::SampleCountFlagBits::e1 );

    auto dsRefCountedImage = CreateImage( imageCreateInfo );

    auto memory = mDeviceMemoryManager->GetDefaultAllocator().Allocate( dsRefCountedImage,
                                                                        vk::MemoryPropertyFlagBits::eDeviceLocal );

    BindImageMemory( dsRefCountedImage, memory, 0 );

    // create the depth stencil ImageView to be used within framebuffer
    auto depthStencilImageView = CreateImageView( dsRefCountedImage );
    auto depthClearValue = vk::ClearDepthStencilValue{}.setDepth( 0.0 )
                                                       .setStencil( 1 );

    // A single depth attachment for the swapchain.
    depthAttachment = FramebufferAttachment::NewDepthAttachment( depthStencilImageView, depthClearValue );
  }

  auto framebuffers = std::vector< RefCountedFramebuffer >{};
  framebuffers.reserve( images.size() );

  auto clearColor = vk::ClearColorValue{}.setFloat32( { 0.0f, 0.0f, 0.0f, 1.0f } );

  /*
   * CREATE FRAMEBUFFERS
   */
  for( auto&& image : images )
  {
    auto colorImageView = CreateImageView( CreateImageFromExternal( image, swapchainImageFormat, swapchainExtent ) );

    // A new color attachment for each framebuffer
    auto colorAttachment = FramebufferAttachment::NewColorAttachment( colorImageView,
                                                                      clearColor,
                                                                      true /* presentable */ );

    framebuffers.push_back( CreateFramebuffer( { colorAttachment },
                                               depthAttachment,
                                               swapchainExtent.width,
                                               swapchainExtent.height ) );
  }

  auto swapChainBuffers = std::vector< SwapchainBuffer >{};
  swapChainBuffers.reserve( swapChainBuffers.size() );

  for( const auto& framebuffer : framebuffers )
  {
    auto masterCmd = CreateCommandBuffer( true );

    auto swapBuffer = SwapchainBuffer{};
    swapBuffer.framebuffer = framebuffer;
    swapBuffer.index = 0;
    swapBuffer.masterCmdBuffer = masterCmd;
    swapBuffer.endOfFrameFence = CreateFence( {} );
    swapChainBuffers.push_back( swapBuffer );
  }

  return RefCountedSwapchain( new Swapchain( *this,
                                             GetPresentQueue(),
                                             surface,
                                             swapChainBuffers,
                                             swapChainCreateInfo,
                                             swapChainVkHandle ) );
}
// --------------------------------------------------------------------------------------------------------------

// Actions ------------------------------------------------------------------------------------------------------
vk::Result Graphics::WaitForFence( RefCountedFence fence, uint32_t timeout )
{
  return mDevice.waitForFences( 1, &fence->mFence, VK_TRUE, timeout );
}

vk::Result Graphics::WaitForFences( const std::vector< RefCountedFence >& fences, bool waitAll, uint32_t timeout )
{
  std::vector< vk::Fence > vkFenceHandles{};
  std::transform( fences.begin(),
                  fences.end(),
                  std::back_inserter( vkFenceHandles ),
                  []( RefCountedFence entry ) { return entry->mFence; } );


  return mDevice.waitForFences( vkFenceHandles, vk::Bool32( waitAll ), timeout );
}

vk::Result Graphics::ResetFence( RefCountedFence fence )
{
  return mDevice.resetFences( 1, &fence->mFence );
}

vk::Result Graphics::ResetFences( const std::vector< RefCountedFence >& fences )
{
  std::vector< vk::Fence > vkFenceHandles{};
  std::transform( fences.begin(),
                  fences.end(),
                  std::back_inserter( vkFenceHandles ),
                  []( RefCountedFence entry ) { return entry->mFence; } );

  return mDevice.resetFences( vkFenceHandles );
}

vk::Result Graphics::BindImageMemory( RefCountedImage image, RefCountedGpuMemoryBlock memory, uint32_t offset )
{
  auto result = VkAssert( mDevice.bindImageMemory( image->mImage, *memory, offset ) );
  image->mDeviceMemory = memory;
  return result;
}

vk::Result Graphics::BindBufferMemory( RefCountedBuffer buffer, RefCountedGpuMemoryBlock memory, uint32_t offset )
{
  assert( buffer->mBuffer && "Buffer not initialised!" );
  auto result = VkAssert( mDevice.bindBufferMemory( buffer->mBuffer, *memory, offset ) );
  buffer->mDeviceMemory = memory;
  return result;
}

void* Graphics::MapMemory( RefCountedGpuMemoryBlock memory ) const
{
  return memory->Map();
}

void* Graphics::MapMemory( RefCountedGpuMemoryBlock memory, uint32_t size, uint32_t offset ) const
{
  return memory->Map( offset, size );
}

void Graphics::UnmapMemory( RefCountedGpuMemoryBlock memory ) const
{
  memory->Unmap();
}

RefCountedGpuMemoryBlock Graphics::AllocateMemory( RefCountedBuffer buffer, vk::MemoryPropertyFlags memoryProperties )
{
  return mDeviceMemoryManager->GetDefaultAllocator().Allocate( std::move( buffer ) , memoryProperties );
}

RefCountedGpuMemoryBlock Graphics::AllocateMemory( RefCountedImage image, vk::MemoryPropertyFlags memoryProperties )
{
  return mDeviceMemoryManager->GetDefaultAllocator().Allocate( std::move( image ), memoryProperties );
}

vk::Result Graphics::Submit( Queue& queue, const std::vector< SubmissionData >& submissionData, RefCountedFence fence )
{


  auto submitInfos = std::vector< vk::SubmitInfo >{};
  submitInfos.reserve( submissionData.size() );
  auto commandBufferHandles = std::vector< vk::CommandBuffer >{};

  // prepare memory
  auto bufferSize = 0u;
  for( auto& data : submissionData )
  {
    bufferSize += uint32_t( data.commandBuffers.size() );
  }
  commandBufferHandles.reserve( bufferSize );

  // Transform SubmissionData to vk::SubmitInfo
  for( const auto& subData : submissionData )
  {
    auto currentBufferIndex = commandBufferHandles.size();

    //Extract the command buffer handles
    std::transform( subData.commandBuffers.cbegin(),
                    subData.commandBuffers.cend(),
                    std::back_inserter( commandBufferHandles ),
                    [ & ]( const RefCountedCommandBuffer& entry ) {
                      return entry->GetVkHandle();
                    } );

    auto retval = vk::SubmitInfo().setWaitSemaphoreCount( U32( subData.waitSemaphores.size() ) )
                                  .setPWaitSemaphores( subData.waitSemaphores.data() )
                                  .setPWaitDstStageMask( &subData.waitDestinationStageMask )
                                  .setCommandBufferCount( U32( subData.commandBuffers.size() ) )
                                  .setPCommandBuffers( &commandBufferHandles[currentBufferIndex] )
                                  .setSignalSemaphoreCount( U32( subData.signalSemaphores.size() ) )
                                  .setPSignalSemaphores( subData.signalSemaphores.data() );

    submitInfos.push_back( retval );
  }

  return VkAssert( queue.mQueue.submit( submitInfos, fence ? fence->GetVkHandle() : vk::Fence{} ) );
}

std::vector< RefCountedDescriptorSet >
Graphics::AllocateDescriptorSets( const std::vector< DescriptorSetLayoutSignature >& signatures,
                                  const std::vector< vk::DescriptorSetLayout >& layouts )
{
  return mDescriptorAllocator->AllocateDescriptorSets( signatures, layouts );
}

vk::Result Graphics::Present( Queue& queue, vk::PresentInfoKHR presentInfo )
{
  return queue.mQueue.presentKHR( &presentInfo );
}

vk::Result Graphics::QueueWaitIdle( Queue& queue )
{
  return queue.mQueue.waitIdle();
}

vk::Result Graphics::DeviceWaitIdle()
{
  auto result  = vkDeviceWaitIdle( static_cast<VkDevice>(mDevice) );
  assert( result == VK_SUCCESS );
  return vk::Result( result );
}
// --------------------------------------------------------------------------------------------------------------

// Getters ------------------------------------------------------------------------------------------------------
RefCountedSurface Graphics::GetSurface( FBID surfaceId )
{
  // TODO: FBID == 0 means default framebuffer, but there should be no
  // such thing as default framebuffer.
  if( surfaceId == 0 )
  {
    return mSurfaceFBIDMap.begin()
                          ->second
                          .surface;
  }
  return mSurfaceFBIDMap[surfaceId].surface;
}

RefCountedSwapchain Graphics::GetSwapchainForSurface( RefCountedSurface surface )
{
  for( auto&& val : mSurfaceFBIDMap )
  {
    if( val.second
           .surface == surface )
    {
      return val.second
                .swapchain;
    }
  }
  return RefCountedSwapchain();
}

RefCountedSwapchain Graphics::GetSwapchainForFBID( FBID surfaceId )
{
  if( surfaceId == 0 )
  {
    return mSurfaceFBIDMap.begin()
                          ->second
                          .swapchain;
  }
  return mSurfaceFBIDMap[surfaceId].swapchain;
}

vk::Device Graphics::GetDevice() const
{
  return mDevice;
}

vk::PhysicalDevice Graphics::GetPhysicalDevice() const
{
  return mPhysicalDevice;
}

vk::Instance Graphics::GetInstance() const
{
  return mInstance;
}

const vk::AllocationCallbacks& Graphics::GetAllocator() const
{
  return *mAllocator;
}

const vk::PhysicalDeviceMemoryProperties& Graphics::GetMemoryProperties() const
{
  return *mPhysicalDeviceMemoryProperties;
}

Queue& Graphics::GetGraphicsQueue( uint32_t index ) const
{
  // todo: at the moment each type of queue may use only one, indices greater than 0 are invalid
  // this will change in the future
  assert( index == 0u && "Each type of queue may use only one, indices greater than 0 are invalid!" );

  return *mGraphicsQueues[0]; // will be mGraphicsQueues[index]
}

Queue& Graphics::GetTransferQueue( uint32_t index ) const
{
  // todo: at the moment each type of queue may use only one, indices greater than 0 are invalid
  // this will change in the future
  assert( index == 0u && "Each type of queue may use only one, indices greater than 0 are invalid!" );

  return *mTransferQueues[0]; // will be mGraphicsQueues[index]
}

Queue& Graphics::GetComputeQueue( uint32_t index ) const
{
  // todo: at the moment each type of queue may use only one, indices greater than 0 are invalid
  // this will change in the future
  assert( index == 0u && "Each type of queue may use only one, indices greater than 0 are invalid!" );

  return *mComputeQueues[0]; // will be mGraphicsQueues[index]
}

Queue& Graphics::GetPresentQueue() const
{
  // fixme: should be a dedicated presentation queue
  return GetGraphicsQueue( 0 );
}

Platform Graphics::GetDefaultPlatform() const
{
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
  mPlatform = Platform::WAYLAND;
#elif VK_USE_PLATFORM_XCB_KHR
  mPlatform = Platform::XCB;
#elif VK_USE_PLATFORM_XLIB_KHR
  mPlatform =  Platform::XLIB;
#else
  return mPlatform;
#endif
}

Dali::Graphics::API::Controller& Graphics::GetController()
{
  if( !mGfxController )
  {
    mGfxController = Dali::Graphics::VulkanAPI::Controller::New( *this );
  }

  return *mGfxController;
}

// --------------------------------------------------------------------------------------------------------------

// Cache manipulation methods -----------------------------------------------------------------------------------
void Graphics::AddBuffer( Buffer& buffer )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->AddBuffer( buffer );
}

void Graphics::AddImage( Image& image )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->AddImage( image );
}

void Graphics::AddImageView( ImageView& imageView )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->AddImageView( imageView );
}

void Graphics::AddShader( Shader& shader )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->AddShader( shader );
}

void Graphics::AddCommandPool( RefCountedCommandPool pool )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mCommandPools[ std::this_thread::get_id() ] = std::move(pool);
}

void Graphics::AddDescriptorPool( DescriptorPool& pool )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->AddDescriptorPool( pool );
}

void Graphics::AddFramebuffer( Framebuffer& framebuffer )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->AddFramebuffer( framebuffer );
}

void Graphics::AddSampler( Sampler& sampler )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->AddSampler( sampler );
}

RefCountedShader Graphics::FindShader( vk::ShaderModule shaderModule )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  return mResourceRegister->FindShader( shaderModule );
}

RefCountedImage Graphics::FindImage( vk::Image image )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  return mResourceRegister->FindImage( image );
}

void Graphics::RemoveBuffer( Buffer& buffer )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->RemoveBuffer( buffer );
}

void Graphics::RemoveImage( Image& image )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->RemoveImage( image );
}

void Graphics::RemoveImageView( ImageView& imageView )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->RemoveImageView( imageView );
}

void Graphics::RemoveShader( Shader& shader )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->RemoveShader( shader );
}

void Graphics::RemoveDescriptorPool( DescriptorPool& pool )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->RemoveDescriptorPool( pool );
}

void Graphics::RemoveFramebuffer( Framebuffer& framebuffer )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->RemoveFramebuffer( framebuffer );
}

void Graphics::RemoveSampler( Sampler& sampler )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mResourceRegister->RemoveSampler( sampler );
}

void Graphics::CollectGarbage()
{
  std::lock_guard< std::mutex > lock{ mMutex };
  DALI_LOG_STREAM( gVulkanFilter, Debug::General,
                   "Beginning graphics garbage collection---------------------------------------" )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "Discard queue size: %ld\n", mDiscardQueue.size() )
  for( const auto& deleter : mDiscardQueue )
  {
    deleter();
  }
  mDiscardQueue.clear();
  DALI_LOG_STREAM( gVulkanFilter, Debug::General,
                   "Graphics garbage collection complete---------------------------------------" )
}

void Graphics::ExecuteActions()
{
  std::lock_guard< std::mutex > lock{ mMutex };
  DALI_LOG_STREAM( gVulkanFilter, Debug::General,
                   "Beginning graphics action execution---------------------------------------" )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "Action queue size: %ld\n", mActionQueue.size() )
  for( const auto& action : mActionQueue )
  {
    action();
  }
  mActionQueue.clear();
  DALI_LOG_STREAM( gVulkanFilter, Debug::General,
                   "Graphics action execution complete---------------------------------------" )
}

void Graphics::DiscardResource( std::function< void() > deleter )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mDiscardQueue.push_back( std::move( deleter ) );
}

void Graphics::EnqueueAction( std::function< void() > action )
{
  std::lock_guard< std::mutex > lock{ mMutex };
  mActionQueue.push_back( std::move( action ) );
}
// --------------------------------------------------------------------------------------------------------------


void Graphics::CreateInstance( const std::vector< const char* >& extensions,
                               const std::vector< const char* >& validationLayers )
{
  auto info = vk::InstanceCreateInfo{};

  info.setEnabledExtensionCount( U32( extensions.size() ) )
      .setPpEnabledExtensionNames( extensions.data() )
      .setEnabledLayerCount( U32( validationLayers.size() ) )
      .setPpEnabledLayerNames( validationLayers.data() );

//#if defined(DEBUG_ENABLED)
//  if( !getenv( "LOG_VULKAN" ) )
//  {
//    info.setEnabledLayerCount( 0 );
//  }
//#else
//  info.setEnabledLayerCount(0);
//#endif

  mInstance = VkAssert( vk::createInstance( info, *mAllocator ) );
}

void Graphics::DestroyInstance()
{
  if( mInstance )
  {
    mInstance.destroy( *mAllocator );
    mInstance = nullptr;
  }
}

void Graphics::PreparePhysicalDevice()
{
  auto devices = VkAssert( mInstance.enumeratePhysicalDevices() );
  assert( !devices.empty() && "No Vulkan supported device found!" );

  // if only one, pick first
  mPhysicalDevice = nullptr;
  if( devices.size() == 1 )
  {
    mPhysicalDevice = devices[0];
  }
  else // otherwise look for one which is a graphics device
  {
    for( auto& device : devices )
    {
      auto properties = device.getProperties();
      if( properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
          properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu )
      {
        mPhysicalDevice = device;
        break;
      }
    }
  }

  assert( mPhysicalDevice && "No suitable Physical Device found!" );

  GetPhysicalDeviceProperties();

  GetQueueFamilyProperties();

  mDeviceMemoryManager = GpuMemoryManager::New( *this );
}

void Graphics::GetPhysicalDeviceProperties()
{
  // store data on heap to keep object smaller
  mPhysicalDeviceProperties =
          MakeUnique< vk::PhysicalDeviceProperties >( mPhysicalDevice.getProperties() );
  mPhysicalDeviceMemoryProperties =
          MakeUnique< vk::PhysicalDeviceMemoryProperties >( mPhysicalDevice.getMemoryProperties() );
  mPhysicalDeviceFeatures =
          MakeUnique< vk::PhysicalDeviceFeatures >( mPhysicalDevice.getFeatures() );
}

void Graphics::GetQueueFamilyProperties()
{
  mQueueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();
}

std::vector< vk::DeviceQueueCreateInfo > Graphics::GetQueueCreateInfos()
{
  // surface is needed in order to find a family that supports presentation to this surface
  // fixme: assuming all surfaces will be compatible with the queue family
  assert( !mSurfaceFBIDMap.empty() &&
          "At least one surface has to be created before creating VkDevice!" );

  std::vector< vk::DeviceQueueCreateInfo > queueInfos{};

  constexpr uint8_t MAX_QUEUE_TYPES = 3;


  // find suitable family for each type of queue
  auto familyIndexTypes = std::array< uint32_t, MAX_QUEUE_TYPES >{};
  familyIndexTypes.fill( std::numeric_limits< uint32_t >::max() );

  // Graphics
  auto& graphicsFamily = familyIndexTypes[0];

  // Transfer
  auto& transferFamily = familyIndexTypes[1];

  // Transfer
  auto& presentFamily = familyIndexTypes[2];

  auto queueFamilyIndex = 0u;
  for( auto& prop : mQueueFamilyProperties )
  {
    if( ( prop.queueFlags & vk::QueueFlagBits::eGraphics ) && graphicsFamily == -1u )
    {
      graphicsFamily = queueFamilyIndex;
    }
    if( ( prop.queueFlags & vk::QueueFlagBits::eTransfer ) && transferFamily == -1u )
    {
      transferFamily = queueFamilyIndex;
    }
    if( mPhysicalDevice.getSurfaceSupportKHR( queueFamilyIndex, mSurfaceFBIDMap.begin()->second.
    surface->GetVkHandle() ).value && presentFamily == -1u )
    {
      presentFamily = queueFamilyIndex;
    }
    ++queueFamilyIndex;
  }

  assert( graphicsFamily != std::numeric_limits< uint32_t >::max()
          && "No queue family that supports graphics operations!" );
  assert( transferFamily != std::numeric_limits< uint32_t >::max()
          && "No queue family that supports transfer operations!" );
  assert( presentFamily != std::numeric_limits< uint32_t >::max()
          && "No queue family that supports present operations!" );

  // todo: we may require that the family must be same for all types of operations, it makes
  // easier to handle synchronisation related issues.

  // sort queues
  std::sort( familyIndexTypes.begin(), familyIndexTypes.end() );

  // allocate all queues from graphics family
  auto prevQueueFamilyIndex = std::numeric_limits< uint32_t >::max();

  for( const auto& familyIndex : familyIndexTypes )
  {
    if( prevQueueFamilyIndex == familyIndex )
    {
      continue;
    }

    auto& queueCount = mQueueFamilyProperties[familyIndex].queueCount;

    // fill queue create info for the family.
    // note the priorities are not being set as local pointer will out of scope, this
    // will be fixed by the caller function
    auto info = vk::DeviceQueueCreateInfo{}
            .setPQueuePriorities( nullptr )
            .setQueueCount( queueCount )
            .setQueueFamilyIndex( familyIndex );
    queueInfos.push_back( info );
    prevQueueFamilyIndex = familyIndex;
  }

  return queueInfos;
}

std::vector< const char* > Graphics::PrepareDefaultInstanceExtensions()
{
  auto extensions = vk::enumerateInstanceExtensionProperties();

  std::string extensionName;

  bool xlibAvailable{ false };
  bool xcbAvailable{ false };
  bool waylandAvailable{ false };

  for( auto&& ext : extensions.value )
  {
    extensionName = ext.extensionName;
    if( extensionName == VK_KHR_XCB_SURFACE_EXTENSION_NAME )
    {
      xcbAvailable = true;
    }
    else if( extensionName == VK_KHR_XLIB_SURFACE_EXTENSION_NAME )
    {
      xlibAvailable = true;
    }
    else if( extensionName == VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME )
    {
      waylandAvailable = true;
    }
  }

  std::vector< const char* > retval{};

  // depending on the platform validate extensions
  auto platform = GetDefaultPlatform();

  if( platform != Platform::UNDEFINED )
  {
    if( platform == Platform::XCB && xcbAvailable )
    {
      retval.push_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
    }
    else if( platform == Platform::XLIB && xlibAvailable )
    {
      retval.push_back( VK_KHR_XLIB_SURFACE_EXTENSION_NAME );
    }
    else if( platform == Platform::WAYLAND && waylandAvailable )
    {
      retval.push_back( VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME );
    }
  }
  else // try to determine the platform based on available extensions
  {
    if( xcbAvailable )
    {
      mPlatform = Platform::XCB;
      retval.push_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
    }
    else if( xlibAvailable )
    {
      mPlatform = Platform::XLIB;
      retval.push_back( VK_KHR_XLIB_SURFACE_EXTENSION_NAME );
    }
    else if( waylandAvailable )
    {
      mPlatform = Platform::WAYLAND;
      retval.push_back( VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME );
    }
    else
    {
      // can't determine the platform!
      mPlatform = Platform::UNDEFINED;
    }
  }

  // other essential extensions
  retval.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
  retval.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

  return retval;
}

RefCountedCommandPool Graphics::GetCommandPool( std::thread::id threadId )
{
  auto commandPool = RefCountedCommandPool{};

  {
    std::lock_guard< std::mutex > lock{ mMutex };
    commandPool = mCommandPools.find( threadId ) == mCommandPools.end() ? RefCountedCommandPool()
                                                                        : mCommandPools[threadId];
  }

  if( !commandPool )
  {
    auto createInfo = vk::CommandPoolCreateInfo{}.setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer );
    commandPool = CommandPool::New( *this, createInfo );
  }

  return commandPool;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
