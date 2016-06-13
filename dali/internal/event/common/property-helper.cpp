/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// HEADER
#include <dali/internal/event/common/property-helper.h>

namespace Dali
{

namespace Internal
{

bool CompareTokens( const char * first, const char * second, size_t& size )
{
  size = 0;
  while( ( *first != '\0' ) && ( *second != '\0' ) && ( *first != ',') && ( *second != ',') )
  {
    ++size;
    char ca = *first;
    char cb = *second;

    if( ( ( ca == '-' ) || ( ca == '_') ) &&
        ( ( cb == '-' ) || ( cb == '_') ) )
    {
      ++first;
      ++second;
      continue;
    }

    if( ( 'A' <= ca ) && ( ca <= 'Z') )
    {
      ca = ca + ( 'a' - 'A' );
    }

    if( ( 'A' <= cb ) && ( cb <= 'Z') )
    {
      cb = cb + ( 'a' - 'A' );
    }

    if( ca != cb )
    {
      return false;
    }

    ++first;
    ++second;
  }

  // enums can be comma separated so check ends and comma
  if( ( ( *first == '\0' ) && ( *second == '\0' ) ) ||
      ( ( *first == '\0' ) && ( *second == ','  ) ) ||
      ( ( *first == ','  ) && ( *second == '\0' ) ) )
  {
    return true;
  }

  return false;
}

} // namespace Internal

} // namespace Dali
