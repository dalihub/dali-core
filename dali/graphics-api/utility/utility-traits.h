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
struct BasicType<T>
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

template<typename T, template<typename> class Ptr>
struct PointerType<> final
{
  using StorageT = Ptr<T>;
  using AccessT  = T&;
  using ValueT   = T;
};


// Pointer types
template<typename T> struct PointerType final : public FalseValue {};
template<typename T> struct PointerType<T*> final : public TrueValue {};
template<typename T> struct PointerType<const T*> final : public TrueValue {};
template<typename T> struct PointerType<std::unique_ptr<T>> final : public TrueValue {};
template<typename T> struct PointerType<std::shared_ptr<T>> final : public TrueValue {};

// Basic types
template<typename T> struct BasicType final : public FalseValue {};
template<> struct BasicType<signed char> final : public TrueValue {};
template<> struct BasicType<unsigned char> final : public TrueValue {};
template<> struct BasicType<signed short> final : public TrueValue {};
template<> struct BasicType<unsigned short> final : public TrueValue {};
template<> struct BasicType<signed long> final : public TrueValue {};
template<> struct BasicType<unsigned long> final : public TrueValue {};
template<> struct BasicType<signed long long> final : public TrueValue {};
template<> struct BasicType<unsigned long long> final : public TrueValue {};
template<> struct BasicType<float> final : public TrueValue {};
template<> struct BasicType<double> final : public TrueValue {};


// Type Traits
template<typename T, template<typename> class Ptr, std::enable_if_t<PointerType<Ptr<T>>::value>>
struct Traits<Ptr<T>> final
{
  using StorageT = Ptr<T>;
  using AccessT  = T&;
  using ValueT   = T;
};

} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_UTILITY_TRAITS_H