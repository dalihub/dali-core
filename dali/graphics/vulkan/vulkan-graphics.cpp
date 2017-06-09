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

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-queue.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/integration-api/graphics/vulkan/vk-surface-factory.h>
#include <dali/graphics/vulkan/vulkan-device-memory-manager.h>

#ifndef VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#define VK_KHR_XLIB_SURFACE_EXTENSION_NAME "VK_KHR_xlib_surface"
#endif

#ifndef VK_KHR_XCB_SURFACE_EXTENSION_NAME
#define VK_KHR_XCB_SURFACE_EXTENSION_NAME "VK_KHR_xcb_surface"
#endif

namespace Dali
{
namespace Graphics
{
using VkSurfaceFactory = Dali::Integration::Graphics::Vulkan::VkSurfaceFactory;
namespace Vulkan
{

Graphics::Graphics() = default;

Graphics::~Graphics() = default;

void Graphics::Create()
{
  CreateInstance();
  PreparePhysicalDevice();
}

void Graphics::CreateInstance()
{
  auto info = vk::InstanceCreateInfo{};
  auto extensions =
      std::vector< const char* >{VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
                                 VK_KHR_XLIB_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME };

  const auto validationLayers = std::vector< const char* >{
      //"VK_LAYER_LUNARG_screenshot",           // screenshot
      "VK_LAYER_LUNARG_parameter_validation", // parameter
      //"VK_LAYER_LUNARG_vktrace",              // vktrace ( requires vktrace connection )
      "VK_LAYER_LUNARG_monitor",             // monitor
      "VK_LAYER_LUNARG_swapchain",           // swapchain
      "VK_LAYER_GOOGLE_threading",           // threading
      "VK_LAYER_LUNARG_api_dump",            // api
      "VK_LAYER_LUNARG_object_tracker",      // objects
      "VK_LAYER_LUNARG_core_validation",     // core
      "VK_LAYER_GOOGLE_unique_objects",      // unique objects
      "VK_LAYER_LUNARG_standard_validation", // standard
  };

  info.setEnabledExtensionCount(U32(extensions.size()))
      .setPpEnabledExtensionNames(extensions.data())
      .setEnabledLayerCount(U32(validationLayers.size()))
      .setPpEnabledLayerNames(validationLayers.data());

  mInstance = VkAssert(vk::createInstance(info, mAllocator));
}

void Graphics::DestroyInstance()
{
  if(mInstance)
  {
    mInstance.destroy(mAllocator);
    mInstance = nullptr;
  }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
void Graphics::PreparePhysicalDevice()
{
  auto devices = VkAssert(mInstance.enumeratePhysicalDevices());
  assert(devices.size() > 0 && "No Vulkan supported device found!");

  // if only one, pick first
  mPhysicalDevice = nullptr;
  if(devices.size() == 1)
  {
    mPhysicalDevice = devices[0];
  }
  else // otherwise look for one which is a graphics device
  {
    for(auto& device : devices)
    {
      auto properties =device.getProperties();
      if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
         properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
      {
        mPhysicalDevice = device;
        break;
      }
    }
  }

  assert(mPhysicalDevice && "No suitable Physical Device found!");

  GetPhysicalDeviceProperties();

  GetQueueFamilyProperties();

  mDeviceMemoryManager = MakeUnique<DeviceMemoryManager>( *this );
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
void Graphics::GetPhysicalDeviceProperties()
{
  // store data on heap to keep object smaller
  mPhysicalDeviceProperties =
    MakeUnique<vk::PhysicalDeviceProperties>(mPhysicalDevice.getProperties());
  mPhysicalDeviceMemoryProperties =
    MakeUnique<vk::PhysicalDeviceMemoryProperties>(mPhysicalDevice.getMemoryProperties());
  mPhysicalDeviceFeatures =
    MakeUnique<vk::PhysicalDeviceFeatures>(mPhysicalDevice.getFeatures());
}
#pragma GCC diagnostic pop

void Graphics::GetQueueFamilyProperties()
{
  mQueueFamilyProperties = std::move(mPhysicalDevice.getQueueFamilyProperties());
}

FBID Graphics::CreateSurface(std::unique_ptr< SurfaceFactory > surfaceFactory)
{
  const auto vkFactory = dynamic_cast< const VkSurfaceFactory* >(surfaceFactory.get());
  if(!vkFactory)
  {
    return FBID{0u};
  }

  auto surface = vkFactory->Create(mInstance, mAllocator, mPhysicalDevice);

  // map surface to FBID
  auto fbid             = ++mBaseFBID;
  mSurfaceFBIDMap[fbid] = MakeUnique<Surface>(*this, surface, 3u);
  return fbid;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
std::vector< vk::DeviceQueueCreateInfo > Graphics::GetQueueCreateInfos()
{
  // surface is needed in order to find a family that supports presentation to this surface
  // fixme: assuming all surfaces will be compatible with the queue family
  assert(!mSurfaceFBIDMap.empty() &&
         "At least one surface has to be created before creating VkDevice!");

  auto queueInfos = std::vector< vk::DeviceQueueCreateInfo >{};

  constexpr uint8_t MAX_QUEUE_TYPES = 3;
  // find suitable family for each type of queue
  uint32_t familyIndexType[MAX_QUEUE_TYPES];
  std::fill(&familyIndexType[0], &familyIndexType[MAX_QUEUE_TYPES], -1u);

  // Graphics
  auto& graphicsFamily = familyIndexType[0];

  // Transfer
  auto& transferFamily = familyIndexType[1];

  // Transfer
  auto& presentFamily = familyIndexType[2];

  auto queueFamilyIndex = 0u;
  for(auto& prop : mQueueFamilyProperties)
  {
    if((prop.queueFlags & vk::QueueFlagBits::eGraphics) && graphicsFamily == -1u)
    {
      graphicsFamily = queueFamilyIndex;
    }
    if((prop.queueFlags & vk::QueueFlagBits::eTransfer) && transferFamily == -1u)
    {
      transferFamily = queueFamilyIndex;
    }
    if(mPhysicalDevice.getSurfaceSupportKHR(queueFamilyIndex, mSurfaceFBIDMap.begin()->second.get()->GetSurfaceKHR())
           .value &&
       presentFamily == -1u)
    {
      presentFamily = queueFamilyIndex;
    }
    ++queueFamilyIndex;
  }

  assert(graphicsFamily != -1u && "No queue family that supports graphics operations!");
  assert(transferFamily != -1u && "No queue family that supports transfer operations!");
  assert(presentFamily != -1u && "No queue family that supports present operations!");

  // todo: we may require that the family must be same for all types of operations, it makes
  // easier to handle synchronisation related issues.

  // sort queues
  std::sort(&familyIndexType[0], &familyIndexType[MAX_QUEUE_TYPES]);

  // allocate all queues from graphics family
  uint32_t prevQueueFamilyIndex = -1u;

  for(auto i = 0u; i < MAX_QUEUE_TYPES; ++i)
  {
    auto& familyIndex = familyIndexType[i];
    if(prevQueueFamilyIndex == familyIndex)
    {
      continue;
    }

    auto& queueCount = mQueueFamilyProperties[familyIndex].queueCount;

    // fill queue create info for the family.
    // note the priorities are not being set as local pointer will out of scope, this
    // will be fixed by the caller function
    auto info = vk::DeviceQueueCreateInfo{}
                    .setPQueuePriorities(nullptr)
                    .setQueueCount(queueCount)
                    .setQueueFamilyIndex(familyIndex);
    queueInfos.push_back(info);
    prevQueueFamilyIndex = familyIndex;
  }

  return queueInfos;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
void Graphics::CreateDevice()
{
  auto queueInfos = GetQueueCreateInfos();
  {
    auto maxQueueCountPerFamily = 0u;
    for( auto&& info : queueInfos )
    {
      maxQueueCountPerFamily = std::max( info.queueCount, maxQueueCountPerFamily );
    }

    auto priorities = std::vector<float>( maxQueueCountPerFamily );
    std::fill( priorities.begin(), priorities.end(), 1.0f );

    for( auto& info : queueInfos )
    {
      info.setPQueuePriorities( priorities.data() );
    }

    auto extensions = std::vector< const char* >{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    auto info = vk::DeviceCreateInfo{};
    info.setEnabledExtensionCount(U32(extensions.size()))
        .setPpEnabledExtensionNames(extensions.data())
        .setPEnabledFeatures(&(*mPhysicalDeviceFeatures))
        .setPQueueCreateInfos(queueInfos.data())
        .setQueueCreateInfoCount(U32(queueInfos.size()));

    mDevice = VkAssert(mPhysicalDevice.createDevice(info, mAllocator));
  }

  // create Queue objects
  for(auto& queueInfo : queueInfos)
  {
    for(auto i = 0u; i < queueInfo.queueCount; ++i)
    {
      auto queue = mDevice.getQueue(queueInfo.queueFamilyIndex, i);

      // based on family push queue instance into right array
      auto flags = mQueueFamilyProperties[queueInfo.queueFamilyIndex].queueFlags;
      if(flags & vk::QueueFlagBits::eGraphics)
      {
        mGraphicsQueues.emplace_back(
          MakeUnique<Queue>(*this, queue, queueInfo.queueFamilyIndex, i, flags));
      }
      if(flags & vk::QueueFlagBits::eTransfer)
      {
        mTransferQueues.emplace_back(
          MakeUnique<Queue>(*this, queue, queueInfo.queueFamilyIndex, i, flags));
      }
      if(flags & vk::QueueFlagBits::eCompute)
      {
        mComputeQueues.emplace_back(
          MakeUnique<Queue>(*this, queue, queueInfo.queueFamilyIndex, i, flags));
      }

      // todo: present queue
    }
  }
}
#pragma GCC diagnostic pop

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

vk::AllocationCallbacks* Graphics::GetAllocator() const
{
  return mAllocator;
}

Queue& Graphics::GetGraphicsQueue(uint32_t index) const
{
  // todo: at the moment each type of queue may use only one, indices greater than 0 are invalid
  // this will change in the future
  assert(index == 0u && "Each type of queue may use only one, indices greater than 0 are invalid!");

  return *mGraphicsQueues[0].get(); // will be mGraphicsQueues[index]
}

Queue& Graphics::GetTransferQueue(uint32_t index) const
{
  // todo: at the moment each type of queue may use only one, indices greater than 0 are invalid
  // this will change in the future
  assert(index == 0u && "Each type of queue may use only one, indices greater than 0 are invalid!");

  return *mTransferQueues[0].get(); // will be mGraphicsQueues[index]
}

Queue& Graphics::GetComputeQueue(uint32_t index) const
{
  // todo: at the moment each type of queue may use only one, indices greater than 0 are invalid
  // this will change in the future
  assert(index == 0u && "Each type of queue may use only one, indices greater than 0 are invalid!");

  return *mComputeQueues[0].get(); // will be mGraphicsQueues[index]
}

Queue& Graphics::GetPresentQueue() const
{
  return *mPresentQueue.get();
}

std::unique_ptr< CommandPool > Graphics::CreateCommandPool(const vk::CommandPoolCreateInfo& info)
{
  auto cmdpool = std::unique_ptr< CommandPool >(new CommandPool(*this, info));
  return cmdpool;
}

Surface& Graphics::GetSurface( FBID surfaceId )
{
  return *mSurfaceFBIDMap[surfaceId].get();
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
