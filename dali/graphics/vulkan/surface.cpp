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
#include <dali/graphics/vulkan/surface.h>
#include <dali/graphics/vulkan/command-buffer.h>
#include <dali/graphics/vulkan/command-pool.h>
#include <dali/graphics/vulkan/fence.h>
#include <dali/graphics/vulkan/graphics.h>
#include <dali/graphics/vulkan/queue.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

SwapchainImage::SwapchainImage()  = default;
SwapchainImage::~SwapchainImage() = default;

Surface::Surface(Graphics& graphics, vk::SurfaceKHR surface, uint32_t bufferCount,
                 bool hasDepthStencil)
: mGraphics(graphics),
  mSurface(surface),
  mSwapchain{nullptr},
  mBufferCount{bufferCount},
  mHasDepthStencil(hasDepthStencil)
{
}

Surface::~Surface()
{
  if(mSwapchain)
  {
    DestroySwapchain();
  }
  if(mSurface)
  {
    mGraphics.GetInstance().destroySurfaceKHR(mSurface, mGraphics.GetAllocator());
  }
}

void Surface::AcquireNextImage()
{
  // if swapchain hasn't been created yet, create it
  // TODO: deferring may bring a lag when surface is swapped very first time
  // it might be good to have an option to create swapchain regardless further usage
  if(!mSwapchain)
  {
    CreateSwapchain();
  }

  if(!mFrameFence)
  {
    mFrameFence = MakeUnique< Fence >(mGraphics);
  }

  // acquire image, for simplicity using fence for acquiring as it is unknown what next command buffer will
  // be executed yet
  mFrameFence->Reset();
  uint32_t index = VkAssert(mGraphics.GetDevice().acquireNextImageKHR(mSwapchain, 1000000, nullptr,
                                                                      mFrameFence->GetFence()));
  mFrameFence->Wait();
  mFrameFence->Reset();

  mCurrentBufferIndex = index;

  auto& swapImage = mSwapImages[index];

  // change layout if necessary to color attachment
  if(swapImage.layout != vk::ImageLayout::eColorAttachmentOptimal)
  {
    auto& queue = mGraphics.GetGraphicsQueue();
    queue.Submit(*swapImage.layoutToColorCmdBuf.get(), *mFrameFence.get())->WaitForFence();
  }

  mFrameFence->Reset();

  // todo: anything to be done before beginning main command buffer?
  BeginRenderPass();
}

void Surface::BeginRenderPass()
{
  auto& swapImage = mSwapImages[mCurrentBufferIndex];

  // begin command buffer ( can be directly obtained through Graphics( surface )
  swapImage.mainCmdBuf->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  /*
   * todo: automatically start main render pass -> this may have to be done manually in future
   * if more flexibility is needed
   */
  auto vkCmdBuf = swapImage.mainCmdBuf->Get();
  {
    std::array< vk::ClearValue, 2 > clearValues;

    static float r = 0.0f;
    r += 0.01f;
    if(r > 1.0f)
      r -= 1.0f;

    clearValues[0].color.setFloat32({r, 0.0f, 0.0f, 1.0f});
    clearValues[1].depthStencil.setDepth(1.0f).setStencil(0.0f);

    auto rpInfo = vk::RenderPassBeginInfo{}
                      .setRenderPass(mDefaultRenderPass)
                      .setFramebuffer(swapImage.framebuffer)
                      .setRenderArea(vk::Rect2D{}.setOffset({0, 0}).setExtent(mExtent))
                      .setClearValueCount(mHasDepthStencil ? 2 : 1)
                      .setPClearValues(clearValues.data());

    auto subpassContents = vk::SubpassContents{vk::SubpassContents::eInline};
    vkCmdBuf.beginRenderPass(rpInfo, subpassContents);
  }
}

void Surface::EndRenderPass()
{
  // todo: use semaphores and do not create fences all over again
  auto& swapImage = mSwapImages[mCurrentBufferIndex];
  auto  vkCmdBuf  = swapImage.mainCmdBuf->Get();

  // complete render pass
  vkCmdBuf.endRenderPass();

  // finalize command buffer
  swapImage.mainCmdBuf->End();

  // submit
  auto& queue = mGraphics.GetGraphicsQueue();
  queue.Submit(*swapImage.mainCmdBuf.get(), *mFrameFence.get())->WaitForFence();
}

void Surface::Present()
{
  // complete render pass and command buffer
  EndRenderPass();

  auto& swapImage = mSwapImages[mCurrentBufferIndex];
  /*
  auto  cmdBuf    = mCommandPool->AllocateCommandBuffer(vk::CommandBufferLevel::ePrimary);
  cmdBuf->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  cmdBuf->ImageLayoutTransition(swapImage.image, vk::ImageLayout::eColorAttachmentOptimal,
                                vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor);
  cmdBuf->End();
  mFrameFence->Reset();
  mGraphics.GetGraphicsQueue().Submit( *cmdBuf.get(), *mFrameFence.get() )->WaitForFence();
  */
  auto result = mGraphics.GetGraphicsQueue().Present(mSwapchain, mCurrentBufferIndex);

  swapImage.layout = vk::ImageLayout::ePresentSrcKHR;
  // todo: test result against swapchain expiration
}

void Surface::CreateSwapchain()
{
  auto formats = VkAssert(mGraphics.GetPhysicalDevice().getSurfaceFormatsKHR(mSurface));
  // find first which is not UNDEFINED
  mFormat = vk::Format::eUndefined;
  for(auto& format : formats)
  {
    if(format.format != vk::Format::eUndefined && mFormat == vk::Format::eUndefined)
    {
      mFormat     = format.format;
      mColorSpace = format.colorSpace;
    }
  }

  assert(mFormat != vk::Format::eUndefined && "No supported surface format!");

  mCapabilities.reset(new vk::SurfaceCapabilitiesKHR(
      VkAssert(mGraphics.GetPhysicalDevice().getSurfaceCapabilitiesKHR(mSurface))));

  mExtent = mCapabilities->currentExtent;

  vk::SwapchainCreateInfoKHR info;
  info.setClipped(true)
      .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
      .setImageArrayLayers(1)
      .setImageColorSpace(mColorSpace)
      .setImageExtent(mCapabilities->currentExtent)
      .setImageFormat(mFormat)
      .setImageSharingMode(vk::SharingMode::eExclusive)
      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
      .setMinImageCount(mBufferCount)
      .setPresentMode(vk::PresentModeKHR::eFifo)
      .setPreTransform(mCapabilities->currentTransform)
      .setOldSwapchain(nullptr)
      .setPQueueFamilyIndices(nullptr)
      .setQueueFamilyIndexCount(0)
      .setSurface(mSurface);

  mSwapchain = VkAssert(mGraphics.GetDevice().createSwapchainKHR(info, mGraphics.GetAllocator()));

  // initialise default render pass
  InitialiseRenderPass();

  // if successful continue with obtaining images etc. also each swapchain will obtain default renderpass
  InitialiseSwapchain();

  // prerecord command buffers per each image in order to provide layout transition
  CreateCommandBuffers();
}

void Surface::DestroySwapchain()
{
  mGraphics.GetDevice().destroySwapchainKHR(mSwapchain, mGraphics.GetAllocator());
}

void Surface::InitialiseSwapchain()
{
  const auto& device    = mGraphics.GetDevice();
  const auto& allocator = mGraphics.GetAllocator();

  auto images = VkAssert(device.getSwapchainImagesKHR(mSwapchain));
  assert(mBufferCount == images.size() && "Swapchain images count not equal requested value!");

  {
    auto swapImages = std::vector< SwapchainImage >{};

    // for each image create framebuffer and image view
    for(auto& image : images)
    {
      AddSwapchainImage(image, swapImages);
    }

    mSwapImages = std::move(swapImages);
  }

  if(mHasDepthStencil)
  {
    CreateDepthStencil();
  }
}

void Surface::AddSwapchainImage(vk::Image image, std::vector< SwapchainImage >& swapchainImages)
{
  const auto& device    = mGraphics.GetDevice();
  const auto& allocator = mGraphics.GetAllocator();

  auto swapImage  = std::move(SwapchainImage{});
  swapImage.image = image;

  // create ImageView
  {
    // Simple image view 2D as a color attachment
    auto ivInfo = vk::ImageViewCreateInfo{}
                      .setFormat(mFormat)
                      .setComponents(vk::ComponentMapping()
                                         .setR(vk::ComponentSwizzle::eR)
                                         .setG(vk::ComponentSwizzle::eG)
                                         .setB(vk::ComponentSwizzle::eB)
                                         .setA(vk::ComponentSwizzle::eA))
                      .setImage(image)
                      .setSubresourceRange(vk::ImageSubresourceRange()
                                               .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                               .setBaseArrayLayer(0)
                                               .setBaseMipLevel(0)
                                               .setLayerCount(1)
                                               .setLevelCount(1))
                      .setViewType(vk::ImageViewType::e2D);

    swapImage.imageView = VkAssert(device.createImageView(ivInfo, allocator));
  }

  // Create framebuffer ( there must be already render pass and information whether
  // we use depth or not )
  {
    vk::FramebufferCreateInfo fbInfo;
    fbInfo.setAttachmentCount(mHasDepthStencil ? 2 : 1)
        .setPAttachments(&swapImage.imageView) // todo: add depth/stencil attachment
        .setHeight(mExtent.height)
        .setWidth(mExtent.width)
        .setLayers(1)
        .setRenderPass(mDefaultRenderPass);

    swapImage.framebuffer = VkAssert(device.createFramebuffer(fbInfo, allocator));
  }

  // initialise semaphores
  {
    swapImage.acqSem     = VkAssert(device.createSemaphore(vk::SemaphoreCreateInfo(), allocator));
    swapImage.presentSem = VkAssert(device.createSemaphore(vk::SemaphoreCreateInfo(), allocator));
  }

  swapImage.layout = vk::ImageLayout::eUndefined;

  swapchainImages.push_back(std::move(swapImage));
}

void Surface::InitialiseRenderPass()
{
  const auto& device    = mGraphics.GetDevice();
  const auto& allocator = mGraphics.GetAllocator();

  vk::AttachmentDescription att[2];

  // color attachment
  att[0]
      .setFormat(mFormat)
      .setLoadOp(vk::AttachmentLoadOp::eClear)
      .setStoreOp(vk::AttachmentStoreOp::eStore)
      .setSamples(vk::SampleCountFlagBits::e1)
      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
      .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

  // optional depth/stencil attachment
  att[1]
      .setFormat(mDepthStencilFormat)
      .setLoadOp(vk::AttachmentLoadOp::eClear)
      .setStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setSamples(vk::SampleCountFlagBits::e1)
      .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
      .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
      .setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
      .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  vk::AttachmentReference attRef[2];
  attRef[0].setLayout(vk::ImageLayout::eColorAttachmentOptimal).setAttachment(0);
  attRef[1].setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal).setAttachment(1);

  // prepare default subpass
  vk::SubpassDescription subpass;
  subpass.setColorAttachmentCount(1)
      .setPColorAttachments(&attRef[0])
      .setPDepthStencilAttachment(mHasDepthStencil ? &attRef[1] : nullptr)
      .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

  vk::RenderPassCreateInfo info;
  info.setPAttachments(att)
      .setAttachmentCount(mHasDepthStencil ? 2 : 1)
      .setPSubpasses(&subpass)
      .setSubpassCount(1);

  mDefaultRenderPass = VkAssert(device.createRenderPass(info, allocator));
}

void Surface::CreateDepthStencil()
{
  const auto& device    = mGraphics.GetDevice();
  const auto& allocator = mGraphics.GetAllocator();
}

void Surface::DestroyDepthStencil()
{
  /// todo
}

void Surface::CreateCommandBuffers()
{
  const auto& device    = mGraphics.GetDevice();
  const auto& allocator = mGraphics.GetAllocator();

  if(!mCommandPool)
  {
    auto info =
        vk::CommandPoolCreateInfo{}.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    info.setQueueFamilyIndex(0); // todo: get correct queue family index ( 0 works by default ;) )
    mCommandPool = MakeUnique< CommandPool >(mGraphics, info);
  }

  // allocate command buffers
  auto cmdBuffers = std::vector< CommandBufferRef >{};
  auto cmdInfo    = vk::CommandBufferAllocateInfo{}.setCommandBufferCount(1).setLevel(
      vk::CommandBufferLevel::ePrimary);

  for(auto& swapImage : mSwapImages)
  {
    swapImage.layoutToColorCmdBuf = mCommandPool->AllocateCommandBuffer(cmdInfo);
    swapImage.mainCmdBuf          = mCommandPool->AllocateCommandBuffer(cmdInfo);

    // Record layout transition for each image, after transition command buffers will be re-recorded
    // and will take in account only present -> color layout transition
    swapImage.layoutToColorCmdBuf->Begin();
    swapImage.layoutToColorCmdBuf->ImageLayoutTransition(swapImage.image,
                                                         swapImage.layout,
                                                         vk::ImageLayout::eColorAttachmentOptimal,
                                                         vk::ImageAspectFlagBits::eColor);
    swapImage.layoutToColorCmdBuf->End();
    swapImage.layout = vk::ImageLayout::eColorAttachmentOptimal;

    cmdBuffers.push_back(std::ref(*swapImage.layoutToColorCmdBuf.get()));
  }

  // submit to the queue
  {
    auto& queue      = mGraphics.GetGraphicsQueue();
    auto  fence      = Fence(mGraphics);
    auto  submission = queue.Submit(cmdBuffers, fence);
    submission->WaitForFence();
  }

  // record present to color transitions for each buffer for further reusing
  for(auto& swapImage : mSwapImages)
  {
    swapImage.layoutToColorCmdBuf->Reset();
    swapImage.layoutToColorCmdBuf->Begin();
    swapImage.layoutToColorCmdBuf->ImageLayoutTransition(swapImage.image,
                                                         vk::ImageLayout::ePresentSrcKHR,
                                                         vk::ImageLayout::eColorAttachmentOptimal,
                                                         vk::ImageAspectFlagBits::eColor);
    swapImage.layoutToColorCmdBuf->End();
    swapImage.layout = vk::ImageLayout::eColorAttachmentOptimal;
  }
}

vk::SurfaceKHR Surface::GetSurfaceKHR() const
{
  return mSurface;
}

vk::RenderPass Surface::GetRenderPass() const
{
  return mDefaultRenderPass;
}

vk::Framebuffer Surface::GetFramebuffer(uint32_t index) const
{
  return mSwapImages[index].framebuffer;
}

vk::ImageView Surface::GetImageView(uint32_t index) const
{
  return mSwapImages[index].imageView;
}

vk::Image Surface::GetImage(uint32_t index) const
{
  return mSwapImages[index].image;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
