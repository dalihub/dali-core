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
#include "long-press-gesture-detector-impl.h"

// INTERNAL INCLUDES
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/events/gesture-event-processor.h>

namespace Dali
{

namespace Internal
{

namespace
{
BaseHandle Create()
{
  return Dali::LongPressGestureDetector::New();
}

TypeRegistration mType( typeid(Dali::LongPressGestureDetector), typeid(Dali::GestureDetector), Create );

SignalConnectorType signalConnector1( mType, Dali::LongPressGestureDetector::SIGNAL_LONG_PRESS_DETECTED, &LongPressGestureDetector::DoConnectSignal );

}


namespace
{
const unsigned int DEFAULT_TOUCHES_REQUIRED = 1;
} // unnamed namespace

LongPressGestureDetectorPtr LongPressGestureDetector::New()
{
  return new LongPressGestureDetector;
}

LongPressGestureDetectorPtr LongPressGestureDetector::New(unsigned int touchesRequired)
{
  return new LongPressGestureDetector(touchesRequired, touchesRequired);
}

LongPressGestureDetectorPtr LongPressGestureDetector::New(unsigned int minTouches, unsigned int maxTouches)
{
  return new LongPressGestureDetector(minTouches, maxTouches);
}

LongPressGestureDetector::LongPressGestureDetector()
: GestureDetector(Gesture::LongPress),
  mMinimumTouchesRequired(DEFAULT_TOUCHES_REQUIRED),
  mMaximumTouchesRequired(DEFAULT_TOUCHES_REQUIRED)
{
}

LongPressGestureDetector::LongPressGestureDetector(unsigned int minTouches, unsigned int maxTouches)
: GestureDetector(Gesture::LongPress),
  mMinimumTouchesRequired(minTouches),
  mMaximumTouchesRequired(maxTouches)
{
}

LongPressGestureDetector::~LongPressGestureDetector()
{
}

void LongPressGestureDetector::SetTouchesRequired(unsigned int touches)
{
  DALI_ASSERT_ALWAYS( touches > 0 && "Can only set a positive number of required touches" );

  if (mMinimumTouchesRequired != touches || mMaximumTouchesRequired != touches)
  {
    mMinimumTouchesRequired = mMaximumTouchesRequired = touches;

    if (!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void LongPressGestureDetector::SetTouchesRequired(unsigned int minTouches, unsigned int maxTouches)
{
  DALI_ASSERT_ALWAYS(minTouches > 0 && "Can only set a positive number of minimum touches");
  DALI_ASSERT_ALWAYS(maxTouches > 0 && "Can only set a positive number of minimum touches");
  DALI_ASSERT_ALWAYS(minTouches <= maxTouches && "Number of minimum touches must be less than maximum");

  if (mMinimumTouchesRequired != minTouches || mMaximumTouchesRequired != maxTouches)
  {
    mMinimumTouchesRequired = minTouches;
    mMaximumTouchesRequired = maxTouches;

    if (!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

unsigned int LongPressGestureDetector::GetMinimumTouchesRequired() const
{
  return mMinimumTouchesRequired;
}

unsigned int LongPressGestureDetector::GetMaximumTouchesRequired() const
{
  return mMaximumTouchesRequired;
}

void LongPressGestureDetector::EmitLongPressGestureSignal(Dali::Actor pressedActor, const LongPressGesture& longPress)
{
  // Guard against destruction during signal emission
  Dali::LongPressGestureDetector handle( this );

  mDetectedSignalV2.Emit( pressedActor, longPress );
}

bool LongPressGestureDetector::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  LongPressGestureDetector* gesture = dynamic_cast<LongPressGestureDetector*>(object);

  if ( Dali::LongPressGestureDetector::SIGNAL_LONG_PRESS_DETECTED == signalName )
  {
    gesture->DetectedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void LongPressGestureDetector::OnActorAttach(Actor& actor)
{
  // Do nothing
}

void LongPressGestureDetector::OnActorDetach(Actor& actor)
{
  // Do nothing
}

void LongPressGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

} // namespace Internal

} // namespace Dali
