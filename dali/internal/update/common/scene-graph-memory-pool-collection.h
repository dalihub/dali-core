#ifndef DALI_INTERNAL_SCENEGRAPH_MEMORY_POOL_COLLECTION_H
#define DALI_INTERNAL_SCENEGRAPH_MEMORY_POOL_COLLECTION_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLDUES
#include <cstdint> ///< for uint32_t
#include <memory>  ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-interface.h>

namespace Dali::Internal::SceneGraph
{
/**
 * The container of memory pools that SceneGraph using.
 * It is owned by Core.
 */
class MemoryPoolCollection
{
public:
  MemoryPoolCollection();
  ~MemoryPoolCollection();

  enum class MemoryPoolType
  {
    // UpdateManager
    ANIMATION = 0,
    CAMERA,
    NODE,
    RENDERER,
    RENDER_ITEM,
    RENDER_TASK_LIST,
    TEXTURE_SET,

    // RenderManager
    RENDER_RENDERER,
    RENDER_TEXTURE,
    RENDER_UBO_VIEW,
  };

  void* AllocateRaw(MemoryPoolType type);
  void* AllocateRawThreadSafe(MemoryPoolType type);

  void Free(MemoryPoolType type, void* object);
  void FreeThreadSafe(MemoryPoolType type, void* object);

  void*                        GetPtrFromKey(MemoryPoolType type, MemoryPoolInterface::KeyType key);
  MemoryPoolInterface::KeyType GetKeyFromPtr(MemoryPoolType type, void* ptr);

  uint32_t GetCapacity(MemoryPoolType type) const;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENEGRAPH_MEMORY_POOL_COLLECTION_H
