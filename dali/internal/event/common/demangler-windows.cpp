/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Internal
{

const std::string DemangleClassName(const char *typeIdName)
{
  std::string name = typeIdName;
  int index = name.find_last_of(' ');

  if( 0 <= index )
  {
    name = name.substr( index + 1, name.size() - index );
  }

  index = name.find_last_of(':');

  if( 0 <= index )
  {
    name = name.substr(index + 1, name.size() - index);
  }

  return name;
}

} // namespace Internal

} // namespace Dali
