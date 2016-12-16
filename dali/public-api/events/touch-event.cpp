/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/touch-event.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

TouchEvent::TouchEvent()
: time(0)
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: TouchEvent() is deprecated and will be removed from next release. Use TouchData instead.\n" );
}

TouchEvent::TouchEvent(unsigned long time)
: time(time)
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: TouchEvent() is deprecated and will be removed from next release. Use TouchData instead.\n" );
}

TouchEvent::~TouchEvent()
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: ~TouchEvent() is deprecated and will be removed from next release. Use TouchData instead.\n" );
}

unsigned int TouchEvent::GetPointCount() const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetPointCount() is deprecated and will be removed from next release. Use TouchData instead.\n" );

  return points.size();
}

const TouchPoint& TouchEvent::GetPoint(unsigned int point) const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetPoint() is deprecated and will be removed from next release. Use TouchData instead.\n" );

  DALI_ASSERT_ALWAYS( point < points.size() && "No point at index" );
  return points[point];
}

} // namespace Dali
