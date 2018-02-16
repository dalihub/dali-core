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

namespace Dali
{
namespace Graphics
{
namespace API
{
class Controller;
}
using SurfaceFactory = Dali::Integration::Graphics::SurfaceFactory;

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
class Controller;

class Graphics
{

public:
  Graphics();
  Graphics(std::unique_ptr< SurfaceFactory > surfaceFactory);
  Graphics(const Graphics&) = delete;
  Graphics& operator=(const Graphics&) = delete;
  ~Graphics();

  void Create();

  // new way
  FBID CreateSurface(std::unique_ptr< SurfaceFactory > surfaceFactory);

  Surface& GetSurface( FBID surfaceId );

  void CreateDevice();

  /** Creates new command pool */
  CommandPoolRef CreateCommandPool(const vk::CommandPoolCreateInfo& info);

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

  std::unique_ptr<GpuMemoryManager> mDeviceMemoryManager;

  vk::Instance             mInstance;
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

  std::unordered_map< FBID, UniqueSurface > mSurfaceFBIDMap;
  FBID mBaseFBID{0u};

  // Sets of queues
  std::vector< std::unique_ptr<Queue> >  mGraphicsQueues;
  std::vector< std::unique_ptr<Queue> >  mTransferQueues;
  std::vector< std::unique_ptr<Queue> >  mComputeQueues;
  std::unique_ptr< Queue > mPresentQueue;

  Platform                               mPlatform  { Platform::UNDEFINED };

public:
  // TODO: all this stuff should go into some vulkan cache

  void AddBuffer( Handle<Buffer> buffer );
  void AddImage( Handle<Image> image );
  void AddPipeline( Handle<Pipeline> pipeline );
  void AddShader( Handle<Shader> shader );
  void AddCommandPool( Handle<CommandPool> pool );
  void AddDescriptorPool( Handle<DescriptorPool> pool );
  void AddFramebuffer( Handle<Framebuffer> framebuffer );

  ShaderRef FindShader( vk::ShaderModule shaderModule );

  void RemoveBuffer( Buffer& buffer );
  void RemoveShader( Shader& shader );
  void RemoveCommandPool( CommandPool& commandPool );
  void RemoveDescriptorPool( std::unique_ptr<DescriptorPool> pool );

private:
  std::vector<BufferRef>                mBuffersCache;
  std::vector<ImageRef>                 mImageCache;
  std::vector<PipelineRef>              mPipelineCache;
  std::vector<ShaderRef>                mShaderCache;
  std::vector<CommandPoolRef>           mCommandPoolCache;
  std::vector<DescriptorPoolRef>        mDescriptorPoolCache;
  std::vector<FramebufferRef>           mFramebufferCache;

private:
  std::unique_ptr<Dali::Graphics::Vulkan::Controller>           mGfxController;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_GRAPHICS
