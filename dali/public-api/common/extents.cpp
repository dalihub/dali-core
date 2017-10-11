/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/extents.h>

namespace Dali
{

Extents::Extents()
: start(0),
  end(0),
  top(0),
  bottom(0)
  {
  }

Extents::Extents( uint16_t start, uint16_t end, uint16_t top, uint16_t bottom )
: start( start ),
  end( end ),
  top( top ),
  bottom( bottom )
  {
  }

Extents& Extents::operator=( const uint16_t* array )
{
  start = array[0];
  end = array[1];
  top = array[2];
  bottom = array[3];

  return *this;
}

bool Extents::operator==( const Extents &rhs ) const
{
  return ( start == rhs.start )&&
    ( end == rhs.end )&&
    ( top == rhs.top )&&
    ( bottom == rhs.bottom );
}

bool Extents::operator!=( const Extents &rhs ) const
{
  return !( *this == rhs );
}

std::ostream& operator<<( std::ostream& stream, const Extents& extents )
{
  return stream << "[" << extents.start << ", " << extents.end << ", " << extents.top << ", " << extents.bottom << "]";
}

} // Dali
