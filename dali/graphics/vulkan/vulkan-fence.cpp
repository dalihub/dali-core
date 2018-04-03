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

/**
 * Class: Fence::Impl
 *
 */
struct Fence::Impl
{
  Impl( Vulkan::Graphics& graphics )
  : mGraphics( graphics )
  {
  }

  ~Impl()
  {
    if( mFence )
    {
      mGraphics.GetDevice().destroyFence( mFence, mGraphics.GetAllocator() );
    }
  }

  vk::Result Initialise()
  {
    mFence = VkAssert( mGraphics.GetDevice().createFence( vk::FenceCreateInfo{}, mGraphics.GetAllocator() ) );
    if( mFence )
      return vk::Result::eSuccess;
    return vk::Result::eErrorInitializationFailed;
  }

  vk::Result Initialise(bool isSignaled)
  {
    vk::FenceCreateInfo info;
    if (isSignaled)
      info.setFlags(vk::FenceCreateFlagBits::eSignaled);

    mFence = VkAssert( mGraphics.GetDevice().createFence( info, mGraphics.GetAllocator() ) );
    if( mFence )
      return vk::Result::eSuccess;
    return vk::Result::eErrorInitializationFailed;
  }

  /**
   *
   * @param timeout
   * @return
   */
  bool Wait( uint32_t timeout = 0u )
  {
    if(mFence)
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
    return false;
  }

  /**
   *
   */
  void Reset()
  {
    if(mFence)
    {
      mGraphics.GetDevice().resetFences(mFence);
    }
  }

  vk::Fence GetVkFence() const
  {
    return mFence;
  }

  Vulkan::Graphics& mGraphics;
  vk::Fence mFence;
};

/**
 * Class: Fence
 *
 */
Handle<Fence> Fence::New( Graphics& graphics )
{
  auto retval = Handle<Fence>( new Fence(graphics) );
  if( vk::Result::eSuccess == retval->mImpl->Initialise() )
  {
    return retval;
  }
  return Handle<Fence>();
}

//Init value is signaled.
Handle<Fence> Fence::New( Graphics& graphics, bool isSignaled )
{
  auto retval = Handle<Fence>( new Fence(graphics) );

  //
  if( vk::Result::eSuccess == retval->mImpl->Initialise(isSignaled) )
  {
    return retval;
  }
  return Handle<Fence>();
}

Fence::Fence(Graphics& graphics)
{
  mImpl = MakeUnique<Impl>(graphics);
}

const Fence& Fence::ConstRef() const
{
  return *this;
}

Fence& Fence::Ref()
{
  return *this;
}

Fence::~Fence() = default;

bool Fence::Wait(uint32_t timeout)
{
  return mImpl->Wait( timeout );
}

void Fence::Reset()
{
  mImpl->Reset();
}

vk::Fence Fence::GetFence() const
{
  return mImpl->GetVkFence();
}



} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
