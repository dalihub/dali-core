#ifndef DALI_INTERNAL_UNIFORM_BUFFER_VIEW_POOL_H
#define DALI_INTERNAL_UNIFORM_BUFFER_VIEW_POOL_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>

// EXTERNAL INCLUDES
#include <stdint.h>
#include <stdlib.h>

namespace Dali::Internal::Render
{
class UniformBufferManager;
class UniformBufferView;
class UniformBuffer;

/**
 * Class UniformBufferPoolView
 *
 * The class is responsible for managing the UniformBuffer memory.
 * It is stack-based pool allocator. It doesn't own the UniformBuffer but
 * it's mere view into the memory.
 *
 * The view may however request UniformBuffer to resize if there is a need
 * to allocate memory beyond its current size.
 *
 * The UniformBuffer may be trimmed on Rollback().
 *
 * Rollback() operation moves allocation pointer to the very beginning
 * of the UniformBuffer. The data stored in the UniformBuffer after
 * Rollback() is considered invalid and shouldn't be used by the client side.
 */
class UniformBufferViewPool
{
  friend class UniformBufferManager;

private:
  UniformBufferViewPool(UniformBufferManager& manager, uint32_t alignment);

public:
  ~UniformBufferViewPool();

  /**
   * @brief Rolls back allocation to the beginning of pool
   */
  void Rollback();

  /**
   * @brief Creates view for next free chunk of UBO memory of specified size.
   */
  Graphics::UniquePtr<UniformBufferView> CreateUniformBufferView(size_t size);

  /**
   * @copydoc Dali::Internal::Render::UniformBufferManager::ReadyToLockUniformBuffer
   */
  void ReadyToLockUniformBuffer();

  /**
   * @copydoc Dali::Internal::Render::UniformBufferManager::UnlockUniformBuffer
   */
  void UnlockUniformBuffer();

private:
  UniformBufferManager&              mUboManager;
  Graphics::UniquePtr<UniformBuffer> mUniformBufferStorage;

  uint32_t mAlignment; // 1 for tightly packed emulated UBO
  uint32_t mCurrentOffset;
};

} // namespace Dali::Internal::Render
#endif //DALI_INTERNAL_UNIFORM_BUFFER_VIEW_POOL_H