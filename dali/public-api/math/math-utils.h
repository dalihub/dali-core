#ifndef DALI_MATH_UTILS_H
#define DALI_MATH_UTILS_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t
#include <limits>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Internal
{
namespace MathUtils
{
/**
 * The C++17 standard does not require the cmath functions used below to be
 * constexpr.  Keep small, platform-independent implementations here so that
 * the public math helpers remain usable in constant expressions on every
 * supported compiler.
 */
constexpr float Abs(float x)
{
  // Using <= also turns -0.0f into +0.0f, matching fabsf.
  return x <= 0.0f ? -x : x;
}

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
constexpr bool IsNaN(float x)
{
  return x != x;
}

constexpr float Floor(float x)
{
  // A conversion from NaN, infinity, or an out-of-range float to an integer is
  // undefined.  Such finite values are already integral at float precision;
  // infinities and NaNs must be returned unchanged, as floorf does.
  if(IsNaN(x) || x <= -2147483648.0f || x >= 2147483648.0f)
  {
    return x;
  }

  // Preserve the sign of zero.
  if(x == 0.0f)
  {
    return x;
  }

  const int32_t i = static_cast<int32_t>(x);
  return (x < 0.0f && static_cast<float>(i) != x) ? static_cast<float>(i - 1) : static_cast<float>(i);
}
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

constexpr float PowerOfTen(int64_t exponent)
{
  // These are the limits at which a base-10 power overflows or underflows a
  // float.  Check them before taking the magnitude, which also makes every
  // signed input (including INT64_MIN) safe.
  if(exponent > 38)
  {
    return std::numeric_limits<float>::infinity();
  }
  if(exponent < -45)
  {
    return 0.0f;
  }

  const bool negativeExponent = exponent < 0;
  uint64_t   magnitude        = static_cast<uint64_t>(negativeExponent ? -exponent : exponent);

  // Accumulate in double and round once to float.  For negative exponents,
  // taking the reciprocal at the end avoids the cumulative error from
  // repeatedly multiplying the inexact float value 0.1f.
  double result = 1.0;
  double base   = 10.0;
  while(magnitude != 0u)
  {
    if((magnitude & 1u) != 0u)
    {
      result *= base;
    }
    magnitude >>= 1u;
    if(magnitude != 0u)
    {
      base *= base;
    }
  }
  return static_cast<float>(negativeExponent ? 1.0 / result : result);
}

} // namespace MathUtils
} // namespace Internal

/**
 * @addtogroup dali_core_math
 * @{
 */

/**
 * @brief Returns the next power of two.
 *
 * In case of numbers which are already a power of two this function returns the original number.
 * If i is zero returns 1.
 * @SINCE_1_0.0
 * @param[in] i Input number
 * @return The next power of two or i itself in case it's a power of two
 */
constexpr uint32_t NextPowerOfTwo(uint32_t i)
{
  DALI_ASSERT_ALWAYS(i <= 1u << (sizeof(uint32_t) * 8 - 1) && "Return type cannot represent the next power of two greater than the argument.");
  if(i == 0u)
  {
    return 1u;
  }

  i--;
  i |= i >> 1;
  i |= i >> 2;
  i |= i >> 4;
  i |= i >> 8;
  i |= i >> 16;
  i++;
  return i;
}

/**
 * @brief Whether a number is power of two.
 *
 * @SINCE_1_0.0
 * @param[in] i Input number
 * @return    True if i is power of two.
 */
constexpr bool IsPowerOfTwo(uint32_t i)
{
  return (i != 0u) && ((i & (i - 1u)) == 0u);
}

/**
 * @brief Clamp a value.
 *
 * @SINCE_1_0.0
 * @param[in] value The value to clamp.
 * @param[in] min The minimum allowed value.
 * @param[in] max The maximum allowed value.
 * @return T the clamped value
 */
template<typename T>
constexpr const T& Clamp(const T& value, const T& min, const T& max)
{
  const T& constrainedUpper         = value < max ? value : max;
  const T& constrainedUpperAndLower = constrainedUpper > min ? constrainedUpper : min;
  return constrainedUpperAndLower;
}

/**
 * @brief Clamp a value directly.
 *
 * @SINCE_1_0.0
 * @param[in,out] value The value that will be clamped.
 * @param[in] min The minimum allowed value.
 * @param[in] max The maximum allowed value.
 */
template<typename T>
constexpr void ClampInPlace(T& value, const T& min, const T& max)
{
  const T& constrainedUpper         = value < max ? value : max;
  const T& constrainedUpperAndLower = constrainedUpper > min ? constrainedUpper : min;
  value                             = constrainedUpperAndLower;
}

/**
 * @brief Linear interpolation between two values.
 *
 * @SINCE_1_0.0
 * @param[in] offset The offset through the range @p low to @p high.
 *                   This value is clamped between 0 and 1.
 * @param[in] low    Lowest value in range
 * @param[in] high   Highest value in range
 * @return A value between low and high.
 */
template<typename T>
constexpr const T Lerp(const float offset, const T& low, const T& high)
{
  return low + ((high - low) * Clamp(offset, 0.0f, 1.0f));
}

/**
 * @brief Get an epsilon that is valid for the given range.
 *
 * @SINCE_1_0.0
 * @param[in] a the first value in the range
 * @param[in] b the second value in the range.
 * @return a suitable epsilon
 */
constexpr float GetRangedEpsilon(float a, float b)
{
  const float absA = Internal::MathUtils::Abs(a);
  const float absB = Internal::MathUtils::Abs(b);

  if(Internal::MathUtils::IsNaN(absA) || Internal::MathUtils::IsNaN(absB))
  {
    return Math::MACHINE_EPSILON_10000;
  }

  const float absF = absA > absB ? absA : absB;

  float epsilon = Math::MACHINE_EPSILON_10000;
  if(absF < 0.1f)
  {
    return Math::MACHINE_EPSILON_0;
  }
  else if(absF < 2.0f)
  {
    return Math::MACHINE_EPSILON_1;
  }
  else if(absF < 20.0f)
  {
    return Math::MACHINE_EPSILON_10;
  }
  else if(absF < 200.0f)
  {
    return Math::MACHINE_EPSILON_100;
  }
  else if(absF < 2000.0f)
  {
    return Math::MACHINE_EPSILON_1000;
  }
  return epsilon;
}

/**
 * @brief Helper function to compare equality of a floating point value with zero.
 *
 * @SINCE_1_0.0
 * @param[in] value the value to compare
 * @return true if the value is equal to zero
 */
#if __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
constexpr bool EqualsZero(float value)
{
  return value == 0.0f;
}
#if __GNUC__
#pragma GCC diagnostic pop
#endif

/**
 * @brief Helper function to compare equality of two floating point values.
 *
 * @SINCE_1_0.0
 * @param[in] a the first value to compare
 * @param[in] b the second value to compare
 * @return true if the values are equal within a minimal epsilon for their values
 */
constexpr bool Equals(float a, float b)
{
  return (Internal::MathUtils::Abs(a - b) <= GetRangedEpsilon(a, b));
}

/**
 * @brief Helper function to compare equality of two floating point values.
 *
 * @SINCE_1_0.0
 * @param[in] a the first value to compare
 * @param[in] b the second value to compare
 * @param[in] epsilon the minimum epsilon value that will be used to consider the values different
 * @return true if the difference between the values is less than the epsilon
 */
constexpr bool Equals(float a, float b, float epsilon)
{
  return (Internal::MathUtils::Abs(a - b) <= epsilon);
}

/**
 * @brief Get an float that is rounded at specified place of decimals.
 *
 * @SINCE_1_0.0
 * @param[in] value float value
 * @param[in] pos decimal place
 * @return a rounded float
 */
constexpr float Round(float value, int32_t pos)
{
  const int64_t exponent = static_cast<int64_t>(pos);
  float         temp     = value * Internal::MathUtils::PowerOfTen(exponent);
  temp                    = Internal::MathUtils::Floor(temp + 0.5f);
  temp *= Internal::MathUtils::PowerOfTen(-exponent);
  return temp;
}

/**
 * @brief Wrap x in domain (start) to (end).
 *
 * This works like a floating point version
 * of the % modulo operation. But with an offset (start).
 *
 * For instance a domain is specified as:
 * start: 2
 * end: 8
 *
 * @code
 *   2                         8
 * (\ / start)               (\ / end)
 *   |----x                    |
 * @endcode
 *
 * The value x will be confined to this domain.
 * If x is below 2 e.g. 0, then it is wrapped to 6.
 * If x is above or equal to 8 e.g. 8.1 then it is
 * wrapped to 2.1.
 *
 * Domain wrapping is useful for various problems from
 * calculating positions in a space that repeats, to
 * computing angles that range from 0 to 360.
 *
 * @SINCE_1_0.0
 * @param[in] x the point to be wrapped within the domain
 * @param[in] start The start of the domain
 * @param[in] end The end of the domain
 *
 * @return the wrapped value over the domain (start) (end)
 * @note If start = end (i.e. size of domain 0), then wrapping will not occur
 * and result will always be equal to start.
 *
 */
constexpr float WrapInDomain(float x, float start, float end)
{
  float domain = end - start;
  x -= start;

  if(Internal::MathUtils::Abs(domain) > Math::MACHINE_EPSILON_1)
  {
    return start + (x - Internal::MathUtils::Floor(x / domain) * domain);
  }

  return start;
}

/**
 * @brief Find the shortest distance (magnitude) and direction (sign)
 * from (a) to (b) in domain (start) to (end).
 *
 * @code
 *  (\ / start)               (\ / end)
 *    |-a                 b<----|
 * @endcode
 *
 * Knowing the shortest distance is useful with wrapped domains
 * to solve problems such as determining the closest object to
 * a given point, or determining whether turning left or turning
 * right is the shortest route to get from angle 10 degrees
 * to angle 350 degrees (clearly in a 0-360 degree domain, turning
 * left 20 degrees is quicker than turning right 340 degrees).
 *
 * The value returned holds the distance and the direction from
 * value a to value b. For instance in the above example it would
 * return -20. i.e. subtract 20 from current value (10) to reach
 * target wrapped value (350).
 *
 * @SINCE_1_0.0
 * @param a the current value
 * @param b the target value
 * @param start the start of the domain
 * @param end the end of the domain
 * @return the shortest direction (the sign) and distance (the magnitude)
 * @note Assumes both (a) and (b) are already within the domain
 * (start) to (end).
 *
 */
constexpr float ShortestDistanceInDomain(float a, float b, float start, float end)
{
  //  (a-start + end-b)
  float size = end - start;
  float vect = b - a;

  if(vect > 0)
  {
    // +ve vector, let's try perspective 1 domain to the right,
    // and see if closer.
    float aRight = a + size;
    if(aRight - b < vect)
    {
      return b - aRight;
    }
  }
  else
  {
    // -ve vector, let's try perspective 1 domain to the left,
    // and see if closer.
    float aLeft = a - size;
    if(aLeft - b > vect)
    {
      return b - aLeft;
    }
  }

  return vect;
}

/**
 * @brief Extracts the sign of a number
 *
 * @SINCE_1_0.0
 * @param[in] value The value we want to extract the sign
 * @return -1 for negative values, +1 for positive values and 0 if value is 0
 */
template<typename T>
constexpr int32_t Sign(T value)
{
  return (T(0) < value) - (value < T(0));
}

/**
 * @}
 */
} // namespace Dali

#endif // DALI_MATH_UTILS_H
