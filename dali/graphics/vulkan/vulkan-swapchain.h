#ifndef DALI_GRAPHICS_VULKAN_SWAPCHAIN_H
#define DALI_GRAPHICS_VULKAN_SWAPCHAIN_H

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
class Surface;
class Queue;
class Swapchain : public VkManaged
{
public:

  static SwapchainRef New( Graphics& graphics, Queue& presentationQueue, Surface& surface, uint8_t bufferCount, uint32_t flags );

public:

  Swapchain( const Swapchain& ) = delete;
  Swapchain& operator=( const Swapchain& ) = delete;

  /**
   * Returns current framebuffer ( the one which is rendering to )
   * @return
   */
  FramebufferRef GetCurrentFramebuffer() const;

  /**
   * Returns any framebuffer from the queue
   * @param index
   * @return
   */
  FramebufferRef GetFramebuffer( uint32_t index ) const;

  /**
   * Requests for next framebuffer
   * @return
   */
  FramebufferRef AcquireNextFramebuffer();

  /**
   * Presents swapchain using a given queue
   * @param queue
   */
  void Present( const QueueRef& queue );

  /**
   * Presents using default present queue
   */
  void Present();

private:

  Swapchain();
  Swapchain( Graphics& graphics, Queue& presentationQueue, Surface& surface, uint8_t bufferCount, uint32_t flags );
  ~Swapchain() final;

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_VULKAN_SWAPCHAIN_H
