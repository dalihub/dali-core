#ifndef __DALI_UINT_16_PAIR_H__
#define __DALI_UINT_16_PAIR_H__

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
#include <stdint.h>

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
 * @brief Simple class for passing around pairs of small unsigned integers.
 *
 * Use this for integer dimensions and points with limited range such as image
 * sizes and pixel coordinates where a pair of floating point numbers is
 * inefficient and illogical (i.e. the data is inherently integer).
 * One of these can be passed in a single 32 bit integer register on
 * common architectures.
 * @SINCE_1_0.0
 */
class Uint16Pair
{
public:
  /**
   * @brief Default constructor for the (0, 0) tuple.
   * @SINCE_1_0.0
   */
  Uint16Pair() : mData(0) {}

  /**
   * @brief Constructor taking separate x and y (width and height) parameters.
   * @SINCE_1_0.0
   * @param[in] width The width or X dimension of the tuple. Make sure it is less than 65536,
   * @param[in] height The height or Y dimension of the tuple. Make sure it is less than 65536,
   */
  Uint16Pair( uint32_t width, uint32_t height )
  {
    DALI_ASSERT_DEBUG( width < ( 1u << 16 ) && "Width parameter not representable." );
    DALI_ASSERT_DEBUG( height < ( 1u << 16 ) && "Height parameter not representable." );

    /* Do equivalent of the code below with one aligned memory access:
     * mComponents[0] = width;
     * mComponents[1] = height;
     * Unit tests make sure this is equivalent.
     **/
    mData = (height << 16u) + width;
  }

  /**
   * @brief Copy constructor.
   * @SINCE_1_0.0
   * @param[in] rhs A reference to assign
   */
  Uint16Pair( const Uint16Pair& rhs )
  {
    mData = rhs.mData;
  }

  /**
   * @brief Sets the width.
   * @SINCE_1_1.13
   * @param[in] width The x dimension to be stored in this 2-tuple.
   */
  void SetWidth( uint16_t width )
  {
    mComponents[0] = width;
  }

  /**
   * @brief @returns the x dimension stored in this 2-tuple.
   * @SINCE_1_0.0
   */
  uint16_t GetWidth() const
  {
    return mComponents[0];
  }

  /**
   * @brief Sets the height.
   * @SINCE_1_1.13
   * @param[in] height The y dimension to be stored in this 2-tuple.
   */
  void SetHeight( uint16_t height )
  {
    mComponents[1] = height;
  }

  /**
   * @brief Returns the y dimension stored in this 2-tuple.
   * @SINCE_1_0.0
   * @return Height
   */
  uint16_t GetHeight() const
  {
    return mComponents[1];
  }

  /**
   * @brief Sets the x dimension (same as width).
   * @SINCE_1_1.14
   * @param[in] x The x dimension to be stored in this 2-tuple.
   */
  void SetX( uint16_t x )
  {
    mComponents[0] = x;
  }

  /**
   * @brief Returns the x dimension stored in this 2-tuple.
   * @SINCE_1_0.0
   * @return X
   */
  uint16_t GetX()  const
  {
    return mComponents[0];
  }

  /**
   * @brief Sets the y dimension (same as height).
   * @SINCE_1_1.14
   * @param[in] y The y dimension to be stored in this 2-tuple.
   */
  void SetY( uint16_t y )
  {
    mComponents[1] = y;
  }

  /**
   * @brief Returns the y dimension stored in this 2-tuple.
   * @SINCE_1_0.0
   * @return Y
   */
  uint16_t GetY() const
  {
    return mComponents[1];
  }

  /**
   * @brief Assignment operator.
   * @SINCE_1_0.0
   */
  Uint16Pair& operator=( const Uint16Pair& rhs )
  {
    if( rhs != *this )
    {
      mData = rhs.mData;
    }
    return *this;
  }

  /**
   * @brief Equality operator.
   * @SINCE_1_0.0
   * @param[in] rhs A reference for comparison
   * @return True if same
   */
  bool operator==( const Uint16Pair& rhs ) const
  {
    return mData == rhs.mData;
  }

  /**
   * @brief Inequality operator.
   * @SINCE_1_0.0
   * @param[in] rhs A reference for comparison
   * @return True if different
   */
  bool operator!=( const Uint16Pair& rhs ) const
  {
    return mData != rhs.mData;
  }

  /**
   * @brief Less than comparison operator for storing in collections (not geometrically
   * meaningful).
   * @SINCE_1_0.0
   * @param[in] rhs A reference for comparison
   * @return True if less
   */
  bool operator<( const Uint16Pair& rhs ) const
  {
    return mData < rhs.mData;
  }

  /**
   * @brief Greater than comparison operator for storing in collections (not
   * geometrically meaningful).
   * @SINCE_1_0.0
   * @param[in] rhs A reference for comparison
   * @return True if greater
   */
  bool operator>( const Uint16Pair& rhs ) const
  {
    return mData > rhs.mData;
  }

  /**
   * @brief Create an instance by rounding a floating point vector to closest
   * integers.
   *
   * Uses a template for loose coupling, to save a header include, and allow any
   * vector type with .x and .y members to be converted.
   * @SINCE_1_0.0
   * @param[in] from Floating point vector2
   * @return Closest integer value.
   */
  template<typename FLOAT_VECTOR_N_TYPE>
  static Uint16Pair FromFloatVec2( const FLOAT_VECTOR_N_TYPE& from )
  {
    DALI_ASSERT_DEBUG( from.x + 0.5f < 65536.0f );
    DALI_ASSERT_DEBUG( from.y + 0.5f < 65536.0f );
    return Uint16Pair( from.x + 0.5f, from.y + 0.5f );
  }

  /**
   * @brief Create an instance by rounding a floating point array to closest
   * integers.
   *
   * Uses a template to allow any vector type with operator [] to be converted
   * in addition to plain arrays.
   * @SINCE_1_0.0
   * @param[in] from Floating point array
   * @return Closest integer value.
   */
  template<typename FLOAT_ARRAY>
  static Uint16Pair FromFloatArray( const FLOAT_ARRAY& from )
  {
    DALI_ASSERT_DEBUG( from[0] + 0.5f < 65536.0f );
    DALI_ASSERT_DEBUG( from[1] + 0.5f < 65536.0f );
    return Uint16Pair( from[0] + 0.5f, from[1] + 0.5f );
  }

private:
  union
  {
    // Addressable view of X and Y:
    uint16_t mComponents[2];
    // Packed view of X and Y to force alignment and allow a faster copy:
    uint32_t mData;
  };
};

// Allow Uint16Pair to be treated as a POD type
template <> struct TypeTraits< Uint16Pair > : public BasicTypes< Uint16Pair > { enum { IS_TRIVIAL_TYPE = true }; };

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_UINT_16_PAIR_H__
