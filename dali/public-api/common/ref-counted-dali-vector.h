#ifndef __REF_COUNTED_DALI_VECTOR_H__
#define __REF_COUNTED_DALI_VECTOR_H__

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali DALI_IMPORT_API
{

/**
 * A reference counting wrapper for a vector class that allows a set of
 * referencing smart pointers to collaborate in managing its lifetime
 * and eventually cleaning it up.
 * This should only be allocated on the new/delete heap, not a thread's
 * stack.
 * @param T type of the data that the vector holds
 */
template< typename T >
class RefCountedVector : public RefObject
{
public:
  /** Construct empty vector. */
  RefCountedVector() {}

  /** @return A reference to the vector that this object wraps. */
  Vector< T >& GetVector() { return mVector; }

protected:
  virtual ~RefCountedVector() {}

private:
  // Disable copy-constructing and copying:
  RefCountedVector(const RefCountedVector &);
  RefCountedVector & operator = (const RefCountedVector &);

  Vector< T > mVector;
};

} // namespace Dali

#endif /* __REF_COUNTED_DALI_VECTOR_H__ */
