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
#include <dali/internal/event/events/pan-gesture-detector-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/degree.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/property-index-ranges.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>

namespace Dali
{

const Property::Index PanGestureDetector::SCREEN_POSITION      = Internal::DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT;
const Property::Index PanGestureDetector::SCREEN_DISPLACEMENT  = Internal::DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT + 1;
const Property::Index PanGestureDetector::SCREEN_VELOCITY      = Internal::DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT + 2;
const Property::Index PanGestureDetector::LOCAL_POSITION       = Internal::DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT + 3;
const Property::Index PanGestureDetector::LOCAL_DISPLACEMENT   = Internal::DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT + 4;
const Property::Index PanGestureDetector::LOCAL_VELOCITY       = Internal::DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT + 5;
const Property::Index PanGestureDetector::PANNING              = Internal::DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT + 6;

namespace Internal
{

namespace
{

// Properties

PropertyDetails DEFAULT_PROPERTIES[] =
{
  { "screen-position",     Property::VECTOR2, false, false, true },
  { "screen-displacement", Property::VECTOR2, false, false, true },
  { "screen-velocity",     Property::VECTOR2, false, false, true },
  { "local-position",      Property::VECTOR2, false, false, true },
  { "local-displacement",  Property::VECTOR2, false, false, true },
  { "local-velocity",      Property::VECTOR2, false, false, true },
  { "panning",             Property::BOOLEAN, false, false, true },
};
const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTIES ) / sizeof( DEFAULT_PROPERTIES[0] );

BaseHandle Create()
{
  return Dali::PanGestureDetector::New();
}

TypeRegistration mType( typeid(Dali::PanGestureDetector), typeid(Dali::GestureDetector), Create );

SignalConnectorType signalConnector1( mType, Dali::PanGestureDetector::SIGNAL_PAN_DETECTED, &PanGestureDetector::DoConnectSignal );

#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_PAN_GESTURE_DETECTOR");

/**
 * When debugging, helper for converting radians to degrees.
 */
inline float RadiansToDegrees( float radian )
{
  return radian * 180.0f / Math::PI;
}

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

PanGestureDetector::DefaultPropertyLookup* PanGestureDetector::mDefaultPropertyLookup = NULL;

PanGestureDetectorPtr PanGestureDetector::New()
{
  return new PanGestureDetector;
}

PanGestureDetector::PanGestureDetector()
: GestureDetector(Gesture::Pan),
  mMinimumTouches(1),
  mMaximumTouches(1),
  mSceneObject(NULL)
{
  if( !mDefaultPropertyLookup )
  {
    mDefaultPropertyLookup = new DefaultPropertyLookup();
    const int start = DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT;
    for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
    {
      ( *mDefaultPropertyLookup )[ DEFAULT_PROPERTIES[i].name ] = i + start;
    }
  }
}

PanGestureDetector::~PanGestureDetector()
{
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

unsigned int PanGestureDetector::GetMinimumTouchesRequired() const
{
  return mMinimumTouches;
}

unsigned int PanGestureDetector::GetMaximumTouchesRequired() const
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

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Angle Added: %.2f, Threshold: %.2f\n", RadiansToDegrees(angle), RadiansToDegrees(threshold) );

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

const PanGestureDetector::AngleContainer& PanGestureDetector::GetAngles() const
{
  return mAngleContainer;
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
                     RadiansToDegrees(angle), RadiansToDegrees(angleAllowed), RadiansToDegrees(threshold) );

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
  if ( !mDetectedSignalV2.Empty() )
  {
    // Guard against destruction during signal emission
    Dali::PanGestureDetector handle( this );

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Emitting Signal (%p)\n", this );

    mDetectedSignalV2.Emit( actor, pan );
  }
}

void PanGestureDetector::SetSceneObject( const SceneGraph::PanGesture* object )
{
  mSceneObject = object;
}

bool PanGestureDetector::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  PanGestureDetector* gesture = dynamic_cast<PanGestureDetector*>(object);

  if ( Dali::PanGestureDetector::SIGNAL_PAN_DETECTED  == signalName )
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

bool PanGestureDetector::IsSceneObjectRemovable() const
{
  return false;
}

unsigned int PanGestureDetector::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void PanGestureDetector::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  int index = DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT;
  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i, ++index )
  {
    indices.push_back( index );
  }
}

const std::string& PanGestureDetector::GetDefaultPropertyName( Property::Index index ) const
{
  index -= DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTIES[ index ].name;
  }
  else
  {
    // Index out-of-range... return empty string.
    static const std::string INVALID_PROPERTY_NAME;
    return INVALID_PROPERTY_NAME;
  }
}

Property::Index PanGestureDetector::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  DALI_ASSERT_DEBUG( NULL != mDefaultPropertyLookup );

  // Look for name in default properties
  DefaultPropertyLookup::const_iterator result = mDefaultPropertyLookup->find( name );
  if ( mDefaultPropertyLookup->end() != result )
  {
    index = result->second;
  }

  return index;
}

bool PanGestureDetector::IsDefaultPropertyWritable(Property::Index index) const
{
  // None of our properties should be writable through the Public API
  return false;
}

bool PanGestureDetector::IsDefaultPropertyAnimatable(Property::Index index) const
{
  // None of our properties are animatable
  return false;
}

bool PanGestureDetector::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  // All our properties can be used as an input to a constraint.
  return true;
}

Property::Type PanGestureDetector::GetDefaultPropertyType(Property::Index index) const
{
  index -= DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT;
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTIES[ index ].type;
  }
  else
  {
    // Index out-of-range
    return Property::NONE;
  }
}

void PanGestureDetector::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  // None of our properties should be settable from Public API
}

void PanGestureDetector::SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  // None of our properties should be settable from Public API
}

Property::Value PanGestureDetector::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  switch ( index )
  {
    case Dali::PanGestureDetector::SCREEN_POSITION:
    {
      if(mSceneObject)
      {
        value = mSceneObject->GetScreenPositionProperty().Get();
      }
      else
      {
        value = Vector2();
      }
      break;
    }

    case Dali::PanGestureDetector::SCREEN_DISPLACEMENT:
    {
      if(mSceneObject)
      {
        value = mSceneObject->GetScreenDisplacementProperty().Get();
      }
      else
      {
        value = Vector2();
      }
      break;
    }

    case Dali::PanGestureDetector::SCREEN_VELOCITY:
    {
      if(mSceneObject)
      {
        value = mSceneObject->GetScreenVelocityProperty().Get();
      }
      else
      {
        value = Vector2();
      }
      break;
    }

    case Dali::PanGestureDetector::LOCAL_POSITION:
    {
      if(mSceneObject)
      {
        value = mSceneObject->GetLocalPositionProperty().Get();
      }
      else
      {
        value = Vector2();
      }
      break;
    }

    case Dali::PanGestureDetector::LOCAL_DISPLACEMENT:
    {
      if(mSceneObject)
      {
        value = mSceneObject->GetLocalDisplacementProperty().Get();
      }
      else
      {
        value = Vector2();
      }
      break;
    }

    case Dali::PanGestureDetector::LOCAL_VELOCITY:
    {
      if(mSceneObject)
      {
        value = mSceneObject->GetLocalVelocityProperty().Get();
      }
      else
      {
        value = Vector2();
      }
      break;
    }

    case Dali::PanGestureDetector::PANNING:
    {
      if(mSceneObject)
      {
        value = mSceneObject->GetPanningProperty().Get();
      }
      else
      {
        value = false;
      }
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

void PanGestureDetector::InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index )
{
  // We do not want the user to install custom properties
  DALI_ASSERT_ALWAYS(false && "PanGestureDetector does not allow custom properties" );
}

const SceneGraph::PropertyOwner* PanGestureDetector::GetSceneObject() const
{
  // This method should only return an object connected to the scene-graph
  return mSceneObject;
}

const SceneGraph::PropertyBase* PanGestureDetector::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );

  // None of our properties are animatable
  return NULL;
}

const PropertyInputImpl* PanGestureDetector::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property( NULL );

  // This method should only return a property of an object connected to the scene-graph
  if ( !mSceneObject )
  {
    return property;
  }

  if ( index >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    CustomPropertyLookup::const_iterator entry = GetCustomPropertyLookup().find( index );

    DALI_ASSERT_ALWAYS( GetCustomPropertyLookup().end() != entry && "property index is invalid" );

    property = entry->second.GetSceneGraphProperty();
  }
  else
  {
    switch ( index )
    {
      case Dali::PanGestureDetector::SCREEN_POSITION:
      {
        property = &mSceneObject->GetScreenPositionProperty();
        break;
      }

      case Dali::PanGestureDetector::SCREEN_DISPLACEMENT:
      {
        property = &mSceneObject->GetScreenDisplacementProperty();
        break;
      }

      case Dali::PanGestureDetector::SCREEN_VELOCITY:
      {
        property = &mSceneObject->GetScreenVelocityProperty();
        break;
      }

      case Dali::PanGestureDetector::LOCAL_POSITION:
      {
        property = &mSceneObject->GetLocalPositionProperty();
        break;
      }

      case Dali::PanGestureDetector::LOCAL_DISPLACEMENT:
      {
        property = &mSceneObject->GetLocalDisplacementProperty();
        break;
      }

      case Dali::PanGestureDetector::LOCAL_VELOCITY:
      {
        property = &mSceneObject->GetLocalVelocityProperty();
        break;
      }

      case Dali::PanGestureDetector::PANNING:
      {
        property = &mSceneObject->GetPanningProperty();
        break;
      }

      default:
        break;
    }
  }

  return property;
}

} // namespace Internal

} // namespace Dali
