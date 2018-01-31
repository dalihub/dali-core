#ifndef DALI_GRAPHICS_UTILITY_STRONG_TYPE_H
#define DALI_GRAPHICS_UTILITY_STRONG_TYPE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <utility>
#include <ostream>

namespace Dali
{
namespace Graphics
{
namespace Utility
{

/**
 * @brief Type wrapper to enforce scemantics in other types
 */
template<typename T, typename Tag>
class StrongType final
{
public:
  explicit StrongType( T value ) noexcept : mValue( std::move( value ) )
  {
  }

  StrongType() = default;

  const T& Get() const noexcept
  {
    return mValue;
  }

  T& Get() noexcept
  {
    return mValue;
  }

  const T& operator*() const noexcept
  {
    return mValue;
  }

  T& operator*() noexcept
  {
    return mValue;
  }

private:
  T mValue;
};

template<typename T, typename Tag>
std::ostream& operator<<( std::ostream& os, const StrongType<T, Tag>& value )
{
  os << value.Get();
  return os;
}

} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_UTILITY_STRONG_TYPE_H