#ifndef __DALI_REF_OBJECT_H__
#define __DALI_REF_OBJECT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali
{

class Value;

/**
 * @brief Base class for reference counted objects.
 *
 * Typically this should be used with a Boost instrusive pointer,
 * instead of calling Reference() and Unreference() methods directly.
 */
class DALI_IMPORT_API RefObject
{
public:

  /**
   * @brief Increment the object's reference count.
   */
  void Reference();

  /**
   * @brief Decrement the object's reference count.
   *
   * When the reference count drops to zero, the object will self-destruct.
   */
  void Unreference();

  /**
   * @brief Retrieve the object's reference count.
   *
   * @return The reference count
   */
  int ReferenceCount();

protected:

  /**
   * @brief Default constructor.
   */
  RefObject();

  /**
   * @brief RefObject is intended as a base class.
   *
   * A RefObject may only be deleted when its reference count is zero.
   */
  virtual ~RefObject();

  /**
   * @brief Copy constructor.
   *
   * The newly copied object will have a reference count of zero.
   * @param[in] rhs The object to copy
   */
  RefObject(const RefObject& rhs);

  /**
   * @brief Assignment operator.
   *
   * The newly copied object will have a reference count of zero.
   * @param[in] rhs The object to copy
   * @return a reference to this
   */
  RefObject& operator=(const RefObject& rhs);

private:

  volatile int mCount; ///< Reference count
};

} // namespace Dali

#endif // __DALI_REF_OBJECT_H__
