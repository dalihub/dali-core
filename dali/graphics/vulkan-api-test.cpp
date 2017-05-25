//
// Created by adam.b on 23/03/17.
//

#include <vulkan/vulkan.hpp>
#ifdef __linux__
#include <X11/Xlib.h>
#include <unistd.h>
#endif


#include <dali/graphics/graphics-physical-device.h>
#include <dali/graphics/vulkan/physical-device.h>
#include <dali/graphics/vulkan/surface/xcb-surface.h>
#include <dali/graphics/vulkan/surface/xlib-surface.h>

// vulkan specific
#include <dali/graphics/vulkan/command-queue.h>
#include <dali/graphics/vulkan/device-memory.h>
#include <dali/graphics/vulkan/image.h>
#include <dali/graphics/vulkan/logical-device.h>
#include <dali/graphics/vulkan/swapchain.h>

using namespace Dali::Graphics;

namespace
{
Vulkan::CommandPool gSecondaryCommandPool{nullptr};
Vulkan::Image       gImage{nullptr};
}

struct TestWindow
{
  int      width, height;
  Display* display;
  ::Window window;
  int      defaultScreen;
} gWnd;

void InitWindow(int width, int height)
{
  // 1. Create Window ( done by DALI
  gWnd.width         = width;
  gWnd.height        = height;
  gWnd.display       = XOpenDisplay(nullptr);
  gWnd.defaultScreen = DefaultScreen(gWnd.display);
  gWnd.window = XCreateSimpleWindow(gWnd.display, RootWindow(gWnd.display, gWnd.defaultScreen), 0, 0,
                                    gWnd.width, gWnd.height, 1, BlackPixel(gWnd.display, gWnd.defaultScreen),
                                    WhitePixel(gWnd.display, gWnd.defaultScreen));

  XSelectInput(gWnd.display, gWnd.window, ExposureMask | KeyPressMask);
  XMapWindow(gWnd.display, gWnd.window);
}

struct XcbWindow
{
  xcb_connection_t* xcb_connection;
  xcb_screen_t*     xcb_screen;
  xcb_window_t      xcb_window;
  int               width, height;
} gXcb;

void initXcbWindow(int width, int height)
{
  // 1. Create Window ( done by DALi )
  gXcb.width  = width;
  gXcb.height = height;
  int                   screenNum(0);
  xcb_connection_t*     connection = xcb_connect(NULL, &screenNum);
  const xcb_setup_t*    setup      = xcb_get_setup(connection);
  xcb_screen_iterator_t iter       = xcb_setup_roots_iterator(setup);
  for(int i = 0; i < screenNum; ++i)
    xcb_screen_next(&iter);

  xcb_screen_t* screen = iter.data;
  xcb_window_t  window = xcb_generate_id(connection);

  uint32_t mask     = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t values[] = {screen->white_pixel, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS};

  xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, gXcb.width,
                    gXcb.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, values);

  xcb_map_window(connection, window);
  const uint32_t coords[] = {100, 100};
  xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
  xcb_flush(connection);

  gXcb.xcb_connection = connection;
  gXcb.xcb_window     = window;
  gXcb.xcb_screen     = screen;
}

void createTexture(const Vulkan::LogicalDevice& device, const Vulkan::CommandBuffer& commandBuffer)
{
  // create buffer
  const int TEXTURE_SIZE = 1024 * 1024 * 4;

  vk::BufferCreateInfo bufInfo;
  bufInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
      .setSharingMode(vk::SharingMode::eExclusive)
      .setQueueFamilyIndexCount(0)
      .setPQueueFamilyIndices(nullptr)
      .setSize(TEXTURE_SIZE);

  auto textureBuffer = Vulkan::Buffer::New(device, bufInfo);
  auto memory = device.AllocateBufferMemory(textureBuffer, vk::MemoryPropertyFlagBits::eHostVisible, true);
  void* ptr = memory.Map(0, VK_WHOLE_SIZE);

  // Load texture directly into the buffer
  FILE* f = fopen("/tmp/out.rgba", "rb");
  fseek(f, 0, SEEK_END);
  assert(ftell(f) == bufInfo.size);
  fseek(f, 0, SEEK_SET);
  fread(ptr, 1, bufInfo.size, f);
  fclose(f);

  // unmap and flush memory
  memory.Unmap();
  memory.Flush();

  // create 2D texture image
  vk::ImageCreateInfo imageInfo;
  imageInfo.setSharingMode(vk::SharingMode::eExclusive);
  imageInfo.setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst);
  imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
  imageInfo.setSamples(vk::SampleCountFlagBits::e1);
  imageInfo.setExtent(vk::Extent3D{1024, 1024, 1});
  imageInfo.setFormat(vk::Format::eR8G8B8A8Unorm);
  imageInfo.setTiling(vk::ImageTiling::eOptimal);
  imageInfo.setMipLevels(1);
  imageInfo.setArrayLayers(1);
  imageInfo.setImageType(vk::ImageType::e2D);

  // create vulkan image
  auto image       = Vulkan::Image::New(device, imageInfo);
  auto imageMemory = device.AllocateImageMemory(image, vk::MemoryPropertyFlagBits::eDeviceLocal, true);

  if(!gSecondaryCommandPool)
  {
    gSecondaryCommandPool = device.CreateCommandPool(QueueType::TRANSFER, true, true);
  }

  // record
  Vulkan::CommandBuffer cmdBuf = gSecondaryCommandPool.AllocateCommandBuffer(true);
  vk::BufferImageCopy   region;
  region.setImageExtent({1024, 1024, 1});
  region.setImageSubresource(vk::ImageSubresourceLayers()
                                 .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                 .setMipLevel(0)
                                 .setLayerCount(1)
                                 .setBaseArrayLayer(0));

  auto layoutBarrier =
      image.GetLayoutChangeBarrier(vk::ImageLayout::eTransferDstOptimal, vk::AccessFlags(),
                                   vk::AccessFlagBits::eTransferWrite, vk::ImageAspectFlagBits::eColor);

  cmdBuf.Begin(true);
  cmdBuf->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &layoutBarrier);
  cmdBuf->copyBufferToImage(*textureBuffer, *image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

  image.SetLayout( vk::ImageLayout::eTransferDstOptimal );
  auto layoutBarrier2 =
  image.GetLayoutChangeBarrier(vk::ImageLayout::eTransferSrcOptimal, vk::AccessFlags(),
                               vk::AccessFlagBits::eTransferWrite, vk::ImageAspectFlagBits::eColor);


  cmdBuf->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe,
                          vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &layoutBarrier2);

  image.SetLayout( vk::ImageLayout::eTransferSrcOptimal );
  cmdBuf.End();

  auto queue = device.GetCommandQueue(0, QueueType::TRANSFER);
  queue.Submit(&cmdBuf, 1, nullptr);

  // should use fence
  queue.WaitIdle();

  gImage = image;
  return;
}

int main(int argc, char** argv)
{

  InitWindow(720, 360);
  // make physical device from vulkan device ( should be done through some sort of
  // implementation factory )
  GraphicsPhysicalDevice physDevice(Vulkan::PhysicalDevice::New());
  ExtensionNameList      list;

  // check extensions ( vulkan specific )

  NativeSurfaceType surfaceType = NativeSurfaceType::UNDEFINED;

  if(physDevice.IsExtensionAvailable(VK_KHR_SURFACE_EXTENSION_NAME))
    list.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
  if(physDevice.IsExtensionAvailable(VK_KHR_XCB_SURFACE_EXTENSION_NAME))
  {
    list.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
    //surfaceType = NativeSurfaceType::XCB;
  }
  if(physDevice.IsExtensionAvailable(VK_KHR_XLIB_SURFACE_EXTENSION_NAME))
  {
    list.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
    if(surfaceType == NativeSurfaceType::UNDEFINED)
      surfaceType = NativeSurfaceType::X11;
  }

  physDevice.Initialise(list,
                        ValidationLayerFlags2() | ValidationLayerBit2::CORE_VALIDATION |
                            ValidationLayerBit2::STANDARD_VALIDATION |
                            ValidationLayerBit2::PARAMETER_VALIDATION | ValidationLayerBit2::API_DUMP);

  // reporting on all channels
  //physDevice.SetValidationDebugChannels(ValidationChannelBit::ALL);

  bool result = physDevice.ChoosePhysicalDevice(PhysicalDeviceBit::ANY);

  GraphicsSurface surface{nullptr};

  if(surfaceType == NativeSurfaceType::XCB)
  {
    XcbSurfaceCreateInfo info;
    info.connection = gXcb.xcb_connection;
    info.window     = gXcb.xcb_window;
    surface         = physDevice.CreateSurface(info);
  }
  else
  {
    XlibSurfaceCreateInfo info;
    info.display    = gWnd.display;
    info.window     = gWnd.window;
    surface         = physDevice.CreateSurface(info);
  }
  auto logicalDevice = physDevice.CreateLogicalDevice();

  auto swapchain = logicalDevice.CreateSwapchain(surface, 2, DepthStencil::NONE, VSyncMode::DISABLED);

  // obtain vulkan interface
  auto vkSwapchain = swapchain.Cast< Vulkan::Swapchain >();
  auto vkDevice    = logicalDevice.Cast< Vulkan::LogicalDevice >();

  createTexture(vkDevice, vkSwapchain.GetCurrentCommandBuffer());

  bool running = true;

  while(running)
  {
    swapchain.AcquireFrame();
    auto          cmdbuf = vkSwapchain.GetCurrentCommandBuffer();
    vk::ImageCopy region;

    vk::ImageSubresourceLayers subres;
    subres.setBaseArrayLayer(0).setAspectMask(vk::ImageAspectFlagBits::eColor).setLayerCount(1).setMipLevel(0);

    region.setExtent({100, 100, 1})
        .setDstOffset({0, 0, 0})
        .setSrcOffset({0, 0, 0})
        .setSrcSubresource(subres)
        .setDstSubresource(subres);

    cmdbuf->copyImage(*gImage, gImage.GetLayout(), vkSwapchain.GetCurrentImage(),
                                               vk::ImageLayout::eColorAttachmentOptimal, 1, &region);
    usleep(1000);
    swapchain.PresentFrame();
  }

  return 0;
}

int main_(int argc, char** argv)
{
  initXcbWindow(720, 360);

#if 0
  // using template argument to pick adaptor implementation
  GraphicsPhysicalDevice   adaptor = GraphicsPhysicalDevice::GraphicsPhysicalDevice< Internal::VulkanAdaptor >();
  ExtensionNameList list;

  // check extension
  if(adaptor.IsExtensionAvailable(VK_KHR_SURFACE_EXTENSION_NAME))
    list.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
  if(adaptor.IsExtensionAvailable(VK_KHR_XLIB_SURFACE_EXTENSION_NAME))
    list.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
  if(adaptor.IsExtensionAvailable(VK_KHR_XCB_SURFACE_EXTENSION_NAME))
    list.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);

  // initialise
  adaptor.Initialise(list,
                     ValidationLayerFlags()
  | ValidationLayerBit2::CORE_VALIDATION |
                         ValidationLayerBit2::STANDARD_VALIDATION |
                         ValidationLayerBit2::PARAMETER_VALIDATION | ValidationLayerBit2::API_DUMP);

  // reporting on all channels
  adaptor.SetValidationDebugChannels(ValidationChannelBit::ALL);

  // choose first available physical adaptor
  bool result = adaptor.ChoosePhysicalDevice(PhysicalDeviceBit::ANY);

  if(!result) // no requested device available ( maybe adding enumeration )
  {
    // fixme, something went very wrong
  }

  // create surface for X11 ( could be templated GraphicsPhysicalDevice function? )
  auto surface = adaptor.CreateSurface< Internal::GraphicsXcbSurface >(adaptor, gXcb.xcb_connection, gXcb.xcb_window);

  if(!surface)
  {
    VkLog("No surface dammit!");
  }

  // create context
  auto context = adaptor.CreateContext(surface);

  // create swapchain ( 2 buffers )
  auto swapchain = context.CreateSwapchain(surface, 3, DepthStencil::NONE, true);

  while( 1 )
  {
    swapchain.AcquireFrame();
    usleep(1000);
    //swapchain.SwapBuffers( true );
    swapchain.PresentFrame();
  }

  return 0;
}

int _main(int argc, char** argv)
{
#if 0
  InitWindow(720, 360);

  GraphicsPhysicalDevice adaptor;
  ExtensionNameList list;

  // check extension
  if(adaptor.IsExtensionAvailable(VK_KHR_SURFACE_EXTENSION_NAME))
    list.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
  if(adaptor.IsExtensionAvailable(VK_KHR_XLIB_SURFACE_EXTENSION_NAME))
    list.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);

  // initialise
  adaptor.Initialise(list, ValidationLayerFlags() |
                    ValidationLayerBit2::CORE_VALIDATION | ValidationLayerBit2::STANDARD_VALIDATION |
  ValidationLayerBit2::PARAMETER_VALIDATION|ValidationLayerBit2::API_DUMP);

  // reporting on all channels
  adaptor.SetValidationDebugChannels( ValidationChannelBit::ALL );

  // choose first available physical adaptor
  bool result = adaptor.ChoosePhysicalDevice(PhysicalDeviceBit::ANY);

  if( !result ) // no requested device available ( maybe adding enumeration )
  {
    // fixme, something went very wrong
  }

  // create surface for X11 ( could be templated GraphicsPhysicalDevice function? )
  //auto& surface = GraphicsSurfaceBase::CreateSurface<GraphicsXlibSurface>( adaptor, gWnd.display, gWnd.window );
  auto& surface = adaptor.CreateSurface<GraphicsXlibSurface>( adaptor, gWnd.display, gWnd.window );

  if( surface == nullptr )
  {
    // surface invalid :((((
  }
  // create context
  GraphicsContext context = adaptor.CreateContext(surface);

  // create swapchain with 2 buffers
  GraphicsSwapchain swapchain = context.CreateSwapchain( surface, 2 );

#endif
#endif
  return 0;
}