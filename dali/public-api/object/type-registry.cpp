/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/common/type-registry-impl.h>
#include <dali/internal/event/object/default-property-metadata.h>
#include <dali/public-api/object/property-index-ranges.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace Dali
{
TypeRegistry::TypeRegistry() = default;

TypeRegistry::~TypeRegistry() = default;

TypeRegistry::TypeRegistry(const TypeRegistry& copy) = default;

TypeRegistry& TypeRegistry::operator=(const TypeRegistry& rhs) = default;

TypeRegistry::TypeRegistry(TypeRegistry&& rhs) noexcept = default;

TypeRegistry& TypeRegistry::operator=(TypeRegistry&& rhs) noexcept = default;

TypeRegistry TypeRegistry::Get()
{
  return TypeRegistry(Internal::TypeRegistry::Get());
}

Dali::TypeInfo TypeRegistry::GetTypeInfo(const Dali::StringView& uniqueTypeName)
{
  return Dali::TypeInfo(GetImplementation(*this).GetTypeInfo(ToStdString(uniqueTypeName)).Get());
}

Dali::TypeInfo TypeRegistry::GetTypeInfo(const std::type_info& registerType)
{
  return Dali::TypeInfo(GetImplementation(*this).GetTypeInfo(registerType).Get());
}

size_t TypeRegistry::GetTypeNameCount() const
{
  return GetImplementation(*this).GetTypeNameCount();
}

Dali::String TypeRegistry::GetTypeName(size_t index) const
{
  return ToDaliString(GetImplementation(*this).GetTypeName(static_cast<uint32_t>(index)));
}

TypeRegistry::TypeRegistry(Internal::TypeRegistry* internal)
: BaseHandle(internal)
{
}

TypeRegistration::TypeRegistration(const std::type_info& registerType, const std::type_info& baseType, TypeInfo::CreateFunction f)
: mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry* impl = Internal::TypeRegistry::Get();

  mName = ToDaliString(impl->Register(registerType, baseType, f, false));
}

TypeRegistration::TypeRegistration(const std::type_info& registerType, const std::type_info& registerObjectType, const std::type_info& baseType, TypeInfo::CreateFunction f)
: mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry* impl = Internal::TypeRegistry::Get();

  mName = ToDaliString(impl->Register(registerType, registerObjectType, baseType, f, false));
}

TypeRegistration::TypeRegistration(const std::type_info& registerType, const std::type_info& baseType, TypeInfo::CreateFunction f, bool callCreateOnInit)
: mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry* impl = Internal::TypeRegistry::Get();

  mName = ToDaliString(impl->Register(registerType, baseType, f, callCreateOnInit));
}

TypeRegistration::TypeRegistration(const std::type_info& registerType, const std::type_info& registerObjectType, const std::type_info& baseType, TypeInfo::CreateFunction f, bool callCreateOnInit)
: mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry* impl = Internal::TypeRegistry::Get();

  mName = ToDaliString(impl->Register(registerType, registerObjectType, baseType, f, callCreateOnInit));
}

TypeRegistration::TypeRegistration(const std::type_info& registerType, const std::type_info& baseType, TypeInfo::CreateFunction f, const DefaultPropertyMetadata& defaultProperties)
{
  Internal::TypeRegistry* impl = Internal::TypeRegistry::Get();

  mName = ToDaliString(impl->Register(registerType, baseType, f, false, defaultProperties.propertyTable, defaultProperties.propertyCount));
}

TypeRegistration::TypeRegistration(const std::type_info& registerType, const std::type_info& registerObjectType, const std::type_info& baseType, TypeInfo::CreateFunction f, const DefaultPropertyMetadata& defaultProperties)
{
  Internal::TypeRegistry* impl = Internal::TypeRegistry::Get();

  mName = ToDaliString(impl->Register(registerType, registerObjectType, baseType, f, false, defaultProperties.propertyTable, defaultProperties.propertyCount));
}

TypeRegistration::TypeRegistration(Dali::String name, const std::type_info& baseType, TypeInfo::CreateFunction f)
: mReference(Internal::TypeRegistry::Get())
{
  Internal::TypeRegistry* impl = Internal::TypeRegistry::Get();

  mName = ToDaliString(impl->Register(ToStdString(std::move(name)), baseType, f, false));
}

Dali::String TypeRegistration::RegisteredName() const
{
  return mName;
}

SignalConnectorType::SignalConnectorType(TypeRegistration& typeRegistration, Dali::String name, TypeInfo::SignalConnectorFunction func)
{
  Internal::TypeRegistry::Get()->RegisterSignal(typeRegistration, ToStdString(std::move(name)), func);
}

TypeAction::TypeAction(TypeRegistration& registered, Dali::String name, TypeInfo::ActionFunction f)
{
  Internal::TypeRegistry::Get()->RegisterAction(registered, ToStdString(std::move(name)), f);
}

PropertyRegistration::PropertyRegistration(TypeRegistration& registered, Dali::String name, Property::Index index, Property::Type type, TypeInfo::SetPropertyFunction setFunc, TypeInfo::GetPropertyFunction getFunc)
{
  DALI_ASSERT_ALWAYS((index >= PROPERTY_REGISTRATION_START_INDEX) && (index <= PROPERTY_REGISTRATION_MAX_INDEX));

  Internal::TypeRegistry::Get()->RegisterProperty(registered, ToStdString(std::move(name)), index, type, setFunc, getFunc);
}

AnimatablePropertyRegistration::AnimatablePropertyRegistration(TypeRegistration& registered, Dali::String name, Property::Index index, Property::Type type, TypeInfo::SetPropertyFunction setFunc, TypeInfo::GetPropertyFunction getFunc)
{
  DALI_ASSERT_ALWAYS((index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX) && (index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX));

  Internal::TypeRegistry::Get()->RegisterAnimatableProperty(registered, ToStdString(std::move(name)), index, type, setFunc, getFunc);
}

AnimatablePropertyRegistration::AnimatablePropertyRegistration(TypeRegistration& registered, Dali::String name, Property::Index index, const Property::Value& value, TypeInfo::SetPropertyFunction setFunc, TypeInfo::GetPropertyFunction getFunc)
{
  DALI_ASSERT_ALWAYS((index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX) && (index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX));

  Internal::TypeRegistry::Get()->RegisterAnimatableProperty(registered, ToStdString(std::move(name)), index, value, setFunc, getFunc);
}

AnimatablePropertyComponentRegistration::AnimatablePropertyComponentRegistration(TypeRegistration& registered, Dali::String name, Property::Index index, Property::Index baseIndex, uint32_t componentIndex)
{
  DALI_ASSERT_ALWAYS((index >= ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX) && (index <= ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX));

  Internal::TypeRegistry::Get()->RegisterAnimatablePropertyComponent(registered, ToStdString(std::move(name)), index, baseIndex, componentIndex);
}

ChildPropertyRegistration::ChildPropertyRegistration(TypeRegistration& registered, Dali::String name, Property::Index index, Property::Type type)
{
  DALI_ASSERT_ALWAYS((index >= CHILD_PROPERTY_REGISTRATION_START_INDEX) && (index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX));

  Internal::TypeRegistry::Get()->RegisterChildProperty(registered, ToStdString(std::move(name)), index, type);
}

ChildPropertyRegistration::ChildPropertyRegistration(Dali::String registered, Dali::String name, Property::Index index, Property::Type type)
{
  DALI_ASSERT_ALWAYS((index >= CHILD_PROPERTY_REGISTRATION_START_INDEX) && (index <= CHILD_PROPERTY_REGISTRATION_MAX_INDEX));

  Internal::TypeRegistry::Get()->RegisterChildProperty(ToStdString(std::move(registered)), ToStdString(std::move(name)), index, type);
}

} // namespace Dali
