#ifndef DALI_GRAPHICS_VULKAN_FRAME_STACK
#define DALI_GRAPHICS_VULKAN_FRAME_STACK

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

#include <cstdint>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

using Frame = char*;

/**
 * Simple stack frame-based fixed size allocator ( single sided )
 */
class Stack
{
public:

  Stack( uint32_t capacityInBytes )
  {
    mData = new char[capacityInBytes];
    mCurrentPtr = mData;
    mCapacity = capacityInBytes;
    mRecentFrame = nullptr;
  }

  ~Stack()
  {
    delete [] mCurrentPtr;
  }

  Frame Mark()
  {
    mRecentFrame = mCurrentPtr;
    return mCurrentPtr;
  }

  Frame Rollback( Frame frame = nullptr )
  {
    if( !frame )
    {
      if( mRecentFrame )
      {
        mCurrentPtr  = mRecentFrame;
        mRecentFrame = nullptr;
      }
      else
      {
        mCurrentPtr = mData;
      }
    }
    return mCurrentPtr;
  }

  void RollbackAll()
  {
    mCurrentPtr = mData;
  }

  template <typename T>
  T* Alloc( uint32_t count = 1 )
  {
    uintptr_t alignment = alignof(T)-(reinterpret_cast<uintptr_t>(mCurrentPtr)%alignof(T));
    auto result = reinterpret_cast<T*>(mCurrentPtr + alignment);
    if( result >= reinterpret_cast<T*>( mData + mCapacity ) )
    {
      return nullptr;
    }
    mCurrentPtr += sizeof(T) * count;
    return result;
  }

  template <typename T, typename... Args>
  T* AllocNew( Args&&... args )
  {
    T* ptr = Alloc<T>(1);
    if(!ptr)
    {
      return nullptr;
    }
    return new(ptr)T(args...);
  }

  template <typename T, typename... Args>
  T& AllocNewRef( Args&&... args )
  {
    T* ptr = Alloc<T>(1);
    return *(new(ptr)T(args...));
  }

private:

  char* mData { nullptr };
  char* mCurrentPtr { nullptr };
  char* mRecentFrame { nullptr };
  uint32_t mCapacity { 0u };
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali


#endif //DALI_GRAPHICS_VULKAN_FRAME_STACK
