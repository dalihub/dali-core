#ifndef DALI_GRAPHICS_VULKAN_SURFACE_H
#define DALI_GRAPHICS_VULKAN_SURFACE_H

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

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 * Vulkan surface is coupled with swapchain -> one swapchain per surface
 * Swapchain won't exist until surface is used in a such way
 *
 */
class Graphics;
class CommandBuffer;
class CommandPool;
class Surface;

using UniqueSurface       = std::unique_ptr< Surface >;
using UniqueCommandBuffer = std::unique_ptr< CommandBuffer >;
using UniqueCommandPool   = std::unique_ptr< CommandPool >;

// simple structure describing single image of swapchain
// non-copyable, only movable
struct SwapchainImage
{
  SwapchainImage();
  ~SwapchainImage();
  SwapchainImage(const SwapchainImage&) = delete;
  SwapchainImage(SwapchainImage&&)      = default;
  SwapchainImage& operator=(const SwapchainImage&) = delete;
  SwapchainImage& operator=(SwapchainImage&&) = default;

  UniqueImage     image;
  UniqueImageView imageView;
  vk::Framebuffer framebuffer;
  vk::ImageLayout layout;
  vk::Semaphore   acqSem;
  vk::Semaphore   presentSem;

  // layout transitions, prerecorded command buffers
  Handle<CommandBuffer> layoutToColorCmdBuf;
  Handle<CommandBuffer> mainCmdBuf;
};

class Surface
{
public:
  Surface(Graphics& graphics, vk::SurfaceKHR surface, uint32_t bufferCount = 2,
          bool hasDepthStencil = false);
  ~Surface();

  /**
   * Prepares new swapchain image
   */
  void AcquireNextImage();

  /**
   * Presents image
   */
  void Present();

  /**
   *
   * @return
   */
  vk::RenderPass GetRenderPass() const;

  /**
   *
   * @param index
   * @return
   */
  vk::Framebuffer GetFramebuffer(uint32_t index = -1u) const;

  /**
   *
   * @param index
   * @return
   */
  ImageView& GetImageView(uint32_t index = -1u) const;

  /**
   *
   * @param index
   * @return
   */
  Image& GetImage(uint32_t index = -1u) const;

  /**
   *
   * @return
   */
  vk::SurfaceKHR GetSurfaceKHR() const;

  /**
   * returns set of clear values for this surface
   * @return
   */
  std::vector<vk::ClearValue> GetClearValues() const;

  /**
   * Returns size of surface
   * @return
   */
  vk::Extent2D GetSize() const;

  /**
   * Returns primary command buffer associated with specific buffer
   * @param index
   * @return
   */
  Handle<CommandBuffer> GetCommandBuffer( uint32_t index );

  /**
   * Returns primary command buffer associated with current buffer
   * @return
   */
  Handle<CommandBuffer> GetCurrentCommandBuffer();

/**
   *
   */
  void CreateSwapchain();

private:

  void CreateVulkanSwapchain();

  void CreateImageView(SwapchainImage& swapImage);
  void CreateFramebuffer(SwapchainImage& swapImage);
  void CreateSemaphores(SwapchainImage& swapImage);

  void DestroySwapchain();

  void InitialiseSwapchain();

  void InitialiseRenderPass();

  void AddSwapchainImage(vk::Image image, std::vector< SwapchainImage >& swapchainImages);

  void CreateCommandBuffers();

  void CreateDepthStencil();

  void DestroyDepthStencil();

  void BeginRenderPass();

  void EndRenderPass();

  Graphics&        mGraphics;
  vk::SurfaceKHR   mSurface;
  vk::SwapchainKHR mSwapchain;

  vk::Format       mDepthStencilFormat{vk::Format::eD16UnormS8Uint};
  vk::Image        mDepthStencilImage;
  vk::ImageView    mDepthStencilImageView;
  vk::DeviceMemory mDepthStencilMemory;

  Handle<CommandPool> mCommandPool;

  vk::Format        mFormat;
  vk::ColorSpaceKHR mColorSpace;
  vk::Extent2D      mExtent;

  std::vector< SwapchainImage >                 mSwapImages;
  std::unique_ptr< vk::SurfaceCapabilitiesKHR > mCapabilities;

  UniqueFence mFrameFence;

  vk::RenderPass mDefaultRenderPass;
  uint32_t       mBufferCount;
  uint32_t       mCurrentBufferIndex;
  bool           mHasDepthStencil;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_SURFACE_H
