#ifndef DALI_VULKAN_INTERNAL_VULKAN_UBO_POOL_H
#define DALI_VULKAN_INTERNAL_VULKAN_UBO_POOL_H

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

#include <cstdint>
#include <memory>
#include <dali/graphics/vulkan/vulkan-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Controller;
}
namespace VulkanAPI
{
using Controller = Vulkan::Controller;
class Ubo;
class UboPool
{
public:

  explicit UboPool(Controller &controller, uint32_t blockSize, uint32_t initialCapacity );

  ~UboPool();

  /**
   * Allocates memory within pool space for new UBO
   * @return unique pointer to the UBO object
   */
  std::unique_ptr<Ubo> Allocate( uint32_t requestedSize );

  /**
   * Releases allocated memory back to the pool
   * @param ubo
   */
  void Release( Ubo& ubo );

  /**
   * Maps UBO buffer memory into the user space
   * @param ubo
   * @return
   */
  void* Map( Ubo& ubo );

  /**
   * Unmaps UBO memory
   */
  void Unmap( Ubo& ubo );

  /**
   * Maps a single page in the pool
   * @param bufferIndex
   * @return
   */
  void* MapPage( uint32_t bufferIndex );


  /**
   * Unmaps a single page in the pool
   * @param bufferIndex
   * @return
   */
  void UnmapPage( uint32_t bufferIndex );

  /**
   * Maps all pages
   * @param bufferIndex
   * @return
   */
  void Map();

  /**
   * Unmaps all pages
   * @param bufferIndex
   * @return
   */
  void Unmap();

  /**
   *
   * @param ubo
   * @return
   */
  bool IsMapped( Ubo& ubo );

  /**
   * Returns Vulkan buffer associated with the UBO
   * @param ubo
   * @return
   */
  Vulkan::BufferRef GetBuffer( Ubo& ubo ) const;

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

/**
 * Ubo is an opaque object
 */
struct Ubo
{
  explicit Ubo( UboPool& pool, struct UboAllocationInfo& allocationInfo );
  ~Ubo();

  /**
   * Writes into UBO object memory. Maps and unmaps buffer. Not very efficient.
   * @param data
   * @param offset
   * @param size
   * @return
   */
  uint32_t Write( const void* data, uint32_t offset, uint32_t size );

  /**
   * Writes into the buffer memory, keeps it mapped
   * @param data
   * @param offset
   * @param size
   * @return
   */
  uint32_t WriteKeepMapped( const void* data, uint32_t offset, uint32_t size );

  /**
   * Returns buffer associated with the UBO
   * @return
   */
  Vulkan::BufferRef GetBuffer() const;

  /**
   * Returns binding offset within Vulkan buffer object
   * @return
   */
  uint32_t GetBindingOffset() const;

  /**
   * Return binding size within Vulkan buffer object
   * @return
   */
  uint32_t GetBindingSize() const;



  struct Impl;
  std::unique_ptr<Impl> mImpl;
  Impl& GetImplementation();
};


}
}
}

#endif //DALI_VULKAN_INTERNAL_VULKAN_UBO_POOL_H
