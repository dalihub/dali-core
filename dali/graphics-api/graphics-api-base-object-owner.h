#ifndef DALI_GRAPHICS_API_BASE_OBJECT_OWNER_H
#define DALI_GRAPHICS_API_BASE_OBJECT_OWNER_H

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

#include <dali/graphics-api/graphics-api-base-factory.h>

namespace Dali
{
namespace Graphics
{
namespace API
{
template< typename T >
class ObjectOwnerBase
{
public:
  using Handle = size_t;

  /**
   * @brief Create an new object using the provided factory and store it.
   *
   * @param[in] factory Factory that will be used to create the object\
   * @return Handle that identifies the object
   */
  virtual Handle CreateObject(const BaseFactory< T >& factory) = 0;

  // @brief default constructor
  ObjectOwnerBase() = default;

  // @brief virtual destructor
  virtual ~ObjectOwnerBase() = default;

  // delete copy constructor/assignment
  ObjectOwnerBase(const ObjectOwnerBase&) = delete;
  ObjectOwnerBase& operator=(const ObjectOwnerBase&) = delete;

protected:
  // protected move constructor/assignment
  ObjectOwnerBase(ObjectOwnerBase&&) = default;
  ObjectOwnerBase& operator=(ObjectOwnerBase&&) = default;
};
} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_BASE_OBJECT_OWNER_H
