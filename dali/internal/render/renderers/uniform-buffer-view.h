#ifndef DALI_INTERNAL_UNIFORM_BUFFER_VIEW_H
#define DALI_INTERNAL_UNIFORM_BUFFER_VIEW_H

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
 *
 */

// EXTERNAL INCLUDES
#include <stdint.h>
#include <stdlib.h>
#include <memory>

namespace Dali
{
namespace Graphics
{
class Buffer;
}
namespace Internal::Render
{
class UniformBufferV2;

/**
 * Class UniformBufferView
 *
 * The class is responsible for providing view into the UniformBuffer object
 * giving access to part or all of its memory. UniformBufferView doesn't own
 * memory.
 *
 * The memory accessed through the UniformBufferView should be addressed at
 * the offset of 0 and up to specified size. The memory beyond the view may be
 * invalid.
 *
 */
class UniformBufferView
{
public:
  /**
   * Construct a new UniformBufferView.
   * @param[in] ubo The buffer for this view
   * @param[in] offset The offset of this view from ubo
   * @return A new UniformBufferView
   */
  static UniformBufferView* New(UniformBufferV2& ubo, uint32_t offset);

  /**
   * Clear memory pool of UniformBufferView.
   * This should be called at the begin of Core.
   * (Since Core could be recreated, we need to reset the memory pool.)
   * After this API call, all UniformBufferView classes are invalid.
   */
  static void ResetMemoryPool();

  ~UniformBufferView();

  /**
   * Overriden delete operator
   * Deletes the UniformBufferView from its global memory pool
   */
  void operator delete(void* ptr);

  /**
   * @brief Writes data into the current uniform buffer view.
   * @note We prefer to call UniformBuffer::ReadyToLockUniformBuffer before call Write API.
   * And also, prefer to call UniformBuffer::UnlockUniformBuffer if current frame's all Write API action done.
   *
   * @param[in] data pointer to the source data
   * @param[in] size size of source data
   * @param[in] offset destination offset from the offset of this view
   */
  void Write(const void* data, uint32_t size, uint32_t offset);

  /**
   * @brief Returns the offset within the UBO
   * @return Offset
   */
  [[nodiscard]] uint32_t GetOffset() const
  {
    return mOffset;
  }

  /**
   * @brief Returns Graphics buffer associated with this View
   *
   * @return Pointer to a valid Graphics::Buffer object
   */
  [[nodiscard]] Graphics::Buffer* GetBuffer() const;

protected:
  /**
   * Protected constructor. See New()
   */
  UniformBufferView(UniformBufferV2& ubo, uint32_t offset);

private:
  UniformBufferV2* mUniformBuffer{nullptr}; ///< UniformBuffer that the view views
  uint32_t         mOffset{0u};             ///< Offset within the buffer
};
} // Namespace Internal::Render
} // Namespace Dali
#endif //DALI_INTERNAL_UNIFORM_BUFFER_VIEW_H
