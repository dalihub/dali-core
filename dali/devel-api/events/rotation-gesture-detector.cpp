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
#include <dali/devel-api/events/rotation-gesture-detector.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-detector-impl.h>

namespace Dali
{

RotationGestureDetector::RotationGestureDetector( Internal::RotationGestureDetector* internal )
: GestureDetector( internal )
{
}

RotationGestureDetector::RotationGestureDetector()
{
}

RotationGestureDetector RotationGestureDetector::New()
{
  Internal::RotationGestureDetectorPtr internal = Internal::RotationGestureDetector::New();

  return RotationGestureDetector( internal.Get() );
}

RotationGestureDetector RotationGestureDetector::DownCast( BaseHandle handle )
{
  return RotationGestureDetector( dynamic_cast<Dali::Internal::RotationGestureDetector*>( handle.GetObjectPtr() ) );
}

RotationGestureDetector::~RotationGestureDetector()
{
}

RotationGestureDetector::RotationGestureDetector( const RotationGestureDetector& handle )
: GestureDetector( handle )
{
}

RotationGestureDetector& RotationGestureDetector::operator=( const RotationGestureDetector& rhs )
{
  BaseHandle::operator=( rhs );
  return *this;
}

RotationGestureDetector::DetectedSignalType& RotationGestureDetector::DetectedSignal()
{
  return GetImplementation( *this ).DetectedSignal();
}

} // namespace Dali
