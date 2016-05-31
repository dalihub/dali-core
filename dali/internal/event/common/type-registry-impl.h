#ifndef __DALI_INTERNAL_TYPE_REGISTRY_H__
#define __DALI_INTERNAL_TYPE_REGISTRY_H__

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

// INTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/common/type-info-impl.h>

namespace Dali
{

namespace Internal
{

////////////////////////////////////////////////////////////////////////////////
class TypeRegistry;

/*
* @copydoc Dali::TypeRegistry
*/
class TypeRegistry : public Dali::BaseObject
{
public:
  /**
   * Get the TypeRegistry
   */
  static TypeRegistry *Get();

  /*
   * @copydoc Dali::TypeRegistry::GetTypeInfo
   */
  Dali::TypeInfo GetTypeInfo( const std::string &uniqueTypeName );

  /*
   * @copydoc Dali::TypeRegistry::GetTypeInfo
   */
  Dali::TypeInfo GetTypeInfo( const std::type_info& registerType );

  /*
   * @copydoc Dali::TypeRegistry::GetTypeNameCount
   */
  size_t GetTypeNameCount() const;

  /*
   * @copydoc Dali::TypeRegistry::GetTypeName
   */
  std::string GetTypeName(size_t index) const;

  /*
   * Register a creation function under a unique name.
   * @param [in] theTypeInfo Type info for the type to be registered
   * @param [in] baseTypeInfo Type info for its base class
   * @param [in] createInstance Instance creation function
   * @param [in] callCreateOnInit If true call createInstance on dali initialisation
   * @return true if the name could be registered.
   */
  bool Register( const std::type_info& theTypeInfo, const std::type_info& baseTypeInfo,
                 Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit );

  /*
   * Register a creation function under a unique name.
   * @param [in] name The name type to be registered (must be unique)
   * @param [in] baseTypeInfo Type info for its base class
   * @param [in] createInstance Instance creation function
   * @param [in] callCreateOnInit If true call createInstance on dali initialisation
   * @return true if the name could be registered.
   */
  bool Register( const std::string& name, const std::type_info& baseTypeInfo,
                 Dali::TypeInfo::CreateFunction createInstance, bool callCreateOnInit  );

  /*
   * Register a signal connector function to a type
   * @param [in] typeRegistration TypeRegistration object used to register the type
   * @param [in] name Signal name
   * @param [in] func Signal connector function
   */
  void RegisterSignal( TypeRegistration& typeRegistration, const std::string& name, Dali::TypeInfo::SignalConnectorFunction func );

  /*
   * Register an action function to a type
   * @param [in] registered TypeRegistration object used to register the type
   * @param [in] name Action name
   * @param [in] f Action function
   * @return true if registered
   */
  bool RegisterAction( TypeRegistration &registered, const std::string &name, Dali::TypeInfo::ActionFunction f);

  /**
   * Register an event-thread only property with a type
   * @param [in] registered TypeRegistration object used to register the type
   * @param [in] name Property name
   * @param [in] index Property index
   * @param [in] type Property type
   * @param [in] setFunc The function to set the property (Can be NULL).
   * @param [in] getFunc The function to get the value of a property.
   * @return true if registered
   */
  bool RegisterProperty( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type, Dali::TypeInfo::SetPropertyFunction setFunc, Dali::TypeInfo::GetPropertyFunction getFunc );

  /**
   * Register a scene graph only property with a type
   * @param [in] registered TypeRegistration object used to register the type
   * @param [in] name Property name
   * @param [in] index Property index
   * @param [in] type Property type
   * @return true if registered
   */
  bool RegisterAnimatableProperty( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type );

  /**
   * Register a scene graph only property with a default value
   * @param [in] registered TypeRegistration object used to register the type
   * @param [in] name Property name
   * @param [in] index Property index
   * @param [in] value Property default value
   * @return true if registered
   */
  bool RegisterAnimatableProperty( TypeRegistration& registered, const std::string& name, Property::Index index, const Property::Value& defaultValue );

  /**
   * Register a component of a scene graph only property that supports components (i.e. Vector2, Vector3 and Vector4)
   * @param [in] registered TypeRegistration object used to register the type
   * @param [in] name Component name
   * @param [in] index Property index
   * @param [in] baseIndex Base animatable property index
   * @param [in] componentIndex Component index
   * @return true if registered
   */
  bool RegisterAnimatablePropertyComponent( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Index baseIndex, unsigned int componentIndex );

  /**
   * Register a event-thread only property with a type and a default value
   * @param [in] registered TypeRegistration object used to register the type
   * @param [in] name Property name
   * @param [in] index Property index
   * @param [in] type Property type
   * @return true if registered
   */
  bool RegisterChildProperty( TypeRegistration& registered, const std::string& name, Property::Index index, Property::Type type );

  /*
   * @copydoc Dali::Internal::TypeInfo::DoActionTo
   * Walks all base types until it finds a doer.
   */
  bool DoActionTo( BaseObject * const object, const std::string &actionName, const Property::Map& properties);

  /**
   * @copydoc Dali::BaseHandle::ConnectSignal()
   */
  bool ConnectSignal( BaseObject* object, ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor );

  /*
   * Return the type info for a given BaseObject pointer
   * @param [in] pBaseObject Pointer to a BaseObject
   * @return TypeInfo for the BaseObject.
   */
  Dali::TypeInfo GetTypeInfo(const Dali::BaseObject * const pBaseObject);

  /*
   * Calls any type creation functions that have been flagged as initialization functions
   */
  void CallInitFunctions(void) const;

public:

  /*
   * Return the name derived from type_info used to register the type
   * @param [in] registerType Type info for the type to be registered
   * @return registered name
   */
  static std::string RegistrationName( const std::type_info& registerType );

private:
  /*
   * Map from type name to TypeInfo
   */
  typedef std::map<std::string, Dali::TypeInfo> RegistryMap;
  RegistryMap mRegistryLut;

  typedef std::vector<Dali::TypeInfo::CreateFunction> InitFunctions;
  InitFunctions mInitFunctions;

private:
  TypeRegistry();
  ~TypeRegistry();

  /**
   * @brief Undefined Copy Constructor
   */
  TypeRegistry(TypeRegistry &);

  /**
   * @brief Undefined Assignment Operator
   */
  TypeRegistry& operator=(const TypeRegistry &);
};


} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::TypeRegistry& GetImplementation(Dali::TypeRegistry& typeRegistry)
{
  DALI_ASSERT_ALWAYS(typeRegistry);

  BaseObject& handle = typeRegistry.GetBaseObject();

  return static_cast<Internal::TypeRegistry&>(handle);
}

inline const Internal::TypeRegistry& GetImplementation(const Dali::TypeRegistry& typeRegistry)
{
  DALI_ASSERT_ALWAYS(typeRegistry);

  const BaseObject& handle = typeRegistry.GetBaseObject();

  return static_cast<const Internal::TypeRegistry&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_TYPE_REGISTRY_H__
