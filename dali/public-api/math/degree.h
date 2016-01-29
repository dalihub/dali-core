#ifndef __DALI_DEGREE_H__
#define __DALI_DEGREE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

struct Radian;

/**
 * @brief An angle in degrees.
 *
 * This reduces ambiguity when using methods which accept angles in degrees or radians.
 * @SINCE_1_0.0
 */
struct Degree
{
  /**
   * @brief default constructor, initialises to 0.
   * @SINCE_1_0.0
   */
  Degree()
  : degree( 0.f )
  { }

  /**
   * @brief Create an angle in degrees.
   *
   * @SINCE_1_0.0
   * @param[in] value The initial value in degrees.
   */
  explicit Degree( float value )
  : degree( value )
  { }

  /**
   * @brief Create an angle in degrees from a Radian.
   *
   * @SINCE_1_0.0
   * @param[in] value The initial value in Radians.
   */
  DALI_EXPORT_API Degree( Radian value );

public:

  // member data
  float degree; ///< The value in degrees

};

// compiler generated destructor, copy constructor and assignment operators are ok as this class is POD

// useful constant angles
DALI_EXPORT_API extern const Radian ANGLE_360; ///< 360 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_315; ///< 315 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_270; ///< 270 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_225; ///< 225 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_180; ///< 180 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_135; ///< 135 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_120; ///< 120 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_90;  ///< 90 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_60;  ///< 60 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_45;  ///< 45 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_30;  ///< 30 degree turn in radians
DALI_EXPORT_API extern const Radian ANGLE_0;   ///< 0 degree turn in radians

/**
 * @brief Compare equality between two degrees.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Degree to compare
 * @param[in] rhs Degree to compare to
 * @return true if the values are identical
 */
inline bool operator==( const Degree& lhs, const Degree& rhs )
{
  return fabsf( lhs.degree - rhs.degree ) < Math::MACHINE_EPSILON_1000; // expect degree angles to be between 0 and 1000
}

/**
 * @brief Compare inequality between two degrees.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Degree to compare
 * @param[in] rhs Degree to compare to
 * @return true if the values are not identical
 */
inline bool operator!=( const Degree& lhs, const Degree& rhs )
{
  return !( operator==( lhs, rhs ) );
}

/**
 * @brief Clamp a radian value
 * @SINCE_1_0.0
 * @param angle to clamp
 * @param min value
 * @param max value
 * @return the resulting radian
 */
inline Degree Clamp( Degree angle, float min, float max )
{
  return Degree( Clamp<float>( angle.degree, min, max ) );
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_DEGREE_H__
