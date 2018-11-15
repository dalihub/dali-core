#ifndef __DALI_INTERNAL_OWNER_POINTER_H__
#define __DALI_INTERNAL_OWNER_POINTER_H__

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

namespace Internal
{

template< typename T >
class OwnerPointer
{
public:

  /**
   * Default constructor. Creates an OwnerPointer that does not own any object.
   * @note This does not protect against two different OwnerPointers pointing to the same object.
   *       Both OwnerPointers will try to release the memory of the same object in that case which
   *       could lead to a crash.
   */
  OwnerPointer()
  : mObject( nullptr )
  {
  }

  /**
   * Constructor. Creates an OwnerPointer that owns the object.
   * @param[in] object A pointer to a heap allocated object.
   */
  OwnerPointer( T* object )
  : mObject( object )
  {
  }

  /**
   * Copy constructor. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerPointer( const OwnerPointer& other )
  : OwnerPointer( static_cast< OwnerPointer&& >( const_cast<OwnerPointer&>( other ) ) ) // Remove constness & cast to rvalue to use the move constructor
  {
    // other needs to be const for compiler to pick up this as copy constructor;
    // though we are using this as move as there can only be one owner
  }

  /**
   * Move constructor. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerPointer( OwnerPointer&& other )
  : mObject( nullptr )
  {
    Swap( other );
  }

  /**
   * Assignment operator. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerPointer& operator=( OwnerPointer& other )
  {
    if( this != &other )    // no self-assignment
    {
      delete mObject;
      mObject = other.mObject;
      other.mObject = nullptr;
    }

    // return self
    return *this;
  }

  /**
   * Move assignment operator. Passes the ownership of a pointer to another.
   * @param[in] other The pointer that gives away the ownership.
   */
  OwnerPointer& operator=( OwnerPointer&& other )
  {
    // Reuse operator=
    return operator=( other );
  }

  /**
   * Assignment operator. Takes the ownership of the object.
   * If it owns an object already, it will be deleted.
   * @param[in] pointer A pointer to a heap allocated object.
   */
  OwnerPointer& operator=( T* pointer )
  {
    if( mObject != pointer )
    {
      Reset();
      mObject = pointer;
    }

    return *this;
  }

  /**
   * Destructor.
   */
  ~OwnerPointer()
  {
    Reset();
  }

  /**
   * Indirection operator.
   * @return a reference to the object.
   */
  T& operator*()
  {
    DALI_ASSERT_DEBUG( mObject );

    return *mObject;
  }

  /**
   * Const indirection operator.
   * @return a reference to the object from const OwnerPointer.
   */
  T& operator*() const
  {
    DALI_ASSERT_DEBUG( mObject );

    // Pointer semantics: A const pointer does not mean const data.
    return const_cast< T& >( *mObject );
  }

  /**
   * Pointer operator.
   * @return a pointer to the object.
   */
  T* operator->()
  {
    return mObject;
  }

  /**
   * Const pointer operator.
   * @return a pointer to the object referenced by a const OwnerPointer.
   */
  T* operator->() const
  {
    // Pointer semantics: A const pointer does not mean const data.
    return const_cast< T* >( mObject );
  }

  /**
   * Compare with a raw pointer.
   * @return true if the raw pointer matches the one owned by this object.
   */
  bool operator==( const T* pointer )
  {
    return ( mObject == pointer );
  }

  /**
   * Reset the pointer, deleting any owned object.
   */
  void Reset()
  {
    delete mObject;
    mObject = nullptr;
  }

  /**
   * Release the ownership, it does not delete the object.
   * @return a pointer to the object.
   */
  T* Release()
  {
    T* tmp = mObject;
    mObject = nullptr;
    return tmp;
  }

  /**
   * Returns a const pointer to the object owned.
   * @return a const pointer to the object.
   */
  const T* Get() const
  {
    return mObject;
  }

  /**
   * Swap owned objects
   * @param[in] other The pointer to swap the owned objects with.
   */
  void Swap( OwnerPointer& other )
  {
    if( this != &other )
    {
      T* tmp = mObject;
      mObject = other.mObject;
      other.mObject = tmp;
    }
  }

  // Handle comparisons - This is a variation of the safe bool idiom

  /**
   * Pointer-to-member type. Objects can be implicitly converted to this for validity checks.
   */
  typedef void (OwnerPointer::*BooleanType)() const;

  /**
   * Converts an object handle to a BooleanType.
   * This is useful for checking whether the handle is NULL.
   */
  operator BooleanType() const
  {
    return ( mObject != nullptr ) ? &OwnerPointer::ThisIsSaferThanReturningVoidStar : nullptr;
  }

private:

  /**
   * Used by the safe bool idiom.
   */
  void ThisIsSaferThanReturningVoidStar() const {}

  // data
  T* mObject; ///< Raw pointer to the object
};

} // namespace Internal

} // namespace Dali

#endif //__DALI_INTERNAL_OWNER_POINTER_H__
