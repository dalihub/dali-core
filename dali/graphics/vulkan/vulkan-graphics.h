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

class GpuMemoryManager;

class FramebufferAttachment;

class Memory
{
  friend class Graphics;

private:

  Memory( Graphics* graphics, vk::DeviceMemory deviceMemory, size_t memSize, size_t memAlign, bool hostVisible );

public:

  ~Memory();

  template<class T>
  T* MapTyped()
  {
    return reinterpret_cast<T*>(Map());
  }

  Memory(Memory&) = delete;
  Memory& operator=(Memory&) = delete;

  void* Map();

  void* Map( uint32_t offset, uint32_t size );

  void Unmap();

  void Flush();

  /**
   * Releases vk::DeviceMemory object so it can be deleted
   * externally
   * @return
   */
  vk::DeviceMemory ReleaseVkObject();

  vk::DeviceMemory GetVkHandle() const;

private:
  Graphics* graphics;
  vk::DeviceMemory memory;
  size_t size;
  size_t alignment;
  void* mappedPtr;
  size_t mappedSize;
  bool hostVisible;
};

using Dali::Integration::Graphics::SurfaceFactory;
using CommandPoolMap = std::unordered_map< std::thread::id, RefCountedCommandPool >;

struct SwapchainSurfacePair
{
  RefCountedSwapchain swapchain;
  RefCountedSurface surface;
};

using DiscardQueue = std::vector< std::function< void() > >;

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

  void InitialiseController();

  FBID CreateSurface( SurfaceFactory& surfaceFactory,
                      const Integration::Graphics::GraphicsCreateInfo& createInfo );

  RefCountedSwapchain CreateSwapchainForSurface( RefCountedSurface surface );

  RefCountedSwapchain ReplaceSwapchainForSurface( RefCountedSurface surface, RefCountedSwapchain&& oldSwapchain);

  RefCountedFence CreateFence( const vk::FenceCreateInfo& fenceCreateInfo );

  RefCountedBuffer CreateBuffer( size_t size, vk::BufferUsageFlags usageFlags );

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

  RefCountedSampler CreateSampler( const vk::SamplerCreateInfo& samplerCreateInfo );

  RefCountedCommandBuffer CreateCommandBuffer( bool primary );

  vk::ImageMemoryBarrier CreateImageMemoryBarrier( RefCountedImage image,
                                                   vk::ImageLayout oldLayout,
                                                   vk::ImageLayout newLayout );

  RefCountedSwapchain CreateSwapchain( RefCountedSurface surface, vk::Format requestedFormat,
                                       vk::PresentModeKHR presentMode,
                                       uint32_t bufferCount, RefCountedSwapchain&& oldSwapchain );

  vk::RenderPass CreateCompatibleRenderPass( const std::vector< RefCountedFramebufferAttachment >& colorAttachments,
                                             RefCountedFramebufferAttachment depthAttachment,
                                             std::vector<vk::ImageView>& attachments);

public: // Actions
  vk::Result WaitForFence( RefCountedFence fence, uint32_t timeout = std::numeric_limits< uint32_t >::max() );

  vk::Result WaitForFences( const std::vector< RefCountedFence >& fences,
                            bool waitAll = true,
                            uint32_t timeout = std::numeric_limits< uint32_t >::max() );

  vk::Result ResetFence( RefCountedFence fence );

  vk::Result ResetFences( const std::vector< RefCountedFence >& fences );

  vk::Result BindImageMemory( RefCountedImage image,std::unique_ptr<Memory> memory, uint32_t offset );

  vk::Result BindBufferMemory( RefCountedBuffer buffer, std::unique_ptr<Memory> memory, uint32_t offset );

  void* MapMemory( Memory* memory ) const;

  void* MapMemory( Memory* memory, uint32_t size, uint32_t offset ) const;

  void UnmapMemory( Memory* memory ) const;

  template< typename T >
  T* MapMemoryTyped( Memory* memory ) const
  {
    return memory->MapTyped<T>();
  }

  std::unique_ptr<Memory> AllocateMemory( RefCountedBuffer buffer, vk::MemoryPropertyFlags memoryProperties );

  std::unique_ptr<Memory> AllocateMemory( RefCountedImage image, vk::MemoryPropertyFlags memoryProperties );

  vk::Result Submit( Queue& queue, const std::vector< SubmissionData >& submissionData, RefCountedFence fence );

  vk::Result Present( Queue& queue, vk::PresentInfoKHR presentInfo );

  vk::Result QueueWaitIdle( Queue& queue );

  vk::Result DeviceWaitIdle();

  void SurfaceResized( unsigned int width, unsigned int height );

public: // Getters
  RefCountedSurface GetSurface( FBID surfaceId );

  RefCountedSwapchain GetSwapchainForSurface( RefCountedSurface surface );

  RefCountedSwapchain GetSwapchainForFBID( FBID surfaceId );

  vk::Device GetDevice() const;

  vk::PhysicalDevice GetPhysicalDevice() const;

  vk::Instance GetInstance() const;

  const vk::AllocationCallbacks& GetAllocator( const char* tag  = nullptr );

  const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const;

  Queue& GetGraphicsQueue( uint32_t index = 0u ) const;

  Queue& GetTransferQueue( uint32_t index = 0u ) const;

  Queue& GetComputeQueue( uint32_t index = 0u ) const;

  Queue& GetPresentQueue() const;

  Platform GetDefaultPlatform() const;

  Dali::Graphics::API::Controller& GetController();

  const vk::PipelineCache& GetVulkanPipelineCache();

  bool HasDepthEnabled() const;

  bool HasStencilEnabled() const;

  bool IsSurfaceResized() const;

public: //Cache management methods

  void AddCommandPool( RefCountedCommandPool pool );

  void CollectGarbage();

  void DiscardResource( std::function< void() > deleter );

  const DiscardQueue& GetDiscardQueue( uint32_t bufferIndex ) const;

  uint32_t SwapBuffers();

  uint32_t GetCurrentBufferIndex();

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
  vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
  vk::PhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
  vk::PhysicalDeviceFeatures mPhysicalDeviceFeatures;

  std::unique_ptr< vk::AllocationCallbacks > mAllocator{ nullptr };

  // queue family properties
  std::vector< vk::QueueFamilyProperties > mQueueFamilyProperties;

  // Sets of queues
  std::vector< std::unique_ptr< Queue > > mAllQueues;
  std::vector< Queue* > mGraphicsQueues;
  std::vector< Queue* > mTransferQueues;
  std::vector< Queue* > mComputeQueues;

  std::unordered_map< FBID, SwapchainSurfacePair > mSurfaceFBIDMap;
  FBID mBaseFBID{ 0u };

  Platform mPlatform{ Platform::UNDEFINED };

  std::mutex mMutex;

  std::unique_ptr< Dali::Graphics::VulkanAPI::Controller > mGfxController;

  // Command pool map using thread IDs as keys
  CommandPoolMap mCommandPools;

  DiscardQueue mDiscardQueue[2u];

  bool mHasDepth { false };
  bool mHasStencil { false };

  vk::PipelineCache mVulkanPipelineCache;

  bool mSurfaceResized { false };

  uint32_t mCurrentBufferIndex{ 0u };

  bool mDisableNativeImage { false };
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_GRAPHICS
