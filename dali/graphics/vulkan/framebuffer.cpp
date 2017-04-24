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

#include "framebuffer.h"
#include <dali/graphics/vulkan/logical-device.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
namespace Internal
{
class Framebuffer : public VkObject
{
public:
  Framebuffer() = delete;

  Framebuffer(const LogicalDevice &device, const vk::FramebufferCreateInfo &info)
  : VkObject(), mDevice(device), mCreateInfo(info)
  {
  }

  virtual ~Framebuffer() = default;

  bool Initialise();

  const vk::Framebuffer &GetVkObject() const
  {
    return mFramebuffer;
  }

  virtual bool OnSafeDelete() override
  {
    if( mFramebuffer )
    {
      mDevice->destroyFramebuffer(mFramebuffer, mDevice.GetVkAllocator());
      return true;
    }
    return false;
  }

private:
  LogicalDevice mDevice;

  vk::FramebufferCreateInfo mCreateInfo {};
  vk::Framebuffer mFramebuffer {nullptr};


};

bool Framebuffer::Initialise()
{
  return VkTestBool(mDevice->createFramebuffer(&mCreateInfo, mDevice.GetVkAllocator(), &mFramebuffer));
};

}

namespace
{
Internal::Framebuffer* GetImpl( const Framebuffer* handle )
{
  return static_cast<Internal::Framebuffer*>(handle->GetObject());
}

}

Framebuffer Framebuffer::New(const class LogicalDevice& device, const vk::FramebufferCreateInfo& info)
{
  auto impl = new Internal::Framebuffer(device, info);
  if(impl->Initialise())
  {
    return impl;
  }
  return nullptr;
}

const vk::Framebuffer& Framebuffer::operator*() const
{
  return GetImpl(this)->GetVkObject();
}

}
}
}