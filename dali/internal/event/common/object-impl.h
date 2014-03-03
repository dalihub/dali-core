#ifndef __DALI_INTERNAL_OBJECT_H__
#define __DALI_INTERNAL_OBJECT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

class PropertyNotification;

namespace Internal
{
class Handle;

class PropertyCondition;

/**
 * A base class for objects which optionally provide properties.
 * The concrete derived class is responsible for implementing the property system methods.
 * Classes may derive from Dali::BaseObject, until any properties are required.
 */
class Object : public Dali::BaseObject
{
public:

  typedef Dali::Handle::Capability Capability;

  /**
   * @copydoc Dali::Handle::Supports()
   */
  virtual bool Supports(Capability capability) const = 0;

  /**
   * @copydoc Dali::Handle::GetPropertyCount()
   */
  virtual unsigned int GetPropertyCount() const = 0;

  /**
   * @copydoc Dali::Handle::GetPropertyName()
   */
  virtual const std::string& GetPropertyName(Property::Index index) const = 0;

  /**
   * @copydoc Dali::Handle::GetPropertyIndex()
   */
  virtual Property::Index GetPropertyIndex(const std::string& name) const = 0;

  /**
   * @copydoc Dali::Handle::IsPropertyWritable()
   */
  virtual bool IsPropertyWritable(Property::Index index) const = 0;

  /**
   * @copydoc Dali::Handle::IsPropertyAnimatable()
   */
  virtual bool IsPropertyAnimatable(Property::Index index) const = 0;

  /**
   * @copydoc Dali::Handle::GetPropertyType()
   */
  virtual Property::Type GetPropertyType(Property::Index index) const = 0;

  /**
   * @copydoc Dali::Handle::SetProperty()
   */
  virtual void SetProperty(Property::Index index, const Property::Value& propertyValue) = 0;

  /**
   * @copydoc Dali::Handle::GetProperty()
   */
  virtual Property::Value GetProperty(Property::Index index) const = 0;

  /**
   * @copydoc Dali::Handle::RegisterProperty()
   */
  virtual Property::Index RegisterProperty(std::string name, const Property::Value& propertyValue) = 0;

  /**
   * @copydoc Dali::Handle::RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode)
   */
  virtual Property::Index RegisterProperty(std::string name, const Property::Value& propertyValue, Property::AccessMode accessMode) = 0;

  /**
   * @copydoc Dali::Handle::AddPropertyNotification()
   */
  virtual Dali::PropertyNotification AddPropertyNotification(Property::Index index,
                                                             const Dali::PropertyCondition& condition) = 0;

  /**
   * @copydoc Dali::Handle::AddPropertyNotification(Property::Index index, const PropertyCondition& condition, PropertyNotifyCallbackType callback)
   */
  virtual Dali::PropertyNotification AddPropertyNotification(Property::Index index,
                                                             const Dali::PropertyCondition& condition,
                                                             PropertyNotifyCallbackType callback) = 0;

  /**
   * @copydoc Dali::Handle::RemovePropertyNotification()
   */
  virtual void RemovePropertyNotification(Dali::PropertyNotification propertyNotification) = 0;

  /**
   * @copydoc Dali::Handle::RemovePropertyNotifications()
   */
  virtual void RemovePropertyNotifications() = 0;


protected:

  /**
   * Default constructor.
   */
  Object()
  {
  }

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Object()
  {
  }

private:

  // Not implemented
  Object(const Object& rhs);

  // Not implemented
  Object& operator=(const Object& rhs);
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Object& GetImplementation(Dali::Handle& object)
{
  DALI_ASSERT_ALWAYS( object && "Object handle is empty" );

  BaseObject& handle = object.GetBaseObject();

  return static_cast<Internal::Object&>(handle);
}

inline const Internal::Object& GetImplementation(const Dali::Handle& object)
{
  DALI_ASSERT_ALWAYS( object && "Object handle is empty" );

  const BaseObject& handle = object.GetBaseObject();

  return static_cast<const Internal::Object&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_OBJECT_H__

