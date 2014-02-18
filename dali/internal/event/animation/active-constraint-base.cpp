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
#include <dali/internal/event/animation/active-constraint-base.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/property-owner-messages.h>

using Dali::Internal::SceneGraph::AnimatableProperty;

namespace Dali
{

const Property::Index ActiveConstraint::WEIGHT = 0;

namespace Internal
{

namespace // unnamed namespace
{

BaseHandle Create()
{
  // not directly creatable
  return BaseHandle();
}

TypeRegistration mType( typeid(Dali::ActiveConstraint), typeid(Dali::Handle), Create );

SignalConnectorType signalConnector1( mType, Dali::ActiveConstraint::SIGNAL_APPLIED, &ActiveConstraintBase::DoConnectSignal );

}


namespace // unnamed namespace
{

/**
 * We want to discourage the use of property strings (minimize string comparisons),
 * particularly for the default properties.
 */
const std::string DEFAULT_PROPERTY_NAMES[] =
{
  "weight"
};
const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_NAMES ) / sizeof( std::string );

const Property::Type DEFAULT_PROPERTY_TYPES[DEFAULT_PROPERTY_COUNT] =
{
  Property::FLOAT // WEIGHT
};

} // unnamed namespace

ActiveConstraintBase::ActiveConstraintBase( EventToUpdate& eventToUpdate, Property::Index targetPropertyIndex )
: mEventToUpdate( eventToUpdate ),
  mTargetPropertyIndex( targetPropertyIndex ),
  mTargetProxy( NULL ),
  mSceneGraphConstraint( NULL ),
  mOffstageWeight( Dali::ActiveConstraint::DEFAULT_WEIGHT ),
  mRemoveTime( 0.0f ),
  mAlphaFunction( Dali::Constraint::DEFAULT_ALPHA_FUNCTION ),
  mRemoveAction( Dali::Constraint::DEFAULT_REMOVE_ACTION ),
  mApplyAnimation(),
  mRemoveAnimation()
{
}

ActiveConstraintBase::~ActiveConstraintBase()
{
  // Disconnect from internal animation signals

  if ( mApplyAnimation )
  {
    GetImplementation(mApplyAnimation).SetFinishedCallback( NULL, NULL );
  }

  if( mRemoveAnimation )
  {
    GetImplementation(mRemoveAnimation).SetFinishedCallback( NULL, NULL );
  }
}

void ActiveConstraintBase::FirstApply( ProxyObject& parent, TimePeriod applyTime, ActiveConstraintCallbackType* callback )
{
  // Notify derived classes
  OnFirstApply( parent );

  if ( applyTime.durationSeconds > 0.0f )
  {
    DALI_ASSERT_DEBUG( !mApplyAnimation );

    // Set start weight
    SetWeight( 0.0f );

    // Automatically animate (increase) the weight, until the constraint is fully applied
    mApplyAnimation = Dali::Animation::New( applyTime.delaySeconds + applyTime.durationSeconds );
    Dali::ActiveConstraint self( this );
    mApplyAnimation.AnimateTo( Property( self, Dali::ActiveConstraint::WEIGHT ), Dali::ActiveConstraint::FINAL_WEIGHT, mAlphaFunction, applyTime );
    mApplyAnimation.Play();

    // Chain "Finish" to "Applied" signal
    GetImplementation(mApplyAnimation).SetFinishedCallback( &ActiveConstraintBase::FirstApplyFinished, this );
  }
}

void ActiveConstraintBase::BeginRemove()
{
  // Notify derived classes
  OnBeginRemove();

  // Remove gradually by animating weight down to zero
  if ( mRemoveTime.durationSeconds > 0.0f )
  {
    // Stop baking behaviour from interfering with remove animation
    if ( mSceneGraphConstraint )
    {
      // Immediately remove from scene-graph
      SetRemoveActionMessage( mEventToUpdate, *mSceneGraphConstraint, Dali::Constraint::Discard );
    }

    // Interrupt ongoing apply-animations
    if ( mApplyAnimation )
    {
      mApplyAnimation.Stop();
    }

    // Reduce the weight to zero
    mRemoveAnimation = Dali::Animation::New( mRemoveTime.delaySeconds + mRemoveTime.durationSeconds );
    Dali::ActiveConstraint self( this );
    mRemoveAnimation.AnimateTo( Property( self, Dali::ActiveConstraint::WEIGHT ), 0.0f, mAlphaFunction, mRemoveTime );
    mRemoveAnimation.Play();

    // Finish removal when animation ends
    GetImplementation(mRemoveAnimation).SetFinishedCallback( &ActiveConstraintBase::OnRemoveFinished, this );
  }
  else
  {
    OnRemoveFinished( this );
  }
}

bool ActiveConstraintBase::IsRemoving()
{
  return mRemoveAnimation;
}

ProxyObject* ActiveConstraintBase::GetParent()
{
  return mTargetProxy;
}

bool ActiveConstraintBase::Supports( Capability capability ) const
{
  return false; // switch-off support for dynamic properties
}

Dali::Handle ActiveConstraintBase::GetTargetObject()
{
  return Dali::Handle( mTargetProxy );
}

Property::Index ActiveConstraintBase::GetTargetProperty()
{
  return mTargetPropertyIndex;
}

void ActiveConstraintBase::SetWeight( float weight )
{
  if ( mSceneGraphConstraint )
  {
    BakeWeightMessage( mEventToUpdate, *mSceneGraphConstraint, weight );
  }
  else
  {
    mOffstageWeight = weight;
  }
}

float ActiveConstraintBase::GetCurrentWeight() const
{
  float currentWeight( mOffstageWeight );

  if ( mSceneGraphConstraint )
  {
    currentWeight = mSceneGraphConstraint->GetWeight( mEventToUpdate.GetEventBufferIndex() );
  }

  return currentWeight;
}

ActiveConstraintSignalV2& ActiveConstraintBase::AppliedSignal()
{
  return mAppliedSignal;
}

bool ActiveConstraintBase::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  ActiveConstraintBase* constraint = dynamic_cast<ActiveConstraintBase*>(object);

  if ( Dali::ActiveConstraint::SIGNAL_APPLIED == signalName )
  {
    constraint->AppliedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void ActiveConstraintBase::SetRemoveTime( TimePeriod removeTime )
{
  mRemoveTime = removeTime;
}

TimePeriod ActiveConstraintBase::GetRemoveTime() const
{
  return mRemoveTime;
}

void ActiveConstraintBase::SetAlphaFunction( AlphaFunction alphaFunc )
{
  mAlphaFunction = alphaFunc;
}

AlphaFunction ActiveConstraintBase::GetAlphaFunction() const
{
  return mAlphaFunction;
}

void ActiveConstraintBase::SetRemoveAction( ActiveConstraintBase::RemoveAction action )
{
  mRemoveAction = action;
}

ActiveConstraintBase::RemoveAction ActiveConstraintBase::GetRemoveAction() const
{
  return mRemoveAction;
}

bool ActiveConstraintBase::IsSceneObjectRemovable() const
{
  return true; // The constraint removed when target SceneGraph::PropertyOwner is destroyed
}

unsigned int ActiveConstraintBase::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void ActiveConstraintBase::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const std::string& ActiveConstraintBase::GetDefaultPropertyName( Property::Index index ) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_NAMES[index];
  }
  else
  {
    // index out of range..return empty string
    static const std::string INVALID_PROPERTY_NAME;
    return INVALID_PROPERTY_NAME;
  }
}

Property::Index ActiveConstraintBase::GetDefaultPropertyIndex( const std::string& name ) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Only one name to compare with...
  if ( name == DEFAULT_PROPERTY_NAMES[0] )
  {
    index = 0;
  }

  return index;
}

bool ActiveConstraintBase::IsDefaultPropertyWritable( Property::Index index ) const
{
  return true; // All default properties are currently writable
}

bool ActiveConstraintBase::IsDefaultPropertyAnimatable( Property::Index index ) const
{
  return true; // All default properties are currently animatable
}

Property::Type ActiveConstraintBase::GetDefaultPropertyType( Property::Index index ) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_TYPES[index];
  }
  else
  {
    // Index out-of-range
    return Property::NONE;
  }
}

void ActiveConstraintBase::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  switch ( index )
  {
    case Dali::ActiveConstraint::WEIGHT:
    {
      SetWeight( propertyValue.Get<float>() );
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "ActiveConstraint property out of bounds" ); // should not come here
      break;
    }
  }
}

void ActiveConstraintBase::SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value )
{
  DALI_ASSERT_ALWAYS( false && "ActiveConstraintBase does not have custom properties"); // should not come here
}

Property::Value ActiveConstraintBase::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;

  switch ( index )
  {
    case Dali::ActiveConstraint::WEIGHT:
    {
      value = GetCurrentWeight();
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS( false && "ActiveConstraint property out of bounds" ); // should not come here
      break;
    }
  }

  return value;
}

void ActiveConstraintBase::InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index )
{
  DALI_ASSERT_ALWAYS( false && "ActiveConstraintBase does not have custom properties" ); // should not come here
}

const SceneGraph::PropertyOwner* ActiveConstraintBase::GetSceneObject() const
{
  return mSceneGraphConstraint;
}

const SceneGraph::PropertyBase* ActiveConstraintBase::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_DEBUG( 0 == index ); // only 1 property supported

  // This method should only return a property which is part of the scene-graph
  if ( !mSceneGraphConstraint )
  {
    return NULL;
  }

  return &mSceneGraphConstraint->mWeight;
}

const PropertyInputImpl* ActiveConstraintBase::GetSceneObjectInputProperty( Property::Index index ) const
{
  DALI_ASSERT_DEBUG( 0 == index ); // only 1 property supported

  // This method should only return a property which is part of the scene-graph
  if ( !mSceneGraphConstraint )
  {
    return NULL;
  }

  return &mSceneGraphConstraint->mWeight;
}

void ActiveConstraintBase::FirstApplyFinished( Object* object )
{
  ActiveConstraintBase& self = dynamic_cast<ActiveConstraintBase&>( *object );

  // This is necessary when the constraint was not added to scene-graph during the animation
  self.SetWeight( Dali::ActiveConstraint::FINAL_WEIGHT );

  // The animation is no longer needed
  GetImplementation(self.mApplyAnimation).SetFinishedCallback( NULL, NULL );
  self.mApplyAnimation.Reset();

  // Chain "Finish" to "Applied" signal

  if ( !self.mAppliedSignal.Empty() )
  {
    Dali::ActiveConstraint handle( &self );

    self.mAppliedSignal.Emit( handle );
  }

  // WARNING - this constraint may now have been deleted; don't do anything else here
}

void ActiveConstraintBase::OnRemoveFinished( Object* object )
{
  ActiveConstraintBase& self = dynamic_cast<ActiveConstraintBase&>( *object );

  const SceneGraph::PropertyOwner* propertyOwner = self.mTargetProxy ? self.mTargetProxy->GetSceneObject() : NULL;

  if ( propertyOwner &&
       self.mSceneGraphConstraint )
  {
    // Notify base class that the scene-graph constraint is being removed
    self.OnSceneObjectRemove();

    // Remove from scene-graph
    RemoveConstraintMessage( self.mEventToUpdate, *propertyOwner, *(self.mSceneGraphConstraint) );

    // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
    self.mSceneGraphConstraint = NULL;
  }

  // The animation is no longer needed
  self.mRemoveAnimation.Reset();
}

} // namespace Internal

} // namespace Dali
