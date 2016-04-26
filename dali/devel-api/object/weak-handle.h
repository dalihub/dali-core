#ifndef __DALI_WEAK_HANDLE_H__
#define __DALI_WEAK_HANDLE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/actors/custom-actor.h>

namespace Dali
{

/**
 * @brief Base class to store a weak pointer to an internal Dali object. The handle to the object
 * can be accessed if the object exists, and such access is not reference counted. When the object
 * is deleted, the weak pointer will be set to NULL, and any further attmpt to access to a deleted
 * object will return an empty handle.
 *
 */
class DALI_IMPORT_API WeakHandleBase
{

public:

  /**
   * @brief Default constructor which provides an uninitialized Dali::WeakHandleBase.
   */
  WeakHandleBase();

  /**
   * @brief This constructor creates a weak handle of the Dali object.
   *
   * @param [in] handle A reference to the handle of the Dali object.
   */
  WeakHandleBase( Handle& handle );

  /**
   * @brief Destructor to free resources.
   */
  ~WeakHandleBase();

  /**
   * @brief Copy constructor.
   *
   * @param [in] handle A reference to the copied WeakHandleBase
   */
  WeakHandleBase(const WeakHandleBase& handle);

  /**
   * @brief Assignment operator.
   *
   * It makes this WeakHandleBase point to the same internal Dali object as the copied WeakHandleBase
   * @param [in] rhs  A reference to the copied WeakHandleBase
   * @return A reference to this WeakHandleBase
   */
  WeakHandleBase& operator=( const WeakHandleBase& rhs );

  /**
   * @brief Equality operator overload.
   *
   * @param [in] rhs A reference to the compared WeakHandleBase.
   * @return true if the handle points to the same Dali resource, or if both are uninitialized.
   */
  bool operator==(const WeakHandleBase& rhs) const;

  /**
   * @brief Inequality operator overload.
   *
   * @param [in] rhs A reference to the compared WeakHandleBase.
   * @return true if the handle points to the different Dali resources.
   */
  bool operator!=(const WeakHandleBase& rhs) const;

  /**
   * @brief Gets the handle to the Dali object.
   *
   * @return The handle of the Dali object pointed by this WeakHandleBase or an empty handle if the Dali object doesn't exist.
   */
  Handle GetBaseHandle() const;

  /**
   * @brief Resets this weak handle to not point to any Dali object
   */
  void Reset();


protected:

  struct Impl;
  Impl* mImpl;
};


/**
 * @brief Weak handle for the given type of Dali object.
 */
template < class T >
class WeakHandle : public WeakHandleBase
{
public:

  /**
   * @copydoc Dali::WeakHandleBase::WeakHandleBase()
   */
  WeakHandle()
  : WeakHandleBase()
  {
  }

  /**
   * @copydoc Dali::WeakHandleBase::WeakHandleBase(Handle&)
   */
  WeakHandle( T& handle )
  : WeakHandleBase( handle )
  {
  }

  /**
   * @copydoc Dali::WeakHandleBase::~WeakHandleBase()
   */
  ~WeakHandle() {}

  /**
   * @copydoc Dali::WeakHandleBase::WeakHandleBase(const WeakHandleBase&)
   */
  WeakHandle(const WeakHandle& handle)
  : WeakHandleBase( handle )
  {
  }

  /**
   * @copydoc Dali::WeakHandleBase::operator=()
   */
  WeakHandle& operator=( const WeakHandle& rhs )
  {
    WeakHandleBase::operator=(rhs);
    return *this;
  }

  /**
   * @copydoc Dali::WeakHandleBase::operator==()
   */
  bool operator==(const WeakHandle& rhs) const
  {
    return WeakHandleBase::operator==(rhs);
  }

  /**
   * @copydoc Dali::WeakHandleBase::operator!=()
   */
  bool operator!=(const WeakHandle& rhs) const
  {
    return WeakHandleBase::operator!=(rhs);
  }

  /**
   * @copydoc Dali::WeakHandleBase::GetBaseHandle()
   */
  T GetHandle() const
  {
    Handle handle( GetBaseHandle() );
    if( handle )
    {
      return DownCast< T >( handle );
    }
    else
    {
      return T();
    }
  }
};

} // namespace Dali

#endif // __DALI_WEAK_HANDLE_H__
