//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <stdlib.h>
#include <string>
#include <cstdio>
#include <execinfo.h>
#include <cxxabi.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace
{
const int MAX_NUM_STACK_FRAMES = 25;
const size_t C_SYMBOL_LENGTH = 4096;
}

namespace Dali
{

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

  return result;
}

DALI_EXPORT_API DaliException::DaliException(const char *location, const char* condition)
: mLocation(location), mCondition(condition)
{
  // Note, if a memory error has occured, then the backtrace won't work - backtrace_symbols relies on
  // allocating memory.

  // Initial dlog error message is output in DALI_ASSERT_ALWAYS macro
  // Also output on stderr
  fprintf(stderr, "Exception: \n%s\n thrown at %s\nSee dlog for backtrace\n", mCondition.c_str(), mLocation.c_str());

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

DALI_EXPORT_API void DaliAssertMessage(const char* condition, const char* file, int line)
{
  DALI_LOG_ERROR_NOFN( "Assertion (%s) failed in: %s:%d\n", condition, file, line );
}

} // Dali
