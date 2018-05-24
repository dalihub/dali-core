#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-fence.h>
#include <dali/graphics/vulkan/vulkan-queue.h>

#include <dali/graphics/vulkan/vulkan-graphics-texture.h>


namespace VulkanTest
{
using namespace Dali::Graphics::Vulkan;

struct Pixel
{
  Pixel() = default;

  explicit Pixel( uint32_t _color )
    : color(_color)
  {}

  explicit Pixel( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
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
  explicit Pixmap( std::vector<Pixel> _data, uint32_t _width, uint32_t _height ) :
  data(_data), width(_width), height(_height), bytesPerPixel(4), pixelFormat( vk::Format::eR8G8B8A8Unorm )
  {
  }
  std::vector<Pixel> data;
  uint32_t          width;
  uint32_t          height;
  uint32_t          bytesPerPixel;
  vk::Format        pixelFormat;
};

struct Texture
{
  Texture( Graphics& graphics, Pixmap pixmap ) : mGraphics( graphics ), mPixmap( pixmap )
  {
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
    auto size = mPixmap.data.size()*sizeof(mPixmap.data[0]);
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
    auto copy = vk::BufferImageCopy{}.setImageExtent( { mPixmap.width, mPixmap.height, 1 } )
                         .setBufferImageHeight( mPixmap.height )
                         .setBufferOffset( 0 )
                         .setBufferRowLength( mPixmap.width )
                         .setImageOffset( { 0, 0, 0 } )
                         .setImageSubresource(
                           vk::ImageSubresourceLayers{}
                           .setMipLevel( 0 )
                           .setAspectMask( vk::ImageAspectFlagBits::eColor )
                           .setLayerCount( 1 )
                           .setBaseArrayLayer( 0 )
                         );
    mCommandPool = CommandPool::New( mGraphics );
    mCommandBuffer = mCommandPool->NewCommandBuffer( true );
    mCommandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    // change layout
    auto barrier = std::vector<vk::ImageMemoryBarrier>{
      mCommandBuffer->ImageLayoutTransitionBarrier( mImage, vk::ImageLayout::ePreinitialized, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor )
    };

    // change layout to prepare image to transfer data
    mCommandBuffer->PipelineBarrier( vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
                                     {}, {}, {}, { barrier } );

    // copy image
    mCommandBuffer->CopyBufferToImage( buffer, mImage, vk::ImageLayout::eTransferDstOptimal, { copy } );

    // change layout to shader read-only optimal
    mCommandBuffer->PipelineBarrier( vk::PipelineStageFlagBits::eVertexShader, vk::PipelineStageFlagBits::eVertexShader,
                                     {}, {}, {}, {
                                       mCommandBuffer->ImageLayoutTransitionBarrier( mImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor )
                                     } );

    mCommandBuffer->End();

    // submit and wait till image is uploaded so temporary buffer can be destroyed safely
    auto fence = mGraphics.CreateFence({});
    mGraphics.GetGraphicsQueue(0u).Submit( mCommandBuffer, fence );
    mGraphics.WaitForFence(fence);

  }

  Graphics&        mGraphics;
  RefCountedImage         mImage;
  RefCountedImageView     mImageView;

  RefCountedCommandPool    mCommandPool;
  RefCountedCommandBuffer  mCommandBuffer; // primary buffer, executed independent

  // layouts
  vk::ImageLayout mOldLayout;
  vk::ImageLayout mNewLayout;

  // Command pool


  Pixmap mPixmap;
};

/**
 * Generates a random pixmap
 * @param width
 * @param height
 * @return
 */
Pixmap GenerateTexture32BPPRGBA( uint32_t width, uint32_t height )
{
  std::vector<Pixel> data;
  data.resize( width * height );

  const Pixel WHITE     { 0xffffffff };
  const Pixel BLACK     { 0x000000ff };
  const Pixel RED       { 0xff0000ff };
  const Pixel GREEN     { 0x00ff00ff };
  const Pixel BLUE      { 0x0000ffff };

  const Pixel COLORS[] = { WHITE, BLACK, RED, GREEN, BLUE };
  const auto COLORS_COUNT = 5u;

  for( auto y = 0u; y < height; ++y )
  {
    for( auto x = 0u; x < width; ++x )
    {
      data[ x + width * y ] = COLORS[ ((x*x)+(y*y)) % COLORS_COUNT];
    }
  }

  return Pixmap{ data, width, height };
}

Pixmap LoadResourceImage( void* data, uint32_t width, uint32_t height, uint32_t bytesPerPixel )
{
  // todo: create Pixmap object
}

Texture CreateTexture( Graphics& graphics, Pixmap pixmap )
{
  return Texture( graphics, pixmap );
}

int TextureTestMain( Dali::Graphics::Vulkan::Graphics& graphics )
{
  auto pixmap = GenerateTexture32BPPRGBA( 1024, 1024 );
  auto texture = Dali::Graphics::Vulkan::Texture::New( graphics, 1024, 1024, vk::Format::eR8G8B8A8Unorm);

  return 0;
}

} // namespace VulkanTest