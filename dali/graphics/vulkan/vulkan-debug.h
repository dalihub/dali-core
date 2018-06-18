#ifndef DALI_GRAPHICS_VULKAN_DEBUG_H
#define DALI_GRAPHICS_VULKAN_DEBUG_H

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

#if defined(DEBUG_ENABLED)

#include <iostream>
#include <cstdarg>
#include <string>


extern const char* LOG_VULKAN;


std::string ArgListToString( const char* format, va_list args );

std::string FormatToString( const char* format, ... );

#define DALI_LOG_STREAM( filter, level, stream )  \
  if( nullptr != LOG_VULKAN)                      \
  {                                               \
    std::cout << stream << std::endl;             \
  }

#define DALI_LOG_INFO( filter, level, format, ... ) \
  if( nullptr != LOG_VULKAN )                       \
  {                                                 \
    std::cout << FormatToString( (format), ##__VA_ARGS__ ); \
  }

#else
#define DALI_LOG_STREAM( filter, level, stream )
#endif

#endif // DALI_GRAPHICS_VULKAN_DEBUG_H
