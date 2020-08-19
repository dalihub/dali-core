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

// INTERNAL INCLUDES
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/devel-api/events/touch-data-devel.h>
#include <dali/internal/event/events/touch-data-impl.h>

namespace Dali
{

namespace DevelTouchData
{

TouchData Convert( const TouchEvent& touchEvent )
{
  Internal::TouchDataPtr touchDataImpl( new Internal::TouchData(touchEvent.time ) );

  for( auto&& touchEventPoint : touchEvent.points )
  {
    touchDataImpl->AddPoint( Integration::Point( touchEventPoint ) );
  }

  return TouchData( touchDataImpl.Get() );
}

} // namespace DevelTouchData

} // namespace Dali
