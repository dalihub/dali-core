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

#include <dali/integration-api/graphics/vulkan/vulkan-hpp-wrapper.h>

#include <dali/integration-api/graphics/vulkan/vk-surface-factory.h>
#include <dali/integration-api/graphics/graphics.h>
#include <xcb/xcb.h>
#include <unistd.h>

// internals
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>

#define USE_XLIB 0

using Dali::Integration::Graphics::Graphics;
using Dali::Integration::Graphics::Vulkan::VkSurfaceFactory;
using Dali::Graphics::Vulkan::Buffer;
using Dali::Graphics::Vulkan::DeviceMemory;
using Dali::Graphics::Vulkan::Shader;
using Dali::Graphics::Vulkan::DescriptorSetLayout;
using Dali::Graphics::Vulkan::GpuMemoryManager;
using Dali::Graphics::Vulkan::GpuMemoryAllocator;

extern std::vector<uint32_t> VSH;
extern std::vector<uint32_t> FSH;

template< typename T, typename... Args >
std::unique_ptr< T > MakeUnique(Args&&... args)
{
  return std::unique_ptr< T >(new T(std::forward< Args >(args)...));
}


class VkSurfaceXlib : public Dali::Integration::Graphics::Vulkan::VkSurfaceFactory
{
public:
  /**
   * Instantiates surface factory ( should
   * @param display
   * @param window
   */
  VkSurfaceXlib(Display* display, Window window)
    : VkSurfaceFactory(), mDisplay(display), mWindow(window)
  {
  }

  virtual vk::SurfaceKHR Create(vk::Instance instance, vk::AllocationCallbacks* allocCallbacks,
                                vk::PhysicalDevice physicalDevice) const override
  {
    vk::XlibSurfaceCreateInfoKHR info;
    info.setDpy(mDisplay).setWindow(mWindow);
    auto retval = instance.createXlibSurfaceKHR(info, allocCallbacks).value;
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
  VkSurfaceXcb(xcb_connection_t* connection, xcb_window_t window)
    : VkSurfaceFactory{}, mConnection(connection), mWindow(window)
  {
  }

  virtual vk::SurfaceKHR Create(vk::Instance instance, vk::AllocationCallbacks* allocCallbacks,
                                vk::PhysicalDevice physicalDevice) const override
  {
    vk::XcbSurfaceCreateInfoKHR info;
    info.setConnection(mConnection).setWindow(mWindow);
    auto retval = instance.createXcbSurfaceKHR(info, allocCallbacks).value;
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
    XDestroyWindow(display, window);
  }
};

std::unique_ptr< xlib_window_t > create_xlib_window(int width, int height)
{
  std::unique_ptr< xlib_window_t > wnd{new xlib_window_t};
  // 1. Create Window ( done by DALI

  wnd->width         = width;
  wnd->height        = height;
  wnd->display       = XOpenDisplay(nullptr);
  auto defaultScreen = DefaultScreen(wnd->display);
  wnd->window =
    XCreateSimpleWindow(wnd->display, RootWindow(wnd->display, defaultScreen), 0, 0, wnd->width,
                        wnd->height, 1, BlackPixel(wnd->display, defaultScreen),
                        WhitePixel(wnd->display, defaultScreen));

  XSelectInput(wnd->display, wnd->window, ExposureMask | KeyPressMask);
  XMapWindow(wnd->display, wnd->window);
  XSync(wnd->display, false);

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
    xcb_destroy_window(connection, window);
  }
};

std::unique_ptr< Test::xcb_window_t > create_xcb_window(int width, int height)
{
  std::unique_ptr< Test::xcb_window_t > wnd{new Test::xcb_window_t};
  // 1. Create Window ( done by DALI

  wnd->width  = width;
  wnd->height = height;

  int screenNum(0);

  xcb_connection_t*     connection = xcb_connect(NULL, &screenNum);
  const xcb_setup_t*    setup      = xcb_get_setup(connection);
  xcb_screen_iterator_t iter       = xcb_setup_roots_iterator(setup);
  for(int i = 0; i < screenNum; ++i)
    xcb_screen_next(&iter);

  xcb_screen_t* screen = iter.data;
  ::xcb_window_t  window = xcb_generate_id(connection);

  uint32_t mask     = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t values[] = {screen->white_pixel, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS};

  xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, wnd->width,
                    wnd->height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask,
                    values);

  xcb_map_window(connection, window);
  const uint32_t coords[] = {100, 100};
  xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
  xcb_flush(connection);

  wnd->connection = connection;
  wnd->window     = window;

  return wnd;
}
}

namespace VulkanTest
{

Dali::Graphics::Vulkan::GpuMemoryBlockHandle test_gpu_memory_manager( Dali::Graphics::Vulkan::Graphics& graphics,
                                         GpuMemoryManager& gpuManager,
                                         Buffer& buffer )
{
  auto device = graphics.GetDevice();
  auto& allocator = graphics.GetAllocator();

  vk::Buffer vertexBuffer = device.createBuffer( vk::BufferCreateInfo{}
  .setUsage( vk::BufferUsageFlagBits::eVertexBuffer )
  .setSize( sizeof(float) * 9 )
  .setSharingMode( vk::SharingMode::eExclusive)
  .setQueueFamilyIndexCount( 0 )
  , allocator ).value;

  auto& gpuAllocator = gpuManager.GetDefaultAllocator();
  auto handle2 = gpuAllocator.Allocate( buffer, vk::MemoryPropertyFlagBits::eHostVisible );
  {
    auto handle = gpuAllocator.Allocate(device.getBufferMemoryRequirements(vertexBuffer),
                                        vk::MemoryPropertyFlagBits::eHostVisible);



    int count = handle.GetRefCount();

    {
      auto newHandle = handle;
      count = newHandle.GetRefCount();
    }
    count     = handle.GetRefCount();
  }
  return std::move(handle2);
}

void test_handle()
{
  /*
  using namespace Dali::Graphics::Vulkan;
  GpuMemoryBlockHandle handle( new GpuMemoryBlock() );

  decltype(handle) handle2 = handle;
  handle.GetRefCount();*/
}

int RunTestMain()
{

#if USE_XLIB == 1
  auto window = Test::create_xlib_window(640, 480);
  auto surfaceFactory =
      std::unique_ptr< VkSurfaceXlib >{new VkSurfaceXlib{window->display, window->window}};
#else
  auto window         = Test::create_xcb_window(640, 480);
  auto surfaceFactory =
         std::unique_ptr<VkSurfaceXcb>{new VkSurfaceXcb{window->connection, window->window}};
#endif


  auto graphics = MakeUnique<Graphics>();
  auto fbid = graphics->Create( std::move(surfaceFactory) );

  // access internal implementation
  auto& gr = graphics->GetImplementation<Dali::Graphics::Vulkan::Graphics>();

  // GPU memory manager
  auto& memmgr = gr.GetDeviceMemoryManager();

  const float VERTICES[] =
          {
            0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
          };

  // shaders
  auto vertexShader = Shader::New( gr, VSH.data(), VSH.size()*sizeof(VSH[0]) );
  auto fragmentShader = Shader::New( gr, FSH.data(), FSH.size()*sizeof(FSH[0]) );

  // buffer
  auto vertexBuffer = Buffer::New( gr, sizeof(float)*3*3, Buffer::Type::VERTEX );

  auto gpuManager = GpuMemoryManager::New( gr );

  auto bufferMemory = test_gpu_memory_manager( gr, *gpuManager, *vertexBuffer );
  vertexBuffer->BindMemory( bufferMemory );

  auto ptr = static_cast<float*>(bufferMemory->Map());
  std::copy( VERTICES, VERTICES+9, ptr);
  bufferMemory->Unmap();

  auto handle2 = vertexBuffer->GetMemoryHandle();

  vk::DescriptorSetLayoutBinding binding;
  binding.setDescriptorCount(1)
    .setDescriptorType( vk::DescriptorType::eUniformBuffer )
      .setBinding( 0 );


  vk::VertexInputAttributeDescription att;
  att.setBinding( 0 ).setLocation( 0 ).setOffset( 0 ).setFormat( vk::Format::eR32G32B32A32Sfloat );

  vk::VertexInputBindingDescription desc;
  desc.setBinding( 0 ).setInputRate( vk::VertexInputRate::eVertex ).setStride( 0 );

  //vk::CommandBuffer buf;
  //buf.bindVertexBuffers( 0, 1, nullptr, 0);

  // descriptor set layouts
  vk::DescriptorSetLayoutCreateInfo layoutInfo;
  layoutInfo.setPBindings( nullptr )
    .setBindingCount( 0 );




  // PIPELINE LAYOUT -> descriptor set layouts
  vk::PipelineLayoutCreateInfo pipelineLayout;
  pipelineLayout.setSetLayoutCount( 0 )
    .setPPushConstantRanges( nullptr )
      .setPSetLayouts( nullptr )
        .setPushConstantRangeCount( 0 );





  // PIPELINE
  vk::GraphicsPipelineCreateInfo info;
  info.setLayout( nullptr );
  info.setPColorBlendState( nullptr );
  info.setPDepthStencilState( nullptr );
  info.setPDynamicState( nullptr );
  info.setPInputAssemblyState( nullptr );
  info.setPMultisampleState( nullptr );
  info.setPRasterizationState( nullptr );
  info.setPStages( nullptr );
  info.setPTessellationState( nullptr );
  info.setPVertexInputState( nullptr );
  info.setPViewportState( nullptr );
  info.setStageCount( 0 );
  info.setSubpass( 0 );

  // pipeline
  //vk::VertexInputAttributeDescription;
  //vk::VertexInputBindingDescription;


  while(1)
  {
    graphics->PreRender( fbid );
    graphics->PostRender( fbid );
  }
  return 0;
}
}

int main()
{
  VulkanTest::RunTestMain();
}

std::vector<uint32_t> VSH = {
  0x07230203,0x00010000,0x00080001,0x00000029,0x00000000,0x00020011,0x00000001,0x0006000b,
  0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
  0x0008000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000d,0x00000020,0x00000025,
  0x00030003,0x00000002,0x000001ae,0x00040005,0x00000004,0x6e69616d,0x00000000,0x00060005,
  0x0000000b,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x0000000b,0x00000000,
  0x505f6c67,0x7469736f,0x006e6f69,0x00070006,0x0000000b,0x00000001,0x505f6c67,0x746e696f,
  0x657a6953,0x00000000,0x00070006,0x0000000b,0x00000002,0x435f6c67,0x4470696c,0x61747369,
  0x0065636e,0x00030005,0x0000000d,0x00000000,0x00040005,0x00000011,0x6c726f77,0x00000064,
  0x00060006,0x00000011,0x00000000,0x65646f6d,0x74614d6c,0x00000000,0x00050006,0x00000011,
  0x00000001,0x77656976,0x0074614d,0x00050006,0x00000011,0x00000002,0x6a6f7270,0x0074614d,
  0x00050006,0x00000011,0x00000003,0x6f6c6f63,0x00000072,0x00030005,0x00000013,0x00000000,
  0x00050005,0x00000020,0x736f5061,0x6f697469,0x0000006e,0x00040005,0x00000025,0x6c6f4376,
  0x0000726f,0x00050048,0x0000000b,0x00000000,0x0000000b,0x00000000,0x00050048,0x0000000b,
  0x00000001,0x0000000b,0x00000001,0x00050048,0x0000000b,0x00000002,0x0000000b,0x00000003,
  0x00030047,0x0000000b,0x00000002,0x00040048,0x00000011,0x00000000,0x00000005,0x00050048,
  0x00000011,0x00000000,0x00000023,0x00000000,0x00050048,0x00000011,0x00000000,0x00000007,
  0x00000010,0x00040048,0x00000011,0x00000001,0x00000005,0x00050048,0x00000011,0x00000001,
  0x00000023,0x00000040,0x00050048,0x00000011,0x00000001,0x00000007,0x00000010,0x00040048,
  0x00000011,0x00000002,0x00000005,0x00050048,0x00000011,0x00000002,0x00000023,0x00000080,
  0x00050048,0x00000011,0x00000002,0x00000007,0x00000010,0x00050048,0x00000011,0x00000003,
  0x00000023,0x000000c0,0x00030047,0x00000011,0x00000002,0x00040047,0x00000013,0x00000022,
  0x00000000,0x00040047,0x00000013,0x00000021,0x00000000,0x00040047,0x00000020,0x0000001e,
  0x00000000,0x00040047,0x00000025,0x0000001e,0x00000000,0x00020013,0x00000002,0x00030021,
  0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,
  0x00000004,0x00040015,0x00000008,0x00000020,0x00000000,0x0004002b,0x00000008,0x00000009,
  0x00000001,0x0004001c,0x0000000a,0x00000006,0x00000009,0x0005001e,0x0000000b,0x00000007,
  0x00000006,0x0000000a,0x00040020,0x0000000c,0x00000003,0x0000000b,0x0004003b,0x0000000c,
  0x0000000d,0x00000003,0x00040015,0x0000000e,0x00000020,0x00000001,0x0004002b,0x0000000e,
  0x0000000f,0x00000000,0x00040018,0x00000010,0x00000007,0x00000004,0x0006001e,0x00000011,
  0x00000010,0x00000010,0x00000010,0x00000007,0x00040020,0x00000012,0x00000002,0x00000011,
  0x0004003b,0x00000012,0x00000013,0x00000002,0x0004002b,0x0000000e,0x00000014,0x00000002,
  0x00040020,0x00000015,0x00000002,0x00000010,0x0004002b,0x0000000e,0x00000018,0x00000001,
  0x00040020,0x0000001f,0x00000001,0x00000007,0x0004003b,0x0000001f,0x00000020,0x00000001,
  0x00040020,0x00000023,0x00000003,0x00000007,0x0004003b,0x00000023,0x00000025,0x00000003,
  0x0004002b,0x00000006,0x00000026,0x3f800000,0x0004002b,0x00000006,0x00000027,0x00000000,
  0x0007002c,0x00000007,0x00000028,0x00000026,0x00000027,0x00000027,0x00000026,0x00050036,
  0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000015,
  0x00000016,0x00000013,0x00000014,0x0004003d,0x00000010,0x00000017,0x00000016,0x00050041,
  0x00000015,0x00000019,0x00000013,0x00000018,0x0004003d,0x00000010,0x0000001a,0x00000019,
  0x00050092,0x00000010,0x0000001b,0x00000017,0x0000001a,0x00050041,0x00000015,0x0000001c,
  0x00000013,0x0000000f,0x0004003d,0x00000010,0x0000001d,0x0000001c,0x00050092,0x00000010,
  0x0000001e,0x0000001b,0x0000001d,0x0004003d,0x00000007,0x00000021,0x00000020,0x00050091,
  0x00000007,0x00000022,0x0000001e,0x00000021,0x00050041,0x00000023,0x00000024,0x0000000d,
  0x0000000f,0x0003003e,0x00000024,0x00000022,0x0003003e,0x00000025,0x00000028,0x000100fd,
  0x00010038
};

std::vector<uint32_t> FSH = {
  0x07230203,0x00010000,0x00080001,0x0000000d,0x00000000,0x00020011,0x00000001,0x0006000b,
  0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
  0x0007000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000b,0x00030003,
  0x00000002,0x000001ae,0x00040005,0x00000004,0x6e69616d,0x00000000,0x00050005,0x00000009,
  0x4374756f,0x726f6c6f,0x00000000,0x00040005,0x0000000b,0x6c6f4376,0x0000726f,0x00040047,
  0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000b,0x0000001e,0x00000000,0x00020013,
  0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,
  0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,0x00000007,0x0004003b,
  0x00000008,0x00000009,0x00000003,0x00040020,0x0000000a,0x00000001,0x00000007,0x0004003b,
  0x0000000a,0x0000000b,0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,
  0x000200f8,0x00000005,0x0004003d,0x00000007,0x0000000c,0x0000000b,0x0003003e,0x00000009,
  0x0000000c,0x000100fd,0x00010038
};