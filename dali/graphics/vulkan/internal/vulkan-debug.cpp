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
 */

#include <dali/graphics/vulkan/internal/vulkan-debug.h>
#include <dali/graphics/vulkan/internal/vulkan-hpp-wrapper.h>
#include <dali/integration-api/debug.h>
#include <stack>
#include <cstdlib>
#include <cstdio>

#if defined(DEBUG_ENABLED)

std::string ArgListToString( const char* format, va_list args )
{
  std::string str; // empty string
  if( format != nullptr )
  {
    char* buffer = nullptr;
    int err = vasprintf( &buffer, format, args );
    if( err >= 0 ) // No error
    {
      str = buffer;
      free( buffer );
    }
  }
  return str;
}

std::string FormatToString( const char* format, ... )
{
  va_list arg;
  va_start( arg, format );
  std::string s = ArgListToString( format, arg );
  va_end( arg );
  return s;
}


const char* LOG_VULKAN( std::getenv( "LOG_VULKAN" ) );

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

thread_local std::stack<BlackBox*> gBlackBoxStack;

void BlackBox::push()
{
  gBlackBoxStack.push( this );
}

void BlackBox::pop()
{
  gBlackBoxStack.pop();
}

BlackBox& BlackBox::get()
{
  return *gBlackBoxStack.top();
}

}
}
}

#endif


#if defined(DEBUG_REPORT_CALLBACK_ENABLED)

#undef LOG_TAG
#define LOG_TAG "VALIDATION"

VkBool32 VulkanReportCallback(
  VkDebugReportFlagsEXT                       flags,
  VkDebugReportObjectTypeEXT                  objectType,
  uint64_t                                    object,
  size_t                                      location,
  int32_t                                     messageCode,
  const char*                                 pLayerPrefix,
  const char*                                 pMessage,
  void*                                       pUserData)
{
  //LOGE("VALIDATION: %s", pMessage );
  printf("VALIDATION: %s", pMessage );
  return VK_FALSE;
}

void CreateDebugReportCallback( vk::Instance& instance, int logLevel )
{
  VkDebugReportCallbackCreateInfoEXT reportInfo;
  reportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  reportInfo.pfnCallback = VulkanReportCallback;
  reportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT;

  if ( logLevel == 1 )
  {
    DALI_LOG_ERROR("LOG_LEVEL VK_DEBUG_REPORT_ERROR_BIT_EXT %d\n", logLevel);
    reportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT;
  }
  else if ( logLevel == 2 )
  {
    DALI_LOG_ERROR("LOG_LEVEL VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT %d\n", logLevel);
    reportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  }
  else if ( logLevel == 3 )
  {
    DALI_LOG_ERROR("LOG_LEVEL VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT %d\n", logLevel);
    reportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                      VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
  }
  else if ( logLevel == 4 )
  {
    DALI_LOG_ERROR("LOG_LEVEL VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | \
                    VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT %d\n", logLevel);
    reportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                      VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
  }
  else if ( logLevel == 5 )
  {
    DALI_LOG_ERROR("LOG_LEVEL VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | \
                    VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT \
                    %d\n", logLevel);
    reportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                      VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                      VK_DEBUG_REPORT_DEBUG_BIT_EXT;
  }
  else
  {
    DALI_LOG_ERROR("LOG_LEVEL VK_DEBUG_REPORT_ERROR_BIT_EXT %d\n", logLevel);
  }

  reportInfo.pUserData = 0u;


  VkDebugReportCallbackEXT vkCallback;
  PFN_vkCreateDebugReportCallbackEXT func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>( instance.getProcAddr( "vkCreateDebugReportCallbackEXT" ));
  func( static_cast<VkInstance>(instance), &reportInfo, nullptr, &vkCallback );
}
#endif
