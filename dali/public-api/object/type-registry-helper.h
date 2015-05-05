#ifndef __DALI_TYPE_REGISTRY_HELPER_H__
#define __DALI_TYPE_REGISTRY_HELPER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/compile-time-assert.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief These macros are used to use the type-registry to register properties and signals.
 * This forces registration to be done in a standard way across actors and controls and facilitates future compile-time checks to be added.
 *
 * Note: Currently a compile time check is done for the order of the properties. The order is not critical to registration, but helps maintain the ordering between the header and implementations to avoid properties being added out of place.
 *
 * Note: The signal macro also generates string const chars.
 */
#define DALI_TOKEN_PASTE_EXPAND( x, y ) x ## y
#define DALI_TOKEN_PASTE( x, y ) DALI_TOKEN_PASTE_EXPAND(x, y)

#define DALI_PROPERTY_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, valueType, enumIndex ) \
  PropertyRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, Property::valueType, &objectType::SetProperty, &objectType::GetProperty ); \
  DALI_COMPILE_TIME_ASSERT( ( objectNamespace::objectType::Property::enumIndex - objectNamespace::objectType::PROPERTY_START_INDEX ) == count );

#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, valueType, enumIndex) \
  AnimatablePropertyRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, Property::valueType );

#define DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex) \
  AnimatablePropertyComponentRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, objectNamespace::objectType::Property::baseEnumIndex, componentIndex );

#define DALI_SIGNAL_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, textVariable ) \
  const char* const textVariable = text; \
  SignalConnectorType DALI_TOKEN_PASTE( signalConnector, count ) ( typeRegistrationObject, text, &objectNamespace::Internal::objectType::DoConnectSignal );

#define DALI_ACTION_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, textVariable ) \
  const char* const textVariable = text; \
  TypeAction DALI_TOKEN_PASTE( signalConnector, count ) ( typeRegistrationObject, text, &objectNamespace::Internal::objectType::DoAction );

// For use within implementations:

#define DALI_TYPE_REGISTRATION_BEGIN( thisType, baseType, createFunction ) \
  TypeRegistration typeRegistration( typeid( thisType ), typeid( baseType ), createFunction );

#define DALI_TYPE_REGISTRATION_BEGIN_CREATE( thisType, baseType, createFunction, createAtStartup ) \
  TypeRegistration typeRegistration( typeid( thisType ), typeid( baseType ), createFunction, createAtStartup );

#define DALI_PROPERTY_REGISTRATION( objectNamespace, objectType, text, valueType, enumIndex ) \
  DALI_PROPERTY_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, valueType, enumIndex )

#define DALI_ANIMATABLE_PROPERTY_REGISTRATION( objectNamespace, objectType, text, valueType, enumIndex ) \
  DALI_ANIMATABLE_PROPERTY_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, valueType, enumIndex )

#define DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex ) \
  DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex )

#define DALI_SIGNAL_REGISTRATION( objectNamespace, objectType, text, textVariable ) \
  DALI_SIGNAL_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, textVariable )

#define DALI_ACTION_REGISTRATION( objectNamespace, objectType, text, textVariable ) \
  DALI_ACTION_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, textVariable )

#define DALI_TYPE_REGISTRATION_END( ) // Empty for now, can be used to perform checks.


} // namespace Internal

} // namespace Dali

#endif // __DALI_TYPE_REGISTRY_HELPER_H__
