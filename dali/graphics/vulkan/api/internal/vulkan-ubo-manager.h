#ifndef DALI_VULKAN_API_VULKAN_UBO_MANAGER_H
#define DALI_VULKAN_API_VULKAN_UBO_MANAGER_H

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

#include <memory>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
class Controller;

class Ubo;

class UboManager
{
public:

  /**
   *
   * @param controller
   */
  explicit UboManager( Controller& controller );

  ~UboManager();

  /** Allocates UBO of specified size
   *
   * @param size
   * @return
   */
  std::unique_ptr< Ubo > Allocate( uint32_t size );

  /**
   * Maps all buffers within all pools
   */
  void MapAllBuffers();

  /**
   * Unmaps all buffers
   */
  void UnmapAllBuffers();

private:

  struct Impl;
  std::unique_ptr< Impl > mImpl;
};


}
}
}
#endif //DALI_VULKAN_API_VULKAN_UBO_MANAGER_H