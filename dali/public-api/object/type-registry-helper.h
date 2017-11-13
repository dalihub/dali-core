#ifndef __DALI_TYPE_REGISTRY_HELPER_H__
#define __DALI_TYPE_REGISTRY_HELPER_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

/**
 * @brief Definition for macros that are used internally by the property macros.
 * Use the the property macros in the section below this one (without the _INTERNAL postfix) when defining properties.
 * These internal macros exist as to perform the compile-time check on the enumeration order, the __COUNTER__ macro is used twice.
 * Using it twice within the same macro would result in two different values.
 */
#define DALI_TOKEN_PASTE_EXPAND( x, y ) x ## y
#define DALI_TOKEN_PASTE( x, y ) DALI_TOKEN_PASTE_EXPAND(x, y)

#define DALI_PROPERTY_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, valueType, enumIndex ) \
  Dali::PropertyRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, Dali::Property::valueType, &objectType::SetProperty, &objectType::GetProperty ); \
  DALI_COMPILE_TIME_ASSERT( ( objectNamespace::objectType::Property::enumIndex - objectNamespace::objectType::PROPERTY_START_INDEX ) == count );

#define DALI_PROPERTY_REGISTRATION_INTERNAL_READ_ONLY( count, typeRegistrationObject, objectNamespace, objectType, text, valueType, enumIndex ) \
  Dali::PropertyRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, Dali::Property::valueType, NULL, &objectType::GetProperty ); \
  DALI_COMPILE_TIME_ASSERT( ( objectNamespace::objectType::Property::enumIndex - objectNamespace::objectType::PROPERTY_START_INDEX ) == count );

#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, valueType, enumIndex) \
  Dali::AnimatablePropertyRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, Dali::Property::valueType );

#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, value, enumIndex) \
  Dali::AnimatablePropertyRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, value );

#define DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex) \
  Dali::AnimatablePropertyComponentRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::Property::enumIndex, objectNamespace::objectType::Property::baseEnumIndex, componentIndex );

#define DALI_CHILD_PROPERTY_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, valueType, enumIndex ) \
  Dali::ChildPropertyRegistration DALI_TOKEN_PASTE( property, count ) ( typeRegistrationObject, text, objectNamespace::objectType::ChildProperty::enumIndex, Property::valueType );

#define DALI_SIGNAL_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, textVariable ) \
  const char* const textVariable = text; \
  Dali::SignalConnectorType DALI_TOKEN_PASTE( signalConnector, count ) ( typeRegistrationObject, text, &objectNamespace::Internal::objectType::DoConnectSignal );

#define DALI_ACTION_REGISTRATION_INTERNAL( count, typeRegistrationObject, objectNamespace, objectType, text, textVariable ) \
  const char* const textVariable = text; \
  Dali::TypeAction DALI_TOKEN_PASTE( signalConnector, count ) ( typeRegistrationObject, text, &objectNamespace::Internal::objectType::DoAction );


/**
 * @brief These macros are used to define properties for implementations of CustomActor.
 * @SINCE_1_1.36
 *
 * These macros should be used when defining properties, signals and actions.
 * They provide the following benefits:
 * - A standard and consistent way to define properties.
 * - Concise definition promotes readability, especially with large numbers of properties.
 * - Provides a built-in compile-time check. This checks the order of the properties within the enumeration match the order of the property macros. Note: This check is not performed for animatable properties.
 * - Enforces how properties are enumerated in the object handles header file.
 *
 * Note: The compile-type check will produce the following message on failure:
 *       error: invalid application of 'sizeof' to incomplete type 'Dali::CompileTimeAssertBool<false>'
 *
 * Macro usage example:
 *
 * Within the your object's implementation cpp:
 * @code
 * #include <dali/public-api/object/type-registry-helper.h>
 * ...
 * DALI_TYPE_REGISTRATION_BEGIN( MyApp::MyCustomActor, Dali::CustomActor, Create )
 * DALI_PROPERTY_REGISTRATION( MyApp, MyCustomActor, "myProperty", INTEGER, MY_PROPERTY )
 * DALI_TYPE_REGISTRATION_END()
 * @endcode
 *
 * Within your handle's header:
 *
 * @code
 * #include <dali/public-api/common/dali-common.h>
 *
 *
 * ///< @brief The start and end property ranges for this control.
 * enum PropertyRange
 * {
 *   PROPERTY_START_INDEX = Dali::PROPERTY_REGISTRATION_START_INDEX,
 *   PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000
 * };
 *
 * ///< @brief Enumeration for the instance of properties belonging to the Button class.
 * struct Property
 * {
 *   enum
 *   {
 *     MY_PROPERTY = PROPERTY_START_INDEX    ///< @brief name "myProperty", type Integer
 *   };
 * };
 * @endcode
 *
 * Using these macros have certain prerequisites on how the property enumeration is defined.
 * Please see the Programming Guide (within the generated Doxygen) for full details.
 */
#define DALI_TYPE_REGISTRATION_BEGIN( thisType, baseType, createFunction ) \
  Dali::TypeRegistration typeRegistration( typeid( thisType ), typeid( baseType ), createFunction );

#define DALI_TYPE_REGISTRATION_BEGIN_CREATE( thisType, baseType, createFunction, createAtStartup ) \
  Dali::TypeRegistration typeRegistration( typeid( thisType ), typeid( baseType ), createFunction, createAtStartup );

#define DALI_PROPERTY_REGISTRATION( objectNamespace, objectType, text, valueType, enumIndex ) \
  DALI_PROPERTY_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, valueType, enumIndex )

#define DALI_PROPERTY_REGISTRATION_READ_ONLY( objectNamespace, objectType, text, valueType, enumIndex ) \
  DALI_PROPERTY_REGISTRATION_INTERNAL_READ_ONLY( __COUNTER__, typeRegistration, objectNamespace, objectType, text, valueType, enumIndex )

#define DALI_ANIMATABLE_PROPERTY_REGISTRATION( objectNamespace, objectType, text, valueType, enumIndex ) \
  DALI_ANIMATABLE_PROPERTY_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, valueType, enumIndex )

#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT( objectNamespace, objectType, text, value, enumIndex ) \
  DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, value, enumIndex )

#define DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION( objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex ) \
  DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex )

#define DALI_CHILD_PROPERTY_REGISTRATION( objectNamespace, objectType, text, valueType, enumIndex ) \
  DALI_CHILD_PROPERTY_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, valueType, enumIndex )

#define DALI_SIGNAL_REGISTRATION( objectNamespace, objectType, text, textVariable ) \
  DALI_SIGNAL_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, textVariable )

#define DALI_ACTION_REGISTRATION( objectNamespace, objectType, text, textVariable ) \
  DALI_ACTION_REGISTRATION_INTERNAL( __COUNTER__, typeRegistration, objectNamespace, objectType, text, textVariable )

#define DALI_TYPE_REGISTRATION_END( ) // This macro exists for consistency and readability.


#endif // __DALI_TYPE_REGISTRY_HELPER_H__
