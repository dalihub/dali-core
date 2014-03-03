#ifndef __DALI_REF_OBJECT_H__
#define __DALI_REF_OBJECT_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali DALI_IMPORT_API
{

class Value;

/**
 * Base class for reference counted objects.
 * Typically this should be used with a Boost instrusive pointer,
 * instead of calling Reference() and Unreference() methods directly.
 */
class RefObject
{
public:

  /**
   * Increment the object's reference count.
   */
  void Reference();

  /**
   * Decrement the object's reference count.
   * When the reference count drops to zero, the object will self-destruct.
   */
  void Unreference();

  /**
   * Retrieve the object's reference count.
   * @return The reference count
   */
  int ReferenceCount();

protected:

  /**
   * Default constructor
   */
  RefObject();

  /**
   * RefObject is intended as a base class.
   * A RefObject may only be deleted when its reference count is zero.
   */
  virtual ~RefObject();

  /**
   * Copy constructor.
   * The newly copied object will have a reference count of zero.
   */
  RefObject(const RefObject& rhs);

  /**
   * Assignment operator.
   * The newly copied object will have a reference count of zero.
   */
  RefObject& operator=(const RefObject& rhs);

private:

  volatile int mCount;
};

} // namespace Dali


/**
 * @}
 */
#endif // __DALI_REF_OBJECT_H__
