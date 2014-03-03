#ifndef __DALI_BASE_HANDLE_H__
#define __DALI_BASE_HANDLE_H__

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
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/signals/functor-delegate.h>

namespace Dali DALI_IMPORT_API
{

class BaseObject;
class ConnectionTrackerInterface;

/**
 * Dali::BaseHandle is a handle to an internal Dali resource.
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
class BaseHandle
{
public:

  // Used for null pointer assignment below
  class NullType
  {
    NullType() { }
  };

  /**
   * This constructor is used by Dali New() methods.
   * @param [in] handle A pointer to a newly allocated Dali resource
   */
  BaseHandle(Dali::BaseObject* handle);

  /**
   * This constructor provides an uninitialized Dali::BaseHandle.
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
   * Dali::BaseHandle is intended as a base class
   */
  virtual ~BaseHandle();

  /**
   * This copy constructor is required for (smart) pointer semantics
   * @param [in] handle A reference to the copied handle
   */
  BaseHandle(const BaseHandle& handle);

  /**
   * This assignment operator is required for (smart) pointer semantics
   * It makes this handle use the same BaseObject as the copied handle
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this handle
   */
  BaseHandle& operator=(const BaseHandle& rhs);

  /**
   * This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  BaseHandle& operator=(NullType* rhs);

  /**
   * Connects a void() functor to a specified signal.
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
   * Perform action on this object with the given action name and attributes.
   * @param [in] actionName The command for the action.
   * @param [in] attributes The list of attributes for the action.
   * @return The action is performed by the object or not.
   */
  bool DoAction(const std::string& actionName, const std::vector<Property::Value>& attributes);

  /**
   * Returns the type name for the Handle.
   * @return The type name.
   */
  std::string GetTypeName();

public:

  // BaseHandle accessors

  /**
   * Retrieve the internal Dali resource.
   * This is useful for checking the reference count of the internal resource.
   * This method will assert, if the Dali::BaseHandle has not been initialized.
   * @return The BaseObject which is referenced by the BaseHandle.
   */
  BaseObject& GetBaseObject();

  /**
   * Retrieve the internal Dali resource.
   * This is useful for checking the reference count of the internal resource.
   * This method will assert, if the Dali::BaseHandle has not been initialized.
   * @return The BaseObject which is referenced by the BaseHandle.
   */
  const BaseObject& GetBaseObject() const;

  /**
   * Resets the handle.
   * If no other handle copies exist, the internal Dali resouce will be deleted.
   * Calling this is not required i.e. it will happen automatically when a Dali::BaseHandle is destroyed.
   */
  void Reset();

  // BaseHandle comparisons - This is a variation of the safe bool idiom

  /**
   * Pointer-to-member type. Objects can be implicitly converted to this for validity checks.
   */
  typedef void (BaseHandle::*BooleanType)() const;

  /**
   * Converts an handle to a BooleanType.
   * This is useful for checking whether the handle is empty.
   */
  operator BooleanType() const;

  /**
   * Equality operator overload.
   * @param [in] rhs A reference to the compared handle.
   * @return true if the handle handles point to the same Dali resource, or if both are NULL.
   */
  bool operator==(const BaseHandle& rhs) const;

  /**
   * Inequality operator overload.
   * @param [in] rhs A reference to the compared handle.
   * @return true if the handle handles point to the different Dali resources.
   */
  bool operator!=(const BaseHandle& rhs) const;

  /**
   * Get the reference counted object pointer.
   * @return A pointer to the reference counted object.
   */
  Dali::RefObject* GetObjectPtr() const;

private:

  /**
   * Not intended for application developers.
   * @param [in] connectionTracker A connection tracker which can be used to disconnect.
   * @param [in] signalName Name of the signal to connect to.
   * @param [in] functorDelegate A newly allocatated functor delegate (takes ownership).
   * @return True if the signal was available.
   */
  bool DoConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functorDelegate );

  /**
   * Used by the safe bool idiom.
   */
  void ThisIsSaferThanReturningVoidStar() const {}

private:

  IntrusivePtr<Dali::RefObject> mObjectHandle;
};

/**
 * Template wrapper to downcast an base object handle to derived class handle
 * @pre The BaseHandle has been initialized.
 * @param handle to a base object
 * @return handle pointer to either a valid deriving handle or an uninitialized handle
 */
template< class T >
T DownCast( BaseHandle handle )
{
  return T::DownCast( handle );
}

// See also BaseHandle::BooleanType() conversion

template <typename T>
bool operator==(const BaseHandle& lhs, const T& rhs)
{
  // We depart from the safe bool idiom to allow Dali::BaseHandle derived classes to be compared
  return lhs == static_cast<const BaseHandle&>(rhs);
}

template <typename T>
bool operator!=(const BaseHandle& lhs, const T& rhs)
{
  // We depart from the safe bool idiom to allow Dali::BaseHandle derived classes to be compared
  return lhs != static_cast<const BaseHandle&>(rhs);
}

// More Operators

bool operator<(const BaseHandle& lhs, const BaseHandle& rhs);

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_BASE_HANDLE_H__
