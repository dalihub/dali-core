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

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
struct Framebuffer::Impl
{
  Impl( Framebuffer& owner, Graphics& graphics, uint32_t width, uint32_t height )
          : mInterface( owner ),
            mGraphics( graphics ),
            mWidth( width ),
            mHeight( height ),
            mColorImageViewAttachments{},
            mDepthStencilImageViewAttachment()
  {
  }

  // creating render pass may happen either as deferred or
  // when framebuffer is initialised into immutable state
  bool Initialise()
  {
    auto attachments = std::vector< vk::ImageView >{};

    if( !mExternalRenderPass )
    {
      mAttachmentReference.clear();
      mAttachmentDescription.clear();
      mDefaultClearValues.clear();
      /*
       * COLOR ATTACHMENTS
       */

      auto colorAttachmentSize = 0u;
      for( auto&& colorAttachment : mColorImageViewAttachments )
      {
        auto attRef = vk::AttachmentReference{};
        attRef.setLayout( vk::ImageLayout::eColorAttachmentOptimal );
        attRef.setAttachment( colorAttachmentSize++ );
        mAttachmentReference.push_back( attRef );
        attachments.push_back( colorAttachment->GetVkHandle() );

        vk::AttachmentDescription attDesc{};
        attDesc.setSamples( vk::SampleCountFlagBits::e1 )
               .setInitialLayout( vk::ImageLayout::eUndefined )
               .setFormat( colorAttachment->GetImage()->GetFormat() )
               .setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
               .setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
               .setLoadOp( vk::AttachmentLoadOp::eClear )
               .setStoreOp( vk::AttachmentStoreOp::eStore )
               .setFinalLayout( vk::ImageLayout::ePresentSrcKHR );

        mAttachmentDescription.push_back( attDesc );

        // update clear color values
        vk::ClearColorValue clear;
        clear.setFloat32( { 0.0f, 0.0f, 0.0f, 1.0f } );
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
        mAttachmentReference.push_back( attRef );
        attachments.push_back( mDepthStencilImageViewAttachment->GetVkHandle() );

        vk::AttachmentDescription attDesc{};
        attDesc.setSamples( vk::SampleCountFlagBits::e1 )
               .setInitialLayout( vk::ImageLayout::eUndefined )
               .setFormat( mDepthStencilImageViewAttachment->GetImage()->GetFormat() )
               .setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
               .setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
               .setLoadOp( vk::AttachmentLoadOp::eClear )
               .setStoreOp( vk::AttachmentStoreOp::eDontCare )
               .setFinalLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
        mAttachmentDescription.push_back( attDesc );

        // update clear depth/stencil values
        vk::ClearDepthStencilValue clear;
        clear.setDepth( 0.0f ).setStencil( 1 );
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

      std::array< vk::SubpassDependency, 2 > subpassDependencies{

              vk::SubpassDependency{}.setSrcSubpass( VK_SUBPASS_EXTERNAL )
                                     .setDstSubpass( 0 )
                                     .setSrcStageMask( vk::PipelineStageFlagBits::eBottomOfPipe )
                                     .setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
                                     .setSrcAccessMask( vk::AccessFlagBits::eMemoryRead )
                                     .setDstAccessMask( vk::AccessFlagBits::eColorAttachmentRead |
                                                        vk::AccessFlagBits::eColorAttachmentWrite )
                                     .setDependencyFlags( vk::DependencyFlagBits::eByRegion ),
              vk::SubpassDependency{}.setSrcSubpass( 0 )
                                     .setDstSubpass( VK_SUBPASS_EXTERNAL )
                                     .setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
                                     .setDstStageMask( vk::PipelineStageFlagBits::eBottomOfPipe )
                                     .setSrcAccessMask( vk::AccessFlagBits::eColorAttachmentRead |
                                                        vk::AccessFlagBits::eColorAttachmentWrite )
                                     .setDstAccessMask( vk::AccessFlagBits::eMemoryRead )
                                     .setDependencyFlags( vk::DependencyFlagBits::eByRegion )

      };

      /*
       * RENDERPASS
       */
      // create compatible render pass
      auto renderPassCreateInfo = vk::RenderPassCreateInfo{}.setAttachmentCount( U32( mAttachmentDescription.size() ) )
                                                            .setPAttachments( mAttachmentDescription.data() )
                                                            .setPSubpasses( &subpassDesc )
                                                            .setSubpassCount( 1 )
                                                            .setPDependencies( subpassDependencies.data() );


      mVkRenderPass = VkAssert( mGraphics.GetDevice().createRenderPass( renderPassCreateInfo,
                                                                        mGraphics.GetAllocator() ) );
    }
    else //Render Pass is externally provided. Just fetch the attachment image views and create the framebuffer
    {
      for( const auto& colorAttachment : mColorImageViewAttachments )
      {
        attachments.push_back( colorAttachment->GetVkHandle() );
      }

      if( mDepthStencilImageViewAttachment )
      {
        attachments.push_back( mDepthStencilImageViewAttachment->GetVkHandle() );
      }
    }

    /*
     * FRAMEBUFFER
     */
    auto framebufferCreateInfo = vk::FramebufferCreateInfo{}.setRenderPass( mVkRenderPass )
                                                            .setPAttachments( attachments.data() )
                                                            .setLayers( 1 )
                                                            .setWidth( mWidth )
                                                            .setHeight( mHeight )
                                                            .setAttachmentCount( U32( attachments.size() ) );

    mVkFramebuffer = VkAssert( mGraphics.GetDevice()
                                        .createFramebuffer( framebufferCreateInfo,
                                                            mGraphics.GetAllocator() ) );

    return true;
  }

  /**
   * Creates immutable framebuffer object
   */
  bool Commit()
  {
    if( !mInitialised )
    {
      mInitialised = Initialise();
      return mInitialised;
    }
    return false;
  }

  void SetAttachment( RefCountedImageView imageViewRef, Framebuffer::AttachmentType type, uint32_t index )
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

  void SetExternalRenderPass( vk::RenderPass externalRenderPass )
  {
    mExternalRenderPass = true;
    mVkRenderPass = externalRenderPass;
  }

  RefCountedImageView GetAttachment( AttachmentType type, uint32_t index ) const
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
        return RefCountedImageView();
      }
    }
    return RefCountedImageView();
  }

  std::vector< RefCountedImageView > GetAttachments( AttachmentType type ) const
  {
    std::vector< RefCountedImageView > retval{};
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
    switch( type )
    {
      case AttachmentType::COLOR:
      {
        return U32( mColorImageViewAttachments.size() );
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

  const std::vector< vk::ClearValue >& GetDefaultClearValues() const
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
  Graphics& mGraphics;

  uint32_t mWidth;
  uint32_t mHeight;

  std::vector< RefCountedImageView > mColorImageViewAttachments;
  RefCountedImageView mDepthStencilImageViewAttachment;
  vk::Framebuffer mVkFramebuffer;
  vk::RenderPass mVkRenderPass;

  // attachment references for the main subpass
  std::vector< vk::AttachmentReference > mAttachmentReference;
  std::vector< vk::AttachmentDescription > mAttachmentDescription;

  std::vector< vk::ClearValue > mDefaultClearValues;
  bool mInitialised{ false };
  bool mExternalRenderPass{ false };
};

RefCountedFramebuffer Framebuffer::New( Graphics& graphics, uint32_t width, uint32_t height )
{
  RefCountedFramebuffer ref( new Framebuffer( graphics, width, height ) );
  return ref;
}

Framebuffer::Framebuffer( Graphics& graphics, uint32_t width, uint32_t height )
{
  mImpl = std::make_unique< Impl >( *this, graphics, width, height );
}

void Framebuffer::SetAttachment( RefCountedImageView imageViewRef, Framebuffer::AttachmentType type, uint32_t index )
{
  mImpl->SetAttachment( std::move( imageViewRef ), type, index );
}

void Framebuffer::SetExternalRenderPass( vk::RenderPass externalRenderPass )
{
  mImpl->SetExternalRenderPass( externalRenderPass );
}

uint32_t Framebuffer::GetWidth() const
{
  return mImpl->mWidth;
}

uint32_t Framebuffer::GetHeight() const
{
  return mImpl->mHeight;
}

RefCountedImageView Framebuffer::GetAttachment( AttachmentType type, uint32_t index ) const
{
  return mImpl->GetAttachment( type, index );
}

std::vector< RefCountedImageView > Framebuffer::GetAttachments( AttachmentType type ) const
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

const std::vector< vk::ClearValue >& Framebuffer::GetDefaultClearValues() const
{
  return mImpl->GetDefaultClearValues();
}

bool Framebuffer::OnDestroy()
{
  if( !mImpl->mGraphics.IsShuttingDown() )
  {
    mImpl->mGraphics.RemoveFramebuffer( *this );
  }

  auto device = mImpl->mGraphics.GetDevice();
  auto frameBuffer = mImpl->mVkFramebuffer;

  vk::RenderPass renderPass;
  if( mImpl->mExternalRenderPass )
  {
    renderPass = nullptr;
  }
  else
  {
    renderPass = mImpl->mVkRenderPass;
  }

  auto allocator = &mImpl->mGraphics.GetAllocator();

  mImpl->mGraphics.DiscardResource( [ device, frameBuffer, renderPass, allocator ]() {

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
