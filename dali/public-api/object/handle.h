#ifndef __DALI_HANDLE_H__
#define __DALI_HANDLE_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property-notification-declarations.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
/**
 * @addtogroup dali-core-object
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
 */
class DALI_IMPORT_API Handle : public BaseHandle
{
public:

  /**
   * @brief An Handle's capabilities can be queried using Handle::Supports()
   */
  enum Capability
  {
    /**
     * @brief Some objects support dynamic property creation at run-time.
     *
     * New properties are registered by calling RegisterProperty() with an unused property name.
     */
    DYNAMIC_PROPERTIES = 0x01,
  };

public:

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] handle A pointer to a newly allocated Dali resource
   */
  Handle( Dali::Internal::Object* handle );

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
   */
  Handle();

  /**
   * @brief Create a new object.
   *
   * @return A handle to a newly allocated object.
   */
  static Handle New();

  /**
   * @brief Dali::Handle is intended as a base class
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Handle();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Handle( const Handle& handle );

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Handle& operator=( const Handle& rhs );

  /**
   * @brief Downcast to a handle.
   *
   * If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle or an uninitialized handle
   */
  static Handle DownCast( BaseHandle handle );

  /**
   * @brief Query whether an handle supports a given capability.
   *
   * @param[in] capability The queried capability.
   * @return True if the capability is supported.
   */
  bool Supports( Capability capability ) const;

  // Properties

  /**
   * @brief Query how many properties are provided by an handle.
   *
   * This may vary between instances of a class, if dynamic properties are supported.
   * @return The number of properties.
   */
  unsigned int GetPropertyCount() const;

  /**
   * @brief Query the name of a property.
   *
   * @param [in] index The index of the property.
   * @return The name of the property.
   */
  std::string GetPropertyName( Property::Index index ) const;

  /**
   * @brief Query the index of a property.
   * Returns the first property index that matches the given name exactly.
   *
   * @param [in] name The name of the property.
   * @return The index of the property, or Property::INVALID_INDEX if no property exists with the given name.
   */
  Property::Index GetPropertyIndex( const std::string& name ) const;

  /**
   * @brief Query whether a property can be set using SetProperty().
   *
   * @pre Property::INVALID_INDEX < index.
   * @param [in] index The index of the property.
   * @return True if the property is writable.
   */
  bool IsPropertyWritable( Property::Index index ) const;

  /**
   * @brief Query whether a writable property can be the target of an animation or constraint.
   *
   * @param [in] index The index of the property.
   * @return True if the property is animatable.
   */
  bool IsPropertyAnimatable( Property::Index index ) const;

  /**
   * @brief Query whether a property can be used as in input to a constraint.
   *
   * @param [in] index The index of the property.
   * @return True if the property can be used as a constraint input.
   */
  bool IsPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @brief Query the type of a property.
   *
   * @param [in] index The index of the property.
   * @return The type of the property.
   */
  Property::Type GetPropertyType( Property::Index index ) const;

  /**
   * @brief Set the value of an existing property.
   *
   * Property should be write-able. Setting a read-only property is a no-op.
   * @pre The property types match i.e. propertyValue.GetType() is equal to GetPropertyType(index).
   * @param [in] index The index of the property.
   * @param [in] propertyValue The new value of the property.
   */
  void SetProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @brief Register a new animatable property.
   *
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
   * @param [in] name The name of the property.
   * @param [in] propertyValue The new value of the property.
   * @return The index of the property or Property::INVALID_INDEX if registration failed
   */
  Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue );

  /**
   * @brief Register a new property.
   *
   * Properties can be set as non animatable using property attributes.
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
   * @param [in] name The name of the property.
   * @param [in] propertyValue The new value of the property.
   * @param [in] accessMode The property access mode (writable, animatable etc).
   * @return The index of the property
   */
  Property::Index RegisterProperty( const std::string& name, const Property::Value& propertyValue, Property::AccessMode accessMode );

  /**
   * @brief Retrieve a property value.
   *
   * @param [in] index The index of the property.
   * @return The property value.
   */
  Property::Value GetProperty( Property::Index index ) const;

  /**
   * @brief Convenience function for obtaining a property of a known type.
   *
   * @pre The property types match i.e. PropertyTypes::Get<T>() is equal to GetPropertyType(index).
   * @param [in] index The index of the property.
   * @return The property value.
   */
  template <typename T>
  T GetProperty( Property::Index index ) const
  {
    Property::Value value = GetProperty(index);

    return T( value.Get<T>() );
  }

  /**
   * @brief Retrieve all the property indices for this object (including custom properties).
   *
   * @param[out] indices A container of property indices for this object.
   * @note the added container is cleared
   */
  void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @brief Add a property notification to this object.
   *
   * @param [in] index The index of the property.
   * @param [in] condition The notification will be triggered when this condition is satisfied.
   *
   * @return A handle to the newly created PropertyNotification
   */
  PropertyNotification AddPropertyNotification( Property::Index index,
                                                const PropertyCondition& condition );

  /**
   * @brief Add a property notification to this object.
   *
   * @param [in] index The index of the property.
   * @param [in] componentIndex Index to the component of a complex property such as a Vector
   * @param [in] condition The notification will be triggered when this condition is satisfied.
   *
   * @return A handle to the newly created PropertyNotification
   */
  PropertyNotification AddPropertyNotification( Property::Index index,
                                                int componentIndex,
                                                const PropertyCondition& condition );

  /**
   * @brief Remove a property notification from this object.
   *
   * @param [in] propertyNotification The propertyNotification to be removed.
   */
  void RemovePropertyNotification( Dali::PropertyNotification propertyNotification );

  /**
   * @brief Remove all property notifications from this object.
   */
  void RemovePropertyNotifications();

  // Constraints

  /**
   * @brief Remove all constraints from an Object.
   *
   * @pre The object has been initialized.
   */
  void RemoveConstraints();

  /**
   * @brief Remove all the constraint from the Object with a matching tag.
   *
   * @pre The Object has been initialized.
   * @param[in] tag The tag of the constraints which will be removed
   */
  void RemoveConstraints( unsigned int tag );

};

namespace WeightObject
{

DALI_IMPORT_API extern const Property::Index WEIGHT; ///< name "weight", type FLOAT

/**
 * @brief Convenience function to create an object with a custom "weight" property.
 *
 * @return A handle to a newly allocated object.
 */
DALI_IMPORT_API Handle New();

} // namespace WeightObject

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_HANDLE_H__
