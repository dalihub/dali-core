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
#include <dali/internal/event/common/object-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/event/animation/active-constraint-base.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/event/common/type-registry-impl.h>

using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{
const int SUPPORTED_CAPABILITIES = Dali::Handle::DYNAMIC_PROPERTIES;  // Object provides this capability
typedef Dali::Vector<Object::Observer*>::Iterator ObserverIter;
typedef Dali::Vector<Object::Observer*>::ConstIterator ConstObserverIter;

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_OBJECT" );
#endif
} // unnamed namespace

Object::Object()
: mEventThreadServices( *Stage::GetCurrent() ),
  mTypeInfo( NULL ),
  mConstraints( NULL ),
  mPropertyNotifications( NULL )
{
}

void Object::AddObserver(Observer& observer)
{
  // make sure an observer doesn't observe the same object twice
  // otherwise it will get multiple calls to OnSceneObjectAdd(), OnSceneObjectRemove() and ObjectDestroyed()
  DALI_ASSERT_DEBUG( mObservers.End() == std::find( mObservers.Begin(), mObservers.End(), &observer));

  mObservers.PushBack( &observer );
}

void Object::RemoveObserver(Observer& observer)
{
  // Find the observer...
  const ConstObserverIter endIter =  mObservers.End();
  for( ObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    if( (*iter) == &observer)
    {
      mObservers.Erase( iter );
      break;
    }
  }
  DALI_ASSERT_DEBUG(endIter != mObservers.End());
}

void Object::OnSceneObjectAdd()
{
  // Notification for this object's constraints
  if( mConstraints )
  {
    const ActiveConstraintConstIter endIter = mConstraints->end();
    for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( *iter );
      baseConstraint.OnParentSceneObjectAdded();
    }
  }

  // Notification for observers
  for( ConstObserverIter iter = mObservers.Begin(),  endIter =  mObservers.End(); iter != endIter; ++iter)
  {
    (*iter)->SceneObjectAdded(*this);
  }

  // enable property notifications in scene graph
  EnablePropertyNotifications();
}

void Object::OnSceneObjectRemove()
{
  // Notification for this object's constraints
  if( mConstraints )
  {
    const ActiveConstraintConstIter endIter = mConstraints->end();
    for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( *iter );
      baseConstraint.OnParentSceneObjectRemoved();
    }
  }

  // Notification for observers
  for( ConstObserverIter iter = mObservers.Begin(), endIter = mObservers.End(); iter != endIter; ++iter )
  {
    (*iter)->SceneObjectRemoved(*this);
  }

  // disable property notifications in scene graph
  DisablePropertyNotifications();
}

int Object::GetPropertyComponentIndex( Property::Index index ) const
{
  return Property::INVALID_COMPONENT_INDEX;
}

bool Object::Supports( Capability capability ) const
{
  return (capability & SUPPORTED_CAPABILITIES);
}

unsigned int Object::GetPropertyCount() const
{
  unsigned int count = GetDefaultPropertyCount();

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Default Properties: %d\n", count );

  const TypeInfo* typeInfo( GetTypeInfo() );
  if ( typeInfo )
  {
    unsigned int manual( typeInfo->GetPropertyCount() );
    count += manual;

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Manual Properties:  %d\n", manual );
  }

  unsigned int custom( mCustomProperties.Count() );
  count += custom;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Custom Properties:  %d\n", custom );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Total Properties:   %d\n", count );

  return count;
}

std::string Object::GetPropertyName( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( index > Property::INVALID_INDEX && "Property index out of bounds" );

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    return GetDefaultPropertyName( index );
  }

  if ( ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
    || ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      return typeInfo->GetPropertyName( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Property index is invalid" );
    }
  }

  CustomPropertyMetadata* custom = FindCustomProperty( index );
  if( custom )
  {
    return custom->name;
  }
  return "";
}

Property::Index Object::GetPropertyIndex(const std::string& name) const
{
  Property::Index index = GetDefaultPropertyIndex( name );

  if(index == Property::INVALID_INDEX)
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      index = typeInfo->GetPropertyIndex( name );
      if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
      {
        // check whether the animatable property is registered already, if not then register one.
        AnimatablePropertyMetadata* animatableProperty = FindAnimatableProperty( index );
        if(!animatableProperty)
        {
          const TypeInfo* typeInfo( GetTypeInfo() );
          if (typeInfo)
          {
            index = RegisterSceneGraphProperty(typeInfo->GetPropertyName(index), index, Property::Value(typeInfo->GetPropertyType(index)));
          }
        }
      }
    }
  }

  if( (index == Property::INVALID_INDEX)&&( mCustomProperties.Count() > 0 ) )
  {
    Property::Index count = PROPERTY_CUSTOM_START_INDEX;
    const PropertyMetadataLookup::ConstIterator end = mCustomProperties.End();
    for( PropertyMetadataLookup::ConstIterator iter = mCustomProperties.Begin(); iter != end; ++iter, ++count )
    {
      CustomPropertyMetadata* custom = static_cast<CustomPropertyMetadata*>(*iter);
      if ( custom->name == name )
      {
        index = count;
        break;
      }
    }
  }

  return index;
}

bool Object::IsPropertyWritable( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds");

  bool writable = false;

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    writable = IsDefaultPropertyWritable( index );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      writable = typeInfo->IsPropertyWritable( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Invalid property index" );
    }
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry scene-graph properties are writable.
    writable = true;
  }
  else
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if( custom )
    {
      writable = custom->IsWritable();
    }
  }

  return writable;
}

bool Object::IsPropertyAnimatable( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds");

  bool animatable = false;

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    animatable = IsDefaultPropertyAnimatable( index );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry event-thread only properties are not animatable.
    animatable = false;
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry scene-graph properties are animatable.
    animatable = true;
  }
  else
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if( custom )
    {
      animatable = custom->IsAnimatable();
    }
  }

  return animatable;
}

bool Object::IsPropertyAConstraintInput( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds");

  bool isConstraintInput = false;

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    isConstraintInput = IsDefaultPropertyAConstraintInput( index );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry event-thread only properties cannot be used as an input to a constraint.
    isConstraintInput = false;
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // scene graph properties can be used as input to a constraint.
    isConstraintInput = true;
  }
  else
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if( custom )
    {
      // ... custom properties can be used as input to a constraint.
      isConstraintInput = true;
    }
  }

  return isConstraintInput;
}

Property::Type Object::GetPropertyType( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds" );

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    return GetDefaultPropertyType( index );
  }

  if ( ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
    || ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      return typeInfo->GetPropertyType( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" );
    }
  }

  CustomPropertyMetadata* custom = FindCustomProperty( index );
  if( custom )
  {
    return custom->type;
  }
  return Property::NONE;
}

void Object::SetProperty( Property::Index index, const Property::Value& propertyValue )
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds" );

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    SetDefaultProperty( index, propertyValue );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      typeInfo->SetProperty( this, index, propertyValue );
    }
    else
    {
      DALI_LOG_ERROR("Cannot find property index\n");
    }
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
    AnimatablePropertyMetadata* animatableProperty = FindAnimatableProperty( index );
    if(!animatableProperty)
    {
      const TypeInfo* typeInfo( GetTypeInfo() );
      if (!typeInfo)
      {
        DALI_LOG_ERROR("Cannot find property index\n");
      }
      else if ( Property::INVALID_INDEX == RegisterSceneGraphProperty( typeInfo->GetPropertyName( index ), index, propertyValue ) )
      {
        DALI_LOG_ERROR("Cannot register property\n");
      }
    }
    else
    {
      // set the scene graph property value
      SetSceneGraphProperty( index, *animatableProperty, propertyValue );
    }
  }
  else
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if( custom )
    {
      if( custom->IsAnimatable() )
      {
        // set the scene graph property value
        SetSceneGraphProperty( index, *custom, propertyValue );
      }
      else if( custom->IsWritable() )
      {
        custom->value = propertyValue;
        OnPropertySet(index, propertyValue);
      }
      // trying to set value on read only property is no-op
    }
    else
    {
      DALI_LOG_ERROR("Invalid property index\n");
    }
  }
}

Property::Value Object::GetProperty(Property::Index index) const
{
  DALI_ASSERT_ALWAYS( index > Property::INVALID_INDEX && "Property index is out of bounds" );

  Property::Value value;

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    value = GetDefaultProperty( index );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      value = typeInfo->GetProperty( this, index );
    }
    else
    {
      DALI_LOG_ERROR("Cannot find property index\n");
    }
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
    AnimatablePropertyMetadata* animatableProperty = FindAnimatableProperty( index );
    if(!animatableProperty)
    {
      const TypeInfo* typeInfo( GetTypeInfo() );
      if (typeInfo)
      {
        if(Property::INVALID_INDEX != RegisterSceneGraphProperty(typeInfo->GetPropertyName(index), index, Property::Value(typeInfo->GetPropertyType(index))))
        {
          value = Property::Value(typeInfo->GetPropertyType(index)); // Return an initialized property value according to the type
        }
        else
        {
          DALI_LOG_ERROR("Cannot register property\n");
        }
      }
      else
      {
        DALI_LOG_ERROR("Cannot find property index\n");
      }
    }
    else
    {
      // get the animatable property value
      value = GetPropertyValue( animatableProperty );
    }
  }
  else if(mCustomProperties.Count() > 0)
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if(custom)
    {
      // get the custom property value
      value = GetPropertyValue( custom );
    }
    else
    {
      DALI_LOG_ERROR("Invalid property index\n");
    }
  } // if custom

  return value;
}

void Object::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.clear();

  // Default Properties
  GetDefaultPropertyIndices( indices );

  // Manual Properties
  const TypeInfo* typeInfo( GetTypeInfo() );
  if ( typeInfo )
  {
    typeInfo->GetPropertyIndices( indices );
  }

  // Custom Properties
  if ( mCustomProperties.Count() > 0 )
  {
    indices.reserve( indices.size() + mCustomProperties.Count() );

    PropertyMetadataLookup::ConstIterator iter = mCustomProperties.Begin();
    const PropertyMetadataLookup::ConstIterator endIter = mCustomProperties.End();
    int i=0;
    for ( ; iter != endIter; ++iter, ++i )
    {
      indices.push_back( PROPERTY_CUSTOM_START_INDEX + i );
    }
  }
}

Property::Index Object::RegisterSceneGraphProperty(const std::string& name, Property::Index index, const Property::Value& propertyValue) const
{
  // Create a new property
  Dali::Internal::OwnerPointer<PropertyBase> newProperty;

  switch ( propertyValue.GetType() )
  {
    case Property::BOOLEAN:
    {
      newProperty = new AnimatableProperty<bool>( propertyValue.Get<bool>() );
      break;
    }

    case Property::FLOAT:
    {
      newProperty = new AnimatableProperty<float>( propertyValue.Get<float>() );
      break;
    }

    case Property::INTEGER:
    {
      newProperty = new AnimatableProperty<int>( propertyValue.Get<int>() );
      break;
    }

    case Property::VECTOR2:
    {
      newProperty = new AnimatableProperty<Vector2>( propertyValue.Get<Vector2>() );
      break;
    }

    case Property::VECTOR3:
    {
      newProperty = new AnimatableProperty<Vector3>( propertyValue.Get<Vector3>() );
      break;
    }

    case Property::VECTOR4:
    {
      newProperty = new AnimatableProperty<Vector4>( propertyValue.Get<Vector4>() );
      break;
    }

    case Property::MATRIX:
    {
      newProperty = new AnimatableProperty<Matrix>( propertyValue.Get<Matrix>() );
      break;
    }

    case Property::MATRIX3:
    {
      newProperty = new AnimatableProperty<Matrix3>( propertyValue.Get<Matrix3>() );
      break;
    }

    case Property::ROTATION:
    {
      newProperty = new AnimatableProperty<Quaternion>( propertyValue.Get<Quaternion>() );
      break;
    }

    case Property::UNSIGNED_INTEGER:
    case Property::RECTANGLE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    {
      DALI_LOG_WARNING( "Property Type %d\n", propertyValue.GetType() );
      DALI_ASSERT_ALWAYS( !"PropertyType is not animatable" );
      break;
    }

    default:
    {
      DALI_LOG_WARNING( "Property Type %d\n", propertyValue.GetType() );
      DALI_ASSERT_ALWAYS( !"PropertyType enumeration is out of bounds" );
      break;
    }
  }

  // get the scene property owner from derived class
  const SceneGraph::PropertyOwner* scenePropertyOwner = GetPropertyOwner();
  // we can only pass properties to scene graph side if there is a scene object
  if( scenePropertyOwner )
  {
    // keep a local pointer to the property as the OwnerPointer will pass its copy to the message
    const PropertyBase* property = newProperty.Get();
    if(index >= PROPERTY_CUSTOM_START_INDEX)
    {
      mCustomProperties.PushBack( new CustomPropertyMetadata( name, propertyValue.GetType(), property ) );
    }
    else
    {
      mAnimatableProperties.PushBack( new AnimatablePropertyMetadata( index, propertyValue.GetType(), property ) );
    }

    // queue a message to add the property
    InstallCustomPropertyMessage( const_cast<EventThreadServices&>(GetEventThreadServices()), *scenePropertyOwner, newProperty.Release() ); // Message takes ownership

    // notify the derived class (optional) method in case it needs to do some more work on the new property
    // note! have to use the local pointer as OwnerPointer now points to NULL as it handed over its ownership
    NotifyScenePropertyInstalled( *property, name, index );

    return index;
  }
  else
  {
    // property was orphaned and killed so return invalid index
    return Property::INVALID_INDEX;
  }
}

Property::Index Object::RegisterProperty( const std::string& name, const Property::Value& propertyValue)
{
  return RegisterSceneGraphProperty(name, PROPERTY_CUSTOM_START_INDEX + mCustomProperties.Count(), propertyValue);
}

Property::Index Object::RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode)
{
  Property::Index index = Property::INVALID_INDEX;

  if(Property::ANIMATABLE == accessMode)
  {
    index = RegisterProperty(name, propertyValue);
  }
  else
  {
    // Add entry to the property lookup
    index = PROPERTY_CUSTOM_START_INDEX + mCustomProperties.Count();
    mCustomProperties.PushBack( new CustomPropertyMetadata( name, propertyValue, accessMode ) );
  }

  return index;
}

Dali::PropertyNotification Object::AddPropertyNotification(Property::Index index,
                                                                int componentIndex,
                                                                const Dali::PropertyCondition& condition)
{
  if ( index >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    if ( index <= PROPERTY_REGISTRATION_MAX_INDEX )
    {
      DALI_ASSERT_ALWAYS( false && "Property notification added to event side only property." );
    }
    else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
    {
      // check whether the animatable property is registered already, if not then register one.
      AnimatablePropertyMetadata* animatable = FindAnimatableProperty( index );
      if( !animatable )
      {
        const TypeInfo* typeInfo( GetTypeInfo() );
        if ( typeInfo )
        {
          if( Property::INVALID_INDEX != RegisterSceneGraphProperty( typeInfo->GetPropertyName( index ), index, Property::Value( typeInfo->GetPropertyType( index ) ) ) )
          {
            animatable = FindAnimatableProperty( index );
          }
        }
      }
      DALI_ASSERT_ALWAYS( animatable && "Property index is invalid" );
    }
    else if ( mCustomProperties.Count() > 0 )
    {
      CustomPropertyMetadata* custom = FindCustomProperty( index );
      DALI_ASSERT_ALWAYS( custom && "Invalid property index" );
      DALI_ASSERT_ALWAYS( custom->IsAnimatable() && "Property notification added to event side only property." );
    }
  }

  Dali::Handle self(this);
  Property target( self, index );

  PropertyNotificationPtr internal = PropertyNotification::New( target, componentIndex, condition );
  Dali::PropertyNotification propertyNotification(internal.Get());

  if( !mPropertyNotifications )
  {
    mPropertyNotifications = new PropertyNotificationContainer;
  }
  mPropertyNotifications->push_back(propertyNotification);

  return propertyNotification;
}

void Object::RemovePropertyNotification(Dali::PropertyNotification propertyNotification)
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    while(iter != mPropertyNotifications->end() )
    {
      if(*iter == propertyNotification)
      {
        mPropertyNotifications->erase(iter);
        // As we can't ensure all references are removed, we can just disable
        // the notification.
        GetImplementation(propertyNotification).Disable();
        return;
      }
      ++iter;
    }
  }
}

void Object::RemovePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    while(iter != mPropertyNotifications->end() )
    {
      // As we can't ensure all references are removed, we can just disable
      // the notification.
      GetImplementation(*iter).Disable();
      ++iter;
    }

    mPropertyNotifications->clear();
  }
}

void Object::EnablePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    PropertyNotificationContainerIter endIter = mPropertyNotifications->end();

    for( ; iter != endIter; ++iter )
    {
      GetImplementation(*iter).Enable();
    }
  }
}

void Object::DisablePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    PropertyNotificationContainerIter iter = mPropertyNotifications->begin();
    PropertyNotificationContainerIter endIter = mPropertyNotifications->end();

    for( ; iter != endIter; ++iter )
    {
      GetImplementation(*iter).Disable();
    }
  }
}

Dali::ActiveConstraint Object::ApplyConstraint( Constraint& constraint )
{
  return Dali::ActiveConstraint( DoApplyConstraint( constraint, Dali::Handle() ) );
}

Dali::ActiveConstraint Object::ApplyConstraint( Constraint& constraint, Dali::Handle weightObject )
{
  return Dali::ActiveConstraint( DoApplyConstraint( constraint, weightObject ) );
}

ActiveConstraintBase* Object::DoApplyConstraint( Constraint& constraint, Dali::Handle weightObject )
{
  ActiveConstraintBase* activeConstraintImpl = constraint.CreateActiveConstraint();
  DALI_ASSERT_DEBUG( NULL != activeConstraintImpl );

  Dali::ActiveConstraint activeConstraint( activeConstraintImpl );

  if( weightObject )
  {
    Object& weightObjectImpl = GetImplementation( weightObject );
    Property::Index weightIndex = weightObjectImpl.GetPropertyIndex( "weight" );

    if( Property::INVALID_INDEX != weightIndex )
    {
      activeConstraintImpl->SetCustomWeightObject( weightObjectImpl, weightIndex );
    }
  }

  if( !mConstraints )
  {
    mConstraints = new ActiveConstraintContainer;
  }
  mConstraints->push_back( activeConstraint );

  activeConstraintImpl->FirstApply( *this, constraint.GetApplyTime() );

  return activeConstraintImpl;
}

Property::Value Object::GetPropertyValue( const PropertyMetadata* entry ) const
{
  Property::Value value;

  DALI_ASSERT_ALWAYS( entry && "Invalid property metadata" );

  if( !entry->IsAnimatable() )
  {
    value = entry->value;
  }
  else
  {
    BufferIndex bufferIndex( GetEventThreadServices().GetEventBufferIndex() );

    switch ( entry->type )
    {
      case Property::BOOLEAN:
      {
        const AnimatableProperty<bool>* property = dynamic_cast< const AnimatableProperty<bool>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::FLOAT:
      {
        const AnimatableProperty<float>* property = dynamic_cast< const AnimatableProperty<float>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::INTEGER:
      {
        const AnimatableProperty<int>* property = dynamic_cast< const AnimatableProperty<int>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::VECTOR2:
      {
        const AnimatableProperty<Vector2>* property = dynamic_cast< const AnimatableProperty<Vector2>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::VECTOR3:
      {
        const AnimatableProperty<Vector3>* property = dynamic_cast< const AnimatableProperty<Vector3>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::VECTOR4:
      {
        const AnimatableProperty<Vector4>* property = dynamic_cast< const AnimatableProperty<Vector4>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::MATRIX:
      {
        const AnimatableProperty<Matrix>* property = dynamic_cast< const AnimatableProperty<Matrix>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::MATRIX3:
      {
        const AnimatableProperty<Matrix3>* property = dynamic_cast< const AnimatableProperty<Matrix3>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::ROTATION:
      {
        const AnimatableProperty<Quaternion>* property = dynamic_cast< const AnimatableProperty<Quaternion>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      default:
      {
        DALI_ASSERT_ALWAYS( false && "PropertyType enumeration is out of bounds" );
        break;
      }
    } // switch(type)
  } // if animatable

  return value;
}

void Object::SetSceneGraphProperty( Property::Index index, const PropertyMetadata& entry, const Property::Value& value )
{
  switch ( entry.type )
  {
    case Property::BOOLEAN:
    {
      const AnimatableProperty<bool>* property = dynamic_cast< const AnimatableProperty<bool>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<bool>( GetEventThreadServices(), *property, value.Get<bool>() );
      break;
    }

    case Property::FLOAT:
    {
      const AnimatableProperty<float>* property = dynamic_cast< const AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<float>( GetEventThreadServices(), *property, value.Get<float>() );
      break;
    }

    case Property::INTEGER:
    {
      const AnimatableProperty<int>* property = dynamic_cast< const AnimatableProperty<int>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<int>( GetEventThreadServices(), *property, value.Get<int>() );
      break;
    }

    case Property::VECTOR2:
    {
      const AnimatableProperty<Vector2>* property = dynamic_cast< const AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Vector2>( GetEventThreadServices(), *property, value.Get<Vector2>() );
      break;
    }

    case Property::VECTOR3:
    {
      const AnimatableProperty<Vector3>* property = dynamic_cast< const AnimatableProperty<Vector3>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Vector3>( GetEventThreadServices(), *property, value.Get<Vector3>() );
      break;
    }

    case Property::VECTOR4:
    {
      const AnimatableProperty<Vector4>* property = dynamic_cast< const AnimatableProperty<Vector4>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Vector4>( GetEventThreadServices(), *property, value.Get<Vector4>() );
      break;
    }

    case Property::ROTATION:
    {
      const AnimatableProperty<Quaternion>* property = dynamic_cast< const AnimatableProperty<Quaternion>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Quaternion>( GetEventThreadServices(), *property, value.Get<Quaternion>() );
      break;
    }

    case Property::MATRIX:
    {
      const AnimatableProperty<Matrix>* property = dynamic_cast< const AnimatableProperty<Matrix>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Matrix>( GetEventThreadServices(), *property, value.Get<Matrix>() );
      break;
    }

    case Property::MATRIX3:
    {
      const AnimatableProperty<Matrix3>* property = dynamic_cast< const AnimatableProperty<Matrix3>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Matrix3>( GetEventThreadServices(), *property, value.Get<Matrix3>() );
      break;
    }

    default:
    {
      // non-animatable scene graph property, do nothing
    }
  }
}

const TypeInfo* Object::GetTypeInfo() const
{
  if ( !mTypeInfo )
  {
    // This uses a dynamic_cast so can be quite expensive so we only really want to do it once
    // especially as the type-info does not change during the life-time of an application

    Dali::TypeInfo typeInfoHandle = TypeRegistry::Get()->GetTypeInfo( this );
    if ( typeInfoHandle )
    {
      mTypeInfo = &GetImplementation( typeInfoHandle );
    }
  }

  return mTypeInfo;
}

void Object::RemoveConstraint( ActiveConstraint& constraint, bool isInScenegraph )
{
  // guard against constraint sending messages during core destruction
  if ( Stage::IsInstalled() )
  {
    if( isInScenegraph )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( constraint );
      baseConstraint.BeginRemove();
    }
  }
}

void Object::RemoveConstraint( Dali::ActiveConstraint activeConstraint )
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    bool isInSceneGraph( NULL != GetSceneObject() );

    ActiveConstraintIter it( std::find( mConstraints->begin(), mConstraints->end(), activeConstraint ) );
    if( it !=  mConstraints->end() )
    {
      RemoveConstraint( *it, isInSceneGraph );
      mConstraints->erase( it );
    }
  }
}

void Object::RemoveConstraints( unsigned int tag )
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    bool isInSceneGraph( NULL != GetSceneObject() );

    ActiveConstraintIter iter( mConstraints->begin() );
    while(iter != mConstraints->end() )
    {
      ActiveConstraintBase& constraint = GetImplementation( *iter );
      if( constraint.GetTag() == tag )
      {
        RemoveConstraint( *iter, isInSceneGraph );
        iter = mConstraints->erase( iter );
      }
      else
      {
        ++iter;
      }
    }
  }
}

void Object::RemoveConstraints()
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    // If we have nothing in the scene-graph, just clear constraint containers
    const SceneGraph::PropertyOwner* propertyOwner = GetSceneObject();
    if ( NULL != propertyOwner )
    {
      const ActiveConstraintConstIter endIter = mConstraints->end();
      for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
      {
        RemoveConstraint( *iter, true );
      }
    }

    delete mConstraints;
    mConstraints = NULL;
  }
}

void Object::SetTypeInfo( const TypeInfo* typeInfo )
{
  mTypeInfo = typeInfo;
}

Object::~Object()
{
  // Notification for this object's constraints
  // (note that the ActiveConstraint handles may outlive the Object)
  if( mConstraints )
  {
    const ActiveConstraintConstIter endIter = mConstraints->end();
    for ( ActiveConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
    {
      ActiveConstraintBase& baseConstraint = GetImplementation( *iter );
      baseConstraint.OnParentDestroyed();
    }
  }

  // Notification for observers
  for( ConstObserverIter iter = mObservers.Begin(), endIter =  mObservers.End(); iter != endIter; ++iter)
  {
    (*iter)->ObjectDestroyed(*this);
  }

  delete mConstraints;
  delete mPropertyNotifications;
}

CustomPropertyMetadata* Object::FindCustomProperty( Property::Index index ) const
{
  CustomPropertyMetadata* property( NULL );
  int arrayIndex = index - PROPERTY_CUSTOM_START_INDEX;
  if( arrayIndex >= 0 )
  {
    if( arrayIndex < (int)mCustomProperties.Count() ) // we can only access the first 2 billion custom properties
    {
      property = static_cast<CustomPropertyMetadata*>(mCustomProperties[ arrayIndex ]);
    }
  }
  return property;
}

AnimatablePropertyMetadata* Object::FindAnimatableProperty( Property::Index index ) const
{
  for ( int arrayIndex = 0; arrayIndex < (int)mAnimatableProperties.Count(); arrayIndex++ )
  {
    AnimatablePropertyMetadata* property = static_cast<AnimatablePropertyMetadata*>( mAnimatableProperties[ arrayIndex ] );
    if( property->index == index )
    {
      return property;
    }
  }
  return NULL;
}

} // namespace Internal

} // namespace Dali
