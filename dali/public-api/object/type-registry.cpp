/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/type-registry.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/internal/event/common/type-registry-impl.h>

namespace Dali
{

TypeRegistry::TypeRegistry()
{
}

TypeRegistry::~TypeRegistry()
{
}

TypeRegistry::TypeRegistry(const TypeRegistry& copy)
: BaseHandle(copy)
{
}

TypeRegistry& TypeRegistry::operator=(const TypeRegistry& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

TypeRegistry TypeRegistry::Get()
{
  return TypeRegistry(Internal::TypeRegistry::Get());
}

Dali::TypeInfo TypeRegistry::GetTypeInfo( const std::string &uniqueTypeName )
{
  return GetImplementation(*this).GetTypeInfo( uniqueTypeName );
}

Dali::TypeInfo TypeRegistry::GetTypeInfo( const std::type_info& registerType )
{
  return GetImplementation(*this).GetTypeInfo( registerType );
}

TypeRegistry::NameContainer TypeRegistry::GetTypeNames() const
{
  return GetImplementation(*this).GetTypeNames();
}

TypeRegistry::TypeRegistry(Internal::TypeRegistry* internal)
: BaseHandle(internal)
{
}

TypeRegistration::TypeRegistration( const std::type_info& registerType, const std::type_info& baseType,
                                    TypeInfo::CreateFunction f )
  : mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry *impl = Internal::TypeRegistry::Get();

  if( impl->Register( registerType, baseType, f, false ) )
  {
    mName = impl->RegistrationName( registerType );
  }
}

TypeRegistration::TypeRegistration( const std::type_info& registerType, const std::type_info& baseType,
                                    TypeInfo::CreateFunction f, bool callCreateOnInit )
  : mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry *impl = Internal::TypeRegistry::Get();

  if( impl->Register( registerType, baseType, f, callCreateOnInit ) )
  {
    mName = impl->RegistrationName( registerType );
  }
}

TypeRegistration::TypeRegistration( const std::string& name, const std::type_info& baseType,
                                    TypeInfo::CreateFunction f )
  : mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry *impl = Internal::TypeRegistry::Get();

  if( impl->Register( name, baseType, f, false ) )
  {
    mName = name;
  }
}


const std::string TypeRegistration::RegisteredName() const
{
  return mName;
}

SignalConnectorType::SignalConnectorType( TypeRegistration& typeRegistration, const std::string& name, TypeInfo::SignalConnectorFunction func )
{
  Internal::TypeRegistry::Get()->RegisterSignal( typeRegistration, name, func );
}

TypeAction::TypeAction( TypeRegistration &registered, const std::string &name, TypeInfo::ActionFunction f)
{
  Internal::TypeRegistry::Get()->RegisterAction( registered, name, f );
}

PropertyRegistration::PropertyRegistration( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type, TypeInfo::SetPropertyFunction setFunc, TypeInfo::GetPropertyFunction getFunc )
{
  DALI_ASSERT_ALWAYS( ( index >= PROPERTY_REGISTRATION_START_INDEX ) && ( index <= PROPERTY_REGISTRATION_MAX_INDEX ) );

  Internal::TypeRegistry::Get()->RegisterProperty( registered, name, index, type, setFunc, getFunc );
}

AnimatablePropertyRegistration::AnimatablePropertyRegistration( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type )
{
  DALI_ASSERT_ALWAYS( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) );

  Internal::TypeRegistry::Get()->RegisterAnimatableProperty( registered, name, index, type );
}

AnimatablePropertyComponentRegistration::AnimatablePropertyComponentRegistration( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Index baseIndex, unsigned int componentIndex)
{
  DALI_ASSERT_ALWAYS( ( index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX ) && ( index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX ) );

  Internal::TypeRegistry::Get()->RegisterAnimatablePropertyComponent( registered, name, index, baseIndex, componentIndex );
}

} // namespace Dali
