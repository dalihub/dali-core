/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/long-press-gesture.h>

namespace Dali
{

LongPressGesture::LongPressGesture(Gesture::State state)
: Gesture( Gesture::LongPress, state ),
  numberOfTouches( 1 )
{
}

LongPressGesture::LongPressGesture( const LongPressGesture& rhs )
: Gesture( rhs ),
  numberOfTouches( rhs.numberOfTouches ),
  screenPoint( rhs.screenPoint ),
  localPoint( rhs.localPoint )
{
}

LongPressGesture& LongPressGesture::operator=( const LongPressGesture& rhs )
{
  if( this != &rhs )
  {
    Gesture::operator=(rhs);
    numberOfTouches = rhs.numberOfTouches;
    screenPoint = rhs.screenPoint;
    localPoint = rhs.localPoint;
  }

  return *this;
}

LongPressGesture::~LongPressGesture()
{
}

} // namespace Dali
