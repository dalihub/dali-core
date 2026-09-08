#ifndef DALI_CALLBACK_H
#define DALI_CALLBACK_H

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

// EXTERNAL INCLUDES
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/functor-delegate.h>

namespace DALI_NAMESPACE
{
/**
 * @addtogroup dali_core_signals
 * @{
 */

class CallbackBase;

/**
 * @brief Callback base class to hold the data for callback function and member function calls.
 * @SINCE_1_0.0
 */
class DALI_CORE_API CallbackBase
{
public:
  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  CallbackBase();

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  virtual ~CallbackBase();

  /**
   * @brief Resets the object pointer so that we know not to call methods of this object any more.
   * @SINCE_1_0.0
   */
  void Reset();

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_9.33
   * @param[in] callback The callback to call
   * @param[in] args parameter pack to pass into the function
   * @return The value from the function
   */
  template<typename R, typename... Args>
  static R ExecuteReturn(CallbackBase& callback, Args... args)
  {
    R returnVal = R();
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if(callback.mImpl.mObjectPointer)
    {
      using Dispatcher      = R (*)(CallbackBase&, Args...);
      Dispatcher dispatcher = reinterpret_cast<Dispatcher>(callback.mImpl.mMemberFunctionDispatcher);
      returnVal             = (*dispatcher)(callback, args...);
    }
    else if(callback.mStaticFunction)
    {
      // convert function type
      using StaticFunction = R (*)(Args...);
      returnVal            = (*(reinterpret_cast<StaticFunction>(callback.mStaticFunction)))(args...);
    }

    return returnVal;
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_9.33
   * @param[in] callback The callback to call
   * @param[in] args parameter pack to pass into the function
   */
  template<typename... Args>
  static void Execute(CallbackBase& callback, Args... args)
  {
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if(callback.mImpl.mObjectPointer)
    {
      using Dispatcher      = void (*)(CallbackBase&, Args...);
      Dispatcher dispatcher = reinterpret_cast<Dispatcher>(callback.mImpl.mMemberFunctionDispatcher);
      (*dispatcher)(callback, args...);
    }
    else if(callback.mStaticFunction)
    {
      // convert function type
      using StaticFunction = void (*)(Args...);
      (*(reinterpret_cast<StaticFunction>(callback.mStaticFunction)))(args...);
    }
  }

public:
  /**
   * @brief Function with static linkage.
   * @SINCE_1_0.0
   */
  using StaticFunction = void (*)();

  /**
   * @brief Constructor for function with static linkage.
   *
   * @SINCE_1_0.0
   * @param[in] function The function to call
   */
  CallbackBase(StaticFunction function);

protected: // Constructors for deriving classes
  /**
   * @brief Member function.
   * @SINCE_1_0.0
   */
  using MemberFunction = void (CallbackBase::*)();

  /**
   * @brief Used to call the correct member function.
   * @SINCE_1_0.0
   */
  using Dispatcher = void (*)(CallbackBase&);

  /**
   * @brief Used to destroy mObjectPointer (NULL if not mObjectPointer is not owned).
   * @SINCE_1_0.0
   */
  using Destructor = void (*)(void*);

  /**
   * @brief Copy constructor operator not declared.
   * @SINCE_1_0.0
   * @param[in] rhs Handle to an object
   */
  CallbackBase(const CallbackBase& rhs);

  /**
   * @brief Assignment operator not declared.
   * @SINCE_1_0.0
   * @param[in] rhs Handle to an object
   * @return A reference to this
   */
  CallbackBase& operator=(const CallbackBase& rhs);

  /**
   * @brief Constructor for member function.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to call (not owned)
   * @param[in] function The member function of the object
   * @param[in] dispatcher Used to call the actual object
   */
  CallbackBase(void* object, MemberFunction function, Dispatcher dispatcher);

  /**
   * @brief Constructor for member function.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to call (owned)
   * @param[in] function The member function of the object
   * @param dispatcher Used to call the actual object
   * @param destructor Used to delete the owned object
   */
  CallbackBase(void* object, MemberFunction function, Dispatcher dispatcher, Destructor destructor);

public: // Data for deriving classes & Dispatchers
  /**
   * @brief Struct to hold the extra data needed for member functions.
   * @SINCE_1_0.0
   */
  struct Impl
  {
    void*      mObjectPointer{nullptr};            ///< Object whose member function will be called. Not owned if mDestructorDispatcher is NULL.
    Dispatcher mMemberFunctionDispatcher{nullptr}; ///< Dispatcher for member functions
    Destructor mDestructorDispatcher{nullptr};     ///< Destructor for owned objects. NULL if mDestructorDispatcher is not owned.
  };
  Impl mImpl;

  union
  {
    MemberFunction mMemberFunction; ///< Pointer to member function
    StaticFunction mStaticFunction; ///< Static function
  };
};

/**
 * @brief Non-member equality operator.
 * @SINCE_1_0.0
 * @param[in] lhs A reference to compare
 * @param[in] rhs A reference to compare to
 * @return True if lhs is same as rhs
 */
inline bool operator==(const CallbackBase& lhs, const CallbackBase& rhs)
{
  if(lhs.mStaticFunction == rhs.mStaticFunction &&
     lhs.mImpl.mObjectPointer == rhs.mImpl.mObjectPointer)
  {
    return true;
  }
  return false;
}

/**
 * @brief Dispatcher to delete an object.
 * @SINCE_1_0.0
 */
template<class T>
struct Destroyer
{
  /**
   * @brief Dispatcher to delete an object.
   * @SINCE_1_0.0
   * @param[in] object An object to delete
   */
  static void Delete(void* object)
  {
    // CallbackBase owns the object but we're the only one who knows the real type so need
    // to delete by "downcasting" from void* to the correct type
    delete reinterpret_cast<T*>(object);
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T>
struct Dispatcher0
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   */
  static void Dispatch(CallbackBase& callback)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)();
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T, typename P1>
struct Dispatcher1
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)(P1);
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(param1);
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2>
struct Dispatcher2
{
  /**
   * @brief Call an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1, P2 param2)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)(P1, P2);
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(param1, param2);
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2, typename P3>
struct Dispatcher3
{
  /**
   * @brief Call an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @param[in] param3 The third parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1, P2 param2, P3 param3)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)(P1, P2, P3);
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(param1, param2, param3);
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T, typename R>
struct DispatcherReturn0
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @return The value
   */
  static R Dispatch(CallbackBase& callback)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = R (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    return (object->*function)();
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1>
struct DispatcherReturn1
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = R (T::*)(P1);
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    return (object->*function)(param1);
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1, typename P2>
struct DispatcherReturn2
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1, P2 param2)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = R (T::*)(P1, P2);
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    return (object->*function)(param1, param2);
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1, typename P2, typename P3>
struct DispatcherReturn3
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @param[in] param3 The third parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1, P2 param2, P3 param3)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = R (T::*)(P1, P2, P3);
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    return (object->*function)(param1, param2, param3);
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T>
struct FunctorDispatcher0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   */
  static void Dispatch(CallbackBase& callback)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    (*object)();
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T, typename P1>
struct FunctorDispatcher1
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function.
   */
  static void Dispatch(CallbackBase& callback, P1 param1)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    (*object)(param1);
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2>
struct FunctorDispatcher2
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1, P2 param2)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    (*object)(param1, param2);
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2, typename P3>
struct FunctorDispatcher3
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @param[in] param3 The third parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1, P2 param2, P3 param3)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    (*object)(param1, param2, param3);
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T, typename R>
struct FunctorDispatcherReturn0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @return The value
   */
  static R Dispatch(CallbackBase& callback)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    return (*object)();
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1>
struct FunctorDispatcherReturn1
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    return (*object)(param1);
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1, typename P2>
struct FunctorDispatcherReturn2
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1, P2 param2)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    return (*object)(param1, param2);
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1, typename P2, typename P3>
struct FunctorDispatcherReturn3
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @param[in] param3 The third parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1, P2 param2, P3 param3)
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    return (*object)(param1, param2, param3);
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a specific void() member function.
 * @SINCE_1_0.0
 */
template<class T>
struct VoidFunctorDispatcher0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   */
  static void Dispatch(CallbackBase& callback)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)();
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, ignoring any signal parameters.
 * @SINCE_1_0.0
 */
template<class T, typename P1>
struct VoidFunctorDispatcher1
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(/*ignore params*/);
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, ignoring any signal parameters.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2>
struct VoidFunctorDispatcher2
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1, P2 param2)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(/*ignore params*/);
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, ignoring any signal parameters.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2, typename P3>
struct VoidFunctorDispatcher3
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @param[in] param3 The third parameter to pass to the real member function
   */
  static void Dispatch(CallbackBase& callback, P1 param1, P2 param2, P3 param3)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(/*ignore params*/);
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, and returns a default-constructed value.
 * @SINCE_1_0.0
 */
template<class T, typename R>
struct VoidFunctorDispatcherReturn0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @return The value
   */
  static R Dispatch(CallbackBase& callback)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(/*ignore params*/);
    return R();
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, and returns a default-constructed value.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1>
struct VoidFunctorDispatcherReturn1
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(/*ignore params*/);
    return R();
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, and returns a default-constructed value.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1, typename P2>
struct VoidFunctorDispatcherReturn2
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1, P2 param2)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(/*ignore params*/);
    return R();
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, and returns a default-constructed value.
 * @SINCE_1_0.0
 */
template<class T, typename R, typename P1, typename P2, typename P3>
struct VoidFunctorDispatcherReturn3
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   * @param[in] param2 The second parameter to pass to the real member function
   * @param[in] param3 The third parameter to pass to the real member function
   * @return The return value from the function
   */
  static R Dispatch(CallbackBase& callback, P1 param1, P2 param2, P3 param3)
  {
    // "downcast" the object and function type back to the correct ones
    T* object               = reinterpret_cast<T*>(callback.mImpl.mObjectPointer);
    using MemberFunction    = void (T::*)();
    MemberFunction function = reinterpret_cast<MemberFunction>(callback.mMemberFunction);
    (object->*function)(/*ignore params*/);
    return R();
  }
};

/**
 * @brief Thin template to provide type safety for member function callbacks.
 *
 * Binds an object to one of its member functions, selecting the dispatcher that
 * matches the member function's signature.
 *
 * @SINCE_2_5.38
 * @see MakeCallback()
 */
template<class T>
class CallbackMember : public CallbackBase
{
public:
  /**
   * @brief Default constructor.
   *
   * @SINCE_2_5.38
   */
  CallbackMember()
  : CallbackBase()
  {
  }

  /**
   * @brief Constructor for member function.
   *
   * Copies the function object.
   * @SINCE_2_5.38
   * @param[in] object The object to call
   * @param[in] memberFunction The member function of the object
   */
  CallbackMember(T* object, void (T::*memberFunction)(void))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&Dispatcher0<T>::Dispatch))
  {
  }
  template<typename P1>
  CallbackMember(T* object, void (T::*memberFunction)(P1))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&Dispatcher1<T, P1>::Dispatch))
  {
  }
  template<typename P1, typename P2>
  CallbackMember(T* object, void (T::*memberFunction)(P1, P2))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&Dispatcher2<T, P1, P2>::Dispatch))
  {
  }
  template<typename P1, typename P2, typename P3>
  CallbackMember(T* object, void (T::*memberFunction)(P1, P2, P3))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&Dispatcher3<T, P1, P2, P3>::Dispatch))
  {
  }
  template<typename R>
  CallbackMember(T* object, R (T::*memberFunction)(void))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&DispatcherReturn0<T, R>::Dispatch))
  {
  }
  template<typename R, typename P1>
  CallbackMember(T* object, R (T::*memberFunction)(P1))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&DispatcherReturn1<T, R, P1>::Dispatch))
  {
  }
  template<typename R, typename P1, typename P2>
  CallbackMember(T* object, R (T::*memberFunction)(P1, P2))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&DispatcherReturn2<T, R, P1, P2>::Dispatch))
  {
  }
  template<typename R, typename P1, typename P2, typename P3>
  CallbackMember(T* object, R (T::*memberFunction)(P1, P2, P3))
  : CallbackBase(object,
                 reinterpret_cast<CallbackBase::MemberFunction>(memberFunction),
                 reinterpret_cast<CallbackBase::Dispatcher>(&DispatcherReturn3<T, R, P1, P2, P3>::Dispatch))
  {
  }
};

/**
 * @brief Specializations for function object callbacks.
 * @SINCE_1_0.0
 */
template<class T>
class CallbackFunctor0 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor0(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcher0<T>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
class CallbackFunctorDelegate0 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object A newly allocated object (ownership is transferred)
   */
  CallbackFunctorDelegate0(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcher0<FunctorDelegate>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template<class T, typename P1>
class CallbackFunctor1 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor1(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcher1<T, P1>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template<typename P1>
class CallbackFunctorDelegate1 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorDelegate1(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcher1<FunctorDelegate, P1>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2>
class CallbackFunctor2 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor2(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcher2<T, P1, P2>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template<typename P1, typename P2>
class CallbackFunctorDelegate2 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorDelegate2(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcher2<FunctorDelegate, P1, P2>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2, typename P3>
class CallbackFunctor3 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor3(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcher3<T, P1, P2, P3>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template<typename P1, typename P2, typename P3>
class CallbackFunctorDelegate3 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorDelegate3(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcher3<FunctorDelegate, P1, P2, P3>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template<class T, typename R>
class CallbackFunctorReturn0 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn0(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcherReturn0<T, R>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template<typename R>
class CallbackFunctorDelegateReturn0 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorDelegateReturn0(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcherReturn0<FunctorDelegate, R>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename R>
class CallbackFunctorReturn1 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn1(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcherReturn1<T, R, P1>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template<typename P1, typename R>
class CallbackFunctorDelegateReturn1 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorDelegateReturn1(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcherReturn1<FunctorDelegate, R, P1>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2, typename R>
class CallbackFunctorReturn2 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn2(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcherReturn2<T, R, P1, P2>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template<typename P1, typename P2, typename R>
class CallbackFunctorDelegateReturn2 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorDelegateReturn2(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcherReturn2<FunctorDelegate, R, P1, P2>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template<class T, typename P1, typename P2, typename P3, typename R>
class CallbackFunctorReturn3 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn3(const T& object)
  : CallbackBase(reinterpret_cast<void*>(new T(object)), // copy the object
                 NULL,                                   // uses operator() instead of member function
                 reinterpret_cast<CallbackBase::Dispatcher>(&FunctorDispatcherReturn3<T, R, P1, P2, P3>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<T>::Delete))
  {
  }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template<typename P1, typename P2, typename P3, typename R>
class CallbackFunctorDelegateReturn3 : public CallbackBase
{
public:
  /**
   * @brief Constructor which copies a function object.
   *
   * This variant calls a void() member, ignoring any signal parameters.
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorDelegateReturn3(FunctorDelegate* object)
  : CallbackBase(reinterpret_cast<void*>(object), // transfer ownership
                 reinterpret_cast<CallbackBase::MemberFunction>(&FunctorDelegate::Execute),
                 reinterpret_cast<CallbackBase::Dispatcher>(&VoidFunctorDispatcherReturn3<FunctorDelegate, R, P1, P2, P3>::Dispatch),
                 reinterpret_cast<CallbackBase::Destructor>(&Destroyer<FunctorDelegate>::Delete))
  {
  }
};

// Callback creation thin templates
/**
 * @brief Creates a callback from a free function with parameter pack.
 *
 * @SINCE_2_5.14
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<typename... Args>
inline CallbackBase* MakeCallback(void (*function)(Args... args))
{
  return new CallbackBase(reinterpret_cast<CallbackBase::StaticFunction>(function));
}

/**
 * @brief Creates a callback from a free function with parameter pack.
 *
 * @SINCE_1_9.33
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<typename R, typename... Args>
inline CallbackBase* MakeCallback(R (*function)(Args... args))
{
  return new CallbackBase(reinterpret_cast<CallbackBase::StaticFunction>(function));
}

/**
 * @brief Creates a callback from a class member function with no parameters.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T>
inline CallbackBase* MakeCallback(T* object, void (T::*function)(void))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class member function with one parameter.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, typename P1>
inline CallbackBase* MakeCallback(T* object, void (T::*function)(P1))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class member function with two parameters.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, typename P1, typename P2>
inline CallbackBase* MakeCallback(T* object, void (T::*function)(P1, P2))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class member function with three parameters.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, typename P1, typename P2, typename P3>
inline CallbackBase* MakeCallback(T* object, void (T::*function)(P1, P2, P3))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class member function with no parameters and a return type.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, typename R>
inline CallbackBase* MakeCallback(T* object, R (T::*function)())
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class member function with one parameter and a return type.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, typename P1, typename R>
inline CallbackBase* MakeCallback(T* object, R (T::*function)(P1))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class member function with two parameters and a return type.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, typename P1, typename P2, typename R>
inline CallbackBase* MakeCallback(T* object, R (T::*function)(P1, P2))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class member function with three parameters and a return type.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, typename P1, typename P2, typename P3, typename R>
inline CallbackBase* MakeCallback(T* object, R (T::*function)(P1, P2, P3))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Creates a callback from a class's parent member function with no parameters.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, class Base>
inline CallbackBase* MakeCallback(T* object, void (Base::*function)(void))
{
  return new CallbackMember<T>(object, function);
}
/**
 * @brief Creates a callback from a class's parent member function with no parameters.
 *
 * Requires the function to be member of the same class.
 * @SINCE_1_0.0
 * @param[in] object The object to call
 * @param[in] function The member function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template<class T, class Base>
inline CallbackBase* MakeCallback(T& object, void (Base::*function)(void))
{
  return new CallbackMember<T>(object, function);
}

/**
 * @brief Move-only typed callback which owns the underlying CallbackBase.
 *
 * Provides a type-safe, owning alternative to passing a raw CallbackBase
 * pointer around. The signature is given as a function type, for example
 * @code
 * Callback<void()>          OnFinished;
 * Callback<bool(float)>     OnTick;
 * Callback<void(int, int)>  OnResized;
 * @endcode
 *
 * The primary template is not defined; only function-type specialisations
 * are valid.
 *
 * @note Free functions may take any number of parameters. Member functions are
 * limited to three, which is the arity of the underlying dispatchers.
 *
 * @SINCE_2_5.38
 * @tparam Signature The function type of the callback
 */
template<typename Signature>
class Callback;

/**
 * @brief Move-only typed callback with no return value.
 *
 * @SINCE_2_5.38
 * @tparam Args The parameter types of the callback
 */
template<typename... Args>
class Callback<void(Args...)>
{
public:
  /**
   * @brief Creates an empty callback.
   * @SINCE_2_5.38
   */
  Callback() = default;

  /**
   * @brief Move constructor.
   * @SINCE_2_5.38
   */
  Callback(Callback&&) noexcept = default;

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.38
   * @return A reference to this
   */
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  /**
   * @brief Queries whether this callback holds a function to call.
   * @SINCE_2_5.38
   * @return True if a function is held
   */
  explicit operator bool() const
  {
    return static_cast<bool>(mCallback);
  }

  /**
   * @brief Creates a callback from a free function.
   * @SINCE_2_5.38
   * @param[in] func The function to call
   * @return The newly created callback
   */
  static Callback New(void (*func)(Args...))
  {
    return Callback(MakeCallback(func));
  }

  /**
   * @brief Creates a callback from a class member function.
   *
   * @SINCE_2_5.38
   * @tparam T The type of the object
   * @param[in] obj The object to call the member function on
   * @param[in] func The member function to call
   * @return The newly created callback
   * @note Member functions are limited to three parameters.
   */
  template<class T>
  static Callback New(T* obj, void (T::*func)(Args...))
  {
    static_assert(sizeof...(Args) <= 3, "Dali::Callback supports at most three parameters for member functions");
    return Callback(MakeCallback(obj, func));
  }

  /**
   * @brief Calls the held function.
   *
   * Does nothing if this callback is empty.
   * @SINCE_2_5.38
   * @param[in] args The arguments to pass to the function
   */
  void Invoke(Args... args)
  {
    if(mCallback)
    {
      CallbackBase::Execute<Args...>(*mCallback, args...);
    }
  }

  /**
   * @brief Relinquishes ownership of the held callback.
   *
   * Used to hand the callback over to an API which takes a raw
   * CallbackBase pointer and assumes ownership of it.
   * @SINCE_2_5.38
   * @return The held callback, ownership transferred to the caller
   */
  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* callback)
  : mCallback(callback)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

/**
 * @brief Move-only typed callback with a return value.
 *
 * @SINCE_2_5.38
 * @tparam Ret The return type of the callback
 * @tparam Args The parameter types of the callback
 */
template<typename Ret, typename... Args>
class Callback<Ret(Args...)>
{
public:
  /**
   * @brief Creates an empty callback.
   * @SINCE_2_5.38
   */
  Callback() = default;

  /**
   * @brief Move constructor.
   * @SINCE_2_5.38
   */
  Callback(Callback&&) noexcept = default;

  /**
   * @brief Move assignment operator.
   * @SINCE_2_5.38
   * @return A reference to this
   */
  Callback& operator=(Callback&&) noexcept = default;

  Callback(const Callback&)            = delete;
  Callback& operator=(const Callback&) = delete;

  /**
   * @brief Queries whether this callback holds a function to call.
   * @SINCE_2_5.38
   * @return True if a function is held
   */
  explicit operator bool() const
  {
    return static_cast<bool>(mCallback);
  }

  /**
   * @brief Creates a callback from a free function.
   * @SINCE_2_5.38
   * @param[in] func The function to call
   * @return The newly created callback
   */
  static Callback New(Ret (*func)(Args...))
  {
    return Callback(MakeCallback(func));
  }

  /**
   * @brief Creates a callback from a class member function.
   *
   * @SINCE_2_5.38
   * @tparam T The type of the object
   * @param[in] obj The object to call the member function on
   * @param[in] func The member function to call
   * @return The newly created callback
   * @note Member functions are limited to three parameters.
   */
  template<class T>
  static Callback New(T* obj, Ret (T::*func)(Args...))
  {
    static_assert(sizeof...(Args) <= 3, "Dali::Callback supports at most three parameters for member functions");
    return Callback(MakeCallback(obj, func));
  }

  /**
   * @brief Calls the held function.
   *
   * @SINCE_2_5.38
   * @param[in] args The arguments to pass to the function
   * @return The value returned by the function, or a value-initialised Ret if
   * this callback is empty
   */
  Ret Invoke(Args... args)
  {
    return mCallback ? CallbackBase::ExecuteReturn<Ret, Args...>(*mCallback, args...) : Ret();
  }

  /**
   * @brief Relinquishes ownership of the held callback.
   *
   * Used to hand the callback over to an API which takes a raw
   * CallbackBase pointer and assumes ownership of it.
   * @SINCE_2_5.38
   * @return The held callback, ownership transferred to the caller
   */
  CallbackBase* Release()
  {
    return mCallback.Release();
  }

private:
  explicit Callback(CallbackBase* callback)
  : mCallback(callback)
  {
  }

  UniquePtr<CallbackBase> mCallback;
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_CALLBACK_H
