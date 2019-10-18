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

// CLASS HEADER
#include <dali/devel-api/events/rotation-gesture.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/devel-api/events/gesture-devel.h>

namespace Dali
{

RotationGesture::RotationGesture( Gesture::State state )
: Gesture( static_cast< Gesture::Type >( DevelGesture::Rotation ), state )
{
}

RotationGesture::RotationGesture( const RotationGesture& rhs )
: Gesture( rhs ),
  rotation( rhs.rotation ),
  screenCenterPoint( rhs.screenCenterPoint ),
  localCenterPoint( rhs.localCenterPoint )
{
}

RotationGesture& RotationGesture::operator=( const RotationGesture& rhs )
{
  if( this != &rhs )
  {
    Gesture::operator=( rhs );
    rotation = rhs.rotation;
    screenCenterPoint = rhs.screenCenterPoint;
    localCenterPoint = rhs.localCenterPoint;
  }

  return *this;
}

RotationGesture::~RotationGesture()
{
}

} // namespace Dali
