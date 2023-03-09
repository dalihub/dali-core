#ifndef DALI_HANDLE_H
#define DALI_HANDLE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/indirect-value.h>
#include <dali/public-api/object/property-key.h>
#include <dali/public-api/object/property-notification-declarations.h>
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

class Constraint;
class PropertyNotification;
class PropertyCondition;

namespace Internal DALI_INTERNAL
{
class Object;
}

/**
 * @brief Dali::Handle is a handle to an internal property owning Dali object that can have constraints applied to it.
 * @SINCE_1_0.0
 */
class DALI_CORE_API Handle : public BaseHandle
{
public:
  /**
   * @brief Enumeration for Handle's capabilities that can be queried using Handle::Supports().
   * @SINCE_1_0.0
   */
  enum Capability
  {
    /**
     * @brief Some objects support dynamic property creation at run-time.
     *
     * New properties are registered by calling RegisterProperty() with an unused property name.
     * @SINCE_1_0.0
     */
    DYNAMIC_PROPERTIES = 0x01,
  };

  /**
   * @brief PropertySetSignal function prototype for signal handler. Called when a property is set on this object.
   */
  using PropertySetSignalType = Signal<void(Handle& handle, Property::Index index, const Property::Value& value)>;

public:
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] handle A pointer to a newly allocated Dali resource
   */
  Handle(Dali::Internal::Object* handle);

  /**
   * @brief This constructor provides an uninitialized Dali::Handle.
   *
   * This should be initialized with a Dali New() method before use.
   * Methods called on an uninitialized Dali::Handle will assert.
   * @code
   * Handle handle; // uninitialized
   * handle.SomeMethod(); // unsafe! This will assert
   *
   * handle = SomeClass::New(); // now initialized
   * handle.SomeMethod(); // safe
   * @endcode
   * @SINCE_1_0.0
   */
  Handle();

  /**
   * @brief Creates a new object.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated object
   */
  static Handle New();

  /**
   * @brief Template to create a derived handle and set properties on it.
   *
   * Marked as DALI_NO_EXPORT_API to prevent internal usage exporting symbols.
   * @SINCE_1_9.27
   * @tparam T The derived class to create
   * @param[in] properties The properties to set
   */
  template<typename Type>
  static DALI_NO_EXPORT_API Type New(const Property::Map& properties)
  {
    Type handle = Type::New();
    handle.SetProperties(properties);
    return handle;
  }

  /**
   * @brief Dali::Handle is intended as a base class.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Handle();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  Handle(const Handle& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  Handle& operator=(const Handle& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   */
  Handle(Handle&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  Handle& operator=(Handle&& rhs) noexcept;

  /**
   * @brief Downcasts to a handle.
   *
   * If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle or an uninitialized handle
   */
  static Handle DownCast(BaseHandle handle);

  /**
   * @brief Queries whether an handle supports a given capability.
   *
   * @SINCE_1_0.0
   * @param[in] capability The queried capability
   * @return True if the capability is supported
   */
  bool Supports(Capability capability) const;

  // Properties

  /**
   * @brief Queries how many properties are provided by an handle.
   *
   * This may vary between instances of a class, if dynamic properties are supported.
   * @SINCE_1_0.0
   * @return The number of properties
   */
  uint32_t GetPropertyCount() const;

  /**
   * @brief Queries the name of a property.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @return The name of the property
   */
  std::string GetPropertyName(Property::Index index) const;

  /**
   * @brief Query the index of a property using the given key.
   *
   * @SINCE_1_9.27
   * @param[in] key The key of the property to search for. (The name or integer key provided to
   * RegisterProperty()).
   * @return the matching property index of the key, or Property::INVALID_INDEX if no
   * property matches the given key.
   */
  Property::Index GetPropertyIndex(Property::Key key) const;

  /**
   * @brief Queries whether a property can be set using SetProperty().
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @return True if the property is writable
   * @pre Property::INVALID_INDEX < index.
   */
  bool IsPropertyWritable(Property::Index index) const;

  /**
   * @brief Queries whether a writable property can be the target of an animation or constraint.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @return True if the property is animatable
   */
  bool IsPropertyAnimatable(Property::Index index) const;

  /**
   * @brief Queries whether a property can be used as in input to a constraint.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @return True if the property can be used as a constraint input
   */
  bool IsPropertyAConstraintInput(Property::Index index) const;

  /**
   * @brief Queries the type of a property.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @return The type of the property
   */
  Property::Type GetPropertyType(Property::Index index) const;

  /**
   * @brief Sets the value of an existing property.
   *
   * Property should be write-able. Setting a read-only property is a no-op.
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @param[in] propertyValue The new value of the property
   * @pre The property types match i.e. propertyValue.GetType() is equal to GetPropertyType(index).
   */
  void SetProperty(Property::Index index, Property::Value propertyValue);

  /**
   * @brief Reserves a number of custom properties
   *
   * Saves automatic re-allocation of vectors for properties when we know in advance how many there
   * will be.
   *
   * @param[in] propertyCount The total number of initial properties.
   */
  void ReserveCustomProperties(int propertyCount);

  /**
   * @brief Registers a new animatable property.
   *
   * @SINCE_1_0.0
   * @param[in] name The name of the property
   * @param[in] propertyValue The new value of the property
   * @return The index of the property or Property::INVALID_INDEX if registration failed
   * @pre The object supports dynamic properties i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.
   * Property names are expected to be unique, but this is not enforced.
   * Property indices are unique to each registered custom property in a given object.
   * returns Property::INVALID_INDEX if registration failed. This can happen if you try to register
   * animatable property on an object that does not have scene graph object.
   * @note Only the following types can be animated:
   *       - Property::BOOLEAN
   *       - Property::FLOAT
   *       - Property::INTEGER
   *       - Property::VECTOR2
   *       - Property::VECTOR3
   *       - Property::VECTOR4
   *       - Property::MATRIX3
   *       - Property::MATRIX
   *       - Property::ROTATION
   * @note If a property with the desired name already exists, then the value given is just set.
   */
  Property::Index RegisterProperty(std::string_view name, Property::Value propertyValue);

  /**
   * @brief Registers a new animatable property.
   *
   * @SINCE_2_1.6
   * @param[in] name The name of the property
   * @param[in] propertyValue The new value of the property
   * @return The index of the property or Property::INVALID_INDEX if registration failed
   * @pre The object supports dynamic properties i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.
   * Property names are expected to be unique, but this is DEFINITELY not enforced. It is up to the
   * caller to enforce uniqueness.
   *
   * Property indices are unique to each registered custom property in a given object.
   * returns Property::INVALID_INDEX if registration failed. This can happen if you try to register
   * animatable property on an object that does not have scene graph object.
   * @note Only the following types can be animated:
   *       - Property::BOOLEAN
   *       - Property::FLOAT
   *       - Property::INTEGER
   *       - Property::VECTOR2
   *       - Property::VECTOR3
   *       - Property::VECTOR4
   *       - Property::MATRIX3
   *       - Property::MATRIX
   *       - Property::ROTATION
   * @note If a property with the desired name already exists, then this creates a secondary
   * entry to a different scene graph property; Access by index works as expected, but uniform
   * values will use the last registered version, not the existing version, so things may break.
   */
  Property::Index RegisterUniqueProperty(std::string_view name, Property::Value propertyValue);

  /**
   * @brief Register a new animatable property with an integer key.
   *
   * @SINCE_1_9.27
   * @param[in] key  The integer key of the property.
   * @param[in] name The text key of the property.
   * @param[in] propertyValue The new value of the property.
   *
   * @return The index of the property or Property::INVALID_INDEX if registration failed
   *
   * @pre The object supports dynamic properties
   * i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.  Property names and keys
   * are expected to be unique, but this is not enforced.  Property indices are unique
   * to each registered custom property in a given object.
   * @todo CHECK THIS!
   *
   * @note Returns Property::INVALID_INDEX if registration failed. This can happen if
   * you try to register animatable property on an object that does not have scene graph
   * object.
   *
   * @note The returned property index is not the same as the integer key (though it
   * shares a type)
   *
   * This version of RegisterProperty associates both an integer key and the text key
   * with the property, allowing for lookup of the property index by either key or name
   * ( which is useful when other classes know the key but not the name )
   *
   * @note Only the following types can be animated:
   *       - Property::BOOLEAN
   *       - Property::FLOAT
   *       - Property::INTEGER
   *       - Property::VECTOR2
   *       - Property::VECTOR3
   *       - Property::VECTOR4
   *       - Property::MATRIX3
   *       - Property::MATRIX
   *       - Property::ROTATION
   * @note If a property with the desired name already exists, then the value given is just set.
   */
  Property::Index RegisterProperty(Property::Index  key,
                                   std::string_view name,
                                   Property::Value  propertyValue);

  /**
   * @brief Register a new animatable property with an integer key.
   *
   * @SINCE_2_1.6
   * @param[in] key  The integer key of the property.
   * @param[in] name The text key of the property.
   * @param[in] propertyValue The new value of the property.
   *
   * @return The index of the property or Property::INVALID_INDEX if registration failed
   * It is up to the caller to guarantee that the property is unique. This allows many
   * checks to be skipped.
   *
   * @pre The object supports dynamic properties
   * i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.  Property names and keys
   * are expected to be unique, and are therefore just added without any checks.
   * Property indices are unique to each registered custom property in a given object.
   *
   * @note Returns Property::INVALID_INDEX if registration failed. This can happen if
   * you try to register animatable property on an object that does not have scene graph
   * object.
   *
   * @note The returned property index is not the same as the integer key (though it
   * shares a type)
   *
   * This version of RegisterProperty associates both an integer key and the text key
   * with the property, allowing for lookup of the property index by either key or name
   * ( which is useful when other classes know the key but not the name )
   *
   * @note Only the following types can be animated:
   *       - Property::BOOLEAN
   *       - Property::FLOAT
   *       - Property::INTEGER
   *       - Property::VECTOR2
   *       - Property::VECTOR3
   *       - Property::VECTOR4
   *       - Property::MATRIX3
   *       - Property::MATRIX
   *       - Property::ROTATION
   *
   * @note If a property with the desired name already exists, then this will create a second entry with
   * the same name, and may cause problems. It is up to the caller to prevent this happening. Possible side
   * effects are: lookup by name always finds the first such property, not the second; whereas, writing
   * uniform value to shader will use the second, not the first;
   * Using the returned Property::Index for future reference will always access the correct property.
   */
  Property::Index RegisterUniqueProperty(Property::Index  key,
                                         std::string_view name,
                                         Property::Value  propertyValue);

  /**
   * @brief Registers a new property.
   *
   * Properties can be set as non animatable using property attributes.
   * @SINCE_1_0.0
   * @param[in] name The name of the property
   * @param[in] propertyValue The new value of the property
   * @param[in] accessMode The property access mode (writable, animatable etc)
   * @return The index of the property
   * @pre The handle supports dynamic properties i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.
   * @pre name is unused i.e. GetPropertyIndex(name) returns PropertyIndex::INVALID.
   * @note Only the following types can be animated:
   *       - Property::BOOLEAN
   *       - Property::FLOAT
   *       - Property::INTEGER
   *       - Property::VECTOR2
   *       - Property::VECTOR3
   *       - Property::VECTOR4
   *       - Property::MATRIX3
   *       - Property::MATRIX
   *       - Property::ROTATION
   * @note If a property with the desired name already exists, then the value given is just set.
   */
  Property::Index RegisterProperty(std::string_view name, Property::Value propertyValue, Property::AccessMode accessMode);

  /**
   * @brief Retrieves a property value.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @return The property value
   * @note This returns the value set by SetProperty() or the animation target value if it is being animated.
   * @note To get the current value on the scene-graph, use GetCurrentProperty().
   */
  Property::Value GetProperty(Property::Index index) const;

  /**
   * @brief Convenience function for obtaining a property of a known type.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @return The property value
   * @pre The property types match i.e. PropertyTypes::Get<T>() is equal to GetPropertyType(index).
   * @see GetProperty()
   */
  template<typename T>
  T GetProperty(Property::Index index) const
  {
    Property::Value value = GetProperty(index);

    return T(value.Get<T>());
  }

  /**
   * @brief Retrieves the latest value of the property from the scene-graph.
   *
   * @SINCE_1_2.41
   * @param[in] index The index of the property
   * @return The property value
   * @note This returns the value of the property in the last rendered frame so can be different to that
   *       set by SetProperty() if the set-message has not been processed by the scene-graph yet.
   * @note To retrieve the value set by SetProperty(), use GetProperty().
   */
  Property::Value GetCurrentProperty(Property::Index index) const;

  /**
   * @brief Convenience function for obtaining the current value of a property of a known type.
   *
   * @SINCE_1_2.41
   * @param[in] index The index of the property
   * @return The property value
   * @pre The property types match i.e. PropertyTypes::Get<T>() is equal to GetPropertyType(index).
   * @see GetCurrentProperty()
   */
  template<typename T>
  T GetCurrentProperty(Property::Index index) const
  {
    Property::Value value = GetCurrentProperty(index);

    return T(value.Get<T>());
  }

  /**
   * @brief Sets all the properties in the given property map.
   *
   * @SINCE_1_9.27
   * @param[in] properties The properties to set
   */
  void SetProperties(const Property::Map& properties);

  /**
   * @brief Retrieves all the properties and the values for this object
   *
   * @SINCE_1_9.27
   * @param[out] properties A map which is populated with the index-value pairs
   *
   * @note The properties map will be cleared by this method first.
   */
  void GetProperties(Property::Map& properties);

  /**
   * @brief Retrieves all the property indices for this object (including custom properties).
   *
   * @SINCE_1_0.0
   * @param[out] indices A container of property indices for this object
   * @note The added container is cleared.
   */
  void GetPropertyIndices(Property::IndexContainer& indices) const;

  /**
   * @brief Determine if the custom property index exists on this object without throwing a Dali::Exception.
   *
   * @SINCE_1_9.27
   * @note This does not check default properties.
   * @param[in] index The index of the property to test for
   */
  bool DoesCustomPropertyExist(Property::Index index);

  /**
   * @brief Adds a property notification to this object.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @param[in] condition The notification will be triggered when this condition is satisfied
   * @return A handle to the newly created PropertyNotification
   */
  PropertyNotification AddPropertyNotification(Property::Index          index,
                                               const PropertyCondition& condition);

  /**
   * @brief Adds a property notification to this object.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the property
   * @param[in] componentIndex Index to the component of a complex property such as a Vector
   * @param[in] condition The notification will be triggered when this condition is satisfied
   * @return A handle to the newly created PropertyNotification
   */
  PropertyNotification AddPropertyNotification(Property::Index          index,
                                               int                      componentIndex,
                                               const PropertyCondition& condition);

  /**
   * @brief Removes a property notification from this object.
   *
   * @SINCE_1_0.0
   * @param[in] propertyNotification The propertyNotification to be removed
   */
  void RemovePropertyNotification(Dali::PropertyNotification propertyNotification);

  /**
   * @brief Removes all property notifications from this object.
   * @SINCE_1_0.0
   */
  void RemovePropertyNotifications();

  // Constraints

  /**
   * @brief Removes all constraints from an Object.
   *
   * @SINCE_1_0.0
   * @pre The object has been initialized.
   */
  void RemoveConstraints();

  /**
   * @brief Removes all the constraint from the Object with a matching tag.
   *
   * @SINCE_1_0.0
   * @param[in] tag The tag of the constraints which will be removed
   * @pre The Object has been initialized.
   */
  void RemoveConstraints(uint32_t tag);

  /**
   * @brief Index operator, using integer lookup.
   *
   * Returns an object that can be assigned to or cast from, enabling
   * the indexed property to be either read or written.
   *
   * @param[in] index The index of the property to access.
   * @return indirect value. Should have shorter scope than the handle
   */
  IndirectValue operator[](Property::Index index);

  /**
   * @brief Index operator, using name lookup.
   *
   * Returns an object that can be assigned to or cast from, enabling
   * the named property to be either read or written.
   *
   * @param[in] name The name of the property to access.
   * @return indirect value. Should have shorter scope than the handle
   */
  IndirectValue operator[](const std::string& name);

public: // Signals
  /**
   * @brief Get a signal when a property is set on this object through the API (i.e. not when animating)
   *
   * @SINCE_1_9.27
   * @return The signal to attach a connection to.
   */
  PropertySetSignalType& PropertySetSignal();
};

/**
 * @brief This namespace provides a convenient function to create an object with a custom "weight" property.
 * @SINCE_1_0.0
 */
namespace WeightObject
{
DALI_CORE_API extern const Property::Index WEIGHT; ///< name "weight", type FLOAT

/**
 * @brief Convenience function to create an object with a custom "weight" property.
 *
 * @SINCE_1_0.0
 * @return A handle to a newly allocated object
 */
DALI_CORE_API Handle New();

} // namespace WeightObject

/**
 * @}
 */
} // namespace Dali

#endif // DALI_HANDLE_H
