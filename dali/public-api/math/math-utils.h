#ifndef __DALI_MATH_UTILS_H__
#define __DALI_MATH_UTILS_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <algorithm> // std::min & max

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief Returns the next power of two.
 *
 * In case of numbers which are already a power of two this function returns the original number.
 * If i is zero returns 1
 * @param[in] i input number
 * @return    next power of two or i itself in case it's a power of two
 */
DALI_IMPORT_API unsigned int NextPowerOfTwo( unsigned int i );

/**
 * @brief Whether a number is power of two.
 *
 * @param[in] i input number
 * @return    true if i is power of two
 */
DALI_IMPORT_API bool IsPowerOfTwo( unsigned int i );

/**
 * @brief Clamp a value.
 *
 * @param[in] value The value to clamp.
 * @param[in] min The minimum allowed value.
 * @param[in] max The maximum allowed value.
 * @return T the clamped value
 */
template< typename T >
const T& Clamp( const T& value, const T& min, const T& max )
{
  return std::max( std::min( value, max ), min );
}

/**
 * @brief Clamp a value directly.
 *
 * @param[in,out] value The value that will be clamped.
 * @param[in] min The minimum allowed value.
 * @param[in] max The maximum allowed value.
 */
template< typename T >
void ClampInPlace( T& value, const T& min, const T& max )
{
  value =  std::max( std::min( value, max ), min );
}


/**
 * @brief Linear interpolation between two values.
 *
 * @param[in] offset The offset through the range @p low to @p high.
 *                   This value is clamped between 0 and 1
 * @param[in] low    Lowest value in range
 * @param[in] high   Highest value in range
 * @return A value between low and high.
 */
template< typename T >
inline const T Lerp( const float offset, const T& low, const T& high )
{
  return low + ((high - low) * Clamp(offset, 0.0f, 1.0f));
}

/**
 * @brief Get an epsilon that is valid for the given range.
 *
 * @param[in] a the first value in the range
 * @param[in] b the second value in the range.
 * @return a suitable epsilon
 */
DALI_IMPORT_API float GetRangedEpsilon(float a, float b);

/**
 * @brief Helper function to compare equality of a floating point value with zero.
 *
 * @param[in] value the value to compare
 * @return true if the value is equal to zero
 */
DALI_IMPORT_API bool EqualsZero( float value );

/**
 * @brief Helper function to compare equality of two floating point values.
 *
 * @param[in] a the first value to compare
 * @param[in] b the second value to compare
 * @return true if the values are equal within a minimal epsilon for their values
 */
DALI_IMPORT_API bool Equals( float a, float b );

/**
 * @brief Helper function to compare equality of two floating point values.
 *
 * @param[in] a the first value to compare
 * @param[in] b the second value to compare
 * @param[in] epsilon the minimum epsilon value that will be used to consider the values different
 * @return true if the difference between the values is less than the epsilon
 */
DALI_IMPORT_API bool Equals( float a, float b, float epsilon );

/**
 * @brief Get an float that is rounded at specified place of decimals.
 *
 * @param[in] value float value
 * @param[in] pos decimal place
 * @return a rounded float
 */
DALI_IMPORT_API float Round( float value, int pos );

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
 *   2                         8
 * (\ / start)               (\ / end)
 *   |----x                    |
 *
 * The value x will be confined to this domain.
 * If x is below 2 e.g. 0, then it is wraped to 6.
 * If x is above or equal to 8 e.g. 8.1 then it is
 * wrapped to 2.1
 *
 * Domain wrapping is useful for various problems from
 * calculating positions in a space that repeats, to
 * computing angles that range from 0 to 360.
 *
 * @param[in] x the point to be wrapped within the domain
 * @param[in] start The start of the domain
 * @param[in] end The end of the domain
 *
 * @note if start = end (i.e. size of domain 0), then wrapping will not occur
 * and result will always be equal to start.
 *
 * @return the wrapped value over the domain (start) (end)
 */
DALI_IMPORT_API float WrapInDomain(float x, float start, float end);

/**
 * @brief Find the shortest distance (magnitude) and direction (sign)
 * from (a) to (b) in domain (start) to (end).
 *
 * (\ / start)               (\ / end)
 *   |-a                 b<----|
 *
 * Knowing the shortest distance is useful with wrapped domains
 * to solve problems such as determing the closest object to
 * a given point, or determing whether turning left or turning
 * right is the shortest route to get from angle 10 degrees
 * to angle 350 degrees (clearly in a 0-360 degree domain, turning
 * left 20 degrees is quicker than turning right 340 degrees).
 *
 * The value returned holds the distance and the direction from
 * value a to value b. For instance in the above example it would
 * return -20. i.e. subtract 20 from current value (10) to reach
 * target wrapped value (350).
 *
 * @note assumes both (a) and (b) are already within the domain
 * (start) to (end)
 *
 * @param a the current value
 * @param b the target value
 * @param start the start of the domain
 * @param end the end of the domain
 * @return the shortest direction (the sign) and distance (the magnitude)
 */
DALI_IMPORT_API float ShortestDistanceInDomain(float a, float b, float start, float end);

} // namespace Dali

#endif // __DALI_MATH_UTILS_H__
