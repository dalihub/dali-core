#ifndef __DALI_TYPE_INFO_H__
#define __DALI_TYPE_INFO_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

class ConnectionTrackerInterface;
class FunctorDelegate;

namespace Internal DALI_INTERNAL
{
  class TypeInfo;
};

/**
 * @brief TypeInfo class for instantiation of registered types and introspection of
 * their actions and signals.
 *
 * See TypeRegistry for methods of type registration and TypeInfo retrieval.
 * @SINCE_1_0.0
 */
class DALI_CORE_API TypeInfo : public BaseHandle
{
public:
  typedef BaseHandle (*CreateFunction)(); ///< Function signature for creating an instance of the associated object type. @SINCE_1_0.0

  typedef bool (*ActionFunction)(BaseObject*, const std::string&, const Property::Map&); ///< Function signature for creating scriptable actions @SINCE_1_0.0

  /**
   * @brief Connects a callback function with the object's signals.
   *
   * @SINCE_1_0.0
   * @param[in] object The object providing the signal
   * @param[in] tracker Used to disconnect the signal
   * @param[in] signalName The signal to connect to
   * @param[in] functor A newly allocated FunctorDelegate
   * @return True if the signal was connected
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  typedef bool (*SignalConnectorFunction)(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

  /**
   * @brief Callback to set an event-thread only property.
   *
   * @SINCE_1_0.0
   * @param[in] object The object whose property should be set
   * @param[in] index The index of the property being set
   * @param[in] value The new value of the property for the object specified
   * @see PropertyRegistration.
   */
  typedef void (*SetPropertyFunction)( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Callback to get the value of an event-thread only property.
   *
   * @SINCE_1_0.0
   * @param[in] object The object whose property value is required
   * @param[in] index The index of the property required
   * @return The current value of the property for the object specified
   * @see PropertyRegistration.
   */
  typedef Property::Value (*GetPropertyFunction)( BaseObject* object, Property::Index index );

  /**
   * @brief Allows the creation of an empty TypeInfo handle.
   * @SINCE_1_0.0
   */
  TypeInfo();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~TypeInfo();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  TypeInfo(const TypeInfo& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  TypeInfo& operator=(const TypeInfo& rhs);

  /**
   * @brief Retrieves the type name for this type.
   *
   * @SINCE_1_0.0
   * @return String name
   */
  const std::string& GetName() const;

  /**
   * @brief Retrieves the base type name for this type.
   *
   * @SINCE_1_0.0
   * @return String of base name
   */
  const std::string& GetBaseName() const;

  /**
   * @brief Creates an object from this type.
   *
   * @SINCE_1_0.0
   * @return The BaseHandle for the newly created object
   */
  BaseHandle CreateInstance() const;

  /**
   * @brief Retrieves the creator function for this type.
   *
   * @SINCE_1_0.0
   * @return The creator function
   */
  CreateFunction GetCreator() const;

  /**
   * @brief Retrieves the number of actions for this type.
   *
   * @SINCE_1_0.0
   * @return The count
   */
  size_t GetActionCount() const;

  /**
   * @brief Retrieves the action name for the index.
   *
   * @SINCE_1_0.0
   * @param[in] index Index to lookup
   * @return Action name or empty string where index is invalid
   */
  std::string GetActionName(size_t index);

  /**
   * @brief Retrieves the number of signals for this type.
   *
   * @SINCE_1_0.0
   * @return The count
   */
  size_t GetSignalCount() const;

  /**
   * @brief Retrieves the signal name for the index.
   *
   * @SINCE_1_0.0
   * @param[in] index Index to lookup
   * @return Signal name or empty string where index is invalid
   */
  std::string GetSignalName(size_t index);

  /**
   * @brief Retrieves the number of event side type registered properties for this type.
   *
   * This count does not include all properties.
   * @SINCE_1_0.0
   * @return The count
   */
  size_t GetPropertyCount() const;

  // Properties

  /**
   * @brief Retrieves all the property indices for this type.
   *
   * @SINCE_1_0.0
   * @param[out] indices Container of property indices
   * @note The container will be cleared
   */
  void GetPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @brief Retrieves all the child property indices for this type.
   *
   * @SINCE_1_3.20
   * @param[out] indices Container of property indices
   * @note The container will be cleared
   */
  void GetChildPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @brief Given a property index, retrieve the property name associated with it.
   *
   * @SINCE_1_0.0
   * @param[in] index The property index
   * @return The name of the property at the given index
   * @exception DaliException If index is not valid.
   *
   */
  const std::string& GetPropertyName( Property::Index index ) const;


  /**
   * @brief Given a child property name, retrieve the property index associated with it,
   *
   * @SINCE_1_3.20
   * @param[in] name The name of the property at the given index,
   * @return The property index or Property::INVALID_INDEX
   */
  Property::Index GetChildPropertyIndex( const std::string& name ) const;

  /**
   * @brief Given a child property index, retrieve the property name associated with it.
   *
   * @SINCE_1_3.20
   * @param[in] index The property index
   * @return The name of the property at the given index, or empty string if it does not exist
   */
  const std::string& GetChildPropertyName( Property::Index index ) const;

  /**
   * @brief Given a child property index, retrieve the property name associated with it.
   *
   * @SINCE_1_3.20
   * @param[in] index The property index
   * @return The name of the property at the given index, or empty string if it does not exist
   */
  Property::Type GetChildPropertyType( Property::Index index ) const;



public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Dali Get() method.
   *
   * @SINCE_1_0.0
   * @param[in] typeInfo A pointer to a Dali resource
   */
  explicit DALI_INTERNAL TypeInfo(Internal::TypeInfo* typeInfo);
  /// @endcond

};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_TYPE_INFO_H__
