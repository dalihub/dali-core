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
#include <dali/public-api/events/pan-gesture-detector.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/pan-gesture-detector-impl.h>

namespace Dali
{

const Radian PanGestureDetector::DIRECTION_LEFT( -Math::PI );
const Radian PanGestureDetector::DIRECTION_RIGHT( 0.0f );
const Radian PanGestureDetector::DIRECTION_UP( -0.5f * Math::PI );
const Radian PanGestureDetector::DIRECTION_DOWN( 0.5f * Math::PI );
const Radian PanGestureDetector::DIRECTION_HORIZONTAL( Math::PI );
const Radian PanGestureDetector::DIRECTION_VERTICAL( -0.5f * Math::PI );
const Radian PanGestureDetector::DEFAULT_THRESHOLD( 0.25f * Math::PI );

PanGestureDetector::PanGestureDetector(Internal::PanGestureDetector* internal)
: GestureDetector(internal)
{
}

PanGestureDetector::PanGestureDetector()
{
}

PanGestureDetector PanGestureDetector::New()
{
  Internal::PanGestureDetectorPtr internal = Internal::PanGestureDetector::New();

  return PanGestureDetector(internal.Get());
}

PanGestureDetector PanGestureDetector::DownCast( BaseHandle handle )
{
  return PanGestureDetector( dynamic_cast<Dali::Internal::PanGestureDetector*>(handle.GetObjectPtr()) );
}

PanGestureDetector::~PanGestureDetector()
{
}

PanGestureDetector::PanGestureDetector(const PanGestureDetector& handle)
: GestureDetector(handle)
{
}

PanGestureDetector& PanGestureDetector::operator=(const PanGestureDetector& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void PanGestureDetector::SetMinimumTouchesRequired(uint32_t minimum)
{
  GetImplementation(*this).SetMinimumTouchesRequired(minimum);
}

void PanGestureDetector::SetMaximumTouchesRequired(uint32_t maximum)
{
  GetImplementation(*this).SetMaximumTouchesRequired(maximum);
}

uint32_t PanGestureDetector::GetMinimumTouchesRequired() const
{
  return GetImplementation(*this).GetMinimumTouchesRequired();
}

uint32_t PanGestureDetector::GetMaximumTouchesRequired() const
{
  return GetImplementation(*this).GetMaximumTouchesRequired();
}

void PanGestureDetector::AddAngle( Radian angle, Radian threshold )
{
  GetImplementation(*this).AddAngle( angle, threshold );
}

void PanGestureDetector::AddDirection( Radian direction, Radian threshold )
{
  GetImplementation(*this).AddDirection( direction, threshold );
}

size_t PanGestureDetector::GetAngleCount() const
{
  return GetImplementation(*this).GetAngleCount();
}

PanGestureDetector::AngleThresholdPair PanGestureDetector::GetAngle(size_t index) const
{
  return GetImplementation(*this).GetAngle(index);
}

void PanGestureDetector::ClearAngles()
{
  GetImplementation(*this).ClearAngles();
}

void PanGestureDetector::RemoveAngle( Radian angle )
{
  GetImplementation(*this).RemoveAngle( angle );
}

void PanGestureDetector::RemoveDirection( Radian direction )
{
  GetImplementation(*this).RemoveDirection( direction );
}

PanGestureDetector::DetectedSignalType& PanGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

void PanGestureDetector::SetPanGestureProperties( const PanGesture& pan )
{
  Internal::PanGestureDetector::SetPanGestureProperties( pan );
}

} // namespace Dali
