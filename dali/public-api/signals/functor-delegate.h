#ifndef __DALI_FUNCTOR_DELEGATE_H__
#define __DALI_FUNCTOR_DELEGATE_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_signals
 * @{
 */

/**
 * @brief Dispatcher to call a functor.
 * @SINCE_1_0.0
 */
template< typename T >
struct FunctorDispatcher
{
  /**
   * @brief Calls a function object.
   *
   * @SINCE_1_0.0
   * @param[in] functorPtr The functor to call
   */
  static void Dispatch( void* functorPtr )
  {
    // "downcast" the functor type back to the correct one
    T* functor = reinterpret_cast< T* >( functorPtr );
    (*functor)();
  }
};

/**
 * @brief Dispatcher to delete a functor object.
 * @SINCE_1_0.0
 */
template< typename T >
struct FunctorDestroyer
{
  /**
   * @brief Dispatcher to delete an object.
   * @SINCE_1_0.0
   * @param[in] functorPtr A functor object to delete
   */
  static void Delete( void* functorPtr )
  {
    // FunctorDelegate owns the object but we're the only one who knows the real type so need
    // to delete by "downcasting" from void* to the correct type
    delete reinterpret_cast< T* >( functorPtr );
  }
};

/**
 * @brief Used to connect a void() functor to a signal via BaseObject::SignalConnect().
 * @SINCE_1_0.0
 */
class DALI_CORE_API FunctorDelegate
{
public:

  /**
   * @brief Constructor which copies a function object.
   *
   * @SINCE_1_0.0
   * @param[in] functor The functor object to copy, either a class with operator() or a C function
   * @return A pointer to the new function object
   */
  template< typename T >
  static FunctorDelegate* New( const T& functor )
  {
    return new FunctorDelegate( reinterpret_cast< void* >( new T( functor ) ), // heap allocate the functor
                                reinterpret_cast< FunctorDelegate::Dispatcher >( &FunctorDispatcher<T>::Dispatch ),
                                reinterpret_cast< FunctorDelegate::Destructor >( &FunctorDestroyer<T>::Delete ) );
  }

  /**
   * @brief Non-virtual destructor; not intended as a base class.
   * @SINCE_1_0.0
   */
  ~FunctorDelegate();

  /**
   * @brief Function to call the function or member function dispatcher.
   * @SINCE_1_0.0
   */
  void Execute();

private:

  /**
   * @brief Used to call the correct function.
   * @SINCE_1_0.0
   */
  typedef void (*Dispatcher)( void* objectPtr );

  /**
   * @brief Used to destroy mObjectPointer.
   * @SINCE_1_0.0
   */
  typedef void(*Destructor)( void* objectPtr );

  /**
   * @brief Not defined.
   * @SINCE_1_0.0
   */
  FunctorDelegate( const FunctorDelegate& rhs );

  /**
   * @brief Not defined.
   * @SINCE_1_0.0
   */
  const FunctorDelegate& operator=( const FunctorDelegate& rhs );

  /**
   * @brief Private constructor.
   *
   * @SINCE_1_0.0
   * @param[in] functorPtr A newly allocated functor object (takes ownership)
   * @param dispatcher Used to call the actual function
   * @param destructor Used to delete the owned functor object
   */
  FunctorDelegate( void* functorPtr, Dispatcher dispatcher, Destructor destructor );

public: // Data for deriving classes & Dispatchers

  void* mFunctorPointer;                ///< Functor that will be called
  Dispatcher mMemberFunctionDispatcher; ///< Dispatcher for member functions
  Destructor mDestructorDispatcher;     ///< Destructor for owned objects
};

/**
 * @}
 */
} // namespace DALI

#endif // __DALI_FUNCTOR_DELEGATE_H__
