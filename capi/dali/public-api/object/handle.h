#ifndef __DALI_HANDLE_H__
#define __DALI_HANDLE_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
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

namespace Dali DALI_IMPORT_API
{

class PropertyNotification;
class PropertyCondition;

namespace Internal DALI_INTERNAL
{
class Object;
}

/**
 * Dali::Handle is a handle to an internal property owning Dali object.
 *
 */
class Handle : public BaseHandle
{
public:

  /**
   * An Handle's capabilities can be queried using Handle::Supports()
   */
  enum Capability
  {
    /**
     * Some objects support dynamic property creation at run-time.
     * New properties are registered by calling RegisterProperty() with an unused property name.
     */
    DYNAMIC_PROPERTIES = 0x01,
  };

public:

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] handle A pointer to a newly allocated Dali resource
   */
  Handle(Dali::Internal::Object* handle);

  /**
   * This constructor provides an uninitialized Dali::Handle.
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
   * Dali::Handle is intended as a base class
   */
  virtual ~Handle();

  /**
   * This copy constructor is required for (smart) pointer semantics
   * @param [in] handle A reference to the copied handle
   */
  Handle(const Handle& handle);

  /**
   * This assignment operator is required for (smart) pointer semantics
   * @param [in] rhs  A reference to the copied handle
   */
  Handle& operator=(const Handle& rhs);

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Downcast to a handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle or an uninitialized handle
   */
  static Handle DownCast( BaseHandle handle );

public:

  /**
   * Query whether an handle supports a given capability.
   * @param[in] capability The queried capability.
   * @return True if the capability is supported.
   */
  bool Supports(Capability capability) const;

  /**
   * Query how many properties are provided by an handle.
   * This may vary between instances of a class, if dynamic properties are supported.
   * @return The number of properties.
   */
  unsigned int GetPropertyCount() const;

  /**
   * Query the name of a property.
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @param [in] index The index of the property.
   * @return The name of the property.
   */
  const std::string& GetPropertyName(Property::Index index) const;

  /**
   * Query the index of a property.
   * @param [in] name The name of the property.
   * @return The index of the property, or Property::INVALID_INDEX if no property exists with the given name.
   */
  Property::Index GetPropertyIndex(std::string name) const;

  /**
   * Query whether a property can be set using SetProperty().
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @param [in] index The index of the property.
   * @return True if the property is writable.
   */
  bool IsPropertyWritable(Property::Index index) const;

  /**
   * Query whether a writable property can be the target of an animation or constraint.
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @param [in] index The index of the property.
   * @return True if the property is animatable.
   */
  bool IsPropertyAnimatable(Property::Index index) const;

  /**
   * Query the type of a property.
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @param [in] index The index of the property.
   * @return The type of the property.
   */
  Property::Type GetPropertyType(Property::Index index) const;

  /**
   * Set the value of an existing property.
   * @pre Handle::IsPropertyWritable(index) returns true.
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @pre The property types match i.e. propertyValue.GetType() is equal to GetPropertyType(index).
   * @param [in] index The index of the property.
   * @param [in] propertyValue The new value of the property.
   */
  void SetProperty(Property::Index index, Property::Value propertyValue);

  /**
   * Register a new property.
   * @pre The handle supports dynamic properties i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.
   * @pre name is unused i.e. GetPropertyIndex(name) returns PropertyIndex::INVALID.
   * @param [in] name The name of the property.
   * @param [in] propertyValue The new value of the property.
   */
  Property::Index RegisterProperty(std::string name, Property::Value propertyValue);

  /**
   * Register a new property.
   * Properties can be set as non animatable using property attributes.
   * @pre The handle supports dynamic properties i.e. Supports(Handle::DYNAMIC_PROPERTIES) returns true.
   * @pre name is unused i.e. GetPropertyIndex(name) returns PropertyIndex::INVALID.
   * @param [in] name The name of the property.
   * @param [in] propertyValue The new value of the property.
   * @param [in] accessMode The property access mode (writable, animatable etc).
   */
  Property::Index RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode);

  /**
   * Retrieve a property value.
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @param [in] index The index of the property.
   * @return The property value.
   */
  Property::Value GetProperty(Property::Index index) const;

  /**
   * Convenience function for obtaining a property of a known type.
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @pre The property types match i.e. PropertyTypes::Get<T>() is equal to GetPropertyType(index).
   * @param [in] index The index of the property.
   * @return The property value.
   */
  template <typename T>
  T GetProperty(Property::Index index) const
  {
    Property::Value value = GetProperty(index);

    return T( value.Get<T>() );
  }

  /**
   * Add a property notification to this object.
   * @pre Property::INVALID_INDEX < index < GetPropertyCount().
   * @param [in] index The index of the property.
   * @param [in] condition The notification will be triggered when this condition is satisfied.
   */
  PropertyNotification AddPropertyNotification(Property::Index index,
                                               const PropertyCondition& condition);

  /**
   * Remove a property notification from this object.
   * @param [in] propertyNotification The propertyNotification to be removed.
   */
  void RemovePropertyNotification(Dali::PropertyNotification propertyNotification);

  /**
   * Remove all property notifications from this object.
   */
  void RemovePropertyNotifications();

};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_HANDLE_H__
