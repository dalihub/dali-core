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
#include <dali/integration-api/events/wheel-event-integ.h>

namespace Dali
{

namespace Integration
{

WheelEvent::WheelEvent()
: Event( Wheel ),
  type( MOUSE_WHEEL ),
  direction( 0 ),
  modifiers( 0 ),
  point( Vector2::ZERO ),
  z( 0 ),
  timeStamp( 0 )
{
}

WheelEvent::WheelEvent( Type type, int direction, unsigned int modifiers, Vector2 point, int z, unsigned int timeStamp )
: Event( Wheel ),
  type( type ),
  direction( direction ),
  modifiers( modifiers ),
  point( point ),
  z( z ),
  timeStamp( timeStamp )
{
}

WheelEvent::~WheelEvent()
{
}

} // namespace Integration

} // namespace Dali
