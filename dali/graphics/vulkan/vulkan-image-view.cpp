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

#include <dali/graphics/vulkan/vulkan-image-view.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-debug.h>
#include <utility>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

ImageView::ImageView( Graphics& graphics, RefCountedImage image, vk::ImageViewCreateInfo createInfo )
        : mGraphics( &graphics ),
          mImage( std::move( image ) ),
          mCreateInfo( std::move( createInfo ) ),
          mImageView( nullptr )
{
}

ImageView::~ImageView() = default;

RefCountedImageView ImageView::New( Graphics& graphics,
                                    const RefCountedImage& image,
                                    const vk::ImageViewCreateInfo& createInfo )
{
  return RefCountedImageView( new ImageView( graphics, image, createInfo ) );
}

vk::ImageView ImageView::GetVkHandle() const
{
  return mImageView;
}

RefCountedImage ImageView::GetImage() const
{
  return mImage;
}

uint32_t ImageView::GetLayerCount() const
{
  return mImage->GetLayerCount();
}

uint32_t ImageView::GetMipLevelCount() const
{
  return mImage->GetMipLevelCount();
}

vk::ImageAspectFlags ImageView::GetImageAspectMask() const
{
  return vk::ImageAspectFlags();
}

const ImageView& ImageView::ConstRef()
{
  return *this;
}

ImageView& ImageView::Ref()
{
  return *this;
}

ImageView::operator vk::ImageView*()
{
  return &mImageView;
}

bool ImageView::OnDestroy()
{
  if( !mGraphics->IsShuttingDown() )
  {
    mGraphics->RemoveImageView( *this );
  }

  auto device = mGraphics->GetDevice();
  auto imageView = mImageView;
  auto allocator = &mGraphics->GetAllocator();

  mGraphics->DiscardResource( [ device, imageView, allocator ]() {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: image view->%p\n",
                   static_cast< void* >(imageView) )
    device.destroyImageView( imageView, allocator );
  } );

  return VkManaged::OnDestroy();
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

