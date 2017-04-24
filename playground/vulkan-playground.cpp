//
// Created by adam.b on 12/04/17.
//
//#define XLIB
#include <X11/Xlib.h>
#include <unistd.h>
#include <vulkan/vulkan.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern unsigned char VERTEX_SHADER_SPV[];
extern unsigned char FRAGMENT_SHADER_SPV[];
extern unsigned int  VERTEX_SHADER_LEN;
extern unsigned int  FRAGMENT_SHADER_LEN;

#define VkMake(type, x) \
  type x;               \
  memset(&x, 0, sizeof(type));

#define vkAssert(x)                           \
  {                                           \
    VkResult _result = x;                     \
    if(_result != VK_SUCCESS)                 \
    {                                         \
      printf("Result: %d\n", ( int )_result); \
    }                                         \
    assert((_result) == VK_SUCCESS);          \
  }

#define vkprintf(format, ...)    \
  {                              \
    printf(format, __VA_ARGS__); \
    puts("");                    \
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

// c-like stack allocated on heap
typedef unsigned int StackMark;
#define STACK_MARKED_LAST -1u

struct Stack
{
  char*        pData;
  unsigned int pos;
  unsigned int size;
  unsigned int alignment;
  StackMark    markedPos;
};

void stackCreate(Stack** ppStack, size_t stackSize, size_t alignment)
{
  Stack* pStack = (*ppStack = ( Stack* )malloc(sizeof(Stack)));
  pStack->pData = ( char* )malloc(pStack->size = (((stackSize / alignment) + 1) * alignment));
  pStack->pos       = 0;
  pStack->alignment = alignment;
  pStack->markedPos = 0;
}

int stackGetMarkedPos(Stack* pStack)
{
  return pStack->markedPos;
}

void* stackAllocate(Stack* stack, size_t size, size_t count, int shouldClear)
{
  if(stack->pos + (size * count) >= stack->size)
    return NULL;

  void* retval = &stack->pData[stack->pos];
  if(shouldClear)
    memset(retval, 0, size * count);

  stack->pos += size * count;

  // adjust stack pos to meet mem alignment requirements
  stack->pos = ((stack->pos / stack->alignment) + 1) * stack->alignment;

  return retval;
}

StackMark stackMark(Stack* pStack)
{
  pStack->markedPos = pStack->pos;
  return pStack->pos;
}

void stackRollback(Stack* pStack, StackMark mark)
{
  if(mark == STACK_MARKED_LAST)
    pStack->pos = pStack->markedPos;
  else
    pStack->pos     = mark;
  pStack->markedPos = 0;
}

void stackRollbackAll(Stack* pStack)
{
  pStack->pos       = 0;
  pStack->markedPos = 0;
}

#define stackNew(stack, type, count) ( type* )stackAllocate(stack, sizeof(type), count, true)

#define MAX_TRANSIENT_CMD_BUFFER_COUNT 16

namespace
{
const VkImageSubresourceRange IMAGE_COLOR_SUBRESOURCE_RANGE = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
}

// copy-paste from vulkan spec
uint32_t getMemoryIndex(uint32_t memoryTypeBits, const VkPhysicalDeviceMemoryProperties* pMemProperties,
                        VkMemoryPropertyFlags properties)
{
  for(uint32_t i = 0; i < pMemProperties->memoryTypeCount; ++i)
  {
    if((memoryTypeBits & (1 << i)) && ((pMemProperties->memoryTypes[i].propertyFlags & properties) == properties))
    {
      return i;
    }
  }
  return -1;
}

struct Texture
{
  VkImage         image;
  VkImageView     imageView;
  VkDeviceMemory  deviceMemory;
  uint32_t        width;
  uint32_t        height;
  VkFormat        format;
  VkImageLayout   layout;
  VkSampler       sampler;
  VkFence         resourceFence;
  VkCommandBuffer cmdBuffer;

  class App* app;
  static Texture* createRGBA2D(class App* app, uint32_t width, uint32_t height, void* pData, size_t dataSize);
  static Texture* createRGBA2D(class App* app, uint32_t width, uint32_t height, const char* filename);

  VkImageMemoryBarrier createLayoutChangeBarrier(VkImageLayout newLayout, VkAccessFlags srcAccess,
                                                 VkAccessFlags dstAccess);
};

struct Mesh
{
  class App*      app;
  VkBuffer        vertices;
  VkDeviceMemory  memory;
  size_t          vertexCount;
  size_t          vertexSize;
  int             isMutable;
};

int meshCreateWithData( App* pApp, size_t vertexCount, size_t vertexSize, Mesh** ppMesh );

VkImageMemoryBarrier makeImageBarrier(VkImage image, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                      VkImageLayout oldLayout, VkImageLayout newLayout)
{
  VkMake(VkImageMemoryBarrier, barrier);
  barrier.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image            = image;
  barrier.subresourceRange = IMAGE_COLOR_SUBRESOURCE_RANGE;
  barrier.srcAccessMask    = srcAccess;
  barrier.dstAccessMask    = dstAccess;
  barrier.oldLayout        = oldLayout;
  barrier.newLayout        = newLayout;
  return barrier;
}

void cmdPipelineImageBarrier(VkCommandBuffer cmdbuf, size_t count, VkImageMemoryBarrier* pBarriers,
                             VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
{
  vkCmdPipelineBarrier(cmdbuf, srcStage, dstStage, 0, 0, 0, 0, 0, count, pBarriers);
}

class App
{
public:
  void initialise();
  void initialiseShaders();
  void enableValidation();
  void createFramebufferRenderPass();
  void initialiseDraw();
  void initialiseBuffers();
  void beginFrame();
  void endFrame();
  void preDrawFrame(VkCommandBuffer primaryBuffer);
  void postDrawFrame(VkCommandBuffer primaryBuffer);
  void drawFrame(VkCommandBuffer primaryBuffer, VkImage swapImage, VkFramebuffer fb, VkRenderPass rp);

  void waitForFences(VkFence* pFences, int count);
  struct SwapImage
  {
    VkImage            image;
    VkImageView        imageView;
    VkImageAspectFlags aspect;
    VkImageLayout      layout;
    VkFramebuffer      framebuffer;
    VkRenderPass       renderPass;
    VkCommandBuffer    cmdPrimaryBuffer;
    VkCommandBuffer    cmdTransientBuffer[MAX_TRANSIENT_CMD_BUFFER_COUNT];
    VkSemaphore        imageAcquireSem;
    VkSemaphore        imagePresentSem;
  } swapImages[16]; // max 16 buffers
  uint32_t swapImageIndex;
  uint32_t swapImageCount;

  //private:
  VkInstance                       instance;
  VkPhysicalDevice                 physicalDevice;
  VkPhysicalDeviceFeatures         pdFeatures;
  VkPhysicalDeviceProperties       pdProperties;
  VkPhysicalDeviceMemoryProperties pdMemoryProperties;

  VkDevice               device;
  VkAllocationCallbacks* allocator;

  uint32_t allQueueFamilyIndex;

  VkSurfaceKHR             surface;
  VkSurfaceCapabilitiesKHR surfaceCaps;
  VkSurfaceFormatKHR       surfaceFormat;

  VkQueue queue;

  VkCommandPool commandPool;

  VkSwapchainKHR swapchain;

  VkRenderPass sharedRenderPass;

  VkFence acquireImageFence[4];
  int     acquireImageFenceIndex;

  // drawable
  VkPipeline     pipeline;
  VkShaderModule vertexShader, fragmentShader;

  Stack*   pStack;
  Texture* pTexture;
};

void App::initialiseShaders()
{
  VkMake(VkShaderModuleCreateInfo, moduleInfo);
  moduleInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  moduleInfo.codeSize = VERTEX_SHADER_LEN;
  moduleInfo.pCode    = ( const uint32_t* )VERTEX_SHADER_SPV;
  moduleInfo.flags    = VK_SHADER_STAGE_VERTEX_BIT;
  vkAssert(vkCreateShaderModule(device, &moduleInfo, allocator, &vertexShader));
  moduleInfo.codeSize = FRAGMENT_SHADER_LEN;
  moduleInfo.pCode    = ( const uint32_t* )FRAGMENT_SHADER_SPV;
  moduleInfo.flags    = VK_SHADER_STAGE_FRAGMENT_BIT;
  vkAssert(vkCreateShaderModule(device, &moduleInfo, allocator, &fragmentShader));
}

void App::initialiseBuffers()
{
  VkBuffer    buffer;
  const float P = 0.5f;
  const float M = 0.5f;
  const float Z = 0.0f;

  const float VERTICES[] = {
      M, P, 1.0f, 0.0f, 0.0f, Z, M, 0.0f, 1.0f, 0.0f, P, P, 0.0f, 0.0f, 1.0f,
  };

  VkMake(VkBufferCreateInfo, info);
  info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size        = sizeof(VERTICES);
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  vkAssert(vkCreateBuffer(device, &info, allocator, &buffer));

  VkMemoryRequirements req;
  vkGetBufferMemoryRequirements(device, buffer, &req);

  // allocate memory as host visible
  VkDeviceMemory memory;
  VkMake(VkMemoryAllocateInfo, meminfo);
  meminfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  meminfo.allocationSize = req.size;
  meminfo.memoryTypeIndex = getMemoryIndex(req.memoryTypeBits, 0, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  vkAssert(vkAllocateMemory(device, &meminfo, allocator, &memory));

  void* ptr = nullptr;
  vkAssert(vkMapMemory(device, memory, 0, info.size, 0, &ptr));

  vkUnmapMemory(device, memory);
  VkMake(VkMappedMemoryRange, range);
  // flush memory ( not necessary if memory is coherent )
  range.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range.offset = 0;
  range.size   = info.size;
  range.memory = memory;
  vkFlushMappedMemoryRanges(device, 1, &range);
}

void App::initialiseDraw()
{
  int createStackFrame = (stackGetMarkedPos(pStack) != 0);
  if(createStackFrame)
    stackMark(pStack);

  VkPipelineLayout      pipelineLayout;
  VkDescriptorSetLayout dsLayout;
  VkMake(VkPipelineLayoutCreateInfo, layoutInfo);

  VkDescriptorSetLayoutBinding dsBinding;
  dsBinding.binding         = 0;
  dsBinding.descriptorCount = 1;
  dsBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  dsBinding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;
  VkMake(VkDescriptorSetLayoutCreateInfo, dsInfo);
  dsInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  dsInfo.bindingCount = 1;
  dsInfo.pBindings    = &dsBinding;
  vkAssert(vkCreateDescriptorSetLayout(device, &dsInfo, allocator, &dsLayout));

  layoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutInfo.pPushConstantRanges    = 0;
  layoutInfo.pSetLayouts            = &dsLayout;
  layoutInfo.setLayoutCount         = 1;
  layoutInfo.pushConstantRangeCount = 0;
  vkAssert(vkCreatePipelineLayout(device, &layoutInfo, allocator, &pipelineLayout));

  VkVertexInputAttributeDescription viad[2];
  // x, y ( pos per vertex )
  viad[0].binding  = 0;
  viad[0].offset   = 0;
  viad[0].format   = VK_FORMAT_R32G32_SFLOAT;
  viad[0].location = 0;

  // r, g, b ( image per vertex )
  viad[1].binding  = 0;
  viad[1].offset   = sizeof(float) * 2;
  viad[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
  viad[1].location = 1;

  VkVertexInputBindingDescription vibd;
  vibd.binding   = 0;
  vibd.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  vibd.stride    = sizeof(float) * 5;

  VkMake(VkPipelineVertexInputStateCreateInfo, viInfo);
  viInfo.sType                        = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  viInfo.pVertexAttributeDescriptions = viad;
  viInfo.pVertexBindingDescriptions   = &vibd;
  viInfo.vertexAttributeDescriptionCount = 2;
  viInfo.vertexBindingDescriptionCount   = 1;

  VkMake(VkPipelineInputAssemblyStateCreateInfo, iaInfo);
  iaInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  iaInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  iaInfo.primitiveRestartEnable = VK_FALSE;

  VkMake(VkPipelineRasterizationStateCreateInfo, rsInfo);
  rsInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rsInfo.cullMode                = VK_CULL_MODE_NONE;
  rsInfo.depthBiasEnable         = VK_FALSE;
  rsInfo.frontFace               = VK_FRONT_FACE_CLOCKWISE;
  rsInfo.polygonMode             = VK_POLYGON_MODE_FILL;
  rsInfo.rasterizerDiscardEnable = VK_TRUE;
  rsInfo.lineWidth               = 1.0f;

  VkMake(VkPipelineColorBlendAttachmentState, attState);
  attState.blendEnable = VK_FALSE;

  VkMake(VkPipelineColorBlendStateCreateInfo, bsInfo);
  bsInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  bsInfo.pAttachments      = &attState;
  bsInfo.attachmentCount   = 1;
  bsInfo.blendConstants[0] = 1.0f;
  bsInfo.blendConstants[1] = 1.0f;
  bsInfo.blendConstants[2] = 1.0f;
  bsInfo.blendConstants[3] = 1.0f;
  bsInfo.logicOpEnable     = VK_FALSE;

  VkViewport viewport;
  viewport.x        = 0;
  viewport.y        = 0;
  viewport.width    = gXcb.width;
  viewport.height   = gXcb.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor;
  scissor.offset.x      = 0;
  scissor.offset.y      = 0;
  scissor.extent.width  = viewport.width;
  scissor.extent.height = viewport.height;

  VkMake(VkPipelineViewportStateCreateInfo, vsInfo);
  vsInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  vsInfo.pScissors     = &scissor;
  vsInfo.pViewports    = &viewport;
  vsInfo.scissorCount  = 1;
  vsInfo.viewportCount = 1;

  VkPipelineShaderStageCreateInfo* shaderInfo = stackNew(pStack, VkPipelineShaderStageCreateInfo, 2);
  shaderInfo[0].sType                         = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderInfo[0].module                        = vertexShader;
  shaderInfo[0].pName                         = "main";
  shaderInfo[0].stage                         = VK_SHADER_STAGE_VERTEX_BIT;
  shaderInfo[1].sType                         = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderInfo[1].module                        = fragmentShader;
  shaderInfo[1].pName                         = "main";
  shaderInfo[1].stage                         = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkMake(VkGraphicsPipelineCreateInfo, info);
  info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  info.layout              = pipelineLayout;
  info.renderPass          = sharedRenderPass;
  info.subpass             = 0;
  info.pVertexInputState   = &viInfo;
  info.pInputAssemblyState = &iaInfo;
  info.pRasterizationState = &rsInfo;
  info.pColorBlendState    = &bsInfo;
  info.pViewportState      = &vsInfo;
  info.pStages             = shaderInfo;
  info.stageCount          = 2;

  vkAssert(vkCreateGraphicsPipelines(device, 0, 1, &info, allocator, &pipeline));

  if(createStackFrame)
    stackRollbackAll(pStack);
}

void App::enableValidation()
{

  uint32_t count = 0u;
  vkEnumerateInstanceLayerProperties(&count, 0);
  VkLayerProperties* properties = stackNew(pStack, VkLayerProperties, count);
  vkEnumerateInstanceLayerProperties(&count, properties);

  for(int i = 0; i < count; ++i)
  {
    vkprintf("\"%s\",", properties[i].layerName);
  }
}

void App::waitForFences(VkFence* pFences, int count)
{
  // busy wait, but making sure fence is met
  while(vkWaitForFences(device, count, pFences, VK_TRUE, 1000000) != VK_SUCCESS)
  {
  }
}

void App::createFramebufferRenderPass()
{
  VkRenderPass  renderPass;
  VkFramebuffer framebuffer;

  VkMake(VkRenderPassCreateInfo, rpInfo);
  VkMake(VkFramebufferCreateInfo, fbInfo);
  VkMake(VkAttachmentDescription, attDesc);
  VkMake(VkSubpassDescription, subpassDesc);
  VkMake(VkAttachmentReference, attRef);

  rpInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rpInfo.attachmentCount = 1;
  rpInfo.dependencyCount = 0;
  rpInfo.pAttachments    = &attDesc;
  rpInfo.pDependencies   = 0;
  rpInfo.pSubpasses      = &subpassDesc;
  rpInfo.subpassCount    = 1;

  attDesc.format        = surfaceFormat.format;
  attDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  attDesc.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attDesc.loadOp        = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attDesc.samples       = VK_SAMPLE_COUNT_1_BIT;
  attDesc.storeOp       = VK_ATTACHMENT_STORE_OP_STORE;

  subpassDesc.colorAttachmentCount    = 1;
  subpassDesc.inputAttachmentCount    = 0;
  subpassDesc.pColorAttachments       = &attRef;
  subpassDesc.pDepthStencilAttachment = 0;
  subpassDesc.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDesc.pPreserveAttachments    = 0;
  subpassDesc.preserveAttachmentCount = 0;
  subpassDesc.pResolveAttachments     = 0;

  attRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  attRef.attachment = 0;

  vkAssert(vkCreateRenderPass(device, &rpInfo, allocator, &renderPass));

  sharedRenderPass = renderPass;

  fbInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  fbInfo.pAttachments    = 0;
  fbInfo.attachmentCount = 1;
  fbInfo.renderPass      = sharedRenderPass;
  fbInfo.height          = gXcb.height;
  fbInfo.width           = gXcb.width;
  fbInfo.layers          = 1;

  for(int i = 0; i < swapImageCount; ++i)
  {
    SwapImage* img      = &swapImages[i];
    fbInfo.pAttachments = &img->imageView;
    vkAssert(vkCreateFramebuffer(device, &fbInfo, allocator, &img->framebuffer));
  }
}

void App::initialise()
{
  // 1mb of stack, 64bytes alignment
  stackCreate(&pStack, 1024 * 1024, 64);

  swapImageIndex         = -1u;
  sharedRenderPass       = 0;
  acquireImageFenceIndex = 0;

  enableValidation();
  const char* extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME,      //
                              VK_KHR_XLIB_SURFACE_EXTENSION_NAME, //
                              VK_EXT_DEBUG_REPORT_EXTENSION_NAME, //
                              VK_KHR_XCB_SURFACE_EXTENSION_NAME};

  const char* LAYERS[] = {
      //"VK_LAYER_LUNARG_screenshot",           //
      "VK_LAYER_LUNARG_parameter_validation", //
      //"VK_LAYER_LUNARG_vktrace",              //
      "VK_LAYER_LUNARG_monitor",   //
      "VK_LAYER_LUNARG_swapchain", //
      //"VK_LAYER_GOOGLE_threading",            //
      "VK_LAYER_LUNARG_api_dump",        //
      "VK_LAYER_LUNARG_object_tracker",  //
      "VK_LAYER_LUNARG_core_validation", //
      "VK_LAYER_GOOGLE_unique_objects",  //
      //"VK_LAYER_RENDERDOC_Capture",           //
      "VK_LAYER_LUNARG_standard_validation", //
  };

  const int LAYER_COUNT = sizeof(LAYERS) / sizeof(char*);

  allocator = VK_NULL_HANDLE;

  // create instance
  VkMake(VkInstanceCreateInfo, info);
  info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  info.enabledExtensionCount   = sizeof(extensions) / sizeof(char*);
  info.ppEnabledExtensionNames = extensions;
  info.ppEnabledLayerNames     = LAYERS;
  info.enabledLayerCount       = LAYER_COUNT;

  vkAssert(vkCreateInstance(&info, allocator, &instance));
  vkprintf("VkInstance = %p", instance);

  // get physical device
  uint32_t count;
  vkEnumeratePhysicalDevices(instance, &count, 0);
  VkPhysicalDevice* phDevs = stackNew(pStack, VkPhysicalDevice, count);
  vkEnumeratePhysicalDevices(instance, &count, phDevs);

  vkGetPhysicalDeviceProperties(*phDevs, &pdProperties);
  vkGetPhysicalDeviceFeatures(*phDevs, &pdFeatures);
  vkGetPhysicalDeviceMemoryProperties(*phDevs, &pdMemoryProperties);
  physicalDevice = *phDevs;

// create surface
#ifdef XLIB
  initWindow(640, 360);
  VkMake(VkXlibSurfaceCreateInfoKHR, surfaceInfo);
  surfaceInfo.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  surfaceInfo.dpy    = gWnd.display;
  surfaceInfo.window = gWnd.window;
  vkAssert(vkCreateXlibSurfaceKHR(instance, &surfaceInfo, allocator, &surface));
#else
  initXcbWindow(640, 360);
  VkMake(VkXcbSurfaceCreateInfoKHR, surfaceInfo);
  surfaceInfo.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
  surfaceInfo.connection = gXcb.xcb_connection;
  surfaceInfo.window     = gXcb.xcb_window;
  vkAssert(vkCreateXcbSurfaceKHR(instance, &surfaceInfo, allocator, &surface));
#endif
  // get surface formats
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, 0);
  VkSurfaceFormatKHR* formats = stackNew(pStack, VkSurfaceFormatKHR, count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats);
  surfaceFormat = formats[0];
  // get surface caps
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps);

  // create device
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, NULL);
  VkQueueFamilyProperties* queueFamilyProps = stackNew(pStack, VkQueueFamilyProperties, count);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilyProps);

  // find one family supporting everything ( we're on nvidia, so will work )
  int queueFamily = -1;
  for(size_t i = 0; i < count; ++i)
  {
    if(queueFamilyProps[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
    {
      VkBool32 supported = 0;
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supported);
      if(supported)
      {
        queueFamily = i;
        break;
      }
    }
  }

  allQueueFamilyIndex = ( uint32_t )queueFamily;

  int queueCount = queueFamilyProps[queueFamily].queueCount;

  float* priorities = stackNew(pStack, float, queueCount);

  // create logical device
  for(int i = 0; i < queueCount; ++i)
  {
    priorities[i] = 1.0f;
  }
  VkMake(VkDeviceQueueCreateInfo, queueInfo);
  queueInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.pQueuePriorities = priorities;
  queueInfo.queueFamilyIndex = allQueueFamilyIndex;
  queueInfo.queueCount       = queueCount;

  const char* pDeviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkMake(VkDeviceCreateInfo, deviceInfo);
  deviceInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.ppEnabledExtensionNames = pDeviceExtensions;
  deviceInfo.enabledExtensionCount   = 1;
  deviceInfo.pEnabledFeatures        = &pdFeatures;
  deviceInfo.pQueueCreateInfos       = &queueInfo;
  deviceInfo.queueCreateInfoCount    = 1;

  vkAssert(vkCreateDevice(physicalDevice, &deviceInfo, allocator, &device));

  // get queue
  vkGetDeviceQueue(device, allQueueFamilyIndex, 0, &queue);

  // create command pool
  VkMake(VkCommandPoolCreateInfo, poolInfo);
  poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = allQueueFamilyIndex;
  poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  vkAssert(vkCreateCommandPool(device, &poolInfo, allocator, &commandPool));

  // create swapchain ( no depth )
  VkMake(VkSwapchainCreateInfoKHR, swapInfo);
  swapInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapInfo.clipped          = VK_TRUE;
  swapInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapInfo.imageArrayLayers = 1;
  swapInfo.imageColorSpace  = surfaceFormat.colorSpace;
  swapInfo.imageFormat      = surfaceFormat.format;
  swapInfo.imageExtent      = {( uint32_t )gXcb.width, ( uint32_t )gXcb.height};
  swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  swapInfo.minImageCount    = 3;
  swapInfo.presentMode      = VK_PRESENT_MODE_FIFO_KHR;
  swapInfo.preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapInfo.surface          = surface;
  vkAssert(vkCreateSwapchainKHR(device, &swapInfo, allocator, &swapchain));

  // get images
  vkGetSwapchainImagesKHR(device, swapchain, &count, NULL);
  VkImage* images = stackNew(pStack, VkImage, count);
  vkGetSwapchainImagesKHR(device, swapchain, &count, images);
  swapImageCount = count;

  VkImageSubresourceRange subrange;
  subrange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  subrange.baseArrayLayer = 0;
  subrange.baseMipLevel   = 0;
  subrange.layerCount     = 1;
  subrange.levelCount     = 1;

  VkMake(VkImageViewCreateInfo, ivInfo);
  ivInfo.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  ivInfo.format           = surfaceFormat.format;
  ivInfo.subresourceRange = subrange;
  ivInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
  ivInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

  VkMake(VkSemaphoreCreateInfo, seminfo);
  seminfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  for(int i = 0; i < count; ++i)
  {
    SwapImage* img = &swapImages[i];
    img->image     = images[i];

    ivInfo.image = images[i];
    vkAssert(vkCreateImageView(device, &ivInfo, allocator, &img->imageView));
    img->layout      = VK_IMAGE_LAYOUT_UNDEFINED;
    img->aspect      = VK_IMAGE_ASPECT_COLOR_BIT;
    img->framebuffer = 0;
    img->renderPass  = 0;
    // allocate command buffers per frame
    VkMake(VkCommandBufferAllocateInfo, cmdInfo);
    cmdInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdInfo.commandPool        = commandPool;
    cmdInfo.commandBufferCount = MAX_TRANSIENT_CMD_BUFFER_COUNT;
    cmdInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    // once allocated, never free
    vkAssert(vkAllocateCommandBuffers(device, &cmdInfo, img->cmdTransientBuffer));
    cmdInfo.commandBufferCount = 1;
    vkAssert(vkAllocateCommandBuffers(device, &cmdInfo, &img->cmdPrimaryBuffer));
    vkAssert(vkCreateSemaphore(device, &seminfo, allocator, &img->imagePresentSem));
    vkAssert(vkCreateSemaphore(device, &seminfo, allocator, &img->imageAcquireSem));
  }

  // create render pass and framebuffers
  createFramebufferRenderPass();

  // initialise primitives
  VkMake(VkFenceCreateInfo, fenceInfo);
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  for(int i = 0; i < 4; ++i)
  {
    vkAssert(vkCreateFence(device, &fenceInfo, allocator, &acquireImageFence[i]));
  }

  stackRollbackAll(pStack);

#ifdef XLIB
  pollWindow();
#endif

  //initialiseShaders();

  //initialiseDraw();

  // load texture
  pTexture = Texture::createRGBA2D(this, 1024, 1024, "/tmp/out.rgba");

  while(1)
  {
    beginFrame();

    endFrame();
    usleep(16000);
  }
}

void App::beginFrame()
{
  VkResult result = vkAcquireNextImageKHR(device, swapchain, 1000000, 0,
                                          acquireImageFence[acquireImageFenceIndex], &swapImageIndex);
  if(result != VK_SUCCESS)
  {
    // wait for fence
    waitForFences(&acquireImageFence[acquireImageFenceIndex], 1);
  }
  acquireImageFenceIndex = (acquireImageFenceIndex + 1) % 4;

  // begin recording main primary buffer for frame
  VkMake(VkCommandBufferBeginInfo, beginInfo);
  beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.pInheritanceInfo = 0;
  beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  SwapImage*      img        = &swapImages[swapImageIndex];
  VkCommandBuffer mainBuffer = img->cmdPrimaryBuffer;

  vkAssert(vkResetCommandBuffer(mainBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT));
  vkAssert(vkBeginCommandBuffer(mainBuffer, &beginInfo));

  // prepare barrier for first layout change
  VkMake(VkImageMemoryBarrier, barrier);
  VkImageSubresourceRange subrange;
  subrange.aspectMask         = VK_IMAGE_ASPECT_COLOR_BIT;
  subrange.baseArrayLayer     = 0;
  subrange.baseMipLevel       = 0;
  subrange.layerCount         = 1;
  subrange.levelCount         = 1;
  barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.subresourceRange    = subrange;
  barrier.srcAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
  barrier.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.dstQueueFamilyIndex = allQueueFamilyIndex;
  barrier.srcQueueFamilyIndex = allQueueFamilyIndex;
  barrier.oldLayout           = img->layout;
  barrier.newLayout = (img->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  barrier.image = img->image;

  // change image layout
  vkCmdPipelineBarrier(mainBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

  drawFrame(mainBuffer, swapImages[swapImageIndex].image, swapImages[swapImageIndex].framebuffer, sharedRenderPass);
}

void App::endFrame()
{
  SwapImage*      img        = &swapImages[swapImageIndex];
  VkCommandBuffer mainBuffer = img->cmdPrimaryBuffer;

  /*
  // change image layout
  VkMake(VkImageMemoryBarrier, barrier);
  VkImageSubresourceRange subrange;
  subrange.aspectMask         = VK_IMAGE_ASPECT_COLOR_BIT;
  subrange.baseArrayLayer     = 0;
  subrange.baseMipLevel       = 0;
  subrange.layerCount         = 1;
  subrange.levelCount         = 1;
  barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.subresourceRange    = subrange;
  barrier.srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_MEMORY_READ_BIT;
  barrier.dstQueueFamilyIndex = allQueueFamilyIndex;
  barrier.srcQueueFamilyIndex = allQueueFamilyIndex;
  barrier.oldLayout           = img->layout;
  barrier.newLayout = (img->layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  barrier.image = img->image;

  // change image layout
  vkCmdPipelineBarrier(mainBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);
  */
  vkAssert(vkEndCommandBuffer(mainBuffer));
  VkMake(VkSubmitInfo, submitInfo);
  submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &mainBuffer;
  submitInfo.pSignalSemaphores    = &img->imagePresentSem; // signal semaphore for presentation
  submitInfo.signalSemaphoreCount = 1;
  vkAssert(vkQueueSubmit(queue, 1, &submitInfo, 0));

  VkMake(VkPresentInfoKHR, presentInfo);
  presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.pImageIndices      = &swapImageIndex;
  presentInfo.pResults           = 0;
  presentInfo.pSwapchains        = &swapchain;
  presentInfo.swapchainCount     = 1;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = &img->imagePresentSem;
  vkAssert(vkQueuePresentKHR(queue, &presentInfo));

  //vkQueueWaitIdle( queue );
  img->layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void App::preDrawFrame(VkCommandBuffer primaryBuffer)
{
}

void App::postDrawFrame(VkCommandBuffer primaryBuffer)
{
}

//to override
void App::drawFrame(VkCommandBuffer primaryBuffer, VkImage swapImage, VkFramebuffer fb, VkRenderPass rp)
{
  static float k = 0.0f;
  k += 0.05f;
  if(k > 1.0f)
    k = k - 1.0f;

  float        col[] = {k, 0.0f, 0.0f, 1.0f};
  VkClearValue clearValue;
  memcpy(clearValue.color.float32, col, sizeof(float) * 4);

  VkRect2D area;
  area.offset.x      = 0;
  area.offset.y      = 0;
  area.extent.width  = gXcb.width;
  area.extent.height = gXcb.height;

  VkMake(VkRenderPassBeginInfo, rpinfo);
  rpinfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rpinfo.renderPass      = rp;
  rpinfo.framebuffer     = fb;
  rpinfo.clearValueCount = 1;
  rpinfo.pClearValues    = &clearValue;
  rpinfo.renderArea      = area;
  vkCmdBeginRenderPass(primaryBuffer, &rpinfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdEndRenderPass(primaryBuffer);

  VkImageMemoryBarrier barrier[] = {
      makeImageBarrier(pTexture->image, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL),
      makeImageBarrier(swapImage, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                       VK_ACCESS_MEMORY_WRITE_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)};
  VkImageMemoryBarrier barrier2[] = {

  makeImageBarrier(pTexture->image, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ),
  makeImageBarrier(swapImage, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR )};

  VkImageCopy region;
  VkExtent3D extent;
  extent.width = 200;
  extent.height = 200;
  extent.depth = 1;

  VkImageSubresourceLayers subLayers;
  subLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subLayers.mipLevel = 0;
  subLayers.baseArrayLayer = 0;
  subLayers.layerCount = 1;

  VkOffset3D offset;
  offset.x = 0;
  offset.y = 0;
  offset.z = 0;
  region.extent = extent;
  region.srcOffset = offset;
  region.dstOffset = offset;
  region.srcSubresource = subLayers;
  region.dstSubresource = subLayers;

  cmdPipelineImageBarrier(primaryBuffer, 2, barrier, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
  vkCmdCopyImage(primaryBuffer, pTexture->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapImage,
                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  cmdPipelineImageBarrier(primaryBuffer, 2, barrier2, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                          VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

}

// texture
Texture* Texture::createRGBA2D(class App* app, uint32_t width, uint32_t height, void* pData, size_t dataSize)
{
  Texture* texture = new Texture();
  texture->app     = app;
  VkMake(VkImageSubresourceLayers, subres);
  subres.mipLevel       = 0;
  subres.layerCount     = 1;
  subres.baseArrayLayer = 0;
  subres.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;

  VkMake(VkBufferCreateInfo, bufferInfo);
  VkMake(VkImageCreateInfo, imageInfo);
  VkMake(VkMemoryAllocateInfo, allocInfo);
  VkMemoryRequirements bufReq, imgReq;
  VkBuffer             buffer;
  VkImage              image;
  VkDeviceMemory       bufferMemory, imageMemory;

  VkExtent3D extent;
  extent.width  = width;
  extent.height = height;
  extent.depth  = 1;

  bufferInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size                  = width * height * 4;
  bufferInfo.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
  bufferInfo.pQueueFamilyIndices   = 0;
  bufferInfo.queueFamilyIndexCount = 0;

  imageInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.queueFamilyIndexCount = 0;
  imageInfo.pQueueFamilyIndices   = 0;
  imageInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.usage                 = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.mipLevels             = 1;
  imageInfo.arrayLayers           = 1;
  imageInfo.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.format                = VK_FORMAT_B8G8R8A8_UNORM;
  imageInfo.tiling                = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.extent                = extent;
  imageInfo.samples               = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.imageType             = VK_IMAGE_TYPE_2D;

  vkAssert(vkCreateBuffer(app->device, &bufferInfo, app->allocator, &buffer));
  vkAssert(vkCreateImage(app->device, &imageInfo, app->allocator, &image));
  vkGetBufferMemoryRequirements(app->device, buffer, &bufReq);
  vkGetImageMemoryRequirements(app->device, image, &imgReq);

  allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = bufReq.size;
  allocInfo.memoryTypeIndex =
      getMemoryIndex(bufReq.memoryTypeBits, &app->pdMemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  vkAssert(vkAllocateMemory(app->device, &allocInfo, app->allocator, &bufferMemory));
  allocInfo.allocationSize = imgReq.size;
  allocInfo.memoryTypeIndex =
      getMemoryIndex(imgReq.memoryTypeBits, &app->pdMemoryProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkAssert(vkAllocateMemory(app->device, &allocInfo, app->allocator, &imageMemory));

  vkAssert(vkBindBufferMemory(app->device, buffer, bufferMemory, 0));
  vkAssert(vkBindImageMemory(app->device, image, imageMemory, 0));

  // copy data to staging buffer
  void* ptr;
  vkAssert(vkMapMemory(app->device, bufferMemory, 0, VK_WHOLE_SIZE, 0, &ptr));
  memcpy(ptr, pData, dataSize);
  vkUnmapMemory(app->device, bufferMemory);

  // generate command buffer
  VkMake(VkCommandBufferAllocateInfo, bufInfo);
  bufInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufInfo.commandBufferCount = 1;
  bufInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufInfo.commandPool        = app->commandPool;
  vkAssert(vkAllocateCommandBuffers(app->device, &bufInfo, &texture->cmdBuffer));

  // create fence
  VkMake(VkFenceCreateInfo, fenceInfo);
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  vkAssert(vkCreateFence(app->device, &fenceInfo, app->allocator, &texture->resourceFence));

  // record layout change, copying resource and layout back to sampled image
  VkMake(VkCommandBufferBeginInfo, beginInfo);
  beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = nullptr;

  VkBufferImageCopy region;
  region.imageExtent      = extent;
  region.imageSubresource = subres;
  texture->image          = image;
  texture->layout         = VK_IMAGE_LAYOUT_UNDEFINED;
  VkImageMemoryBarrier barrier0 =
      texture->createLayoutChangeBarrier(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, VK_ACCESS_MEMORY_WRITE_BIT);
  VkImageMemoryBarrier barrier1 =
      texture->createLayoutChangeBarrier(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                         VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT,
                                         VK_ACCESS_MEMORY_WRITE_BIT);

  vkAssert(vkBeginCommandBuffer(texture->cmdBuffer, &beginInfo));
  vkCmdPipelineBarrier(texture->cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &barrier0);
  vkCmdCopyBufferToImage(texture->cmdBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
  vkCmdPipelineBarrier(texture->cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &barrier1);
  vkAssert(vkEndCommandBuffer(texture->cmdBuffer));

  // submit
  VkMake(VkSubmitInfo, submitInfo);
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &texture->cmdBuffer;
  vkAssert(vkQueueSubmit(app->queue, 1, &submitInfo, texture->resourceFence));
  // wait for fence
  while(vkWaitForFences(app->device, 1, &texture->resourceFence, VK_TRUE, 1000000) != VK_SUCCESS)
  {
  }
  vkAssert(vkResetFences(app->device, 1, &texture->resourceFence));

  // destroy staging buffer and memory
  vkDestroyBuffer(app->device, buffer, app->allocator);
  vkFreeMemory(app->device, bufferMemory, app->allocator);

  texture->deviceMemory = imageMemory;
  texture->image        = image;
  texture->width        = width;
  texture->height       = height;
  texture->layout       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  texture->format       = imageInfo.format;
  texture->sampler      = 0;
  texture->imageView    = 0;

  return texture;
}

Texture* Texture::createRGBA2D(App* app, uint32_t width, uint32_t height, const char* filename)
{
  FILE* f    = fopen(filename, "rb");
  char* data = ( char* )malloc(width * height * 4);
  fread(data, width * height * 4, 1, f);
  fclose(f);

  Texture* tex = createRGBA2D(app, width, height, data, width * height * 4);
  free(data);
  return tex;
}

//Texture* CreateRGBA2D( uint32_t width, uint32_t height, const char* filename );
VkImageMemoryBarrier Texture::createLayoutChangeBarrier(VkImageLayout newLayout,
                                                        VkAccessFlags srcAccess, VkAccessFlags dstAccess)
{
  VkMake(VkImageMemoryBarrier, barrier);
  VkMake(VkImageSubresourceRange, subres);
  subres.levelCount     = 1;
  subres.layerCount     = 1;
  subres.baseMipLevel   = 0;
  subres.baseArrayLayer = 0;
  subres.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;

  barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image               = image;
  barrier.srcQueueFamilyIndex = 0;
  barrier.oldLayout           = layout;
  barrier.newLayout           = newLayout;
  barrier.srcAccessMask       = srcAccess;
  barrier.dstAccessMask       = dstAccess;
  barrier.subresourceRange    = subres;

  layout = newLayout;

  return barrier;
}

int meshCreateWithData( App* pApp, size_t vertexCount, size_t vertexSize, Mesh** ppMesh )
{
  Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
  VkBuffer buffer;
  VkDeviceMemory memory;
  VkMake( VkBufferCreateInfo, bufferInfo );
  VkMake( VkMemoryAllocateInfo, memInfo );
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = (vertexCount*vertexSize);
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  vkAssert( vkCreateBuffer( pApp->device, &bufferInfo, pApp->allocator, &buffer ) );
  VkMemoryRequirements req;
  vkGetBufferMemoryRequirements( pApp->device, buffer, &req );
  memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memInfo.memoryTypeIndex = getMemoryIndex( req.memoryTypeBits, &pApp->pdMemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
  memInfo.allocationSize = req.size;
  vkAssert( vkAllocateMemory( pApp->device, &memInfo, pApp->allocator, &memory ) );
  vkAssert( vkBindBufferMemory( pApp->device, buffer, memory, 0 ) );
  *ppMesh = mesh;
  mesh->app = pApp;
  mesh->memory = memory;
  mesh->isMutable = 1;
  mesh->vertexCount = vertexCount;
  mesh->vertexSize = vertexSize;
  mesh->vertices = buffer;
  return 0;
}

void* meshMap( Mesh* pMesh )
{
  if( !pMesh->isMutable )
    return NULL;

  void* ptr = NULL;
  vkAssert( vkMapMemory( pMesh->app->device, pMesh->memory, 0, VK_WHOLE_SIZE, 0, &ptr ) );
  return ptr;
}

void meshUnmap( Mesh* pMesh )
{
  vkUnmapMemory( pMesh->app->device, pMesh->memory );
}

void meshMakeImmutable( Mesh* pMesh )
{
  VkBuffer buffer;
  VkDeviceMemory memory;
  VkMake( VkBufferCreateInfo, bufferInfo );
  VkMake( VkMemoryAllocateInfo, memInfo );
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = (pMesh->vertexCount*pMesh->vertexSize);
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  vkAssert( vkCreateBuffer( pMesh->app->device, &bufferInfo, pMesh->app->allocator, &buffer ) );
  VkMemoryRequirements req;
  vkGetBufferMemoryRequirements( pMesh->app->device, buffer, &req );
  memInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memInfo.memoryTypeIndex = getMemoryIndex( req.memoryTypeBits, &pMesh->app->pdMemoryProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
  memInfo.allocationSize = req.size;
  vkAssert( vkAllocateMemory( pMesh->app->device, &memInfo, pMesh->app->allocator, &memory ) );
  vkAssert( vkBindBufferMemory( pMesh->app->device, buffer, memory, 0 ) );

  // copy
  VkCommandBuffer cmdbuf;
  VkMake( VkCommandBufferAllocateInfo, cmdbufInfo );
  cmdbufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdbufInfo.commandBufferCount = 1;
  cmdbufInfo.commandPool = pMesh->app->commandPool;
  cmdbufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  vkAssert( vkAllocateCommandBuffers( pMesh->app->device, &cmdbufInfo, &cmdbuf) );

  VkMake( VkSubmitInfo, info );
  VkMake( VkFenceCreateInfo, fenceInfo );
  info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  info.commandBufferCount = 1;
  info.pCommandBuffers = &cmdbuf;


}

int main(int argc, char** argv)
{
  App app;
  app.initialise();
  return 0;
}



// data
unsigned char VERTEX_SHADER_SPV[] = {
    0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x08, 0x00, 0x2c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0f, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
    0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00,
    0x26, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0xae, 0x01, 0x00, 0x00,
    0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x06, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x50, 0x65, 0x72, 0x56, 0x65,
    0x72, 0x74, 0x65, 0x78, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00,
    0x06, 0x00, 0x07, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x50,
    0x6f, 0x69, 0x6e, 0x74, 0x53, 0x69, 0x7a, 0x65, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x07, 0x00,
    0x0b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x43, 0x6c, 0x69, 0x70, 0x44,
    0x69, 0x73, 0x74, 0x61, 0x6e, 0x63, 0x65, 0x00, 0x05, 0x00, 0x03, 0x00, 0x0d, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x11, 0x00, 0x00, 0x00, 0x77, 0x6f, 0x72, 0x6c,
    0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x70, 0x72, 0x6f, 0x6a, 0x56, 0x69, 0x65, 0x77, 0x4d, 0x61, 0x74, 0x00, 0x05, 0x00, 0x03, 0x00,
    0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x19, 0x00, 0x00, 0x00,
    0x61, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,
    0x23, 0x00, 0x00, 0x00, 0x76, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,
    0x26, 0x00, 0x00, 0x00, 0x61, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00,
    0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x48, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x0b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x48, 0x00, 0x04, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x13, 0x00, 0x00, 0x00,
    0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x13, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x19, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x05, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00,
    0x0e, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00,
    0x0e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x04, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x12, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x12, 0x00, 0x00, 0x00,
    0x13, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x14, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x17, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x19, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x20, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00,
    0x23, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00,
    0x26, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x14, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
    0x13, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x16, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x17, 0x00, 0x00, 0x00,
    0x1a, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x1d, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x50, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x91, 0x00, 0x05, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00,
    0x41, 0x00, 0x05, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
    0x0f, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x22, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x3d, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00,
    0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00,
    0x27, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x2a, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00,
    0x2a, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x23, 0x00, 0x00, 0x00,
    0x2b, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00};
unsigned int VERTEX_SHADER_LEN = 1308;

unsigned char FRAGMENT_SHADER_SPV[] = {
    0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x08, 0x00, 0x0d, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0f, 0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
    0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
    0xae, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x09, 0x00, 0x00, 0x00, 0x6f, 0x75, 0x74, 0x43,
    0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x76, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00,
    0x0b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00,
    0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x0b, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x09, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00};
unsigned int FRAGMENT_SHADER_LEN = 376;
