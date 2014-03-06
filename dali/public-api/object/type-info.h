#ifndef __DALI_TYPE_INFO_H__
#define __DALI_TYPE_INFO_H__

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
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

class ConnectionTrackerInterface;
class FunctorDelegate;

namespace Internal DALI_INTERNAL
{
  class TypeInfo;
};

/**
 * TypeInfo class for instantiation of registered types and introspection of
 * their actions and signals.
 * See TypeRegistry for methods of type registration and TypeInfo retrieval.
 */
class TypeInfo : public BaseHandle
{
public:
  typedef BaseHandle (*CreateFunction)();
  typedef bool (*ActionFunction)(BaseObject*, const std::string&, const std::vector<Property::Value>&);

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  typedef bool (*SignalConnectorFunctionV2)(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

  /**
   * Callback to set an event-thread only property @see PropertyRegistration
   * @param[in] object The object whose property should be set.
   * @param[in] index The index of the property being set.
   * @param[in] value The new value of the property for the object specified.
   */
  typedef void (*SetPropertyFunction)( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Callback to get the value of an event-thread only property @see PropertyRegistration
   * @param[in] object The object whose property value is required.
   * @param[in] index The index of the property required.
   * @return The current value of the property for the object specified.
   */
  typedef Property::Value (*GetPropertyFunction)( BaseObject* object, Property::Index index );

  typedef std::vector<std::string> NameContainer;

  /**
   * Allows the creation of an empty TypeInfo handle.
   */
  TypeInfo();

  /**
   * destructor.
   */
  virtual ~TypeInfo();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Retrieve the type name for this type
   * @return string name
   */
  const std::string& GetName();

  /**
   * Retrieve the base type name for this type
   * @return string of base name
   */
  const std::string& GetBaseName();

  /**
   * Create an object from this type
   * @return the BaseHandle for the newly created object
   */
  BaseHandle CreateInstance();

  /**
   * Retrieve the creator function for this type
   * @return the creator function
   */
  CreateFunction GetCreator();

  /**
   * Retrieve the actions for this type
   * @return Container of action names
   */
  NameContainer GetActions();

  /**
   * Retrieve the signals for this type
   * @return Container of signal names
   */
  NameContainer GetSignals();

  // Properties

  /**
   * Retrieve all the property indices for this type
   * @param[out] indices Container of property indices
   * @note The container will be cleared
   */
  void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * Given a property index, retrieve the property name associated with it.
   * @param[in] index The property index.
   * @return The name of the property at the given index.
   */
  const std::string& GetPropertyName( Property::Index index ) const;

public: // Not intended for application developers

  /**
   * This constructor is used by Dali Get() method
   * @param [in] typeInfo A pointer to a Dali resource
   */
  explicit DALI_INTERNAL TypeInfo(Internal::TypeInfo* typeInfo);

};

} // namespace Dali

#endif // __DALI_TYPE_INFO_H__
