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

#include <dali/graphics/vulkan/common.h>
#include <dali/graphics/vulkan/physical-device.h>
#include <dali/graphics/vulkan/logical-device.h>
#include <dali/graphics/vulkan/surface.h>
#include <dali/graphics/vulkan/surface/xcb-surface.h>
#include <dali/graphics/vulkan/surface/xlib-surface.h>
#include <cstdarg>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

#define MAX_VKLOG_SIZE (1024 * 1024) // 1mb of log memory per thread
__thread char gLogBuffer[MAX_VKLOG_SIZE];

// something to replace in the future, maybe
void VkLog(const char* format, ...)
{
  std::va_list args;
  va_start(args, format);
  vsnprintf(gLogBuffer, MAX_VKLOG_SIZE - 4, format, args);
  puts(gLogBuffer);
  va_end(args);
}

namespace
{

template< typename T >
inline int Eval(const T& type)
{
  return static_cast< int >(type);
}

// debug callbacks
#if VULKAN_USE_DEBUG_REPORT_CALLBACK
VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT      flags,
                                                   VkDebugReportObjectTypeEXT objectType,
                                                   uint64_t                   object,
                                                   size_t                     location,
                                                   int32_t                    messageCode,
                                                   const char*                pLayerPrefix,
                                                   const char*                pMessage,
                                                   void*                      pUserData)
{
  VkLog("[VALIDATION:%s]: %s", pLayerPrefix, pMessage);
  return VK_FALSE;
}
#endif
const char* VALIDATION_LAYERS[] = {"VK_LAYER_LUNARG_screenshot",           // screenshot
                                   "VK_LAYER_LUNARG_parameter_validation", // parameter
                                   "VK_LAYER_LUNARG_vktrace", // vktrace ( requires vktrace connection )
                                   "VK_LAYER_LUNARG_monitor",             // monitor
                                   "VK_LAYER_LUNARG_swapchain",           // swapchain
                                   "VK_LAYER_GOOGLE_threading",           // threading
                                   "VK_LAYER_LUNARG_api_dump",            // api
                                   "VK_LAYER_LUNARG_object_tracker",      // objects
                                   "VK_LAYER_LUNARG_core_validation",     // core
                                   "VK_LAYER_GOOGLE_unique_objects",      // unique objects
                                   "VK_LAYER_LUNARG_standard_validation", // standard
                                   nullptr};
}

/**
 * Internal implementation of the Vulkan instance/physical device
 */
class PhysicalDeviceImpl : public Integration::GraphicsPhysicalDeviceBase
{
public:
  //  GraphicsPhysicalDeviceBase
public:
  virtual bool Initialise(const ExtensionNameList& extensions, const ValidationLayerFlags2& layers);

  virtual bool IsExtensionAvailable(const std::string& instanceExtensionName);

  virtual bool IsLayerAvailable(const std::string& instanceExtensionName);

  virtual GraphicsLogicalDevice CreateLogicalDevice(const ExtensionNameList& enabledExtensions);

  virtual bool ChoosePhysicalDevice(const PhysicalDeviceFlags& flags);

  virtual void SetValidationDebugChannels(const ValidationChannelFlags& flags)
  {
  }

  virtual GraphicsSurface CreateSurface(const NativeSurfaceCreateInfo& info);

  void PrepareQueueFamilies();

  // Vulkan getters
public:

  vk::Instance GetInstance() const
  {
    return mVkInstance;
  }

  vk::PhysicalDevice GetPhysicalDevice() const
  {
    return mVkPhysicalDevice;
  }

  const vk::PhysicalDeviceFeatures& GetFeatures() const
  {
    return mPhysFeatures;
  }

  const vk::PhysicalDeviceProperties& GetProperties() const
  {
    return mPhysProperties;
  }

  const vk::PhysicalDeviceLimits& GetLimits() const
  {
    return mPhysLimits;
  }

  const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const
  {
    return mPhysMemoryProperties;
  }

  const std::string& GetName() const
  {
    return mPhysName;
  }

  const std::string& GetVendorName() const
  {
    return mPhysVendorName;
  }

  const vk::AllocationCallbacks* GetAllocator() const
  {
    return mAllocatorCallbacks;
  }

  int GetQueueFamilyIndex(QueueType type) const
  {
    return mQueueFamilyIndex[Eval(type)];
  }

private:
  // Vulkan instance extension properties list
  std::vector< vk::ExtensionProperties > mInstanceExtensionProperties;

  // Vulkan device extension properties list
  std::vector< vk::ExtensionProperties > mDeviceExtensionProperties;

  // Vulkan instance layer properties list
  std::vector< vk::LayerProperties > mInstanceLayerProperties;

  // Allocators
  vk::AllocationCallbacks* mAllocatorCallbacks{nullptr};

  // Instance
  vk::Instance mVkInstance{nullptr};

  // Physical device
  vk::PhysicalDevice                 mVkPhysicalDevice{nullptr};
  vk::PhysicalDeviceFeatures         mPhysFeatures{};
  vk::PhysicalDeviceProperties       mPhysProperties{};
  vk::PhysicalDeviceMemoryProperties mPhysMemoryProperties{};
  vk::PhysicalDeviceType             mPhysType;
  vk::PhysicalDeviceLimits           mPhysLimits;
  uint32_t                           mPhysApiVersion{0};
  uint32_t                           mPhysDriverVersion{0};
  std::string                        mPhysName;
  std::string                        mPhysVendorName;
  bool                               mPhysIsDiscrete{false};

  // Queue families
  std::vector< vk::QueueFamilyProperties > mQueueFamilyProperties;
  std::array< int, static_cast< uint32_t >(QueueType::END) > mQueueFamilyIndex;

  // todo add surface type register
};

bool PhysicalDeviceImpl::IsExtensionAvailable(const std::string& instanceExtensionName)
{
  if(mInstanceExtensionProperties.empty())
  {
    auto result = vk::enumerateInstanceExtensionProperties();
    if(result.result != vk::Result::eSuccess)
    {
      // todo no vulkan support????
      return false;
    }
    mInstanceExtensionProperties = std::move(result.value);
  }

  for(auto& ext : mInstanceExtensionProperties)
  {
    if(instanceExtensionName == ext.extensionName)
    {
      return true;
    }
  }
  return false;
}

bool PhysicalDeviceImpl::IsLayerAvailable(const std::string& instanceLayerName)
{
  if(mInstanceLayerProperties.empty())
  {
    auto result = vk::enumerateInstanceLayerProperties();
    if(result.result != vk::Result::eSuccess)
    {
      // todo no vulkan support????
      return false;
    }
    mInstanceLayerProperties = std::move(result.value);
  }

  for(auto& ext : mInstanceLayerProperties)
  {
    if(instanceLayerName == ext.layerName)
    {
      return true;
    }
  }
  return false;
}

bool PhysicalDeviceImpl::Initialise(const ExtensionNameList& extensions, const ValidationLayerFlags2& layers)
{
  // make a copy and add debug report extension in case there are any
  // validation layers listed
  auto extCopy = ExtensionNameList{extensions};
  if(layers != 0u)
  {
    extCopy.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  }

  // add validation layers
  LayerNameList layerTable;
  auto          retval = LayerNameList{};
  for(uint32_t bit = 0; VALIDATION_LAYERS[bit]; ++bit)
  {
    if(layers & (1 << bit))
    {
      layerTable.emplace_back(VALIDATION_LAYERS[bit]);
    }
  }

  vk::InstanceCreateInfo info;
  info.setEnabledExtensionCount(extCopy.size())
      .setPpEnabledExtensionNames(extCopy.data())
      .setPApplicationInfo(nullptr)
      .setPpEnabledLayerNames(layerTable.empty() ? nullptr : layerTable.data())
      .setEnabledLayerCount(layerTable.size());

  return (vk::Result::eSuccess == VkTestCall(vk::createInstance(&info, mAllocatorCallbacks, &mVkInstance)));
}

/**
 *
 * @param flags
 * @return
 */
bool PhysicalDeviceImpl::ChoosePhysicalDevice(const PhysicalDeviceFlags& flags)
{
  auto preferredDiscrete = flags & PhysicalDeviceBit::DISCRETE ? true : false;

  // set preferred device type
  const vk::PhysicalDeviceType preferredType =
      preferredDiscrete ? vk::PhysicalDeviceType::eDiscreteGpu : vk::PhysicalDeviceType::eIntegratedGpu;

  // set fallback device type
  const vk::PhysicalDeviceType fallbackType =
      preferredDiscrete ? vk::PhysicalDeviceType::eIntegratedGpu : vk::PhysicalDeviceType::eDiscreteGpu;

  auto result = mVkInstance.enumeratePhysicalDevices();
  VkAssertCallMsg(result.result, "No Vulkan devices found!");

  auto GPUs            = result.value;
  auto currentGPU      = GPUs.front();
  auto currentGPUProps = currentGPU.getProperties();

  // select discreet over integrated
  for(auto& gpu : GPUs)
  {
    auto gpuProperties = gpu.getProperties();
    if((currentGPUProps.deviceType != preferredType && gpuProperties.deviceType == preferredType) ||
       (currentGPUProps.deviceType != preferredType && gpuProperties.deviceType == fallbackType))
    {
      currentGPU      = gpu;
      currentGPUProps = gpuProperties;
    }
  }

  // make sure selected GPU is THE REAL GPU not a blanket or refrigerator
  if(currentGPUProps.deviceType != preferredType && currentGPUProps.deviceType != fallbackType)
  {
    // error
    return false;
  }

  // store GPU data
  // fixme
  auto& gpu                 = currentGPU;
  auto& gpuProperties       = currentGPUProps;
  auto  gpuMemoryProperties = currentGPU.getMemoryProperties();
  auto  gpuFeatures         = currentGPU.getFeatures();

  auto deviceExtensionProperties = gpu.enumerateDeviceExtensionProperties();
  mDeviceExtensionProperties     = deviceExtensionProperties.value;

  auto queueFamilyProperties = gpu.getQueueFamilyProperties();

  // collect all physical device related information
  mPhysIsDiscrete       = preferredDiscrete;
  mVkPhysicalDevice     = gpu;
  mPhysProperties       = gpuProperties;
  mPhysFeatures         = gpuFeatures;
  mPhysMemoryProperties = gpuMemoryProperties;
  mPhysLimits           = mPhysProperties.limits;
  mPhysType             = mPhysProperties.deviceType;

  // fill common stuff
  mPhysName = mPhysProperties.deviceName;

  switch(mPhysProperties.vendorID)
  {
  case 0x10DE: //nvidia
    mPhysVendorName = "NVIDIA";
    break;
  default:
    mPhysVendorName = "Unknown";
  }

  mPhysApiVersion    = mPhysProperties.apiVersion;
  mPhysDriverVersion = mPhysProperties.driverVersion;

  VkLog("[Vulkan] Found physical device: %s", mPhysName.c_str());
  VkLog("[Vulkan]                vendor: %s", mPhysVendorName.c_str());

  // prepare queue families
  PrepareQueueFamilies();

  return true;
}

GraphicsLogicalDevice PhysicalDeviceImpl::CreateLogicalDevice(const ExtensionNameList& enabledExtensions)
{
  LogicalDevice logicDevice( LogicalDevice::New( this ) );
  if(!logicDevice.Initialise())
  {
    logicDevice = nullptr;
  }
  return logicDevice;
}

void PhysicalDeviceImpl::PrepareQueueFamilies()
{
  int& familyGraphics{mQueueFamilyIndex[Eval(QueueType::GRAPHICS)]};
  int& familyTransfer{mQueueFamilyIndex[Eval(QueueType::TRANSFER)]};
  int& familyCompute{mQueueFamilyIndex[Eval(QueueType::COMPUTE)]};
  int& familySparseBinding{mQueueFamilyIndex[Eval(QueueType::SPARSE_BINDING)]};
  int& familyPresent{mQueueFamilyIndex[Eval(QueueType::PRESENT)]};
  familyGraphics = familyTransfer = familyCompute = familySparseBinding = familyPresent = -1;

  mQueueFamilyProperties = mVkPhysicalDevice.getQueueFamilyProperties();

  int queueIndex = 0;
  for(auto& queueFamily : mQueueFamilyProperties)
  {
    if(familyGraphics < 0 && (queueFamily.queueFlags | vk::QueueFlagBits::eGraphics))
    {
      familyGraphics = queueIndex;
    }
    if(familyTransfer < 0 && (queueFamily.queueFlags | vk::QueueFlagBits::eTransfer))
    {
      familyTransfer = queueIndex;
    }
    if(familyCompute < 0 && (queueFamily.queueFlags | vk::QueueFlagBits::eCompute))
    {
      familyCompute = queueIndex;
    }
    if(familySparseBinding < 0 && (queueFamily.queueFlags | vk::QueueFlagBits::eSparseBinding))
    {
      familySparseBinding = queueIndex;
    }

    ++queueIndex;
  }
}

GraphicsSurface PhysicalDeviceImpl::CreateSurface(const NativeSurfaceCreateInfo& info)
{
  int& familyPresent{mQueueFamilyIndex[Eval(QueueType::PRESENT)]};

  assert( familyPresent < 0 && "Looks like the surface has been created!");

  // should be part of adaptor????
  GraphicsSurface retval{nullptr};
  if( info.surfaceType == NativeSurfaceType::XCB )
  {
    const XcbSurfaceCreateInfo* xcbInfo = reinterpret_cast<const XcbSurfaceCreateInfo*>(&info);
    GraphicsPhysicalDevice physDevice(this);
    retval = Surface::New<XcbSurface>( physDevice, xcbInfo->connection, xcbInfo->window );
  }
  else if( info.surfaceType == NativeSurfaceType::X11 )
  {
    const XlibSurfaceCreateInfo* xlibInfo = reinterpret_cast<const XlibSurfaceCreateInfo*>(&info);
    GraphicsPhysicalDevice physDevice(this);
    retval = Surface::New<XlibSurface>( physDevice, xlibInfo->display, xlibInfo->window );
  }
  // check surface compatibility against queue families
  int queueIndex = 0;
  for(size_t i = 0; i < mQueueFamilyProperties.size(); ++i)
  {

    // set present family upon surface creation
    if(familyPresent < 0)
    {
      vk::Bool32 supported{false};

      mVkPhysicalDevice.getSurfaceSupportKHR(queueIndex, Surface(retval.GetObject()).GetSurface(), &supported);
      if(supported)
      {
        familyPresent = queueIndex;
        break;
      }
    }
    ++queueIndex;
  }

  assert( familyPresent >= 0 && "Surface not supported" );

  return retval;
}

/**************************************************************************
 * Handle implementation
 */

PhysicalDeviceImpl* GetImpl(const PhysicalDevice* handle)
{
  return static_cast< PhysicalDeviceImpl* >(handle->GetObject());
}

GraphicsPhysicalDevice PhysicalDevice::New()
{
  return GraphicsPhysicalDevice(new PhysicalDeviceImpl());
}

vk::Instance PhysicalDevice::GetInstance() const
{
  return GetImpl(this)->GetInstance();
}

vk::PhysicalDevice PhysicalDevice::GetPhysicalDevice() const
{
  return GetImpl(this)->GetPhysicalDevice();
}

const vk::PhysicalDeviceFeatures& PhysicalDevice::GetFeatures() const
{
  return GetImpl(this)->GetFeatures();
}

const vk::PhysicalDeviceProperties& PhysicalDevice::GetProperties() const
{
  return GetImpl(this)->GetProperties();
}

const vk::PhysicalDeviceLimits& PhysicalDevice::GetLimits() const
{
  return GetImpl(this)->GetLimits();
}

const vk::PhysicalDeviceMemoryProperties& PhysicalDevice::GetMemoryProperties() const
{
  return GetImpl(this)->GetMemoryProperties();
}

const std::string& PhysicalDevice::GetName() const
{
  return GetImpl(this)->GetName();
}

const std::string& PhysicalDevice::GetVendorName() const
{
  return GetImpl(this)->GetVendorName();
}

const vk::AllocationCallbacks* PhysicalDevice::GetAllocator() const
{
  return GetImpl(this)->GetAllocator();
}

int PhysicalDevice::GetQueueFamilyIndex(QueueType type) const
{
  return GetImpl(this)->GetQueueFamilyIndex(type);
}

} // Vulkan
} // Graphics
} // Dali