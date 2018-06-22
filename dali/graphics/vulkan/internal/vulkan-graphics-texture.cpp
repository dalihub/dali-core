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

#include <dali/graphics/vulkan/internal/vulkan-graphics-texture.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/internal/vulkan-queue.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
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
  explicit Pixmap( uint32_t _width, uint32_t _height, vk::Format format )
          : width( _width ), height( _height ), bytesPerPixel( 1 ), pixelFormat( format )
  {
    totalSizeInBytes = width * height * bytesPerPixel;
    data.resize( totalSizeInBytes );
  }

  explicit Pixmap( uint32_t _width, uint32_t _height )
          : width( _width ), height( _height ), bytesPerPixel( 4 ), pixelFormat( vk::Format::eR8G8B8A8Unorm )
  {
    data.resize( _width * _height );
  }

  std::vector< uint8_t > data;
  uint32_t width;
  uint32_t height;
  uint32_t bytesPerPixel;
  uint32_t totalSizeInBytes;
  vk::Format pixelFormat;
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
    auto size = sizeInBytes;
    auto buffer = mGraphics.CreateBuffer( vk::BufferCreateInfo{}
                                                  .setUsage( vk::BufferUsageFlagBits::eTransferSrc )
                                                  .setSharingMode( vk::SharingMode::eExclusive )
                                                  .setSize( size ) );

    buffer->BindMemory( allocator.Allocate( buffer, vk::MemoryPropertyFlagBits::eHostVisible |
                                                    vk::MemoryPropertyFlagBits::eHostCoherent ) );

    // copy pixels to the buffer
    auto ptr = buffer->GetMemoryHandle()->MapTyped< char >();
    std::copy( reinterpret_cast<const char*>(data),
               reinterpret_cast<const char*>(data) + sizeInBytes,
               ptr );

    buffer->GetMemoryHandle()->Unmap();

    // record copy and layout change
    auto copy = vk::BufferImageCopy{}
            .setImageExtent( { mWidth, mHeight, 1 } )
            .setBufferImageHeight( mHeight )
            .setBufferOffset( 0 )
            .setBufferRowLength( mWidth )
            .setImageOffset( { 0, 0, 0 } )
            .setImageSubresource( vk::ImageSubresourceLayers{}
                                          .setMipLevel( 0 )
                                          .setAspectMask( mImage->GetAspectFlags() )
                                          .setLayerCount( 1 )
                                          .setBaseArrayLayer( 0 ) );

    auto commandBuffer = mGraphics.CreateCommandBuffer( true );

    commandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );

    // change layout to prepare image to transfer data
    commandBuffer->PipelineBarrier( vk::PipelineStageFlagBits::eTopOfPipe,
                                    vk::PipelineStageFlagBits::eTransfer,
                                    {},
                                    {},
                                    {},
                                    { mGraphics.CreateImageMemoryBarrier( mImage,
                                                                          mImage->GetImageLayout(),
                                                                          vk::ImageLayout::eTransferDstOptimal ) } );

    // copy image
    commandBuffer->CopyBufferToImage( buffer, mImage, vk::ImageLayout::eTransferDstOptimal, { copy } );

    // change layout to shader read-only optimal
    commandBuffer->PipelineBarrier(
            vk::PipelineStageFlagBits::eVertexShader,
            vk::PipelineStageFlagBits::eVertexShader,
            {},
            {},
            {},
            { mGraphics.CreateImageMemoryBarrier( mImage,
                                                  vk::ImageLayout::eTransferDstOptimal,
                                                  vk::ImageLayout::eShaderReadOnlyOptimal ) } );

    commandBuffer->End();

    // submit and wait till image is uploaded so temporary buffer can be destroyed safely
    auto fence = mGraphics.CreateFence( {} );
    VkAssert( mGraphics.Submit( mGraphics.GetGraphicsQueue( 0u ),
                                { SubmissionData{}.SetCommandBuffers( { commandBuffer } ) }, fence ) );
    VkAssert( mGraphics.WaitForFence( fence, std::numeric_limits< uint32_t >::max() ) );

    // Update the image with its new layout
    mImage->SetImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal );

    return true;
  }

  // creates image with pre-allocated memory and default sampler, no data
  // uploaded at this point
  bool Initialise()
  {
    // create image
    auto imageCreateInfo = vk::ImageCreateInfo{}
            .setFormat( mPixmap.pixelFormat )
            .setInitialLayout( vk::ImageLayout::ePreinitialized )
            .setSamples( vk::SampleCountFlagBits::e1 )
            .setSharingMode( vk::SharingMode::eExclusive )
            .setUsage( vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst )
            .setExtent( { mPixmap.width, mPixmap.height, 1 } )
            .setArrayLayers( 1 )
            .setImageType( vk::ImageType::e2D )
            .setTiling( vk::ImageTiling::eOptimal )
            .setMipLevels( 1 );

    // Create the image handle
    mImage = mGraphics.CreateImage( imageCreateInfo );

    // allocate memory for the image
    auto memory = mGraphics.GetDeviceMemoryManager()
                           .GetDefaultAllocator()
                           .Allocate( mImage, vk::MemoryPropertyFlagBits::eDeviceLocal );

    // bind the allocated memory to the image
    mGraphics.BindImageMemory( mImage, memory, 0 );

    // create default image view
    mImageView = mGraphics.CreateImageView( mImage );

    // create basic sampler
    CreateSampler();

    return true;
  }

  void CreateSampler()
  {
    auto samplerCreateInfo = vk::SamplerCreateInfo()
            .setAddressModeU( vk::SamplerAddressMode::eClampToEdge )
            .setAddressModeV( vk::SamplerAddressMode::eClampToEdge )
            .setAddressModeW( vk::SamplerAddressMode::eClampToEdge )
            .setBorderColor( vk::BorderColor::eFloatOpaqueBlack )
            .setCompareOp( vk::CompareOp::eNever )
            .setMinFilter( vk::Filter::eLinear )
            .setMagFilter( vk::Filter::eLinear )
            .setMipmapMode( vk::SamplerMipmapMode::eLinear );

    mSampler = mGraphics.CreateSampler( samplerCreateInfo );
  }

  RefCountedSampler GetSampler() const
  {
    return mSampler;
  }

  Graphics& mGraphics;
  RefCountedImage mImage;
  RefCountedImageView mImageView;
  RefCountedSampler mSampler;

  // layouts
  vk::ImageLayout mOldLayout;
  vk::ImageLayout mNewLayout;

  uint32_t mWidth{ 0u }, mHeight{ 0u };
  vk::Format mFormat{};
  // Command pool
  Pixmap mPixmap;
};

/**
 * Texture
 *
 */

RefCountedTexture Texture::New( Graphics& graphics, uint32_t width, uint32_t height, vk::Format format )
{
  auto result = RefCountedTexture( new Texture( graphics, width, height, format ) );
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
  mImpl->UploadData( data, 0, U32( size ) );
}

/**
 *
 * @param buffer
 * @param mode
 */
void Texture::UploadFromBuffer( RefCountedBuffer buffer, TextureUploadMode mode )
{

}

RefCountedImage Texture::GetImage() const
{
  return mImpl->mImage;
}

RefCountedImageView Texture::GetImageView() const
{
  return mImpl->mImageView;
}

RefCountedSampler Texture::GetSampler() const
{
  return mImpl->mSampler;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
