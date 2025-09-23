#ifndef DALI_UINT_16_PAIR_H
#define DALI_UINT_16_PAIR_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/math/int-pair.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

/**
 * @brief Simple class for passing around pairs of small unsigned integers.
 *
 * Use this for integer dimensions and points with limited range such as image
 * sizes and pixel coordinates where a pair of floating point numbers is
 * inefficient and illogical (i.e. the data is inherently integer).
 * One of these can be passed in a single 32 bit integer register on
 * common architectures.
 * @SINCE_1_0.0
 */
class Uint16Pair : public IntPair<16, false>
{
public:
  /**
   * @brief Default constructor for the (0, 0) tuple.
   * @SINCE_1_0.0
   */
  constexpr Uint16Pair() = default;

  /**
   * @brief Constructor taking separate x and y (width and height) parameters.
   * @SINCE_1_0.0
   * @param[in] width The width or X dimension of the tuple. Make sure it is less than 65536.
   * @param[in] height The height or Y dimension of the tuple. Make sure it is less than 65536.
   */
  constexpr Uint16Pair(uint32_t width, uint32_t height)
  : IntPair(static_cast<uint16_t>(width), static_cast<uint16_t>(height))
  {
    DALI_ASSERT_DEBUG(width < (1u << 16) && "Width parameter not representable.");
    DALI_ASSERT_DEBUG(height < (1u << 16) && "Height parameter not representable.");
  }

  /**
   * @brief Creates an instance by rounding a floating point vector to closest
   * integers.
   *
   * Uses a template for loose coupling, to save a header include, and allow any
   * vector type with .x and .y members to be converted.
   * @SINCE_1_0.0
   * @param[in] from Floating point vector2
   * @return Closest integer value
   */
  template<typename FLOAT_VECTOR_N_TYPE>
  static Uint16Pair FromFloatVec2(const FLOAT_VECTOR_N_TYPE& from)
  {
    DALI_ASSERT_DEBUG(from.x + 0.5f < 65536.0f);
    DALI_ASSERT_DEBUG(from.y + 0.5f < 65536.0f);
    return Uint16Pair(from.x + 0.5f, from.y + 0.5f);
  }

  /**
   * @brief Creates an instance by rounding a floating point array to closest
   * integers.
   *
   * Uses a template to allow any vector type with operator [] to be converted
   * in addition to plain arrays.
   * @SINCE_1_0.0
   * @param[in] from Floating point array
   * @return Closest integer value
   */
  template<typename FLOAT_ARRAY>
  static Uint16Pair FromFloatArray(const FLOAT_ARRAY& from)
  {
    DALI_ASSERT_DEBUG(from[0] + 0.5f < 65536.0f);
    DALI_ASSERT_DEBUG(from[1] + 0.5f < 65536.0f);
    return Uint16Pair(from[0] + 0.5f, from[1] + 0.5f);
  }

public:
  Uint16Pair(const Uint16Pair&)                = default; ///< Default copy constructor
  Uint16Pair(Uint16Pair&&) noexcept            = default; ///< Default move constructor
  Uint16Pair& operator=(const Uint16Pair&)     = default; ///< Default copy assignment operator
  Uint16Pair& operator=(Uint16Pair&&) noexcept = default; ///< Default move assignment operator
};

// Allow Uint16Pair to be treated as a POD type
template<>
struct TypeTraits<Uint16Pair> : public BasicTypes<Uint16Pair>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_UINT_16_PAIR_H
