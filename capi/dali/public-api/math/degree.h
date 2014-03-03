#ifndef __DALI_DEGREE_H__
#define __DALI_DEGREE_H__

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
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

struct Radian;

/**
 * An angle in degrees.
 * This reduces ambiguity when using methods which accept angles in degrees or radians.
 */
struct DALI_IMPORT_API Degree
{
  /**
   * Create an angle in degrees.
   * @param[in] value The initial value in degrees.
   */
  explicit Degree( float value );

  /**
   * Create an angle in degrees from an angle in radians.
   * @param[in] value The initial value in radians.
   */
  Degree( const Radian& value );

  /**
   * Compare equality between two degrees
   * @param[in] rhs Degree to compare to
   */
  bool operator==( const Degree& rhs ) const;

  /**
   * Compare inequality between two degrees
   * @param[in] rhs Degree to compare to
   */
  bool operator!=( const Degree& rhs ) const;

  /**
   * Compare two degrees
   * @param[in] rhs Degree to compare to
   */
  bool operator<( const Degree& rhs ) const;

  /**
   * Assign an angle from a float value
   * @param[in] value Float value in degrees
   */
  Degree& operator=( const float value );

  /**
   * Assign an angle in radians to a Degree
   * @param[in] rhs Radian to get the value from
   */
  Degree& operator=( const Radian& rhs );

  /**
   * Cast operator to const float reference
   */
  operator const float&() const;

  /**
   * Cast operator to float reference
   */
  operator float&();

private:
  // member data
  float mValue; ///< The value in degrees

  // disable default constructor
  Degree();
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_DEGREE_H__
