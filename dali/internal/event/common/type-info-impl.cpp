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

namespace Dali
{

namespace Internal
{

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
  PropertyComponentFinder( Property::Index basePropertyIndex, const int find )
  : mBasePropertyIndex( basePropertyIndex ),
    mFind( find )
  {
  }

  bool operator()(const T &p) const
  {
    return ( p.second.basePropertyIndex == mBasePropertyIndex && p.second.componentIndex == mFind );
  }

private:

  Property::Index mBasePropertyIndex;
  const int mFind;
};

/**
 * Helper function to find the right default property with given index and return the desired detail of it
 */
template< typename Parameter, typename Member >
inline bool GetDefaultPropertyField( const Dali::PropertyDetails* propertyTable, Property::Index count, Property::Index index, Member member, Parameter& parameter )
{
  bool found = false;
  // is index inside this table (bigger than first index but smaller than first + count)
  if( ( index >= propertyTable->enumIndex ) && ( index < ( propertyTable->enumIndex + count ) ) )
  {
    // return the match. we're assuming here that there is no gaps between the indices in a table
    parameter = propertyTable[ index - propertyTable->enumIndex ].*member;
    found = true;
  }
  // should never really get here
  return found;
}

// static pointer value to mark that a base class address has not been resolved
// 0x01 is not a valid pointer but used here to differentiate from nullptr
// unfortunately it cannot be constexpr as C++ does not allow them to be initialised with reinterpret_cast
Internal::TypeInfo* const UNRESOLVED = reinterpret_cast<Internal::TypeInfo*>( 0x1 );

/**
 * Helper function to resolve and return the pointer to the base type info
 * Not a member function to avoid having to #include additional headers and to make sure this gets inlined inside this cpp
 * @param[in/out] baseType pointer to resolve and set
 * @param[in] typeRegistry reference to the type registry
 * @param[in] baseTypeName string name of the base type
 * @return true is base type exists
 */
inline bool GetBaseType( Internal::TypeInfo*& baseType, TypeRegistry& typeRegistry, const std::string& baseTypeName )
{
  // if greater than unresolved means we have a base type, null means no base
  bool baseExists = ( baseType > UNRESOLVED );
  // base only needs to be resolved once
  if( UNRESOLVED == baseType )
  {
    TypeRegistry::TypeInfoPointer base = typeRegistry.GetTypeInfo( baseTypeName );
    if( base )
    {
      baseType = base.Get(); // dont pass ownership, just return raw pointer
      baseExists = true;
    }
    else
    {
      // no type info found so assuming no base as all type registration is done in startup for now
      baseType = nullptr;
    }
  }
  return baseExists;
}

} // unnamed namespace

TypeInfo::TypeInfo( const std::string &name, const std::string &baseTypeName, Dali::TypeInfo::CreateFunction creator,
                    const Dali::PropertyDetails* defaultProperties, Property::Index defaultPropertyCount )
: mTypeRegistry( *TypeRegistry::Get() ), mBaseType( UNRESOLVED ),
  mTypeName( name ), mBaseTypeName( baseTypeName ), mCreate( creator ), mDefaultProperties( defaultProperties ),
  mDefaultPropertyCount( defaultPropertyCount ), mCSharpType( false )
{
  DALI_ASSERT_ALWAYS(!name.empty() && "Type info construction must have a name");
  DALI_ASSERT_ALWAYS(!baseTypeName.empty() && "Type info construction must have a base type name");
}

TypeInfo::TypeInfo(const std::string &name, const std::string &baseTypeName, Dali::CSharpTypeInfo::CreateFunction creator)
: mTypeRegistry( *TypeRegistry::Get() ), mBaseType( UNRESOLVED ),
  mTypeName( name ), mBaseTypeName( baseTypeName ), mCSharpCreate( creator ), mCSharpType( true )
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

  if( !done )
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      done = mBaseType->DoActionTo( object, actionName, properties );
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

  if( !connected )
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      connected = mBaseType->ConnectSignal( object, connectionTracker, signalName, functor );
    }
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

uint32_t TypeInfo::GetActionCount() const
{
  uint32_t count = static_cast<uint32_t>( mActions.size() );

  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    count += mBaseType->GetActionCount();
  }

  return count;
}

std::string TypeInfo::GetActionName( uint32_t index ) const
{
  std::string name;
  const uint32_t count = static_cast<uint32_t>( mActions.size() );

  if( index < count )
  {
    name = mActions[index].first;
  }
  else
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      return mBaseType->GetActionName( index - count );
    }
  }

  return name;
}

uint32_t TypeInfo::GetSignalCount() const
{
  uint32_t count = static_cast<uint32_t>( mSignalConnectors.size() );

  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    count += mBaseType->GetSignalCount();
  }

  return count;
}

std::string TypeInfo::GetSignalName( uint32_t index ) const
{
  std::string name;
  const uint32_t count = static_cast<uint32_t>( mSignalConnectors.size() );

  if( index < count )
  {
    name = mSignalConnectors[index].first;
  }
  else
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      return mBaseType->GetSignalName( index - count );
    }
  }

  return name;
}

void TypeInfo::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  // Default Properties
  if( mDefaultProperties )
  {
    indices.Reserve( indices.Size() + mDefaultPropertyCount );
    for( Property::Index index = 0; index < mDefaultPropertyCount; ++index )
    {
      indices.PushBack( mDefaultProperties[ index ].enumIndex );
    }
  }

  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    mBaseType->GetPropertyIndices( indices );
  }

  AppendProperties( indices, mRegisteredProperties );
}

void TypeInfo::GetChildPropertyIndices( Property::IndexContainer& indices ) const
{
  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    mBaseType->GetChildPropertyIndices( indices );
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

const std::string& TypeInfo::GetRegisteredPropertyName( Property::Index index ) const
{
  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );
  if ( iter != mRegisteredProperties.end() )
  {
    return iter->second.name;
  }
  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    return mBaseType->GetRegisteredPropertyName( index );
  }
  static std::string empty;
  return empty;
}

std::string TypeInfo::GetPropertyName( Property::Index index ) const
{
  std::string propertyName;
  // default or custom
  if ( mDefaultProperties && ( index < DEFAULT_PROPERTY_MAX_COUNT ) )
  {
    const char* name = nullptr;
    if( GetDefaultPropertyField( mDefaultProperties, mDefaultPropertyCount,index, &Dali::PropertyDetails::name, name ) )
    {
      propertyName = name;
    }
  }
  else
  {
    RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                            PairFinder< Property::Index, RegisteredPropertyPair >( index ) );
    if ( iter != mRegisteredProperties.end() )
    {
      return iter->second.name;
    }
  }
  // if not our property, go to parent
  if( propertyName.empty() )
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      return mBaseType->GetPropertyName( index );
    }
  }

  return propertyName;
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

void TypeInfo::AddAnimatablePropertyComponent( const std::string& name, Property::Index index, Property::Index baseIndex, uint32_t componentIndex )
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

uint32_t TypeInfo::GetPropertyCount() const
{
  uint32_t count = mDefaultPropertyCount + static_cast<uint32_t>( mRegisteredProperties.size() );

  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    count += mBaseType->GetPropertyCount();
  }

  return count;
}

Property::Index TypeInfo::GetPropertyIndex( const std::string& name ) const
{
  Property::Index index = Property::INVALID_INDEX;
  bool found = false;

  // check default properties
  if( mDefaultProperties )
  {
    for( Property::Index tableIndex = 0; tableIndex < mDefaultPropertyCount; ++tableIndex )
    {
      if( 0 == name.compare( mDefaultProperties[ tableIndex ].name ) )
      {
        index = mDefaultProperties[ tableIndex ].enumIndex;
        found = true;
        break;
      }
    }
  }
  if( !found )
  {
    // Slow but should not be done that often
    RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                            PropertyNameFinder< RegisteredPropertyPair >( name ) );
    if ( iter != mRegisteredProperties.end() )
    {
      index = iter->first;
    }
    else if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      index = mBaseType->GetPropertyIndex( name );
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
  else if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    basePropertyIndex = mBaseType->GetBasePropertyIndex( index );
  }

  return basePropertyIndex;
}

int32_t TypeInfo::GetComponentIndex( Property::Index index ) const
{
  int componentIndex = Property::INVALID_COMPONENT_INDEX;

  RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                          PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

  if ( iter != mRegisteredProperties.end() )
  {
    componentIndex = iter->second.componentIndex;
  }
  else if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    componentIndex = mBaseType->GetComponentIndex( index );
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
  else if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    index = mBaseType->GetChildPropertyIndex( name );
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

  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    return mBaseType->GetChildPropertyName( index );
  }

  DALI_LOG_ERROR( "Property index %d not found\n", index );

  static std::string empty;
  return empty;
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
  else if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    type = mBaseType->GetChildPropertyType( index );
  }
  else
  {
    DALI_LOG_ERROR( "Property index %d not found\n", index );
  }

  return type;
}

bool TypeInfo::IsPropertyWritable( Property::Index index ) const
{
  bool writable = false;
  bool found = false;

  // default property?
  if ( ( index < DEFAULT_PROPERTY_MAX_COUNT ) && mDefaultProperties )
  {
    found = GetDefaultPropertyField( mDefaultProperties, mDefaultPropertyCount,index, &Dali::PropertyDetails::writable, writable );
  }
  else if( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    writable = true; // animatable property is writable
    found = true;
  }
  else
  {
    RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                            PairFinder< Property::Index, RegisteredPropertyPair >( index ) );
    if ( iter != mRegisteredProperties.end() )
    {
      writable = iter->second.setFunc ? true : false;
      found = true;
    }
  }

  // if not found, continue to base
  if( !found )
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      writable = mBaseType->IsPropertyWritable( index );
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }

  return writable;
}

bool TypeInfo::IsPropertyAnimatable( Property::Index index ) const
{
  bool animatable = false;
  bool found = false;

  // default property?
  if ( ( index < DEFAULT_PROPERTY_MAX_COUNT ) && mDefaultProperties )
  {
    found = GetDefaultPropertyField( mDefaultProperties, mDefaultPropertyCount,index, &Dali::PropertyDetails::animatable, animatable );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry event-thread only properties are not animatable.
    animatable = false;
    found = true;
  }
  else if( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    animatable = true;
    found = true;
  }

  // if not found, continue to base
  if( !found )
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      animatable = mBaseType->IsPropertyAnimatable( index );
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }

  return animatable;
}

bool TypeInfo::IsPropertyAConstraintInput( Property::Index index ) const
{
  bool constraintInput = false;
  bool found = false;

  // default property?
  if ( ( index < DEFAULT_PROPERTY_MAX_COUNT ) && mDefaultProperties )
  {
    found = GetDefaultPropertyField( mDefaultProperties, mDefaultPropertyCount,index, &Dali::PropertyDetails::constraintInput, constraintInput );
  }
  else if ( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    // Type Registry event-thread only properties cannot be used as constraint input
    constraintInput = false;
    found = true;
  }
  else if( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) )
  {
    constraintInput = true;
    found = true;
  }

  // if not found, continue to base
  if( !found )
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      constraintInput = mBaseType->IsPropertyAConstraintInput( index );
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }

  return constraintInput;
}


Property::Type TypeInfo::GetPropertyType( Property::Index index ) const
{
  Property::Type type( Property::NONE );
  bool found = false;

  // default property?
  if ( ( index < DEFAULT_PROPERTY_MAX_COUNT ) && mDefaultProperties )
  {
    found = GetDefaultPropertyField( mDefaultProperties, mDefaultPropertyCount,index, &Dali::PropertyDetails::type, type );
  }
  else
  {
    RegisteredPropertyContainer::const_iterator iter = find_if( mRegisteredProperties.begin(), mRegisteredProperties.end(),
                                                            PairFinder< Property::Index, RegisteredPropertyPair >( index ) );

    if ( iter != mRegisteredProperties.end() )
    {
      if( iter->second.componentIndex == Property::INVALID_COMPONENT_INDEX )
      {
        type = iter->second.type;
        found = true;
      }
      else
      {
        // If component index is set, then we should return FLOAT
        type = Property::FLOAT;
        found = true;
      }
    }
  }

  if( !found )
  {
    if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
    {
      // call base type recursively
      type = mBaseType->GetPropertyType( index );
    }
    else
    {
      DALI_LOG_ERROR( "Property index %d not found\n", index );
    }
  }

  return type;
}

Property::Value TypeInfo::GetPropertyDefaultValue( Property::Index index ) const
{
  PropertyDefaultValueContainer::const_iterator iter = find_if( mPropertyDefaultValues.begin(), mPropertyDefaultValues.end(),
                                                    PairFinder< Property::Index, PropertyDefaultValuePair >( index ) );
  if( iter != mPropertyDefaultValues.end() )
  {
    return iter->second;
  }
  // we didn't have a value so ask base
  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    return mBaseType->GetPropertyDefaultValue( index );
  }
  return Property::Value(); // return none
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
  else if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    mBaseType->SetProperty( object, index, value );
  }
  else
  {
    DALI_LOG_ERROR( "Property index %d not found\n", index );
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
  else if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    mBaseType->SetProperty( object, name, value );
  }
  else
  {
    DALI_LOG_ERROR( "Property %s not found", name.c_str() );
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

  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    return mBaseType->GetProperty( object, index );
  }

  DALI_LOG_ERROR( "Property index %d not found\n", index );
  return Property::Value();
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

  if( GetBaseType( mBaseType, mTypeRegistry, mBaseTypeName ) )
  {
    // call base type recursively
    return mBaseType->GetProperty( object, name );
  }

  DALI_LOG_ERROR( "Property %s not found", name.c_str() );
  return Property::Value();
}

} // namespace Internal

} // namespace Dali
