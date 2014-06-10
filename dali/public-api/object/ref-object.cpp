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

// CLASS HEADER
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

RefObject::RefObject()
: mCount(0)
{
}

RefObject::RefObject(const RefObject&)
: mCount(0) // Do not copy the reference count
{
}

RefObject::~RefObject()
{
  DALI_ASSERT_DEBUG(mCount == 0);
}

RefObject& RefObject::operator=(const RefObject&)
{
  // Do not copy the reference count
  return *this;
}

void RefObject::Reference()
{
  // gcc > 4.1 builtin atomic add and fetch (++mCount; return mCount)
  __sync_add_and_fetch(&mCount, 1);
}

void RefObject::Unreference()
{
  // gcc > 4.1 builtin atomic subtract and fetch (--mCount; return mCount)
  if (__sync_sub_and_fetch(&mCount, 1) == 0)
  {
    delete this;
  }
}

int RefObject::ReferenceCount()
{
  return mCount;
}

} // namespace Dali


