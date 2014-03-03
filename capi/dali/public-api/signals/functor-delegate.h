#ifndef __DALI_FUNCTOR_DELEGATE_H__
#define __DALI_FUNCTOR_DELEGATE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

/**
 * Dispatcher to call a functor
 */
template< class T >
struct FunctorDispatcher
{
  /**
   * Call a function object.
   * @param[in] objectPtr The object to call.
   */
  static void Dispatch( void* objectPtr )
  {
    // "downcast" the object and function type back to the correct ones
    T* object = reinterpret_cast< T* >( objectPtr );
    (*object)();
  }
};

/**
 * Dispatcher to delete an object
 */
template< class T >
struct FunctorDestroyer
{
  /**
   * Dispatcher to delete an object
   */
  static void Delete( void* object )
  {
    // FunctorDelegate owns the object but we're the only one who knows the real type so need
    // to delete by "downcasting" from void* to the correct type
    delete reinterpret_cast< T* >( object );
  }
};

/**
 * Used to connect a void() functor to a signal via BaseObject::SignalConnect()
 */
class FunctorDelegate
{
public:

  /**
   * Constructor which copies a function object.
   * @param[in] object The object to copy.
   */
  template< class T >
  static FunctorDelegate* New( const T& object )
  {
    return new FunctorDelegate( reinterpret_cast< void* >( new T( object ) ), // copy the object
                            reinterpret_cast< FunctorDelegate::Dispatcher >( &FunctorDispatcher<T>::Dispatch ),
                            reinterpret_cast< FunctorDelegate::Destructor >( &FunctorDestroyer<T>::Delete ) );
  }

  /**
   * Non-virtual destructor; not intended as a base class.
   */
  ~FunctorDelegate();

  /**
   * Function to call the function or member function dispatcher
   */
  void Execute();

private:

  /// Used to call the correct member function
  typedef void (*Dispatcher)( void* objectPtr );

  /// Used to destroy mObjectPointer (NULL if not mObjectPointer is not owned)
  typedef void(*Destructor)( void* objectPtr );

  // Not defined
  FunctorDelegate( const FunctorDelegate& rhs );
  const FunctorDelegate& operator=( const FunctorDelegate& rhs );

  /**
   * Private constructor.
   * @param[in] objectPtr A newly allocated object (takes ownership)
   * @param dispatcher Used to call the actual object.
   * @param destructor Used to delete the owned object.
   */
  FunctorDelegate( void* objectPtr, Dispatcher dispatcher, Destructor destructor );

public: // Data for deriving classes & Dispatchers

  void* mObjectPointer;                 ///< Object whose member function will be called. Not owned if mDestructorDispatcher is NULL.
  Dispatcher mMemberFunctionDispatcher; ///< Dispatcher for member functions
  Destructor mDestructorDispatcher;     ///< Destructor for owned objects. NULL if mDestructorDispatcher is not owned.
};

} // namespace DALI

/**
 * @}
 */
#endif // __DALI_FUNCTOR_DELEGATE_H__
