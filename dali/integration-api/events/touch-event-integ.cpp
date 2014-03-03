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
#include <dali/integration-api/events/touch-event-integ.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Integration
{

TouchEvent::TouchEvent()
: Event(Touch),
  time(0)
{
}

TouchEvent::TouchEvent(unsigned long time)
: Event(Touch),
  time(time)
{
}

TouchEvent::~TouchEvent()
{
}

void TouchEvent::AddPoint(const TouchPoint& point)
{
  points.push_back(point);
}

TouchPoint& TouchEvent::GetPoint(unsigned int point)
{
  DALI_ASSERT_ALWAYS(point < points.size() && "TouchEvent: Point index out of bounds");
  return points[point];
}

const TouchPoint& TouchEvent::GetPoint(unsigned int point) const
{
  DALI_ASSERT_ALWAYS(point < points.size() && "TouchEvent: Point index out of bounds");
  return points[point];
}

unsigned int TouchEvent::GetPointCount() const
{
  return points.size();
}

} // namespace Integration

} // namespace Dali
