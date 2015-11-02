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

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

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

const unsigned int DEFAULT_TAPS_REQUIRED = 1u;
const unsigned int DEFAULT_TOUCHES_REQUIRED = 1u;

// Signals
const char* const SIGNAL_TAP_DETECTED = "tapDetected";

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

TapGestureDetectorPtr TapGestureDetector::New( unsigned int tapsRequired )
{
  return new TapGestureDetector( tapsRequired );
}

TapGestureDetector::TapGestureDetector()
: GestureDetector( Gesture::Tap ),
  mMinimumTapsRequired( DEFAULT_TAPS_REQUIRED ),
  mMaximumTapsRequired( DEFAULT_TAPS_REQUIRED ),
  mTouchesRequired( DEFAULT_TOUCHES_REQUIRED )
{
}

TapGestureDetector::TapGestureDetector( unsigned int tapsRequired )
: GestureDetector( Gesture::Tap ),
  mMinimumTapsRequired( tapsRequired ),
  mMaximumTapsRequired( tapsRequired ),
  mTouchesRequired( DEFAULT_TOUCHES_REQUIRED )
{
}

TapGestureDetector::~TapGestureDetector()
{
}

void TapGestureDetector::SetMinimumTapsRequired(unsigned int taps)
{
  if ( mMinimumTapsRequired != taps )
  {
    mMinimumTapsRequired = taps;

    if ( !mAttachedActors.empty() )
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void TapGestureDetector::SetMaximumTapsRequired(unsigned int taps)
{
  if ( mMaximumTapsRequired != taps )
  {
    mMaximumTapsRequired = taps;

    if ( !mAttachedActors.empty() )
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void TapGestureDetector::SetTouchesRequired(unsigned int touches)
{
  if (mTouchesRequired != touches)
  {
    mTouchesRequired = touches;

    if (!mAttachedActors.empty())
    {
      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

unsigned int TapGestureDetector::GetMinimumTapsRequired() const
{
  return mMinimumTapsRequired;
}

unsigned int TapGestureDetector::GetMaximumTapsRequired() const
{
  return mMaximumTapsRequired;
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
