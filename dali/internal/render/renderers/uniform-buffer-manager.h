#ifndef DALI_INTERNAL_UNIFORM_BUFFER_MANAGER_H
#define DALI_INTERNAL_UNIFORM_BUFFER_MANAGER_H

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
#include <dali/graphics-api/graphics-controller.h>

namespace Dali::Internal::Render
{
class UniformBuffer;
class UniformBufferView;
class UniformBufferViewPool;

/**
 * Class UniformBufferManager
 *
 * Manages the uniform buffers.
 *
 */
class UniformBufferManager
{
public:
  explicit UniformBufferManager(Dali::Graphics::Controller* controller);

  ~UniformBufferManager();

  /**
   * @brief Allocates uniform buffer with given size and alignment
   * @param size Size of uniform buffer
   * @param alignment Alignment
   * @return new UniformBuffer
   */
  Graphics::UniquePtr<UniformBuffer> AllocateUniformBuffer(uint32_t size, uint32_t alignment = 256);

  /**
   * @brief Creates a view on UniformBuffer
   *
   * @param uniformBuffer
   * @param size
   * @return Uniform buffer view
   */
  Graphics::UniquePtr<UniformBufferView> CreateUniformBufferView(UniformBuffer* uniformBuffer, uint32_t offset, uint32_t size);

  /**
   * @brief Creates uniform buffer pool view
   * @param size
   * @return
   */
  Graphics::UniquePtr<UniformBufferViewPool> CreateUniformBufferViewPool();

  /**
   * @brief Returns Controller object
   * @return controller object
   */
  [[nodiscard]] Graphics::Controller& GetController() const
  {
    return *mController;
  }

  /**
   * @brief Returns embedded uniform buffer pool view for specified DAli buffer index
   * @return Pointer to valid uniform buffer pool view
   */
  [[nodiscard]] UniformBufferViewPool* GetUniformBufferViewPool(uint32_t bufferIndex);

  /**
   * @brief Prepare to lock the uniform buffer so we can write to the standalone uniform map directly.
   * Uniform buffer will be locked at the first call of UniformBuffer::Write after call this API.
   * @note After all write done, We should call UnlockUniformBuffer.
   *
   * @param bufferIndex current update/render buffer index
   */
  void ReadyToLockUniformBuffer(uint32_t bufferIndex);

  /**
   * @brief Unlock the uniform buffer.
   * @note We should call ReadyToLockUniformBuffer before call this.
   *
   * @param bufferIndex current update/render buffer index
   */
  void UnlockUniformBuffer(uint32_t bufferIndex);

private:
  Dali::Graphics::Controller* mController;

  Graphics::UniquePtr<UniformBufferViewPool> mUniformBufferPoolStorage[2u]; ///< The pool view into UniformBuffer (double buffered)
};

} // namespace Dali::Internal::Render

#endif // DALI_INTERNAL_UNIFORM_BUFFER_MANAGER_H
