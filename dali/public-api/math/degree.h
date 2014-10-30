#ifndef __DALI_DEGREE_H__
#define __DALI_DEGREE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

struct Radian;

/**
 * @brief An angle in degrees.
 *
 * This reduces ambiguity when using methods which accept angles in degrees or radians.
 */
struct DALI_IMPORT_API Degree
{
  /**
   * @brief Create an angle in degrees.
   *
   * @param[in] value The initial value in degrees.
   */
  explicit Degree( float value );

  /**
   * @brief Create an angle in degrees from an angle in radians.
   *
   * @param[in] value The initial value in radians.
   */
  Degree( const Radian& value );

  /**
   * @brief Compare equality between two degrees.
   *
   * @param[in] rhs Degree to compare to
   * @return true if the value is identical
   */
  bool operator==( const Degree& rhs ) const;

  /**
   * @brief Compare inequality between two degrees.
   *
   * @param[in] rhs Degree to compare to
   * @return true if the value is not identical
   */
  bool operator!=( const Degree& rhs ) const;

  /**
   * @brief Compare two degrees.
   *
   * @param[in] rhs Degree to compare to
   * @return true if this is less than the value
   */
  bool operator<( const Degree& rhs ) const;

  /**
   * @brief Assign an angle from a float value.
   *
   * @param[in] value Float value in degrees
   * @return a reference to this
   */
  Degree& operator=( const float value );

  /**
   * @brief Assign an angle in radians to a Degree.
   *
   * @param[in] rhs Radian to get the value from
   * @return a reference to this
   */
  Degree& operator=( const Radian& rhs );

  /**
   * @brief Cast operator to const float reference
   */
  operator const float&() const;

  /**
   * @brief Cast operator to float reference.
   */
  operator float&();

private:
  // member data
  float mValue; ///< The value in degrees

  /**
   * @brief Disable the default constructor.
   */
  Degree();
};

} // namespace Dali

#endif // __DALI_DEGREE_H__
