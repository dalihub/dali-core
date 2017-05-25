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
#include <dali/graphics/vulkan/device-memory.h>
#include <dali/graphics/vulkan/frame-stack.h>
#include <dali/graphics/vulkan/image.h>
#include <dali/graphics/vulkan/logical-device.h>
#include <dali/graphics/vulkan/surface.h>
#include <dali/graphics/vulkan/swapchain.h>

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
  SwapchainImpl(const LogicalDevice& device, const Vulkan::Surface& surface, uint32_t bufferCount,
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

  /**
   *
   * @param index
   * @return
   */
  const CommandBuffer& GetCommandBuffer(uint32_t index) const
  {
    return mBuffers[index].commandBuffer;
  }

  /**
   *
   * @return
   */
  const CommandBuffer& GetCurrentCommandBuffer() const
  {
    return mBuffers[mAcquiredBufferIndex].commandBuffer;
  }

  const vk::Image& GetCurrentImage() const
  {
    return mBuffers[mAcquiredBufferIndex].image;
  }

  // swapchain integration
public:
  virtual bool Initialise();

  virtual bool AcquireFrame();

  virtual bool PresentFrame();

private:
  void GenerateViewFramebuffer(uint32_t bufferIndex, const vk::Image& image);

  void BeginRenderPass(CommandBuffer& cmdbuf);
  void RecordDepthStencilBarrier(vk::ImageMemoryBarrier& barrier);
  void RecordColorBarrier(vk::ImageMemoryBarrier& barrier, SwapchainFramebuffer& swapbuffer);
  void AcquireNextImage();

  vk::SwapchainKHR mVkSwapchain{nullptr};
  vk::SwapchainKHR mVkOldSwapchain{nullptr};

  // Depth/stencil
  Image         mVkDepthStencilImage{nullptr};
  vk::ImageView mVkDepthStencilImageView{nullptr};
  DeviceMemory  mVkDepthStencilImageMemory{nullptr};
  vk::Format    mDepthStencilFormat{};
  bool          mDepthStencilLayoutChangeNeeded{false}; // this flag indicates whether with new frame it's also necessary to change image layout (done once only)

  // renderpass
  vk::RenderPass mRenderPass{nullptr};

  LogicalDevice mDevice;

  Surface mSurface;

  SwapchainBufferArray mBuffers{};

  std::vector< vk::Semaphore > mAcquireSemaphore; // array of semaphores, can't be placed inside the buffer as
                                                  // we don't know the first index
  std::vector< vk::Semaphore > mPresentSemaphore;

  CommandPool mCommandPool{nullptr}; // command pool

  uint32_t     mCurrentBufferIndex{0};
  uint32_t     mAcquiredBufferIndex{-1u};
  uint32_t     mMaxBufferCount{-1u};
  DepthStencil mDepthStencilMode;
};

SwapchainImpl::SwapchainImpl(const LogicalDevice& device, const Vulkan::Surface& surface,
                             uint32_t bufferCount, DepthStencil depthStencil)
: GraphicsSwapchainBase(), mDevice(device), mSurface{surface}, mMaxBufferCount(bufferCount), mDepthStencilMode(depthStencil)
{
}

bool SwapchainImpl::Initialise()
{
  // create new command pool for new swapchain
  if(!mCommandPool)
  {
    mCommandPool = mDevice.CreateCommandPool(QueueType::GRAPHICS, false, false);
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
}

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
    mDevice.GetVkDevice().waitIdle();

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
      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
      .setImageSharingMode(vk::SharingMode::eExclusive)
      .setImageExtent({mSurface.GetWidth(), mSurface.GetHeight()})
      .setImageFormat(mSurface.GetFormat().format)
      .setImageColorSpace(mSurface.GetFormat().colorSpace)
      .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
      .setImageArrayLayers(1)
      .setClipped(true);

  VkAssertCall(mDevice.GetVkDevice().createSwapchainKHR(&info, mDevice.GetVkAllocator(), &mVkSwapchain));

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
    mDevice.GetVkDevice().destroyImageView(mVkDepthStencilImageView, mDevice.GetVkAllocator());
  }
  if(mVkDepthStencilImage)
  {
    //mDevice.GetVkDevice().destroyImage(mVkDepthStencilImage, mDevice.GetVkAllocator());
    mVkDepthStencilImage = nullptr;
  }

  // todo: include the proper depth stencil settings
  vk::Format format   = vk::Format::eD16Unorm;
  mDepthStencilFormat = format;

  Stack stack(1024);

  auto& imageInfo     = *stack.AllocNew< vk::ImageCreateInfo >();
  auto& imageViewInfo = *stack.AllocNew< vk::ImageViewCreateInfo >();

  imageInfo.setPQueueFamilyIndices(nullptr)
      .setQueueFamilyIndexCount(0)
      .setSamples(vk::SampleCountFlagBits::e1)
      .setInitialLayout(vk::ImageLayout::eUndefined)
      .setArrayLayers(1)
      .setExtent({mSurface.GetWidth(), mSurface.GetHeight(), 1})
      .setImageType(vk::ImageType::e2D)
      .setMipLevels(1)
      .setSharingMode(vk::SharingMode::eExclusive)
      .setTiling(vk::ImageTiling::eOptimal)
      .setFormat(format)
      .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);

  // create image
  mVkDepthStencilImage = Image::New(mDevice, imageInfo);

  imageViewInfo.setFormat(format)
      .setSubresourceRange(vk::ImageSubresourceRange()
                               .setLayerCount(1)
                               .setBaseArrayLayer(0)
                               .setBaseMipLevel(0)
                               .setLevelCount(1)
                               .setLayerCount(1)
                               .setAspectMask(vk::ImageAspectFlagBits::eDepth))
      .setImage(*mVkDepthStencilImage)
      .setViewType(vk::ImageViewType::e2D)
      .setComponents(vk::ComponentMapping()
                         .setR(vk::ComponentSwizzle::eR)
                         .setG(vk::ComponentSwizzle::eG)
                         .setB(vk::ComponentSwizzle::eB)
                         .setA(vk::ComponentSwizzle::eA));

  // create view
  VkAssertCall(mDevice.GetVkDevice().createImageView(&imageViewInfo, mDevice.GetVkAllocator(),
                                                     &mVkDepthStencilImageView));

  // allocate memory
  mVkDepthStencilImageMemory =
      mDevice.AllocateImageMemory(mVkDepthStencilImage, vk::MemoryPropertyFlagBits::eDeviceLocal, false);
  mVkDepthStencilImage.BindDeviceMemory(mVkDepthStencilImageMemory);

  // request changing layout with next frame
  mDepthStencilLayoutChangeNeeded = true;
}

void SwapchainImpl::GetSwapchainImages()
{
  auto device = mDevice.GetVkDevice();
  if(!mBuffers.empty())
  {
    // delete only views, images are discarded by swapchain as swapchain bound resource
    // we do not discard existing command buffers, no need when replacing swapchain
    for(auto&& buffer : mBuffers)
    {
      device.destroyImageView(buffer.imageView, mDevice.GetVkAllocator());
      device.destroyFramebuffer(buffer.framebuffer, mDevice.GetVkAllocator());
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

  // generate views and framebuffers
  for(uint32_t i = 0; i < mBuffers.size(); ++i)
  {
    GenerateViewFramebuffer(i, images[i]);
  }
}

void SwapchainImpl::GenerateViewFramebuffer(uint32_t bufferIndex, const vk::Image& image)
{
  Stack stack(1024);
  auto& ivInfo      = *stack.AllocNew< vk::ImageViewCreateInfo >();
  ivInfo.components = (*stack.AllocNew< vk::ComponentMapping >())
                          .setR(vk::ComponentSwizzle::eR)
                          .setG(vk::ComponentSwizzle::eG)
                          .setB(vk::ComponentSwizzle::eB)
                          .setA(vk::ComponentSwizzle::eA);
  ivInfo.viewType         = vk::ImageViewType::e2D;
  ivInfo.image            = image;
  ivInfo.subresourceRange = (*stack.AllocNew< vk::ImageSubresourceRange >())
                                .setLayerCount(1)
                                .setBaseArrayLayer(0)
                                .setBaseMipLevel(0)
                                .setLevelCount(1)
                                .setLayerCount(1)
                                .setAspectMask(vk::ImageAspectFlagBits::eColor);
  ivInfo.setFormat(mSurface.GetFormat().format);

  auto& fbInfo = *stack.AllocNew< vk::FramebufferCreateInfo >();

  fbInfo.pAttachments    = nullptr;
  fbInfo.attachmentCount = mDepthStencilMode != DepthStencil::NONE ? 2 : 1;
  fbInfo.renderPass      = mRenderPass;
  fbInfo.layers          = 1;
  fbInfo.width           = mSurface.GetWidth();
  fbInfo.height          = mSurface.GetHeight();

  auto& buffer = mBuffers[bufferIndex];

  // create image view
  buffer.image = image;
  ivInfo.setImage(buffer.image);
  VkAssertCall(mDevice.GetVkDevice().createImageView(&ivInfo, mDevice.GetVkAllocator(), &buffer.imageView));

  auto attachments = stack.Alloc< vk::ImageView >(2);
  attachments[0]   = buffer.imageView;
  attachments[1]   = mVkDepthStencilImageView;

  // create framebuffer
  fbInfo.pAttachments = attachments;
  VkAssertCall(mDevice.GetVkDevice().createFramebuffer(&fbInfo, mDevice.GetVkAllocator(), &buffer.framebuffer));
  buffer.layout = vk::ImageLayout::eUndefined;
}

void SwapchainImpl::CreateMainRenderPass()
{
  // Todo: sampling
  // Todo: stencil
  // Todo: separate clear if not needed ( however it's better if render pass does clear framebuffer automatically )
  auto device    = mDevice.GetVkDevice();
  auto allocator = mDevice.GetVkAllocator();

  auto stack = Stack{1024};

  uint32_t attCount = mDepthStencilMode != DepthStencil::NONE ? 2 : 1;

  vk::AttachmentReference attRef[2] = {
      vk::AttachmentReference().setLayout(vk::ImageLayout::eColorAttachmentOptimal).setAttachment(0),
      vk::AttachmentReference().setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal).setAttachment(1)};

  auto& subpassDescription = *stack.AllocNew< vk::SubpassDescription >();
  subpassDescription.setPResolveAttachments(nullptr)
      .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
      .setPDepthStencilAttachment(attCount == 2 ? &attRef[1] : nullptr)
      .setPColorAttachments(&attRef[0])
      .setColorAttachmentCount(1);

  auto attDesc = stack.Alloc< vk::AttachmentDescription >(2);
  attDesc[0]   = vk::AttachmentDescription()
                   .setFormat(mSurface.GetFormat().format)
                   .setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
                   .setSamples(vk::SampleCountFlagBits::e1)
                   .setStoreOp(vk::AttachmentStoreOp::eStore)
                   .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                   .setLoadOp(vk::AttachmentLoadOp::eClear)
                   .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
  attDesc[1] = vk::AttachmentDescription()
                   .setFormat(mDepthStencilFormat)
                   .setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                   .setSamples(vk::SampleCountFlagBits::e1)
                   .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                   .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                   .setLoadOp(vk::AttachmentLoadOp::eClear)
                   .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto& rpInfo = stack.AllocNewRef< vk::RenderPassCreateInfo >();
  rpInfo.setSubpassCount(1).setPSubpasses(&subpassDescription).setAttachmentCount(attCount).setPAttachments(attDesc);

  VkAssertCall(device.createRenderPass(&rpInfo, allocator, &mRenderPass));
}

void SwapchainImpl::CreateCommandBuffers()
{
  auto device = mDevice.GetVkDevice();
  for(auto&& buffer : mBuffers)
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
      VkAssertCall(device.createFence(&info, mDevice.GetVkAllocator(), &buffer.fence));
    }
  }
}

void SwapchainImpl::CreateSemaphores()
{
  auto device = mDevice.GetVkDevice();
  if(mAcquireSemaphore.empty())
  {
    mAcquireSemaphore.resize(mMaxBufferCount + 1);
  }
  if(mPresentSemaphore.empty())
  {
    mPresentSemaphore.resize(mMaxBufferCount + 1);
  }

  vk::SemaphoreCreateInfo info;
  for(auto&& sem : mAcquireSemaphore)
  {
    if(!sem)
    {
      VkAssertCall(device.createSemaphore(&info, mDevice.GetVkAllocator(), &sem));
    }
  }
  for(auto&& sem : mPresentSemaphore)
  {
    if(!sem)
    {
      VkAssertCall(device.createSemaphore(&info, mDevice.GetVkAllocator(), &sem));
    }
  }
}

void SwapchainImpl::BeginFrame()
{
  auto& stack = mDevice.GetStack(0);
  auto  frame = stack.Mark();

  auto& swapbuffer = mBuffers[mAcquiredBufferIndex];
  auto& cmdbuf     = swapbuffer.commandBuffer;

  AcquireNextImage();

  cmdbuf.Reset();
  cmdbuf.Begin(true);

  // change layouts if necessary
  auto     barriers     = stack.Alloc< vk::ImageMemoryBarrier >(2);
  uint32_t barrierIndex = 0;
  // change depth/stencil attachment layout
  if(mDepthStencilMode != DepthStencil::NONE && mDepthStencilLayoutChangeNeeded)
  {
    RecordDepthStencilBarrier(barriers[barrierIndex++]);
    mDepthStencilLayoutChangeNeeded = false;
  }

  // change color attachment layout
  if(swapbuffer.layout != vk::ImageLayout::eColorAttachmentOptimal)
  {
    RecordColorBarrier(barriers[barrierIndex++], swapbuffer);
  }

  // record pipeline barrier command
  cmdbuf->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::PipelineStageFlagBits::eTopOfPipe, vk::DependencyFlags(), 0, nullptr,
                          0, nullptr, static_cast< uint32_t >(barrierIndex), &barriers[0]);

  BeginRenderPass(cmdbuf);

  stack.Rollback(frame);
}

void SwapchainImpl::AcquireNextImage()
{
  auto  device     = mDevice.GetVkDevice();
  auto  semaphore  = mAcquireSemaphore[mCurrentBufferIndex];
  auto& swapbuffer = mBuffers[mAcquiredBufferIndex];

  // acquire new image
  VkTestCall(device.acquireNextImageKHR(mVkSwapchain, 1000000u, semaphore, nullptr, &mAcquiredBufferIndex));

  // check fence of selected buffer
  if(swapbuffer.submitted)
  {
    // fixme: ugly busy wait which in most of cases should not happen

    while(device.waitForFences(1, &swapbuffer.fence, false, 1000000) != vk::Result::eSuccess)
    {
    }
  }
  VkAssertCall(device.resetFences(1, &swapbuffer.fence));
}

void SwapchainImpl::RecordDepthStencilBarrier(vk::ImageMemoryBarrier& barrier)
{
  barrier.setImage(*mVkDepthStencilImage)
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
}

void SwapchainImpl::RecordColorBarrier(vk::ImageMemoryBarrier& barrier, SwapchainFramebuffer& swapbuffer)
{
  vk::AccessFlags srcAccess{};
  if(swapbuffer.layout != vk::ImageLayout::eUndefined)
  {
    srcAccess = (vk::AccessFlagBits::eMemoryRead);
  }

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

  swapbuffer.layout = vk::ImageLayout::eColorAttachmentOptimal;
}

void SwapchainImpl::BeginRenderPass(CommandBuffer& cmdbuf)
{
  auto& stack = mDevice.GetStack(0);
  auto  frame = stack.Mark();

  auto& swapbuffer = mBuffers[mAcquiredBufferIndex];
  auto& area       = stack.AllocNewRef< vk::Rect2D >();
  area.setOffset({0, 0}).setExtent({mSurface.GetWidth(), mSurface.GetHeight()});

  // THIS CODE IS VERY TEMPORARY!!!
  static float red = 0.0f;
  red += 0.01f;
  if(red > 1.0f)
  {
    red -= 1.0f;
  }

  // clear values
  auto clearValues = stack.Alloc< vk::ClearValue >(2);

  // clear color
  clearValues[0] = vk::ClearValue().color.setFloat32({red, 0.0f, 0.0f, 1.0f});

  // clear depth/stencil
  clearValues[1] = vk::ClearValue().depthStencil.setDepth(1.0f).setStencil(0);

  // begin main render pass
  auto& rpInfo = stack.AllocNewRef< vk::RenderPassBeginInfo >();
  rpInfo.setRenderPass(mRenderPass)
      .setRenderArea(area)
      .setPClearValues(clearValues)
      .setClearValueCount(mDepthStencilMode != DepthStencil::NONE ? 2 : 1)
      .setFramebuffer(swapbuffer.framebuffer);

  // begin render pass
  cmdbuf->beginRenderPass(&rpInfo, vk::SubpassContents::eInline);

  stack.Rollback(frame);
}

void SwapchainImpl::Present()
{
  Present(mDevice.GetCommandQueue(0, QueueType::PRESENT));
}

void SwapchainImpl::Present(const CommandQueue& queue)
{
  auto& swapbuffer = mBuffers[mAcquiredBufferIndex];
  auto& cmdbuf     = swapbuffer.commandBuffer;

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

namespace
{
SwapchainImpl* GetImpl(const Swapchain* handle)
{
  return static_cast< SwapchainImpl* >(handle->GetObject());
}
}

Swapchain Swapchain::New(const LogicalDevice& context, const Vulkan::Surface& surface,
                         uint32_t bufferCount, DepthStencil depthStencil)
{
  return Swapchain(new SwapchainImpl(context, surface, bufferCount, depthStencil));
}

const CommandBuffer& Swapchain::GetCommandBuffer(uint32_t index) const
{
  return GetImpl(this)->GetCommandBuffer(index);
}

const CommandBuffer& Swapchain::GetCurrentCommandBuffer() const
{
  return GetImpl(this)->GetCurrentCommandBuffer();
}

const vk::Image& Swapchain::GetCurrentImage() const
{
  return GetImpl(this)->GetCurrentImage();
}

} // Vulkan
} // Graphics
} // Dali
