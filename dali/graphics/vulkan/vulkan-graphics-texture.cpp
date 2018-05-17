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

#include <dali/graphics/vulkan/vulkan-graphics-texture.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-fence.h>
#include <dali/graphics/vulkan/vulkan-queue.h>
#include <dali/graphics/vulkan/vulkan-sampler.h>
#include <thread>
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
using namespace Dali::Graphics::Vulkan;

struct Pixmap
{
  explicit Pixmap( uint32_t _width, uint32_t _height, vk::Format format  )
    : width( _width ), height( _height ), bytesPerPixel(1), pixelFormat( format )
  {
    totalSizeInBytes = width*height*bytesPerPixel;
    data.resize( totalSizeInBytes );

  }

  std::vector<uint8_t>  data;
  uint32_t              width;
  uint32_t              height;
  uint32_t              bytesPerPixel;
  uint32_t              totalSizeInBytes;
  vk::Format            pixelFormat;
};

/**
 * Texture::Impl
 * Responsible for creating an image, basic image view, allocating texture memory,
 * transfering the data into into the texture
 *
 */
struct Texture::Impl
{
  Impl( Texture& owner,
        Graphics& graphics,
        uint32_t width,
        uint32_t height,
        vk::Format format ) : mGraphics( graphics ),
        mPixmap( width, height, format )
  {
    mWidth = width;
    mHeight = height;
    mFormat = format;
  }

  bool UploadData( const void* data, uint32_t offsetInBytes, uint32_t sizeInBytes )
  {
    // create buffer
    auto& allocator = mGraphics.GetDeviceMemoryManager().GetDefaultAllocator();
    auto size   = sizeInBytes;
    auto buffer = Buffer::New( mGraphics,
                               vk::BufferCreateInfo{}
                                 .setUsage( vk::BufferUsageFlagBits::eTransferSrc )
                                 .setSharingMode( vk::SharingMode::eExclusive )
                                 .setSize( size ) );

    buffer->BindMemory( allocator.Allocate( buffer, vk::MemoryPropertyFlagBits::eHostVisible ) );

    // copy pixels to the buffer
    auto ptr = buffer->GetMemoryHandle()->MapTyped<char>();
    std::copy( reinterpret_cast<const char*>(data),
               reinterpret_cast<const char*>(data) + sizeInBytes,
               ptr );
    buffer->GetMemoryHandle()->Unmap();

    // record copy and layout change
    auto copy = vk::BufferImageCopy{}
      .setImageExtent( { mWidth, mHeight, 1} )
      .setBufferImageHeight( mHeight )
      .setBufferOffset( 0 )
      .setBufferRowLength( mWidth )
      .setImageOffset( {0, 0, 0} )
      .setImageSubresource( vk::ImageSubresourceLayers{}
                              .setMipLevel( 0 )
                              .setAspectMask( vk::ImageAspectFlagBits::eColor )
                              .setLayerCount( 1 )
                              .setBaseArrayLayer( 0 ) );

    if(!mCommandPool)
    {
      mCommandPool   = CommandPool::New(mGraphics);
      mCommandBuffer = mCommandPool->NewCommandBuffer(true);
    }
    else
    {
      mCommandBuffer->Reset();
    }

    mCommandBuffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    // change layout
    auto barrier = std::vector<vk::ImageMemoryBarrier>{mCommandBuffer->ImageLayoutTransitionBarrier(
      mImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor )};

    // change layout to prepare image to transfer data
    mCommandBuffer->PipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {barrier} );

    // copy image
    mCommandBuffer->CopyBufferToImage( buffer, mImage, vk::ImageLayout::eTransferDstOptimal, {copy} );

    // change layout to shader read-only optimal
    mCommandBuffer->PipelineBarrier(
      vk::PipelineStageFlagBits::eVertexShader,
      vk::PipelineStageFlagBits::eVertexShader,
      {},
      {},
      {},
      {mCommandBuffer->ImageLayoutTransitionBarrier(
        mImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor )} );

    mCommandBuffer->End();

    // submit and wait till image is uploaded so temporary buffer can be destroyed safely
    auto fence = Fence::New( mGraphics );
    mGraphics.GetGraphicsQueue( 0u ).Submit( mCommandBuffer, fence );
    fence->Wait();
    return true;
  }

  // creates image with preallocated memory and default sampler, no data
  // uploaded at this point
  bool Initialise()
  {
    // create image
    mImage = Image::New( mGraphics,
                         vk::ImageCreateInfo{}
                           .setFormat( mPixmap.pixelFormat )
                           .setInitialLayout( vk::ImageLayout::ePreinitialized )
                           .setSamples( vk::SampleCountFlagBits::e1 )
                           .setSharingMode( vk::SharingMode::eExclusive )
                           .setUsage( vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst )
                           .setExtent( {mPixmap.width, mPixmap.height, 1} )
                           .setArrayLayers( 1 )
                           .setImageType( vk::ImageType::e2D )
                           .setTiling( vk::ImageTiling::eOptimal )
                           .setMipLevels( 1 ) );
    // allocate memory and bind to the image
    auto& allocator = mGraphics.GetDeviceMemoryManager().GetDefaultAllocator();
    mImage->BindMemory( allocator.Allocate( mImage, vk::MemoryPropertyFlagBits::eDeviceLocal ) );

    // create default image view
    mImageView = ImageView::New( mGraphics, mImage );

    // create basic sampler
    CreateSampler();

    return true;
  }

  void CreateSampler()
  {
    // mutable sampler creation will be deferred until it's used
    mSampler = Sampler::New( mGraphics );
    mSampler->SetAddressMode( vk::SamplerAddressMode::eClampToEdge,
                              vk::SamplerAddressMode::eClampToEdge,
                              vk::SamplerAddressMode::eClampToEdge );
    mSampler->SetBorderColor( vk::BorderColor::eFloatOpaqueBlack );
    mSampler->SetCompareOp( vk::CompareOp::eNever );
    mSampler->SetFilter( vk::Filter::eLinear, vk::Filter::eLinear );
    mSampler->SetMipmapMode( vk::SamplerMipmapMode::eLinear );

    mSampler->GetVkHandle();
  }

  SamplerRef GetSampler() const
  {
    return mSampler;
  }

  Graphics&    mGraphics;
  ImageRef     mImage;
  ImageViewRef mImageView;
  SamplerRef   mSampler;

  // command pools should be 'per-thread' so they can be safely
  // used withing one single thread before submitting them
  CommandPoolRef   mCommandPool;
  CommandBufferRef mCommandBuffer; // primary buffer, executed independent

  // layouts
  vk::ImageLayout mOldLayout;
  vk::ImageLayout mNewLayout;

  uint32_t mWidth { 0u }, mHeight { 0u };
  vk::Format mFormat {};
  // Command pool
  Pixmap mPixmap;
};

/**
 * Texture
 *
 */

TextureRef Texture::New( Graphics& graphics, uint32_t width, uint32_t height, vk::Format format )
{
  auto result = TextureRef( new Texture( graphics, width, height, format ));
  if( !result->mImpl->Initialise() )
  {
    result.Reset();
  }
  return result;
}

Texture::Texture( Graphics& graphics, uint32_t width, uint32_t height, vk::Format format )
{
  mImpl.reset( new Impl( *this, graphics, width, height, format ) );
}


/**
 * Schedules data upload from CPU
 * @param data
 * @param size
 * @param mode
 */
void Texture::UploadData( const void* data, size_t size, TextureUploadMode mode )
{
  mImpl->UploadData( data, 0, U32(size) );
}

/**
 *
 * @param buffer
 * @param mode
 */
void Texture::UploadFromBuffer( BufferRef buffer, TextureUploadMode mode )
{

}

ImageRef Texture::GetImage() const
{
  return mImpl->mImage;
}

ImageViewRef Texture::GetImageView() const
{
  return mImpl->mImageView;
}

SamplerRef Texture::GetSampler() const
{
  return mImpl->mSampler;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
