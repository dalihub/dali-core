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
#include <dali/public-api/events/wheel-event.h>

namespace Dali
{

namespace
{
const uint32_t MODIFIER_SHIFT = 0x1;
const uint32_t MODIFIER_CTRL  = 0x2;
const uint32_t MODIFIER_ALT   = 0x4;

}

WheelEvent::WheelEvent()
: type( MOUSE_WHEEL ),
  direction( 0 ),
  modifiers( 0 ),
  point( Vector2::ZERO ),
  z( 0 ),
  timeStamp( 0 )
{
}

WheelEvent::WheelEvent( Type type, int32_t direction, uint32_t modifiers, Vector2 point, int32_t z, uint32_t timeStamp )
: type( type ),
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

bool WheelEvent::IsShiftModifier() const
{
  if ((MODIFIER_SHIFT & modifiers) == MODIFIER_SHIFT)
  {
    return true;
  }

  return false;
}

bool WheelEvent::IsCtrlModifier() const
{
  if ((MODIFIER_CTRL & modifiers) == MODIFIER_CTRL)
  {
    return true;
  }

  return false;
}

bool WheelEvent::IsAltModifier() const
{
  if ((MODIFIER_ALT & modifiers) == MODIFIER_ALT)
  {
    return true;
  }

  return false;
}

} // namespace Dali
