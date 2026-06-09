#ifndef DALI_BASE_HANDLE_H
#define DALI_BASE_HANDLE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/invoke-method.h>
#include <dali/public-api/object/property-types.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/signals/functor-delegate.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

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
 * @SINCE_1_0.0
 */
class DALI_CORE_API BaseHandle
{
public:
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @SINCE_1_0.0
   * @param[in] handle A pointer to a newly allocated Dali resource
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
   * @SINCE_1_0.0
   */
  BaseHandle();

  /**
   * @brief Dali::BaseHandle is intended as a base class.
   *
   * This is non-virtual since derived BaseHandle types must not contain data.
   * @SINCE_1_0.0
   */
  ~BaseHandle();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  BaseHandle(const BaseHandle& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * It makes this handle use the same BaseObject as the copied handle
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this handle
   */
  BaseHandle& operator=(const BaseHandle& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   */
  BaseHandle(BaseHandle&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  BaseHandle& operator=(BaseHandle&& rhs) noexcept;

  /**
   * @brief Connects a void() functor to a specified signal.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] signalName Name of the signal to connect to
   * @param[in] functor The functor to move
   * @return True if the signal was available
   * @pre The signal must be available in this object.
   */
  template<class T>
  bool ConnectSignal(ConnectionTrackerInterface* connectionTracker, const Dali::StringView& signalName, T&& functor)
  {
    return DoConnectSignal(connectionTracker, signalName, FunctorDelegate::New(std::move(functor)));
  }

  /**
   * @brief Performs an action on this object with the given action name and attributes.
   *
   * Usage example: -
   * @code
   * BaseHandle handle = SomeClass::New(); // Initialized with New() method
   *
   * Property::Map attributes; // Type is Property Map
   * handle.DoAction("show", attributes);
   * @endcode
   * @SINCE_1_0.0
   * @param[in] actionName The command for the action
   * @param[in] attributes The list of attributes for the action
   * @return The action is performed by the object or not
   */
  bool DoAction(const Dali::StringView& actionName, const Property::Map& attributes);

  /**
   * @brief Invokes a registered method on this object with the given method name and arguments.
   *
   * InvokeMethod dispatches by name through the DALi TypeRegistry. Methods are
   * registered by generated wrappers for supported public API methods in
   * exported DALi BaseHandle-derived classes. The target object's runtime type
   * must have TypeRegistry information.
   *
   * At build time, DALi's invoke-method generator scans classes marked with the
   * library export macros such as DALI_CORE_API, DALI_ADAPTOR_API,
   * DALI_TOOLKIT_API, or DALI_UI_API. It emits wrappers only for public
   * non-static member functions on BaseHandle-derived classes that can be called
   * safely through the TypeRegistry.
   * Constructors, destructors, operators, New(), DownCast(), GetImplementation(),
   * signal accessors, raw pointer APIs, callback/functor APIs, std public types,
   * output parameters, non-const reference parameters, and APIs using incomplete
   * forward-declared value types are intentionally excluded.
   *
   * The target object must be a valid Dali::BaseHandle whose runtime object has
   * TypeRegistry information and a registered invokable method matching
   * @p methodName. The wrapper validates the argument count and argument types
   * before calling the underlying public API method.
   *
   * Arguments are passed as Dali::Any values in declaration order. If the
   * invoked method returns a value, the wrapper stores it in @p result as a
   * Dali::Any. For void methods, @p result is reset to an empty Dali::Any.
   *
   * Overloaded invokable methods are resolved by argument count and exact
   * Dali::Any value types. Methods with trailing default parameters may be
   * invoked with those trailing arguments omitted, provided the generated
   * overload set remains unambiguous.
   *
   * InvokeMethod does not perform normal C++ implicit conversions. For example,
   * if a method expects a float, the argument must be stored in Dali::Any as a
   * float, not as an int. When a method is not registered, the argument count is
   * wrong, or an argument type does not match, the generated wrapper reports the
   * failure through DALI_LOG_ERROR and returns false.
   *
   * InvokeMethod dispatch starts from the object's runtime TypeRegistry type
   * and then falls back to registered base types. For virtual public API
   * methods this can match normal override behavior. For non-virtual methods
   * hidden by derived classes, InvokeMethod follows runtime registry dispatch
   * and may not match C++ static dispatch through a base-typed handle.
   *
   * @note This API is intended for ABI-tolerant dynamic dispatch of selected
   * public API methods. It is not a general C++ reflection system, and it only
   * works for methods that have generated TypeRegistry wrappers.
   *
   * @param[in] methodName The name of the registered method
   * @param[in] arguments The arguments for the method
   * @param[out] result The method result, if any
   * @return True if the method was found and invoked
   */
  bool InvokeMethod(const Dali::StringView& methodName, const InvokeArguments& arguments, InvokeResult& result);

  /**
   * @brief Returns the type name for the Handle.
   *
   * Will return an empty string if the typename does not exist. This will happen for types that
   * have not registered with type-registry.
   *
   * @SINCE_1_0.0
   * @return The type name. Empty string if the typename does not exist
   */
  Dali::String GetTypeName() const;

  /**
   * @brief Returns the type info for the Handle.
   *
   * @SINCE_1_0.0
   * @param[out] info The type information
   * @return true if the type info exists
   */
  bool GetTypeInfo(Dali::TypeInfo& info) const;

public:
  // BaseHandle accessors

  /**
   * @brief Retrieves the internal Dali resource.
   *
   * This is useful for checking the reference count of the internal resource.
   * This method will not check the validity of the handle so the caller is expected to check it by using if (handle).
   * @SINCE_1_0.0
   * @return The BaseObject which is referenced by the BaseHandle
   */
  BaseObject& GetBaseObject();

  /**
   * @brief Retrieves the internal Dali resource.
   *
   * This is useful for checking the reference count of the internal resource.
   * This method will not check the validity of the handle so the caller is expected to check it by using if (handle).
   * @SINCE_1_0.0
   * @return The BaseObject which is referenced by the BaseHandle
   */
  const BaseObject& GetBaseObject() const;

  /**
   * @brief Resets the handle.
   *
   * If no other handle copies exist, the internal Dali resource will be deleted.
   * Calling this is not required i.e. it will happen automatically when a Dali::BaseHandle is destroyed.
   * @SINCE_1_0.0
   */
  void Reset();

  // BaseHandle comparisons

  /**
   * @brief Converts an handle to a bool.
   *
   * This is useful for checking whether the handle is empty.
   * @SINCE_1_0.0
   */
  explicit operator bool() const;

  /**
   * @brief Equality operator overload.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the compared handle
   * @return True if the handle handles point to the same Dali resource, or if both are NULL
   */
  bool operator==(const BaseHandle& rhs) const;

  /**
   * @brief Inequality operator overload.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the compared handle
   * @return True if the handle handles point to the different Dali resources
   */
  bool operator!=(const BaseHandle& rhs) const;

  /**
   * @brief Gets the reference counted object pointer.
   *
   * @SINCE_1_0.0
   * @return A pointer to the reference counted object
   */
  Dali::RefObject* GetObjectPtr() const;

private:
  /**
   * @brief Not intended for application developers.
   *
   * @SINCE_1_0.0
   * @param[in] connectionTracker A connection tracker which can be used to disconnect
   * @param[in] signalName Name of the signal to connect to
   * @param[in] functorDelegate A newly allocated functor delegate (takes ownership)
   * @return True if the signal was available
   */
  bool DoConnectSignal(ConnectionTrackerInterface* connectionTracker, const Dali::StringView& signalName, FunctorDelegate* functorDelegate);

private:
  IntrusivePtr<Dali::RefObject> mObjectHandle; ///< Object this handle points at.
};

/**
 * @brief Template wrapper to downcast a base object handle to derived class handle.
 *
 * @SINCE_1_0.0
 * @param[in] handle Handle to a base object
 * @return Handle pointer to either a valid deriving handle or an uninitialized handle
 * @pre The BaseHandle has been initialized.
 */
template<class T>
inline T DownCast(BaseHandle handle)
{
  return T::DownCast(handle);
}

/**
 * @brief Equality operator.
 * @SINCE_1_0.0
 * @param[in] lhs A reference to compare
 * @param[in] rhs A reference to compare to
 * @return True if the handle handles point to the same Dali resource, or if both are NULL
 */
template<typename T>
inline bool operator==(const BaseHandle& lhs, const T& rhs)
{
  return lhs == static_cast<const BaseHandle&>(rhs);
}

/**
 * @brief Equality operator.
 * @SINCE_1_0.0
 * @param[in] lhs A reference to compare
 * @param[in] rhs A reference to compare to
 * @return True if the handle handles point to the different Dali resources
 */
template<typename T>
inline bool operator!=(const BaseHandle& lhs, const T& rhs)
{
  return lhs != static_cast<const BaseHandle&>(rhs);
}

/**
 * @brief Less than operator.
 * @SINCE_1_0.0
 * @param[in] lhs A reference to compare
 * @param[in] rhs A reference to compare to
 * @return True if lhs less than rhs
 */
inline bool operator<(const BaseHandle& lhs, const BaseHandle& rhs)
{
  return lhs.GetObjectPtr() < rhs.GetObjectPtr();
}

/**
 * @}
 */
} // namespace Dali

#endif // DALI_BASE_HANDLE_H
