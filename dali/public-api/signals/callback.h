#ifndef __DALI_CALLBACK_H__
#define __DALI_CALLBACK_H__

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
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/functor-delegate.h>

namespace Dali
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
class DALI_IMPORT_API CallbackBase
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
  ~CallbackBase();

  /**
   * @brief Resets the object pointer so that we know not to call methods of this object any more.
   * @SINCE_1_0.0
   */
  void Reset();

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   */
  static void Execute( CallbackBase& callback )
  {
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code so the library containing
    // the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      Dispatcher dispatcher = callback.mImpl->mMemberFunctionDispatcher;
      (*dispatcher)( callback );
    }
    // its also possible to have a member function pointer to a CallbackProvider
    // that has been deleted, so check if we have impl still
    else if( !callback.mImpl && callback.mFunction )
    {
      (*(callback.mFunction))();
    }
    else
    {
      DALI_ASSERT_ALWAYS( 0 && "no function to execute" );
    }
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   * @return The value from the function
   */
  template< typename R >
  static R ExecuteReturn( CallbackBase& callback )
  {
    R returnVal = R();
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code so the library containing
    // the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      typedef R(*Dispatcher)(CallbackBase& base);
      Dispatcher dispatcher = reinterpret_cast< Dispatcher >( callback.mImpl->mMemberFunctionDispatcher );
      returnVal = (*dispatcher)( callback );
    }
    else if( !callback.mImpl && callback.mFunction )
    {
      typedef R(*Function1)();
      returnVal = (*(reinterpret_cast< Function1 >( callback.mFunction )))();
    }

    return returnVal;
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   * @param[in] param1 The first parameter to pass into the function
   */
  template< typename P1 >
  static void Execute( CallbackBase& callback, P1 param1 )
  {
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      typedef void(*Dispatcher)(CallbackBase& base,P1);
      Dispatcher dispatcher = reinterpret_cast< Dispatcher >( callback.mImpl->mMemberFunctionDispatcher );
      (*dispatcher)( callback, param1 );
    }
    else if( !callback.mImpl && callback.mFunction )
    {
      // convert function type
      typedef void(*Function1)(P1);
      (*(reinterpret_cast< Function1 >( callback.mFunction )))( param1 );
    }
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   * @param[in] param1 The first parameter to pass into the function
   * @return The value from the function
   */
  template< typename R, typename P1 >
  static R ExecuteReturn( CallbackBase& callback, P1 param1 )
  {
    R returnVal =  R();
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      typedef R(*Dispatcher)(CallbackBase& base,P1);
      Dispatcher dispatcher = reinterpret_cast< Dispatcher >( callback.mImpl->mMemberFunctionDispatcher );
      returnVal = (*dispatcher)( callback, param1 );
    }
    else if( !callback.mImpl && callback.mFunction )
    {
      // convert function type
      typedef R(*Function1)(P1);
      returnVal = (*(reinterpret_cast< Function1 >( callback.mFunction )))( param1 );
    }

    return returnVal;
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   * @param[in] param1 The first parameter to pass into the function
   * @param[in] param2 The second parameter to pass into the function
   */
  template< typename P1, typename P2 >
  static void Execute( CallbackBase& callback, P1 param1, P2 param2 )
  {
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      typedef void(*Dispatcher)(CallbackBase& base,P1,P2);
      Dispatcher dispatcher = reinterpret_cast< Dispatcher >( callback.mImpl->mMemberFunctionDispatcher );
      (*dispatcher)( callback, param1, param2 );
    }
    else if( !callback.mImpl && callback.mFunction )
    {
      // convert function type
      typedef void(*Function2)(P1,P2);
      (*(reinterpret_cast< Function2 >( callback.mFunction )))( param1, param2 );
    }
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   * @param[in] param1 The first parameter to pass into the function
   * @param[in] param2 The second parameter to pass into the function
   * @return The return value from the function
   */
  template< typename R, typename P1, typename P2 >
  static R ExecuteReturn( CallbackBase& callback, P1 param1, P2 param2 )
  {
    R returnVal= R();
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      typedef R(*Dispatcher)(CallbackBase& base,P1,P2);
      Dispatcher dispatcher = reinterpret_cast< Dispatcher >( callback.mImpl->mMemberFunctionDispatcher );
      returnVal = (*dispatcher)( callback, param1, param2 );
    }
    else if( !callback.mImpl && callback.mFunction )
    {
      // convert function type
      typedef R(*Function2)(P1,P2);
      returnVal = (*(reinterpret_cast< Function2 >( callback.mFunction )))( param1, param2 );
    }

    return returnVal;
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   * @param[in] param1 The first parameter to pass into the function
   * @param[in] param2 The second parameter to pass into the function
   * @param[in] param3 The third parameter to pass into the function
   */
  template< typename P1, typename P2, typename P3 >
  static void Execute( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      typedef void(*Dispatcher)(CallbackBase& base,P1,P2,P3);
      Dispatcher dispatcher = reinterpret_cast< Dispatcher >( callback.mImpl->mMemberFunctionDispatcher );
      (*dispatcher)( callback, param1, param2, param3 );
    }
    else if( !callback.mImpl && callback.mFunction )
    {
      // convert function type
      typedef void(*Function2)(P1,P2,P3);
      (*(reinterpret_cast< Function2 >( callback.mFunction )))( param1, param2, param3 );
    }
  }

  /**
   * @brief Function to call the function or member function dispatcher.
   *
   * This function template gets instantiated at the call site.
   * @SINCE_1_0.0
   * @param[in] callback The callback to call
   * @param[in] param1 The first parameter to pass into the function
   * @param[in] param2 The second parameter to pass into the function
   * @param[in] param3 The third parameter to pass into the function
   * @return The return value from the function
   */
  template< typename R, typename P1, typename P2, typename P3 >
  static R ExecuteReturn( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    R returnVal= R();
    // if we point to a function, we can call it directly
    // otherwise call the dispatcher function that knows the real type of the object
    // Note that this template dispatcher lives in client code (where the callback was created)
    // so the library containing the code has to be loaded, otherwise we crash boom bang
    if( callback.mImpl && callback.mImpl->mObjectPointer )
    {
      typedef R(*Dispatcher)(CallbackBase& base,P1,P2,P3);
      Dispatcher dispatcher = reinterpret_cast< Dispatcher >( callback.mImpl->mMemberFunctionDispatcher );
      returnVal = (*dispatcher)( callback, param1, param2, param3 );
    }
    else if( !callback.mImpl && callback.mFunction )
    {
      // convert function type
      typedef R(*Function2)(P1,P2,P3);
      returnVal = (*(reinterpret_cast< Function2 >( callback.mFunction )))( param1, param2, param3 );
    }

    return returnVal;
  }

protected: // Constructors for deriving classes

  /**
   * @brief Function with static linkage.
   * @SINCE_1_0.0
   */
  typedef void(*Function)(void);

  /**
   * @brief Member function.
   * @SINCE_1_0.0
   */
  typedef void (CallbackBase::*MemberFunction)( void );

  /**
   * @brief Used to call the correct member function.
   * @SINCE_1_0.0
   */
  typedef void (*Dispatcher)( CallbackBase& base );

  /**
   * @brief Used to destroy mObjectPointer (NULL if not mObjectPointer is not owned).
   * @SINCE_1_0.0
   */
  typedef void(*Destructor)(void* object);

  /**
   * @brief Copy constructor operator not declared.
   * @SINCE_1_0.0
   * @param[in] rhs Handle to an object
   */
  CallbackBase( const CallbackBase& rhs );
  /**
   * @brief Assignment operator not declared.
   * @SINCE_1_0.0
   * @param[in] rhs Handle to an object
   * @return A reference to this
   */
  const CallbackBase& operator=( const CallbackBase& rhs );

  /**
   * @brief Constructor for function with static linkage.
   *
   * @SINCE_1_0.0
   * @param[in] function The function to call
   */
  CallbackBase( Function function );

  /**
   * @brief Constructor for member function.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to call (not owned)
   * @param[in] function The member function of the object
   * @param[in] dispatcher Used to call the actual object
   */
  CallbackBase( void* object, MemberFunction function, Dispatcher dispatcher );

  /**
   * @brief Constructor for member function.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to call (owned)
   * @param[in] function The member function of the object
   * @param dispatcher Used to call the actual object
   * @param destructor Used to delete the owned object
   */
  CallbackBase( void* object, MemberFunction function, Dispatcher dispatcher, Destructor destructor );

public: // Data for deriving classes & Dispatchers

  /**
   * @brief Struct to hold the extra data needed for member functions.
   * @SINCE_1_0.0
   */
  struct Impl
  {
    Impl();                               ///< Default constructor @SINCE_1_0.0

    void* mObjectPointer;                 ///< Object whose member function will be called. Not owned if mDestructorDispatcher is NULL.
    Dispatcher mMemberFunctionDispatcher; ///< Dispatcher for member functions
    Destructor mDestructorDispatcher;     ///< Destructor for owned objects. NULL if mDestructorDispatcher is not owned.
  };
  Impl* mImpl;                            ///< Implementation pointer

  union
  {
    MemberFunction mMemberFunction;       ///< Pointer to member function
    Function mFunction;                   ///< Static function
  };
};

/**
 * @brief Non-member equality operator.
 * @SINCE_1_0.0
 * @param[in] lhs A reference to compare
 * @param[in] rhs A reference to compare to
 * @return True if lhs is same as rhs
 */
bool operator==( const CallbackBase& lhs, const CallbackBase& rhs );

/**
 * @brief Dispatcher to delete an object.
 * @SINCE_1_0.0
 */
template< class T >
struct Destroyer
{
  /**
   * @brief Dispatcher to delete an object.
   * @SINCE_1_0.0
   * @param[in] object An object to delete
   */
  static void Delete( void* object )
  {
    // CallbackBase owns the object but we're the only one who knows the real type so need
    // to delete by "downcasting" from void* to the correct type
    delete reinterpret_cast< T* >( object );
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T >
struct Dispatcher0
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   */
  static void Dispatch( CallbackBase& callback )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)();
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T, typename P1 >
struct Dispatcher1
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   */
  static void Dispatch( CallbackBase& callback, P1 param1 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(P1);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)( param1 );
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2 >
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
  static void Dispatch( CallbackBase& callback, P1 param1, P2 param2 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(P1, P2);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)( param1, param2 );
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2, typename P3 >
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
  static void Dispatch( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(P1, P2, P3);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)( param1, param2, param3 );
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T, typename R >
struct DispatcherReturn0
{
  /**
   * @brief Calls an actual member function.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @return The value
   */
  static R Dispatch( CallbackBase& callback )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef R(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    return (object->*function)();
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T, typename R, typename P1 >
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
  static R Dispatch( CallbackBase& callback, P1 param1 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef R(T::*MemberFunction)(P1);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    return (object->*function)( param1 );
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T, typename R, typename P1, typename P2 >
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
  static R Dispatch( CallbackBase& callback, P1 param1, P2 param2 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef R(T::*MemberFunction)(P1, P2);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    return (object->*function)( param1, param2 );
  }
};

/**
 * @brief Dispatcher to call the actual member function.
 * @SINCE_1_0.0
 */
template< class T, typename R, typename P1, typename P2, typename P3 >
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
  static R Dispatch( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef R(T::*MemberFunction)(P1, P2, P3);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    return (object->*function)( param1, param2, param3 );
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T >
struct FunctorDispatcher0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   */
  static void Dispatch( CallbackBase& callback )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    (*object)();
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T, typename P1 >
struct FunctorDispatcher1
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function.
   */
  static void Dispatch( CallbackBase& callback, P1 param1 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    (*object)( param1 );
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2 >
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
  static void Dispatch( CallbackBase& callback, P1 param1, P2 param2 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    (*object)( param1, param2 );
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2, typename P3 >
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
  static void Dispatch( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    (*object)( param1, param2, param3 );
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T, typename R >
struct FunctorDispatcherReturn0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @return The value
   */
  static R Dispatch( CallbackBase& callback )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    return (*object)();
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T, typename R, typename P1 >
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
  static R Dispatch( CallbackBase& callback, P1 param1 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    return (*object)( param1 );
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T, typename R, typename P1, typename P2 >
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
  static R Dispatch( CallbackBase& callback, P1 param1, P2 param2 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    return (*object)( param1, param2 );
  }
};

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< class T, typename R, typename P1, typename P2, typename P3 >
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
  static R Dispatch( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    return (*object)( param1, param2, param3 );
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a specific void() member function.
 * @SINCE_1_0.0
 */
template< class T >
struct VoidFunctorDispatcher0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   */
  static void Dispatch( CallbackBase& callback )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)();
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, ignoring any signal parameters.
 * @SINCE_1_0.0
 */
template< class T, typename P1 >
struct VoidFunctorDispatcher1
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @param[in] param1 The first parameter to pass to the real member function
   */
  static void Dispatch( CallbackBase& callback, P1 param1 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)(/*ignore params*/);
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, ignoring any signal parameters.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2 >
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
  static void Dispatch( CallbackBase& callback, P1 param1, P2 param2 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)(/*ignore params*/);
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, ignoring any signal parameters.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2, typename P3 >
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
  static void Dispatch( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)(/*ignore params*/);
  }
};

/**
 * @brief Dispatcher to call a functor.
 *
 * This variant calls a void() member, and returns a default-constructed value.
 * @SINCE_1_0.0
 */
template< class T, typename R >
struct VoidFunctorDispatcherReturn0
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] callback The callback information
   * @return The value
   */
  static R Dispatch( CallbackBase& callback )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
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
template< class T, typename R, typename P1 >
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
  static R Dispatch( CallbackBase& callback, P1 param1 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
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
template< class T, typename R, typename P1, typename P2 >
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
  static R Dispatch( CallbackBase& callback, P1 param1, P2 param2 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
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
template< class T, typename R, typename P1, typename P2, typename P3 >
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
  static R Dispatch( CallbackBase& callback, P1 param1, P2 param2, P3 param3 )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( callback.mImpl->mObjectPointer );
    typedef void(T::*MemberFunction)(void);
    MemberFunction function = reinterpret_cast< MemberFunction >( callback.mMemberFunction );
    (object->*function)(/*ignore params*/);
    return R();
  }
};

/**
 * @brief Thin template to provide type safety for member function callbacks.
 *
 * Version with two parameters and return value.
 * @SINCE_1_0.0
 */
template< class T >
class Callback : public CallbackBase
{
public:

  /**
   * @brief Default constructor.
   *
   * @SINCE_1_0.0
   */
  Callback()
  : CallbackBase()
  {
  }

  /**
   * @brief Constructor for member function.
   *
   * Copies the function object.
   * @SINCE_1_0.0
   * @param[in] object The object to call
   * @param[in] memberFunction The member function of the object
   */
  Callback( T* object, void(T::*memberFunction)(void) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &Dispatcher0<T>::Dispatch ) ) { }
  template< typename P1 >
  Callback( T* object, void(T::*memberFunction)(P1) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &Dispatcher1<T,P1>::Dispatch ) ) { }
  template< typename P1, typename P2 >
  Callback( T* object, void(T::*memberFunction)(P1, P2) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &Dispatcher2<T,P1,P2>::Dispatch ) ) { }
  template< typename P1, typename P2, typename P3 >
  Callback( T* object, void(T::*memberFunction)(P1, P2, P3) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &Dispatcher3<T,P1,P2,P3>::Dispatch ) ) { }
  template< typename R >
  Callback( T* object, R(T::*memberFunction)(void) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &DispatcherReturn0<T,R>::Dispatch ) ) { }
  template< typename R, typename P1 >
  Callback( T* object, R(T::*memberFunction)(P1) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &DispatcherReturn1<T,R,P1>::Dispatch ) ) { }
  template< typename R, typename P1, typename P2 >
  Callback( T* object, R(T::*memberFunction)(P1, P2) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &DispatcherReturn2<T,R,P1,P2>::Dispatch ) ) { }
  template< typename R, typename P1, typename P2, typename P3 >
  Callback( T* object, R(T::*memberFunction)(P1, P2, P3) )
  : CallbackBase( object,
                  reinterpret_cast< CallbackBase::MemberFunction >( memberFunction ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &DispatcherReturn3<T,R,P1,P2,P3>::Dispatch ) ) { }

};

/**
 * @brief Specializations for static function callbacks.
 * @SINCE_1_0.0
 */
class CallbackFunction : public CallbackBase
{
public:

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  CallbackFunction()
  : CallbackBase()
  {
  }

  /**
   * @brief Constructors for functions with static linkage.
   *
   * @SINCE_1_0.0
   * @param[in] function The function to call
   */
  CallbackFunction( void(*function)() )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }
  template< typename R >
  CallbackFunction( R(*function)() )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }
  template< typename P1 >
  CallbackFunction( void(*function)(P1) )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }
  template< typename P1, typename R >
  CallbackFunction( R(*function)(P1)  )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }
  template< typename P1, typename P2 >
  CallbackFunction( void(*function)(P1,P2)  )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }
  template< typename P1, typename P2, typename R >
  CallbackFunction( R(*function)(P1,P2) )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }
  template< typename P1, typename P2, typename P3 >
  CallbackFunction( void(*function)(P1,P2,P3)  )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }
  template< typename P1, typename P2, typename P3, typename R >
  CallbackFunction( R(*function)(P1,P2,P3) )
  : CallbackBase( reinterpret_cast< CallbackBase::Function >( function ) )
  { }

};

/**
 * @brief Specializations for function object callbacks.
 * @SINCE_1_0.0
 */
template< class T >
class CallbackFunctor0 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor0( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcher0<T>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
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
  CallbackFunctorDelegate0( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcher0<FunctorDelegate>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template< class T, typename P1 >
class CallbackFunctor1 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor1( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcher1<T,P1>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template< typename P1 >
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
  CallbackFunctorDelegate1( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcher1<FunctorDelegate,P1>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2 >
class CallbackFunctor2 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor2( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcher2<T,P1,P2>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template< typename P1, typename P2 >
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
  CallbackFunctorDelegate2( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcher2<FunctorDelegate,P1,P2>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2, typename P3 >
class CallbackFunctor3 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctor3( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcher3<T,P1,P2,P3>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template< typename P1, typename P2, typename P3 >
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
  CallbackFunctorDelegate3( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcher3<FunctorDelegate,P1,P2,P3>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template< class T, typename R >
class CallbackFunctorReturn0 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn0( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcherReturn0<T,R>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template< typename R >
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
  CallbackFunctorDelegateReturn0( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcherReturn0<FunctorDelegate,R>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename R >
class CallbackFunctorReturn1 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn1( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcherReturn1<T,R,P1>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template< typename P1, typename R >
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
  CallbackFunctorDelegateReturn1( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcherReturn1<FunctorDelegate,R,P1>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2, typename R >
class CallbackFunctorReturn2 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn2( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcherReturn2<T,R,P1,P2>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template< typename P1, typename P2, typename R >
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
  CallbackFunctorDelegateReturn2( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcherReturn2<FunctorDelegate,R,P1,P2>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

/**
 * @brief Function object callback for matching callbacks to signal signature.
 * @SINCE_1_0.0
 */
template< class T, typename P1, typename P2, typename P3, typename R >
class CallbackFunctorReturn3 : public CallbackBase
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] object The object to copy
   */
  CallbackFunctorReturn3( const T& object )
  : CallbackBase( reinterpret_cast< void* >( new T( object ) ), // copy the object
                  NULL, // uses operator() instead of member function
                  reinterpret_cast< CallbackBase::Dispatcher >( &FunctorDispatcherReturn3<T,R,P1,P2,P3>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<T>::Delete ) ) { }
};

/**
 * @brief Function object callback for connecting void() methods.
 * @SINCE_1_0.0
 */
template< typename P1, typename P2, typename P3, typename R >
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
  CallbackFunctorDelegateReturn3( FunctorDelegate* object )
  : CallbackBase( reinterpret_cast< void* >( object ), // transfer ownership
                  reinterpret_cast< CallbackBase::MemberFunction >( &FunctorDelegate::Execute ),
                  reinterpret_cast< CallbackBase::Dispatcher >( &VoidFunctorDispatcherReturn3<FunctorDelegate,R,P1,P2,P3>::Dispatch ),
                  reinterpret_cast< CallbackBase::Destructor >( &Destroyer<FunctorDelegate>::Delete ) ) { }
};

// Callback creation thin templates

/**
 * @brief Creates a callback from a C function or static member function with no parameters.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
inline CallbackBase* MakeCallback( void(*function)(void) )
{
  return new CallbackFunction( function );
}

/**
 * @brief Creates a callback from a C function or static member function with one parameter.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template< typename P1 >
inline CallbackBase* MakeCallback( void(*function)(P1) )
{
  return new CallbackFunction( function );
}

/**
 * @brief Creates a callback from a C function or static member function with no parameters and a return type.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template< typename R >
inline CallbackBase* MakeCallback( R(*function)(void) )
{
  return new CallbackFunction( function );
}

/**
 * @brief Creates a callback from a C function or static member function with one parameter and a return type.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template< typename R, typename P1 >
inline CallbackBase* MakeCallback( R(*function)(P1) )
{
  return new CallbackFunction( function );
}

/**
 * @brief Creates a callback from a C function or static member function with two parameters.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template< typename P1, typename P2 >
inline CallbackBase* MakeCallback( void(*function)(P1,P2) )
{
  return new CallbackFunction( function );
}

/**
 * @brief Creates a callback from a C function or static member function with two parameters and a return type.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template< typename R, typename P1, typename P2 >
inline CallbackBase* MakeCallback( R(*function)(P1,P2) )
{
  return new CallbackFunction( function );
}

/**
 * @brief Creates a callback from a C function or static member function with three parameters.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template< typename P1, typename P2, typename P3 >
inline CallbackBase* MakeCallback( void(*function)(P1,P2,P3) )
{
  return new CallbackFunction( function );
}

/**
 * @brief Creates a callback from a C function or static member function with three parameters and a return type.
 *
 * @SINCE_1_0.0
 * @param[in] function The function to call
 * @return A newly allocated Callback object, ownership transferred to caller
 */
template< typename R, typename P1, typename P2, typename P3 >
inline CallbackBase* MakeCallback( R(*function)(P1,P2,P3) )
{
  return new CallbackFunction( function );
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
template< class T >
inline CallbackBase* MakeCallback( T* object, void(T::*function)(void) )
{
  return new Callback< T >( object, function );
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
template< class T, typename P1 >
inline CallbackBase* MakeCallback( T* object, void(T::*function)(P1) )
{
  return new Callback< T >( object, function );
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
template< class T, typename P1, typename P2 >
inline CallbackBase* MakeCallback( T* object, void(T::*function)(P1,P2) )
{
  return new Callback< T >( object, function );
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
template< class T, typename P1, typename P2, typename P3 >
inline CallbackBase* MakeCallback( T* object, void(T::*function)(P1,P2,P3) )
{
  return new Callback< T >( object, function );
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
template< class T, typename R >
inline CallbackBase* MakeCallback( T* object, R(T::*function)() )
{
  return new Callback< T >( object, function );
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
template< class T, typename P1, typename R >
inline CallbackBase* MakeCallback( T* object, R(T::*function)(P1) )
{
  return new Callback< T >( object, function );
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
template< class T, typename P1, typename P2, typename R >
inline CallbackBase* MakeCallback( T* object, R(T::*function)(P1,P2) )
{
  return new Callback< T >( object, function );
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
template< class T, typename P1, typename P2, typename P3, typename R >
inline CallbackBase* MakeCallback( T* object, R(T::*function)(P1,P2,P3) )
{
  return new Callback< T >( object, function );
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
template< class T, class Base >
inline CallbackBase* MakeCallback( T* object, void(Base::*function)(void) )
{
  return new Callback< T >( object, function );
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
template< class T, class Base >
inline CallbackBase* MakeCallback( T& object, void(Base::*function)(void) )
{
  return new Callback< T >( object, function );
}

/**
 * @}
 */
} // namespace DALI

#endif // __DALI_CALLBACK_H__
