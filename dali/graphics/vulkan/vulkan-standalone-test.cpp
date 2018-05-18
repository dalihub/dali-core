/*
 * This is the test code that allows to run Vulkan backend
 * as standalone application. Supports both Xcb and Xlib window
 * integration. MUST NOT BE COMPILED WITH DALI!
 */

// for surface implementation
#ifndef VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#ifndef VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vector_relational.hpp>

using namespace glm;

#include <dali/integration-api/graphics/graphics.h>
#include <dali/integration-api/graphics/vulkan/vk-surface-factory.h>
#include <dali/integration-api/graphics/vulkan/vulkan-hpp-wrapper.h>

#include <unistd.h>
#include <xcb/xcb.h>

// internals
#include "generated/spv-shaders-gen.h"

#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>

#define USE_XLIB 0
#include <iostream>

using Dali::Graphics::Vulkan::Buffer;
using Dali::Graphics::Vulkan::CommandBuffer;
using Dali::Graphics::Vulkan::CommandPool;
using Dali::Graphics::Vulkan::DescriptorPool;
using Dali::Graphics::Vulkan::GpuMemoryAllocator;
using Dali::Graphics::Vulkan::GpuMemoryManager;
using Dali::Graphics::Vulkan::Pipeline;
using Dali::Graphics::Vulkan::RefCountedPipeline;
using Dali::Graphics::Vulkan::Shader;
using Dali::Graphics::Vulkan::RefCountedShader;
using Dali::Integration::Graphics::Graphics;
using Dali::Integration::Graphics::Vulkan::VkSurfaceFactory;

extern std::vector<uint8_t> VSH;
extern std::vector<uint8_t> FSH;

template<typename T, typename... Args>
std::unique_ptr<T> MakeUnique( Args&&... args )
{
  return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
}

class VkSurfaceXlib : public Dali::Integration::Graphics::Vulkan::VkSurfaceFactory
{
public:
  /**
   * Instantiates surface factory ( should
   * @param display
   * @param window
   */
  VkSurfaceXlib( Display* display, Window window ) : VkSurfaceFactory(), mDisplay( display ), mWindow( window )
  {
  }

  virtual vk::SurfaceKHR Create( vk::Instance             instance,
                                 vk::AllocationCallbacks* allocCallbacks,
                                 vk::PhysicalDevice       physicalDevice ) const override
  {
    vk::XlibSurfaceCreateInfoKHR info;
    info.setDpy( mDisplay ).setWindow( mWindow );
    auto retval = instance.createXlibSurfaceKHR( info, allocCallbacks ).value;
    return retval;
  }

private:
  Display*       mDisplay;
  Window         mWindow;
  vk::SurfaceKHR mSurface;
};

class VkSurfaceXcb : public Dali::Integration::Graphics::Vulkan::VkSurfaceFactory
{
public:
  /**
   * Instantiates surface factory ( should
   * @param display
   * @param window
   */
  VkSurfaceXcb( xcb_connection_t* connection, xcb_window_t window )
  : VkSurfaceFactory{}, mConnection( connection ), mWindow( window )
  {
  }

  virtual vk::SurfaceKHR Create( vk::Instance             instance,
                                 vk::AllocationCallbacks* allocCallbacks,
                                 vk::PhysicalDevice       physicalDevice ) const override
  {
    vk::XcbSurfaceCreateInfoKHR info;
    info.setConnection( mConnection ).setWindow( mWindow );
    auto retval = instance.createXcbSurfaceKHR( info, allocCallbacks ).value;
    return retval;
  }

private:
  xcb_connection_t* mConnection;
  xcb_window_t      mWindow;
  vk::SurfaceKHR    mSurface;
};

namespace Test
{
struct xlib_window_t
{
  uint32_t width{0u};
  uint32_t height{0u};
  Window   window{};
  Display* display{nullptr};

  ~xlib_window_t()
  {
    XDestroyWindow( display, window );
  }
};

std::unique_ptr<xlib_window_t> create_xlib_window( int width, int height )
{
  std::unique_ptr<xlib_window_t> wnd{new xlib_window_t};
  // 1. Create Window ( done by DALI

  wnd->width         = width;
  wnd->height        = height;
  wnd->display       = XOpenDisplay( nullptr );
  auto defaultScreen = DefaultScreen( wnd->display );
  wnd->window        = XCreateSimpleWindow( wnd->display,
                                     RootWindow( wnd->display, defaultScreen ),
                                     0,
                                     0,
                                     wnd->width,
                                     wnd->height,
                                     1,
                                     BlackPixel( wnd->display, defaultScreen ),
                                     WhitePixel( wnd->display, defaultScreen ) );

  XSelectInput( wnd->display, wnd->window, ExposureMask | KeyPressMask );
  XMapWindow( wnd->display, wnd->window );
  XSync( wnd->display, false );

  return wnd;
}

struct xcb_window_t
{
  uint32_t          width{0u};
  uint32_t          height{0u};
  ::xcb_window_t    window;
  xcb_connection_t* connection;

  ~xcb_window_t()
  {
    xcb_destroy_window( connection, window );
  }
};

std::unique_ptr<Test::xcb_window_t> create_xcb_window( int width, int height )
{
  std::unique_ptr<Test::xcb_window_t> wnd{new Test::xcb_window_t};
  // 1. Create Window ( done by DALI

  wnd->width  = width;
  wnd->height = height;

  int screenNum( 0 );

  xcb_connection_t*     connection = xcb_connect( NULL, &screenNum );
  const xcb_setup_t*    setup      = xcb_get_setup( connection );
  xcb_screen_iterator_t iter       = xcb_setup_roots_iterator( setup );
  for( int i = 0; i < screenNum; ++i )
    xcb_screen_next( &iter );

  xcb_screen_t*  screen = iter.data;
  ::xcb_window_t window = xcb_generate_id( connection );

  uint32_t mask     = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t values[] = {screen->white_pixel, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS};

  xcb_create_window( connection,
                     XCB_COPY_FROM_PARENT,
                     window,
                     screen->root,
                     0,
                     0,
                     wnd->width,
                     wnd->height,
                     0,
                     XCB_WINDOW_CLASS_INPUT_OUTPUT,
                     screen->root_visual,
                     mask,
                     values );

  xcb_map_window( connection, window );
  const uint32_t coords[] = {100, 100};
  xcb_configure_window( connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords );
  xcb_flush( connection );

  wnd->connection = connection;
  wnd->window     = window;

  return wnd;
}
} // namespace Test

namespace VulkanTest
{
Dali::Graphics::Vulkan::RefCountedGpuMemoryBlock test_gpu_memory_manager(
  Dali::Graphics::Vulkan::Graphics &graphics,
  GpuMemoryManager &gpuManager,
  const Dali::Graphics::Vulkan::Handle<Buffer> &buffer)
{
  auto device = graphics.GetDevice();
  auto &allocator = graphics.GetAllocator();

  auto &gpuAllocator = gpuManager.GetDefaultAllocator();
  return gpuAllocator.Allocate(buffer, vk::MemoryPropertyFlagBits::eHostVisible);
}

struct UniformData
{
  mat4 mvp;
  vec4 color;
  vec3 size;
} __attribute__(( aligned( 16 )));

struct UniformClipData
{
  mat4 clip;
} __attribute__(( aligned( 16 )));

mat4 MVP;

template<class T>
void update_buffer(Dali::Graphics::Vulkan::RefCountedBuffer buffer, T &value)
{
  auto ptr = reinterpret_cast<T *>( buffer->GetMemoryHandle()
                                          ->Map());
  *ptr = value;
  buffer->GetMemoryHandle()
        ->Unmap();
}

void update_translation(Dali::Graphics::Vulkan::RefCountedBuffer buffer)
{
  static float x = 0.0f;
  x += 0.5f;

  /*
  UniformData ub;

  auto modelMat = glm::translate( mat4{1}, vec3( x, x, 0.0f ) );
  modelMat      = glm::rotate( modelMat, glm::radians( x ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

  ub.mvp = ortho( 0.0f, 640.0f, 480.0f, 0.0f, 0.0f, 100.0f ) *
           lookAt( vec3( 0.0f, 0.0f, 10.0f ), vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 1.0f, 0.0f ) ) * modelMat;

  update_buffer( buffer, ub );
  */
}

Dali::Graphics::Vulkan::RefCountedBuffer create_uniform_buffer(Dali::Graphics::Vulkan::Graphics &gr)
{
  // create uniform buffer
  auto uniformBuffer = Buffer::New(gr, sizeof(UniformData), Buffer::Type::UNIFORM);

  // allocate memory
  auto memory = gr.GetDeviceMemoryManager()
                  .GetDefaultAllocator()
                  .Allocate(uniformBuffer,
                            vk::MemoryPropertyFlagBits::eHostVisible);

  // bind memory
  uniformBuffer->BindMemory(memory);

  auto ub = reinterpret_cast<UniformData *>( memory->Map());

  ub->mvp   = mat4{1.0f} * ortho(0.0f, 640.0f, 480.0f, 0.0f, 0.0f, 100.0f) *
              lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
  ub->color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
  ub->size  = vec3(1.0f, 1.0f, 1.0f);

  memory->Unmap();

  return uniformBuffer;
}

Dali::Graphics::Vulkan::RefCountedBuffer create_clip_buffer(Dali::Graphics::Vulkan::Graphics &gr)
{
  const glm::mat4 clip(
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f);

  // create uniform buffer
  auto uniformBuffer = Buffer::New(gr, sizeof(UniformClipData), Buffer::Type::UNIFORM);

  // allocate memory
  auto memory = gr.GetDeviceMemoryManager()
                  .GetDefaultAllocator()
                  .Allocate(uniformBuffer,
                            vk::MemoryPropertyFlagBits::eHostVisible);
  // bind memory
  uniformBuffer->BindMemory(memory);
  auto dst = memory->MapTyped<mat4>();
  std::copy(&clip, &clip + 1, dst);
  memory->Unmap();
  return uniformBuffer;
}

Dali::Graphics::Vulkan::Handle<DescriptorPool> create_descriptor_pool(Dali::Graphics::Vulkan::Graphics &gr)
{
  vk::DescriptorPoolSize size;
  size.setDescriptorCount(1024)
      .setType(vk::DescriptorType::eUniformBuffer);

  // TODO: how to organize this???
  auto pool = DescriptorPool::New(
    gr, vk::DescriptorPoolCreateInfo{}.setMaxSets(1024)
                                      .setPoolSizeCount(1)
                                      .setPPoolSizes(&size));
  return pool;
}

void test_framebuffer(Dali::Graphics::Vulkan::Graphics &graphics)
{
  using namespace Dali::Graphics::Vulkan;
  // framebuffer
  auto fb = NewRef<Framebuffer>(graphics, 640, 480);

  // attachment
  auto image = NewRef<Image>(graphics,
                             vk::ImageCreateInfo{}
                               .setFormat(vk::Format::eR32G32B32A32Sfloat)
                               .setTiling(vk::ImageTiling::eOptimal)
                               .setMipLevels(1)
                               .setImageType(vk::ImageType::e2D)
                               .setExtent(vk::Extent3D(640, 480, 1))
                               .setArrayLayers(1)
                               .setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment)
                               .setSharingMode(vk::SharingMode::eExclusive)
                               .setInitialLayout(vk::ImageLayout::eUndefined)
                               .setSamples(vk::SampleCountFlagBits::e1));

  auto imageView = NewRef<ImageView>(
    graphics,
    image,
    vk::ImageViewCreateInfo{}
      .setFormat(vk::Format::eR32G32B32A32Sfloat)
      .setComponents(vk::ComponentMapping(
        vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA))
      .setImage(image->GetVkHandle())
      .setSubresourceRange(vk::ImageSubresourceRange{}
                             .setLayerCount(1)
                             .setLevelCount(1)
                             .setBaseMipLevel(0)
                             .setBaseArrayLayer(0)
                             .setAspectMask(vk::ImageAspectFlagBits::eColor))
      .setViewType(vk::ImageViewType::e2D));

  fb->SetAttachment(imageView, Framebuffer::AttachmentType::COLOR, 0u);

  //fb->GetRenderPass();

  //fb->SetAttachment( )
}

void test_handle()
{
  /*
  using namespace Dali::Graphics::Vulkan;
  RefCountedGpuMemoryBlock handle( new GpuMemoryBlock() );

  decltype(handle) handle2 = handle;
  handle.GetRefCount();*/
}

RefCountedPipeline create_pipeline(Dali::Graphics::Vulkan::Graphics &graphics,
                            Dali::Graphics::Vulkan::RefCountedShader vertexShader,
                            Dali::Graphics::Vulkan::RefCountedShader fragmentShader)
{
  using namespace Dali::Graphics::Vulkan;
  auto pipelineInfo = vk::GraphicsPipelineCreateInfo{};
  auto pipeline     = Pipeline::New(graphics, pipelineInfo);

  pipeline->SetShader(vertexShader, Shader::Type::VERTEX);
  pipeline->SetShader(fragmentShader, Shader::Type::FRAGMENT);
  pipeline->SetViewport(0, 0, 640, 480);
  pipeline->SetVertexInputState(
    std::vector<vk::VertexInputAttributeDescription>{
      vk::VertexInputAttributeDescription{}.setBinding(0)
                                           .setOffset(0)
                                           .setLocation(0)
                                           .setFormat(
                                             vk::Format::eR32G32B32Sfloat)},
    std::vector<vk::VertexInputBindingDescription>{vk::VertexInputBindingDescription{}
      .setBinding(0)
      .setStride(sizeof(float) * 3)
      .setInputRate(vk::VertexInputRate::eVertex)});
  pipeline->SetInputAssemblyState(vk::PrimitiveTopology::eTriangleList, false);

  if (!pipeline->Compile())
  {
    pipeline.Reset();
  }
  return pipeline;
}

int TextureTestMain( Dali::Graphics::Vulkan::Graphics& );

int RunTestMain2()
{
#if USE_XLIB == 1
  auto window         = Test::create_xlib_window( 640, 480 );
  auto surfaceFactory = std::unique_ptr<VkSurfaceXlib>{new VkSurfaceXlib{window->display, window->window}};
#else
  auto window         = Test::create_xcb_window(640, 480);
  auto surfaceFactory = std::unique_ptr<VkSurfaceXcb>{new VkSurfaceXcb{window->connection, window->window}};
#endif

  auto graphics = MakeUnique<Graphics>();
  auto fbid     = graphics->Create(std::move(surfaceFactory));

  // access internal implementation
  auto &gr = graphics->GetImplementation<Dali::Graphics::Vulkan::Graphics>();

  // GPU memory manager
  auto &memmgr = gr.GetDeviceMemoryManager();

  return TextureTestMain( gr );
}

int RunTestMain()
{
#if USE_XLIB == 1
  auto window         = Test::create_xlib_window( 640, 480 );
  auto surfaceFactory = std::unique_ptr<VkSurfaceXlib>{new VkSurfaceXlib{window->display, window->window}};
#else
  auto window         = Test::create_xcb_window(640, 480);
  auto surfaceFactory = std::unique_ptr<VkSurfaceXcb>{new VkSurfaceXcb{window->connection, window->window}};
#endif

  auto graphics = MakeUnique<Graphics>();
  auto fbid     = graphics->Create(std::move(surfaceFactory));

  // access internal implementation
  auto &gr = graphics->GetImplementation<Dali::Graphics::Vulkan::Graphics>();

  // GPU memory manager
  auto &memmgr = gr.GetDeviceMemoryManager();

  const vec3 VERTICES[] = {
    {0.0f,   0.0f,   0.0f},
    {320.0f, 0.0f,   0.0f},
    {0.0f,   160.0f, 0.0f},
  };

  // shaders
  auto vertexShader = Shader::New(gr, VSH_CODE.data(), VSH_CODE.size());

  auto fragmentShader = Shader::New(gr, FSH_CODE.data(), FSH_CODE.size());

  // buffer
  auto vertexBuffer = Buffer::New(gr, sizeof(float) * 3 * 3, Buffer::Type::VERTEX);

  auto descriptorPool = create_descriptor_pool(gr);

  auto &gpuManager = gr.GetDeviceMemoryManager();

  auto bufferMemory = test_gpu_memory_manager(gr, gpuManager, vertexBuffer);
  vertexBuffer->BindMemory(bufferMemory);

  auto ptr = static_cast<uint8_t *>( bufferMemory->Map());
  std::copy(reinterpret_cast<const uint8_t *>( VERTICES ),
            reinterpret_cast<const uint8_t *>( VERTICES ) + (sizeof(float) * 9),
            ptr);
  bufferMemory->Unmap();

  auto pipeline = create_pipeline(gr, vertexShader, fragmentShader);

  auto descriptorSet = descriptorPool->AllocateDescriptorSets(
    vk::DescriptorSetAllocateInfo{}
      .setPSetLayouts(pipeline->GetVkDescriptorSetLayouts()
                              .data())
      .setDescriptorSetCount(pipeline->GetVkDescriptorSetLayouts()
                                     .size()));


  auto commandPool = CommandPool::New(gr);

  auto uniformBuffer = create_uniform_buffer(gr);

  auto clipBuffer = create_clip_buffer(gr);

  descriptorSet[0]->WriteUniformBuffer(0, uniformBuffer, 0, uniformBuffer->GetSize());
  descriptorSet[0]->WriteUniformBuffer(1, clipBuffer, 0, clipBuffer->GetSize());

  // get new buffer
  auto cmdDraw = commandPool->NewCommandBuffer(false);

  // begin recording
  cmdDraw->Begin(vk::CommandBufferUsageFlagBits::eRenderPassContinue);

  // vertex buffer
  cmdDraw->BindVertexBuffer(0, vertexBuffer, 0);

  // pipeline
  cmdDraw->BindGraphicsPipeline(pipeline);

  // descriptor sets
  cmdDraw->BindDescriptorSets(descriptorSet, 0);

  // do draw
  cmdDraw->Draw(3, 1, 0, 0);

  // finish
  cmdDraw->End();

  bool running = true;

  while (running)
  {
    graphics->PreRender(fbid);
    // queue submit draw

    auto cmdbuf = gr.GetSwapchainForFBID(fbid)
                    ->GetPrimaryCommandBuffer();

    // get command buffer for current frame and execute the draw call
    cmdbuf->ExecuteCommands({cmdDraw});

    graphics->PostRender(fbid);

    update_translation(uniformBuffer);
  }
  return 0;
}

void texture_test(void *data, size_t size)
{

}

using namespace Dali::Graphics::Vulkan::SpirV;
void spirv_test0( std::vector<SPIRVWord> code )
{
  auto shader = SPIRVUtils::Parse( code, vk::ShaderStageFlagBits::eVertex );
  auto opcodeCount = shader->GetOpCodeCount();
  std::cout << "opcodecount" << opcodeCount << std::endl;

  auto layoutCreateInfo = shader->GenerateDescriptorSetLayoutCreateInfo();

  std::cout<< "yay!" <<std::endl;

}

void RunSPIRVTest()
{
  std::vector<SPIRVWord> data;
  data.resize( VSH_CODE.size()/4 );
  std::copy( VSH_CODE.begin(), VSH_CODE.end(), reinterpret_cast<decltype(VSH_CODE.data())>(data.data()) );
  spirv_test0( data );
}



} // namespace VulkanTest

int main()
{
  VulkanTest::RunTestMain2();

  //VulkanTest::RunSPIRVTest();
}
