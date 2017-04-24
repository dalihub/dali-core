//
// Created by adam.b on 23/03/17.
//

#include <vulkan/vulkan.hpp>
#ifdef __linux__
#include <X11/Xlib.h>
#include <unistd.h>
#endif

/*
#include <dali/graphics/graphics-adaptor.h>
#include <dali/graphics/graphics-context.h>
#include <dali/graphics/vulkan/surface/graphics-xcb-surface.h>
#include <dali/graphics/vulkan/surface/graphics-xlib-surface.h>
#include <dali/graphics/vulkan/vulkan-adaptor.h>
*/

#include <dali/graphics/graphics-physical-device.h>
#include <dali/graphics/vulkan/physical-device.h>
#include <dali/graphics/vulkan/surface/xcb-surface.h>

using namespace Dali::Graphics;

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

int main(int argc, char** argv)
{
  initXcbWindow(720, 360);
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
    if( surfaceType == NativeSurfaceType::UNDEFINED )
      surfaceType = NativeSurfaceType::XCB;
  }
  if(physDevice.IsExtensionAvailable(VK_KHR_XLIB_SURFACE_EXTENSION_NAME))
  {
    list.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
    if( surfaceType == NativeSurfaceType::UNDEFINED )
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

  if( surfaceType == NativeSurfaceType::XCB )
  {
    XcbSurfaceCreateInfo info;
    info.connection = gXcb.xcb_connection;
    info.window = gXcb.xcb_window;
    surface = physDevice.CreateSurface( info );
  }

  auto logicalDevice = physDevice.CreateLogicalDevice();

  auto swapchain = logicalDevice.CreateSwapchain( surface, 2, DepthStencil::NONE, false );

  while( 1 )
  {
    swapchain.AcquireFrame();
    usleep(1000);
    //swapchain.SwapBuffers( true );
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