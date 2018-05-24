#ifndef DALI_GRAPHICS_VULKAN_GRAPHICS
#define DALI_GRAPHICS_VULKAN_GRAPHICS

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/graphics/vulkan/vulkan-types.h>
#include <dali/integration-api/graphics/surface-factory.h>
#include <dali/graphics/vulkan/vulkan-swapchain.h>
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
class PipelineCache;
class ResourceCache;

using SurfaceFactory = Dali::Integration::Graphics::SurfaceFactory;
using ResourceCacheMap = std::unordered_map< std::thread::id, std::unique_ptr<ResourceCache> >;

struct SwapchainSurfacePair
{
  RefCountedSwapchain swapchain;
  RefCountedSurface surface;
};

class Graphics
{

public:
  Graphics();
  Graphics(const Graphics&) = delete;
  Graphics& operator=(const Graphics&) = delete;
  ~Graphics();

public: // Create methods

  void                                    Create();
  void                                    CreateDevice();
  FBID                                    CreateSurface(std::unique_ptr< SurfaceFactory > surfaceFactory);
  RefCountedSwapchain                     CreateSwapchainForSurface( RefCountedSurface surface );
  RefCountedShader                        CreateShader(); //will see if this will work
  RefCountedPipeline                      CreatePipeline();
  RefCountedFence                         CreateFence( const vk::FenceCreateInfo& fenceCreateInfo );
  RefCountedBuffer                        CreateBuffer( size_t size, BufferType type );
  RefCountedBuffer                        CreateBuffer( const vk::BufferCreateInfo& bufferCreateInfo );
  RefCountedFramebuffer                   CreateFramebuffer();
  RefCountedImage                         CreateImage();
  RefCountedImageView                     CreateImageView();
  RefCountedDescriptorPool                CreateDescriptorPool();
  RefCountedCommandPool                   CreateCommandPool(const vk::CommandPoolCreateInfo& info);
  RefCountedCommandBuffer                 CreateCommandBuffer();
  std::vector< RefCountedCommandBuffer >  CreateCommandBuffers();
  RefCountedGpuMemoryBlock                CreateGpuMemoryBlock();
  RefCountedDescriptorSet                 CreateDescriptorSet();
  RefCountedSampler                       CreateSampler();

public: // Actions
  vk::Result WaitForFence( RefCountedFence fence, uint32_t timeout = 0 );
  vk::Result WaitForFences( const std::vector< RefCountedFence >& fences,
                            bool waitAll = true,
                            uint32_t timeout = std::numeric_limits<uint32_t>::max() );
  vk::Result ResetFence( RefCountedFence fence );
  vk::Result ResetFences( const std::vector< RefCountedFence >& fences );

public: // Getters
  RefCountedSurface GetSurface( FBID surfaceId );

  RefCountedSwapchain GetSwapchainForSurface( RefCountedSurface surface );

  RefCountedSwapchain GetSwapchainForFBID( FBID surfaceId );

  vk::Device GetDevice() const;

  vk::PhysicalDevice GetPhysicalDevice() const;

  vk::Instance GetInstance() const;

  const vk::AllocationCallbacks& GetAllocator() const;

  GpuMemoryManager& GetDeviceMemoryManager() const;

  const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const;

  Queue& GetGraphicsQueue(uint32_t index = 0u) const;
  Queue& GetTransferQueue(uint32_t index = 0u) const;
  Queue& GetComputeQueue(uint32_t index = 0u) const;
  Queue& GetPresentQueue() const;

  Platform GetDefaultPlatform() const;

  Dali::Graphics::API::Controller& GetController();

  PipelineCache& GetPipelineCache();

public: //Cache management methods

  void AddBuffer( RefCountedBuffer buffer );
  void AddImage( RefCountedImage image );
  void AddShader( RefCountedShader shader );
  void AddCommandPool( RefCountedCommandPool pool );
  void AddDescriptorPool( RefCountedDescriptorPool pool );
  void AddFramebuffer( RefCountedFramebuffer framebuffer );

  RefCountedShader FindShader( vk::ShaderModule shaderModule );
  RefCountedImage FindImage( vk::Image image );

  void RemoveBuffer( Buffer& buffer );
  void RemoveShader( Shader& shader );
  void RemoveCommandPool( CommandPool& commandPool );
  void RemoveDescriptorPool( DescriptorPool& pool );
  void RemoveFramebuffer( Framebuffer& framebuffer );
  void RemoveSampler( Sampler& sampler );

  void CollectGarbage();

  void DiscardResource(std::function<void()> deleter);

private: // Methods

  void                                      CreateInstance( const std::vector<const char*>& extensions,
                                                            const std::vector<const char*>& validationLayers );
  void                                      DestroyInstance();
  void                                      PreparePhysicalDevice();
  void                                      GetPhysicalDeviceProperties();
  void                                      GetQueueFamilyProperties();
  std::vector< vk::DeviceQueueCreateInfo >  GetQueueCreateInfos();
  std::vector<const char*>                  PrepareDefaultInstanceExtensions();

  std::unique_ptr< ResourceCache >&         GetResourceCache(std::thread::id thread_id);

private: // Members

  std::unique_ptr<GpuMemoryManager>                           mDeviceMemoryManager;

  vk::Instance                                                mInstance;
  std::unique_ptr<vk::AllocationCallbacks>                    mAllocator{nullptr};

  // physical device
  vk::PhysicalDevice mPhysicalDevice;

  // logical device
  vk::Device                                                  mDevice;

  // physical device properties
  std::unique_ptr< vk::PhysicalDeviceProperties >             mPhysicalDeviceProperties;
  std::unique_ptr< vk::PhysicalDeviceMemoryProperties >       mPhysicalDeviceMemoryProperties;
  std::unique_ptr< vk::PhysicalDeviceFeatures >               mPhysicalDeviceFeatures;

  // queue family properties
  std::vector< vk::QueueFamilyProperties >                    mQueueFamilyProperties;

  std::unordered_map< FBID, SwapchainSurfacePair >            mSurfaceFBIDMap;
  FBID mBaseFBID{0u};

  // Sets of queues
  std::vector< std::unique_ptr<Queue> >                       mGraphicsQueues;
  std::vector< std::unique_ptr<Queue> >                       mTransferQueues;
  std::vector< std::unique_ptr<Queue> >                       mComputeQueues;
  //std::unique_ptr< Queue > mPresentQueue;

  Platform                                                    mPlatform  { Platform::UNDEFINED };

  std::unique_ptr<Dali::Graphics::VulkanAPI::Controller>      mGfxController;

  // TODO: rename
  std::unique_ptr<PipelineCache>                              mPipelineDatabase;

  std::mutex                                                  mMutex;
  ResourceCacheMap                                            mResourceCacheMap;

};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_GRAPHICS
