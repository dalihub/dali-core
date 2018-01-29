#ifndef DALI_GRAPHICS_VULKAN_GRAPHICS_H
#define DALI_GRAPHICS_VULKAN_GRAPHICS_H

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

class Surface;
class CommandPool;
class GpuMemoryManager;
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
  std::unique_ptr< CommandPool > CreateCommandPool(const vk::CommandPoolCreateInfo& info);

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

  // AB: resource cache, should be moved somewhere else
  //
public:

  void AddBuffer( std::unique_ptr<Buffer> buffer );
  void AddImage( std::unique_ptr<Image> image );
  void AddPipeline( std::unique_ptr<Pipeline> pipeline );
  void AddShader( std::unique_ptr<Shader> shader );

  Handle<Shader> FindShader( vk::ShaderModule shaderModule );

  void RemoveBuffer( Buffer& buffer );
  void RemoveShader( Shader& buffer );
private:
  std::vector<std::unique_ptr<Buffer>>      mBuffersCache;
  std::vector<std::unique_ptr<Image>>       mImageCache;
  std::vector<std::unique_ptr<Pipeline>>    mPipelineCache;
  std::vector<std::unique_ptr<Shader>>      mShaderCache;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_GRAPHICS_H
