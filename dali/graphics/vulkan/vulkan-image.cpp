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

  Image&              mOwner;
  Graphics&           mGraphics;
  vk::Image           mVkImage;
  vk::ImageLayout     mVkImageLayout;
  vk::ImageCreateInfo mCreateInfo;

  bool mIsExternal;
};

Image::~Image() = default;

ImageRef Image::New( Graphics& graphics, vk::ImageCreateInfo createInfo )
{
  ImageRef retval( new Image( graphics, createInfo, nullptr ) );
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

ImageRef Image::New( Graphics& graphics, vk::ImageCreateInfo createInfo, vk::Image externalImage )
{
  ImageRef retval( new Image( graphics, createInfo, externalImage ) );
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

vk::Image Image::GetVkImage() const
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

void Image::BindMemory( GpuMemoryBlockRef& handle )
{
}

/***************************************************************************
 *
 *  ImageView
 *
 */

struct ImageView::Impl
{
  Impl( ImageView& owner, Graphics& graphics, ImageRef image, vk::ImageViewCreateInfo createInfo )
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
  ImageRef                mImage;
  vk::ImageViewCreateInfo mCreateInfo;

  vk::ImageView mVkImageView;
};

ImageViewRef ImageView::New( Graphics& graphics, ImageRef image, vk::ImageViewCreateInfo info )
{
  auto retval = ImageViewRef( new ImageView( graphics, image, info ) );
  if(!retval->mImpl->Initialise())
  {
    return ImageViewRef();
  }
  return retval;
}

ImageView::ImageView( Graphics& graphics, ImageRef image, const VkImageViewCreateInfo& createInfo )
{
  mImpl = MakeUnique<Impl>( *this, graphics, image, createInfo );
}

ImageView::~ImageView() = default;

const vk::ImageView& ImageView::GetVkImageView() const
{
  return mImpl->mVkImageView;
}

ImageRef ImageView::GetImage() const
{
  return mImpl->mImage;
}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali