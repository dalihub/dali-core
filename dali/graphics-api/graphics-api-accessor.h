#ifndef DALI_GRAPHICS_API_ACCESSOR_H
#define DALI_GRAPHICS_API_ACCESSOR_H
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

// EXTERNAL INCLUDES
#include <memory>

#include <dali/graphics/graphics-object-owner.h>

namespace Dali
{
namespace Graphics
{
namespace API
{

template< typename T >
class Accessor
{
public:
  Accessor(ObjectOwner< T >& owner, typename ObjectOwner< T >::Handle handle)
  : mOwner(owner),  // save owner to access object
    mHandle(handle) // handle to the object
  {
  }

  bool Exists() const
  {
    return mOwner.Contains(mHandle);
  }
  operator bool() const
  {
    return Exists();
  }

  T& Get()
  {
    return mOwner[mHandle];
  }

  const T& Get() const
  {
    return mOwner[mHandle];
  }

  Accessor(const Accessor&) = default;
  Accessor& operator=(const Accessor&) = default;

  Accessor(Accessor&&) = default;
  Accessor& operator=(Accessor&&) = default;

private:
  ObjectOwner< T >&                 mOwner;
  typename ObjectOwner< T >::Handle mHandle;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_ACCESSOR_H
