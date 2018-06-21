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

#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-image-view.h>
#include <dali/graphics/vulkan/vulkan-debug.h>
#include <utility>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

//FramebufferAttachment ------------------------
RefCountedFramebufferAttachment FramebufferAttachment::NewColorAttachment( RefCountedImageView imageView,
                                                                 vk::ClearColorValue clearColorValue,
                                                                 bool presentable )
{
  assert( imageView->GetImage()->GetUsageFlags() & vk::ImageUsageFlagBits::eColorAttachment );

  auto attachment = new FramebufferAttachment( std::move( imageView ), clearColorValue, presentable );

  return RefCountedFramebufferAttachment( attachment );
}

RefCountedFramebufferAttachment FramebufferAttachment::NewDepthAttachment( RefCountedImageView imageView,
                                                                 vk::ClearDepthStencilValue clearDepthStencilValue )
{
  assert( imageView->GetImage()->GetUsageFlags() & vk::ImageUsageFlagBits::eDepthStencilAttachment );

  auto attachment = new FramebufferAttachment( std::move( imageView ), clearDepthStencilValue, false );

  return RefCountedFramebufferAttachment( attachment );
}

FramebufferAttachment::FramebufferAttachment( RefCountedImageView imageView,
                                              vk::ClearValue clearColor,
                                              bool presentable )
        : mImageView( imageView ),
          mClearValue( clearColor )
{
  auto image = imageView->GetImage();
  auto usage = image->GetUsageFlags();

  auto sampleCountFlags = image->GetSampleCount();

  mDescription.setSamples( sampleCountFlags );

  mDescription.setStoreOp( vk::AttachmentStoreOp::eStore );
  mDescription.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare );
  mDescription.setStencilLoadOp( vk::AttachmentLoadOp::eDontCare );
  mDescription.setFormat( image->GetFormat() );
  mDescription.setInitialLayout( vk::ImageLayout::eUndefined );
  mDescription.setLoadOp( vk::AttachmentLoadOp::eClear );

  if( usage & vk::ImageUsageFlagBits::eDepthStencilAttachment )
  {
    mDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
  }
  else
  {
    mDescription.finalLayout = presentable ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eShaderReadOnlyOptimal;
  }
}

RefCountedImageView FramebufferAttachment::GetImageView() const
{
  return mImageView;
}

const vk::AttachmentDescription& FramebufferAttachment::GetDescription() const
{
  return mDescription;
}

const vk::ClearValue& FramebufferAttachment::GetClearValue() const
{
  return mClearValue;
}

bool FramebufferAttachment::IsValid() const
{
  return mImageView;
}


//Framebuffer -------------------------------
RefCountedFramebuffer Framebuffer::New( Graphics& graphics,
                                        const std::vector< RefCountedFramebufferAttachment >& colorAttachments,
                                        const RefCountedFramebufferAttachment& depthAttachment,
                                        vk::Framebuffer vkHandle,
                                        vk::RenderPass renderPass,
                                        uint32_t width,
                                        uint32_t height,
                                        bool externalRenderPass )
{
  return RefCountedFramebuffer( new Framebuffer( graphics,
                                                 colorAttachments,
                                                 depthAttachment,
                                                 vkHandle,
                                                 renderPass,
                                                 width,
                                                 height,
                                                 externalRenderPass ) );
}


Framebuffer::Framebuffer( Graphics& graphics,
                          const std::vector< RefCountedFramebufferAttachment >& colorAttachments,
                          const RefCountedFramebufferAttachment& depthAttachment,
                          vk::Framebuffer vkHandle,
                          vk::RenderPass renderPass,
                          uint32_t width,
                          uint32_t height,
                          bool externalRenderPass )
        : mGraphics( &graphics ),
          mWidth( width ),
          mHeight( height ),
          mColorAttachments( std::move( colorAttachments ) ),
          mDepthAttachment( std::move( depthAttachment ) ),
          mFramebuffer( vkHandle ),
          mRenderPass( renderPass ),
          mExternalRenderPass( externalRenderPass )
{
}

uint32_t Framebuffer::GetWidth() const
{
  return mWidth;
}

uint32_t Framebuffer::GetHeight() const
{
  return mHeight;
}

RefCountedFramebufferAttachment Framebuffer::GetAttachment( AttachmentType type, uint32_t index ) const
{
  switch( type )
  {
    case AttachmentType::COLOR:
    {
      return mColorAttachments[index];
    }
    case AttachmentType::DEPTH_STENCIL:
    {
      return mDepthAttachment;
    }
    case AttachmentType::DEPTH:
    case AttachmentType::INPUT:
    case AttachmentType::RESOLVE:
    case AttachmentType::PRESERVE:
    case AttachmentType::UNDEFINED:
      break;
  }

  return RefCountedFramebufferAttachment{};
}

std::vector< RefCountedFramebufferAttachment > Framebuffer::GetAttachments( AttachmentType type ) const
{
  auto retval = std::vector< RefCountedFramebufferAttachment >{};
  switch( type )
  {
    case AttachmentType::COLOR:
    {
      retval.reserve( mColorAttachments.size() );
      retval.insert( retval.end(), mColorAttachments.begin(), mColorAttachments.end() );
      return std::move( retval );
    }
    case AttachmentType::DEPTH_STENCIL:
    {
      retval.reserve( 1 );
      retval.push_back( mDepthAttachment );
      return std::move( retval );
    }
    case AttachmentType::DEPTH:
    case AttachmentType::INPUT:
    case AttachmentType::RESOLVE:
    case AttachmentType::PRESERVE:
    case AttachmentType::UNDEFINED:
    {
      return std::move( retval );
    }
  }
  return std::move( retval );
}

uint32_t Framebuffer::GetAttachmentCount( AttachmentType type ) const
{
  switch( type )
  {
    case AttachmentType::COLOR:
    {
      return U32( mColorAttachments.size() );
    }
    case AttachmentType::DEPTH_STENCIL:
    {
      return mDepthAttachment->IsValid() ? 1u : 0u;
    }
    case AttachmentType::DEPTH:
    case AttachmentType::INPUT:
    case AttachmentType::RESOLVE:
    case AttachmentType::PRESERVE:
    case AttachmentType::UNDEFINED:
      return 0u;
  }
  return 0u;
}

vk::RenderPass Framebuffer::GetRenderPass() const
{
  return mRenderPass;
}

vk::Framebuffer Framebuffer::GetVkHandle() const
{
  return mFramebuffer;
}

std::vector< vk::ClearValue > Framebuffer::GetClearValues() const
{
  auto result = std::vector< vk::ClearValue >{};

  std::transform(mColorAttachments.begin(),
                 mColorAttachments.end(),
                 std::back_inserter( result ),
                 []( const RefCountedFramebufferAttachment& attachment )
                 {
                   return attachment->GetClearValue();
                 });

  if( mDepthAttachment->IsValid() )
  {
    result.push_back( mDepthAttachment->GetClearValue() );
  }

  return std::move( result );
}

bool Framebuffer::OnDestroy()
{
  if( !mGraphics->IsShuttingDown() )
  {
    mGraphics->RemoveFramebuffer( *this );
  }

  auto device = mGraphics->GetDevice();
  auto frameBuffer = mFramebuffer;

  vk::RenderPass renderPass;
  if( mExternalRenderPass )
  {
    renderPass = nullptr;
  }
  else
  {
    renderPass = mRenderPass;
  }

  auto allocator = &mGraphics->GetAllocator();

  mGraphics->DiscardResource( [ device, frameBuffer, renderPass, allocator ]() {

    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: framebuffer->%p\n",
                   static_cast< void* >(frameBuffer) )
    device.destroyFramebuffer( frameBuffer, allocator );

    if( renderPass )
    {
      DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: render pass->%p\n",
                     static_cast< void* >(renderPass) )
      device.destroyRenderPass( renderPass, allocator );
    }

  } );

  return false;
}

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali
