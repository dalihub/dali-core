/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// EXTERNAL HEADER
#include <string_view>

// FILE HEADER
#include <dali/internal/event/common/demangler.h>

namespace
{
// Extracts the number from the src view and update the view.
size_t ExtractNumber(std::string_view& src)
{
  auto   IsDigit = [](char c) { return (c >= '0' && c <= '9'); };
  size_t number  = 0;

  for(auto i = 0u; i < src.size(); ++i)
  {
    char c = src[i];
    if(!IsDigit(c))
    {
      //update the src view.
      src.remove_prefix(i);
      break;
    }
    else
    {
      number = 10 * number + (c - '0');
    }
  }

  return number;
}

/**
 * @brief Demangle a nested typeid name into its component parts and return
 * the last component.
 * A nested type name is one containing namespaces and class names only.
 *   eg ExtractDemangleNestedName(typeid(Dali::Actor).name());
 * @param[in] typeIdName The type id name string to demangle.
 * @returns the last component "Actor" or an empty string_view
 */
std::string_view ExtractDemangleNestedName(std::string_view mangledName)
{
  if(mangledName.empty())
  {
    return {};
  }

  // classes nested inside a namespace starts with 'N' and ends with 'E'
  // so trim those
  if(mangledName.front() == 'N' && mangledName.back() == 'E')
  {
    mangledName.remove_prefix(1);
    mangledName.remove_suffix(1);
  }

  std::string_view result;
  while(!mangledName.empty())
  {
    auto length = ExtractNumber(mangledName);
    result      = {mangledName.data(), length};
    mangledName.remove_prefix(length);
  }

  return result;
}

} // namespace

namespace Dali
{
namespace Internal
{
std::string DemangleClassName(const char* typeIdName)
{
  return std::string(ExtractDemangleNestedName(typeIdName));
}

} // namespace Internal

} // namespace Dali
