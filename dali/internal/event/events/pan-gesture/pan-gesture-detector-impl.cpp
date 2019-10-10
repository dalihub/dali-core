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
#include <dali/internal/event/events/pan-gesture/pan-gesture-detector-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/degree.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties

//              Name                  Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "screenPosition",      VECTOR2, false, false, true,   Dali::PanGestureDetector::Property::SCREEN_POSITION     )
DALI_PROPERTY( "screenDisplacement",  VECTOR2, false, false, true,   Dali::PanGestureDetector::Property::SCREEN_DISPLACEMENT )
DALI_PROPERTY( "screenVelocity",      VECTOR2, false, false, true,   Dali::PanGestureDetector::Property::SCREEN_VELOCITY     )
DALI_PROPERTY( "localPosition",       VECTOR2, false, false, true,   Dali::PanGestureDetector::Property::LOCAL_POSITION      )
DALI_PROPERTY( "localDisplacement",   VECTOR2, false, false, true,   Dali::PanGestureDetector::Property::LOCAL_DISPLACEMENT  )
DALI_PROPERTY( "localVelocity",       VECTOR2, false, false, true,   Dali::PanGestureDetector::Property::LOCAL_VELOCITY      )
DALI_PROPERTY( "panning",             BOOLEAN, false, false, true,   Dali::PanGestureDetector::Property::PANNING             )
DALI_PROPERTY_TABLE_END( DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX, PanGestureDetectorDefaultProperties )

// Signals

const char* const SIGNAL_PAN_DETECTED = "panDetected";

BaseHandle Create()
{
  return Dali::PanGestureDetector::New();
}

TypeRegistration mType( typeid(Dali::PanGestureDetector), typeid(Dali::GestureDetector), Create, PanGestureDetectorDefaultProperties );

SignalConnectorType signalConnector1( mType, SIGNAL_PAN_DETECTED, &PanGestureDetector::DoConnectSignal );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_PAN_GESTURE_DETECTOR");
#endif

/**
 * Returns the angle going in the opposite direction to that specified by angle.
 */
float GetOppositeAngle( float angle )
{
  // Calculate the opposite angle so that we cover both directions.
  if ( angle <= 0.0f )
  {
    angle += Math::PI;
  }
  else
  {
    angle -= Math::PI;
  }

  return angle;
}

} // unnamed namespace

PanGestureDetectorPtr PanGestureDetector::New()
{
  const SceneGraph::PanGesture& sceneObject = ThreadLocalStorage::Get().GetGestureEventProcessor().GetPanGestureProcessor().GetSceneObject();
  return new PanGestureDetector( sceneObject );
}

void PanGestureDetector::SetMinimumTouchesRequired(unsigned int minimum)
{
  DALI_ASSERT_ALWAYS( minimum > 0 && "Can only set a positive number of required touches" );

  if (mMinimumTouches != minimum)
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "Minimum Touches Set: %d\n", minimum );

    mMinimumTouches = minimum;

    if (!mAttachedActors.empty())
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "Updating Gesture Detector\n");

      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

void PanGestureDetector::SetMaximumTouchesRequired(unsigned int maximum)
{
  DALI_ASSERT_ALWAYS( maximum > 0 && "Can only set a positive number of maximum touches" );

  if (mMaximumTouches != maximum)
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "Maximum Touches Set: %d\n", maximum );

    mMaximumTouches = maximum;

    if (!mAttachedActors.empty())
    {
      DALI_LOG_INFO( gLogFilter, Debug::General, "Updating Gesture Detector\n");

      mGestureEventProcessor.GestureDetectorUpdated(this);
    }
  }
}

uint32_t PanGestureDetector::GetMinimumTouchesRequired() const
{
  return mMinimumTouches;
}

uint32_t PanGestureDetector::GetMaximumTouchesRequired() const
{
  return mMaximumTouches;
}

void PanGestureDetector::AddAngle( Radian angle, Radian threshold )
{
  threshold = fabsf( threshold ); // Ensure the threshold is positive.

  // If the threshold is greater than PI, then just use PI
  // This means that any panned angle will invoke the pan gesture. We should still add this angle as
  // an angle may have been added previously with a small threshold.
  if ( threshold > Math::PI )
  {
    threshold = Math::PI;
  }

  angle = WrapInDomain( angle, -Math::PI, Math::PI );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Angle Added: %.2f, Threshold: %.2f\n", Degree(angle), Degree(threshold) );

  AngleThresholdPair pair( angle, threshold );
  mAngleContainer.push_back( pair );
}

void PanGestureDetector::AddDirection( Radian direction, Radian threshold )
{
  AddAngle( direction, threshold );

  // Calculate the opposite angle so that we cover the entire direction.
  direction = GetOppositeAngle( direction );

  AddAngle( direction, threshold );
}

uint32_t PanGestureDetector::GetAngleCount() const
{
  return static_cast<uint32_t>( mAngleContainer.size() );
}

PanGestureDetector::AngleThresholdPair PanGestureDetector::GetAngle(uint32_t index) const
{
  PanGestureDetector::AngleThresholdPair ret( Radian(0),Radian(0) );

  if( index < mAngleContainer.size() )
  {
    ret = mAngleContainer[index];
  }

  return ret;
}


void PanGestureDetector::ClearAngles()
{
  mAngleContainer.clear();
}

void PanGestureDetector::RemoveAngle( Radian angle )
{
  angle = WrapInDomain( angle, -Math::PI, Math::PI );

  for (AngleContainer::iterator iter = mAngleContainer.begin(), endIter = mAngleContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == angle )
    {
      mAngleContainer.erase( iter );
      break;
    }
  }
}

void PanGestureDetector::RemoveDirection( Radian direction )
{
  RemoveAngle( direction );

  // Calculate the opposite angle so that we cover the entire direction.
  direction = GetOppositeAngle( direction );

  RemoveAngle( direction );
}

bool PanGestureDetector::RequiresDirectionalPan() const
{
  // If no directional angles have been added to the container then we do not require directional panning
  return !mAngleContainer.empty();
}

bool PanGestureDetector::CheckAngleAllowed( Radian angle ) const
{
  bool allowed( false );
  if ( mAngleContainer.empty() )
  {
    allowed = true;
  }
  else
  {
    for ( AngleContainer::const_iterator iter = mAngleContainer.begin(), endIter = mAngleContainer.end(); iter != endIter; ++iter )
    {
      float angleAllowed( iter->first );
      float threshold ( iter->second );

      DALI_LOG_INFO( gLogFilter, Debug::General,
                     "AngleToCheck: %.2f, CompareWith: %.2f, Threshold: %.2f\n",
                     Degree(angle), Degree(angleAllowed), Degree(threshold) );

      float relativeAngle( fabsf( WrapInDomain( angle - angleAllowed, -Math::PI, Math::PI ) ) );
      if ( relativeAngle <= threshold )
      {
        allowed = true;
        break;
      }
    }
  }

  return allowed;
}

void PanGestureDetector::EmitPanGestureSignal(Dali::Actor actor, const PanGesture& pan)
{
  if ( !mDetectedSignal.Empty() )
  {
    // Guard against destruction during signal emission
    Dali::PanGestureDetector handle( this );

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Emitting Signal (%p)\n", this );

    mDetectedSignal.Emit( actor, pan );
  }
}

bool PanGestureDetector::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  PanGestureDetector* gesture = static_cast< PanGestureDetector* >(object); // TypeRegistry guarantees that this is the correct type.

  if ( 0 == strcmp( signalName.c_str(), SIGNAL_PAN_DETECTED ) )
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

void PanGestureDetector::SetPanGestureProperties( const PanGesture& pan )
{
  ThreadLocalStorage::Get().GetGestureEventProcessor().SetGestureProperties( pan );
}

PanGestureDetector::PanGestureDetector( const SceneGraph::PanGesture& sceneObject )
: GestureDetector(Gesture::Pan, &sceneObject ),
  mMinimumTouches(1),
  mMaximumTouches(1)
{
}

PanGestureDetector::~PanGestureDetector()
{
}

const SceneGraph::PanGesture& PanGestureDetector::GetPanGestureSceneObject() const
{
  return static_cast<const SceneGraph::PanGesture&>( GetSceneObject() );
}

void PanGestureDetector::OnActorAttach(Actor& actor)
{
  // Do nothing
}

void PanGestureDetector::OnActorDetach(Actor& actor)
{
  // Do nothing
}

void PanGestureDetector::OnActorDestroyed(Object& object)
{
  // Do nothing
}

void PanGestureDetector::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  // None of our properties should be settable from Public API
}

Property::Value PanGestureDetector::GetDefaultProperty( Property::Index index ) const
{
  return GetDefaultPropertyCurrentValue( index ); // Scene-graph only properties
}

Property::Value PanGestureDetector::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  Property::Value value;

  switch ( index )
  {
    case Dali::PanGestureDetector::Property::SCREEN_POSITION:
    {
      value = GetPanGestureSceneObject().GetScreenPositionProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_DISPLACEMENT:
    {
      value = GetPanGestureSceneObject().GetScreenDisplacementProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_VELOCITY:
    {
      value = GetPanGestureSceneObject().GetScreenVelocityProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_POSITION:
    {
      value = GetPanGestureSceneObject().GetLocalPositionProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_DISPLACEMENT:
    {
      value = GetPanGestureSceneObject().GetLocalDisplacementProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_VELOCITY:
    {
      value = GetPanGestureSceneObject().GetLocalVelocityProperty().Get();
      break;
    }

    case Dali::PanGestureDetector::Property::PANNING:
    {
      value = GetPanGestureSceneObject().GetPanningProperty().Get();
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "PanGestureDetector Property index invalid" ); // should not come here
      break;
    }
  }

  return value;
}

const PropertyInputImpl* PanGestureDetector::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property = nullptr;

  switch ( index )
  {
    case Dali::PanGestureDetector::Property::SCREEN_POSITION:
    {
      property = &GetPanGestureSceneObject().GetScreenPositionProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_DISPLACEMENT:
    {
      property = &GetPanGestureSceneObject().GetScreenDisplacementProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::SCREEN_VELOCITY:
    {
      property = &GetPanGestureSceneObject().GetScreenVelocityProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_POSITION:
    {
      property = &GetPanGestureSceneObject().GetLocalPositionProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_DISPLACEMENT:
    {
      property = &GetPanGestureSceneObject().GetLocalDisplacementProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::LOCAL_VELOCITY:
    {
      property = &GetPanGestureSceneObject().GetLocalVelocityProperty();
      break;
    }

    case Dali::PanGestureDetector::Property::PANNING:
    {
      property = &GetPanGestureSceneObject().GetPanningProperty();
      break;
    }

    default:
      break;
  }
  if( !property )
  {
    // not our property, ask base
    property = Object::GetSceneObjectInputProperty( index );
  }

  return property;
}

} // namespace Internal

} // namespace Dali
