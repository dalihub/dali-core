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
#include <dali/internal/event/common/type-registry-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/internal/event/actors/custom-actor-internal.h>
#include <dali/internal/event/common/demangler.h>
#include <dali/internal/event/common/thread-local-storage.h>

#include <dali/integration-api/debug.h>

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TYPE_REGISTRY");
#endif

} // namespace anon

namespace Dali
{

namespace Internal
{

TypeRegistry *TypeRegistry::Get()
{
  static TypeRegistry *_reg(new TypeRegistry());
  DALI_ASSERT_DEBUG(_reg);
  return _reg;
}

TypeRegistry::TypeRegistry()
{

}

TypeRegistry::~TypeRegistry()
{
  mRegistryLut.clear();
}

TypeRegistry::TypeInfoPointer TypeRegistry::GetTypeInfo( const std::string& uniqueTypeName )
{
  for( auto&& iter : mRegistryLut )
  {
    // Note! mRegistryLut contains Dali::TypeInfo handles, so cannot call GetTypeName()
    // as it calls us back resulting in infinite loop (GetTypeName is in BaseHandle part)
    if( iter->GetName() == uniqueTypeName )
    {
      return iter;
    }
  }
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Cannot find requested type '%s'\n", uniqueTypeName.c_str() );

  return TypeRegistry::TypeInfoPointer();
}

TypeRegistry::TypeInfoPointer TypeRegistry::GetTypeInfo( const std::type_info& registerType )
{
  std::string typeName = DemangleClassName( registerType.name() );

  return GetTypeInfo( typeName );
}

uint32_t TypeRegistry::GetTypeNameCount() const
{
  return static_cast<uint32_t>( mRegistryLut.size() );
}

std::string TypeRegistry::GetTypeName( uint32_t index ) const
{
  std::string name;

  if( index < mRegistryLut.size() )
  {
    name = mRegistryLut[ index ]->GetName();
  }

  return name;
}

std::string TypeRegistry::Register( const std::type_info& theTypeInfo, const std::type_info& baseTypeInfo,
                             Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit )
{
  std::string uniqueTypeName  = DemangleClassName( theTypeInfo.name() );

  return Register( uniqueTypeName, baseTypeInfo, createInstance, callCreateOnInit );
}

std::string TypeRegistry::Register( const std::type_info& theTypeInfo, const std::type_info& baseTypeInfo,
                             Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit,
                             const Dali::PropertyDetails* defaultProperties, Property::Index defaultPropertyCount )
{
  std::string uniqueTypeName  = DemangleClassName( theTypeInfo.name() );

  return Register( uniqueTypeName, baseTypeInfo, createInstance, callCreateOnInit, defaultProperties, defaultPropertyCount );
}

std::string TypeRegistry::Register( const std::string& uniqueTypeName, const std::type_info& baseTypeInfo,
                             Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit,
                             const Dali::PropertyDetails* defaultProperties, Property::Index defaultPropertyCount )
{
  std::string baseTypeName = DemangleClassName( baseTypeInfo.name() );

  // check for duplicates using uniqueTypeName
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == uniqueTypeName )
    {
      DALI_LOG_WARNING( "Duplicate name in TypeRegistry for '%s'\n", + uniqueTypeName.c_str() );
      DALI_ASSERT_ALWAYS( !"Duplicate type name in Type Registration" );
      return uniqueTypeName; // never actually happening due to the assert
    }
  }

  mRegistryLut.push_back( TypeRegistry::TypeInfoPointer( new Internal::TypeInfo( uniqueTypeName, baseTypeName, createInstance, defaultProperties, defaultPropertyCount ) ) );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Type Registration %s(%s)\n", uniqueTypeName.c_str(), baseTypeName.c_str() );

  if( callCreateOnInit )
  {
    mInitFunctions.push_back(createInstance);
  }

  return uniqueTypeName;
}

void TypeRegistry::Register( const std::string& uniqueTypeName, const std::type_info& baseTypeInfo,
    Dali::CSharpTypeInfo::CreateFunction createInstance )
{
  std::string baseTypeName = DemangleClassName( baseTypeInfo.name() );

  // check for duplicates using uniqueTypeName
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == uniqueTypeName )
    {
      DALI_LOG_WARNING( "Duplicate name in TypeRegistry for '%s'\n", + uniqueTypeName.c_str() );
      DALI_ASSERT_ALWAYS( !"Duplicate type name in Type Registration" );
      return; // never actually happening due to the assert
    }
  }

  mRegistryLut.push_back( TypeRegistry::TypeInfoPointer( new Internal::TypeInfo( uniqueTypeName, baseTypeName, createInstance ) ) );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Type Registration %s(%s)\n", uniqueTypeName.c_str(), baseTypeName.c_str() );
}

void TypeRegistry::CallInitFunctions(void) const
{
  for( auto&& iter : mInitFunctions )
  {
    (*iter)();
  }
}

std::string TypeRegistry::RegistrationName( const std::type_info& registerType )
{
  return DemangleClassName( registerType.name() );
}

void TypeRegistry::RegisterSignal( TypeRegistration& typeRegistration, const std::string& name, Dali::TypeInfo::SignalConnectorFunction func )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == typeRegistration.RegisteredName() )
    {
      iter->AddConnectorFunction( name, func );
      break;
    }
  }
}

bool TypeRegistry::RegisterAction( TypeRegistration& typeRegistration, const std::string &name, Dali::TypeInfo::ActionFunction f )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == typeRegistration.RegisteredName() )
    {
      iter->AddActionFunction( name, f );
      return true;
    }
  }
  return false;
}

bool TypeRegistry::RegisterProperty( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, Property::Type type, Dali::TypeInfo::SetPropertyFunction setFunc, Dali::TypeInfo::GetPropertyFunction getFunc )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == typeRegistration.RegisteredName() )
    {
      iter->AddProperty( name, index, type, setFunc, getFunc );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterProperty( const std::string& objectName, const std::string& name, Property::Index index, Property::Type type, Dali::CSharpTypeInfo::SetPropertyFunction setFunc, Dali::CSharpTypeInfo::GetPropertyFunction getFunc )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == objectName )
    {
      iter->AddProperty( name, index, type, setFunc, getFunc );
      return true;
    }
  }

  return false;
}


bool TypeRegistry::RegisterAnimatableProperty( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, Property::Type type )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == typeRegistration.RegisteredName() )
    {
      iter->AddAnimatableProperty( name, index, type );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterAnimatableProperty( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, const Property::Value& value )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == typeRegistration.RegisteredName() )
    {
      iter->AddAnimatableProperty( name, index, value );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterAnimatablePropertyComponent( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, Property::Index baseIndex, unsigned int componentIndex )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == typeRegistration.RegisteredName() )
    {
      iter->AddAnimatablePropertyComponent( name, index, baseIndex, componentIndex );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterChildProperty( const std::string& registeredType, const std::string& name, Property::Index index, Property::Type type )
{
  for( auto&& iter : mRegistryLut )
  {
    if( iter->GetName() == registeredType )
    {
      iter->AddChildProperty( name, index, type );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterChildProperty( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, Property::Type type )
{
  return RegisterChildProperty( typeRegistration.RegisteredName(), name, index, type );
}

bool TypeRegistry::DoActionTo( BaseObject * const object, const std::string& actionName, const Property::Map& properties )
{
  bool done = false;

  auto&& type = GetTypeInfo( object );

  // DoActionTo recurses through base classes
  done = type->DoActionTo( object, actionName, properties );

  if( !done )
  {
    DALI_LOG_WARNING("Type '%s' cannot do action '%s'\n", type->GetName().c_str(), actionName.c_str());
  }

  return done;
}

bool TypeRegistry::ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( false );

  auto&& type = GetTypeInfo( object );

  // Connect iterates through base classes
  connected = type->ConnectSignal( object, connectionTracker, signalName, functor );

  if( !connected )
  {
    DALI_LOG_WARNING("Type '%s' signal '%s' connection failed \n", type->GetName().c_str(), signalName.c_str());
    // Ownership of functor was not passed to Dali::CallbackBase, so clean-up now
    delete functor;
  }

  return connected;
}

TypeRegistry::TypeInfoPointer TypeRegistry::GetTypeInfo(const Dali::BaseObject * const pBaseObject)
{
  TypeInfoPointer type;

  // test for custom actor which has another indirection to get to the type hiearchy we're after
  const Dali::Internal::CustomActor * const pCustom = dynamic_cast<const Dali::Internal::CustomActor*>(pBaseObject);

  if( pCustom )
  {
    const Dali::CustomActorImpl& custom = pCustom->GetImplementation();
    type = GetTypeInfo( typeid( custom ) );
    if( !type )
    {
      // the most derived type is a descendant of custom actor but has not registered itself
      // so we'll just treat it as a custom actor for now so it "inherits" all of actors properties, actions and signals
      type = GetTypeInfo( typeid( Dali::Internal::CustomActor ) );
    }
  }
  else
  {
    type = GetTypeInfo( typeid( *pBaseObject ) );
  }

  return type;
}

} // namespace Internal

} // namespace Dali
