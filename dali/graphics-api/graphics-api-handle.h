#ifndef DALI_GRAPHICS_API_HANDLE_H
#define DALI_GRAPHICS_API_HANDLE_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Graphics
{
namespace API
{

template< typename T >
class Handle
{
public:
  constexpr Handle(T& reference) : mObject(reference)
  {
  }

  Handle(const Handle&) = default;
  Handle& operator=(const Handle&) = default;

  Handle(Handle&&) = default;
  Handle& operator=(Handle&&) = default;

  ~Handle() = default;

  T* operator->()
  {
    return &mObject;
  }

  constexpr const T* operator->() const
  {
    return &mObject;
  }

  T& operator()()
  {
    return mObject;
  }

  constexpr const T& operator()() const
  {
    return mObject;
  }

private:
  T& mObject;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_HANDLE_H
