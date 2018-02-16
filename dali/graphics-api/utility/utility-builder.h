#ifndef DALI_GRAPHICS_UTILITY_BUILDER_H
#define DALI_GRAPHICS_UTILITY_BUILDER_H

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
#include <cstddef>
#include <utility>
#include <tuple>

// INTERNAL INCLUDES
#include <dali/graphics-api/utility/utility-traits.h>

namespace Dali
{
namespace Graphics
{
namespace Utility
{

template<typename T, typename... Params>
class Builder
{
public:
  template<typename... Values>
  Builder( Values&&... values ) : mParameters{}
  {
    SetParameters( values... );
  }

  template<typename Return>
  operator Return()
  {
    return Build( std::index_sequence_for<Params...>{} );
  }

  auto Build()
  {
    return Build( std::index_sequence_for<Params...>{} );
  }

  template<typename V>
  void Set( V&& value )
  {
    SetParameters( std::forward<V>( value ) );
  }

private:
  void SetParameters() const {};

  template<typename V>
  void SetParameters( V&& value )
  {
    std::get<std::remove_reference_t<V>>( mParameters ) = std::forward<V>( value );
  }

  template<typename Head, typename... Tail>
  void SetParameters( Head&& head, Tail&&... tail )
  {
    SetParameters( std::forward<Head>( head ) );
    SetParameters( std::forward<Tail...>( tail... ) );
  }

  template<size_t... Is>
  T Build( std::index_sequence<Is...> )
  {
    return T(std::move(std::get<Is>( mParameters ) )...);
  }

private: // data
  std::tuple<Params...> mParameters;
};

} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_UTILITY_BUILDER_H