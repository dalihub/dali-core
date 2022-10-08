#ifndef DALI_INT_PAIR_H
#define DALI_INT_PAIR_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
 * @tparam Bits The number of bits. It should be 8, 16, or 32.
 * @tparam IsSigned Determine whether we use signed integer or not.
 */
template<unsigned Bits, bool IsSigned, typename = std::enable_if_t<(Bits == 8 || Bits == 16 || Bits == 32)>>
class IntPair
{
public:
  // Type definition.

  /**
   * @brief Integer type for get/set access.
   * @SINCE_2_1.45
   */
  using IntType = typename std::conditional<Bits == 8, typename std::conditional<IsSigned, int8_t, uint8_t>::type, typename std::conditional<Bits == 16, typename std::conditional<IsSigned, int16_t, uint16_t>::type, typename std::conditional<IsSigned, int32_t, uint32_t>::type>::type>::type;

  /**
   * @brief Unsigned integer type that will be stored.
   * Signed integer bit shift is implementation-defined behaviour.
   * To more safty works, we need to cast from IntType to UintType before store data.
   * @SINCE_2_1.45
   */
  using UintType = typename std::conditional<Bits == 8, uint8_t, typename std::conditional<Bits == 16, uint16_t, uint32_t>::type>::type;

  /**
   * @brief 2-tuple stored integer type.
   * @SINCE_2_1.45
   */
  using DataType = typename std::conditional<Bits == 8, uint16_t, typename std::conditional<Bits == 16, uint32_t, uint64_t>::type>::type;

public:
  /**
   * @brief Default constructor for the (0, 0) tuple.
   * @SINCE_2_1.45
   */
  constexpr IntPair()
  : mData(0)
  {
  }

  /**
   * @brief Constructor taking separate x and y (width and height) parameters.
   * @SINCE_2_1.45
   * @param[in] width The width or X dimension of the tuple.
   * @param[in] height The height or Y dimension of the tuple.
   */
  constexpr IntPair(IntType width, IntType height)
  {
    /* Do equivalent of the code below with one aligned memory access:
     * mComponents[0] = width;
     * mComponents[1] = height;
     * Unit tests make sure this is equivalent.
     **/
    mData = (static_cast<DataType>(static_cast<UintType>(height)) << Bits) | static_cast<DataType>(static_cast<UintType>(width));
  }

  /**
   * @brief Sets the width.
   * @SINCE_2_1.45
   * @param[in] width The x dimension to be stored in this 2-tuple
   */
  void SetWidth(IntType width)
  {
    mComponents[0] = width;
  }

  /**
   * @brief Get the width.
   * @SINCE_2_1.45
   * @return the x dimension stored in this 2-tuple
   */
  IntType GetWidth() const
  {
    return mComponents[0];
  }

  /**
   * @brief Sets the height.
   * @SINCE_2_1.45
   * @param[in] height The y dimension to be stored in this 2-tuple
   */
  void SetHeight(IntType height)
  {
    mComponents[1] = height;
  }

  /**
   * @brief Returns the y dimension stored in this 2-tuple.
   * @SINCE_2_1.45
   * @return Height
   */
  IntType GetHeight() const
  {
    return mComponents[1];
  }

  /**
   * @brief Sets the x dimension (same as width).
   * @SINCE_2_1.45
   * @param[in] x The x dimension to be stored in this 2-tuple
   */
  void SetX(IntType x)
  {
    mComponents[0] = x;
  }

  /**
   * @brief Returns the x dimension stored in this 2-tuple.
   * @SINCE_2_1.45
   * @return X
   */
  IntType GetX() const
  {
    return mComponents[0];
  }

  /**
   * @brief Sets the y dimension (same as height).
   * @SINCE_2_1.45
   * @param[in] y The y dimension to be stored in this 2-tuple
   */
  void SetY(IntType y)
  {
    mComponents[1] = y;
  }

  /**
   * @brief Returns the y dimension stored in this 2-tuple.
   * @SINCE_2_1.45
   * @return Y
   */
  IntType GetY() const
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
  IntPair(const IntPair&)     = default;            ///< Default copy constructor
  IntPair(IntPair&&) noexcept = default;            ///< Default move constructor
  IntPair& operator=(const IntPair&) = default;     ///< Default copy assignment operator
  IntPair& operator=(IntPair&&) noexcept = default; ///< Default move assignment operator

private:
  union
  {
    // Addressable view of X and Y:
    IntType mComponents[2];
    // Packed view of X and Y to force alignment and allow a faster copy:
    DataType mData;
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
class Int32Pair : public IntPair<32, true>
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
  Int32Pair(const Int32Pair&)     = default;            ///< Default copy constructor
  Int32Pair(Int32Pair&&) noexcept = default;            ///< Default move constructor
  Int32Pair& operator=(const Int32Pair&) = default;     ///< Default copy assignment operator
  Int32Pair& operator=(Int32Pair&&) noexcept = default; ///< Default move assignment operator
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

/**
 * @}
 */
} // namespace Dali

#endif // DALI_INT_PAIR_H
