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
#include <stack>
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


const char* LOG_VULKAN( getenv( "LOG_VULKAN" ) );

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

