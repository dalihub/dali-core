/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali/internal/event/common/demangler.h>

// EXTERNAL HEADER
#include <string_view>

namespace Dali
{
namespace Internal
{
std::string DemangleClassName(const char* typeIdName)
{
  std::string name  = typeIdName;
  int         index = name.find_last_of(' ');

  if(0 <= index)
  {
    name = name.substr(index + 1, name.size() - index);
  }

  index = name.find_last_of(':');

  if(0 <= index)
  {
    name = name.substr(index + 1, name.size() - index);
  }

  return name;
}

std::string DemangleTypeInfoName(const char* typeIdName)
{
  if(!typeIdName || typeIdName[0] == '\0')
  {
    return {};
  }

  // MSVC's typeid(T).name() returns human-readable strings with a leading
  // keyword prefix for non-primitive types:
  // "int" (primitives — no prefix)
  // "class Dali::Actor" (class types)
  // "struct Foo::Bar" (struct types)
  // "enum MyEnum" (enum types)
  // "union MyUnion" (union types)
  // Strip the leading prefix so the result matches what DALI_TYPE_ID() stringifies.
  //
  // Limitation: MSVC expands default template arguments in typeid().name(), so
  // typeid(std::vector<int>).name() yields "class std::vector<int,class std::allocator<int> >"
  // which, even after prefix stripping, will not match DALI_TYPE_ID(std::vector<int>)
  // ("std::vector<int>"). The same mismatch occurs on GCC/Clang after __cxa_demangle.
  // This is a fundamental limitation of mixing preprocessor stringification (DALI_TYPE_ID)
  // with typeid()-based lookup (TypeInfoIdFromTypeid) for types with defaulted template arguments.
  // In practice this is not a problem because the types stored in Any are primitives,
  // DALi value types, and pointer types — none of which are affected. For template
  // types, always use IsType<T>() rather than comparing against DALI_TYPE_ID(), since
  // both sides then go through TypeInfoIdFromTypeid and are guaranteed consistent.
  static const char* prefixes[] = {"class ", "struct ", "enum ", "union "};

  std::string name(typeIdName);
  for(const char* prefix : prefixes)
  {
    std::string_view p(prefix);
    if(name.compare(0, p.size(), prefix) == 0)
    {
      name.erase(0, p.size());
      break;
    }
  }

  return name;
}

} // namespace Internal

} // namespace Dali
