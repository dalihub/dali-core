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
#include <dali/integration-api/events/multi-point-event-integ.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

MultiPointEvent::MultiPointEvent( Type eventType )
: Event( eventType ),
  time( 0 )
{
}

MultiPointEvent::MultiPointEvent( Type eventType, unsigned long time )
: Event( eventType ),
  time( time )
{
}

MultiPointEvent::~MultiPointEvent()
{
}

void MultiPointEvent::AddPoint( const Point& point )
{
  points.push_back(point);
}

Point& MultiPointEvent::GetPoint( unsigned int point )
{
  DALI_ASSERT_ALWAYS(point < points.size() && "MultiPointEvent: Point index out of bounds");
  return points[point];
}

const Point& MultiPointEvent::GetPoint( unsigned int point ) const
{
  DALI_ASSERT_ALWAYS(point < points.size() && "MultiPointEvent: Point index out of bounds");
  return points[point];
}

unsigned int MultiPointEvent::GetPointCount() const
{
  return points.size();
}

} // namespace Integration

} // namespace Dali
