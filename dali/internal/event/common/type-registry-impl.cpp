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
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/internal/event/actors/custom-actor-internal.h>
#include <dali/internal/event/common/demangler.h>

#include <dali/integration-api/debug.h>

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TYPE_REGISTRY");
#endif

} // namespace anon

namespace Dali
{

extern std::string Demangle(const char* symbol);

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

Dali::TypeInfo TypeRegistry::GetTypeInfo( const std::string& uniqueTypeName )
{
  for( auto&& iter : mRegistryLut )
  {
    // Note! mRegistryLut contains Dali::TypeInfo handles, so cannot call GetTypeName()
    // as it calls us back resulting in infinite loop (GetTypeName is in BaseHandle part)
    if( GetImplementation( iter ).GetName() == uniqueTypeName )
    {
      return iter;
    }
  }
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Cannot find requested type '%s'\n", uniqueTypeName.c_str() );

  return Dali::TypeInfo();
}

Dali::TypeInfo TypeRegistry::GetTypeInfo( const std::type_info& registerType )
{
  std::string typeName = DemangleClassName( registerType.name() );

  return GetTypeInfo( typeName );
}

size_t TypeRegistry::GetTypeNameCount() const
{
  return mRegistryLut.size();
}


std::string TypeRegistry::GetTypeName( size_t index ) const
{
  std::string name;

  if( index < mRegistryLut.size() )
  {
    name = GetImplementation( mRegistryLut[ index ] ).GetName();
  }

  return name;
}


bool TypeRegistry::Register( const std::type_info& theTypeInfo, const std::type_info& baseTypeInfo,
                             Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit )
{
  std::string uniqueTypeName  = DemangleClassName( theTypeInfo.name() );

  return Register( uniqueTypeName, baseTypeInfo, createInstance, callCreateOnInit );
}

bool TypeRegistry::Register( const std::string& uniqueTypeName, const std::type_info& baseTypeInfo,
                             Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit )
{
  std::string baseTypeName = DemangleClassName( baseTypeInfo.name() );

  // check for duplicates using uniqueTypeName
  for( auto&& iter : mRegistryLut )
  {
    if( GetImplementation( iter ).GetName() == uniqueTypeName )
    {
      DALI_LOG_WARNING( "Duplicate name in TypeRegistry for '%s'\n", + uniqueTypeName.c_str() );
      DALI_ASSERT_ALWAYS( !"Duplicate type name in Type Registration" );
      return false;
    }
  }

  mRegistryLut.push_back( Dali::TypeInfo( new Internal::TypeInfo( uniqueTypeName, baseTypeName, createInstance ) ) );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Type Registration %s(%s)\n", uniqueTypeName.c_str(), baseTypeName.c_str() );

  if( callCreateOnInit )
  {
    mInitFunctions.push_back(createInstance);
  }

  return true;
}

bool TypeRegistry::Register( const std::string& uniqueTypeName, const std::type_info& baseTypeInfo,
    Dali::CSharpTypeInfo::CreateFunction createInstance )
{
  std::string baseTypeName = DemangleClassName( baseTypeInfo.name() );

  // check for duplicates using uniqueTypeName
  for( auto&& iter : mRegistryLut )
  {
    if( GetImplementation( iter ).GetName() == uniqueTypeName )
    {
      DALI_LOG_WARNING( "Duplicate name in TypeRegistry for '%s'\n", + uniqueTypeName.c_str() );
      DALI_ASSERT_ALWAYS( !"Duplicate type name in Type Registration" );
      return false;
    }
  }

  mRegistryLut.push_back( Dali::TypeInfo( new Internal::TypeInfo( uniqueTypeName, baseTypeName, createInstance ) ) );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "Type Registration %s(%s)\n", uniqueTypeName.c_str(), baseTypeName.c_str() );

  return true;
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
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == typeRegistration.RegisteredName() )
    {
      impl.AddConnectorFunction( name, func );
      break;
    }
  }
}

bool TypeRegistry::RegisterAction( TypeRegistration& typeRegistration, const std::string &name, Dali::TypeInfo::ActionFunction f )
{
  for( auto&& iter : mRegistryLut )
  {
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == typeRegistration.RegisteredName() )
    {
      impl.AddActionFunction( name, f );
      return true;
    }
  }
  return false;
}

bool TypeRegistry::RegisterProperty( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, Property::Type type, Dali::TypeInfo::SetPropertyFunction setFunc, Dali::TypeInfo::GetPropertyFunction getFunc )
{
  for( auto&& iter : mRegistryLut )
  {
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == typeRegistration.RegisteredName() )
    {
      impl.AddProperty( name, index, type, setFunc, getFunc );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterProperty( const std::string& objectName, const std::string& name, Property::Index index, Property::Type type, Dali::CSharpTypeInfo::SetPropertyFunction setFunc, Dali::CSharpTypeInfo::GetPropertyFunction getFunc )
{
  for( auto&& iter : mRegistryLut )
  {
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == objectName )
    {
      impl.AddProperty( name, index, type, setFunc, getFunc );
      return true;
    }
  }

  return false;
}


bool TypeRegistry::RegisterAnimatableProperty( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, Property::Type type )
{
  for( auto&& iter : mRegistryLut )
  {
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == typeRegistration.RegisteredName() )
    {
      impl.AddAnimatableProperty( name, index, type );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterAnimatableProperty( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, const Property::Value& value )
{
  for( auto&& iter : mRegistryLut )
  {
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == typeRegistration.RegisteredName() )
    {
      impl.AddAnimatableProperty( name, index, value );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterAnimatablePropertyComponent( TypeRegistration& typeRegistration, const std::string& name, Property::Index index, Property::Index baseIndex, unsigned int componentIndex )
{
  for( auto&& iter : mRegistryLut )
  {
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == typeRegistration.RegisteredName() )
    {
      impl.AddAnimatablePropertyComponent( name, index, baseIndex, componentIndex );
      return true;
    }
  }

  return false;
}

bool TypeRegistry::RegisterChildProperty( const std::string& registeredType, const std::string& name, Property::Index index, Property::Type type )
{
  for( auto&& iter : mRegistryLut )
  {
    auto&& impl = GetImplementation( iter );
    if( impl.GetName() == registeredType )
    {
      impl.AddChildProperty( name, index, type );
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

  Dali::TypeInfo type = GetTypeInfo( object );

  while( type )
  {
    auto&& impl = GetImplementation( type );
    if( impl.DoActionTo( object, actionName, properties ) )
    {
      done = true;
      break;
    }
    type = GetTypeInfo( impl.GetBaseName() );
  }

  return done;
}

bool TypeRegistry::ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( false );

  Dali::TypeInfo type = GetTypeInfo( object );

  while( type )
  {
    auto&& impl = GetImplementation( type );
    connected = impl.ConnectSignal( object, connectionTracker, signalName, functor );
    if( connected )
    {
      break;
    }
    type = GetTypeInfo( impl.GetBaseName() );
  }

  if( !connected )
  {
    // Ownership of functor was not passed to Dali::CallbackBase, so clean-up now
    delete functor;
  }

  return connected;
}

Dali::TypeInfo TypeRegistry::GetTypeInfo(const Dali::BaseObject * const pBaseObject)
{
  Dali::TypeInfo type;

  // test for custom actor which has another indirection to get to the type hiearchy we're after
  const Dali::Internal::CustomActor * const pCustom = dynamic_cast<const Dali::Internal::CustomActor*>(pBaseObject);

  if(pCustom)
  {
    const Dali::CustomActorImpl& custom = pCustom->GetImplementation();
    type = GetTypeInfo( typeid( custom ) );
  }
  else
  {
    type = GetTypeInfo( typeid( *pBaseObject ) );
  }

  return type;
}

} // namespace Internal

} // namespace Dali
