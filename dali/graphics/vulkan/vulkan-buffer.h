#ifndef DALI_GRAPHICS_VULKAN_BUFFER_H
#define DALI_GRAPHICS_VULKAN_BUFFER_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
namespace Impl
{
struct Buffer;
}

class DeviceMemory;
class Graphics;

using VertexBuffer = Buffer;
using UniformBuffer = Buffer;
using IndexBuffer = Buffer;

class Buffer
{
public:
  /**
   * Creates new VkBuffer with given specification, it doesn't
   * bind the memory.
   * @param graphics
   * @param createInfo
   */
  Buffer(Graphics& graphics, const vk::BufferCreateInfo& createInfo);

  /**
   * Destructor
   */
  virtual ~Buffer();


  // single purpose constructors helpers
  static std::unique_ptr<VertexBuffer> NewVertexBuffer( Graphics& graphics, size_t size  );

  static std::unique_ptr<UniformBuffer> NewUniformBuffer( Graphics& graphics, size_t size );

  static std::unique_ptr<IndexBuffer> NewIndexBuffer( Graphics& graphics, size_t size );

  vk::BufferUsageFlags GetUsage() const;

  DeviceMemory& GetDeviceMemory() const;

  size_t GetSize() const;

  bool HasDeviceMemory() const;

  vk::Buffer GetVkObject() const;

private:

  std::unique_ptr<Impl::Buffer> mImpl;


};


}
}
}

#endif //VULKAN_PROJECT_BUFFER_H
