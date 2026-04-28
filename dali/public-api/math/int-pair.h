#ifndef DALI_INT_PAIR_H
#define DALI_INT_PAIR_H

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
#include <cstdint>
#include <type_traits>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/type-traits.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

/**
 * @brief Simple class for passing around pairs of small integers.
 *
 * Use this for integer dimensions and points with limited range such as image
 * sizes and pixel coordinates where a pair of floating point numbers is
 * inefficient and illogical (i.e. the data is inherently integer).
 *
 * For example in IntPair<uint16_t>
 * one of these can be passed in a single 32 bit integer register on
 * common architectures.
 *
 * @SINCE_2_1.45
 * @tparam tIntType The signed/unsigned integer type for get/set access.
 * @tparam tUintType The unsigned integer type that will be stored.
 * @tparam tDataType The 2-tuple stored integer type.
 */
template<typename tIntType, typename tUintType, typename tDataType>
class IntPair
{
public:
  using IntType  = tIntType;
  using UintType = tUintType;
  using DataType = tDataType;

public:
  /**
   * @brief Default constructor for the (0, 0) tuple.
   * @SINCE_2_1.45
   */
  constexpr IntPair()
  : mData(0)
  {
    static_assert(sizeof(tIntType) == sizeof(tUintType));
    static_assert(sizeof(tIntType) * 2 == sizeof(tDataType));
  }

  /**
   * @brief Constructor taking separate x and y (width and height) parameters.
   * @SINCE_2_1.45
   * @param[in] width The width or X dimension of the tuple.
   * @param[in] height The height or Y dimension of the tuple.
   */
  constexpr IntPair(tIntType width, tIntType height)
  {
    /**
     * Do equivalent of the code below with one aligned memory access:
     * mComponents[0] = width;
     * mComponents[1] = height;
     * Unit tests make sure this is equivalent.
     **/
    constexpr uint32_t Bits = sizeof(tIntType) * 8u;
    mData                   = (static_cast<tDataType>(static_cast<tUintType>(height)) << Bits) | static_cast<tDataType>(static_cast<tUintType>(width));
  }

  /**
   * @brief Sets the width.
   * @SINCE_2_1.45
   * @param[in] width The x dimension to be stored in this 2-tuple
   */
  void SetWidth(tIntType width)
  {
    mComponents[0] = width;
  }

  /**
   * @brief Get the width.
   * @SINCE_2_1.45
   * @return the x dimension stored in this 2-tuple
   */
  tIntType GetWidth() const
  {
    return mComponents[0];
  }

  /**
   * @brief Sets the height.
   * @SINCE_2_1.45
   * @param[in] height The y dimension to be stored in this 2-tuple
   */
  void SetHeight(tIntType height)
  {
    mComponents[1] = height;
  }

  /**
   * @brief Returns the y dimension stored in this 2-tuple.
   * @SINCE_2_1.45
   * @return Height
   */
  tIntType GetHeight() const
  {
    return mComponents[1];
  }

  /**
   * @brief Sets the x dimension (same as width).
   * @SINCE_2_1.45
   * @param[in] x The x dimension to be stored in this 2-tuple
   */
  void SetX(tIntType x)
  {
    mComponents[0] = x;
  }

  /**
   * @brief Returns the x dimension stored in this 2-tuple.
   * @SINCE_2_1.45
   * @return X
   */
  tIntType GetX() const
  {
    return mComponents[0];
  }

  /**
   * @brief Sets the y dimension (same as height).
   * @SINCE_2_1.45
   * @param[in] y The y dimension to be stored in this 2-tuple
   */
  void SetY(tIntType y)
  {
    mComponents[1] = y;
  }

  /**
   * @brief Returns the y dimension stored in this 2-tuple.
   * @SINCE_2_1.45
   * @return Y
   */
  tIntType GetY() const
  {
    return mComponents[1];
  }

  /**
   * @brief Equality operator.
   * @SINCE_2_1.45
   * @param[in] rhs A reference for comparison
   * @return True if same
   */
  bool operator==(const IntPair& rhs) const
  {
    return mData == rhs.mData;
  }

  /**
   * @brief Inequality operator.
   * @SINCE_2_1.45
   * @param[in] rhs A reference for comparison
   * @return True if different
   */
  bool operator!=(const IntPair& rhs) const
  {
    return mData != rhs.mData;
  }

  /**
   * @brief Less than comparison operator for storing in collections (not geometrically
   * meaningful).
   * @SINCE_2_1.45
   * @param[in] rhs A reference for comparison
   * @return True if less
   */
  bool operator<(const IntPair& rhs) const
  {
    return mData < rhs.mData;
  }

  /**
   * @brief Greater than comparison operator for storing in collections (not
   * geometrically meaningful).
   * @SINCE_2_1.45
   * @param[in] rhs A reference for comparison
   * @return True if greater
   */
  bool operator>(const IntPair& rhs) const
  {
    return mData > rhs.mData;
  }

public:
  IntPair(const IntPair&)                = default; ///< Default copy constructor
  IntPair(IntPair&&) noexcept            = default; ///< Default move constructor
  IntPair& operator=(const IntPair&)     = default; ///< Default copy assignment operator
  IntPair& operator=(IntPair&&) noexcept = default; ///< Default move assignment operator

private:
  union
  {
    // Addressable view of X and Y:
    tIntType mComponents[2];
    // Packed view of X and Y to force alignment and allow a faster copy:
    tDataType mData;
  };
};

/**
 * @brief Simple class for passing around pairs of signed integers.
 *
 * Use this for integer dimensions and points with limited range such as window
 * position and screen coordinates where a pair of floating point numbers is
 * inefficient and illogical (i.e. the data is inherently integer).
 * One of these can be passed in a single 64 bit integer.
 * @SINCE_2_1.45
 */
class Int32Pair : public IntPair<int32_t, uint32_t, uint64_t>
{
public:
  /**
   * @brief Default constructor for the (0, 0) tuple.
   * @SINCE_2_1.45
   */
  constexpr Int32Pair() = default;

  /**
   * @brief Constructor taking separate x and y (width and height) parameters.
   * @SINCE_2_1.45
   * @param[in] x The width or X dimension of the tuple.
   * @param[in] y The height or Y dimension of the tuple.
   */
  constexpr Int32Pair(uint32_t x, uint32_t y)
  : IntPair(x, y)
  {
  }

public:
  Int32Pair(const Int32Pair&)                = default; ///< Default copy constructor
  Int32Pair(Int32Pair&&) noexcept            = default; ///< Default move constructor
  Int32Pair& operator=(const Int32Pair&)     = default; ///< Default copy assignment operator
  Int32Pair& operator=(Int32Pair&&) noexcept = default; ///< Default move assignment operator
};

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
class Uint16Pair : public IntPair<uint16_t, uint16_t, uint32_t>
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

// Allow Int32Pair to be treated as a POD type
template<>
struct TypeTraits<Int32Pair> : public BasicTypes<Int32Pair>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
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

#endif // DALI_INT_PAIR_H
