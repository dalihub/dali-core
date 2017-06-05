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
#include <dali/graphics/vulkan/types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

// TODO: this should be probably contained in the own file
class SurfaceFactory
{
public:
  virtual ~SurfaceFactory() = default;
};

class Surface;
class CommandPool;
class DeviceMemoryManager;
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

  vk::AllocationCallbacks* GetAllocator() const;

  DeviceMemoryManager& GetDeviceMemoryManager() const
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

private:

  void                                     CreateInstance();
  void                                     DestroyInstance();
  void                                     PreparePhysicalDevice();
  void                                     GetPhysicalDeviceProperties();
  void                                     GetQueueFamilyProperties();
  std::vector< vk::DeviceQueueCreateInfo > GetQueueCreateInfos();

private:

  std::unique_ptr<DeviceMemoryManager> mDeviceMemoryManager;

  vk::Instance             mInstance;
  vk::AllocationCallbacks* mAllocator{nullptr};

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
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_GRAPHICS_H
