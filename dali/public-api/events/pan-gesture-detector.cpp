//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/events/pan-gesture-detector.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/pan-gesture-detector-impl.h>

namespace Dali
{

const char* const PanGestureDetector::SIGNAL_PAN_DETECTED = "pan-detected";

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

void PanGestureDetector::SetMinimumTouchesRequired(unsigned int minimum)
{
  GetImplementation(*this).SetMinimumTouchesRequired(minimum);
}

void PanGestureDetector::SetMaximumTouchesRequired(unsigned int maximum)
{
  GetImplementation(*this).SetMaximumTouchesRequired(maximum);
}

unsigned int PanGestureDetector::GetMinimumTouchesRequired() const
{
  return GetImplementation(*this).GetMinimumTouchesRequired();
}

unsigned int PanGestureDetector::GetMaximumTouchesRequired() const
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

const PanGestureDetector::AngleContainer& PanGestureDetector::GetAngles() const
{
  return GetImplementation(*this).GetAngles();
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

PanGestureDetector::DetectedSignalV2& PanGestureDetector::DetectedSignal()
{
  return GetImplementation(*this).DetectedSignal();
}

void PanGestureDetector::SetPanGestureProperties( const PanGesture& pan )
{
  Internal::PanGestureDetector::SetPanGestureProperties( pan );
}

} // namespace Dali
