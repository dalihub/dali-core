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
  Impl( Framebuffer& owner, Graphics& graphics, uint32_t width, uint32_t height ) :
    mInterface( owner ),
    mGraphics( graphics )
  {

  }

  // Framebuffer creation may be deferred
  bool Initialise()
  {
    if(mInitialised)
    {
      return true;
    }

    if(!Validate())
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


  void SetAttachment( Handle<Image> image, Framebuffer::AttachmentType type, uint32_t index )
  {
    std::vector<Handle<Image>>& attachments =
      type == AttachmentType::COLOR ? mColorAttachments : mDepthStencilAttachments;

    if( attachments.size() <= index )
    {
      attachments.resize( index+1 );
    }
    attachments[index] = image;
  }

  ImageRef GetAttachmentImage( AttachmentType type, uint32_t index ) const
  {
    return ImageRef();
  }

  ImageViewRef GetAttachmentImageView( AttachmentType type, uint32_t index ) const
  {
    return ImageViewRef();
  }

  bool Validate()
  {
    if( mWidth == 0u || mHeight == 0  )
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

  Framebuffer&            mInterface;
  Graphics&               mGraphics;

  uint32_t mWidth;
  uint32_t mHeight;
  std::vector<Handle<Image>>      mColorAttachments;
  std::vector<Handle<Image>>      mDepthStencilAttachments;

  std::vector<ImageView>  mImageViewAttachments;
  vk::Framebuffer         mVkFramebuffer;
  vk::RenderPass          mVkRenderPass;

  bool mInitialised { false };
};


Handle<Framebuffer> Framebuffer::New( Graphics& graphics, uint32_t width, uint32_t height )
{
  return FramebufferRef();
}

void Framebuffer::SetAttachment( Handle<Image> image, Framebuffer::AttachmentType type, uint32_t index )
{
  mImpl->SetAttachment( image, type, index );
}

uint32_t Framebuffer::GetWidth() const
{
  return mImpl->mWidth;
}

uint32_t Framebuffer::GetHeight() const
{
  return mImpl->mHeight;
}

Handle<Image> Framebuffer::GetAttachmentImage( AttachmentType type, uint32_t index ) const
{
  return mImpl->GetAttachmentImage( type, index );
}

Handle<ImageView> Framebuffer::GetAttachmentImageView( AttachmentType type, uint32_t index ) const
{
  return mImpl->GetAttachmentImageView( type, index );
}

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali
