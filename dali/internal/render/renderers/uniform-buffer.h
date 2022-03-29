#ifndef DALI_INTERNAL_UNIFORM_BUFFER_H
#define DALI_INTERNAL_UNIFORM_BUFFER_H

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

// EXTERNAL INCLUDES
#include <memory>

namespace Dali::Internal::Render
{
/**
 * Class UniformBuffer
 *
 * The class wraps one or more Graphics::Buffer objects and creates
 * a continuous memory to store uniforms. The UniformBuffer may
 * reallocate and merge individual Graphics::Buffer objects into one.
 *
 * From the client side, the UBO memory is continuous and individual
 * Graphics::Buffer objects are not visible.
 */
class UniformBuffer
{
  friend class UniformBufferManager;
  friend class UniformBufferView;
  friend class UniformBufferViewPool;

private:
  /**
   * Constructor of UniformBuffer
   *
   * @param[in] mController Pointer of the graphics controller
   * @param[in] sizeInBytes initial size of allocated buffer
   * @param[in] alignment memory alignment in bytes
   * @param[in] usageFlags type of usage ( Graphics::BufferUsage )
   * @param[in] propertiesFlags buffer properties (Gracphis::BufferPropertiesFlags)
   */
  UniformBuffer(Dali::Graphics::Controller*     mController,
                uint32_t                        sizeInBytes,
                uint32_t                        alignment,
                Graphics::BufferUsageFlags      usageFlags,
                Graphics::BufferPropertiesFlags propertiesFlags);

public:
  /**
   * Destructor of UniformBuffer
   */
  ~UniformBuffer();

  /**
   * @brief Writes data into the buffer
   * @note We prefer to call ReadyToLockUniformBuffer before call Write API.
   * And also, prefer to call UnlockUniformBuffer if current frame's all Write API action done.
   *
   * @param[in] data pointer to the source data
   * @param[in] size size of source data
   * @param[in] offset destination offset
   */
  void Write(const void* data, uint32_t size, uint32_t offset);

  /**
   * @brief Flushes whole buffer range
   *
   * @param[in] bufferIndex Index of Graphics::Buffer
   */
  void Flush(uint32_t bufferIndex = 0);

  /**
   * @brief Returns allocated ( requested ) size
   * @return size of buffer
   */
  [[nodiscard]] uint32_t GetSize() const
  {
    return mSize;
  }

  /**
   * @brief Return Graphics::Buffer object at specified array index
   *
   * @param[in] bufferIndex index of Graphics buffer
   *
   * @return pointer to the buffer object
   */
  [[nodiscard]] Dali::Graphics::Buffer* GetBuffer(uint32_t bufferIndex) const
  {
    return mBuffers[bufferIndex].buffer.get();
  }

  /**
   * @brief Maps individual Graphics buffer memory
   *
   * @param[in] bufferIndex index of Graphics buffer
   */
  void Map(uint32_t bufferIndex = 0);

  /**
   * Unmaps individual Graphics buffer memory
   *
   * @param[in] bufferIndex index of Graphics buffer
   */
  void Unmap(uint32_t bufferIndex = 0);

  /**
   * @brief Resizes the buffer
   *
   * The resize strategy depends on 'invalidate' parameter.
   *
   * If 'invalidate' is true, all the content if the buffer
   * is discarded, the individual Graphics::Buffers are deleted
   * and a single Graphics::Buffer is allocated.
   *
   * If 'invalidate' is false, additional Graphics::Buffer
   * is created and all recorded content is kept unchanged.
   *
   * @param[in] newSize new size of UniformBuffer
   * @param[in] invalidate specifies whether the content should be discarded
   *
   */
  void Resize(uint32_t newSize, bool invalidate);

  /**
   * @copydoc Dali::Internal::Render::UniformBufferViewPool::ReadyToLockUniformBuffer
   */
  void ReadyToLockUniformBuffer();

  /**
   * @copydoc Dali::Internal::Render::UniformBufferViewPool::UnlockUniformBuffer
   */
  void UnlockUniformBuffer();

private:
  /**
   * @brief GfxBuffer wraps single GPU buffer and encapsulates individual
   * buffer mapping and create info details.
   *
   * The array of GfxBuffers makes a single UniformBuffer.
   */
  struct GfxBuffer
  {
    GfxBuffer()            = default;
    ~GfxBuffer()           = default;
    GfxBuffer(GfxBuffer&&) = default;
    GfxBuffer(Graphics::UniquePtr<Graphics::Buffer>&& b, const Graphics::BufferCreateInfo& i)
    : buffer(std::move(b)),
      createInfo(i)
    {
    }

    Graphics::UniquePtr<Graphics::Buffer> buffer{};          ///< Graphics buffer
    Graphics::UniquePtr<Graphics::Memory> memory{};          ///< Mapped memory associated with buffer
    Graphics::BufferCreateInfo            createInfo{};      ///< create info describing the buffer
    void*                                 mappedPtr{};       ///< Mapped pointer (if mapped)
    bool                                  needsUpdate{true}; ///< Indicates whether the buffer needs flushing the queue
  };

  /**
   * @brief Returns GfxBuffer object by offset
   *
   * The memory of UniformBuffer is considered to be continuous, however,
   * it may contain multiple graphics buffers.
   *
   */
  const GfxBuffer* GetBufferByOffset(uint32_t offset, uint32_t* newOffset, uint32_t* bufferIndex) const;

  std::vector<GfxBuffer> mBuffers; ///< List of GfxBuffer objects

  Dali::Graphics::Controller* mController; ///< Pointer to the controller

  uint32_t mSize;         ///< Current size of buffer
  uint32_t mAlignment{0}; ///< Buffer alignment

  Graphics::BufferUsageFlags      mUsageFlags;
  Graphics::BufferPropertiesFlags mPropertiesFlags;

  uint32_t mLockedBufferIndex;   ///< Current locked buffer region index.
  uint8_t* mLockedPtr;           ///< Current locked buffer pointer.
  bool     mReadyToBeLocked : 1; ///< True if current uniform buffer is ready to be locked.
};

} // namespace Dali::Internal::Render

#endif //DALI_INTERNAL_UNIFORM_BUFFER_H
