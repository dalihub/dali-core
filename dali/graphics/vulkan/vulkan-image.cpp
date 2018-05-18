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

#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
struct Image::Impl
{
  Impl( Image& owner, Graphics& graphics, vk::ImageCreateInfo createInfo, vk::Image externalImage = nullptr )
  : mOwner( owner ),
    mGraphics( graphics ),
    mVkImage( externalImage ),
    mCreateInfo( std::move( createInfo ) ),
    mIsExternal( static_cast<bool>( externalImage ) )
  {
    mVkImageLayout = mCreateInfo.initialLayout;
  }

  ~Impl()
  {
    if( !mIsExternal )
    {
      // destroy
    }
  }

  bool Initialise()
  {
    if( !mIsExternal )
    {
      mVkImage = VkAssert( mGraphics.GetDevice().createImage( mCreateInfo, mGraphics.GetAllocator() ) );
      if( !mVkImage )
      {
        return false;
      }
    }
    return true;
  }

  void BindMemory( const RefCountedGpuMemoryBlock& handle )
  {
    mGraphics.GetDevice().bindImageMemory( mVkImage, *handle, 0 );
    mDeviceMemory = handle;
  }

  Image&              mOwner;
  Graphics&           mGraphics;
  vk::Image           mVkImage;
  vk::ImageLayout     mVkImageLayout;
  vk::ImageCreateInfo mCreateInfo;

  RefCountedGpuMemoryBlock   mDeviceMemory;
  bool mIsExternal;
};

Image::~Image() = default;

RefCountedImage Image::New( Graphics& graphics, vk::ImageCreateInfo createInfo )
{
  RefCountedImage retval( new Image( graphics, createInfo, nullptr ) );
  if( !retval->mImpl->Initialise() )
  {
    retval.Reset();
  }
  else
  {
    graphics.AddImage( retval );
  }
  return retval;
}

RefCountedImage Image::New( Graphics& graphics, vk::ImageCreateInfo createInfo, vk::Image externalImage )
{
  RefCountedImage retval( new Image( graphics, createInfo, externalImage ) );
  if( !retval->mImpl->Initialise() )
  {
    retval.Reset();
  }
  else
  {
    graphics.AddImage( retval );
  }
  return retval;
}

Image::Image( Graphics& graphics, const vk::ImageCreateInfo& createInfo, vk::Image externalImage )
{
  mImpl = MakeUnique<Impl>( *this, graphics, createInfo, externalImage );
}

vk::Image Image::GetVkHandle() const
{
  return mImpl->mVkImage;
}

vk::ImageLayout Image::GetVkImageLayout() const
{
  return mImpl->mVkImageLayout;
}

uint32_t Image::GetWidth() const
{
  return mImpl->mCreateInfo.extent.width;
}

uint32_t Image::GetHeight() const
{
  return mImpl->mCreateInfo.extent.height;
}

uint32_t Image::GetLayerCount() const
{
  return mImpl->mCreateInfo.arrayLayers;
}

uint32_t Image::GetLevelCount() const
{
  return mImpl->mCreateInfo.mipLevels;
}

vk::Format Image::GetVkFormat() const
{
  return mImpl->mCreateInfo.format;
}

vk::ImageType Image::GetVkImageType() const
{
  return mImpl->mCreateInfo.imageType;
}

vk::ImageTiling Image::GetVkImageTiling() const
{
  return mImpl->mCreateInfo.tiling;
}

void Image::BindMemory( const RefCountedGpuMemoryBlock& handle )
{
  mImpl->BindMemory( handle );
}

vk::ImageUsageFlags Image::GetVkImageUsageFlags() const
{
  return mImpl->mCreateInfo.usage;
}

/***************************************************************************
 *
 *  ImageView
 *
 */

struct ImageView::Impl
{
  Impl( ImageView& owner, Graphics& graphics, RefCountedImage image, vk::ImageViewCreateInfo createInfo )
  : mOwner( owner ), mGraphics( graphics ), mImage( image ), mCreateInfo( createInfo )
  {
  }

  ~Impl()
  {
  }

  bool Initialise()
  {
    mVkImageView = VkAssert( mGraphics.GetDevice().createImageView( mCreateInfo, mGraphics.GetAllocator() ) );
    if( !mVkImageView )
    {
      return false;
    }
    return true;
  }

  ImageView&              mOwner;
  Graphics&               mGraphics;
  RefCountedImage                mImage;
  vk::ImageViewCreateInfo mCreateInfo;

  vk::ImageView mVkImageView;
};

RefCountedImageView ImageView::New( Graphics& graphics, RefCountedImage image, vk::ImageViewCreateInfo info )
{
  auto retval = RefCountedImageView( new ImageView( graphics, image, info ) );
  if( !retval->mImpl->Initialise() )
  {
    return RefCountedImageView();
  }
  return retval;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
RefCountedImageView ImageView::New( Graphics& graphics, RefCountedImage image )
{
  vk::ComponentMapping componentsMapping = { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB,vk::ComponentSwizzle::eA };
  vk::ImageAspectFlags aspectFlags{};
  if( image->GetVkImageUsageFlags() & vk::ImageUsageFlagBits::eColorAttachment )
  {
    aspectFlags |= vk::ImageAspectFlagBits::eColor;
    //componentsMapping = { vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eB,vk::ComponentSwizzle::eA };
  }
  if( image->GetVkImageUsageFlags() & vk::ImageUsageFlagBits::eDepthStencilAttachment )
  {
    aspectFlags |= (vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
  }
  if( image->GetVkImageUsageFlags() & vk::ImageUsageFlagBits::eSampled )
  {
    aspectFlags |= (vk::ImageAspectFlagBits::eColor);
    //componentsMapping = { vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eR,vk::ComponentSwizzle::eA };
  }
  auto subresourceRange = vk::ImageSubresourceRange{}
    .setAspectMask( aspectFlags )
    .setBaseArrayLayer( 0 )
    .setBaseMipLevel( 0 )
    .setLevelCount( image->GetLevelCount() )
    .setLayerCount( image->GetLayerCount() );

  // create reference, image may be null
  auto retval = RefCountedImageView( new ImageView( graphics,
                                             image,
                                             vk::ImageViewCreateInfo{}
                                               .setViewType( vk::ImageViewType::e2D )
                                               .setFormat( image->GetVkFormat() )
                                               .setSubresourceRange(subresourceRange)
                                               .setComponents( componentsMapping )
                                                .setImage(image->GetVkHandle() )));
  if(!retval->mImpl->Initialise())
  {
    return RefCountedImageView();
  }

  return retval;
}
#pragma GCC diagnostic pop

ImageView::ImageView( Graphics& graphics, RefCountedImage image, const VkImageViewCreateInfo& createInfo )
{
  mImpl = MakeUnique<Impl>( *this, graphics, image, createInfo );
}


ImageView::~ImageView() = default;

const vk::ImageView& ImageView::GetVkImageView() const
{
  return mImpl->mVkImageView;
}

RefCountedImage ImageView::GetImage() const
{
  return mImpl->mImage;
}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali