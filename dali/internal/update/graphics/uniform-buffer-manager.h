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

#include <dali/graphics-api/graphics-api-buffer.h>
#include <dali/graphics-api/graphics-api-buffer-factory.h>

#include <vector>
#include <memory>
#include <bitset>

namespace Dali
{
namespace Graphics
{
namespace API
{
class Controller;
}
}
namespace Internal
{
namespace SceneGraph
{

using BufferPool = std::vector<std::unique_ptr<Graphics::API::Buffer>>;
using UboAllocationHandle = std::bitset<16+32+32>;

/**
 * Describes allocated uniform buffer block
 */
class UniformBuffer
{
  friend class UniformBufferManager;

public:

  UniformBuffer( Graphics::API::Buffer* buffer, uint32_t alignment, uint32_t offset, uint32_t size )
  : mBuffer( buffer ),
    mAlignment( alignment ),
    mOffset( offset ),
    mSize( size ),
    mPoolIndexReserved( 0u ),
    mPoolAllocationIndexReserved( 0u )
  {
  }

  ~UniformBuffer();

  Graphics::API::Buffer* GetBuffer() const
  {
    return mBuffer;
  }

  uint32_t GetAlignment() const
  {
    return mAlignment;
  }

  uint32_t GetOffset() const
  {
    return mOffset;
  }

  uint32_t GetSize() const
  {
    return mSize;
  }

private:

  Graphics::API::Buffer* mBuffer;
  uint32_t               mAlignment;
  uint32_t               mOffset;
  uint32_t               mSize;

  // reserved, used by the UniformManager
  uint32_t mPoolIndexReserved;
  uint32_t mPoolAllocationIndexReserved;
};

class UniformBufferManager
{
public:

  UniformBufferManager( Graphics::API::Controller& controller );

  ~UniformBufferManager();

  std::unique_ptr<UniformBuffer> Allocate( uint32_t size, uint32_t alignment );

  void Free( UniformBuffer& ubo );

private:

  void AddPool( uint32_t blockSize, uint32_t blockCount, uint32_t blockAlignment );


private:

  Graphics::API::Controller& mGraphicsController;

  class Pool;
  std::vector<std::unique_ptr<Pool>> mPools;
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
#endif // DALI_INTERNAL_GRAPHICS_UNIFORM_BUFFER_MANAGER_H
