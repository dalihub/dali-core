#ifndef DALI_GRAPHICS_VULKAN_TYPES
#define DALI_GRAPHICS_VULKAN_TYPES

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

// EXTERNAL INCLUDES
#include <atomic>
#include <unordered_map>
#include <memory>

#include <dali/integration-api/graphics/vulkan/vulkan-hpp-wrapper.h>

namespace Dali
{
namespace Graphics
{

template< typename T, typename... Args >
std::unique_ptr< T > MakeUnique(Args&&... args)
{
  return std::unique_ptr< T  >(new T(std::forward< Args >(args)...));
}

namespace Vulkan
{

/**
 * Forward class declarations
 */
class Graphics;
class Surface;
class Queue;

/**
 * Unique pointers to Vulkan types
 */
using UniqueQueue         = std::unique_ptr< Queue >;

/**
 * Reference wrappers
 */
using QueueRef         = std::reference_wrapper< Queue >;

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

/**
 * Vulkan object handle
 * @tparam T
 */
template<class T>
class Handle
{
public:

  Handle();
  explicit Handle(T* object );
  Handle( const Handle& handle);
  Handle& operator=( const Handle& handle );
  Handle& operator=( Handle&& handle );
  Handle( Handle&& handle ) noexcept;
  ~Handle();

  operator bool() const;

  T* operator->() const
  {
    return mObject;
  }

  uint32_t GetRefCount() const
  {
    return mObject->GetRefCount();
  }

  T& operator*() const
  {
    return *mObject;
  }

  template <class K>
  Handle<K> StaticCast()
  {
    return Handle<K>(static_cast<K*>(mObject));
  }

  template<class K>
  bool operator==( const Handle<K>& object ) const
  {
    return mObject == &*object;
  }

  template <class K>
  Handle<K> DynamicCast();

  void Reset()
  {
    if( mObject )
    {
      mObject->Release();
      mObject = nullptr;
    }
  }

private:

  T* mObject { nullptr };
};

template <class K, class T>
static Handle<K> VkTypeCast( const Handle<T>& inval )
{
  return Handle<K>(static_cast<K*>(&*inval));
}

template<class T>
Handle<T>::Handle(T* object)
  : mObject( object )
{
  if(mObject)
  {
    mObject->Retain();
  }
}

template<class T>
Handle<T>::Handle()
  : mObject( nullptr )
{
}

template<class T>
Handle<T>::Handle(const Handle& handle)
{
  mObject = handle.mObject;
  if(mObject)
  {
    mObject->Retain();
  }
}

template<class T>
Handle<T>::Handle( Handle&& handle ) noexcept
{
  mObject = handle.mObject;
  handle.mObject = nullptr;
}

template<class T>
Handle<T>::operator bool() const
{
  return mObject != nullptr;
}

template<class T>
Handle<T>& Handle<T>::operator=( Handle&& handle )
{
  mObject = handle.mObject;
  handle.mObject = nullptr;
  return *this;
}

template<class T>
Handle<T>& Handle<T>::operator=( const Handle<T>& handle )
{
  mObject = handle.mObject;
  if(mObject)
  {
    mObject->Retain();
  }
  return *this;
}

template<class T>
Handle<T>::~Handle()
{
  if(mObject)
  {
    mObject->Release();
  }
}

template<class T>
template<class K>
Handle<K> Handle<T>::DynamicCast()
{
  auto val = dynamic_cast<K*>(mObject);
  if(val)
  {
    return Handle<K>(val);
  }
  return Handle<K>();
}

template< typename T, typename... Args >
Handle< T > MakeRef(Args&&... args)
{
  return Handle< T >(new T(std::forward< Args >(args)...));
}

template< typename T, typename... Args >
Handle< T > NewRef(Args&&... args)
{
  return Handle< T >(T::New(std::forward< Args >(args)...));
}


template<class T>
typename T::Impl& GetImpl( Handle<T>& object )
{
  return static_cast<typename T::Impl&>(*object->mImpl);
}

class VkManaged
{
public:

  VkManaged() = default;
  virtual ~VkManaged() = default;

  void Release()
  {
    OnRelease(--mRefCount);
    if(mRefCount == 0)
    {
      // orphaned
      if(!Destroy())
      {
        delete this;
      }
    }
  }

  void Retain()
  {
    OnRetain(++mRefCount);
  }

  uint32_t GetRefCount()
  {
    return mRefCount;
  }

  bool Destroy()
  {
    return OnDestroy();
  }

  virtual void OnRetain( uint32_t refcount ) {};

  virtual void OnRelease( uint32_t refcount ) {};

  virtual bool OnDestroy() { return false; };

private:

  std::atomic_uint mRefCount { 0u };
};

using FBID = int32_t;

#define NotImplemented() \
{\
printf("Function %s isn't implemented!\n", __FUNCTION__);\
assert( "Function no implemented" );\
}

/*
 * Forward declarations of reference types
 */
using ShaderRef = Handle<class Shader>;
using PipelineRef = Handle<class Pipeline>;
using FenceRef = Handle<class Fence>;
using BufferRef = Handle<class Buffer>;
using FramebufferRef = Handle<class Framebuffer>;
using ImageRef = Handle<class Image>;
using ImageViewRef = Handle<class ImageView>;
using DescriptorPoolRef = Handle<class DescriptorPool>;
using CommandPoolRef = Handle<class CommandPool>;
using CommandBufferRef = Handle<class CommandBuffer>;
using GpuMemoryBlockRef = Handle<class GpuMemoryBlock>;
using DescriptorSetRef = Handle<class DescriptorSet>;
using SwapchainRef = Handle<class Swapchain>;
using SurfaceRef = Handle<class Surface>;
using SamplerRef = Handle<class Sampler>;
/*
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
#pragma GCC diagnostic pop
*/
} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_TYPES
