#ifndef __DALI_TYPE_REGISTRY_H__
#define __DALI_TYPE_REGISTRY_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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


// EXTERNAL INCLUDES
#include <typeinfo>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/type-info.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

namespace Internal DALI_INTERNAL
{
class TypeRegistry;
}

/**
 * @brief The TypeRegistry allows registration of type instance creation functions.
 *
 * These can then be created later by name and down cast to the appropriate type.
 *
 * Usage: (Registering)
 *
 * In my-actor.cpp
 * @code
 * // Note: object construction in namespace scope is defined in a translation unit as being
 * //       in appearance order (C++ standard 3.6/2). So TypeRegistration is declared first in
 * //       the cpp file below. Signal, action and property declarations follow in any order.
 * namespace
 * {
 *   TypeRegistration myActorType(typeid(MyActor), typeid(Actor), CreateMyActor );
 *
 *   SignalConnectorType( myActorType, "highlighted", ConnectSignalForMyActor );
 *   TypeAction( myActorType, "open", DoMyActorAction );
 *   TypeAction( myActorType, "close", DoMyActorAction );
 *   PropertyRegistration( myActorType, "status", PropertyRegistration::START_INDEX, Property::BOOLEAN, SetPropertyFunction, GetPropertyFunction );
 * }
 * @endcode
 *
 * Usage: (Creation)
 *
 * @code
 *   TypeRegistry::TypeInfo type = TypeRegistry::Get().GetTypeInfo("MyActor");
 *   MyActor a = MyActor::DownCast(type.CreateInstance());
 *   if(a)
 *   {
 *     ...
 *   }
 * @endcode
 *
 * CustomActor
 *
 *  Actors that inherit from the CustomActor framework must ensure the implementation
 *  class has an identical name to the Actor class. This is to ensure the class can be
 *  found at runtime for signals and actions. Otherwise these will silently fail.
 *
 *  As namespaces are discarded the convention is to use a namespace ie
 *
 *    'class MyActor {}; namespace Internal { class MyActor {}; }'
 *
 *  Warning: this arrangement will silently fail
 *
 *    'class MyActor {}; class MyActorImpl {};'
 *
 * Naming Conventions
 *
 *   Signal and action names follow properties and are by convention lower case hyphen
 *   separated ie 'next-page'. This maintains consistency with the scripted interface.
 *
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API TypeRegistry : public BaseHandle
{
public:
  /**
   * @brief Get Type Registry handle.
   *
   * @SINCE_1_0.0
   * @return TypeRegistry handle
   */
  static TypeRegistry Get();

  /**
   * @brief Allows the creation of an empty typeRegistry handle.
   * @SINCE_1_0.0
   */
  TypeRegistry();

  /**
   * @brief destructor.
   * @SINCE_1_0.0
   */
  ~TypeRegistry();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  TypeRegistry(const TypeRegistry& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  TypeRegistry& operator=(const TypeRegistry& rhs);

  /**
   * @brief Get TypeInfo for a registered type.
   *
   * @SINCE_1_0.0
   * @param [in] uniqueTypeName A unique type name
   * @return TypeInfo if the type exists otherwise an empty handle
   */
  TypeInfo GetTypeInfo( const std::string &uniqueTypeName );

  /**
   * @brief Get TypeInfo for a registered type.
   *
   * @SINCE_1_0.0
   * @param [in] registerType The registered type info
   * @return TypeInfo if the type exists otherwise an empty handle
   */
  TypeInfo GetTypeInfo( const std::type_info& registerType );

  /**
   * @brief Get type name count.
   *
   * @SINCE_1_0.0
   * @return The count
   */
  size_t GetTypeNameCount() const;

  /**
   * @brief Get type names by index.
   *
   * @SINCE_1_0.0
   * @param[in] index The index to get the type name
   * @return The type name or an empty string when index is not valid
   */
  std::string GetTypeName(size_t index) const;

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Dali Get() method.
   *
   * @SINCE_1_0.0
   * @param [in] typeRegistry A pointer to a Dali resource
   */
  explicit DALI_INTERNAL TypeRegistry(Internal::TypeRegistry*typeRegistry);
  /// @endcond
};

/**
 * @brief Register a type from type info.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API TypeRegistration
{
public:
  /**
   * @brief Constructor registers the type creation function.
   *
   * @SINCE_1_0.0
   * @param [in] registerType the type info for the type to be registered
   * @param [in] baseType the base type info of registerType
   * @param [in] f registerType instance creation function
   */
  TypeRegistration( const std::type_info& registerType, const std::type_info& baseType,
                    TypeInfo::CreateFunction f );

  /**
   * @brief Constructor registers the type creation function.
   *
   * @SINCE_1_0.0
   * @param [in] registerType the type info for the type to be registered
   * @param [in] baseType the base type info of registerType
   * @param [in] f registerType instance creation function
   * @param [in] callCreateOnInit If true the creation function is called as part of Dali initialisation
   */
  TypeRegistration( const std::type_info& registerType, const std::type_info& baseType,
                    TypeInfo::CreateFunction f, bool callCreateOnInit );

  /**
   * @brief Constructor registers the type creation function for a named class or type.
   *
   * This allows types to be created dynamically from script. The name must be
   * unique for successful registration.
   * @SINCE_1_0.0
   * @param [in] name the name of the type to be registered
   * @param [in] baseType the base type info of registerType
   * @param [in] f registerType instance creation function
   */
  TypeRegistration( const std::string& name, const std::type_info& baseType,
                    TypeInfo::CreateFunction f );

  /**
   * @brief The name the type is registered under (derived from type_info).
   *
   * @SINCE_1_0.0
   * @return the registered name or empty if unregistered
   */
  const std::string RegisteredName() const;

private:
  TypeRegistry mReference; ///< Reference to the type registry
  std::string mName;       ///< Name of the type
};

/**
 * @brief Register a signal connector function to a registered type.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API SignalConnectorType
{
public:
  /**
   * @brief Constructor registers the type creation function.
   *
   * @SINCE_1_0.0
   * @param [in] typeRegistration The TypeRegistration object
   * @param [in] name The signal name
   * @param [in] func The signal connector function
   */
  SignalConnectorType( TypeRegistration& typeRegistration, const std::string& name, TypeInfo::SignalConnectorFunction func );
};

/**
 * @brief Register an action function.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API TypeAction
{
public:
  /**
   * @brief Constructor registers the type creation function.
   *
   * @SINCE_1_0.0
   * @param [in] registered The TypeRegistration object
   * @param [in] name The action name
   * @param [in] f The action function
   */
  TypeAction( TypeRegistration &registered, const std::string &name, TypeInfo::ActionFunction f);
};

/**
 * @brief Register a property for the given type.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API PropertyRegistration
{
public:

  /**
   * @brief This constructor registers the property with the registered type.
   *
   * This constructor is for event-thread only properties where the
   * value of the property can be retrieved and set via specified
   * functions.
   *
   * Functions of the following type may be used for setFunc and getFunc respectively:
   * @code
   *   void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );
   *   Property::Value GetProperty( BaseObject* object, Property::Index index );
   * @endcode
   *
   * @SINCE_1_0.0
   * @param [in] registered The TypeRegistration object
   * @param [in] name The name of the property
   * @param [in] index The property index. Must be a value between PROPERTY_REGISTRATION_START_INDEX and PROPERTY_REGISTRATION_MAX_INDEX inclusive.
   * @param [in] type The property value type.
   * @param [in] setFunc The function to call when setting the property. If NULL, then the property becomes read-only.
   * @param [in] getFunc The function to call to retrieve the current value of the property. MUST be provided.
   *
   * @pre "registered" must be registered with the TypeRegistry.
   * @note The "index" value must be between START_INDEX and MAX_INDEX inclusive.
   * @note If "setFunc" is NULL, then the property becomes a read-only property.
   * @note "getFunc" MUST be provided
   *
   */
  PropertyRegistration( TypeRegistration& registered,
                        const std::string& name, Property::Index index, Property::Type type,
                        TypeInfo::SetPropertyFunction setFunc, TypeInfo::GetPropertyFunction getFunc );
};

/**
 * @brief Register an animatable property for the given type.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API AnimatablePropertyRegistration
{
public:

  /**
   * @brief This constructor registers the animatable property with the registered type.
   *
   * This constructor is for scene-graph only properties where the
   * value of the property can be retrieved and set via specified
   * functions.
   *
   * @SINCE_1_0.0
   * @param [in] registered The TypeRegistration object
   * @param [in] name The name of the property
   * @param [in] index The property index. Must be a value between ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX and ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX inclusive.
   * @param [in] type The property value type.
   *
   * @pre "registered" must be registered with the TypeRegistry.
   */
  AnimatablePropertyRegistration( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type );

  /**
   * @brief This constructor registers the animatable property with the registered default value.
   *
   * This constructor is for scene-graph only properties where the
   * value of the property can be retrieved and set via specified
   * functions.
   *
   * @SINCE_1_1.18
   * @param [in] registered The TypeRegistration object
   * @param [in] name The name of the property
   * @param [in] index The property index. Must be a value between ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX and ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX inclusive.
   * @param [in] value The property default value.
   *
   * @pre "registered" must be registered with the TypeRegistry.
   */
  AnimatablePropertyRegistration( TypeRegistration& registered, const std::string& name, Property::Index index, const Property::Value& value );
};

/**
 * @brief Register a component of animatable property for the given component index.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API AnimatablePropertyComponentRegistration
{
public:

  /**
   * @brief This constructor registers a component of an animatable property where
   * the base animatable property must be a property that supports property component
   * (i.e. Vector2, Vector3 or Vector4) and the base animatable property must have
   * been registered.
   *
   * This constructor is for a component of scene-graph only properties where the
   * value of the property can be retrieved and set via specified functions.
   *
   * @SINCE_1_0.0
   * @param [in] registered The TypeRegistration object
   * @param [in] name The name of the component
   * @param [in] index The property index. Must be a value between ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX and ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX inclusive.
   * @param [in] baseIndex The index of the base animatable property. Must be a value between ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX and ANIMATABLE_PROPERTY_REGISTRATION_MAX_INDEX inclusive.
   * @param [in] componentIndex The index of the component (e.g. 0 for the x component of a Vector2 property and 1 for the y component of a Vector2 property).
   *
   * @pre "registered" must be registered with the TypeRegistry.
   */
  AnimatablePropertyComponentRegistration( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Index baseIndex, unsigned int componentIndex );
};

/**
 * @brief Register a child property for the given type.
 * @SINCE_1_1.35
 */
class DALI_IMPORT_API ChildPropertyRegistration
{
public:

  /**
   * @brief This constructor registers an event-thread only child property (i.e. a property
   * that the parent supports in its children) with the registered type.
   *
   * @SINCE_1_1.35
   * @param [in] registered The TypeRegistration object
   * @param [in] name The name of the property
   * @param [in] index The property index. Must be a value between CHILD_PROPERTY_REGISTRATION_START_INDEX and CHILD_PROPERTY_REGISTRATION_MAX_INDEX inclusive.
   * @param [in] type The property value type.
   *
   * @pre "registered" must be registered with the TypeRegistry.
   */
  ChildPropertyRegistration( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type );
};


/**
 * @}
 */
} // namespace Dali

#endif // header
