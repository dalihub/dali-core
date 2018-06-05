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

namespace Dali
{
namespace Graphics
{
using SurfaceFactory = Dali::Integration::Graphics::SurfaceFactory;
namespace API
{
class Controller;
}
namespace Vulkan
{

enum class Platform
{
  UNDEFINED,
  XLIB,
  XCB,
  WAYLAND,
};

class Buffer;
class Image;
class Pipeline;
class Shader;
class Framebuffer;
class Surface;
class CommandPool;
class DescriptorPool;
class GpuMemoryManager;
class ResourceCache;

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

  void Create();

  // new way
  FBID CreateSurface(std::unique_ptr< SurfaceFactory > surfaceFactory);

  RefCountedSwapchain CreateSwapchainForSurface( RefCountedSurface surface );

  RefCountedSurface GetSurface( FBID surfaceId );

  RefCountedSwapchain GetSwapchainForSurface( RefCountedSurface surface );

  RefCountedSwapchain GetSwapchainForFBID( FBID surfaceId );

  void CreateDevice();

  /** Creates new command pool */
  RefCountedCommandPool CreateCommandPool(const vk::CommandPoolCreateInfo& info);

  vk::Device GetDevice() const;

  vk::PhysicalDevice GetPhysicalDevice() const;

  vk::Instance GetInstance() const;

  const vk::AllocationCallbacks& GetAllocator() const;

  GpuMemoryManager& GetDeviceMemoryManager() const
  {
    return *mDeviceMemoryManager;
  }

  const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const
  {
    return *mPhysicalDeviceMemoryProperties;
  }

  Queue& GetGraphicsQueue(uint32_t index = 0u) const;
  Queue& GetTransferQueue(uint32_t index = 0u) const;
  Queue& GetComputeQueue(uint32_t index = 0u) const;
  Queue& GetPresentQueue() const;

  Platform GetDefaultPlatform() const;

  Dali::Graphics::API::Controller& GetController();

private:

  void                                     CreateInstance( const std::vector<const char*>& extensions, const std::vector<const char*>& validationLayers );
  void                                     DestroyInstance();
  void                                     PreparePhysicalDevice();
  void                                     GetPhysicalDeviceProperties();
  void                                     GetQueueFamilyProperties();
  std::vector< vk::DeviceQueueCreateInfo > GetQueueCreateInfos();
  std::vector<const char*>                 PrepareDefaultInstanceExtensions();

private:

  std::unique_ptr<GpuMemoryManager>        mDeviceMemoryManager;

  vk::Instance                             mInstance;
  std::unique_ptr<vk::AllocationCallbacks> mAllocator{nullptr};

  // physical device
  vk::PhysicalDevice mPhysicalDevice;

  // logical device
  vk::Device mDevice;

  // physical device properites
  std::unique_ptr< vk::PhysicalDeviceProperties >       mPhysicalDeviceProperties;
  std::unique_ptr< vk::PhysicalDeviceMemoryProperties > mPhysicalDeviceMemoryProperties;
  std::unique_ptr< vk::PhysicalDeviceFeatures >         mPhysicalDeviceFeatures;

  // queue family properties
  std::vector< vk::QueueFamilyProperties > mQueueFamilyProperties;

  std::unordered_map< FBID, SwapchainSurfacePair > mSurfaceFBIDMap;
  FBID mBaseFBID{0u};

  // Sets of queues
  std::vector< std::unique_ptr<Queue> >  mGraphicsQueues;
  std::vector< std::unique_ptr<Queue> >  mTransferQueues;
  std::vector< std::unique_ptr<Queue> >  mComputeQueues;
  //std::unique_ptr< Queue > mPresentQueue;

  Platform                               mPlatform  { Platform::UNDEFINED };

public:
  // TODO: all this stuff should go into some vulkan cache

  void AddBuffer( RefCountedBuffer buffer );
  void AddImage( RefCountedImage image );
  void AddPipeline( RefCountedPipeline pipeline );
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

private:
  std::unique_ptr<Dali::Graphics::API::Controller>           mGfxController;

  std::unique_ptr<ResourceCache>        mResourceCache;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_GRAPHICS
