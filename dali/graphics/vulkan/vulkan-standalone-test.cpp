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

#include <dali/graphics/vulkan/vulkan-hpp-wrapper.h>

#include <dali/integration-api/graphics/vulkan/vk-surface-factory.h>
#include <dali/integration-api/graphics/graphics.h>
#include <xcb/xcb.h>
#include <unistd.h>

#define USE_XLIB 0

using Dali::Integration::Graphics::Graphics;
using Dali::Integration::Graphics::Vulkan::VkSurfaceFactory;

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

  while(1)
  {
    graphics->PreRender( fbid );
    graphics->PostRender( fbid );
  }
  return 0;
}
}
