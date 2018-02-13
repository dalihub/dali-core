#ifndef DALI_GRAPHICS_UTILITY_TRAITS_H
#define DALI_GRAPHICS_UTILITY_TRAITS_H

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
#include <memory>
#include <type_traits>

namespace Dali
{
namespace Graphics
{
namespace Utility
{

template<typename T>
struct BasicType
{
  using StorageT = T;
  using AccessT  = T;
  using ValueT   = T;
};

template<typename T>
struct ComplexType
{
  using StorageT = T;
  using AccessT  = const T&;
  using ValueT   = T;
};

template<typename T, template<typename...> class Ptr, typename...Ts>
struct SmartPointerType
{
  using StorageT = Ptr<T, Ts...>;
  using AccessT  = T&;
  using ValueT   = T;
};

// Default types
template<typename T> struct TraitsType : ComplexType<T> {};

// Pointer types
template<typename T, typename D>
struct TraitsType<std::unique_ptr<T, D>> : SmartPointerType<T, std::unique_ptr, D>
{
};
template<typename T>
struct TraitsType<std::shared_ptr<T>> : SmartPointerType<T, std::shared_ptr>
{
};

// Basic types
template<> struct TraitsType<signed char> : BasicType<signed char> { };
template<> struct TraitsType<unsigned char> : BasicType<unsigned char> { };
template<> struct TraitsType<signed short> : BasicType<signed short> { };
template<> struct TraitsType<unsigned short> : BasicType<unsigned short> { };
template<> struct TraitsType<signed long> : BasicType<signed long> { };
template<> struct TraitsType<unsigned long> : BasicType<unsigned long> { };
template<> struct TraitsType<signed long long> : BasicType<signed long long> { };
template<> struct TraitsType<unsigned long long> : BasicType<unsigned long long> { };
template<> struct TraitsType<float> : BasicType<float> { };
template<> struct TraitsType<double> : BasicType<double> { };

// Invalid tyoes

template<typename T> struct TraitsType<T*> {};
template<typename T> struct TraitsType<const T*> {};
template<typename T> struct TraitsType<T&> {};
template<typename T> struct TraitsType<const T&> {};

} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_UTILITY_TRAITS_H