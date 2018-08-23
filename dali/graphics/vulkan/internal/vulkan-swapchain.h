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
#include <dali/graphics/vulkan/internal/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Surface;

class Queue;

/**
   * SwapchainBuffer stores all per-buffer data
   */
struct SwapchainBuffer
{
  /*
   * Each buffer has own master command buffer which executes
   * secondary buffers
   */
  RefCountedCommandBuffer masterCmdBuffer;

  /*
   * Framebuffer object associated with the buffer
   */
  RefCountedFramebuffer framebuffer;

  /*
   * Sync primitives
   */
  RefCountedFence endOfFrameFence;

  /*
   * Buffer index
   */
  uint32_t index;
};

/**
 * Creates swapchain for given surface and queue
 */
class Swapchain : public VkManaged
{

  friend class Graphics;

public:

  ~Swapchain() override;

  Swapchain( const Swapchain& ) = delete;

  Swapchain& operator=( const Swapchain& ) = delete;

  /**
   * Returns current framebuffer ( the one which is rendering to )
   * @return
   */
  RefCountedFramebuffer GetCurrentFramebuffer() const;

  /**
   * Returns any framebuffer from the queue
   * @param index
   * @return
   */
  RefCountedFramebuffer GetFramebuffer( uint32_t index ) const;

  /**
   * This function acquires next framebuffer
   * @todo we should rather use round robin method
   * @return
   */
  RefCountedFramebuffer AcquireNextFramebuffer();

  /**
   * Returns primary command buffer associated with currently
   * being recorded frame
   * @return
   */
  RefCountedCommandBuffer GetCurrentCommandBuffer() const;

  /**
   * Presents using default present queue, asynchronously
   */
  void Present();

  /**
   * Presents using default queue, synchronized with supplied semaphores
   * @param waitSemaphores
   */
  void Present( std::vector< vk::Semaphore > waitSemaphores );

  bool OnDestroy() override;

  vk::SwapchainKHR GetVkHandle();

  /**
   * Returns true when swapchain expired
   * @return
   */
  bool IsValid() const;

  void Invalidate();

private:

  Swapchain( Graphics& graphics,
             Queue& presentationQueue,
             RefCountedSurface surface,
             std::vector< SwapchainBuffer > framebuffers,
             vk::SwapchainCreateInfoKHR createInfo,
             vk::SwapchainKHR vkHandle );

private:
  Graphics* mGraphics;
  Queue* mQueue;
  RefCountedSurface mSurface;

  uint32_t mCurrentBufferIndex;

  RefCountedFence mFrameFence;

  vk::SwapchainKHR mSwapchainKHR;
  vk::SwapchainCreateInfoKHR mSwapchainCreateInfoKHR;

  std::vector< SwapchainBuffer > mSwapchainBuffer;

  struct SyncPrimitives
  {
    SyncPrimitives( Graphics& _graphics );

    ~SyncPrimitives();

    Graphics& graphics;
    /**
     * Semaphore signalled on acquire next image
     */
    vk::Semaphore acquireNextImageSemaphore;

    /**
     * Semaphore signalled on complete commands submission
     */
    vk::Semaphore submitSemaphore;
  };

  std::vector<std::unique_ptr<SyncPrimitives>> mSyncPrimitives;

  bool mIsValid; // indicates whether the swapchain is still valid or requires to be recreated
  uint32_t mFrameCounter { 0u };
  bool mFrameSyncEnabled;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_VULKAN_SWAPCHAIN_H
