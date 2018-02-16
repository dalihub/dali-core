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
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-queue.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/integration-api/graphics/vulkan/vk-surface-factory.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>

#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-graphics-controller.h>

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

namespace Dali
{
namespace Graphics
{
using VkSurfaceFactory = Dali::Integration::Graphics::Vulkan::VkSurfaceFactory;
namespace Vulkan
{

const auto VALIDATION_LAYERS = std::vector< const char* >{

  //"VK_LAYER_LUNARG_screenshot",           // screenshot
  "VK_LAYER_RENDERDOC_Capture",
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

Graphics::Graphics() = default;

Graphics::~Graphics() = default;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="

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
  if(!mGfxController)
  {
    mGfxController = Dali::Graphics::Vulkan::Controller::New(*this);
  }

  return *mGfxController.get();
}

std::vector<const char*> Graphics::PrepareDefaultInstanceExtensions()
{
  auto extensions = vk::enumerateInstanceExtensionProperties();

  std::string extensionName;

  bool xlibAvailable    { false };
  bool xcbAvailable     { false };
  bool waylandAvailable { false };

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

  std::vector<const char*> retval;

  // depending on the platform validate extensions
  auto platform = GetDefaultPlatform();

  if( platform != Platform::UNDEFINED )
  {
    if (platform == Platform::XCB && xcbAvailable)
    {
      retval.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
    }
    else if (platform == Platform::XLIB && xlibAvailable)
    {
      retval.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
    }
    else if (platform == Platform::WAYLAND && waylandAvailable)
    {
      retval.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
    }
  }
  else // try to determine the platform based on available extensions
  {
    if (xcbAvailable)
    {
      mPlatform = Platform::XCB;
      retval.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
    }
    else if (xlibAvailable)
    {
      mPlatform = Platform::XLIB;
      retval.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
    }
    else if (waylandAvailable)
    {
      mPlatform = Platform::WAYLAND;
      retval.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
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

void Graphics::Create()
{

  auto extensions = PrepareDefaultInstanceExtensions();

  auto layers = vk::enumerateInstanceLayerProperties();
  std::vector<const char*> validationLayers;
  for( auto&& reqLayer : VALIDATION_LAYERS )
  {
    for( auto&& prop : layers.value )
    {
      //std::cout << prop.layerName << std::endl;
      if( std::string(prop.layerName) == reqLayer )
      {
        validationLayers.push_back(reqLayer);
      }
    }
  }

  CreateInstance(extensions, validationLayers);
  PreparePhysicalDevice();
}

void Graphics::CreateInstance( const std::vector<const char*>& extensions, const std::vector<const char*>& validationLayers )
{
  auto info = vk::InstanceCreateInfo{};

  info.setEnabledExtensionCount(U32(extensions.size()))
      .setPpEnabledExtensionNames(extensions.data())
      //.setEnabledLayerCount(U32(validationLayers.size()))
      .setEnabledLayerCount(0)
      .setPpEnabledLayerNames(validationLayers.data());

  mInstance = VkAssert(vk::createInstance(info, *mAllocator));
}

void Graphics::DestroyInstance()
{
  if(mInstance)
  {
    mInstance.destroy(*mAllocator);
    mInstance = nullptr;
  }
}


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

  mDeviceMemoryManager = GpuMemoryManager::New( *this );
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

  auto surface = vkFactory->Create(mInstance, mAllocator.get(), mPhysicalDevice);

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

    mDevice = VkAssert(mPhysicalDevice.createDevice(info, *mAllocator));
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

const vk::AllocationCallbacks& Graphics::GetAllocator() const
{
  return *mAllocator;
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

Handle< CommandPool > Graphics::CreateCommandPool(const vk::CommandPoolCreateInfo& info)
{
  auto cmdpool = CommandPool::New( *this, vk::CommandPoolCreateInfo{}.
                                                                       setQueueFamilyIndex( 0u ).
                                                                       setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer ) );
  return cmdpool;
}

Surface& Graphics::GetSurface( FBID surfaceId )
{
  // TODO: FBID == 0 means default framebuffer, but there should be no
  // such thing as default framebuffer.
  if( surfaceId == 0 )
  {
    return *mSurfaceFBIDMap.begin()->second.get();
  }
  return *mSurfaceFBIDMap[surfaceId].get();
}

// TODO: all this stuff should go into some vulkan cache

void Graphics::AddBuffer( Handle<Buffer> buffer )
{
  mBuffersCache.push_back( buffer );
}

void Graphics::AddImage( Handle<Image> image )
{
  mImageCache.push_back( image );
}

void Graphics::AddPipeline( Handle<Pipeline> pipeline )
{
  mPipelineCache.push_back( pipeline );
}

void Graphics::AddShader( Handle<Shader> shader )
{
  mShaderCache.push_back( shader );
}

void Graphics::AddCommandPool( Handle<CommandPool> pool )
{
  mCommandPoolCache.push_back( pool );
}

void Graphics::AddDescriptorPool( Handle<DescriptorPool> pool )
{
  mDescriptorPoolCache.push_back( pool );
}

void Graphics::AddFramebuffer( Handle<Framebuffer> framebuffer )
{
  mFramebufferCache.push_back( framebuffer );
}

void Graphics::RemoveBuffer( Buffer& buffer )
{
  auto index = 0u;
  for( auto&& iter : mBuffersCache )
  {
    if( &*iter == &buffer )
    {
      iter.Reset();
      mBuffersCache.erase( mBuffersCache.begin()+index );
      return;
    }
  }
}

void Graphics::RemoveShader( Shader& shader )
{
  auto index = 0u;
  for( auto&& iter : mShaderCache )
  {
    if( &*iter == &shader )
    {
      iter.Reset();
      mShaderCache.erase( mShaderCache.begin()+index );
      return;
    }
    ++index;
  }
}

void Graphics::RemoveCommandPool( CommandPool& commandPool )
{
  NotImplemented();
}

void Graphics::RemoveDescriptorPool( std::unique_ptr<DescriptorPool> pool )
{
  NotImplemented();
}

Handle<Shader> Graphics::FindShader( vk::ShaderModule shaderModule )
{
  for( auto&& iter : mShaderCache )
  {
    if( iter->GetVkShaderModule() == shaderModule )
    {
      return Handle<Shader>(&*iter);
    }
  }
  return Handle<Shader>();
}


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
