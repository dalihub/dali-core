#ifndef __DALI_BASE_HANDLE_H__
#define __DALI_BASE_HANDLE_H__

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
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/signals/functor-delegate.h>

namespace Dali
{

class BaseObject;
class ConnectionTrackerInterface;
class TypeInfo;

/**
 * @brief Dali::BaseHandle is a handle to an internal Dali resource.
 *
 * Each Dali handle consists of a single private pointer, and a set of non-virtual forwarding functions.
 * This hides the internal implementation, so it may be modified without affecting the public interface.
 *
 * Dali handles have implicit smart-pointer semantics.
 * This avoids the need to match resource allocation methods like new/delete (the RAII idiom).
 *
 * Dali handles can be copied by value.
 * When a Dali handle is copied, both the copy and original will point to the same Dali resource.
 *
 * The internal Dali resources are reference counted. copying a Dali handle will increase the reference count.
 * A resource will not be deleted until all its Dali::BaseHandle handles are destroyed, or reset.
 *
 */
class DALI_IMPORT_API BaseHandle
{
public:

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] handle A pointer to a newly allocated Dali resource
   */
  BaseHandle(Dali::BaseObject* handle);

  /**
   * @brief This constructor provides an uninitialized Dali::BaseHandle.
   *
   * This should be initialized with a Dali New() method before use.
   * Methods called on an uninitialized Dali::BaseHandle will assert.
   * @code
   * BaseHandle handle; // uninitialized
   * handle.SomeMethod(); // unsafe! This will assert
   *
   * handle = SomeClass::New(); // now initialized
   * handle.SomeMethod(); // safe
   * @endcode
   */
  BaseHandle();

  /**
   * @brief Dali::BaseHandle is intended as a base class
   *
   * This is non-virtual since derived BaseHandle types must not contain data.
   */
  ~BaseHandle();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  BaseHandle(const BaseHandle& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * It makes this handle use the same BaseObject as the copied handle
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this handle
   */
  BaseHandle& operator=(const BaseHandle& rhs);

  /**
   * @brief Connects a void() functor to a specified signal.
   *
   * @pre The signal must be available in this object.
   * @param [in] connectionTracker A connection tracker which can be used to disconnect.
   * @param [in] signalName Name of the signal to connect to.
   * @param [in] functor The functor to copy.
   * @return True if the signal was available.
   */
  template <class T>
  bool ConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, const T& functor )
  {
    return DoConnectSignal( connectionTracker, signalName, FunctorDelegate::New( functor ) );
  }

  /**
   * @brief Perform action on this object with the given action name and attributes.
   *
   * @param [in] actionName The command for the action.
   * @param [in] attributes The list of attributes for the action.
   * @return The action is performed by the object or not.
   */
  bool DoAction(const std::string& actionName, const Property::Map& attributes);

  /**
   * @brief Returns the type name for the Handle.
   *
   * Will return an empty string if the typename does not exist. This will happen for types that
   * have not registered with type-registry.
   *
   * @return The type name. Empty string if the typename does not exist.
   */
  const std::string& GetTypeName() const;

  /**
   * @brief Returns the type info for the Handle.
   *
   * @return The type info.
   */
  bool GetTypeInfo(Dali::TypeInfo& info) const;

public:

  // BaseHandle accessors

  /**
   * @brief Retrieve the internal Dali resource.
   *
   * This is useful for checking the reference count of the internal resource.
   * This method will not check the validity of the handle so the caller is expected to check it by using if (handle)
   * @return The BaseObject which is referenced by the BaseHandle.
   */
  BaseObject& GetBaseObject();

  /**
   * @brief Retrieve the internal Dali resource.
   *
   * This is useful for checking the reference count of the internal resource.
   * This method will not check the validity of the handle so the caller is expected to check it by using if (handle)
   * @return The BaseObject which is referenced by the BaseHandle.
   */
  const BaseObject& GetBaseObject() const;

  /**
   * @brief Resets the handle.
   *
   * If no other handle copies exist, the internal Dali resouce will be deleted.
   * Calling this is not required i.e. it will happen automatically when a Dali::BaseHandle is destroyed.
   */
  void Reset();

  // BaseHandle comparisons - This is a variation of the safe bool idiom

  /**
   * @brief Pointer-to-member type.
   * Objects can be implicitly converted to this for validity checks.
   */
  typedef void (BaseHandle::*BooleanType)() const;

  /**
   * @brief Converts an handle to a BooleanType.
   *
   * This is useful for checking whether the handle is empty.
   */
  operator BooleanType() const;

  /**
   * @brief Equality operator overload.
   *
   * @param [in] rhs A reference to the compared handle.
   * @return true if the handle handles point to the same Dali resource, or if both are NULL.
   */
  bool operator==(const BaseHandle& rhs) const;

  /**
   * @brief Inequality operator overload.
   *
   * @param [in] rhs A reference to the compared handle.
   * @return true if the handle handles point to the different Dali resources.
   */
  bool operator!=(const BaseHandle& rhs) const;

  /**
   * @brief Get the reference counted object pointer.
   *
   * @return A pointer to the reference counted object.
   */
  Dali::RefObject* GetObjectPtr() const;

private:

  /**
   * @brief Not intended for application developers.
   *
   * @param [in] connectionTracker A connection tracker which can be used to disconnect.
   * @param [in] signalName Name of the signal to connect to.
   * @param [in] functorDelegate A newly allocatated functor delegate (takes ownership).
   * @return True if the signal was available.
   */
  bool DoConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functorDelegate );

protected:

  /**
   * @brief Used by the safe bool idiom.
   *
   */
  void ThisIsSaferThanReturningVoidStar() const {}

private:

  IntrusivePtr<Dali::RefObject> mObjectHandle; ///< Object this handle points at.

};

/**
 * @brief Template wrapper to downcast an base object handle to derived class handle.
 *
 * @pre The BaseHandle has been initialized.
 * @param handle to a base object
 * @return handle pointer to either a valid deriving handle or an uninitialized handle
 */
template< class T >
inline T DownCast( BaseHandle handle )
{
  return T::DownCast( handle );
}

// See also BaseHandle::BooleanType() conversion

/**
 * @brief Equality operator
 */
template <typename T>
inline bool operator==(const BaseHandle& lhs, const T& rhs)
{
  // We depart from the safe bool idiom to allow Dali::BaseHandle derived classes to be compared
  return lhs == static_cast<const BaseHandle&>(rhs);
}

/**
 * @brief Equality operator
 */
template <typename T>
inline bool operator!=(const BaseHandle& lhs, const T& rhs)
{
  // We depart from the safe bool idiom to allow Dali::BaseHandle derived classes to be compared
  return lhs != static_cast<const BaseHandle&>(rhs);
}

/**
 * @brief Less than operator
 */
inline bool operator<(const BaseHandle& lhs, const BaseHandle& rhs)
{
  return lhs.GetObjectPtr() < rhs.GetObjectPtr();
}

} // namespace Dali

#endif // __DALI_BASE_HANDLE_H__
