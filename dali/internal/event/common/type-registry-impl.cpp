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
#include <dali/internal/event/common/type-registry-impl.h>

// EXTERNAL INCLUDES
#include <string.h>
#include <sstream>

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

Dali::TypeInfo TypeRegistry::GetTypeInfo( const std::string &uniqueTypeName )
{
  Dali::TypeInfo ret;

  RegistryMap::iterator iter = mRegistryLut.find(uniqueTypeName);

  if( iter != mRegistryLut.end() )
  {
    ret = iter->second;
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Cannot find requested type '%s'\n", uniqueTypeName.c_str());
  }

  return ret;
}

Dali::TypeInfo TypeRegistry::GetTypeInfo( const std::type_info& registerType )
{
  Dali::TypeInfo ret;

  std::string typeName = DemangleClassName(registerType.name());

  RegistryMap::iterator iter = mRegistryLut.find(typeName);

  if( iter != mRegistryLut.end() )
  {
    ret = iter->second;
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Cannot find requested type '%s'\n", registerType.name());
  }

  return ret;
}

Dali::TypeRegistry::NameContainer TypeRegistry::GetTypeNames() const
{
  Dali::TypeRegistry::NameContainer ret;

  for(RegistryMap::const_iterator iter = mRegistryLut.begin(); iter != mRegistryLut.end(); ++iter)
  {
    ret.push_back(iter->first);
  }

  return ret;
}

bool TypeRegistry::Register( const std::type_info& theTypeInfo, const std::type_info& baseTypeInfo,
                             Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit )
{
  std::string uniqueTypeName  = DemangleClassName(theTypeInfo.name());

  return Register( uniqueTypeName, baseTypeInfo, createInstance, callCreateOnInit );
}

bool TypeRegistry::Register( const std::string& uniqueTypeName, const std::type_info& baseTypeInfo,
                             Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit )
{
  bool ret = false;

  std::string baseTypeName    = DemangleClassName(baseTypeInfo.name());

  RegistryMap::iterator iter = mRegistryLut.find(uniqueTypeName);

  if( iter == mRegistryLut.end() )
  {
    mRegistryLut[uniqueTypeName] = Dali::TypeInfo(new Internal::TypeInfo(uniqueTypeName, baseTypeName, createInstance));
    ret = true;
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "Type Registration %s(%s)\n", uniqueTypeName.c_str(), baseTypeName.c_str());
  }
  else
  {
    DALI_LOG_WARNING("Duplicate name for TypeRegistry for '%s'\n", + uniqueTypeName.c_str());
    DALI_ASSERT_ALWAYS(!"Duplicate type name for Type Registation");
  }

  if( callCreateOnInit )
  {
    mInitFunctions.push_back(createInstance);
  }

  return ret;
}

void TypeRegistry::CallInitFunctions(void) const
{
  for( InitFunctions::const_iterator iter = mInitFunctions.begin(); iter != mInitFunctions.end(); ++iter)
  {
    (*iter)();
  }
}

std::string TypeRegistry::RegistrationName( const std::type_info& registerType )
{
  return DemangleClassName( registerType.name() );
}

void TypeRegistry::RegisterSignal( TypeRegistration& typeRegistration, const std::string& name, Dali::TypeInfo::SignalConnectorFunctionV2 func )
{
  RegistryMap::iterator iter = mRegistryLut.find( typeRegistration.RegisteredName() );

  if( iter != mRegistryLut.end() )
  {
    DALI_ASSERT_DEBUG(iter->second);

    GetImplementation(iter->second).AddConnectorFunction( name, func );
  }
}

bool TypeRegistry::RegisterAction( TypeRegistration &registered, const std::string &name, Dali::TypeInfo::ActionFunction f)
{
  RegistryMap::iterator iter = mRegistryLut.find( registered.RegisteredName() );

  if( iter != mRegistryLut.end() )
  {
    DALI_ASSERT_DEBUG(iter->second);

    GetImplementation(iter->second).AddActionFunction( name, f );

    return true;
  }
  else
  {
    return false;
  }
}

bool TypeRegistry::RegisterProperty( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type, Dali::TypeInfo::SetPropertyFunction setFunc, Dali::TypeInfo::GetPropertyFunction getFunc )
{
  RegistryMap::iterator iter = mRegistryLut.find( registered.RegisteredName() );

  if( iter != mRegistryLut.end() )
  {
    DALI_ASSERT_DEBUG(iter->second);

    GetImplementation(iter->second).AddProperty( name, index, type, setFunc, getFunc );

    return true;
  }

  return false;
}

bool TypeRegistry::DoActionTo( BaseObject * const object, const std::string &actionName, const std::vector<Property::Value> &properties)
{
  bool done = false;

  Dali::TypeInfo type = GetTypeInfo( object );

  while( type )
  {
    if(GetImplementation(type).DoActionTo(object, actionName, properties))
    {
      done = true;
      break;
    }
    type = GetTypeInfo( type.GetBaseName() );
  }

  return done;
}

bool TypeRegistry::ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( false );

  Dali::TypeInfo type = GetTypeInfo( object );

  while( type )
  {
    connected = GetImplementation(type).ConnectSignal( object, connectionTracker, signalName, functor );
    if( connected )
    {
      break;
    }
    type = GetTypeInfo( type.GetBaseName() );
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
    type = GetTypeInfo( typeid( pCustom->GetImplementation() ) );
  }
  else
  {
    type = GetTypeInfo( typeid( *pBaseObject ) );
  }

  return type;
}

} // namespace Internal

} // namespace Dali
