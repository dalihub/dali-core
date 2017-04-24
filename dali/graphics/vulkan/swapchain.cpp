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

#include <dali/graphics/vulkan/command-buffer.h>
#include <dali/graphics/vulkan/command-pool.h>
#include <dali/graphics/vulkan/command-queue.h>
#include <dali/graphics/vulkan/logical-device.h>
#include <dali/graphics/vulkan/surface.h>
#include <dali/graphics/vulkan/swapchain.h>

#include <dali/graphics/integration/graphics-swapchain-base.h>

#include <vector>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

struct SwapchainFramebuffer
{
  vk::Framebuffer framebuffer{nullptr};

  vk::Image       image{nullptr};
  vk::ImageView   imageView{nullptr};
  vk::ImageLayout layout{vk::ImageLayout::eUndefined};

  CommandBuffer commandBuffer;
  bool          submitted{false};
  vk::Fence     fence{nullptr};
};

using SwapchainBufferArray = std::vector< SwapchainFramebuffer >;

// swapchain implementation
class SwapchainImpl : public Integration::GraphicsSwapchainBase
{
public:
  SwapchainImpl(const LogicalDevice& device, const Vulkan::Surface& surface, int bufferCount,
                DepthStencil depthStencil);
  virtual ~SwapchainImpl() = default;

  /**
   * Presents frame
   * @param queue
   */
  void Present(const CommandQueue& queue);

  /**
   *
   */
  void Present();

  /**
   * Begins new frame
   */
  void BeginFrame();

  /**
   * Replace swapchain
   */
  void Replace();

  /**
   * Creates optional depth/stencil buffer
   */
  void CreateDepthStencil();

  /**
   * Obtains swapchain images
   */
  void GetSwapchainImages();

  /**
   * Creates main render pass used by swapchain
   */
  void CreateMainRenderPass();

  /**
   * Creates main commandbuffers related to the swapchain
   */
  void CreateCommandBuffers();

  /**
   * Creates necessary semaphores
   */
  void CreateSemaphores();

  // swapchain integration
public:
  virtual bool Initialise();

  virtual bool AcquireFrame();

  virtual bool PresentFrame();

  // deprecated
  virtual void SwapBuffers(bool vsync)
  {
  }

private:
  vk::SwapchainKHR mVkSwapchain{nullptr};
  vk::SwapchainKHR mVkOldSwapchain{nullptr};

  // Depth/stencil
  vk::Image        mVkDepthStencilImage{nullptr};
  vk::ImageView    mVkDepthStencilImageView{nullptr};
  vk::DeviceMemory mVkDepthStencilImageMemory{nullptr};
  vk::Format       mDepthStencilFormat{};
  bool             mDepthStencilLayoutChangeNeeded{false}; // this flag indicates whether with new frame it's also necessary to change image layout (done once only)

  // renderpass
  vk::RenderPass mRenderPass{nullptr};

  LogicalDevice mContext;

  Surface mSurface;

  DepthStencil mDepthStencilMode;

  SwapchainBufferArray mBuffers{};

  std::vector< vk::Semaphore > mAcquireSemaphore; // array of semaphores, can't be placed inside the buffer as
                                                  // we don't know the first index
  std::vector< vk::Semaphore > mPresentSemaphore;

  CommandPool   mCommandPool{ nullptr }; // command pool

  uint32_t mCurrentBufferIndex{0};
  uint32_t mAcquiredBufferIndex{-1u};
  uint32_t mPrevBufferIndex{-1u};
  uint32_t mMaxBufferCount{-1u};
};

SwapchainImpl::SwapchainImpl(const LogicalDevice& context, const Vulkan::Surface& surface,
                             int bufferCount, DepthStencil depthStencil)
: GraphicsSwapchainBase(), mContext(context), mSurface{surface}, mMaxBufferCount(bufferCount), mDepthStencilMode(depthStencil)
{
}

bool SwapchainImpl::Initialise()
{
  // create new command pool for new swapchain
  if( !mCommandPool )
  {
    mCommandPool = mContext.CreateCommandPool( QueueType::GRAPHICS, false, false );
  }
  // just replace swapchain
  Replace();
  return true;
}

bool SwapchainImpl::AcquireFrame()
{
  BeginFrame();
  return true;
}

bool SwapchainImpl::PresentFrame()
{
  Present();
  return true;
};

/**
 * Replacing surface makes sure all device operations completed as new surface will bring a
 * new image resources etc.
 */
void SwapchainImpl::Replace()
{

  // wait for device to finish
  if(mVkSwapchain)
  {
    // Todo: make sure no other part of implementation is executing on other threads
    // This situation is very rare so vkDeviceWaitIdle() won't hurt performance
    mContext.GetVkDevice().waitIdle();

    mVkOldSwapchain = mVkSwapchain;
    mVkSwapchain    = nullptr;

    // todo: if size of surface changed the depth may need to be recreated
    // vkDestroyImageView()
    // vkDestroyImage()
  }


  vk::SwapchainCreateInfoKHR info;
  info.setSurface(mSurface.GetSurface())
      .setQueueFamilyIndexCount(0)
      .setPQueueFamilyIndices(nullptr)
      .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
      .setPresentMode(vk::PresentModeKHR::eFifo)
      .setOldSwapchain(mVkOldSwapchain)
      .setMinImageCount(mMaxBufferCount)
      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
      .setImageSharingMode(vk::SharingMode::eExclusive)
      .setImageExtent({( uint32_t )mSurface.GetWidth(), ( uint32_t )mSurface.GetHeight()})
      .setImageFormat(mSurface.GetFormat().format)
      .setImageColorSpace(mSurface.GetFormat().colorSpace)
      .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
      .setImageArrayLayers(1)
      .setClipped(true);

  VkAssertCall(mContext.GetVkDevice().createSwapchainKHR(&info, mContext.GetVkAllocator(), &mVkSwapchain));

  CreateMainRenderPass();

  // create depth stencil buffer
  CreateDepthStencil();

  // extract images ( no need to delete existing ones except of views )
  GetSwapchainImages();

  // Create semaphores ( 2 per buffer );
  CreateSemaphores();

  // create command buffers ( won't do anything if buffers are already allocated )
  // fixme: it might not be a good idea to reuse previous buffers
  CreateCommandBuffers();

  mCurrentBufferIndex = 0;
}

void SwapchainImpl::CreateDepthStencil()
{
  if(mDepthStencilMode == DepthStencil::NONE)
  {
    return;
  }

  // free any existing depth buffer
  if(mVkDepthStencilImageView)
  {
    mContext.GetVkDevice().destroyImageView(mVkDepthStencilImageView, mContext.GetVkAllocator());
  }
  if(mVkDepthStencilImage)
  {
    mContext.GetVkDevice().destroyImage(mVkDepthStencilImage, mContext.GetVkAllocator());
  }
  if(mVkDepthStencilImageMemory)
  {
    mContext.GetVkDevice().freeMemory(mVkDepthStencilImageMemory, mContext.GetVkAllocator());
  }

  // todo: include the proper depth stencil settings
  vk::Format format   = vk::Format::eD16Unorm;
  mDepthStencilFormat = format;

  vk::ImageCreateInfo     imageInfo;
  vk::ImageViewCreateInfo imageViewInfo;

  imageInfo.setPQueueFamilyIndices(nullptr)
      .setQueueFamilyIndexCount(0)
      .setSamples(vk::SampleCountFlagBits::e1)
      .setInitialLayout(vk::ImageLayout::eUndefined)
      .setArrayLayers(1)
      .setExtent({( uint32_t )mSurface.GetWidth(), ( uint32_t )mSurface.GetHeight(), 1})
      .setImageType(vk::ImageType::e2D)
      .setMipLevels(1)
      .setSharingMode(vk::SharingMode::eExclusive)
      .setTiling(vk::ImageTiling::eOptimal)
      .setFormat(format)
      .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

  // create image
  VkAssertCall(mContext.GetVkDevice().createImage(&imageInfo, mContext.GetVkAllocator(), &mVkDepthStencilImage));

  imageViewInfo.setFormat(format)
      .setSubresourceRange(vk::ImageSubresourceRange()
                               .setLayerCount(1)
                               .setBaseArrayLayer(0)
                               .setBaseMipLevel(0)
                               .setLevelCount(1)
                               .setLayerCount(1)
                               .setAspectMask(vk::ImageAspectFlagBits::eDepth))
      .setImage(mVkDepthStencilImage)
      .setViewType(vk::ImageViewType::e2D)
      .setComponents(vk::ComponentMapping()
                         .setR(vk::ComponentSwizzle::eR)
                         .setG(vk::ComponentSwizzle::eG)
                         .setB(vk::ComponentSwizzle::eB)
                         .setA(vk::ComponentSwizzle::eA));

  // create view
  VkAssertCall(mContext.GetVkDevice().createImageView(&imageViewInfo, mContext.GetVkAllocator(),
                                                      &mVkDepthStencilImageView));

  // allocate memory
  mVkDepthStencilImageMemory =
      mContext.AllocateImageMemory(mVkDepthStencilImage, vk::MemoryPropertyFlagBits::eDeviceLocal, true);

  // request changing layout with next frame
  mDepthStencilLayoutChangeNeeded = true;
}

void SwapchainImpl::GetSwapchainImages()
{
  auto device = mContext.GetVkDevice();
  if(!mBuffers.empty())
  {
    // delete only views, images are discarded by swapchain as swapchain bound resource
    // we do not discard existing command buffers, no need when replacing swapchain
    for(auto& buffer : mBuffers)
    {
      device.destroyImageView(buffer.imageView, mContext.GetVkAllocator());
      device.destroyFramebuffer(buffer.framebuffer, mContext.GetVkAllocator());
      buffer.imageView   = nullptr;
      buffer.framebuffer = nullptr;
    }
  }

  // obtain images
  auto images = device.getSwapchainImagesKHR(mVkSwapchain).value;
  assert(images.size() == mMaxBufferCount);

  if(mBuffers.empty())
  {
    mBuffers.resize(images.size());
  }

  vk::ImageViewCreateInfo ivInfo;
  ivInfo
      .setComponents(vk::ComponentMapping()
                         .setR(vk::ComponentSwizzle::eR)
                         .setG(vk::ComponentSwizzle::eG)
                         .setB(vk::ComponentSwizzle::eB)
                         .setA(vk::ComponentSwizzle::eA))
      .setViewType(vk::ImageViewType::e2D)
      .setImage(nullptr)
      .setSubresourceRange(vk::ImageSubresourceRange()
                               .setLayerCount(1)
                               .setBaseArrayLayer(0)
                               .setBaseMipLevel(0)
                               .setLevelCount(1)
                               .setLayerCount(1)
                               .setAspectMask(vk::ImageAspectFlagBits::eColor))
      .setFormat(mSurface.GetFormat().format);

  vk::ImageView attachments[2] = {nullptr, // filled in loop
                                  mVkDepthStencilImageView};

  vk::FramebufferCreateInfo fbInfo;
  fbInfo.setPAttachments(nullptr)
      .setAttachmentCount(mDepthStencilMode != DepthStencil::NONE ? 2 : 1)
      .setRenderPass(mRenderPass)
      .setLayers(1)
      .setWidth(mSurface.GetWidth())
      .setHeight(mSurface.GetHeight());

  // generate views and framebuffers
  int i = 0;
  for(auto& buffer : mBuffers)
  {
    // create image view
    buffer.image = images[i++];
    ivInfo.setImage(buffer.image);
    VkAssertCall(device.createImageView(&ivInfo, mContext.GetVkAllocator(), &buffer.imageView));

    // create framebuffer
    attachments[0] = buffer.imageView;
    fbInfo.setPAttachments(attachments);
    VkAssertCall(device.createFramebuffer(&fbInfo, mContext.GetVkAllocator(), &buffer.framebuffer));

    buffer.layout = vk::ImageLayout::eUndefined;
  }
}

void SwapchainImpl::CreateMainRenderPass()
{
  // Todo: sampling
  // Todo: stencil
  // Todo: separate clear if not needed ( however it's better if render pass does clear framebuffer automatically )
  auto device    = mContext.GetVkDevice();
  auto allocator = mContext.GetVkAllocator();

  uint32_t attCount = mDepthStencilMode != DepthStencil::NONE ? 2 : 1;

  vk::AttachmentReference attRef[2] = {
      vk::AttachmentReference().setLayout(vk::ImageLayout::eColorAttachmentOptimal).setAttachment(0),
      vk::AttachmentReference().setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal).setAttachment(1)};

  vk::SubpassDescription subpassDescription;
  subpassDescription.setPResolveAttachments(nullptr)
      .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
      .setPDepthStencilAttachment(attCount == 2 ? &attRef[1] : nullptr)
      .setPColorAttachments(&attRef[0])
      .setColorAttachmentCount(1);

  vk::AttachmentDescription attDesc[2] = {
      vk::AttachmentDescription()
          .setFormat(mSurface.GetFormat().format)
          .setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setFinalLayout(vk::ImageLayout::ePresentSrcKHR),
      vk::AttachmentDescription()
          .setFormat(mDepthStencilFormat)
          .setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal),
  };

  vk::RenderPassCreateInfo rpInfo;
  rpInfo.setSubpassCount(1).setPSubpasses(&subpassDescription).setAttachmentCount(attCount).setPAttachments(attDesc);

  VkAssertCall(device.createRenderPass(&rpInfo, allocator, &mRenderPass));
}

void SwapchainImpl::CreateCommandBuffers()
{
  auto device = mContext.GetVkDevice();
  for(auto& buffer : mBuffers)
  {
    // create command buffer
    if(!buffer.commandBuffer)
    {
      buffer.commandBuffer = mCommandPool.AllocateCommandBuffer(true);
    }

    // create fence
    if(!buffer.fence)
    {
      vk::FenceCreateInfo info;
      VkAssertCall(device.createFence(&info, mContext.GetVkAllocator(), &buffer.fence));
    }
  }
}

void SwapchainImpl::CreateSemaphores()
{
  auto device = mContext.GetVkDevice();
  if(mAcquireSemaphore.empty())
  {
    mAcquireSemaphore.resize(mMaxBufferCount + 1);
  }
  if(mPresentSemaphore.empty())
  {
    mPresentSemaphore.resize(mMaxBufferCount + 1);
  }

  vk::SemaphoreCreateInfo info;
  for(auto& sem : mAcquireSemaphore)
  {
    if(!sem)
    {
      VkAssertCall(device.createSemaphore(&info, mContext.GetVkAllocator(), &sem));
    }
  }
  for(auto& sem : mPresentSemaphore)
  {
    if(!sem)
    {
      VkAssertCall(device.createSemaphore(&info, mContext.GetVkAllocator(), &sem));
    }
  }
}

void SwapchainImpl::BeginFrame()
{
  auto device    = mContext.GetVkDevice();
  auto allocator = mContext.GetVkAllocator();

  auto semaphore = mAcquireSemaphore[mCurrentBufferIndex];

  // acquire new image
  auto result = VkTestCall(
      device.acquireNextImageKHR(mVkSwapchain, 1000000u, semaphore, nullptr, &mAcquiredBufferIndex));

  // todo: test result and decide if swapchain must be replaced
  // ...
  //

  // start recording command buffer
  auto& swapbuffer = mBuffers[mAcquiredBufferIndex];
  auto& cmdbuf     = swapbuffer.commandBuffer;

  // check fence of selected buffer
  if(swapbuffer.submitted)
  {
    // fixme: ugly busy wait which in most of cases should not happen
    while(device.waitForFences(1, &swapbuffer.fence, false, 1000000) != vk::Result::eSuccess)
    {
    }
    // reset fence
  }
  VkAssertCall(device.resetFences(1, &swapbuffer.fence));
  cmdbuf.Reset();
  cmdbuf.Begin(true);

  // change layouts if necessary
  std::vector< vk::ImageMemoryBarrier > barriers;

  // change depth/stencil attachment layout
  if(mDepthStencilMode != DepthStencil::NONE && mDepthStencilLayoutChangeNeeded)
  {
    vk::ImageMemoryBarrier barrier;
    barrier.setImage(mVkDepthStencilImage)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setLayerCount(1)
                                 .setBaseArrayLayer(0)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1)
                                 .setAspectMask(vk::ImageAspectFlagBits::eDepth))
        .setSrcAccessMask(vk::AccessFlagBits())
        .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    barriers.emplace_back(barrier);
    mDepthStencilLayoutChangeNeeded = false;
  }

  // change color attachment layout
  if(swapbuffer.layout != vk::ImageLayout::eColorAttachmentOptimal)
  {
    vk::ImageMemoryBarrier barrier;

    vk::AccessFlags srcAccess{};
    if(swapbuffer.layout != vk::ImageLayout::eUndefined)
      srcAccess = (vk::AccessFlagBits::eMemoryRead);

    barrier.setImage(swapbuffer.image)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setLayerCount(1)
                                 .setBaseArrayLayer(0)
                                 .setBaseMipLevel(0)
                                 .setLevelCount(1)
                                 .setLayerCount(1)
                                 .setAspectMask(vk::ImageAspectFlagBits::eColor))
        .setSrcAccessMask(srcAccess)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setOldLayout(swapbuffer.layout)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal);
    barriers.emplace_back(barrier);
    swapbuffer.layout = vk::ImageLayout::eColorAttachmentOptimal;
  }

  // record pipeline barrier command
  cmdbuf->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::DependencyFlags(), 0, nullptr, 0, nullptr, barriers.size(), barriers.data());

  // render area ( full window )
  vk::Rect2D area;
  area.setOffset({0, 0}).setExtent({( uint32_t )mSurface.GetWidth(), ( uint32_t )mSurface.GetHeight()});

  static float red = 0.0f;
  red += 0.01f;
  if(red > 1.0f)
    red -= 1.0f;

  // clear values
  const vk::ClearValue clearValues[2] = {vk::ClearValue().color.setFloat32({red, 0.0f, 0.0f, 1.0f}),
                                         vk::ClearValue().depthStencil.setDepth(1.0f).setStencil(0)};

  // begin main render pass
  vk::RenderPassBeginInfo rpInfo;
  rpInfo.setRenderPass(mRenderPass)
      .setRenderArea(area)
      .setPClearValues(clearValues)
      .setClearValueCount(mDepthStencilMode != DepthStencil::NONE ? 2 : 1)
      .setFramebuffer(swapbuffer.framebuffer);

  // begin render pass
  cmdbuf->beginRenderPass(&rpInfo, vk::SubpassContents::eInline);
}

void SwapchainImpl::Present()
{
  // finish recording main command buffer
  auto device    = mContext.GetVkDevice();
  auto allocator = mContext.GetVkAllocator();

  Present(mContext.GetCommandQueue(0, QueueType::PRESENT));
}

void SwapchainImpl::Present(const CommandQueue& queue)
{
  auto& swapbuffer = mBuffers[mAcquiredBufferIndex];
  auto& cmdbuf     = swapbuffer.commandBuffer;
  auto  device     = mContext.GetVkDevice();
  auto  allocator  = mContext.GetVkAllocator();

  cmdbuf->endRenderPass();
  cmdbuf.End();

  vk::PipelineStageFlags waitFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  // submit command buffer
  queue.Submit(&cmdbuf, 1, &mAcquireSemaphore[mCurrentBufferIndex], 1, // wait for acquire sem
               &mPresentSemaphore[mCurrentBufferIndex], 1,             // signal present sem
               &waitFlags, swapbuffer.fence);

  // present
  vk::PresentInfoKHR presentInfo;
  presentInfo.setPImageIndices(&mAcquiredBufferIndex)
      .setPResults(nullptr)
      .setPSwapchains(&mVkSwapchain)
      .setSwapchainCount(1)
      .setWaitSemaphoreCount(1)
      .setPWaitSemaphores(&mPresentSemaphore[mCurrentBufferIndex]);

  VkAssertCall(queue.GetVkQueue().presentKHR(&presentInfo));

  // set swap buffer image layout manually ( will be changed with end of the render pass automatically )
  swapbuffer.layout    = vk::ImageLayout::ePresentSrcKHR;
  swapbuffer.submitted = true;

  // increase "logical" buffer
  mCurrentBufferIndex = (mCurrentBufferIndex + 1) % (mMaxBufferCount + 1);
}

Swapchain Swapchain::New(const LogicalDevice& context, const Vulkan::Surface& surface,
                         int bufferCount, DepthStencil depthStencil)
{
  return Swapchain(new SwapchainImpl(context, surface, bufferCount, depthStencil));
}

} // Vulkan
} // Graphics
} // Dali
