#ifndef DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H
#define DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-controller.h>

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
   * @param[in] usageFlags type of usage ( Graphics::BufferUsage )
   */
  GraphicsBuffer( Dali::Graphics::Controller* mController,
                  uint32_t sizeInBytes,
                  uint32_t alignment,
                  bool persistentMappingEnabled,
                  Graphics::BufferUsageFlags usageFlags );

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
  const Dali::Graphics::Buffer* GetBuffer() const
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
   * @param discardOldBuffer If true discards old buffer without waiting for garbage collection
   */
  void Reserve( uint32_t size, bool discardOldBuffer );

  /**
   * Maps buffer memory
   * @return pointer to mapped memory
   */
  void* Map();

  /**
   * Unmaps buffer memory
   */
  void Unmap();

  /**
   * Fills the buffer with given data ( single 8bit value )
   * @param data char type data
   * @param offset start offset
   * @param size size to write, 0 if whole size
   */
  void Fill( char data, uint32_t offset, uint32_t size );

private:

  std::unique_ptr<Dali::Graphics::Buffer> mBuffer;
  Dali::Graphics::Controller* mController;

  uint32_t  mSize;
  uint32_t  mAlignment;
  void*     mMappedPtr;
  bool      mPersistentMappedEnabled;

  Graphics::BufferUsageFlags mUsageFlags;
};

class GraphicsBufferManager
{
public:

  GraphicsBufferManager( Dali::Graphics::Controller* controller );

  ~GraphicsBufferManager();

  /**
   * Allocates uniform buffer with given size
   * @param size
   * @return
   */
  std::unique_ptr<GraphicsBuffer> AllocateUniformBuffer( uint32_t size );

private:

  Dali::Graphics::Controller* mController;
};



} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H
