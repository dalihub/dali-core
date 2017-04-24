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
#include <dali/graphics/vulkan/physical-device.h>
#include <dali/graphics/vulkan/logical-device.h>
#include <dali/graphics/vulkan/command-buffer.h>
#include <dali/graphics/vulkan/command-pool.h>
#include <dali/graphics/vulkan/command-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class QueueImpl : public VkObject
{
public:
  QueueImpl(const LogicalDevice& device, QueueType type, int queueIndex)
  : VkObject(), mDevice(device), mType(type), mQueue(nullptr), mQueueIndex(queueIndex)
  {
  }

  bool Initialise()
  {
    auto phDevice = mDevice.GetPhysicalDevice();
    auto vkDevice = mDevice.GetVkDevice();

    int familyIndex = phDevice.GetQueueFamilyIndex(mType);
    vkDevice.getQueue(familyIndex, mQueueIndex, &mQueue);
    return true;
  }

  bool Submit(const std::vector< CommandBuffer >& buffers, vk::Fence fence = nullptr)
  {
    return Submit(buffers.data(), buffers.size(), nullptr, 0, nullptr, 0, nullptr, fence);
  }

  bool Submit(const CommandBuffer* pBuffers, const uint32_t bufferCount,
              const vk::Semaphore* pWaitSemaphores, const uint32_t waitSemaphoreCount,
              const vk::Semaphore* pSignalSemaphores, const uint32_t signalSemaphoreCount,
              const vk::PipelineStageFlags* pWaitDstStageFlags, vk::Fence fence)
  {
    // todo make temporary copy more efficient
    vk::CommandBuffer vkBuffers[bufferCount];
    for(int index = 0; index < bufferCount; ++index)
    {
      vkBuffers[index] = pBuffers[index].GetVkBuffer();
    }

    if( fence )
    {
      mDevice.GetVkDevice().resetFences( 1, &fence );
    }

    vk::SubmitInfo info;
    info.setCommandBufferCount(bufferCount)
        .setPCommandBuffers(vkBuffers)
        .setWaitSemaphoreCount(waitSemaphoreCount)
        .setPWaitSemaphores(pWaitSemaphores)
        .setSignalSemaphoreCount(signalSemaphoreCount)
        .setPSignalSemaphores(pSignalSemaphores)
        .setPWaitDstStageMask(pWaitDstStageFlags);

    if(VkTestCall(mQueue.submit(1, &info, fence)) != vk::Result::eSuccess)
    {
      return false;
    }
    return true;
  }

  LogicalDevice  mDevice;
  QueueType      mType;
  vk::Queue      mQueue;
  int            mQueueIndex;
};

namespace
{
QueueImpl* GetImpl(const CommandQueue* queue)
{
  return static_cast< QueueImpl* >(queue->GetObject());
}
QueueImpl* GetImpl(CommandQueue* queue)
{
  return static_cast< QueueImpl* >(queue->GetObject());
}
}

CommandQueue CommandQueue::Get(const LogicalDevice& device, QueueType type, int queueIndex)
{
  auto impl = new QueueImpl(device, type, queueIndex);
  impl->Initialise();
  CommandQueue retval( impl );
  return retval;
}

vk::Queue CommandQueue::GetVkQueue() const
{
  return GetImpl(this)->mQueue;
}

int CommandQueue::GetIndex() const
{
  return GetImpl(this)->mQueueIndex;
}

QueueType CommandQueue::GetType() const
{
  return GetImpl(this)->mType;
}

bool CommandQueue::Submit(const std::vector< CommandBuffer >& buffers, vk::Fence fence)
{
  return GetImpl(this)->Submit(buffers, fence);
}

bool CommandQueue::Submit(const CommandBuffer* buffers, uint32_t count, vk::Fence fence)
{
  return GetImpl(this)->Submit( buffers, count, nullptr, 0, nullptr, 0, nullptr, fence );
}

bool CommandQueue::Submit(const CommandBuffer* pBuffers, const uint32_t bufferCount,
            const vk::Semaphore* pWaitSemaphores, const uint32_t waitSemaphoreCount,
            const vk::Semaphore* pSignalSemaphores, const uint32_t signalSemaphoreCount,
            const vk::PipelineStageFlags* pWaitDstStageFlags, vk::Fence fence) const
{
  return GetImpl(this)->Submit( pBuffers, bufferCount,
                                pWaitSemaphores, waitSemaphoreCount,
                                pSignalSemaphores, signalSemaphoreCount,
                                pWaitDstStageFlags, fence );
}

}
}
}