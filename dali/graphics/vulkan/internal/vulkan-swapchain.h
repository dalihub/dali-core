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
class SwapchainBuffer;

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
   * Allocate command buffers for each render pass.
   * @param[in] renderPassCount The number of render passes to allocate for (includes main renderpass)
   */
  void AllocateCommandBuffers( size_t renderPassCount );

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
  RefCountedFramebuffer AcquireNextFramebuffer( bool shouldCollectGarbageNow = true );

  /**
   * Return the primary command buffer associated with the swapchain
   */
  RefCountedCommandBuffer GetLastCommandBuffer() const;

  /**
   * Returns the primary command buffers associated with each render pass
   * being recorded
   * @return mutable vector of command buffers
   */
  std::vector<RefCountedCommandBuffer>& GetCommandBuffers() const;

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

  /**
   * Enables depth/stencil buffer for swapchain ( off by default )
   *
   * @param[in] depthStencilFormat valid depth/stencil pixel format
   */
  void SetDepthStencil( vk::Format depthStencilFormat );

private:

  Swapchain( Graphics& graphics,
             Queue& presentationQueue,
             RefCountedSurface surface,
             std::vector< RefCountedFramebuffer >&& framebuffers,
             vk::SwapchainCreateInfoKHR createInfo,
             vk::SwapchainKHR vkHandle );

private:
  Graphics* mGraphics;
  Queue* mQueue;
  RefCountedSurface mSurface;

  uint32_t mSwapchainImageIndex; ///< Swapchain image index returned by vkAcquireNextImageKHR

  vk::SwapchainKHR mSwapchainKHR;
  vk::SwapchainCreateInfoKHR mSwapchainCreateInfoKHR;

  /*
   * Framebuffer object associated with the buffer
   */
  std::vector<RefCountedFramebuffer> mFramebuffers;

  /**
   * Array of swapchain buffers
   */
  std::vector<std::unique_ptr<SwapchainBuffer>> mSwapchainBuffers;

  RefCountedFence mBetweenRenderPassFence;

  uint32_t mFrameCounter { 0u }; ///< Current frame number
  uint32_t mBufferIndex { 0u }; ///< Current buffer index number

  bool mIsValid; // indicates whether the swapchain is still valid or requires to be recreated
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_VULKAN_SWAPCHAIN_H
