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

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
using namespace Dali::Graphics::Vulkan;

/**
 * Helper structure, temporary
 */
struct Pixel
{
  Pixel() = default;

  explicit Pixel( uint32_t _color ) : color( _color )
  {
  }

  explicit Pixel( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a )
  {
    r = _r;
    g = _g;
    b = _b;
    a = _a;
  }

  union
  {
    uint32_t color;
    struct
    {
      uint8_t r, g, b, a;
    };
  };
};

struct Pixmap
{
  explicit Pixmap( std::vector<Pixel> _data, uint32_t _width, uint32_t _height )
  : data( _data ), width( _width ), height( _height ), bytesPerPixel( 4 ), pixelFormat( vk::Format::eR8G8B8A8Unorm )
  {
  }

  explicit Pixmap( uint32_t _width, uint32_t _height )
  : width( _width ), height( _height ), bytesPerPixel( 4 ), pixelFormat( vk::Format::eR8G8B8A8Unorm )
  {
    data.resize( _width * _height );
  }

  std::vector<Pixel> data;
  uint32_t           width;
  uint32_t           height;
  uint32_t           bytesPerPixel;
  vk::Format         pixelFormat;
};

/**
 * Texture::Impl
 * Responsible for creating an image, basic image view, allocating texture memory,
 * transfering the data into into the texture
 *
 */
struct Texture::Impl
{
  Impl( Graphics& graphics, Pixmap pixmap ) : mGraphics( graphics ), mPixmap( pixmap )
  {
  }

  Impl( Texture& owner, Graphics& graphics, uint32_t width, uint32_t height, vk::Format format ) : mGraphics( graphics ),
                                                                                                   mPixmap( width, height )
  {
  }

  bool UploadData( void* data, uint32_t offsetInBytes, uint32_t sizeInBytes )
  {
    // create buffer
    auto& allocator = mGraphics.GetDeviceMemoryManager().GetDefaultAllocator();
    auto size   = mPixmap.data.size() * sizeof( mPixmap.data[0] );
    auto buffer = Buffer::New( mGraphics,
                               vk::BufferCreateInfo{}
                                 .setUsage( vk::BufferUsageFlagBits::eTransferSrc )
                                 .setSharingMode( vk::SharingMode::eExclusive )
                                 .setSize( size ) );

    buffer->BindMemory( allocator.Allocate( buffer, vk::MemoryPropertyFlagBits::eHostVisible ) );

    // copy pixels to the buffer
    auto ptr = buffer->GetMemoryHandle()->MapTyped<char>();
    std::copy( reinterpret_cast<char*>(mPixmap.data.data()),
               reinterpret_cast<char*>(mPixmap.data.data() + mPixmap.data.size()), ptr );
    buffer->GetMemoryHandle()->Unmap();

    // record copy and layout change
    auto copy = vk::BufferImageCopy{}
      .setImageExtent( {mPixmap.width, mPixmap.height, 1} )
      .setBufferImageHeight( mPixmap.height )
      .setBufferOffset( 0 )
      .setBufferRowLength( mPixmap.width )
      .setImageOffset( {0, 0, 0} )
      .setImageSubresource( vk::ImageSubresourceLayers{}
                              .setMipLevel( 0 )
                              .setAspectMask( vk::ImageAspectFlagBits::eColor )
                              .setLayerCount( 1 )
                              .setBaseArrayLayer( 0 ) );

    mCommandBuffer.Reset();


  }

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

    // create transient buffer to copy data
    auto size   = mPixmap.data.size() * sizeof( mPixmap.data[0] );
    auto buffer = Buffer::New( mGraphics,
                               vk::BufferCreateInfo{}
                                 .setUsage( vk::BufferUsageFlagBits::eTransferSrc )
                                 .setSharingMode( vk::SharingMode::eExclusive )
                                 .setSize( size ) );

    buffer->BindMemory( allocator.Allocate( buffer, vk::MemoryPropertyFlagBits::eHostVisible ) );

    // copy pixels to the buffer
    auto ptr = buffer->GetMemoryHandle()->MapTyped<Pixel>();
    std::copy( mPixmap.data.begin(), mPixmap.data.end(), ptr );
    buffer->GetMemoryHandle()->Unmap();

    // record copy and layout change
    auto copy = vk::BufferImageCopy{}
                  .setImageExtent( {mPixmap.width, mPixmap.height, 1} )
                  .setBufferImageHeight( mPixmap.height )
                  .setBufferOffset( 0 )
                  .setBufferRowLength( mPixmap.width )
                  .setImageOffset( {0, 0, 0} )
                  .setImageSubresource( vk::ImageSubresourceLayers{}
                                          .setMipLevel( 0 )
                                          .setAspectMask( vk::ImageAspectFlagBits::eColor )
                                          .setLayerCount( 1 )
                                          .setBaseArrayLayer( 0 ) );
    mCommandPool   = CommandPool::New( mGraphics );
    mCommandBuffer = mCommandPool->NewCommandBuffer( true );
    mCommandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );

    // change layout
    auto barrier = std::vector<vk::ImageMemoryBarrier>{mCommandBuffer->ImageLayoutTransitionBarrier(
      mImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor )};

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
        mImage, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor )} );

    mCommandBuffer->End();

    // submit and wait till image is uploaded so temporary buffer can be destroyed safely
    auto fence = Fence::New( mGraphics );
    mGraphics.GetGraphicsQueue( 0u ).Submit( mCommandBuffer, fence );
    fence->Wait();

    // create basic sampler
    CreateSampler();
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

    mSampler->GetVkSampler();
  }

  SamplerRef GetSampler() const
  {
    return mSampler;
  }

  Graphics&    mGraphics;
  ImageRef     mImage;
  ImageViewRef mImageView;
  SamplerRef   mSampler;

  CommandPoolRef   mCommandPool;
  CommandBufferRef mCommandBuffer; // primary buffer, executed independent

  // layouts
  vk::ImageLayout mOldLayout;
  vk::ImageLayout mNewLayout;

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
void Texture::UploadData( void* data, size_t size, TextureUploadMode mode )
{

}

/**
 *
 * @param buffer
 * @param mode
 */
void Texture::UploadFromBuffer( BufferRef buffer, TextureUploadMode mode )
{

}

/**
 * Returns Image object
 * @return
 */
ImageRef Texture::GetImage() const
{

}


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali