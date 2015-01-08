/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
 *
 */

// CLASS HEADER
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <stdlib.h>
#include <string>
#include <cstdio>

#ifndef EMSCRIPTEN // cxxabi not supported
# include <execinfo.h>
# include <cxxabi.h>
#endif

#include <cstring>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace
{
const int MAX_NUM_STACK_FRAMES = 25;
}

namespace Dali
{

#if defined(BACKTRACE_ENABLED)

std::string Demangle(const char* symbol)
{
  std::string result;

  // backtrace ::=  <filename>'('['_'<mangled-symbol>'_']['+'<offset>]')'
  // Only want <mangled-symbol>:
  const char* openParen = strchr(symbol, '(');
  if(openParen != NULL)
  {
    const char* startOfToken = openParen + 1;
    const char* plus = strchr(startOfToken, '+');
    const char* closeParen = strchr(startOfToken, ')');
    const char* endOfToken = NULL;
    if(plus != NULL)
    {
      endOfToken = plus;
    }
    else if(closeParen != NULL)
    {
      endOfToken = closeParen;
    }
    if(endOfToken != NULL)
    {
      size_t tokenLength = endOfToken - startOfToken;

      // Allocate space for symbol
      char *mangledSymbol = (char*)malloc(tokenLength+1u);
      if(mangledSymbol != NULL)
      {
        strncpy(mangledSymbol, startOfToken, tokenLength);
        mangledSymbol[tokenLength] = '\0';

        size_t size;
        int    status;
        char*  demangled=NULL;
        demangled = abi::__cxa_demangle( mangledSymbol, NULL, &size, &status );
        if( demangled != NULL )
        {
          result = demangled;
          free(demangled); // demangle() allocates returned string, so free it
        }
        else
        {
          result = symbol;
        }
        free(mangledSymbol);
      }
    }
  }

  return result;
}

DALI_EXPORT_API DaliException::DaliException( const char* location, const char* condition )
: location( location ), condition( condition )
{
  // Note, if a memory error has occured, then the backtrace won't work - backtrace_symbols relies on
  // allocating memory.

  // Initial dlog error message is output in DALI_ASSERT_ALWAYS macro
  // Also output on stderr
#if defined(DEBUG_ENABLED)
  fprintf(stderr, "Exception: \n%s\n thrown at %s\nSee dlog for backtrace\n", condition, location);
#else
  fprintf(stderr, "Exception: \n%s\n thrown\nSee dlog for backtrace\n", condition );
#endif

  DALI_LOG_ERROR_NOFN("Backtrace:\n");

  void* frameArray[MAX_NUM_STACK_FRAMES];
  int nSize = backtrace(frameArray, MAX_NUM_STACK_FRAMES);
  char** symbols = backtrace_symbols(frameArray, nSize);
  for(int i=1; i< nSize; i++)
  {
    std::string demangled_symbol = Demangle(symbols[i]);
    DALI_LOG_ERROR_NOFN("[%02d]   %s\n", i, demangled_symbol.c_str() );
  }
  free(symbols);
}


#else // BACKTRACE_ENABLED

DALI_EXPORT_API DaliException::DaliException( const char* location, const char* condition )
: location( location ), condition( condition )
{
#if defined(DEBUG_ENABLED)
  printf("Exception: \n%s\n thrown at %s\n", condition, location );
#else
  printf("Exception: \n%s\n thrown\n", condition );
#endif
}


#endif // BACKTRACE_ENABLED

DALI_EXPORT_API void DaliAssertMessage( const char* location, const char* condition )
{
#if defined(DEBUG_ENABLED)
  DALI_LOG_ERROR_NOFN( "Assert (%s) failed in: %s\n", condition, location );
#else
  DALI_LOG_ERROR_NOFN( "Assert (%s) failed\n", condition );
#endif
}

} // Dali
