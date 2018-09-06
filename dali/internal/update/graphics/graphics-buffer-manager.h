#ifndef DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H
#define DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H

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

#include <dali/graphics-api/graphics-api-controller.h>

#include <memory>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

class GraphicsBuffer
{
  friend class GraphicsBufferManager;
private:

  /**
   * Constructor of GraphicsBuffer
   *
   * @param[in] mController Pointer to the controller object
   * @param[in] sizeInBytes initial size of allocated buffer
   * @param[in] alignment memory alignment in bytes
   * @param[in] persistentMappingEnabled if true, buffer is mapped persistently
   * @param[in] usageFlags type of usage ( Graphics::API::BufferUsage )
   */
  GraphicsBuffer( Dali::Graphics::API::Controller* mController,
                  uint32_t sizeInBytes,
                  uint32_t alignment,
                  bool persistentMappingEnabled,
                  Graphics::API::BufferUsageFlags usageFlags );

public:

  /**
   * Destructor of GraphicsBuffer
   */
  ~GraphicsBuffer();

  /**
   * Writes data into the buffer
   *
   * @param[in] data pointer to the source data
   * @param[in] size size of source data
   * @param[in] offset destination offset
   * @param[in] enables cached write ( write on flush )
   */
  void Write( const void* data, uint32_t size, uint32_t offset, bool cachedWrite );

  /**
   * Flushes whole buffer range
   */
  void Flush();

  /**
   * Returns allocated ( requested ) size
   * @return size of buffer
   */
  uint32_t GetSize() const
  {
    return mSize;
  }

  /**
   * Return Graphics API buffer
   * @return pointer to the buffer object
   */
  const Dali::Graphics::API::Buffer* GetBuffer() const
  {
    return mBuffer.get();
  }

  /**
   * Returns memory alignment
   * @return memory alignment
   */
  uint32_t GetAlignment() const
  {
    return mAlignment;
  }

  /**
   * Reserves buffer memory
   *
   * @param size requested size
   */
  void Reserve( uint32_t size );

  /**
   * Maps buffer memory
   * @return pointer to mapped memory
   */
  void* Map();

  /**
   * Unmaps buffer memory
   */
  void Unmap();

private:

  std::unique_ptr<Dali::Graphics::API::Buffer> mBuffer;
  Dali::Graphics::API::Controller* mController;

  uint32_t  mSize;
  uint32_t  mAlignment;
  void*     mMappedPtr;
  bool      mPersistentMappedEnabled;

  Graphics::API::BufferUsageFlags mUsageFlags;
};

class GraphicsBufferManager
{
public:

  GraphicsBufferManager( Dali::Graphics::API::Controller* controller );

  ~GraphicsBufferManager();

  /**
   * Allocates uniform buffer with given size
   * @param size
   * @return
   */
  std::unique_ptr<GraphicsBuffer> AllocateUniformBuffer( uint32_t size );

private:

  Dali::Graphics::API::Controller* mController;
};



} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H
