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
#include <dali/graphics/vulkan/frame-stack.h>
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
  LogicalDeviceImpl(const PhysicalDevice &physicalDevice)
  : GraphicsLogicalDeviceBase(), mPhysicalDevice(physicalDevice), mStack{1024}
  {
  }

  const vk::Device& GetDevice() const
  {
    return mDevice;
  }

  const PhysicalDevice &GetPhysicalDevice() const
  {
    return mPhysicalDevice;
  }

  virtual bool Initialise() override;

  virtual GraphicsSwapchain CreateSwapchain(const GraphicsSurface &surface,
                                            uint32_t bufferCount,
                                            DepthStencil depthStencil,
                                            bool enforceVSync) override;

  virtual GraphicsPhysicalDevice GetGraphicsPhysicalDevice() const override
  {
    return GraphicsPhysicalDevice(mPhysicalDevice);
  }

  const CommandQueue &GetCommandQueue(uint32_t index, QueueType type) const;

  vk::MemoryRequirements GetResourceMemoryRequirements(const vk::Image &image) const;

  vk::MemoryRequirements GetResourceMemoryRequirements(const vk::Buffer &buffer) const;

  uint32_t GetMemoryIndex(uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties);

  template< typename T >
  DeviceMemory AllocateMemory(T object, vk::MemoryPropertyFlags flags, bool doBind);

  Image CreateImage(vk::ImageCreateInfo info);

  Stack& GetStack( uint32_t stackIndex )
  {
    return mStack;
  }

private:
  vk::Device mDevice {nullptr};
  PhysicalDevice mPhysicalDevice;

  ExtensionNameList mEnabledExtensionCStr;
  std::vector< std::string > mEnabledExtensionStr;
  FamilyQueueArray mQueueFamilyArray; // queues allocated per family
  Stack mStack;
  //VulkanCommandPool mTransientPool;
};


bool LogicalDeviceImpl::Initialise()
{
  auto physDevice = mPhysicalDevice.GetPhysicalDevice();
  auto& features = mPhysicalDevice.GetFeatures();
  auto& stack = GetStack(0);
  stack.Mark();

  // for each family allocate all possible queues when creating device
  std::vector< vk::DeviceQueueCreateInfo > queueInfoArray;

  // make a copy of used families and sort
  std::vector< int > familyIndices;

  auto newArray = std::vector< uint32_t >{mPhysicalDevice.GetQueueFamilyIndex(QueueType::GRAPHICS),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::COMPUTE),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::TRANSFER),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::SPARSE_BINDING),
                                     mPhysicalDevice.GetQueueFamilyIndex(QueueType::PRESENT)};

  auto queueFamilyProperties = physDevice.getQueueFamilyProperties();

  std::sort(newArray.begin(), newArray.end());

  mQueueFamilyArray.resize(newArray.back() + 1);

  uint32_t oldIndex = -1u;
  std::vector< float > priorities;
  for(auto &newIndex : newArray)
  {
    if(newIndex != oldIndex)
    {
      auto &familyProperties = queueFamilyProperties[newIndex];

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

  mEnabledExtensionCStr.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  vk::DeviceCreateInfo deviceInfo;
  deviceInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueInfoArray.size()));
  deviceInfo.setPQueueCreateInfos(queueInfoArray.data());
  deviceInfo.setEnabledExtensionCount(static_cast<uint32_t>(mEnabledExtensionCStr.size()));
  deviceInfo.setPpEnabledExtensionNames(mEnabledExtensionCStr.data());
  deviceInfo.setPEnabledFeatures(&features);

  VkAssertCall(physDevice.createDevice(&deviceInfo, mPhysicalDevice.GetAllocator(), &mDevice));

  int familyIndex = 0;

  // based on queue infos create queue objects.

  // make a handle of the implementation
  LogicalDevice logicalDevice(this);
  for(auto &family : mQueueFamilyArray)
  {
    for(uint32_t queueIndex = 0; queueIndex < family.size(); ++queueIndex)
    {
      family[queueIndex] = CommandQueue::Get(logicalDevice, static_cast< QueueType >(familyIndex), queueIndex);
    }
    ++familyIndex;
  }

  VkLog("[GraphicsPhysicalDevice] VkDevice created.");

  return true;
}

GraphicsSwapchain LogicalDeviceImpl::CreateSwapchain(const GraphicsSurface &surface,
                                                     uint32_t bufferCount,
                                                     DepthStencil depthStencil,
                                                     bool enforceVSync)
{
  auto swapchain = Swapchain::New(LogicalDevice(this), Surface(surface.GetObject()), bufferCount, depthStencil);
  swapchain.Initialise();
  return swapchain;
}

const CommandQueue &LogicalDeviceImpl::GetCommandQueue(uint32_t index, QueueType type) const
{
  return mQueueFamilyArray[mPhysicalDevice.GetQueueFamilyIndex(type)][index];
}

vk::MemoryRequirements LogicalDeviceImpl::GetResourceMemoryRequirements(const vk::Image &image) const
{
  return mDevice.getImageMemoryRequirements(image);
}

vk::MemoryRequirements LogicalDeviceImpl::GetResourceMemoryRequirements(const vk::Buffer &buffer) const
{
  return mDevice.getBufferMemoryRequirements(buffer);
}

uint32_t LogicalDeviceImpl::GetMemoryIndex(uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties)
{
  auto &memprops = mPhysicalDevice.GetMemoryProperties();
  for(uint32_t i = 0; i < memprops.memoryTypeCount; ++i)
  {
    if((memoryTypeBits & (1u << i)) && ((memprops.memoryTypes[i].propertyFlags & properties) == properties))
    {
      return i;
    }
  }
  return -1u;
}

template< typename T >
DeviceMemory LogicalDeviceImpl::AllocateMemory(T object, vk::MemoryPropertyFlags flags, bool doBind)
{
  vk::MemoryRequirements req(GetResourceMemoryRequirements(*object));

  vk::MemoryAllocateInfo info;
  uint32_t memoryIndex = GetMemoryIndex(req.memoryTypeBits, flags);
  assert( memoryIndex != -1u && "Wrong memory index!");
  info.setMemoryTypeIndex(memoryIndex).setAllocationSize(req.size);

  LogicalDevice deviceHandle( this );
  DeviceMemory memory = DeviceMemory::New( deviceHandle, info );

  if( doBind )
  {
    object.BindDeviceMemory( memory );
  }

  return memory;
}

Image LogicalDeviceImpl::CreateImage(vk::ImageCreateInfo info)
{
  return nullptr;
}

// End of implementation

//

namespace
{
LogicalDeviceImpl *GetImpl(const LogicalDevice *handle)
{
  return static_cast< LogicalDeviceImpl * >(handle->GetObject());
}

LogicalDeviceImpl *GetImpl(LogicalDevice *handle)
{
  return static_cast< LogicalDeviceImpl * >(handle->GetObject());
}

} //

LogicalDevice LogicalDevice::New(const PhysicalDevice &physicalDevice)
{
  return LogicalDevice(new LogicalDeviceImpl(physicalDevice));
}

bool LogicalDevice::Initialise()
{
  return GetImpl(this)->Initialise();
}

const vk::Device* LogicalDevice::operator->() const
{
  auto& vkdevice = GetImpl(this)->GetDevice();
  return &vkdevice;
}

DeviceMemory LogicalDevice::AllocateImageMemory(const Image& image, vk::MemoryPropertyFlags flags, bool doBind) const
{
  return GetImpl(this)->AllocateMemory(image, flags, doBind);
}

DeviceMemory LogicalDevice::AllocateBufferMemory(const Buffer& buffer, vk::MemoryPropertyFlags flags, bool doBind) const
{
  return GetImpl(this)->AllocateMemory(buffer, flags, doBind);
}

const vk::Device LogicalDevice::GetVkDevice() const
{
  return GetImpl(this)->GetDevice();
}

const PhysicalDevice &LogicalDevice::GetPhysicalDevice() const
{
  return GetImpl(this)->GetPhysicalDevice();
}

const vk::AllocationCallbacks *LogicalDevice::GetVkAllocator() const
{
  return GetImpl(this)->GetPhysicalDevice().GetAllocator();
}

CommandPool LogicalDevice::CreateCommandPool(QueueType type, bool createTransient, bool createResetCommandBuffer) const
{
  return CommandPool::New(*this, type, false, createTransient, createResetCommandBuffer);
}

const CommandQueue &LogicalDevice::GetCommandQueue(uint32_t index, QueueType type) const
{
  return GetImpl(this)->GetCommandQueue(index, type);
}

Image LogicalDevice::CreateImage(vk::ImageCreateInfo imageInfo)
{
  return GetImpl(this)->CreateImage(imageInfo);
}

Stack& LogicalDevice::GetStack( uint32_t stackIndex )
{
  return GetImpl(this)->GetStack( stackIndex );
}


}
}
}
