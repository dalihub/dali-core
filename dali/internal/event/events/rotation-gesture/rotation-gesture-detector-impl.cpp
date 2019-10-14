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
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/devel-api/events/gesture-devel.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/events/rotation-gesture.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Signals

const char* const SIGNAL_ROTATION_DETECTED = "rotationDetected";

BaseHandle Create()
{
  return Dali::RotationGestureDetector::New();
}

TypeRegistration mType( typeid( Dali::RotationGestureDetector ), typeid( Dali::GestureDetector ), Create );

SignalConnectorType signalConnector1( mType, SIGNAL_ROTATION_DETECTED, &RotationGestureDetector::DoConnectSignal );

}


RotationGestureDetectorPtr RotationGestureDetector::New()
{
  return new RotationGestureDetector;
}

RotationGestureDetector::RotationGestureDetector()
: GestureDetector( DevelGesture::Rotation )
{
}

void RotationGestureDetector::EmitRotationGestureSignal( Dali::Actor actor, const RotationGesture& rotation )
{
  // Guard against destruction during signal emission
  Dali::RotationGestureDetector handle( this );

  mDetectedSignal.Emit( actor, rotation );
}

bool RotationGestureDetector::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  RotationGestureDetector* gesture = static_cast< RotationGestureDetector* >( object ); // TypeRegistry guarantees that this is the correct type.

  if ( 0 == strcmp( signalName.c_str(), SIGNAL_ROTATION_DETECTED ) )
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

} // namespace Internal

} // namespace Dali
