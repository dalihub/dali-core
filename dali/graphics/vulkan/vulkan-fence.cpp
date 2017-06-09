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
#include <dali/graphics/vulkan/vulkan-fence.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

Fence::Fence(Graphics& graphics) : mGraphics(graphics), mFence(nullptr)
{
  mFence =
      VkAssert(mGraphics.GetDevice().createFence(vk::FenceCreateInfo{}, mGraphics.GetAllocator()));
}

Fence::~Fence()
{
  if(mFence)
  {
    mGraphics.GetDevice().destroyFence(mFence, mGraphics.GetAllocator());
  }
}

bool Fence::Wait(uint32_t timeout)
{
  if(timeout)
  {
    return mGraphics.GetDevice().waitForFences(mFence, true, timeout) == vk::Result::eSuccess;
  }
  else
  {
    timeout = 16000000;
    while(mGraphics.GetDevice().waitForFences(mFence, true, timeout) != vk::Result::eSuccess)
    {
      // fixme: busy wait, bit ugly
    }
    return true;
  }
}

void Fence::Reset()
{
  if(mFence)
  {
    mGraphics.GetDevice().resetFences(mFence);
  }
}

vk::Fence Fence::GetFence() const
{
  return mFence;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
