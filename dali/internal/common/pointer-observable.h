#ifndef __DALI_INTERNAL_OBSERVABLE_POINTER_H__
#define __DALI_INTERNAL_OBSERVABLE_POINTER_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/common/observer-pointer.h>

namespace Dali
{

namespace Internal
{

template < typename T >
class ObserverPointer;

template < typename T >
class PointerObservable
{
public:

  typedef std::vector< ObserverPointer< T >* > ObserverContainer;

  /**
   * Default constructor. Creates an observable that does not reference any object.
   */
  PointerObservable()
  : mObjectPtr( NULL )
  {
  }

  /**
   * Constructor. Creates an observable for the object. The object will be deleted when the observable is deleted.
   */
  PointerObservable( T* object )
  : mObjectPtr( object )
  {
  }

  /**
   * Assignment operator. Changes the object referenced by the observable.
   */
  PointerObservable< T >& operator=( T* objectPtr )
  {
    mObjectPtr = objectPtr;
    return *this;
  }

  /**
   * Destructor. Disconnect the observers connected to this observable and free's the referenced object.
   */
  virtual ~PointerObservable()
  {
    for ( typename ObserverContainer::iterator it = mObservers.begin();
        it != mObservers.end(); ++it )
    {
      (*it)->Reset();
    }
    delete mObjectPtr;
    mObjectPtr = NULL;
  }

  /**
   * Indirection operator.
   * @return a reference to the object.
   */
  T& operator*()
  {
    DALI_ASSERT_DEBUG( mObjectPtr != NULL );
    return *mObjectPtr;
  }

  /**
   * Const indirection operator.
   * @return a const reference to the object.
   */
  const T& operator*() const
  {
    DALI_ASSERT_DEBUG( mObjectPtr != NULL );
    return *mObjectPtr;
  }

  /**
   * Pointer operator.
   * @return a pointer to the object.
   */
  T* operator->()
  {
    return mObjectPtr;
  }

  /**
   * Const pointer operator.
   * @return a const pointer to the object.
   */
  const T* operator->() const
  {
    return mObjectPtr;
  }

  /**
   * Boolean cast.
   * @return true if there is an object referenced.
   */
  operator bool() const
  {
    return mObjectPtr != NULL;
  }

private:

  friend class ObserverPointer< T >;

  void Bind( ObserverPointer< T >* observerPointer )
  {
    mObservers.push_back( observerPointer );
  }

  void Unbind( ObserverPointer< T >* observer )
  {
    for ( typename ObserverContainer::iterator it = mObservers.begin();
        it != mObservers.end(); ++it )
    {
      if ( observer == *it )
      {
        mObservers.erase( it );
        return;
      }
    }
  }

private:

  T* mObjectPtr;
  ObserverContainer mObservers;

private: //disable

  PointerObservable( const PointerObservable< T >& );
  PointerObservable< T >& operator=( const PointerObservable< T >& );

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_OBSERVABLE_POINTER_H__
