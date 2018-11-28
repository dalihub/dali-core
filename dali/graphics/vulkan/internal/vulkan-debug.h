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
#undef DEBUG_ENABLED
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#if defined(DEBUG_ENABLED)


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
#define DALI_LOG_INFO( filter, level, format, ... )
#endif


namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

struct Config
{
  std::vector<std::string> split( const std::string& str, char separator )
  {
    std::vector<std::string> strings;
    std::istringstream f(str);
    std::string s;
    while (getline(f, s, separator ))
      {
        strings.push_back(s);
      }
    return std::move(strings);
  }

  Config()
  {
    auto fin = fopen("/home/owner/dali.conf", "rb");
    if( fin )
      {
        char tag[256];
        auto value = fgets( tag, 256, fin );
        if( value )
          {
            tag[strlen (tag) - 1] = 0;
            auto result = split (tag, '=');
            if (result.size () == 2)
              {
                config[result[0]] = result[1];
              }
          }
      }
    fclose( fin );
  }

  std::string Get( const std::string& tag )
  {
    auto it = config.find( tag );
    if( it != config.end() )
      {
        return it->second;
      }
    return {};
  }

  int GetNumber( const std::string& tag )
  {
    auto it = config.find( tag );
    if( it != config.end() )
      {
        return atoi(it->second.c_str());
      }
    return {};
  }

  std::vector<std::string> GetStringArray(const std::string& tag)
  {
    auto it = config.find( tag );
    if( it != config.end() )
      {
        return split( it->second, ',');
      }
    return {};
  }

  void PrintConfig()
  {
    for( auto& keyValue : config )
      {
        std::cout << keyValue.first << " = '" << keyValue.second << "'" << std::endl;
      }
  }

  std::map<std::string, std::string> config;
};

}
}
}


namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 * Helper function converting Vulkan C++ types into void* ( for logging )
 */
template<class T, class K>
const void* VkVoidCast( const K& o )
{
  return static_cast<T>(o);
}

#ifdef DEBUG_ENABLED
struct BlackBox
{
  std::queue<std::string> debugLog;

  template<class T>
  BlackBox& operator <<(const T& value )
  {
    stream << value;
    return *this;
  }

  BlackBox& log()
  {
    stream.str(std::string());
    return *this;
  }

  const std::string& end()
  {
    static const std::string terminator("");
    debugLog.push( stream.str() );
    if( debugLog.size() > 128 )
    {
      debugLog.pop();
    }
    return terminator;
  }

  void push();

  static void pop();

  static BlackBox& get();

  std::stringstream stream;
};
#endif


} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali


//#define DEBUG_REPORT_CALLBACK_ENABLED

#if defined(DEBUG_REPORT_CALLBACK_ENABLED)
namespace vk
{
class Instance;
}

void CreateDebugReportCallback( vk::Instance& instance, int logLevel );
#endif

#endif // DALI_GRAPHICS_VULKAN_DEBUG_H
