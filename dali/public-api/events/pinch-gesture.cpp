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
#include <dali/public-api/events/pinch-gesture.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

PinchGesture::PinchGesture(Gesture::State state)
: Gesture(Gesture::Pinch, state),
  scale(0.0f),
  speed(0.0f)
{
}

PinchGesture::PinchGesture( const PinchGesture& rhs )
: Gesture( rhs ),
  scale( rhs.scale ),
  speed( rhs.speed ),
  screenCenterPoint( rhs.screenCenterPoint ),
  localCenterPoint( rhs.localCenterPoint )
{
}

PinchGesture& PinchGesture::operator=( const PinchGesture& rhs )
{
  Gesture::operator=(rhs);
  scale = rhs.scale;
  speed = rhs.speed;
  screenCenterPoint = rhs.screenCenterPoint;
  localCenterPoint = rhs.localCenterPoint;

  return *this;
}

PinchGesture::~PinchGesture()
{
}

} // namespace Dali
