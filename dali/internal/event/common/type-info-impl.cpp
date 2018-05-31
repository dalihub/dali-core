/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/type-info-impl.h>

// EXTERNAL INCLUDES
#include <algorithm> // std::find_if
#include <string>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/type-registry-impl.h>
#include <dali/internal/event/common/object-impl.h>

using std::find_if;

namespace
{

/*
 * Functor to find by given type for vector of pairs
 */
template <typename S, typename T>
struct PairFinder
{
  PairFinder(const S& find)
  : mFind(find)
  {
  }

  bool operator()(const T& p) const
  {
    return p.first == mFind;
  }

private:

  const S& mFind;
};

/**
 * Functor to find a matching property name
 */
template <typename T>
struct PropertyNameFinder
{
  PropertyNameFinder( const std::string& find )
  : mFind( find )
  {
  }

  bool operator()(const T &p) const
  {
    return p.second.name == mFind;
  }

private:

  const std::string& mFind;
};

/**
 * Functor to find a matching property component index
 */
template <typename T>
struct PropertyComponentFinder
{
  PropertyComponentFinder( Dali::Property::Index basePropertyIndex, const int find )
  : mBasePropertyIndex( basePropertyIndex ),
    mFind( find )
  {
  }

  bool operator()(const T &p) const
  {
    return ( p.second.basePropertyIndex == mBasePropertyIndex && p.second.componentIndex == mFind );
  }

private:

  Dali::Property::Index mBasePropertyIndex;
  const int mFind;
};

} // namespace anon

namespace Dali
{

namespace Internal
{

TypeInfo::TypeInfo(const std::string &name, const std::string &baseTypeName, Dali::TypeInfo::CreateFunction creator)
  : mTypeName(name), mBaseTypeName(baseTypeName), mCSharpType(false), mCreate(creator)
{
  DALI_ASSERT_ALWAYS(!name.empty() && "Type info construction must have a name");
  DALI_ASSERT_ALWAYS(!baseTypeName.empty() && "Type info construction must have a base type name");
}

TypeInfo::TypeInfo(const std::string &name, const std::string &baseTypeName, Dali::CSharpTypeInfo::CreateFunction creator)
  : mTypeName(name), mBaseTypeName(baseTypeName), mCSharpType(true), mCSharpCreate(creator)
{
  DALI_ASSERT_ALWAYS(!name.empty() && "Type info construction must have a name");
  DALI_ASSERT_ALWAYS(!baseTypeName.empty() && "Type info construction must have a base type name");
}

TypeInfo::~TypeInfo()
{
}

BaseHandle TypeInfo::CreateInstance() const
{
  BaseHandle ret;

  if(mCreate)
  {
    if ( mCSharpType )
    {
      // CSharp currently only registers one create function for all custom controls
      // it uses the type name to decide which one to create
      ret = *mCSharpCreate( mTypeName.c_str() );
    }
    else
    {
      ret = mCreate();
    }

    if ( ret )
    {
      BaseObject& handle = ret.GetBaseObject();
      Object *object = dynamic_cast<Internal::Object*>(&handle);

      if ( object )
      {
        object->SetTypeInfo( this );
      }
    }
  }
  return ret;
}

  bool TypeInfo::DoActionTo(BaseObject *object, const std::string &actionName, const Property::Map &properties)
{
  bool done = false;

  ActionContainer::iterator iter = find_if(mActions.begin(), mActions.end(), PairFinder<std::string, ActionPair>(actionName));

  if( iter != mActions.end() )
  {
    done = (iter->second)(object, actionName, properties);
  }
  else
  {
    DALI_LOG_WARNING("Type '%s' cannot do action '%s'\n", mTypeName.c_str(), actionName.c_str());
  }

  if(!done)
  {
    Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
    while( base )
    {
      done = GetImplementation(base).DoActionTo(object, actionName, properties);
      if( done )
      {
        break;
      }
      base =  Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
    }
  }

  return done;
}

bool TypeInfo::ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( false );

  ConnectorContainer::iterator iter = find_if( mSignalConnectors.begin(), mSignalConnectors.end(),
                                                 PairFinder<std::string, ConnectionPair>(signalName) );

  if( iter != mSignalConnectors.end() )
  {
    connected = (iter->second)( object, connectionTracker, signalName, functor );
  }

  return connected;
}

const std::string& TypeInfo::GetName() const
{
  return mTypeName;
}

const std::string& TypeInfo::GetBaseName() const
{
  return mBaseTypeName;
}

Dali::TypeInfo::CreateFunction TypeInfo::GetCreator() const
{
  return mCreate;
}

size_t TypeInfo::GetActionCount() const
{
  size_t count = mActions.size();

  Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
  while( base )
  {
    count += GetImplementation(base).mActions.size();
    base = Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
  }

  return count;
}

std::string TypeInfo::GetActionName(size_t index) const
{
  std::string name;

  if( index < mActions.size() )
  {
    name = mActions[index].first;
  }
  else
  {
    size_t count = mActions.size();

    Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
    while( base )
    {
      size_t baseCount = GetImplementation(base).mActions.size();

      if( index < count + baseCount )
      {
        name = GetImplementation(base).mActions[ index - count ].first;
        break;
      }

      count += baseCount;

      base = Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
    }
  }

  return name;
}

size_t TypeInfo::GetSignalCount() const
{
  size_t count = mSignalConnectors.size();

  Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
  while( base )
  {
    count += GetImplementation(base).mSignalConnectors.size();
    base = Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
  }

  return count;
}

std::string TypeInfo::GetSignalName(size_t index) const
{
  std::string name;

  if( index < mSignalConnectors.size() )
  {
    name = mSignalConnectors[index].first;
  }
  else
  {
    size_t count = mSignalConnectors.size();

    Dali::TypeInfo base = Dali::TypeRegistry::Get().GetTypeInfo( mBaseTypeName );
    while( base )
    {
      size_t baseCount = GetImplementation(base).mSignalConnectors.size();

      if( index < count + baseCount )
      {
        name = GetImplementation(base).mSignalConnectors[ index - count ].first;
        break;
      }

      count += baseCount;

      base = Dali::TypeRegistry::Get().GetTypeInfo( base.GetBaseName() );
    }
  }

  return name;
}

void TypeInfo::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
  if ( base )
  {
    const TypeInfo& baseImpl( GetImplementation( base ) );
    baseImpl.GetPropertyIndices( indices );
  }

  AppendProperties( indices, mRegisteredProperties );
}

void TypeInfo::GetChildPropertyIndices( Property::IndexContainer& indices ) const
{
  Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
  if ( base )
  {
    const TypeInfo& baseImpl( GetImplementation( base ) );
    baseImpl.GetChildPropertyIndices( indices );
  }

  AppendProperties( indices, mRegisteredChildProperties );
}

/**
 * Append the indices in RegisteredProperties to the given index container.
 */
void TypeInfo::AppendProperties( Dali::Property::IndexContainer& indices,
                                 const TypeInfo::RegisteredPropertyContainer& registeredProperties ) const
{
  if ( ! registeredProperties.empty() )
  {
    indices.Reserve( indices.Size() + registeredProperties.size() );

    for( auto&& elem : registeredProperties )
    {
      indices.PushBack( elem.first );
    }
  }
}

const std::string& TypeInfo::GetPropertyName( Property::Index index ) const
{
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredProperties.end() )
  {
    return iter->second.name;
  }

  Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
  if ( base )
  {
    return GetImplementation(base).GetPropertyName( index );
  }

  DALI_ASSERT_ALWAYS( ! "Cannot find property index" ); // use the same assert as Object
}

void TypeInfo::AddActionFunction( const std::string &actionName, Dali::TypeInfo::ActionFunction function )
{
  if( NULL == function)
  {
    DALI_LOG_WARNING("Action function is empty\n");
  }
  else
  {
    ActionContainer::iterator iter = std::find_if(mActions.begin(), mActions.end(),
                                                  PairFinder<std::string, ActionPair>(actionName));

    if( iter == mActions.end() )
    {
      mActions.push_back( ActionPair( actionName, function ) );
    }
    else
    {
      DALI_LOG_WARNING("Action already exists in TypeRegistry Type\n", actionName.c_str());
    }
  }
}

void TypeInfo::AddConnectorFunction( const std::string& signalName, Dali::TypeInfo::SignalConnectorFunction function )
{
  if( NULL == function)
  {
    DALI_LOG_WARNING("Connector function is empty\n");
  }
  else
  {
    ConnectorContainer::iterator iter = find_if( mSignalConnectors.begin(), mSignalConnectors.end(),
                                                   PairFinder<std::string, ConnectionPair>(signalName) );

    if( iter == mSignalConnectors.end() )
    {
      mSignalConnectors.push_back( ConnectionPair( signalName, function ) );
    }
    else
    {
      DALI_LOG_WARNING("Signal name already exists in TypeRegistry Type for signal connector function\n", signalName.c_str());
    }
  }
}

void TypeInfo::AddProperty( const std::string& name, Property::Index index, Property::Type type, Dali::TypeInfo::SetPropertyFunction setFunc, Dali::TypeInfo::GetPropertyFunction getFunc )
{
  // The setter can be empty as a property can be read-only.

  if ( NULL == getFunc )
  {
    DALI_ASSERT_ALWAYS( ! "GetProperty Function is empty" );
  }
  else
  {
    RegisteredPropertyContainer::iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair>(index) );

    if ( iter == mRegisteredProperties.end() )
    {
      mRegisteredProperties.push_back( RegisteredPropertyPair( index, RegisteredProperty( type, setFunc, getFunc, name, Property::INVALID_INDEX, Property::INVALID_COMPONENT_INDEX ) ) );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Property index already added to Type" );
    }
  }
}

void TypeInfo::AddProperty( const std::string& name, Property::Index index, Property::Type type, Dali::CSharpTypeInfo::SetPropertyFunction setFunc, Dali::CSharpTypeInfo::GetPropertyFunction getFunc)
{

  // The setter can be empty as a property can be read-only.

  if ( NULL == getFunc )
  {
    DALI_ASSERT_ALWAYS( ! "GetProperty Function is empty" );
  }
  else
  {
    RegisteredPropertyContainer::iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair>(index) );

    if ( iter == mRegisteredProperties.end() )
    {
      mRegisteredProperties.push_back( RegisteredPropertyPair( index, RegisteredProperty( type, setFunc, getFunc, name, Property::INVALID_INDEX, Property::INVALID_COMPONENT_INDEX ) ) );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Property index already added to Type" );
    }
  }

}


void TypeInfo::AddAnimatableProperty( const std::string& name, Property::Index index, Property::Type type )
{
  RegisteredPropertyContainer::iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                        PairFinder< Property::Index, RegisteredPropertyPair>(index) );

  if ( iter == mRegisteredProperties.end() )
  {
    mRegisteredProperties.push_back( RegisteredPropertyPair( index, RegisteredProperty( type, name, Property::INVALID_INDEX, Property::INVALID_COMPONENT_INDEX ) ) );
  }
  else
  {
    DALI_ASSERT_ALWAYS( ! "Property index already added to Type" );
  }
}

void TypeInfo::AddAnimatableProperty( const std::string& name, Property::Index index, const Property::Value& defaultValue )
{
  RegisteredPropertyContainer::iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                        PairFinder< Property::Index, RegisteredPropertyPair>(index) );

  if ( iter == mRegisteredProperties.end() )
  {
    mRegisteredProperties.push_back( RegisteredPropertyPair( index, RegisteredProperty( defaultValue.GetType(), name, Property::INVALID_INDEX, Property::INVALID_COMPONENT_INDEX ) ) );
    mPropertyDefaultValues.push_back( PropertyDefaultValuePair( index, defaultValue ) );
  }
  else
  {
    DALI_ASSERT_ALWAYS( ! "Property index already added to Type" );
  }
}

void TypeInfo::AddAnimatablePropertyComponent( const std::string& name, Property::Index index, Property::Index baseIndex, unsigned int componentIndex )
{
  Property::Type type = GetPropertyType( baseIndex );
  DALI_ASSERT_ALWAYS( ( type == Property::VECTOR2 || type == Property::VECTOR3 || type == Property::VECTOR4 ) && "Base property does not support component" );

  bool success = false;

  RegisteredPropertyContainer::iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                        PairFinder< Property::Index, RegisteredPropertyPair>(index) );

  if ( iter == mRegisteredProperties.end() )
  {
    iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                    PropertyComponentFinder< RegisteredPropertyPair >( baseIndex, componentIndex ) );

    if ( iter == mRegisteredProperties.end() )
    {
      mRegisteredProperties.push_back( RegisteredPropertyPair( index, RegisteredProperty( type, name, baseIndex, componentIndex ) ) );
      success = true;
    }
  }

  DALI_ASSERT_ALWAYS( success && "Property component already registered" );
}

void TypeInfo::AddChildProperty( const std::string& name, Property::Index index, Property::Type type )
{
  RegisteredPropertyContainer::iterator iter = find_if( mRegisteredChildProperties.begin(), mRegisteredChildProperties.end(),
                                                        PairFinder< Property::Index, RegisteredPropertyPair>(index) );

  if ( iter == mRegisteredChildProperties.end() )
  {
    mRegisteredChildProperties.push_back( RegisteredPropertyPair( index, RegisteredProperty( type, name, Property::INVALID_INDEX, Property::INVALID_COMPONENT_INDEX ) ) );
  }
  else
  {
    DALI_ASSERT_ALWAYS( ! "Property index already added to Type" );
  }
}

size_t TypeInfo::GetPropertyCount() const
{
  size_t count( mRegisteredProperties.size() );

  Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
  while ( base )
  {
    const TypeInfo& baseImpl( GetImplementation(base) );
    count += baseImpl.mRegisteredProperties.size();
    base = TypeRegistry::Get()->GetTypeInfo( baseImpl.mBaseTypeName );
  }

  return count;
}

Property::Index TypeInfo::GetPropertyIndex( const std::string& name ) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Slow but should not be done that often
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PropertyNameFinder< RegisteredPropertyPair >( name ) );

  if ( iter != mRegisteredProperties.end() )
  {
    index = iter->first;
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      index = GetImplementation(base).GetPropertyIndex( name );
    }
  }

  return index;
}

Property::Index TypeInfo::GetBasePropertyIndex( Property::Index index ) const
{
  Property::Index basePropertyIndex = Property::INVALID_INDEX;

  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredProperties.end() )
  {
    basePropertyIndex = iter->second.basePropertyIndex;
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      basePropertyIndex = GetImplementation(base).GetBasePropertyIndex( index );
    }
  }

  return basePropertyIndex;
}

int TypeInfo::GetComponentIndex( Property::Index index ) const
{
  int componentIndex = Property::INVALID_COMPONENT_INDEX;

  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredProperties.end() )
  {
    componentIndex = iter->second.componentIndex;
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      componentIndex = GetImplementation(base).GetComponentIndex( index );
    }
  }

  return componentIndex;
}

Property::Index TypeInfo::GetChildPropertyIndex( const std::string& name ) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Slow but should not be done that often
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredChildProperties.begin(), mRegisteredChildProperties.end(),
                                                          PropertyNameFinder< RegisteredPropertyPair >( name ) );

  if ( iter != mRegisteredChildProperties.end() )
  {
    index = iter->first;
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      index = GetImplementation(base).GetChildPropertyIndex( name );
    }
  }

  return index;
}

const std::string& TypeInfo::GetChildPropertyName( Property::Index index ) const
{
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredChildProperties.begin(), mRegisteredChildProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredChildProperties.end() )
  {
    return iter->second.name;
  }

  Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
  if ( base )
  {
    return GetImplementation(base).GetChildPropertyName( index );
  }

  DALI_LOG_WARNING("Cannot find property index");

  static std::string emptyString;
  return emptyString;
}

Property::Type TypeInfo::GetChildPropertyType( Property::Index index ) const
{
  Property::Type type( Property::NONE );

  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredChildProperties.begin(), mRegisteredChildProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredChildProperties.end() )
  {
    type = iter->second.type;
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      type = GetImplementation(base).GetChildPropertyType( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" ); // use the same assert as Object
    }
  }

  return type;
}

bool TypeInfo::IsPropertyWritable( Property::Index index ) const
{
  bool writable( false );

  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredProperties.end() )
  {
    if( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
    {
      writable = true; // animatable property is writable
    }
    else
    {
      writable = iter->second.setFunc ? true : false;
    }
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      writable = GetImplementation(base).IsPropertyWritable( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" ); // use the same assert as Object
    }
  }

  return writable;
}

Property::Type TypeInfo::GetPropertyType( Property::Index index ) const
{
  Property::Type type( Property::NONE );

  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredProperties.end() )
  {
    if( iter->second.componentIndex == Property::INVALID_COMPONENT_INDEX )
    {
      type = iter->second.type;
    }
    else
    {
      // If component index is set, then we should return FLOAT
      type = Property::FLOAT;
    }
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      type = GetImplementation(base).GetPropertyType( index );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" ); // use the same assert as Object
    }
  }

  return type;
}

Property::Value TypeInfo::GetPropertyDefaultValue( Property::Index index ) const
{
  PropertyDefaultValueContainer::const_iterator iter = find_if( mPropertyDefaultValues.begin(), mPropertyDefaultValues.end(),
                                                    PairFinder< Property::Index, PropertyDefaultValuePair >( index ) );
  if( iter !=  mPropertyDefaultValues.end() )
  {
    return iter->second;
  }
  else
  {
    return Property::Value( GetPropertyType( index ) );
  }
}

void TypeInfo::SetProperty( BaseObject *object, Property::Index index, const Property::Value& value ) const
{
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                              PairFinder< Property::Index, RegisteredPropertyPair >( index ) );
  if ( iter != mRegisteredProperties.end() )
  {
    if( iter->second.setFunc )
    {
      if( mCSharpType )
      {
        // CSharp wants a property name not an index
        const std::string& name = (iter->second).name;

        iter->second.cSharpSetFunc( object,name.c_str(), const_cast< Property::Value* >(&value) );
      }
      else
      {
        iter->second.setFunc( object, index, value );
      }
    }
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      GetImplementation(base).SetProperty( object, index, value );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property index" ); // use the same assert as Object
    }
  }
}

void TypeInfo::SetProperty( BaseObject *object, const std::string& name, const Property::Value& value ) const
{
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                              PropertyNameFinder< RegisteredPropertyPair >( name ) );
  if ( iter != mRegisteredProperties.end() )
  {
    DALI_ASSERT_ALWAYS( iter->second.setFunc && "Trying to write to a read-only property" );

    if( mCSharpType )
    {
      // CSharp wants a property name not an index
      iter->second.cSharpSetFunc( object,name.c_str(), const_cast< Property::Value* >(&value ));
    }
    else
    {
      iter->second.setFunc( object, iter->first, value );
    }
  }
  else
  {
    Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
    if ( base )
    {
      GetImplementation(base).SetProperty( object, name, value );
    }
    else
    {
      DALI_ASSERT_ALWAYS( ! "Cannot find property name" );
    }
  }
}

Property::Value TypeInfo::GetProperty( const BaseObject *object, Property::Index index ) const
{
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );
  if( iter != mRegisteredProperties.end() )
  {
    if( mCSharpType ) // using csharp property get which returns a pointer to a Property::Value
    {
      // CSharp wants a property name not an index
      // CSharp callback can't return an object by value, it can only return a pointer
      // CSharp has ownership of the pointer contents, which is fine because we are returning by from this function by value
      const std::string& name = (iter->second).name;

      return *( iter->second.cSharpGetFunc( const_cast< BaseObject* >( object ), name.c_str()) );

    }
    else
    {
      // Need to remove the constness here as CustomActor will not be able to call Downcast with a const pointer to the object
      return iter->second.getFunc( const_cast< BaseObject* >( object ), index );
    }
  }

  Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
  if ( base )
  {
    return GetImplementation( base ).GetProperty( object, index );
  }

  DALI_ASSERT_ALWAYS( ! "Cannot find property index" ); // use the same assert as Object
}

Property::Value TypeInfo::GetProperty( const BaseObject *object, const std::string& name ) const
{
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                            PropertyNameFinder< RegisteredPropertyPair >( name ) );



  if( iter != mRegisteredProperties.end() )
  {
    if( mCSharpType ) // using csharp property get which returns a pointer to a Property::Value
    {
       // CSharp wants a property name not an index
       // CSharp callback can't return an object by value, it can only return a pointer
       // CSharp has ownership of the pointer contents, which is fine because we are returning by from this function by value
       return *( iter->second.cSharpGetFunc( const_cast< BaseObject* >( object ), name.c_str() ));

    }
    else
    {
      // Need to remove the constness here as CustomActor will not be able to call Downcast with a const pointer to the object
      return iter->second.getFunc( const_cast< BaseObject* >( object ), iter->first );
    }
  }

  Dali::TypeInfo base = TypeRegistry::Get()->GetTypeInfo( mBaseTypeName );
  if ( base )
  {
    return GetImplementation( base ).GetProperty( object, name );
  }

  DALI_ASSERT_ALWAYS( ! "Cannot find property name" );
}

} // namespace Internal

} // namespace Dali
