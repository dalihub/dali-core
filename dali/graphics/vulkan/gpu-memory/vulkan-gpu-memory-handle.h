#ifndef DALI_VULKAN_161117_GPU_MEMORY_HANDLE_H
#define DALI_VULKAN_161117_GPU_MEMORY_HANDLE_H

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

#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class GpuMemoryAllocator;
const uint32_t INVALID_ALLOCATION { UINT32_MAX };

struct Any {};
class GpuMemoryBlock : public VkManaged
{
public:

  template <class T>
  GpuMemoryBlock( GpuMemoryAllocator& allocator, std::unique_ptr<T> data ) :
  mAllocator(allocator), mAllocationData(reinterpret_cast<Any*>(data.release())){}

  /**
   *
   * @return
   */
  void* Map();

  /**
   * Returns mapped memory to specified type
   * @tparam T
   * @return
   */
  template<class T>
  T* MapTyped()
  {
    return reinterpret_cast<T*>(Map());
  }

  /**
   *
   * @param offset
   * @param size
   * @return
   */
  void* Map( uint32_t offset, uint32_t size );

  /**
   *
   */
  void Unmap();

  /**
   *
   */
  void Flush();

  /**
   *
   * @return
   */
  operator vk::DeviceMemory();

  /**
   *
   * @tparam T
   * @return
   */
  template <class T>
  T* GetData() const
  {
    return reinterpret_cast<T*>(mAllocationData.get());
  }

  /**
   *
   * @return
   */
  GpuMemoryAllocator& GetAllocator() const
  {
    return mAllocator;
  }

public:

  /**
   *
   * @param refcount
   */
  void OnRelease( uint32_t refcount ) override;

  /**
   *
   * @param refcount
   */
  void OnRetain( uint32_t refcount ) override;

  /**
   *
   */
  bool OnDestroy() override;

private:

  GpuMemoryAllocator&         mAllocator;
  std::unique_ptr<Any>        mAllocationData; // may be any POD object pointer, depends on allocator
};

}
}
}

#endif //DALI_VULKAN_161117_GPU_MEMORY_HANDLE_H
