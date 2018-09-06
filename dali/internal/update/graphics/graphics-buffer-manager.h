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

  GraphicsBuffer( Dali::Graphics::API::Controller* mController,
  uint32_t sizeInBytes, uint32_t alignment, bool persistentMappingEnabled,
  Graphics::API::BufferUsageFlags usageFlags);

public:

  ~GraphicsBuffer();

  /**
   * Writes data into the buffer
   * @param data
   * @param size
   * @param offset
   */
  void Write( const void* data, uint32_t size, uint32_t offset, bool cachedWrite );

  /**
   * Flushes buffer range
   */
  void Flush();

  /**
   * Returns allocated ( requested ) size
   * @return
   */
  uint32_t GetSize() const
  {
    return mSize;
  }

  const Dali::Graphics::API::Buffer* GetBuffer() const
  {
    return mBuffer.get();
  }

  /**
   * Returns aligned size
   * @return
   */
  uint32_t GetAlignment() const;

  void Reserve( uint32_t size );

  void* Map();

  void Unmap();

private:

  std::unique_ptr<Dali::Graphics::API::Buffer> mBuffer;
  Dali::Graphics::API::Controller* mController;

  uint32_t mSize;
  uint32_t mAlignment;
  void* mMappedPtr;
  bool mPersistentMappedEnabled;
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

public:

  /**
   * Flushes all uniform buffers
   */
  void FlushAll();

private:

  Dali::Graphics::API::Controller* mController;
};



} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H
