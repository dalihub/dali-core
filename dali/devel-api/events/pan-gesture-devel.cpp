/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/pan-gesture-devel.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/pan-gesture/pan-gesture-impl.h>


namespace Dali
{

namespace DevelPanGesture
{

Dali::PanGesture New( GestureState state )
{
  return PanGesture( new Internal::PanGesture( state ) );
}

void SetTime( Dali::PanGesture& gesture, uint32_t time )
{
  GetImplementation( gesture ).SetTime( time );
}

void SetVelocity( Dali::PanGesture& gesture, const Vector2& velocity)
{
  GetImplementation( gesture ).SetVelocity( velocity );
}

void SetDisplacement( Dali::PanGesture& gesture, const Vector2& displacement)
{
  GetImplementation( gesture ).SetDisplacement( displacement );
}

void SetPosition( Dali::PanGesture& gesture, const Vector2& position)
{
  GetImplementation( gesture ).SetPosition( position );
}

void SetScreenVelocity( Dali::PanGesture& gesture, const Vector2& screenVelocity)
{
  GetImplementation( gesture ).SetScreenVelocity( screenVelocity );
}

void SetScreenDisplacement( Dali::PanGesture& gesture, const Vector2& screenDisplacement)
{
  GetImplementation( gesture ).SetScreenDisplacement( screenDisplacement );
}

void SetScreenPosition( Dali::PanGesture& gesture, const Vector2& screenPosition)
{
  GetImplementation( gesture ).SetScreenPosition( screenPosition );
}

void SetNumberOfTouches( Dali::PanGesture& gesture, uint32_t numberOfTouches )
{
  GetImplementation( gesture ).SetNumberOfTouches( numberOfTouches );
}

} // namespace DevelPanGesture

} // namespace Dali
