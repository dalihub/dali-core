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
#include <dali/internal/event/events/tap-gesture-detector-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/events/gesture-event-processor.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Signals
const char* const SIGNAL_TAP_DETECTED = "tap-detected";

BaseHandle Create()
{
  return Dali::TapGestureDetector::New();
}

TypeRegistration mType( typeid(Dali::TapGestureDetector), typeid(Dali::GestureDetector), Create );

SignalConnectorType signalConnector1( mType, SIGNAL_TAP_DETECTED, &TapGestureDetector::DoConnectSignal );

}

TapGestureDetectorPtr TapGestureDetector::New()
{
  return new TapGestureDetector;
}

TapGestureDetectorPtr TapGestureDetector::New(unsigned int tapsRequired, unsigned int touchesRequired)
{
  return new TapGestureDetector(tapsRequired, touchesRequired);
}

TapGestureDetector::TapGestureDetector()
: GestureDetector(Gesture::Tap),
  mTapsRequired(1),
  mTouchesRequired(1)
{
}

TapGestureDetector::TapGestureDetector(unsigned int tapsRequired, unsigned int touchesRequired)
: GestureDetector(Gesture::Tap),
  mTapsRequired(tapsRequired),
  mTouchesRequired(touchesRequired)
{
}

TapGestureDetector::~TapGestureDetector()
{
}

void TapGestureDetector::SetTapsRequired(unsigned int taps)
{
  DALI_ASSERT_ALWAYS(taps > 0 && "Can only set a positive number of taps" );

  if (mTapsRequired != taps)
  {
    mTapsRequired = taps;

    if (!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void TapGestureDetector::SetTouchesRequired(unsigned int touches)
{
  DALI_ASSERT_ALWAYS(touches > 0 && "Can only set a positive number of touches" );

  if (mTouchesRequired != touches)
  {
    mTouchesRequired = touches;

    if (!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

unsigned int TapGestureDetector::GetTapsRequired() const
{
  return mTapsRequired;
}

unsigned int TapGestureDetector::GetTouchesRequired() const
{
  return mTouchesRequired;
}

void TapGestureDetector::EmitTapGestureSignal(Dali::Actor tappedActor, const TapGesture& tap)
{
  // Guard against destruction during signal emission
  Dali::TapGestureDetector handle( this );

  mDetectedSignal.Emit( tappedActor, tap );
}

bool TapGestureDetector::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  TapGestureDetector* gesture = dynamic_cast<TapGestureDetector*>(object);

  if ( 0 == strcmp( signalName.c_str(), SIGNAL_TAP_DETECTED ) )
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

void TapGestureDetector::OnActorAttach(Actor& actor)
{
  // Do nothing
}

void TapGestureDetector::OnActorDetach(Actor& actor)
{
  // Do nothing
}

void TapGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

} // namespace Internal

} // namespace Dali
