#ifndef __DALI_RADIAN_H__
#define __DALI_RADIAN_H__

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
#include <dali/public-api/math/degree.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

/**
 * @brief An angle in radians.
 *
 * This reduces ambiguity when using methods which accept angles in degrees or radians.
 * @SINCE_1_0.0
 */
struct Radian
{
  /**
   * @brief Default constructor, initializes to 0.
   * @SINCE_1_0.0
   */
  Radian()
  : radian( 0.f )
  { }

  /**
   * @brief Creates an angle in radians.
   *
   * @SINCE_1_0.0
   * @param[in] value The initial value in radians
   */
  explicit Radian( float value )
  : radian( value )
  { }

  /**
   * @brief Creates an angle in radians from an angle in degrees.
   *
   * @SINCE_1_0.0
   * @param[in] degree The initial value in degrees
   */
  Radian( Degree degree )
  : radian( degree.degree * Math::PI_OVER_180 )
  { }

  /**
   * @brief Assigns an angle from a float value.
   *
   * @SINCE_1_0.0
   * @param[in] value Float value in radians
   * @return A reference to this
   */
  Radian& operator=( float value )
  {
    radian = value;
    return *this;
  }

  /**
   * @brief Assigns an angle from a Degree value.
   *
   * @SINCE_1_0.0
   * @param[in] degree The value in degrees
   * @return A reference to this
   */
  Radian& operator=( Degree degree )
  {
    radian = degree.degree * Math::PI_OVER_180;
    return *this;
  }

  /**
   * @brief Conversion to float.
   * @SINCE_1_0.0
   * @return The float value of this Radian
   */
  operator float() const
  {
    return radian;
  }

public:

  // member data
  float radian; ///< The value in radians

};

// compiler generated destructor, copy constructor and assignment operators are ok as this class is POD

/**
 * @brief Compares equality between two radians.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return True if the values are identical
 */
inline bool operator==( Radian lhs, Radian rhs )
{
  return fabsf( lhs.radian - rhs.radian ) < Math::MACHINE_EPSILON_10; // expect Radian angles to be between 0 and 10 (multiplies of Math::PI)
}

/**
 * @brief Compares inequality between two radians.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return True if the values are not identical
 */
inline bool operator!=( Radian lhs, Radian rhs )
{
  return !( operator==( lhs, rhs ) );
}

/**
 * @brief Compares equality between a radian and degree.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return True if the values are identical
 */
inline bool operator==( Radian lhs, Degree rhs )
{
  return fabsf( lhs.radian - Radian( rhs ).radian ) < Math::MACHINE_EPSILON_100; // expect Degree angles to be between 0 and 999
}

/**
 * @brief Compares inequality between a radian and a degree.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return True if the values are not identical
 */
inline bool operator!=( Radian lhs, Degree rhs )
{
  return !( operator==( lhs, rhs ) );
}

/**
 * @brief Compares equality between a degree and a radian.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Degree to compare
 * @param[in] rhs Radian to compare to
 * @return True if the values are identical
 */
inline bool operator==( Degree lhs, Radian rhs )
{
  return fabsf( Radian( lhs ).radian - rhs.radian ) < Math::MACHINE_EPSILON_100; // expect Degree angles to be between 0 and 999
}

/**
 * @brief Compares inequality between a degree and a radian.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Degree to compare
 * @param[in] rhs Radian to compare to
 * @return True if the values are not identical
 */
inline bool operator!=( Degree lhs, Radian rhs )
{
  return !( operator==( lhs, rhs ) );
}

/**
 * @brief Compares greater than between two radians
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return True if lhs is greater than rhs
 */
inline bool operator>( Radian lhs, Radian rhs )
{
  return lhs.radian > rhs.radian;
}

/**
 * @brief Compares greater than between a radian and a degree.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return True if lhs is greater than rhs
 */
inline bool operator>( Radian lhs, Degree rhs )
{
  return lhs.radian > Radian(rhs).radian;
}

/**
 * @brief Compares greater than between a radian and a degree.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return True if lhs is greater than rhs
 */
inline bool operator>( Degree lhs, Radian rhs )
{
  return Radian(lhs).radian > rhs.radian;
}

/**
 * @brief Compares less than between two radians.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return True if lhs is less than rhs
 */
inline bool operator<( Radian lhs, Radian rhs )
{
  return lhs.radian < rhs.radian;
}

/**
 * @brief Compares less than between a radian and a degree.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return True if lhs is less than rhs
 */
inline bool operator<( Radian lhs, Degree rhs )
{
  return lhs.radian < Radian(rhs).radian;
}

/**
 * @brief Compares less than between a degree and a radian.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Degree to compare
 * @param[in] rhs Radian to compare to
 * @return True if lhs is less than rhs
 */
inline bool operator<( Degree lhs, Radian rhs )
{
  return Radian(lhs).radian < rhs.radian;
}

/**
 * @brief Multiplies Radian with a float.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Radian to multiply
 * @param[in] rhs float to multiply
 * @return Result of the multiplication
 */
inline Radian operator*( Radian lhs, float rhs )
{
  return Radian( lhs.radian * rhs );
}

/**
 * @brief Negates the radian.
 * @SINCE_1_0.0
 * @param[in] in Radian to negate
 * @return The negative angle
 */
inline Radian operator-( Radian in )
{
   return Radian( -in.radian );
}

/**
 * @brief Clamps a radian value.
 * @SINCE_1_0.0
 * @param angle to clamp
 * @param min value
 * @param max value
 * @return The resulting radian
 */
inline Radian Clamp( Radian angle, float min, float max )
{
  return Radian( Clamp<float>( angle.radian, min, max ) );
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_RADIAN_H__
