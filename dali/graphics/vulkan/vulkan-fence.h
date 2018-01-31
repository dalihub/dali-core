#ifndef DALI_GRAPHICS_VULKAN_FENCE
#define DALI_GRAPHICS_VULKAN_FENCE

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

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Graphics;
class Fence : public VkManaged
{
public:

  static Handle<Fence> New( Graphics& graphics );

  Fence( Fence&& ) = default;
  ~Fence();

private:

  Fence( Graphics& graphics );

public:

  const Fence& ConstRef() const;

  Fence& Ref();

  /**
   *
   * @param timeout
   * @return
   */
  bool Wait( uint32_t timeout = 0u );

  /**
   *
   */
  void Reset();

  vk::Fence GetFence() const;

private:

  class Impl;
  std::unique_ptr<Impl> mImpl;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_FENCE
