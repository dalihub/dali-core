#ifndef DALI_INTERNAL_RENDERERS_GPU_BUFFER_H
#define DALI_INTERNAL_RENDERERS_GPU_BUFFER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

namespace Dali:: Internal::SceneGraph
{

/**
 * Used to create and update a GPU memory buffer.
 *
 * The buffer can be used for storing vertex data, index arrays (indices)
 * or pixel data (PBO).
 *
 * The buffer allows data to be stored in high-performance
 * graphics memory on the server side and
 * promotes efficient data transfer.
 */
class GpuBuffer
{
public:
  /**
   * When writing into the buffer, the WritePolicy
   * determines whether the current content would be preserved
   * or discarded.
   *
   * RETAIN - buffer content is retained
   *
   * DISCARD - buffer content is discarded. In this case, writing into
   *           a part of buffer will result with undefined content outside
   *           the specified area. The client should rewrite whole area
   *           in order to have coherent and valid data.
   */
  enum class WritePolicy
  {
    RETAIN, ///< Buffer content is preserved
    DISCARD ///< Buffer content is invalidated and discarded
  };

  /**
   * constructor
   * @param[in] graphicsController the graphics controller
   * @param[in] usage The type of buffer
   * @param[in] writePolicy The buffer data write policy to be used, default is WritePolicy::RETAIN
   */
  GpuBuffer(Graphics::Controller& graphicsController, Graphics::BufferUsageFlags usage, GpuBuffer::WritePolicy writePolicy);

  /**
   * Destructor, non virtual as no virtual methods or inheritance
   */
  ~GpuBuffer() = default;

  /**
   * Creates or updates a buffer object and binds it to the target.
   * @param graphicsController The graphics controller
   * @param size Specifies the size in bytes of the buffer object's new data store.
   * @param data pointer to the data to load
   */
  void UpdateDataBuffer(Graphics::Controller& graphicsController, uint32_t size, const void* data);

  /**
   * Get the size of the buffer
   * @return size Size of the buffer in bytes
   */
  [[nodiscard]] uint32_t GetBufferSize() const
  {
    return mSize;
  }

  [[nodiscard]] inline const Graphics::Buffer* GetGraphicsObject() const
  {
    return mGraphicsObject.get();
  }

  /**
   * Destroy the graphics buffer and reset.
   */
  void Destroy();

private:
  Graphics::UniquePtr<Graphics::Buffer> mGraphicsObject;
  uint32_t                              mCapacity{0}; ///< buffer capacity
  uint32_t                              mSize{0};     ///< buffer size
  Graphics::BufferUsageFlags            mUsage;
  WritePolicy                           mWritePolicy{WritePolicy::RETAIN}; ///< data write policy for the buffer
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_RENDERERS_GPU_BUFFER_H
