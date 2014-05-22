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

//
// gcc and clang minimal demangling
// Both follow Itanium C++ ABI
//
// We only decode namespaces and class typeid names for simplicity as its all we need.
//
// From http://mentorembedded.github.io/cxx-abi/abi.html#mangling-structure
//
// <nested-name> ::= N [<CV-qualifiers>] <prefix> <unqualified-name> E
//     ::= N [<CV-qualifiers>] <template-prefix> <template-args> E
//
// <prefix> ::= <prefix> <unqualified-name>
//     ::= <template-prefix> <template-args>
//          ::= <template-param>
//          ::= <decltype>
//     ::= # empty
//     ::= <substitution>
//          ::= <prefix> <data-member-prefix>
//
// <template-prefix> ::= <prefix> <template unqualified-name>
//                   ::= <template-param>
//                   ::= <substitution>
// <unqualified-name> ::= <operator-name>
//                    ::= <ctor-dtor-name>
//                    ::= <source-name>
//                    ::= <unnamed-type-name>
//
// <source-name> ::= <positive length number> <identifier>
// <number> ::= [n] <non-negative decimal integer>
// <identifier> ::= <unqualified source code identifier>
//
// So for example
//
// Dali::Internal::Actor would be
//
//   N4Dali8Internal5ActorE
//

// CLASS HEADER
#include <dali/internal/event/common/demangler.h>

namespace
{

// true if character represent a digit
inline bool IsDigit(char c)
{
  return (c >= '0' && c <= '9');
}

// Gets the number of characters (number is in string)
//   start The start position to look for a number
//   result The number as an integer
//   returns the number of characters used to define the number ie '12' is 2
size_t GetNumberOfCharacters(const std::string& s, const size_t& start, int& result)
{
  size_t size = s.size();

  size_t i = start;

  int number = 0;

  for( ; i < size; ++i )
  {
    char c = s.at(i);
    if( !IsDigit( c ) )
    {
      break;
    }
    else
    {
      number = 10 * number + (c - '0');
    }
  }

  if( i - start )
  {
    result = number;
  }

  return i - start;
}

} // anon namespace


namespace Dali
{

namespace Internal
{

#if defined(__clang__) || defined(__GNUC__)

// Demangle class name mangled according to the Itanium C++ ABI
// Returns demangled names ie "N4Dali8Internal5ActorE" is ["Dali","Internal","Actor"]
std::vector<std::string> DemangleNestedNames(const char *typeIdName)
{
  std::vector<std::string> ret;

  const std::string mangledName(typeIdName);


  size_t size = mangledName.size();

  if( size >= 2 )
  {
    int number = 0;
    size_t start = 0;

    // If the class isnt nested in a namespace then it just starts with the
    // number of characters
    if(mangledName[0] == 'N' && mangledName[size-1]  == 'E')
    {
      start = 1;
    }

    while( size_t chars = GetNumberOfCharacters(mangledName, start, number) )
    {
      ret.push_back( mangledName.substr( start + chars, number ) );

      start += chars + number;
    }
  }

  return ret;
}

#else
# error Unsupported Compiler
#endif


const std::string DemangleClassName(const char *typeIdName)
{
  std::string name;
  std::vector<std::string> names = DemangleNestedNames(typeIdName);

  if( names.size() )
  {
    name = names[ names.size() - 1 ];
  }

  return name;
}

} // namespace Internal

} // namespace Dali
