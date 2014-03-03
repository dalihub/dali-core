#ifndef __DALI_INTRUSIVE_PTR_H__
#define __DALI_INTRUSIVE_PTR_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

/**
 * Templated class to emulate a sub-set of functions served by deprecating boost::intrusive_ptr
 * Uses the Dali:Refobject type supply actual reference counting
 * The object is responsible for destroying itself
 */
template<typename T>
class IntrusivePtr
{

public:

  /**
  * Standard constructor to unassigned object
  */
  IntrusivePtr() : mPtr( 0 ) {}

  /**
   * Constructor to attach existing object
   * @param p pointer to object,
   */
  IntrusivePtr( T* p ) : mPtr( p )
  {
    if( mPtr )
    {
      mPtr->Reference();
    }
  }

  /**
   * Copy constructor
   * @param rhs const reference to an IntrusivePtr
   * @tparam U reference counter object type
   */
  template<typename U>
  IntrusivePtr( IntrusivePtr<U> const& rhs ) : mPtr( rhs.Get() )
  {
    if( mPtr )
    {
      mPtr->Reference();
    }
  }

  /**
   * Copy constructor
   */
  IntrusivePtr( IntrusivePtr const& rhs ) : mPtr( rhs.mPtr )
  {
    if( mPtr )
    {
      mPtr->Reference();
    }
  }

  /**
   * Destructor
   * Object will self-destruct if reference count is zero
   */
  ~IntrusivePtr()
  {
    if( mPtr )
    {
      mPtr->Unreference();
    }
  }

  /**
  * Get pointer to reference counted object
  * @return pointer to reference counted object
  */
  T* Get() const
  {
    return mPtr;
  }

  /**
  * Pointer operator ovveride
  * @return pointer to reference counted object
  */
  T* operator->() const
  {
    return mPtr;
  }

  /**
  * Dereference operator override
  * @return reference to reference counted object
  */
  T& operator*() const
  {
    return *mPtr;
  }

  /**
  * Assignment operator
  * @param rhs const reference to intrusive pointer
  * @return reference to reference counted object
  */
  IntrusivePtr& operator=( IntrusivePtr const& rhs )
  {
    IntrusivePtr( rhs ).Swap( *this );
    return *this;
  }

  /**
  * Assignment operator
  * @param rhs pointer to object to wrap
  */
  IntrusivePtr& operator=( T* rhs )
  {
    IntrusivePtr( rhs ).Swap( *this );
    return *this;
  }

  /**
   * Reset intrusive pointer
   */
  void Reset()
  {
    IntrusivePtr().Swap( *this );
  }

  /**
   * Reset intrusive pointer with reference counted object
   * @param rhs pointer to object
   */
  void Reset( T* rhs )
  {
    IntrusivePtr( rhs ).Swap( *this );
  }

  // IntrusivePtr comparisons - This is a variation of the safe bool idiom

  /**
   * Pointer-to-member type. Objects can be implicitly converted to this for validity checks.
   */
  typedef void (IntrusivePtr::*BooleanType)() const;

  /**
   * Converts an object handle to a BooleanType.
   * This is useful for checking whether the handle is NULL.
   */
  operator BooleanType() const
  {
    return mPtr ? &IntrusivePtr::ThisIsSaferThanReturningVoidStar : 0;
  }

private:

  /**
   * Used by the safe bool idiom.
   */
  void ThisIsSaferThanReturningVoidStar() const {}

  /**
   * Internal swap function
   */
  void Swap( IntrusivePtr& rhs )
  {
    T* tmp = mPtr;
    mPtr = rhs.mPtr;
    rhs.mPtr = tmp;
  }

  T* mPtr;  // pointer to RefObject
};

/**
 * Comparison overrides of objects wrapped by intrusive pointers
 *
 * @param lhs intrusive pointer to compare with
 * @param rhs intrusive pointer to compare against
 */
template<typename T, typename U> inline bool operator==( IntrusivePtr<T>const& lhs, IntrusivePtr<U>const& rhs )
{
  return lhs.Get() == rhs.Get();
}

template<typename T, typename U> inline bool operator!=( IntrusivePtr<T>const& lhs, IntrusivePtr<U>const &rhs)
{
  return lhs.Get() != rhs.Get();
}

/**
 * Comparison overrides of objects wrapped by intrusive pointers
 *
 * @param lhs intrusive pointer to compare with
 * @param rhs object compare against
 */
template<typename T, typename U> inline bool operator==( IntrusivePtr<T>const& lhs, U* rhs )
{
  return lhs.Get() == rhs;
}

template<typename T, typename U> inline bool operator!=( IntrusivePtr<T>const& lhs, U* rhs )
{
  return lhs.Get() != rhs;
}

/**
 * Comparison overrides of objects wrapped by intrusive pointers
 *
 * @param lhs object to compare with
 * @param rhs intrusive pointer to compare against
 */
template<typename T, typename U> inline bool operator==( T* lhs, IntrusivePtr<U>const& rhs )
{
  return lhs == rhs.Get();
}

template<typename T, typename U> inline bool operator!=( T* lhs, IntrusivePtr<U>const& rhs )
{
  return lhs != rhs.Get();
}

/**
 * Get pointer to reference counted object (Dali camel case variant)
 *
 * @param rhs intrusive pointer wrapping object
 * @return pointer to object
 */
template<typename T>inline T* GetPointer(IntrusivePtr<T> const& rhs)
{
  return rhs.Get();
}

/**
 * Get pointer to reference counted object (boost:: naming convention)
 *
 * @param rhs intrusive pointer wrapping object
 * @return pointer to object
 */
template<typename T>inline T* get_pointer(IntrusivePtr<T> const& rhs)
{
  return rhs.Get();
}

}

#endif /* __DALI_INTRUSIVE_PTR_H__ */
