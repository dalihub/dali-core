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
#include <dali/public-api/events/pan-gesture.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

PanGesture::PanGesture(Gesture::State state)
: Gesture(Gesture::Pan, state),
  numberOfTouches(1)
{
}

PanGesture::PanGesture( const PanGesture& rhs )
: Gesture( rhs ),
  velocity( rhs.velocity ),
  displacement( rhs.displacement ),
  position( rhs.position ),
  screenVelocity( rhs.screenVelocity ),
  screenDisplacement( rhs.screenDisplacement ),
  screenPosition( rhs.screenPosition ),
  numberOfTouches( rhs.numberOfTouches )
{
}

PanGesture& PanGesture::operator=( const PanGesture& rhs )
{
  Gesture::operator=(rhs);
  velocity = rhs.velocity;
  displacement = rhs.displacement;
  position = rhs.position;
  screenVelocity = rhs.screenVelocity;
  screenDisplacement = rhs.screenDisplacement;
  screenPosition = rhs.screenPosition;
  numberOfTouches = rhs.numberOfTouches;

  return *this;
}

PanGesture::~PanGesture()
{
}

float PanGesture::GetSpeed() const
{
  return velocity.Length();
}

float PanGesture::GetDistance() const
{
  return displacement.Length();
}

float PanGesture::GetScreenSpeed() const
{
  return screenVelocity.Length();
}

float PanGesture::GetScreenDistance() const
{
  return screenDisplacement.Length();
}

} // namespace Dali
