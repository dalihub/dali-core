#ifndef DALI_GRAPHICS_VULKAN_BUFFER_H
#define DALI_GRAPHICS_VULKAN_BUFFER_H

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

#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
class GpuMemoryBlock;

class Buffer : public VkManaged
{
public:

  /**
   * Type of the buffer
   */
  enum class Type
  {
    VERTEX,
    INDEX,
    UNIFORM,
    SHADER_STORAGE
  };

  /**
   * Destructor
   */
  ~Buffer() override;

  /**
   * Creates logical buffer without allocating any memory
   * @param graphics
   * @param size
   * @param type
   * @return
   */
  static RefCountedBuffer New( Graphics& graphics, size_t size, Type type );

  /**
   *
   * @param graphics
   * @param info
   * @return
   */
  static RefCountedBuffer New( Graphics& graphics, vk::BufferCreateInfo info );

  /**
   * Returns buffer usage flags
   * @return
   */
  vk::BufferUsageFlags GetUsage() const;

  /**
   * Returns buffer size in bytes
   * @return
   */
  uint32_t GetSize() const;

  /**
   * Returns Vulkan VkBuffer object associated with this buffer
   * @return
   */
  vk::Buffer GetVkHandle() const;

  /**
   * Returns handle to the allocated memory
   * @return
   */
  const RefCountedGpuMemoryBlock& GetMemoryHandle() const;

  /**
   * Binds buffer memory
   * @param handle
   */
  void BindMemory( const RefCountedGpuMemoryBlock& handle );

public:

  /**
   *
   * @return
   */
  bool OnDestroy() override;


private:

  /**
   * Creates new VkBuffer with given specification, it doesn't
   * bind the memory.
   * @param graphics
   * @param createInfo
   */
  Buffer(Graphics& graphics, const vk::BufferCreateInfo& createInfo);

  Buffer( const Buffer& ) = delete;
  Buffer& operator =( const Buffer& ) = delete;

private:

  struct Impl;
  std::unique_ptr<Buffer::Impl> mImpl;
};


}
}
}

#endif //VULKAN_PROJECT_BUFFER_H
