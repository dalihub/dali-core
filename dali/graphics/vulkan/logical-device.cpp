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

#include <dali/graphics/vulkan/command-queue.h>
#include <dali/graphics/vulkan/logical-device.h>
#include <dali/graphics/vulkan/physical-device.h>
#include <dali/graphics/vulkan/surface.h>
#include <dali/graphics/vulkan/swapchain.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

using QueueArray       = std::vector< CommandQueue >;
using FamilyQueueArray = std::vector< QueueArray >;

class LogicalDeviceImpl : public GraphicsLogicalDeviceBase
{
public:
  LogicalDeviceImpl(const PhysicalDevice& physicalDevice)
  : GraphicsLogicalDeviceBase(), mPhysicalDevice(physicalDevice)
  {
  }

  const vk::Device GetDevice() const
  {
    return mDevice;
  }

  const PhysicalDevice& GetPhysicalDevice() const
  {
    return mPhysicalDevice;
  }

  virtual bool Initialise();

  virtual GraphicsSwapchain CreateSwapchain(const GraphicsSurface& surface,
                                            uint32_t               bufferCount,
                                            DepthStencil           depthStencil,
                                            bool                   enforceVSync);

  virtual GraphicsPhysicalDevice GetGraphicsPhysicalDevice() const
  {
    return GraphicsPhysicalDevice(mPhysicalDevice);
  }

  virtual const GraphicsSurface& GetGraphicsSurface() const
  {
  }

  const CommandQueue& GetCommandQueue(uint32_t index, QueueType type) const;

  vk::MemoryRequirements GetResourceMemoryRequirements(const vk::Image& image) const;

  vk::MemoryRequirements GetResourceMemoryRequirements(const vk::Buffer& buffer) const;

  bool BindResourceMemory(vk::Image resource, vk::DeviceMemory memory, uint32_t offset);

  bool BindResourceMemory(vk::Buffer resource, vk::DeviceMemory memory, uint32_t offset);

  int32_t GetMemoryIndex(uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties);

  template< typename T >
  vk::DeviceMemory AllocateMemory(T object, vk::MemoryPropertyFlags flags, bool doBind);

private:
  vk::Device     mDevice{nullptr};
  PhysicalDevice mPhysicalDevice;

  ExtensionNameList          mEnabledExtensionCStr;
  std::vector< std::string > mEnabledExtensionStr;
  FamilyQueueArray           mQueueFamilyArray; // queues allocated per family
  //VulkanCommandPool mTransientPool;
};

bool LogicalDeviceImpl::Initialise()
{
  auto physDevice = mPhysicalDevice.GetPhysicalDevice();
  auto features   = mPhysicalDevice.GetFeatures();

  // for each family allocate all possible queues when creating device
  std::vector< vk::DeviceQueueCreateInfo > queueInfoArray;

  // make a copy of used families and sort
  std::vector< int > familyIndices;

  auto newArray = std::vector< int >{mPhysicalDevice.GetQueueFamilyIndex(QueueType::GRAPHICS),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::COMPUTE),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::TRANSFER),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::SPARSE_BINDING),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::PRESENT)};

  auto queueFamilyProperties = physDevice.getQueueFamilyProperties();

  std::sort(newArray.begin(), newArray.end());

  mQueueFamilyArray.resize(newArray.back()+1);

  int                  oldIndex = -1;
  std::vector< float > priorities;
  for(auto& newIndex : newArray)
  {
    if(newIndex != oldIndex)
    {
      auto& familyProperties = queueFamilyProperties[newIndex];

      // set priorities ( all queues have priority 1.0 to keep it simple )
      priorities.resize(familyProperties.queueCount);
      std::fill(priorities.begin(), priorities.end(), 1.0f);

      // just resize arrays
      mQueueFamilyArray[newIndex].resize(familyProperties.queueCount);

      vk::DeviceQueueCreateInfo queueInfo;
      queueInfo.setQueueCount(familyProperties.queueCount);
      queueInfo.setQueueFamilyIndex(newIndex);
      queueInfo.setPQueuePriorities(priorities.data());

      queueInfoArray.emplace_back(queueInfo);

      oldIndex = newIndex;
    }
  }

  mEnabledExtensionCStr.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );

  vk::DeviceCreateInfo deviceInfo;
  deviceInfo.setQueueCreateInfoCount(queueInfoArray.size());
  deviceInfo.setPQueueCreateInfos(queueInfoArray.data());
  deviceInfo.setEnabledExtensionCount(mEnabledExtensionCStr.size());
  deviceInfo.setPpEnabledExtensionNames(mEnabledExtensionCStr.data());
  deviceInfo.setPEnabledFeatures(&features);

  VkAssertCall(physDevice.createDevice(&deviceInfo, mPhysicalDevice.GetAllocator(), &mDevice));

  int familyIndex = 0;

  // based on queue infos create queue objects.

  // make a handle of the implementation
  LogicalDevice logicalDevice(this);
  for(auto& family : mQueueFamilyArray)
  {
    for(int queueIndex = 0; queueIndex < family.size(); ++queueIndex)
    {
      family[queueIndex] = CommandQueue::Get(logicalDevice, static_cast< QueueType >(familyIndex), queueIndex);
    }
    ++familyIndex;
  }

  VkLog("[GraphicsPhysicalDevice] VkDevice created.");

  return true;
}

GraphicsSwapchain LogicalDeviceImpl::CreateSwapchain(const GraphicsSurface& surface,
                                                     uint32_t               bufferCount,
                                                     DepthStencil           depthStencil,
                                                     bool                   enforceVSync)
{
  auto swapchain = Swapchain::New(LogicalDevice(this), Surface(surface.GetObject()), bufferCount, depthStencil);
  swapchain.Initialise();
  return swapchain;
}

const CommandQueue& LogicalDeviceImpl::GetCommandQueue(uint32_t index, QueueType type) const
{
  return mQueueFamilyArray[mPhysicalDevice.GetQueueFamilyIndex(type)][index];
}

vk::MemoryRequirements LogicalDeviceImpl::GetResourceMemoryRequirements(const vk::Image& image) const
{
  return mDevice.getImageMemoryRequirements(image);
}

vk::MemoryRequirements LogicalDeviceImpl::GetResourceMemoryRequirements(const vk::Buffer& buffer) const
{
  return mDevice.getBufferMemoryRequirements(buffer);
}

bool LogicalDeviceImpl::BindResourceMemory(vk::Image resource, vk::DeviceMemory memory, uint32_t offset)
{
  return (VkTestCall(mDevice.bindImageMemory(resource, memory, offset)) == vk::Result::eSuccess);
}

bool LogicalDeviceImpl::BindResourceMemory(vk::Buffer resource, vk::DeviceMemory memory, uint32_t offset)
{
  return (VkTestCall(mDevice.bindBufferMemory(resource, memory, offset)) == vk::Result::eSuccess);
}

int32_t LogicalDeviceImpl::GetMemoryIndex(uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties)
{
  auto& memprops = mPhysicalDevice.GetMemoryProperties();
  for(int32_t i = 0; i < memprops.memoryTypeCount; ++i)
  {
    if((memoryTypeBits & (1 << i)) && ((memprops.memoryTypes[i].propertyFlags & properties) == properties))
    {
      return i;
    }
  }
  return -1;
}

template< typename T >
vk::DeviceMemory LogicalDeviceImpl::AllocateMemory(T object, vk::MemoryPropertyFlags flags, bool doBind)
{
  vk::MemoryRequirements req(GetResourceMemoryRequirements(object));
  vk::DeviceMemory       memory{nullptr};

  vk::MemoryAllocateInfo info;
  info.setMemoryTypeIndex(GetMemoryIndex(req.memoryTypeBits, flags)).setAllocationSize(req.size);

  VkAssertCall(mDevice.allocateMemory(&info, mPhysicalDevice.GetAllocator(), &memory));

  if(doBind)
  {
    assert(BindResourceMemory(object, memory, 0) && "Can't bind resource memory!");
  }
  return memory;
}

// End of implementation

//

namespace
{
LogicalDeviceImpl* GetImpl(const LogicalDevice* handle)
{
  return static_cast< LogicalDeviceImpl* >(handle->GetObject());
}

LogicalDeviceImpl* GetImpl(LogicalDevice* handle)
{
  return static_cast< LogicalDeviceImpl* >(handle->GetObject());
}

} //

LogicalDevice LogicalDevice::New(const PhysicalDevice& physicalDevice)
{
  return LogicalDevice(new LogicalDeviceImpl(physicalDevice));
}

bool LogicalDevice::Initialise()
{
  return GetImpl(this)->Initialise();
}

vk::DeviceMemory LogicalDevice::AllocateImageMemory(vk::Image image, vk::MemoryPropertyFlags flags, bool doBind)
{
  return GetImpl(this)->AllocateMemory(image, flags, doBind);
}

vk::DeviceMemory LogicalDevice::AllocateBufferMemory(vk::Buffer buffer, vk::MemoryPropertyFlags flags, bool doBind)
{
  return GetImpl(this)->AllocateMemory(buffer, flags, doBind);
}

const vk::Device LogicalDevice::GetVkDevice() const
{
  return GetImpl(this)->GetDevice();
}

const PhysicalDevice& LogicalDevice::GetPhysicalDevice() const
{
  return GetImpl(this)->GetPhysicalDevice();
}

const vk::AllocationCallbacks* LogicalDevice::GetVkAllocator() const
{
  return GetImpl(this)->GetPhysicalDevice().GetAllocator();
}

CommandPool LogicalDevice::CreateCommandPool(QueueType type, bool createTransient, bool createResetCommandBuffer)
{
  return CommandPool::New(*this, type, false, createTransient, createResetCommandBuffer);
}

const CommandQueue& LogicalDevice::GetCommandQueue(uint32_t index, QueueType type)
{
  return GetImpl(this)->GetCommandQueue(index, type);
}
}
}
}
