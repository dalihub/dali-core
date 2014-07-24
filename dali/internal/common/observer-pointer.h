#ifndef __DALI_INTERNAL_OBSERVER_POINTER_H__
#define __DALI_INTERNAL_OBSERVER_POINTER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/pointer-observable.h>

namespace Dali
{

namespace Internal
{

template < typename T >
class PointerObservable;

template < typename T >
class ObserverPointer
{
public:

  /**
   * Default constructor. Creates an observer pointer pointing  to a null object.
   */
  ObserverPointer()
  {
    mObservable = NULL;
  }

  /**
   * Constructor from a PointerObservable. Create an observer pointer connected to the observable.
   * @param observable to connect to.
   */
  ObserverPointer( PointerObservable< T >& observable )
  {
    // Initialise the ObserverPointer
    Init( &observable );
  }

  /**
   * Copy constructor. Creates a new observer pointer connected the same observer.
   * @param observerPointer to copy data from.
   */
  ObserverPointer( const ObserverPointer< T >& observerPointer )
  {
    // Initialise the ObserverPointer
    Init( observerPointer.mObservable );
  }

  /**
   * Assignment operator. Connects this observer with the same observable.
   * @param observerPointer connected to the observable.
   */
  ObserverPointer< T >& operator=( const ObserverPointer< T >& observerPointer )
  {
    // Initialise the ObserverPointer
    Init( observerPointer.mObservable );

    // return self
    return *this;
  }

  /**
   * Destructor.
   */
  ~ObserverPointer()
  {
    if ( mObservable != NULL )
    {
      mObservable->Unbind( this );
      mObservable = NULL;
    }
  }

  /**
   * Indirection operator.
   * @return a reference to the object referenced by the connected observable.
   */
  T& operator*()
  {
    DALI_ASSERT_DEBUG( mObservable != NULL );
    return *(*mObservable);
  }

  /**
   * Const indirection operator.
   * @return a const reference to the object referenced by the connected observable.
   */
  const T& operator*() const
  {
    DALI_ASSERT_DEBUG( mObservable != NULL );
    return *(*mObservable);
  }

  /**
   * Pointer operator.
   * @return a pointer to the object referenced by the connect observer.
   */
  T* operator->()
  {
    // Operator bool() should be used to check if the Observable is still valid.
    DALI_ASSERT_DEBUG( mObservable != NULL );
    return mObservable->operator->();
  }

  /**
   * Const pointer operator.
   * @return a const pointer to the object referenced by the connect observer.
   */
  const T* operator->() const
  {
    // Operator bool() should use to check if the Observable is still valid.
    DALI_ASSERT_DEBUG( mObservable != NULL );
    return mObservable->operator->();
  }

  /**
   * Boolean cast.
   * @return true if it is connected to an observable and there is an object referenced.
   */
  operator bool() const
  {
    return ( mObservable != NULL && *mObservable );
  }

  /**
   * Returns the Observable pointed by this observer.
   * @return observable pointed by this observer.
   */
  PointerObservable< T >& GetObservable()
  {
    return *mObservable;
  }

private:

  friend class PointerObservable< T >;

  void Reset()
  {
    mObservable = NULL;
  }

  void Init( PointerObservable< T >* observablePointer )
  {
    // keep a raw pointer to the observable
    mObservable = observablePointer;

    // Bind this Pointer to the Observable
    if ( mObservable != NULL )
    {
      mObservable->Bind( this );
    }
  }

  // data
  PointerObservable< T >* mObservable; ///< Raw pointer to the observable object

};

} // namespace Internal

} // namespace Dali

#endif //__DALI_INTERNAL_OBSERVER_POINTER_H__
