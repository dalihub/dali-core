#ifndef __REF_COUNTED_DALI_VECTOR_H__
#define __REF_COUNTED_DALI_VECTOR_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

/**
 * @brief A reference counting wrapper for a vector class that allows
 * a set of referencing smart pointers to collaborate in managing its
 * lifetime and eventually cleaning it up.
 *
 * This should only be allocated on the new/delete heap, not a thread's
 * stack.
 * @tparam T type of the data that the vector holds
 */
template< typename T >
class RefCountedVector : public RefObject
{
public:
  /**
   * @brief Construct empty vector.
   */
  RefCountedVector()
  {
  }

  /**
   * @brief Get the referenced vector.
   *
   * @return A reference to the vector that this object wraps.
   */
  Vector< T >& GetVector()
  {
    return mVector;
  }

protected:
  virtual ~RefCountedVector()
  {
  }

private:
  // Disable copy-constructing and copying:
  RefCountedVector(const RefCountedVector &); ///< Undefined
  RefCountedVector & operator = (const RefCountedVector &); ///< Undefined

  Vector< T > mVector; ///< The vector of data
};

} // namespace Dali

#endif /* __REF_COUNTED_DALI_VECTOR_H__ */
