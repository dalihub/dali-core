#ifndef DALI_INTERNAL_UNIFORM_BUFFER_H
#define DALI_INTERNAL_UNIFORM_BUFFER_H

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
class UniformBufferV2
{
public:
  /**
   * factory constructor method.
   * @param[in] mController Pointer to the graphics controller
   * @param[in] emulated True if this buffer is for standalone uniforms,
   *            false if for uniform blocks
   * @param[in] alignment The block alignment to use
   */
  static Graphics::UniquePtr<UniformBufferV2> New(Dali::Graphics::Controller* mController, bool emulated, uint32_t alignment);

  /**
   * Create the memory backing this buffer and map it.
   */
  void ReSpecify(uint32_t sizeInBytes);

  void Write(const void* data, uint32_t size, uint32_t offset);

  void Map();

  void Unmap();

  void Flush(); // We aren't guaranteeing creation with this call.

  void Rollback();

  /**
   * Increase offset by value.
   * Note that new offset will meet alignment requirements
   */
  uint32_t IncrementOffsetBy(uint32_t value);

  bool MemoryCompare(void* data, uint32_t offset, uint32_t size);

  [[nodiscard]] uint32_t GetBlockAlignment() const;

  [[nodiscard]] uint32_t GetCurrentOffset() const;

  [[nodiscard]] uint32_t GetCurrentCapacity() const;

  [[nodiscard]] Dali::Graphics::Buffer* GetGraphicsBuffer() const;

private:
  UniformBufferV2(Dali::Graphics::Controller* controller, bool emulated, uint32_t alignment);

  void ReSpecifyCPU(uint32_t sizeInBytes);

  void ReSpecifyGPU(uint32_t sizeInBytes);

  void WriteCPU(const void* data, uint32_t size, uint32_t offset);

  void WriteGPU(const void* data, uint32_t size, uint32_t offset);

  void MapCPU();

  void MapGPU();

  void UnmapCPU();

  void UnmapGPU();

private:
  Graphics::Controller* mController{nullptr};

  const uint32_t mBlockAlignment;

  struct GfxBuffer
  {
    Graphics::UniquePtr<Graphics::Buffer> graphicsBuffer{nullptr};
    Graphics::UniquePtr<Graphics::Memory> graphicsMemory{nullptr};
    uint32_t                              capacity{0};
    uint32_t                              currentOffset{0};
  };

  // List of buffers, in case of CPU one buffer will be sufficient
  std::vector<GfxBuffer> mBufferList;
  void*                  mMappedPtr{nullptr};
  uint32_t               mCurrentGraphicsBufferIndex{0u};
  bool                   mEmulated{false};
};
} // namespace Dali::Internal::Render
#endif //DALI_INTERNAL_UNIFORM_BUFFER_H
