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
 *
 */

// CLASS HEADER
#include <dali/public-api/events/hover-event.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

HoverEvent::HoverEvent()
: time(0)
{
}

HoverEvent::HoverEvent(unsigned long time)
: time(time)
{
}

HoverEvent::~HoverEvent()
{
}

uint32_t HoverEvent::GetPointCount() const
{
  return static_cast<uint32_t>( points.size() );
}

const TouchPoint& HoverEvent::GetPoint( uint32_t point ) const
{
  DALI_ASSERT_ALWAYS( point < points.size() && "No point at index" );
  return points[point];
}

} // namespace Dali
