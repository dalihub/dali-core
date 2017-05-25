#ifndef DALI_CORE_GRAPHICS_VULKAN_COMMON_H
#define DALI_CORE_GRAPHICS_VULKAN_COMMON_H

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

#include <vulkan/vulkan.hpp>

#include <cassert>
#include <cinttypes>
#include <memory>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
void VkLog(const char* format, ...);

#define OBJECT_HANDLE(TYPE)\
public:\
  TYPE( VkObject* object = nullptr ) : VkHandle{ object }{}\
  using VkHandle::operator=;


#define VkAssert(var, message)                    \
  {                                               \
    if(var.result != vk::Result::eSuccess)        \
    {                                             \
      Dali::Graphics::Vulkan::VkLog("[Assert] %s", message);              \
      assert(var.result == vk::Result::eSuccess); \
    }                                             \
  }

#define VkAssertCall(call)                                          \
  {                                                                 \
    const auto& _result = call;                                            \
    if(_result != vk::Result::eSuccess)                             \
    {                                                               \
      Dali::Graphics::Vulkan::VkLog("[VkAssertCall] %s:%d: %s", __FILE__, __LINE__, #call); \
      assert(_result == vk::Result::eSuccess);                      \
    }                                                               \
  }

#define VkAssertCallMsg(call, msg)                                           \
  {                                                                          \
    auto _result = call;                                                     \
    if(_result != vk::Result::eSuccess)                                      \
    {                                                                        \
      Dali::Graphics::Vulkan::VkLog("[VkAssertCall] %s:%d: %s, %s", __FILE__, __LINE__, #call, msg); \
      assert(_result == vk::Result::eSuccess);                               \
    }                                                                        \
  }

inline vk::Result VkTestCallFn(vk::Result result, const char* file, const char* call, int line)
{
  if(result != vk::Result::eSuccess)
  {
    Dali::Graphics::Vulkan::VkLog("[VkTestCall] %s:%d: %s -> Result = %d", file, line, call, static_cast< int >(result));
  }
  return result;
}

#define VkTestCall(call) VkTestCallFn(call, __FILE__, #call, __LINE__)
#define VkTestBool(call) (vk::Result::eSuccess == VkTestCallFn(call, __FILE__, #call, __LINE__))
}

// helper template to create relation between flags and bits
template< typename BitType, typename FlagType = uint32_t >
struct BitFlags
{
  BitFlags() = default;

  BitFlags(FlagType f) : flags(f)
  {
  }

  BitFlags(const BitType& bit)
  {
    flags = static_cast< FlagType >(bit);
  }

  operator FlagType()
  {
    return flags;
  }

  bool operator==(FlagType srcFlags) const
  {
    return srcFlags == flags;
  }

  bool operator!=(const FlagType& srcFlags) const
  {
    return srcFlags != flags;
  }

  BitFlags operator|(const BitType& bit)
  {
    return BitFlags(flags | static_cast< FlagType >(bit));
  }

  BitFlags& operator|=(const BitType& bit)
  {
    flags |= static_cast< FlagType >(bit);
    return *this;
  }

  BitFlags operator&(const BitFlags& rhs) const
  {
    return FlagType{(rhs.flags & flags)};
  }

  BitFlags operator&(unsigned int rhs) const
  {
    return BitFlags{(rhs & flags)};
  }

  FlagType flags{0};
};

using ExtensionNameList = std::vector< const char* >;
using LayerNameList     = std::vector< const char* >;

// common for adaptor
enum class ValidationLayerBit2
{
  NONE                 = 0,
  SCREENSHOT           = (1 << 0),
  PARAMETER_VALIDATION = (1 << 1),
  VKTRACE              = (1 << 2),
  MONITOR              = (1 << 3),
  SWAPCHAIN            = (1 << 4),
  THREADING            = (1 << 5),
  API_DUMP             = (1 << 6),
  OBJECT_TRACKER       = (1 << 7),
  CORE_VALIDATION      = (1 << 8),
  UNIQUE_OBJECTS       = (1 << 9),
  STANDARD_VALIDATION  = (1 << 10),
  ALL                  = 0xFFFF
};

enum class ValidationChannelBit
{
  NONE                = 0,
  INFORMATION         = (1 << 0),
  WARNING             = (1 << 1),
  PERFORMANCE_WARNING = (1 << 2),
  ERROR               = (1 << 3),
  DEBUG               = (1 << 4),
  ALL                 = 0xFF
};

using ValidationLayerFlags2  = BitFlags< ValidationLayerBit2 >;
using ValidationChannelFlags = BitFlags< ValidationChannelBit >;

enum class PhysicalDeviceBit
{
  // physical device type
  ANY        = 0,
  DISCRETE   = (1 << 0),
  INTEGRATED = (1 << 1)
};

using PhysicalDeviceFlags = BitFlags< PhysicalDeviceBit >;

/// -----------------------------------------------------
// vulkan only some sort of managed objects support
//
class VkObject
{
public:
  VkObject()          = default;
  virtual ~VkObject() = default;

  uint32_t RetainRef()
  {
    return ++mObjectRefCount;
  }

  uint32_t ReleaseRef()
  {
    return --mObjectRefCount;
  }

  uint32_t GetRefCount()
  {
    return mObjectRefCount;
  }

  // handles releasing of vulkan object before being deleted
  // or moved to discard queue
  virtual bool OnSafeDelete()
  {
    return true;
  }

private:
  uint32_t mObjectRefCount{0};
};

template< typename OBJECT >
class VkHandleBase
{
public:
  VkHandleBase() = default;
  VkHandleBase(OBJECT* ptr)
  {
    mObject = ptr;
    if(mObject)
    {
      static_cast< VkObject * >(mObject)->RetainRef();
    }
  }

  VkHandleBase(const VkHandleBase& handle)
  {
    if(handle.mObject)
    {
      static_cast< VkObject * >(handle.mObject)->RetainRef();
    }

    mObject = handle.mObject;
  }

  virtual ~VkHandleBase()
  {
    if(mObject)
    {
      static_cast< VkObject* >(mObject)->ReleaseRef();
      TrySafeDelete();
    }
  }

  VkHandleBase& operator=(OBJECT* ptr)
  {
    if(mObject == ptr)
    {
      return *this;
    }
    if(mObject)
    {
      static_cast< VkObject* >(mObject)->ReleaseRef();
      TrySafeDelete();
    }
    if(ptr)
    {
      static_cast< VkObject * >(ptr)->RetainRef();
    }
    mObject = ptr;
    return *this;
  }

  VkHandleBase& operator=(const VkHandleBase& handle)
  {
    if(handle.mObject == mObject)
    {
      return *this;
    }
    if(mObject)
    {
      static_cast< VkObject* >(mObject)->ReleaseRef();
      TrySafeDelete();
    }
    mObject = handle.mObject;
    if(mObject)
    {
      static_cast< VkObject * >(mObject)->RetainRef();
    }
    return *this;
  }

  OBJECT* GetObject() const
  {
    return mObject;
  }

  operator bool() const
  {
    return mObject != nullptr;
  }

  bool operator!() const
  {
    return mObject == nullptr;
  }

  void Reset()
  {
    if(mObject)
    {
      static_cast< VkObject * >(mObject)->ReleaseRef();
    }
    TrySafeDelete();
    mObject = nullptr;
  }

  bool TrySafeDelete()
  {
    if(!static_cast< VkObject* >(mObject)->GetRefCount())
    {
      if(static_cast< VkObject* >(mObject)->OnSafeDelete())
      {
        delete mObject;
      }
    }
    // todo: deletion didn't go well, should be asserted?
    return false;
  }

  template< typename T >
  T Cast()
  {
    return T(dynamic_cast< OBJECT* >(mObject));
  }

protected:
  OBJECT* mObject{nullptr};
};

using VkHandle = VkHandleBase< VkObject >;

enum class QueueType : uint32_t
{
  GRAPHICS       = 0u,
  COMPUTE        = 1u,
  TRANSFER       = 2u,
  SPARSE_BINDING = 3u,
  PRESENT        = 4u,
  END
};
}
}

#endif //DALI_CORE_GRAPHICS_VULKAN_COMMON_H
