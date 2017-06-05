#ifndef DALI_GRAPHICS_VULKAN_TYPES_H
#define DALI_GRAPHICS_VULKAN_TYPES_H

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

// EXTERNAL INCLUDES

// Vulkan
#include <vulkan/vulkan.hpp>

// StdC++
#include <atomic>
#include <unordered_map>

#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
#endif

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 * Forward class declarations
 */
class Graphics;
class CommandBuffer;
class CommandPool;
class Surface;
class Queue;
class Fence;
class DeviceMemory;
class Image;
class ImageView;
class Buffer;

/**
 * Unique pointers to Vulkan types
 */
using UniqueSurface       = std::unique_ptr< Surface >;
using UniqueCommandBuffer = std::unique_ptr< CommandBuffer >;
using UniqueCommandPool   = std::unique_ptr< CommandPool >;
using UniqueQueue         = std::unique_ptr< Queue >;
using UniqueFence         = std::unique_ptr< Fence >;
using UniqueDeviceMemory  = std::unique_ptr< DeviceMemory >;
using UniqueBuffer        = std::unique_ptr< Buffer >;
using UniqueImage         = std::unique_ptr< Image >;
using UniqueImageView     = std::unique_ptr< ImageView >;

/**
 * Reference wrappers
 */
using CommandBufferRef = std::reference_wrapper< CommandBuffer >;
using QueueRef         = std::reference_wrapper< Queue >;
using FenceRef         = std::reference_wrapper< Fence >;
using SurfaceRef       = std::reference_wrapper< Surface >;

template< typename T, typename... Args >
std::unique_ptr< T > MakeUnique(Args&&... args)
{
  return std::unique_ptr< T >(new T(std::forward< Args >(args)...));
}

template< typename T >
T VkAssert(const vk::ResultValue< T >& result, vk::Result expected = vk::Result::eSuccess)
{
  assert(result.result == expected);
  return result.value;
}

inline vk::Result VkAssert(vk::Result result, vk::Result expected = vk::Result::eSuccess)
{
  assert(result == expected);
  return result;
}

inline vk::Result VkTest(vk::Result result, vk::Result expected = vk::Result::eSuccess)
{
  // todo: log if result different than expected?
  return result;
}

template< typename T >
inline uint32_t U32(T value)
{
  return static_cast< uint32_t >(value);
}

class Resource
{
public:
  Resource() : mUserCount{0u} {}
  virtual ~Resource() = default;

  void IncreaseUserCount()
  {
    ++mUserCount;
  }

  void DecreaseUserCount()
  {
    --mUserCount;
  }

  uint32_t GetUserCount() const
  {
    return mUserCount;
  }

private:

  std::atomic<uint32_t> mUserCount;
};

template< typename T>
class ResourceRef
{
public:

  ResourceRef( T& object )
  : mObject( &object )
  {
    mObject->IncreaseUserCount();
  }

  ResourceRef( ResourceRef& object )
  {
    if(mObject)
    {
      mObject->DecreaseUserCount();
    }

    mObject = object.mObject;
    mObject->IncreaseUserCount();
  }

  ResourceRef operator=(ResourceRef& object )
  {
    if(mObject)
    {
      mObject->DecreaseUserCount();
    }

    mObject = object.mObject;
    mObject->IncreaseUserCount();
  }

  ~ResourceRef()
  {
    if(mObject)
    {
      mObject->DecreaseUserCount();
    }
  }

  T& GetResource() const
  {
    return *mObject;
  }

private:

  T* mObject;
};

using FBID = uint32_t;

/*
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
#pragma GCC diagnostic pop
*/
} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_TYPES_H
