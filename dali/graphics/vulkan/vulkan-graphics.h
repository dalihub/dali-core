#ifndef DALI_GRAPHICS_VULKAN_GRAPHICS
#define DALI_GRAPHICS_VULKAN_GRAPHICS

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

#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
#endif

// INTERNAL INCLUDES
#include <dali/integration-api/graphics/surface-factory.h>
#include <dali/graphics/vulkan/internal/vulkan-types.h>
#include <dali/graphics/vulkan/internal/vulkan-queue.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/internal/vulkan-descriptor-allocator.h>
#include <dali/integration-api/graphics/graphics.h>

#include <thread>
#include <mutex>
#include <map>
#include <functional>


namespace Dali
{
namespace Graphics
{
namespace API
{
class Controller;
}
namespace VulkanAPI
{
class Controller;
}

namespace Vulkan
{

class Buffer;

class Image;

class Pipeline;

class Shader;

class Framebuffer;

class Surface;

class CommandPool;

class DescriptorPool;

class GpuMemoryManager;

class ResourceRegister;

class FramebufferAttachment;

class DescriptorSetAllocator;

using Dali::Integration::Graphics::SurfaceFactory;
using CommandPoolMap = std::unordered_map< std::thread::id, RefCountedCommandPool >;

struct SwapchainSurfacePair
{
  RefCountedSwapchain swapchain;
  RefCountedSurface surface;
};

class Graphics
{

public:
  Graphics();

  Graphics( const Graphics& ) = delete;

  Graphics& operator=( const Graphics& ) = delete;

  ~Graphics();

public: // Create methods

  void Create();

  void CreateDevice();

  FBID CreateSurface( const Integration::Graphics::GraphicsCreateInfo& createInfo );

  RefCountedSwapchain CreateSwapchainForSurface( RefCountedSurface surface );

  RefCountedSwapchain ReplaceSwapchainForSurface( RefCountedSurface surface, RefCountedSwapchain&& oldSwapchain);

  RefCountedShader CreateShader(); //will see if this will work

  RefCountedPipeline CreatePipeline();

  RefCountedFence CreateFence( const vk::FenceCreateInfo& fenceCreateInfo );

  RefCountedBuffer CreateBuffer( size_t size, BufferType type );

  RefCountedBuffer CreateBuffer( const vk::BufferCreateInfo& bufferCreateInfo );



  RefCountedFramebuffer CreateFramebuffer( const std::vector< RefCountedFramebufferAttachment >& colorAttachments,
                                           RefCountedFramebufferAttachment depthAttachment,
                                           uint32_t width,
                                           uint32_t height,
                                           vk::RenderPass externalRenderPass = nullptr );

  RefCountedImage CreateImage( const vk::ImageCreateInfo& imageCreateInfo );

  RefCountedImage CreateImageFromExternal( vk::Image externalImage, vk::Format imageFormat, vk::Extent2D extent );

  RefCountedImageView CreateImageView( const vk::ImageViewCreateFlags& flags,
                                       const RefCountedImage& image,
                                       vk::ImageViewType viewType,
                                       vk::Format format,
                                       vk::ComponentMapping components,
                                       vk::ImageSubresourceRange subresourceRange );

  RefCountedImageView CreateImageView( RefCountedImage image );

  RefCountedDescriptorPool CreateDescriptorPool();

  RefCountedGpuMemoryBlock CreateGpuMemoryBlock();

  RefCountedDescriptorSet CreateDescriptorSet();

  RefCountedSampler CreateSampler( const vk::SamplerCreateInfo& samplerCreateInfo );

  RefCountedCommandBuffer CreateCommandBuffer( bool primary );

  vk::ImageMemoryBarrier CreateImageMemoryBarrier( RefCountedImage image,
                                                   vk::ImageLayout oldLayout,
                                                   vk::ImageLayout newLayout );

  RefCountedSwapchain CreateSwapchain( RefCountedSurface surface, vk::Format requestedFormat,
                                       vk::PresentModeKHR presentMode,
                                       uint32_t bufferCount, RefCountedSwapchain&& oldSwapchain );

public: // Actions
  vk::Result WaitForFence( RefCountedFence fence, uint32_t timeout = std::numeric_limits< uint32_t >::max() );

  vk::Result WaitForFences( const std::vector< RefCountedFence >& fences,
                            bool waitAll = true,
                            uint32_t timeout = std::numeric_limits< uint32_t >::max() );

  vk::Result ResetFence( RefCountedFence fence );

  vk::Result ResetFences( const std::vector< RefCountedFence >& fences );

  vk::Result BindImageMemory( RefCountedImage image, RefCountedGpuMemoryBlock memory, uint32_t offset );

  vk::Result BindBufferMemory( RefCountedBuffer buffer, RefCountedGpuMemoryBlock memory, uint32_t offset );

  void* MapMemory( RefCountedGpuMemoryBlock memory ) const;

  void* MapMemory( RefCountedGpuMemoryBlock memory, uint32_t size, uint32_t offset ) const;

  void UnmapMemory( RefCountedGpuMemoryBlock memory ) const;

  template< typename T >
  T* MapMemoryTyped( RefCountedGpuMemoryBlock memory ) const
  {
    return memory->MapTyped<T>();
  }

  RefCountedGpuMemoryBlock AllocateMemory( RefCountedBuffer buffer, vk::MemoryPropertyFlags memoryProperties );

  RefCountedGpuMemoryBlock AllocateMemory( RefCountedImage image, vk::MemoryPropertyFlags memoryProperties );

  vk::Result Submit( Queue& queue, const std::vector< SubmissionData >& submissionData, RefCountedFence fence );

  std::vector< RefCountedDescriptorSet >
  AllocateDescriptorSets( const std::vector< DescriptorSetLayoutSignature >& signatures,
                          const std::vector< vk::DescriptorSetLayout >& layouts);

  vk::Result Present( Queue& queue, vk::PresentInfoKHR presentInfo );

  vk::Result QueueWaitIdle( Queue& queue );

  vk::Result DeviceWaitIdle();

public: // Getters
  RefCountedSurface GetSurface( FBID surfaceId );

  RefCountedSwapchain GetSwapchainForSurface( RefCountedSurface surface );

  RefCountedSwapchain GetSwapchainForFBID( FBID surfaceId );

  vk::Device GetDevice() const;

  vk::PhysicalDevice GetPhysicalDevice() const;

  vk::Instance GetInstance() const;

  const vk::AllocationCallbacks& GetAllocator() const;

  const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const;

  Queue& GetGraphicsQueue( uint32_t index = 0u ) const;

  Queue& GetTransferQueue( uint32_t index = 0u ) const;

  Queue& GetComputeQueue( uint32_t index = 0u ) const;

  Queue& GetPresentQueue() const;

  Platform GetDefaultPlatform() const;

  Dali::Graphics::API::Controller& GetController();

public: //Cache management methods

  void AddBuffer( Buffer& buffer );

  void AddImage( Image& image );

  void AddImageView( ImageView& imageView );

  void AddShader( Shader& shader );

  void AddCommandPool( RefCountedCommandPool pool );

  void AddDescriptorPool( DescriptorPool& pool );

  void AddFramebuffer( Framebuffer& framebuffer );

  void AddSampler( Sampler& sampler );

  RefCountedShader FindShader( vk::ShaderModule shaderModule );

  RefCountedImage FindImage( vk::Image image );

  void RemoveBuffer( Buffer& buffer );

  void RemoveImage( Image& image );

  void RemoveImageView( ImageView& imageView );

  void RemoveShader( Shader& shader );

  void RemoveDescriptorPool( DescriptorPool& pool );

  void RemoveFramebuffer( Framebuffer& framebuffer );

  void RemoveSampler( Sampler& sampler );

  void CollectGarbage();

  void ExecuteActions();

  void DiscardResource( std::function< void() > deleter );

  void EnqueueAction( std::function< void() > action );

private: // Methods

  void CreateInstance( const std::vector< const char* >& extensions,
                       const std::vector< const char* >& validationLayers );

  void DestroyInstance();

  void PreparePhysicalDevice();

  void GetPhysicalDeviceProperties();

  void GetQueueFamilyProperties();

  std::vector< vk::DeviceQueueCreateInfo > GetQueueCreateInfos();

  std::vector< const char* > PrepareDefaultInstanceExtensions();

  RefCountedCommandPool GetCommandPool( std::thread::id threadId );

private: // Members

  // physical device
  vk::PhysicalDevice mPhysicalDevice;

  // logical device
  vk::Device mDevice;

  vk::Instance mInstance;

  // physical device properties
  std::unique_ptr< vk::PhysicalDeviceProperties > mPhysicalDeviceProperties;
  std::unique_ptr< vk::PhysicalDeviceMemoryProperties > mPhysicalDeviceMemoryProperties;
  std::unique_ptr< vk::PhysicalDeviceFeatures > mPhysicalDeviceFeatures;

  std::unique_ptr< vk::AllocationCallbacks > mAllocator{ nullptr };

  // queue family properties
  std::vector< vk::QueueFamilyProperties > mQueueFamilyProperties;

  // Sets of queues
  std::vector< std::unique_ptr< Queue > > mGraphicsQueues;
  std::vector< std::unique_ptr< Queue > > mTransferQueues;
  std::vector< std::unique_ptr< Queue > > mComputeQueues;
  //std::unique_ptr< Queue > mPresentQueue;

  std::unique_ptr< ResourceRegister > mResourceRegister;

  std::unordered_map< FBID, SwapchainSurfacePair > mSurfaceFBIDMap;
  FBID mBaseFBID{ 0u };

  std::unique_ptr< GpuMemoryManager > mDeviceMemoryManager;

  Platform mPlatform{ Platform::UNDEFINED };

  std::mutex mMutex;

  std::unique_ptr< Dali::Graphics::VulkanAPI::Controller > mGfxController;

  // Command pool map using thread IDs as keys
  CommandPoolMap mCommandPools;

  std::unique_ptr< DescriptorSetAllocator > mDescriptorAllocator;

  std::vector< std::function< void() > > mActionQueue;
  std::vector< std::function< void() > > mDiscardQueue;

  bool mHasDepth;
  bool mHasStencil;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_GRAPHICS
