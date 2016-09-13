/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/common/uniform-map.h>
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
  int componentIndex = Property::INVALID_COMPONENT_INDEX;

  const TypeInfo* typeInfo( GetTypeInfo() );
  if ( typeInfo )
  {
    componentIndex = typeInfo->GetComponentIndex(index);
  }

  // For animatable property, check whether it is registered already and register it if not yet.
  if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) && ( NULL == RegisterAnimatableProperty(index) ) )
  {
    componentIndex = Property::INVALID_COMPONENT_INDEX;
  }

  return componentIndex;
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
    std::string string;

    const char * propertyName = GetDefaultPropertyName( index );
    if( propertyName )
    {
      string = propertyName;
    }
    return string;
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
        if ( NULL == RegisterAnimatableProperty(index) )
        {
          index = Property::INVALID_INDEX;
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
        if ( custom->childPropertyIndex != Property::INVALID_INDEX )
        {
          // If it is a child property, return the child property index
          index = custom->childPropertyIndex;
        }
        else
        {
          index = count;
        }
        break;
      }
    }
  }

  return index;
}

Property::Index Object::GetPropertyIndex( Property::Index key ) const
{
  Property::Index index = Property::INVALID_INDEX;

  if( mCustomProperties.Count() > 0 )
  {
    Property::Index count = PROPERTY_CUSTOM_START_INDEX;
    const PropertyMetadataLookup::ConstIterator end = mCustomProperties.End();
    for( PropertyMetadataLookup::ConstIterator iter = mCustomProperties.Begin(); iter != end; ++iter, ++count )
    {
      CustomPropertyMetadata* custom = static_cast<CustomPropertyMetadata*>(*iter);
      if( custom->key == key )
      {
        if( custom->childPropertyIndex != Property::INVALID_INDEX )
        {
          // If it is a child property, return the child property index
          index = custom->childPropertyIndex;
        }
        else
        {
          index = count;
        }
        break;
      }
    }
  }

  return index;
}

Property::Index Object::GetPropertyIndex( Property::Key key ) const
{
  Property::Index index = Property::INVALID_INDEX;
  if( key.type == Property::Key::INDEX )
  {
    index = GetPropertyIndex( key.indexKey );
  }
  else
  {
    index = GetPropertyIndex( key.stringKey );
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
    return custom->GetType();
  }

  return Property::NONE;
}

void Object::SetProperty( Property::Index index, const Property::Value& propertyValue )
{
  DALI_ASSERT_ALWAYS(index > Property::INVALID_INDEX && "Property index is out of bounds" );

  bool propertySet( true );

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
      propertySet = false;
    }
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
    AnimatablePropertyMetadata* animatableProperty = RegisterAnimatableProperty( index );
    if(!animatableProperty)
    {
      DALI_LOG_ERROR("Cannot find property index\n");
      propertySet = false;
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

    if ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX ) )
    {
      if( !custom )
      {
        // If the child property is not registered yet, register it.
        custom = new CustomPropertyMetadata( "", propertyValue, Property::READ_WRITE );
        mCustomProperties.PushBack( custom );
      }

      custom->childPropertyIndex = index;

      // Resolve name for the child property
      Object* parent = GetParentObject();
      if( parent )
      {
        const TypeInfo* parentTypeInfo( parent->GetTypeInfo() );
        if( parentTypeInfo )
        {
          custom->name = parentTypeInfo->GetChildPropertyName( index );
        }
      }
    }

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
      }
      else
      {
        // trying to set value on read only property is no-op
        propertySet = false;
      }
    }
    else
    {
      DALI_LOG_ERROR("Invalid property index\n");
      propertySet = false;
    }
  }

  // Let derived classes know that a property has been set
  // TODO: We should not call this for read-only properties, SetDefaultProperty() && TypeInfo::SetProperty() should return a bool, which would be true if the property is set
  if ( propertySet )
  {
    OnPropertySet(index, propertyValue);
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
    AnimatablePropertyMetadata* animatableProperty = RegisterAnimatableProperty( index );
    if(!animatableProperty)
    {
      DALI_LOG_ERROR("Cannot find property index\n");
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
  indices.Clear();

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
    indices.Reserve( indices.Size() + mCustomProperties.Count() );

    PropertyMetadataLookup::ConstIterator iter = mCustomProperties.Begin();
    const PropertyMetadataLookup::ConstIterator endIter = mCustomProperties.End();
    int i=0;
    for ( ; iter != endIter; ++iter, ++i )
    {
      CustomPropertyMetadata* custom = static_cast<CustomPropertyMetadata*>( *iter );
      if ( custom->childPropertyIndex != Property::INVALID_INDEX )
      {
        // If it is a child property, add the child property index
        indices.PushBack( custom->childPropertyIndex );
      }
      else
      {
        indices.PushBack( PROPERTY_CUSTOM_START_INDEX + i );
      }
    }
  }
}

Property::Index Object::RegisterSceneGraphProperty(const std::string& name, Property::Index key, Property::Index index, const Property::Value& propertyValue) const
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

    case Property::INTEGER:
    {
      newProperty = new AnimatableProperty<int>( propertyValue.Get<int>() );
      break;
    }

    case Property::FLOAT:
    {
      newProperty = new AnimatableProperty<float>( propertyValue.Get<float>() );
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

    case Property::RECTANGLE:
    case Property::STRING:
    case Property::ARRAY:
    case Property::MAP:
    case Property::NONE:
    {
      DALI_ASSERT_ALWAYS( !"PropertyType is not animatable" );
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
      DALI_ASSERT_ALWAYS( index <= PROPERTY_CUSTOM_MAX_INDEX && "Too many custom properties have been registered" );

      mCustomProperties.PushBack( new CustomPropertyMetadata( name, key, propertyValue.GetType(), property ) );
    }
    else
    {
      mAnimatableProperties.PushBack( new AnimatablePropertyMetadata( index, Property::INVALID_COMPONENT_INDEX, propertyValue.GetType(), property ) ); // base property
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

Property::Index Object::RegisterProperty( const std::string& name, const Property::Value& propertyValue )
{
  return RegisterProperty( name, Property::INVALID_KEY, propertyValue, Property::ANIMATABLE );
}

Property::Index Object::RegisterProperty( const std::string& name, Property::Index key, const Property::Value& propertyValue )
{
  return RegisterProperty( name, key, propertyValue, Property::ANIMATABLE );
}

Property::Index Object::RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode )
{
  return RegisterProperty( name, Property::INVALID_KEY, propertyValue, accessMode );
}

Property::Index Object::RegisterProperty( const std::string& name, Property::Index key, const Property::Value& propertyValue, Property::AccessMode accessMode )
{
  // If property with the required key already exists, then just set it.
  Property::Index index = Property::INVALID_INDEX;
  if( key != Property::INVALID_KEY ) // Try integer key first if it's valid
  {
    index = GetPropertyIndex( key );
  }
  if( index == Property::INVALID_INDEX ) // If it wasn't valid, or doesn't exist, try name
  {
    index = GetPropertyIndex( name );
  }

  if( index != Property::INVALID_INDEX ) // If there was a valid index found by either key, set it.
  {
    SetProperty( index, propertyValue );
  }
  else
  {
    // Otherwise register the property

    if( Property::ANIMATABLE == accessMode )
    {
      index = RegisterSceneGraphProperty( name, key, PROPERTY_CUSTOM_START_INDEX + mCustomProperties.Count(), propertyValue );
      AddUniformMapping( index, name );
    }
    else
    {
      // Add entry to the property lookup
      index = PROPERTY_CUSTOM_START_INDEX + mCustomProperties.Count();

      CustomPropertyMetadata* customProperty = new CustomPropertyMetadata( name, propertyValue, accessMode );

      // Resolve index for the child property
      Object* parent = GetParentObject();
      if( parent )
      {
        const TypeInfo* parentTypeInfo( parent->GetTypeInfo() );
        if( parentTypeInfo )
        {
          Property::Index childPropertyIndex = parentTypeInfo->GetChildPropertyIndex( name );
          if( childPropertyIndex != Property::INVALID_INDEX )
          {
            customProperty->childPropertyIndex = childPropertyIndex;
            index = childPropertyIndex;
          }
        }
      }

      mCustomProperties.PushBack( customProperty );
    }
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
      AnimatablePropertyMetadata* animatable = RegisterAnimatableProperty( index );
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

void Object::AddUniformMapping( Property::Index propertyIndex, const std::string& uniformName ) const
{
  // Get the address of the property if it's a scene property
  const PropertyInputImpl* propertyPtr = GetSceneObjectInputProperty( propertyIndex );

  // Check instead for newly registered properties
  if( propertyPtr == NULL )
  {
    PropertyMetadata* animatable = FindAnimatableProperty( propertyIndex );
    if( animatable != NULL )
    {
      propertyPtr = animatable->GetSceneGraphProperty();
    }
  }

  if( propertyPtr == NULL )
  {
    PropertyMetadata* custom = FindCustomProperty( propertyIndex );
    if( custom != NULL )
    {
      propertyPtr = custom->GetSceneGraphProperty();
    }
  }

  if( propertyPtr != NULL )
  {
    const SceneGraph::PropertyOwner* sceneObject = GetPropertyOwner();

    if( sceneObject != NULL )
    {
      SceneGraph::UniformPropertyMapping* map = new SceneGraph::UniformPropertyMapping( uniformName, propertyPtr );
      // Message takes ownership of Uniform map (and will delete it after copy)
      AddUniformMapMessage( const_cast<EventThreadServices&>(GetEventThreadServices()), *sceneObject, map);
    }
    else
    {
      DALI_ASSERT_ALWAYS(0 && "MESH_REWORK - Need to store property whilst off-stage" );
    }
  }
}

void Object::RemoveUniformMapping( const std::string& uniformName )
{
  const SceneGraph::PropertyOwner* sceneObject = GetSceneObject();
  RemoveUniformMapMessage( GetEventThreadServices(), *sceneObject, uniformName);
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

    switch ( entry->GetType() )
    {
      case Property::BOOLEAN:
      {
        const AnimatableProperty<bool>* property = static_cast< const AnimatableProperty<bool>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::INTEGER:
      {
        const AnimatableProperty<int>* property = static_cast< const AnimatableProperty<int>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::FLOAT:
      {
        const AnimatableProperty<float>* property = static_cast< const AnimatableProperty<float>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::VECTOR2:
      {
        const AnimatableProperty<Vector2>* property = static_cast< const AnimatableProperty<Vector2>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        if(entry->componentIndex == 0)
        {
          value = (*property)[ bufferIndex ].x;
        }
        else if(entry->componentIndex == 1)
        {
          value = (*property)[ bufferIndex ].y;
        }
        else
        {
          value = (*property)[ bufferIndex ];
        }
        break;
      }

      case Property::VECTOR3:
      {
        const AnimatableProperty<Vector3>* property = static_cast< const AnimatableProperty<Vector3>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        if(entry->componentIndex == 0)
        {
          value = (*property)[ bufferIndex ].x;
        }
        else if(entry->componentIndex == 1)
        {
          value = (*property)[ bufferIndex ].y;
        }
        else if(entry->componentIndex == 2)
        {
          value = (*property)[ bufferIndex ].z;
        }
        else
        {
          value = (*property)[ bufferIndex ];
        }
        break;
      }

      case Property::VECTOR4:
      {
        const AnimatableProperty<Vector4>* property = static_cast< const AnimatableProperty<Vector4>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        if(entry->componentIndex == 0)
        {
          value = (*property)[ bufferIndex ].x;
        }
        else if(entry->componentIndex == 1)
        {
          value = (*property)[ bufferIndex ].y;
        }
        else if(entry->componentIndex == 2)
        {
          value = (*property)[ bufferIndex ].z;
        }
        else if(entry->componentIndex == 3)
        {
          value = (*property)[ bufferIndex ].w;
        }
        else
        {
          value = (*property)[ bufferIndex ];
        }
        break;
      }

      case Property::MATRIX:
      {
        const AnimatableProperty<Matrix>* property = static_cast< const AnimatableProperty<Matrix>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::MATRIX3:
      {
        const AnimatableProperty<Matrix3>* property = static_cast< const AnimatableProperty<Matrix3>* >( entry->GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( NULL != property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::ROTATION:
      {
        const AnimatableProperty<Quaternion>* property = static_cast< const AnimatableProperty<Quaternion>* >( entry->GetSceneGraphProperty() );
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
  switch ( entry.GetType() )
  {
    case Property::BOOLEAN:
    {
      const AnimatableProperty<bool>* property = dynamic_cast< const AnimatableProperty<bool>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<bool>( GetEventThreadServices(), *property, value.Get<bool>() );
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

    case Property::FLOAT:
    {
      const AnimatableProperty<float>* property = dynamic_cast< const AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<float>( GetEventThreadServices(), *property, value.Get<float>() );
      break;
    }

    case Property::VECTOR2:
    {
      const AnimatableProperty<Vector2>* property = dynamic_cast< const AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SetXComponentMessage<Vector2>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else if(entry.componentIndex == 1)
      {
        SetYComponentMessage<Vector2>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else
      {
        BakeMessage<Vector2>( GetEventThreadServices(), *property, value.Get<Vector2>() );
      }
      break;
    }

    case Property::VECTOR3:
    {
      const AnimatableProperty<Vector3>* property = dynamic_cast< const AnimatableProperty<Vector3>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SetXComponentMessage<Vector3>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else if(entry.componentIndex == 1)
      {
        SetYComponentMessage<Vector3>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else if(entry.componentIndex == 2)
      {
        SetZComponentMessage<Vector3>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else
      {
        BakeMessage<Vector3>( GetEventThreadServices(), *property, value.Get<Vector3>() );
      }

      break;
    }

    case Property::VECTOR4:
    {
      const AnimatableProperty<Vector4>* property = dynamic_cast< const AnimatableProperty<Vector4>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( NULL != property );

      // property is being used in a separate thread; queue a message to set the property
      if(entry.componentIndex == 0)
      {
        SetXComponentMessage<Vector4>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else if(entry.componentIndex == 1)
      {
        SetYComponentMessage<Vector4>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else if(entry.componentIndex == 2)
      {
        SetZComponentMessage<Vector4>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else if(entry.componentIndex == 3)
      {
        SetWComponentMessage<Vector4>( GetEventThreadServices(), *property, value.Get<float>() );
      }
      else
      {
        BakeMessage<Vector4>( GetEventThreadServices(), *property, value.Get<Vector4>() );
      }
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

void Object::ApplyConstraint( ConstraintBase& constraint )
{
  if( !mConstraints )
  {
    mConstraints = new ConstraintContainer;
  }
  mConstraints->push_back( Dali::Constraint( &constraint ) );
}

void Object::RemoveConstraint( ConstraintBase& constraint )
{
  // NULL if the Constraint sources are destroyed before Constraint::Apply()
  if( mConstraints )
  {
    ConstraintIter it( std::find( mConstraints->begin(), mConstraints->end(), Dali::Constraint( &constraint ) ) );
    if( it != mConstraints->end() )
    {
      mConstraints->erase( it );
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
      const ConstraintConstIter endIter = mConstraints->end();
      for ( ConstraintIter iter = mConstraints->begin(); endIter != iter; ++iter )
      {
        GetImplementation( *iter ).RemoveInternal();
      }
    }

    delete mConstraints;
    mConstraints = NULL;
  }
}

void Object::RemoveConstraints( unsigned int tag )
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    ConstraintIter iter( mConstraints->begin() );
    while(iter != mConstraints->end() )
    {
      ConstraintBase& constraint = GetImplementation( *iter );
      if( constraint.GetTag() == tag )
      {
        GetImplementation( *iter ).RemoveInternal();
        iter = mConstraints->erase( iter );
      }
      else
      {
        ++iter;
      }
    }

    if ( mConstraints->empty() )
    {
      delete mConstraints;
      mConstraints = NULL;
    }
  }
}

void Object::SetTypeInfo( const TypeInfo* typeInfo )
{
  mTypeInfo = typeInfo;
}

Object::~Object()
{
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
  if ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    for ( std::size_t arrayIndex = 0; arrayIndex < mCustomProperties.Count(); arrayIndex++ )
    {
      CustomPropertyMetadata* custom = static_cast<CustomPropertyMetadata*>( mCustomProperties[ arrayIndex ] );
      if( custom->childPropertyIndex == index )
      {
        property = custom;
      }
    }
  }
  else
  {
    int arrayIndex = index - PROPERTY_CUSTOM_START_INDEX;
    if( arrayIndex >= 0 )
    {
      if( arrayIndex < (int)mCustomProperties.Count() ) // we can only access the first 2 billion custom properties
      {
        property = static_cast<CustomPropertyMetadata*>(mCustomProperties[ arrayIndex ]);
      }
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

AnimatablePropertyMetadata* Object::RegisterAnimatableProperty(Property::Index index) const
{
  DALI_ASSERT_ALWAYS( (( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ))
                      && "Property index is out of bounds" );

  // check whether the animatable property is registered already, if not then register one.
  AnimatablePropertyMetadata* animatableProperty = FindAnimatableProperty( index );
  if( !animatableProperty )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if( typeInfo )
    {
      Property::Index basePropertyIndex = typeInfo->GetBasePropertyIndex(index);
      if( basePropertyIndex == Property::INVALID_INDEX )
      {
        // If the property is not a component of a base property, register the whole property itself.
        const  std::string& propertyName = typeInfo->GetPropertyName(index);
        RegisterSceneGraphProperty(propertyName, Property::INVALID_KEY, index, typeInfo->GetPropertyDefaultValue(index));
        AddUniformMapping( index, propertyName );
      }
      else
      {
        // Since the property is a component of a base property, check whether the base property is registered.
        animatableProperty = FindAnimatableProperty( basePropertyIndex );
        if( !animatableProperty )
        {
          // If the base property is not registered yet, register the base property first.
          const  std::string& basePropertyName = typeInfo->GetPropertyName(basePropertyIndex);

          if( Property::INVALID_INDEX != RegisterSceneGraphProperty( basePropertyName, Property::INVALID_KEY, basePropertyIndex, Property::Value(typeInfo->GetPropertyType( basePropertyIndex ) ) ) )
          {
            animatableProperty = static_cast<AnimatablePropertyMetadata*>(mAnimatableProperties[mAnimatableProperties.Size()-1]);
            AddUniformMapping( basePropertyIndex, basePropertyName );
          }
        }

        if(animatableProperty)
        {
          // Create the metadata for the property component.
          mAnimatableProperties.PushBack( new AnimatablePropertyMetadata( index, typeInfo->GetComponentIndex(index), animatableProperty->GetType(), animatableProperty->GetSceneGraphProperty() ) );
        }
      }

      // The metadata has just been added and therefore should be in the end of the vector.
      animatableProperty = static_cast<AnimatablePropertyMetadata*>(mAnimatableProperties[mAnimatableProperties.Size()-1]);
    }
  }

  return animatableProperty;
}

void Object::ResolveChildProperties()
{
  // Resolve index for the child property
  Object* parent = GetParentObject();
  if( parent )
  {
    const TypeInfo* parentTypeInfo( parent->GetTypeInfo() );
    if( parentTypeInfo )
    {
      // Go through each custom property
      for ( int arrayIndex = 0; arrayIndex < (int)mCustomProperties.Count(); arrayIndex++ )
      {
        CustomPropertyMetadata* customProperty = static_cast<CustomPropertyMetadata*>( mCustomProperties[ arrayIndex ] );

        if( customProperty->name == "" )
        {
          if( customProperty->childPropertyIndex != Property::INVALID_INDEX )
          {
            // Resolve name for any child property with no name
            customProperty->name = parentTypeInfo->GetChildPropertyName( customProperty->childPropertyIndex );
          }
        }
        else
        {
          Property::Index childPropertyIndex = parentTypeInfo->GetChildPropertyIndex( customProperty->name );
          if( childPropertyIndex != Property::INVALID_INDEX )
          {
            // Resolve index for any property with a name that matches the parent's child property name
            customProperty->childPropertyIndex = childPropertyIndex;
          }
        }
      }
    }
  }
}

} // namespace Internal

} // namespace Dali
