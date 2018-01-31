#ifndef DALI_VULKAN_161117_GPU_MEMORY_MANAGER_H
#define DALI_VULKAN_161117_GPU_MEMORY_MANAGER_H

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
using GpuMemoryAllocatorUID = uint32_t;
class Graphics;
class GpuMemoryAllocator;

class GpuMemoryManager
{
public:

  static std::unique_ptr<GpuMemoryManager> New( Graphics& graphics );

  /**
   *
   */
  ~GpuMemoryManager();

  /**
   *
   * @return
   */
  GpuMemoryAllocator& GetDefaultAllocator() const;

  /**
   *
   * @param allocator
   * @return
   */
  GpuMemoryAllocatorUID RegisterAllocator( std::unique_ptr<GpuMemoryAllocator> allocator );

  /**
   *
   * @param allocatorHandle
   * @return
   */
  bool UnregisterAllocator( GpuMemoryAllocatorUID allocatorHandle );

  /**
   *
   * @return
   */
  Graphics& GetGraphics() const;

private:

  GpuMemoryManager();

  /**
   *
   */
  GpuMemoryManager( Graphics& graphics );

private:

  class Impl;
  std::unique_ptr<Impl> mImpl;
};

}

}

}

#endif //DALI_VULKAN_161117_GPU_MEMORY_MANAGER_H
