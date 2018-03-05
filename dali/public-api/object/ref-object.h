#ifndef __DALI_REF_OBJECT_H__
#define __DALI_REF_OBJECT_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

class Value;

/**
 * @brief Base class for reference counted objects.
 *
 * Typically this should be used with an intrusive pointer,
 * instead of calling Reference() and Unreference() methods directly.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API RefObject
{
public:

  /**
   * @brief Increments the object's reference count.
   * @SINCE_1_0.0
   */
  void Reference();

  /**
   * @brief Decrements the object's reference count.
   *
   * When the reference count drops to zero, the object will self-destruct.
   * @SINCE_1_0.0
   */
  void Unreference();

  /**
   * @brief Retrieves the object's reference count.
   *
   * @SINCE_1_0.0
   * @return The reference count
   */
  int ReferenceCount();

protected:

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  RefObject();

  /**
   * @brief RefObject is intended as a base class.
   *
   * A RefObject may only be deleted when its reference count is zero.
   * @SINCE_1_0.0
   */
  virtual ~RefObject();

  /**
   * @brief Copy constructor.
   *
   * The newly copied object will have a reference count of zero.
   * @SINCE_1_0.0
   * @param[in] rhs The object to copy
   */
  RefObject(const RefObject& rhs);

  /**
   * @brief Assignment operator.
   *
   * The newly copied object will have a reference count of zero.
   * @SINCE_1_0.0
   * @param[in] rhs The object to copy
   * @return A reference to this
   */
  RefObject& operator=(const RefObject& rhs);

private:

  volatile int mCount; ///< Reference count
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_REF_OBJECT_H__
