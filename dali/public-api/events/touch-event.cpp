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
#include <dali/public-api/events/touch-event.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

TouchEvent::TouchEvent()
: time(0)
{
}

TouchEvent::TouchEvent(unsigned long time)
: time(time)
{
}

TouchEvent::~TouchEvent()
{
}

unsigned int TouchEvent::GetPointCount() const
{
  return points.size();
}

const TouchPoint& TouchEvent::GetPoint(unsigned int point) const
{
  DALI_ASSERT_ALWAYS( point < points.size() && "No point at index" );
  return points[point];
}

} // namespace Dali
