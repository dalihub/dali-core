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
#include <bitset>

#include <dali/integration-api/graphics/vulkan/vulkan-hpp-wrapper.h>

namespace Dali
{
namespace Graphics
{

template< typename T, typename... Args >
std::unique_ptr< T > MakeUnique( Args&& ... args )
{
  return std::unique_ptr< T >( new T( std::forward< Args >( args )... ) );
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
T VkAssert( const vk::ResultValue< T >& result, vk::Result expected = vk::Result::eSuccess )
{
  assert( result.result == expected );
  return result.value;
}

inline vk::Result VkAssert( vk::Result result, vk::Result expected = vk::Result::eSuccess )
{
  assert( result == expected );
  return result;
}

inline vk::Result VkTest( vk::Result result, vk::Result expected = vk::Result::eSuccess )
{
  // todo: log if result different than expected?
  return result;
}

template< typename T >
inline uint32_t U32( T value )
{
  return static_cast< uint32_t >(value);
}

template <typename T>
inline int32_t I32( T value )
{
  return static_cast< int32_t >( value );
}

template <typename T>
inline float F32( T value )
{
  return static_cast< float >( value );
}

template <typename T>
inline double F64( T value )
{
  return static_cast< double >( value );
}

/**
 * Vulkan object handle
 * @tparam T
 */
template< class T >
class Handle
{
public:

  Handle();

  explicit Handle( T* object );

  Handle( const Handle& handle );

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

  template< class K >
  Handle< K > StaticCast()
  {
    return Handle< K >( static_cast<K*>(mObject) );
  }

  template< class K >
  bool operator==( const Handle< K >& object ) const
  {
    return mObject == &*object;
  }

  template< class K >
  bool operator!=( const Handle< K >& object ) const
  {
    return !(mObject == &*object);
  }


  template< class K >
  Handle< K > DynamicCast();

  void Reset()
  {
    if( mObject )
    {
      mObject->Release();
      mObject = nullptr;
    }
  }

private:

  T* mObject{ nullptr };
};

template< class K, class T >
static Handle< K > VkTypeCast( const Handle< T >& inval )
{
  return Handle< K >( static_cast<K*>(&*inval) );
}

template< class T >
Handle< T >::Handle( T* object )
        : mObject( object )
{
  if( mObject )
  {
    mObject->Retain();
  }
}

template< class T >
Handle< T >::Handle()
        : mObject( nullptr )
{
}

template< class T >
Handle< T >::Handle( const Handle& handle )
{
  mObject = handle.mObject;
  if( mObject )
  {
    mObject->Retain();
  }
}

template< class T >
Handle< T >::Handle( Handle&& handle ) noexcept
{
  mObject = handle.mObject;
  handle.mObject = nullptr;
}

template< class T >
Handle< T >::operator bool() const
{
  return mObject != nullptr;
}

template< class T >
Handle< T >& Handle< T >::operator=( Handle&& handle )
{
  if( mObject )
  {
    mObject->Release();
  }
  mObject = handle.mObject;
  handle.mObject = nullptr;
  return *this;
}

template< class T >
Handle< T >& Handle< T >::operator=( const Handle< T >& handle )
{
  mObject = handle.mObject;
  if( mObject )
  {
    mObject->Retain();
  }
  return *this;
}

template< class T >
Handle< T >::~Handle()
{
  if( mObject )
  {
    mObject->Release();
  }
}

template< class T >
template< class K >
Handle< K > Handle< T >::DynamicCast()
{
  auto val = dynamic_cast<K*>(mObject);
  if( val )
  {
    return Handle< K >( val );
  }
  return Handle< K >();
}

template< typename T, typename... Args >
Handle< T > MakeRef( Args&& ... args )
{
  return Handle< T >( new T( std::forward< Args >( args )... ) );
}

template< typename T, typename... Args >
Handle< T > NewRef( Args&& ... args )
{
  return Handle< T >( T::New( std::forward< Args >( args )... ) );
}


class VkManaged
{
public:

  VkManaged() = default;

  virtual ~VkManaged() = default;

  void Release()
  {
    OnRelease( --mRefCount );

    if( mRefCount == 0 )
    {
      // orphaned
      if( !Destroy() )
      {
        delete this;
      }
    }
  }

  void Retain()
  {
    OnRetain( ++mRefCount );
  }

  uint32_t GetRefCount()
  {
    return mRefCount;
  }

  virtual bool Destroy()
  {
    return OnDestroy();
  }

  virtual void OnRetain( uint32_t refcount )
  {
  }

  virtual void OnRelease( uint32_t refcount )
  {
  }

  virtual bool OnDestroy()
  {
    return false;
  }

private:

  std::atomic_uint mRefCount{ 0u };
};

using FBID = int32_t;

enum class BufferType
{
  VERTEX,
  INDEX,
  UNIFORM,
  SHADER_STORAGE
};

enum class Platform
{
  UNDEFINED,
  XLIB,
  XCB,
  WAYLAND,
};

struct FormatInfo
{
  bool packed     { false };
  bool compressed { false };
  unsigned int paletteSizeInBits { 0u };
  unsigned int blockSizeInBits   { 0u };
  unsigned int blockWidth        { 0u };
  unsigned int blockHeight       { 0u };
  unsigned int blockDepth        { 0u };
};

enum class DescriptorType : size_t
{
  STORAGE_IMAGE,
  SAMPLER,
  SAMPLED_IMAGE,
  COMBINED_IMAGE_SAMPLER,
  UNIFORM_TEXEL_BUFFER,
  STORAGE_TEXEL_BUFFER,
  UNIFORM_BUFFER,
  STORAGE_BUFFER,
  DYNAMIC_UNIFORM_BUFFER,
  DYNAMIC_STORAGE_BUFFER,
  INPUT_ATTACHMENT,
  DESCRIPTOR_TYPE_COUNT
};

template< size_t valueSizeInBits, typename EnumT, size_t enumVariantCount >
class TypeValueEncoder final
{
private: // Compile time constants
  static constexpr size_t bitsetSize = enumVariantCount + valueSizeInBits * enumVariantCount;

public:

  TypeValueEncoder()
  {
    static_assert( std::is_same< size_t, UnderlyingEnumType >::value, "Enum class underlying type must be size_t." );
  }

  bool Contains( EnumT enumVariant ) const
  {
    return mMask.test( ToIntegral( enumVariant ) );
  }

  uint64_t GetValueFor( EnumT enumVariant ) const
  {
    return ( ( mMask >> ( enumVariantCount +
                          ToIntegral( enumVariant ) * valueSizeInBits ) ) &
             std::bitset< bitsetSize >( std::bitset< valueSizeInBits >().flip().to_ulong() ) ).to_ulong();
  }

  TypeValueEncoder& EncodeValue( size_t count, EnumT type )
  {
    mMask.set( ToIntegral( type ) );
    EncodeCount( count, type );

    return *this;
  }

  std::vector< std::tuple< EnumT, uint64_t > > Decode() const
  {
    auto result = std::vector< std::tuple< EnumT, uint64_t > >{};

    for( auto i = 0u; i < enumVariantCount; ++i)
    {
      if( mMask.test( i ) )
      {
        auto castEnum = static_cast< EnumT >( i );
        auto value = GetValueFor( castEnum );

        result.emplace_back( castEnum, value );
      }
    }

    return result;
  };

private: // Functions and aliases

  using UnderlyingEnumType = typename std::underlying_type< EnumT >::type;

  constexpr UnderlyingEnumType ToIntegral( EnumT descriptorType ) const
  {
    return static_cast< UnderlyingEnumType >( descriptorType );
  }

  void EncodeCount( size_t count, DescriptorType descriptorType )
  {
    assert(count <= std::bitset< valueSizeInBits >().flip().to_ulong()
           && "Cannot encode descriptor count. Maximum possible value exceeded.");

    mMask |= std::bitset< bitsetSize >( count ) << enumVariantCount
                                                   + ToIntegral( descriptorType ) * valueSizeInBits;
  }

  template<size_t valueSizeInBits_, typename EnumT_, size_t enumVariantCount_>
  friend bool operator==(const TypeValueEncoder< valueSizeInBits_, EnumT_, enumVariantCount_ >& lhs,
                         const TypeValueEncoder< valueSizeInBits_, EnumT_, enumVariantCount_ >& rhs);

  template<size_t valueSizeInBits_, typename EnumT_, size_t enumVariantCount_>
  friend bool operator!=(const TypeValueEncoder< valueSizeInBits_, EnumT_, enumVariantCount_ >& lhs,
                         const TypeValueEncoder< valueSizeInBits_, EnumT_, enumVariantCount_ >& rhs);

  template<size_t valueSizeInBits_, typename EnumT_, size_t enumVariantCount_>
  friend std::ostream& operator<<( std::ostream& os,
                                   const TypeValueEncoder< valueSizeInBits_, EnumT_, enumVariantCount_ >& rhs );

private: // Members

  std::bitset< bitsetSize > mMask;
};

template <size_t valueSizeInBits, typename EnumT, size_t enumVariantCount>
bool operator==(const TypeValueEncoder< valueSizeInBits, EnumT, enumVariantCount >& lhs,
                const TypeValueEncoder< valueSizeInBits, EnumT, enumVariantCount >& rhs)
{
  return lhs.mMask == rhs.mMask;
}

template <size_t valueSizeInBits, typename EnumT, size_t enumVariantCount>
bool operator!=(const TypeValueEncoder< valueSizeInBits, EnumT, enumVariantCount >& lhs,
                const TypeValueEncoder< valueSizeInBits, EnumT, enumVariantCount >& rhs)
{
  return lhs.mMask != rhs.mMask;
}

template<size_t valueSizeInBits, typename EnumT, size_t enumVariantCount>
std::ostream& operator<<( std::ostream& os,
                          const TypeValueEncoder< valueSizeInBits, EnumT, enumVariantCount >& rhs )
{
  os << rhs.mMask.to_string();
  return os;
}

using DescriptorSetLayoutSignature =
TypeValueEncoder< 4, DescriptorType, static_cast< size_t >( DescriptorType::DESCRIPTOR_TYPE_COUNT ) >;

/**
 * Helper function converting Vulkan C++ types into void* ( for logging )
 */
template<class T, class K>
const void* VkVoidCast( const K& o )
{
  return static_cast<T>(o);
}

/*
 * Forward declarations of reference types
 */
using RefCountedShader = Handle< class Shader >;
using RefCountedPipeline = Handle< class Pipeline >;
using RefCountedFence = Handle< class Fence >;
using RefCountedBuffer = Handle< class Buffer >;
using RefCountedFramebuffer = Handle< class Framebuffer >;
using RefCountedImage = Handle< class Image >;
using RefCountedImageView = Handle< class ImageView >;
using RefCountedDescriptorPool = Handle< class DescriptorPool >;
using RefCountedCommandPool = Handle< class CommandPool >;
using RefCountedCommandBuffer = Handle< class CommandBuffer >;
using RefCountedGpuMemoryBlock = Handle< class GpuMemoryBlock >;
using RefCountedDescriptorSet = Handle< class DescriptorSet >;
using RefCountedSwapchain = Handle< class Swapchain >;
using RefCountedSurface = Handle< class Surface >;
using RefCountedSampler = Handle< class Sampler >;
using RefCountedTexture = Handle< class Texture >;
using RefCountedFramebufferAttachment = Handle< class FramebufferAttachment >;

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_TYPES
