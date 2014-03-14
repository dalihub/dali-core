#ifndef __DALI_RADIAN_H__
#define __DALI_RADIAN_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_MATH_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

struct Degree;

/**
 * @brief An angle in radians.
 *
 * This reduces ambiguity when using methods which accept angles in degrees or radians.
 */
struct DALI_IMPORT_API Radian
{
  /**
   * @brief Create an angle in radians.
   *
   * @param[in] value The initial value in radians.
   */
  explicit Radian( float value );

  /**
   * @brief Create an angle in radians from an angle in degrees.
   *
   * @param[in] value The initial value in degrees.
   */
  Radian( const Degree& value );

  /**
   * @brief Compare equality between two radians.
   *
   * @param[in] rhs Radian to compare to
   * @return true if the value is identical
   */
  bool operator==( const Radian& rhs ) const;

  /**
   * @brief Compare inequality between two radians.
   *
   * @param[in] rhs Radian to compare to
   * @return true if the value is not identical
   */
  bool operator!=( const Radian& rhs ) const;

  /**
   * @brief Compare two radians.
   *
   * @param[in] rhs Radian to compare to
   * @return true if this is less than the value
   */
  bool operator<( const Radian& rhs ) const;

  /**
   * @brief Assign an angle from a float value.
   *
   * @param[in] value Float value in radians
   * @return a reference to this object
   */
  Radian& operator=( const float value );

  /**
   * @brief Assign an angle in degrees to a Radian.
   *
   * @param[in] rhs Degree to get the value from
   * @return a reference to this object
   */
  Radian& operator=( const Degree& rhs );

  /**
   * @brief Cast operator to const float reference.
   */
  operator const float&() const;

  /**
   * @brief Cast operator to float reference.
   */
  operator float&();

private:
  // member data
  float mValue; ///< The value in radians

  // disable default constructor
  Radian();
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_RADIAN_H__
