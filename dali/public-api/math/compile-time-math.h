#ifndef __DALI_COMPILE_TIME_MATH_H__
#define __DALI_COMPILE_TIME_MATH_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <stdlib.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @brief Compile time template to calculate base to the power of N.
 *
 * Note! values need to be compile time constants
 * Usage: <code>Power< 10, 2 >::value; // value=100</code>
 * @param mantissa to raise to exponent
 * @param N exponent to use for mantissa
 */
template< size_t mantissa, size_t exponent >
struct Power
{
  enum { value = mantissa * Power< mantissa, exponent - 1 >::value };
};

/**
 * @brief Compile time template to calculate base to the power of N.
 *
 * Specialisation for power of 1
 * @param mantissa to raise to exponent
 */
template< size_t mantissa >
struct Power< mantissa, 1 >
{
  enum { value = mantissa };
};

/**
 * @brief Compile time template to calculate base to the power of N.
 *
 * Specialisation for power of 0
 * @param mantissa to raise to exponent
 */
template< size_t mantissa >
struct Power< mantissa, 0 >
{
  enum { value = 1 };
};

/**
 * @brief Compile time template to calculate base logarithm of N.
 *
 * Note! values need to be compile time constants
 * Usage: <code>Log< 100, 10 >::value; value equals 2</code>
 * @param number for which to calculate the logarithm
 * @param base logarithm to calculate
 */
template< size_t number, size_t base = 2 >
struct Log
{
  enum { value = 1 + Log< number / base, base >::value };
};

/**
 * @brief Compile time template to calculate base logarithm of N.
 *
 * Specialisation for logarithm of 1
 * @param base logarithm to calculate
 */
template< size_t base >
struct Log< 1, base >
{
  enum { value = 0 };
};

/**
 * @brief Compile time template to calculate base logarithm of N.
 *
 * Specialisation for logarithm of 0
 * @param base logarithm to calculate
 */
template< size_t base >
struct Log< 0, base >
{
  enum { value = 0 };
};


/**
 * @brief Compile time template to calculate the machine epsilon for a given floating point number.
 *
 * Note! value needs to be compile time constant
 * Usage: <code>Epsilon<1000>::value; value equals 0.000119209</code>
 * @param N the number for which to calculate the machine epsilon
 */
template< size_t N >
struct Epsilon
{
  // take log10 of the number to get to the nearest power of 10 number and divide that by 10
  // template recursion will take care of the rest
#ifdef _CPP11
  static constexpr float value = 10.0f * Epsilon< Power< 10, Log< N, 10 >::value >::value / 10 >::value;
#else
  static const float value = 10.0f * Epsilon< Power< 10, Log< N, 10 >::value >::value / 10 >::value;
#endif
};

/**
 * @brief Compile time template to calculate the machine epsilon for a given floating point number.
 *
 * Specialisation for epsilon of 1
 * predefined value calculated on ARM Cortex A9 target
 */
template<>
struct Epsilon< 1 >
{
#ifdef _CPP11
  static constexpr float value = 1.19209e-07f;
#else
  static const float value = 1.19209e-07f;
#endif
};

/**
 * @brief Compile time template to calculate the machine epsilon for a given floating point number.
 *
 * Specialisation for epsilon of 0
 * predefined value calculated on ARM Cortex A9 target
 */
template<>
struct Epsilon< 0 >
{
#ifdef _CPP11
  static constexpr float value = 1.4013e-45f;
#else
  static const float value = 1.4013e-45f;
#endif
};

} // namespace Dali

#endif // __DALI_COMPILE_TIME_MATH_H__
