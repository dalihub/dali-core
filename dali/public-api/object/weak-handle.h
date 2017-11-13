#ifndef DALI_WEAK_HANDLE_H
#define DALI_WEAK_HANDLE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Base class to store a weak pointer to an internal DALi object.
 *
 * The handle to the object can be accessed if the object exists, and such access is not reference counted.
 * When the object is deleted, the weak pointer will be set to NULL, and any further attempt to access to a
 * deleted object will return an empty handle.
 * @SINCE_1_2.60
 */
class DALI_IMPORT_API WeakHandleBase
{

public:

  /**
   * @brief Default constructor which provides an uninitialized Dali::WeakHandleBase.
   * @SINCE_1_2.60
   */
  WeakHandleBase();

  /**
   * @brief This constructor creates a weak handle of the DALi object.
   *
   * @SINCE_1_2.60
   * @param [in] handle A reference to the handle of the DALi object
   */
  WeakHandleBase( Handle& handle );

  /**
   * @brief Destructor to free resources.
   * @SINCE_1_2.60
   */
  ~WeakHandleBase();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_2.60
   * @param [in] handle A reference to the copied WeakHandleBase
   */
  WeakHandleBase(const WeakHandleBase& handle);

  /**
   * @brief Assignment operator.
   *
   * It makes this WeakHandleBase point to the same internal DALi object as the copied WeakHandleBase
   * @SINCE_1_2.60
   * @param [in] rhs  A reference to the copied WeakHandleBase
   * @return A reference to this WeakHandleBase
   */
  WeakHandleBase& operator=( const WeakHandleBase& rhs );

  /**
   * @brief Equality operator overload.
   *
   * @SINCE_1_2.60
   * @param [in] rhs A reference to the compared WeakHandleBase
   * @return true if the handle points to the same DALi resource, or if both are uninitialized
   */
  bool operator==(const WeakHandleBase& rhs) const;

  /**
   * @brief Inequality operator overload.
   *
   * @SINCE_1_2.60
   * @param [in] rhs A reference to the compared WeakHandleBase
   * @return true if the handle points to the different DALi resources
   */
  bool operator!=(const WeakHandleBase& rhs) const;

  /**
   * @brief Gets the handle to the DALi object.
   *
   * @SINCE_1_2.60
   * @return The handle of the DALi object pointed by this WeakHandleBase or an empty handle if the DALi object doesn't exist
   */
  Handle GetBaseHandle() const;

  /**
   * @brief Resets this weak handle to not point to any DALi object
   * @SINCE_1_2.60
   */
  void Reset();


protected:

  /// @cond internal
  struct Impl;
  Impl* mImpl;
  /// @endcond
};


/**
 * @brief Weak handle for the given type of DALi object.
 * @SINCE_1_2.60
 * @see WeakHandleBase
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

/**
 * @}
 */
} // namespace Dali

#endif // DALI_WEAK_HANDLE_H
