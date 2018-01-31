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

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
struct Framebuffer::Impl
{
  Impl( Framebuffer& owner, Graphics& graphics, uint32_t width, uint32_t height )
  : mInterface( owner ), mGraphics( graphics ), mColorImageViewAttachments{}, mDepthStencilImageViewAttachment()
  {
  }

  // Framebuffer creation may be deferred
  bool Initialise()
  {
    if( mInitialised )
    {
      return true;
    }

    if( !Validate() )
    {
      return false;
    }

    /*
    auto attRef = vk::AttachmentReference{};
    attRef.setLayout();
    attRef.setAttachment();

    // creating single subpass per framebuffer
    auto subpassDesc = vk::SubpassDescription{};
    subpassDesc.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics );
    subpassDesc.setColorAttachmentCount(0);
    subpassDesc.setInputAttachmentCount(0);
    subpassDesc.setPDepthStencilAttachment(nullptr);
    subpassDesc.setPColorAttachments( nullptr );
    subpassDesc.setPInputAttachments( nullptr );
    subpassDesc.setPPreserveAttachments( nullptr );
    subpassDesc.setPResolveAttachments( nullptr );


    auto rpInfo = vk::RenderPassCreateInfo{};
    rpInfo.setAttachmentCount( mAttachments.size() );
    //rpInfo.setPAttachments( )
    rpInfo.setDependencyCount( 0 );
    rpInfo.setPDependencies( nullptr );
    rpInfo.setPSubpasses( &subpassDesc );
    rpInfo.setSubpassCount( 1 );

    auto fbInfo = vk::FramebufferCreateInfo{};
    fbInfo.setWidth( mWidth );
    fbInfo.setHeight( mHeight );
    //fbInfo.setRenderPass( )
    //fbInfo.setAttachmentCount( 0 );
    //fbInfo.setPAttachments( ImageViews );
    */
    mInitialised = true;
  }

  // creating render pass may happen either as deferred or
  // when framebuffer is initialised into immutable state
  void CreateRenderPass()
  {
    // for each attachment...
#if 0
    auto attRef = vk::AttachmentReference{};

    // 1. Need to know layout during render pass ( Image::GetLayout() )
    // 2. Usually it's going to be:
    //    - color_attachment_optimal
    //    - depth_stencil_attachment_optimal
    //attRef.setLayout();
    //attRef.setAttachment();

    // Single subpass support, all attachments used
    // TODO: input, preserve, resolve
    // TODO: create subpasses

    // creating single subpass per framebuffer
    auto subpassDesc = vk::SubpassDescription{};
    subpassDesc.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics );
    subpassDesc.setColorAttachmentCount( 0 );
    subpassDesc.setInputAttachmentCount( 0 );
    subpassDesc.setPDepthStencilAttachment( nullptr );
    subpassDesc.setPColorAttachments( nullptr );
    subpassDesc.setPInputAttachments( nullptr );
    subpassDesc.setPPreserveAttachments( nullptr );
    subpassDesc.setPResolveAttachments( nullptr );

    // create compatible render pass
    auto rpInfo = vk::RenderPassCreateInfo{};
    //rpInfo.setAttachmentCount( mAttachments.size() );
    //rpInfo.setPAttachments( )
    rpInfo.setDependencyCount( 0 );
    rpInfo.setPDependencies( nullptr );
    rpInfo.setPSubpasses( &subpassDesc );
    rpInfo.setSubpassCount( 1 );
#endif
  }

  void InitialiseAttachments()
  {
    ImageRef                attachment;
    vk::ImageViewCreateInfo info;
    info.setViewType( vk::ImageViewType::e2D );
    info.setSubresourceRange( //get layercount, get level count
      vk::ImageSubresourceRange{}.setLevelCount( 1 ).setLayerCount( 1 ).setBaseMipLevel( 0 ).setBaseArrayLayer( 0 ) );
    info.setImage( attachment->GetVkImage() ); //
    info.setComponents( vk::ComponentMapping(
      vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA ) );
    info.setFormat( vk::Format::eD16Unorm ); // get format

    //ImageViewRef ref = ImageView::New( attachment );
  }

  void CreateFramebuffer()
  {
    // assert if framebuffer is already created
    InitialiseAttachments();

    vk::FramebufferCreateInfo info;
    info.setRenderPass( mVkRenderPass )
      .setPAttachments( nullptr ) // attach imageviews, imageviews are created from supplied images
      .setLayers( 1 )
      .setWidth( mWidth )
      .setHeight( mHeight )
      .setAttachmentCount( 1 );

    mVkFramebuffer = VkAssert( mGraphics.GetDevice().createFramebuffer( info, mGraphics.GetAllocator() ) );
  }

  void SetAttachment( ImageViewRef imageViewRef, Framebuffer::AttachmentType type, uint32_t index )
  {
    // TODO: all array-type atyachments
    if( type == AttachmentType::COLOR )
    {
      auto& attachments = mColorImageViewAttachments;
      if( attachments.size() <= index )
      {
        attachments.resize( index + 1 );
      }
      attachments[index] = imageViewRef;
    }
    else if( type == AttachmentType::DEPTH_STENCIL )
    {
      mDepthStencilImageViewAttachment = imageViewRef;
    }
  }

  ImageViewRef GetAttachmentImageView( AttachmentType type, uint32_t index ) const
  {
    switch( type )
    {
      case AttachmentType::COLOR:
      {
        return mColorImageViewAttachments[index];
      }
      case AttachmentType::DEPTH_STENCIL:
      {
        return mDepthStencilImageViewAttachment;
      }
      case AttachmentType::DEPTH:
      case AttachmentType::INPUT:
      case AttachmentType::RESOLVE:
      case AttachmentType::PRESERVE:
      {
        return ImageViewRef();
      }
    }
    return ImageViewRef();
  }

  bool Validate()
  {
    if( mWidth == 0u || mHeight == 0 )
    {
      return false;
    }
  }

  ~Impl()
  {
  }

  vk::RenderPass GetVkRenderPass() const
  {
    return mVkRenderPass;
  }

  vk::Framebuffer GetVkFramebuffer() const
  {
    return mVkFramebuffer;
  }

  Framebuffer& mInterface;
  Graphics&    mGraphics;

  uint32_t mWidth;
  uint32_t mHeight;

  std::vector<ImageViewRef> mColorImageViewAttachments;
  ImageViewRef              mDepthStencilImageViewAttachment;
  vk::Framebuffer           mVkFramebuffer;
  vk::RenderPass            mVkRenderPass;

  bool mInitialised{false};
};

FramebufferRef Framebuffer::New( Graphics& graphics, uint32_t width, uint32_t height )
{
  return FramebufferRef();
}

void Framebuffer::SetAttachment( ImageViewRef imageViewRef, Framebuffer::AttachmentType type, uint32_t index )
{
  mImpl->SetAttachment( imageViewRef, type, index );
}

uint32_t Framebuffer::GetWidth() const
{
  return mImpl->mWidth;
}

uint32_t Framebuffer::GetHeight() const
{
  return mImpl->mHeight;
}

Handle<ImageView> Framebuffer::GetAttachmentImageView( AttachmentType type, uint32_t index ) const
{
  return mImpl->GetAttachmentImageView( type, index );
}

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali
