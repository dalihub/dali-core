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
#include <dali/internal/event/common/object-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/type-registry-impl.h>

using Dali::Internal::SceneGraph::AnimatableProperty;
using Dali::Internal::SceneGraph::PropertyBase;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{
const int32_t SUPPORTED_CAPABILITIES = Dali::Handle::DYNAMIC_PROPERTIES;  // Object provides this capability

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_OBJECT" );
#endif

constexpr Property::Index MAX_PER_CLASS_PROPERTY_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX;

} // unnamed namespace

IntrusivePtr<Object> Object::New()
{
  return new Object( nullptr ); // no scene object by default
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
  const auto endIter =  mObservers.End();
  for( auto iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    if( (*iter) == &observer)
    {
      mObservers.Erase( iter );
      break;
    }
  }
  DALI_ASSERT_DEBUG(endIter != mObservers.End());
}

bool Object::Supports( Capability capability ) const
{
  return (capability & SUPPORTED_CAPABILITIES);
}

uint32_t Object::GetPropertyCount() const
{
  uint32_t count = 0u;
  const TypeInfo* typeInfo( GetTypeInfo() );
  if ( typeInfo )
  {
    count = typeInfo->GetPropertyCount();

    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Registered Properties:  %d\n", count );
  }

  uint32_t custom = static_cast<uint32_t>( mCustomProperties.Count() );
  count += custom;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Custom Properties:  %d\n", custom );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Total Properties:   %d\n", count );

  return count;
}

std::string Object::GetPropertyName( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( index > Property::INVALID_INDEX && "Property index out of bounds" );

  // is this a per class or per instance property
  if ( index < MAX_PER_CLASS_PROPERTY_INDEX )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      return typeInfo->GetPropertyName( index );
    }
  }
  else // child property or custom property
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if( custom )
    {
      return custom->name;
    }
  }

  DALI_LOG_ERROR( "Property index %d not found\n", index );
  return std::string();
}

Property::Index Object::GetPropertyIndex( const std::string& name ) const
{
  Property::Index index = Property::INVALID_INDEX;

  const TypeInfo* typeInfo( GetTypeInfo() );
  if ( typeInfo )
  {
    index = typeInfo->GetPropertyIndex( name );
  }
  if( (index == Property::INVALID_INDEX)&&( mCustomProperties.Count() > 0 ) )
  {
    Property::Index count = PROPERTY_CUSTOM_START_INDEX;
    const auto end = mCustomProperties.End();
    for( auto iter = mCustomProperties.Begin(); iter != end; ++iter, ++count )
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
    const auto end = mCustomProperties.End();
    for( auto iter = mCustomProperties.Begin(); iter != end; ++iter, ++count )
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

  // is this a per class or per instance property
  if ( index < MAX_PER_CLASS_PROPERTY_INDEX )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      writable = typeInfo->IsPropertyWritable( index );
    }
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

  // is this a per class or per instance property
  if ( index < MAX_PER_CLASS_PROPERTY_INDEX )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      animatable = typeInfo->IsPropertyAnimatable( index );
    }
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

  // is this a per class or per instance property
  if ( index < MAX_PER_CLASS_PROPERTY_INDEX )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      isConstraintInput = typeInfo->IsPropertyAConstraintInput( index );
    }
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

  // is this a per class or per instance property
  if ( index < MAX_PER_CLASS_PROPERTY_INDEX )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      return typeInfo->GetPropertyType( index );
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
      // cannot register this property as there is no setter for it.
      // event side properties must have a setter for now so need to be registered
      DALI_LOG_ERROR( "Property index %d not found\n", index );
      propertySet = false;
    }
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
    AnimatablePropertyMetadata* animatableProperty = GetSceneAnimatableProperty( index, &propertyValue );
    if( !animatableProperty )
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
      propertySet = false;
    }
    else
    {
      // update the cached property value
      animatableProperty->SetPropertyValue( propertyValue );

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
        // update the cached property value
        custom->SetPropertyValue( propertyValue );

        // set the scene graph property value
        SetSceneGraphProperty( index, *custom, propertyValue );
      }
      else if( custom->IsWritable() )
      {
        // update the cached property value
        custom->SetPropertyValue( propertyValue );
      }
      else
      {
        // trying to set value on read only property is no-op
        propertySet = false;
      }
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
      propertySet = false;
    }
  }

  // Let derived classes know that a property has been set
  // TODO: We should not call this for read-only properties, SetDefaultProperty() && TypeInfo::SetProperty() should return a bool, which would be true if the property is set
  if ( propertySet )
  {
    OnPropertySet( index, propertyValue );
    Dali::Handle handle( this );
    mPropertySetSignal.Emit( handle, index, propertyValue );
  }
}

Property::Value Object::GetProperty( Property::Index index ) const
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
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
	  // this is needed because property value may have been set as full property and get as a property component
    AnimatablePropertyMetadata* animatableProperty = GetSceneAnimatableProperty( index, nullptr );
    if( animatableProperty )
    {
      // get the cached animatable property value
      value = animatableProperty->GetPropertyValue();
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }
  else if(mCustomProperties.Count() > 0)
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if(custom)
    {
      // get the cached custom property value
      value = custom->GetPropertyValue();
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  } // if custom

  return value;
}

Property::Value Object::GetCurrentProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( index > Property::INVALID_INDEX && "Property index is out of bounds" );

  Property::Value value;

  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    value = GetDefaultPropertyCurrentValue( index );
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
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }
  else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
	  // this is needed because property value may have been set as full property and get as a property component
    AnimatablePropertyMetadata* animatableProperty = GetSceneAnimatableProperty( index, nullptr );
    if( animatableProperty )
    {
      // get the animatable property value
      value = GetCurrentPropertyValue( *animatableProperty );
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }
  else if(mCustomProperties.Count() > 0)
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    if(custom)
    {
      // get the custom property value
      value = GetCurrentPropertyValue( *custom );
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  } // if custom

  return value;
}

void Object::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.Clear();

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

    auto iter = mCustomProperties.Begin();
    const auto endIter = mCustomProperties.End();
    int32_t i = 0;
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

Property::Index Object::RegisterProperty( const std::string& name, const Property::Value& propertyValue )
{
  return RegisterProperty( name, Property::INVALID_KEY, propertyValue, Property::ANIMATABLE );
}

Property::Index Object::RegisterProperty( const std::string& name, Property::Index key, const Property::Value& propertyValue )
{
  return RegisterProperty( name, key, propertyValue, Property::ANIMATABLE );
}

void Object::SetProperties( const Property::Map& properties )
{
  const auto count = properties.Count();
  for( auto position = 0u; position < count; ++position )
  {
    // GetKeyAt and GetValue both return references which means no potential copying of maps/arrays.
    // Iterating twice to get the value we want should still be fairly quick in a Property::Map.

    const auto& key = properties.GetKeyAt( position );
    const auto propertyIndex = ( key.type == Property::Key::INDEX ) ? key.indexKey : GetPropertyIndex( key.stringKey );

    if( propertyIndex != Property::INVALID_INDEX )
    {
      const auto& value = properties.GetValue( position );
      SetProperty( propertyIndex, value );
    }
  }
}

void Object::GetProperties( Property::Map& properties )
{
  properties.Clear();

  Property::IndexContainer indexContainer;
  GetPropertyIndices( indexContainer );

  for( auto index : indexContainer )
  {
    properties[ index ] = GetProperty( index );
  }
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
      index = RegisterSceneGraphProperty( name, key, PROPERTY_CUSTOM_START_INDEX + static_cast<Property::Index>( mCustomProperties.Count() ), propertyValue );
      AddUniformMapping( index, name );
    }
    else
    {
      // Add entry to the property lookup
      index = PROPERTY_CUSTOM_START_INDEX + static_cast<Property::Index>( mCustomProperties.Count() );

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

bool Object::DoesCustomPropertyExist( Property::Index index )
{
  auto metadata = FindCustomProperty( index );
  return metadata != nullptr;
}

Dali::PropertyNotification Object::AddPropertyNotification( Property::Index index,
                                                            int32_t componentIndex,
                                                            const Dali::PropertyCondition& condition)
{
  if ( index >= DEFAULT_PROPERTY_MAX_COUNT )
  {
    if ( index <= PROPERTY_REGISTRATION_MAX_INDEX )
    {
      DALI_ABORT( "Property notification added to event side only property." );
    }
    else if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
    {
      // check whether the animatable property is registered already, if not then register one.
      AnimatablePropertyMetadata* animatable = GetSceneAnimatableProperty( index, nullptr );
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
    auto iter = mPropertyNotifications->begin();
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
    auto iter = mPropertyNotifications->begin();
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

void Object::NotifyPropertyAnimation( Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType )
{
  if ( index < DEFAULT_PROPERTY_MAX_COUNT )
  {
    OnNotifyDefaultPropertyAnimation( animation, index, value, animationType );
  }
  else
  {
    PropertyMetadata* propertyMetadata = nullptr;
    if( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
    {
      propertyMetadata = FindAnimatableProperty( index );
    }
    else
    {
      CustomPropertyMetadata* custom = FindCustomProperty( index );
      if( custom && custom->IsAnimatable() )
      {
        propertyMetadata = custom;
      }
    }

    if( propertyMetadata )
    {
      switch( animationType )
      {
        case Animation::TO:
        case Animation::BETWEEN:
        {
          // Update the cached property value
          propertyMetadata->SetPropertyValue( value );
          break;
        }
        case Animation::BY:
        {
          // Adjust the cached property value
          propertyMetadata->AdjustPropertyValueBy( value );
          break;
        }
      }
    }
  }
}

void Object::AddUniformMapping( Property::Index propertyIndex, const std::string& uniformName ) const
{
  // Get the address of the property if it's a scene property
  const PropertyInputImpl* propertyPtr = GetSceneObjectInputProperty( propertyIndex );

  // Check instead for newly registered properties
  if( propertyPtr == nullptr )
  {
    PropertyMetadata* animatable = FindAnimatableProperty( propertyIndex );
    if( animatable )
    {
      propertyPtr = animatable->GetSceneGraphProperty();
    }
  }

  if( propertyPtr == nullptr )
  {
    PropertyMetadata* custom = FindCustomProperty( propertyIndex );
    if( custom )
    {
      propertyPtr = custom->GetSceneGraphProperty();
    }
  }

  if( propertyPtr )
  {
    const SceneGraph::PropertyOwner& sceneObject = GetSceneObject();

    OwnerPointer< SceneGraph::UniformPropertyMapping > map = new SceneGraph::UniformPropertyMapping( uniformName, propertyPtr );
    // Message takes ownership of Uniform map (and will delete it after copy)
    AddUniformMapMessage( const_cast<EventThreadServices&>(GetEventThreadServices()), sceneObject, map );
  }
}

void Object::RemoveUniformMapping( const std::string& uniformName ) const
{
  const SceneGraph::PropertyOwner& sceneObject = GetSceneObject();
  RemoveUniformMapMessage( const_cast<EventThreadServices&>(GetEventThreadServices()), sceneObject, uniformName);
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
  // nullptr if the Constraint sources are destroyed before Constraint::Apply()
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
    for ( auto&& item : *mConstraints )
    {
      GetImplementation( item ).RemoveInternal();
    }

    delete mConstraints;
    mConstraints = nullptr;
  }
}

void Object::RemoveConstraints( uint32_t tag )
{
  // guard against constraint sending messages during core destruction
  if( mConstraints && Stage::IsInstalled() )
  {
    auto iter( mConstraints->begin() );
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
      mConstraints = nullptr;
    }
  }
}

void Object::SetTypeInfo( const TypeInfo* typeInfo )
{
  mTypeInfo = typeInfo;
}

const SceneGraph::PropertyOwner& Object::GetSceneObject() const
{
  if( !mUpdateObject )
  {
    auto sceneObject = SceneGraph::PropertyOwner::New();
    OwnerPointer< SceneGraph::PropertyOwner > transferOwnership( sceneObject );
    mUpdateObject = sceneObject;
    AddObjectMessage( const_cast<EventThreadServices&>( GetEventThreadServices() ).GetUpdateManager(), transferOwnership );
  }
  DALI_ASSERT_DEBUG( mUpdateObject && "there must always be a scene object" );
  return *mUpdateObject;
}

const PropertyBase* Object::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  const SceneGraph::PropertyBase* property = nullptr;
  if ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX && index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX )
  {
    AnimatablePropertyMetadata* animatable = GetSceneAnimatableProperty( index, nullptr );
    DALI_ASSERT_ALWAYS( animatable && "Property index is invalid" );

    property = animatable->GetSceneGraphProperty();
  }
  else if ( ( index >= CHILD_PROPERTY_REGISTRATION_START_INDEX ) && // Child properties are also stored as custom properties
            ( index <= PROPERTY_CUSTOM_MAX_INDEX ) )
  {
    CustomPropertyMetadata* custom = FindCustomProperty( index );
    DALI_ASSERT_ALWAYS( custom && "Property index is invalid" );

    property = custom->GetSceneGraphProperty();
  }
  return property;
}

const PropertyInputImpl* Object::GetSceneObjectInputProperty( Property::Index index ) const
{
  // reuse animatable version as they are inputs as well
  return GetSceneObjectAnimatableProperty( index );
}

int32_t Object::GetPropertyComponentIndex( Property::Index index ) const
{
  int32_t componentIndex = Property::INVALID_COMPONENT_INDEX;

  if ( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // check whether the animatable property is registered already, if not then register one.
    AnimatablePropertyMetadata* animatableProperty = GetSceneAnimatableProperty( index, nullptr );
    if( animatableProperty )
    {
      componentIndex = animatableProperty->componentIndex;
    }
  }
  if( Property::INVALID_COMPONENT_INDEX == componentIndex )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if ( typeInfo )
    {
      componentIndex = typeInfo->GetComponentIndex(index);
    }
  }

  return componentIndex;
}

DevelHandle::PropertySetSignalType& Object::PropertySetSignal()
{
  return mPropertySetSignal;
}

Object::Object( const SceneGraph::PropertyOwner* sceneObject )
: mEventThreadServices( EventThreadServices::Get() ),
  mUpdateObject( sceneObject ),
  mTypeInfo( nullptr ),
  mConstraints( nullptr ),
  mPropertyNotifications( nullptr )
{
}

Object::~Object()
{
  // Notification for observers
  for( auto&& item : mObservers )
  {
    item->ObjectDestroyed( *this );
  }
  delete mConstraints;
  delete mPropertyNotifications;

  // Guard to allow handle destruction after Core has been destroyed
  if( Stage::IsInstalled() )
  {
    if( nullptr != mUpdateObject )
    {
      RemoveObjectMessage( GetEventThreadServices().GetUpdateManager(), mUpdateObject );
    }
  }
}

void Object::OnSceneObjectAdd()
{
  // Notification for observers
  for( auto&& item : mObservers )
  {
    item->SceneObjectAdded(*this);
  }

  // enable property notifications in scene graph
  EnablePropertyNotifications();
}

void Object::OnSceneObjectRemove()
{
  // Notification for observers
  for( auto&& item : mObservers )
  {
    item->SceneObjectRemoved(*this);
  }

  // disable property notifications in scene graph
  DisablePropertyNotifications();
}

const TypeInfo* Object::GetTypeInfo() const
{
  if ( !mTypeInfo )
  {
    // This uses a dynamic_cast so can be quite expensive so we only really want to do it once
    // especially as the type-info does not change during the life-time of an application

    TypeRegistry::TypeInfoPointer typeInfoHandle = TypeRegistry::Get()->GetTypeInfo( this );
    if ( typeInfoHandle )
    {
      mTypeInfo = typeInfoHandle.Get(); // just a raw pointer to use, ownership is kept
    }
  }

  return mTypeInfo;
}

CustomPropertyMetadata* Object::FindCustomProperty( Property::Index index ) const
{
  CustomPropertyMetadata* property = nullptr;
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
    int32_t arrayIndex = index - PROPERTY_CUSTOM_START_INDEX;
    if( arrayIndex >= 0 )
    {
      if( arrayIndex < static_cast<int32_t>( mCustomProperties.Count() ) ) // we can only access the first 2 billion custom properties
      {
        property = static_cast<CustomPropertyMetadata*>(mCustomProperties[ arrayIndex ]);
      }
    }
  }
  return property;
}

AnimatablePropertyMetadata* Object::FindAnimatableProperty( Property::Index index ) const
{
  for( auto&& entry : mAnimatableProperties )
  {
    AnimatablePropertyMetadata* property = static_cast<AnimatablePropertyMetadata*>( entry );
    if( property->index == index )
    {
      return property;
    }
  }
  return nullptr;
}

Property::Index Object::RegisterSceneGraphProperty( const std::string& name, Property::Index key, Property::Index index, const Property::Value& propertyValue ) const
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
      newProperty = new AnimatableProperty<int32_t>( propertyValue.Get<int32_t>() );
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
    case Property::EXTENTS:
    case Property::NONE:
    {
      DALI_ASSERT_ALWAYS( !"Property type is not animatable" );
      break;
    }
  }

  // get the scene property owner
  const SceneGraph::PropertyOwner& scenePropertyOwner = GetSceneObject();
  // keep a local pointer to the property as the OwnerPointer will pass its copy to the message
  const PropertyBase* property = newProperty.Get();
  if(index >= PROPERTY_CUSTOM_START_INDEX)
  {
    DALI_ASSERT_ALWAYS( index <= PROPERTY_CUSTOM_MAX_INDEX && "Too many custom properties have been registered" );

    mCustomProperties.PushBack( new CustomPropertyMetadata( name, key, propertyValue, property ) );
  }
  else
  {
    mAnimatableProperties.PushBack( new AnimatablePropertyMetadata( index, propertyValue, property ) );
  }

  // queue a message to add the property
  InstallCustomPropertyMessage( const_cast<EventThreadServices&>(GetEventThreadServices()), scenePropertyOwner, newProperty ); // Message takes ownership

  return index;
}

void Object::RegisterAnimatableProperty( const TypeInfo& typeInfo,
                                          Property::Index index,
                                          const Property::Value* value ) const
{
  // If the property is not a component of a base property, register the whole property itself.
  const std::string& propertyName = typeInfo.GetPropertyName( index );
  Property::Value initialValue;
  if( value )
  {
    initialValue = *value;
  }
  else
  {
    initialValue = typeInfo.GetPropertyDefaultValue( index ); // recurses type hierarchy
    if( Property::NONE == initialValue.GetType() )
    {
      initialValue = Property::Value( typeInfo.GetPropertyType( index ) ); // recurses type hierarchy
    }
  }
  RegisterSceneGraphProperty( propertyName, Property::INVALID_KEY, index, initialValue );
  AddUniformMapping( index, propertyName );
}

AnimatablePropertyMetadata* Object::GetSceneAnimatableProperty( Property::Index index, const Property::Value* value ) const
{
  // property range already checked by calling methods
  // check whether the animatable property is registered already, if not then register one.
  AnimatablePropertyMetadata* animatableProperty = FindAnimatableProperty( index );
  if( !animatableProperty )
  {
    const TypeInfo* typeInfo( GetTypeInfo() );
    if( typeInfo )
    {
      Property::Index basePropertyIndex = typeInfo->GetBasePropertyIndex( index );
      if( basePropertyIndex == Property::INVALID_INDEX )
      {
        // If the property is not a component of a base property, register the whole property itself.
        RegisterAnimatableProperty( *typeInfo, index, value );
      }
      else
      {
        // Since the property is a component of a base property, check whether the base property is registered.
        animatableProperty = FindAnimatableProperty( basePropertyIndex );
        if( !animatableProperty )
        {
          // If the base property is not registered yet, register the base property first.
          RegisterAnimatableProperty( *typeInfo, basePropertyIndex, value );
          animatableProperty = static_cast<AnimatablePropertyMetadata*>(mAnimatableProperties[mAnimatableProperties.Size()-1]);
        }

        // Create the metadata for the property component.
        mAnimatableProperties.PushBack( new AnimatablePropertyMetadata( index, typeInfo->GetComponentIndex(index), animatableProperty->value, animatableProperty->GetSceneGraphProperty() ) );
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
      for( auto&& entry : mCustomProperties )
      {
        CustomPropertyMetadata* customProperty = static_cast<CustomPropertyMetadata*>( entry );

        if( customProperty->name.empty() )
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

void Object::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  // do nothing
}

Property::Value Object::GetDefaultProperty(Property::Index index) const
{
  return Property::Value();
}

Property::Value Object::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  return GetDefaultProperty( index );
}

void Object::EnablePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    for( auto&& element : *mPropertyNotifications )
    {
      GetImplementation( element ).Enable();
    }
  }
}

void Object::DisablePropertyNotifications()
{
  if( mPropertyNotifications )
  {
    for( auto&& element : *mPropertyNotifications )
    {
      GetImplementation( element ).Disable();
    }
  }
}

Property::Value Object::GetCurrentPropertyValue( const PropertyMetadata& entry ) const
{
  Property::Value value;

  if( !entry.IsAnimatable() )
  {
    value = entry.GetPropertyValue();
  }
  else
  {
    BufferIndex bufferIndex( GetEventThreadServices().GetEventBufferIndex() );

    switch ( entry.GetType() )
    {
      case Property::BOOLEAN:
      {
        const AnimatableProperty<bool>* property = static_cast< const AnimatableProperty<bool>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::INTEGER:
      {
        const AnimatableProperty<int32_t>* property = static_cast< const AnimatableProperty<int32_t>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::FLOAT:
      {
        const AnimatableProperty<float>* property = static_cast< const AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::VECTOR2:
      {
        const AnimatableProperty<Vector2>* property = static_cast< const AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        if(entry.componentIndex == 0)
        {
          value = (*property)[ bufferIndex ].x;
        }
        else if(entry.componentIndex == 1)
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
        const AnimatableProperty<Vector3>* property = static_cast< const AnimatableProperty<Vector3>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        if(entry.componentIndex == 0)
        {
          value = (*property)[ bufferIndex ].x;
        }
        else if(entry.componentIndex == 1)
        {
          value = (*property)[ bufferIndex ].y;
        }
        else if(entry.componentIndex == 2)
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
        const AnimatableProperty<Vector4>* property = static_cast< const AnimatableProperty<Vector4>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        if(entry.componentIndex == 0)
        {
          value = (*property)[ bufferIndex ].x;
        }
        else if(entry.componentIndex == 1)
        {
          value = (*property)[ bufferIndex ].y;
        }
        else if(entry.componentIndex == 2)
        {
          value = (*property)[ bufferIndex ].z;
        }
        else if(entry.componentIndex == 3)
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
        const AnimatableProperty<Matrix>* property = static_cast< const AnimatableProperty<Matrix>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::MATRIX3:
      {
        const AnimatableProperty<Matrix3>* property = static_cast< const AnimatableProperty<Matrix3>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        value = (*property)[ bufferIndex ];
        break;
      }

      case Property::ROTATION:
      {
        const AnimatableProperty<Quaternion>* property = static_cast< const AnimatableProperty<Quaternion>* >( entry.GetSceneGraphProperty() );
        DALI_ASSERT_DEBUG( property );

        value = (*property)[ bufferIndex ];
        break;
      }

      default:
      {
        // unreachable code due to higher level logic
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
      DALI_ASSERT_DEBUG( property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<bool>( GetEventThreadServices(), *property, value.Get<bool>() );
      break;
    }

    case Property::INTEGER:
    {
      const AnimatableProperty<int32_t>* property = dynamic_cast< const AnimatableProperty<int32_t>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<int32_t>( GetEventThreadServices(), *property, value.Get<int32_t>() );
      break;
    }

    case Property::FLOAT:
    {
      const AnimatableProperty<float>* property = dynamic_cast< const AnimatableProperty<float>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<float>( GetEventThreadServices(), *property, value.Get<float>() );
      break;
    }

    case Property::VECTOR2:
    {
      const AnimatableProperty<Vector2>* property = dynamic_cast< const AnimatableProperty<Vector2>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( property );

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
      DALI_ASSERT_DEBUG( property );

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
      DALI_ASSERT_DEBUG( property );

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
      DALI_ASSERT_DEBUG( property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Quaternion>( GetEventThreadServices(), *property, value.Get<Quaternion>() );
      break;
    }

    case Property::MATRIX:
    {
      const AnimatableProperty<Matrix>* property = dynamic_cast< const AnimatableProperty<Matrix>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( property );

      // property is being used in a separate thread; queue a message to set the property
      BakeMessage<Matrix>( GetEventThreadServices(), *property, value.Get<Matrix>() );
      break;
    }

    case Property::MATRIX3:
    {
      const AnimatableProperty<Matrix3>* property = dynamic_cast< const AnimatableProperty<Matrix3>* >( entry.GetSceneGraphProperty() );
      DALI_ASSERT_DEBUG( property );

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

} // namespace Internal

} // namespace Dali
