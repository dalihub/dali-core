#ifndef DALI_GRAPHICS_OBJECT_OWNER_H
#define DALI_GRAPHICS_OBJECT_OWNER_H

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
#include <cassert>
#include <memory>
#include <unordered_map>
#include <utility>

#include <dali/graphics-api/graphics-api-base-object-owner.h>

namespace Dali
{
namespace Graphics
{
template< typename T >
class ObjectOwner final : public API::ObjectOwnerBase< T >
{
public:
  using Type   = T;
  using Handle = size_t;

  constexpr ObjectOwner()
  : mObjects{},      // Initialise without objects
    mNextObjecId{0u} // Initialize to zero
  {
  }

  Handle CreateObject(const API::BaseFactory< T >& factory)
  {
    auto id = mNextObjecId++;

    auto&& object = factory.Create();
    auto   it     = mObjects.emplace(std::make_pair(id, std::move(object)));
    assert(it.second);

    return Handle{id};
  }

  T& operator[](Handle handle)
  {
    return *mObjects[handle];
  }

  const T& operator[](Handle handle) const
  {
    return *mObjects[handle];
  }

  bool Contains(Handle handle) const
  {
    return mObjects.end() != mObjects.find(handle);
  }

  ObjectOwner(const ObjectOwner&) = delete;
  ObjectOwner& operator=(const ObjectOwner&) = delete;

  ObjectOwner(ObjectOwner&&) = default;
  ObjectOwner& operator=(ObjectOwner&&) = default;

  virtual ~ObjectOwner() = default;

private:
  std::unordered_map< size_t, std::unique_ptr< T > > mObjects;
  size_t mNextObjecId;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_OBJECT_OWNER_H
