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
    mWidth = width;
    mHeight = height;
  }

  // creating render pass may happen either as deferred or
  // when framebuffer is initialised into immutable state
  bool Initialise()
  {
    mAttachmentReference.clear();
    mAttachmentDescription.clear();
    mDefaultClearValues.clear();
    /*
     * COLOR ATTACHMENTS
     */
    auto attachments         = std::vector<vk::ImageView>{};
    auto colorAttachmentSize = 0u;
    for( auto&& colorAttachment : mColorImageViewAttachments )
    {
      auto attRef = vk::AttachmentReference{};
      attRef.setLayout( vk::ImageLayout::eColorAttachmentOptimal );
      attRef.setAttachment( colorAttachmentSize++ );
      mAttachmentReference.emplace_back( attRef );
      attachments.emplace_back( colorAttachment->GetVkImageView() );

      vk::AttachmentDescription attDesc{};
      attDesc.setSamples( vk::SampleCountFlagBits::e1 )
        .setInitialLayout( vk::ImageLayout::eUndefined )
        .setFormat( colorAttachment->GetImage()->GetVkFormat() )
        .setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
        .setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
        .setLoadOp( vk::AttachmentLoadOp::eClear )
        .setStoreOp( vk::AttachmentStoreOp::eStore )
        .setFinalLayout( vk::ImageLayout::ePresentSrcKHR );

      mAttachmentDescription.emplace_back( attDesc );

      // update clear color values
      vk::ClearColorValue clear;
      clear.setFloat32( {0.0f, 0.0f, 0.0f, 1.0f} );
      mDefaultClearValues.emplace_back( clear );
    }

    /*
     * DEPTH-STENCIL ATTACHMENT
     */
    if( mDepthStencilImageViewAttachment )
    {
      auto attRef = vk::AttachmentReference{};
      attRef.setLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
      attRef.setAttachment( colorAttachmentSize );
      mAttachmentReference.emplace_back( attRef );
      attachments.emplace_back( mDepthStencilImageViewAttachment->GetVkImageView() );

      vk::AttachmentDescription attDesc{};
      attDesc.setSamples( vk::SampleCountFlagBits::e1 )
             .setInitialLayout( vk::ImageLayout::eUndefined )
             .setFormat( mDepthStencilImageViewAttachment->GetImage()->GetVkFormat() )
             .setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
             .setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
             .setLoadOp( vk::AttachmentLoadOp::eClear )
             .setStoreOp( vk::AttachmentStoreOp::eDontCare )
             .setFinalLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
      mAttachmentDescription.emplace_back( attDesc );

      // update clear depth/stencil values
      vk::ClearDepthStencilValue clear;
      clear.setDepth( 0.0f ).setStencil( 1.0f );
      mDefaultClearValues.emplace_back( clear );
    }

    /*
     * SUBPASS
     */
    // creating single subpass per framebuffer
    auto subpassDesc = vk::SubpassDescription{};
    subpassDesc.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics );
    subpassDesc.setColorAttachmentCount( colorAttachmentSize );
    if( mDepthStencilImageViewAttachment )
    {
      subpassDesc.setPDepthStencilAttachment( &mAttachmentReference[colorAttachmentSize] );
    }
    subpassDesc.setPColorAttachments( &mAttachmentReference[0] );

    /*
     * RENDERPASS
     */
    // create compatible render pass
    auto rpInfo = vk::RenderPassCreateInfo{};
    rpInfo.setAttachmentCount( U32(mAttachmentDescription.size()) );
    rpInfo.setPAttachments( mAttachmentDescription.data() );
    rpInfo.setPSubpasses( &subpassDesc );
    rpInfo.setSubpassCount( 1 );
    mVkRenderPass = VkAssert( mGraphics.GetDevice().createRenderPass( rpInfo, mGraphics.GetAllocator() ));

    /*
     * FRAMEBUFFER
     */
    vk::FramebufferCreateInfo info;
    info.setRenderPass( mVkRenderPass )
        .setPAttachments( attachments.data() )
        .setLayers( 1 )
        .setWidth( mWidth )
        .setHeight( mHeight )
        .setAttachmentCount( U32(attachments.size()) );

    mVkFramebuffer = VkAssert( mGraphics.GetDevice().createFramebuffer( info, mGraphics.GetAllocator() ) );

    return true;
  }

  /**
   * Creates immutable framebuffer object
   */
  bool Commit()
  {
    if(!mInitialised)
    {
      mInitialised = Initialise();
      return mInitialised;
    }
    return false;
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

  ImageViewRef GetAttachment( AttachmentType type, uint32_t index ) const
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

  std::vector<ImageViewRef> GetAttachments( AttachmentType type ) const
  {
    std::vector<ImageViewRef> retval{};
    switch( type )
    {
      case AttachmentType::COLOR:
      {
        retval.insert( retval.end(), mColorImageViewAttachments.begin(), mColorImageViewAttachments.end() );
        return retval;
      }
      case AttachmentType::DEPTH_STENCIL:
      {
        retval.push_back( mDepthStencilImageViewAttachment );
        return retval;
      }
      case AttachmentType::DEPTH:
      case AttachmentType::INPUT:
      case AttachmentType::RESOLVE:
      case AttachmentType::PRESERVE:
      {
        return retval;
      }
    }
    return retval;
  }

  uint32_t GetAttachmentCount( AttachmentType type ) const
  {
    std::vector<ImageViewRef> retval{};
    switch( type )
    {
      case AttachmentType::COLOR:
      {
        return U32(mColorImageViewAttachments.size());
      }
      case AttachmentType::DEPTH_STENCIL:
      {
        return mDepthStencilImageViewAttachment ? 1u : 0u;
      }
      case AttachmentType::DEPTH:
      case AttachmentType::INPUT:
      case AttachmentType::RESOLVE:
      case AttachmentType::PRESERVE:
      {
        return 0u;
      }
    }
    return 0u;
  }

  const std::vector<vk::ClearValue>& GetDefaultClearValues() const
  {
    return mDefaultClearValues;
  }

  ~Impl() = default;

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

  // attachment references for the main subpass
  std::vector<vk::AttachmentReference>   mAttachmentReference;
  std::vector<vk::AttachmentDescription> mAttachmentDescription;

  std::vector<vk::ClearValue> mDefaultClearValues;
  bool mInitialised{false};
};

FramebufferRef Framebuffer::New( Graphics& graphics, uint32_t width, uint32_t height )
{
  FramebufferRef ref( new Framebuffer( graphics, width, height ) );
  return ref;
}

Framebuffer::Framebuffer( Graphics& graphics, uint32_t width, uint32_t height )
{
  mImpl = std::make_unique<Impl>( *this, graphics, width, height );
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

ImageViewRef Framebuffer::GetAttachment( AttachmentType type, uint32_t index ) const
{
  return mImpl->GetAttachment( type, index );
}

std::vector<ImageViewRef> Framebuffer::GetAttachments( AttachmentType type ) const
{
  return mImpl->GetAttachments( type );
}

uint32_t Framebuffer::GetAttachmentCount( AttachmentType type ) const
{
  return mImpl->GetAttachmentCount( type );
}

void Framebuffer::Commit()
{
  mImpl->Commit();
}

vk::RenderPass Framebuffer::GetRenderPassVkHandle() const
{
  return mImpl->mVkRenderPass;
}

vk::Framebuffer Framebuffer::GetVkHandle() const
{
  return mImpl->mVkFramebuffer;
}

const std::vector<vk::ClearValue>& Framebuffer::GetDefaultClearValues() const
{
  return mImpl->GetDefaultClearValues();
}

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali
