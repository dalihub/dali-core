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
 */
struct Radian
{
  /**
   * @brief default constructor, initialises to 0.
   */
  Radian()
  : radian( 0.f )
  { }

  /**
   * @brief Create an angle in radians.
   *
   * @param[in] value The initial value in radians.
   */
  explicit Radian( float value )
  : radian( value )
  { }

  /**
   * @brief Create an angle in radians from an angle in degrees.
   *
   * @param[in] degree The initial value in degrees.
   */
  Radian( Degree degree )
  : radian( degree.degree * Math::PI_OVER_180 )
  { }

  /**
   * @brief Assign an angle from a float value.
   *
   * @param[in] value Float value in radians
   * @return a reference to this object
   */
  Radian& operator=( float value )
  {
    radian = value;
    return *this;
  }

  /**
   * @brief Assign an angle from a Degree value.
   *
   * @param[in] degree The value in degrees.
   * @return a reference to this object
   */
  Radian& operator=( Degree degree )
  {
    radian = degree.degree * Math::PI_OVER_180;
    return *this;
  }

  /**
   * @brief Conversion to float
   * @return the float value of this Radian
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
 * @brief Compare equality between two radians.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return true if the values are identical
 */
inline bool operator==( Radian lhs, Radian rhs )
{
  return fabsf( lhs.radian - rhs.radian ) < Math::MACHINE_EPSILON_10; // expect Radian angles to be between 0 and 10 (multiplies of Math::PI)
}

/**
 * @brief Compare inequality between two radians.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return true if the values are not identical
 */
inline bool operator!=( Radian lhs, Radian rhs )
{
  return !( operator==( lhs, rhs ) );
}

/**
 * @brief Compare equality between a radian and degree.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return true if the values are identical
 */
inline bool operator==( Radian lhs, Degree rhs )
{
  return fabsf( lhs.radian - Radian( rhs ).radian ) < Math::MACHINE_EPSILON_100; // expect Degree angles to be between 0 and 999
}

/**
 * @brief Compare inequality between a radian and a degree.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return true if the values are not identical
 */
inline bool operator!=( Radian lhs, Degree rhs )
{
  return !( operator==( lhs, rhs ) );
}

/**
 * @brief Compare equality between a degree and a radian.
 *
 * @param[in] lhs Degree to compare
 * @param[in] rhs Radian to compare to
 * @return true if the values are identical
 */
inline bool operator==( Degree lhs, Radian rhs )
{
  return fabsf( Radian( lhs ).radian - rhs.radian ) < Math::MACHINE_EPSILON_100; // expect Degree angles to be between 0 and 999
}

/**
 * @brief Compare inequality between a degree and a radian.
 *
 * @param[in] lhs Degree to compare
 * @param[in] rhs Radian to compare to
 * @return true if the values are not identical
 */
inline bool operator!=( Degree lhs, Radian rhs )
{
  return !( operator==( lhs, rhs ) );
}

/**
 * @brief Compare greater than between two radians
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return true if lhs is greater than rhs
 */
inline bool operator>( Radian lhs, Radian rhs )
{
  return lhs.radian > rhs.radian;
}

/**
 * @brief Compare greater than between a radian and a degree.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return true if lhs is greater than rhs
 */
inline bool operator>( Radian lhs, Degree rhs )
{
  return lhs.radian > Radian(rhs).radian;
}

/**
 * @brief Compare greater than between a radian and a degree.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return true if lhs is greater than rhs
 */
inline bool operator>( Degree lhs, Radian rhs )
{
  return Radian(lhs).radian > rhs.radian;
}

/**
 * @brief Compare less than between two radians.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Radian to compare to
 * @return true if lhs is less than rhs
 */
inline bool operator<( Radian lhs, Radian rhs )
{
  return lhs.radian < rhs.radian;
}

/**
 * @brief Compare less than between a radian and a degree.
 *
 * @param[in] lhs Radian to compare
 * @param[in] rhs Degree to compare to
 * @return true if lhs is less than rhs
 */
inline bool operator<( Radian lhs, Degree rhs )
{
  return lhs.radian < Radian(rhs).radian;
}

/**
 * @brief Compare less than between a degree and a radian.
 *
 * @param[in] lhs Degree to compare
 * @param[in] rhs Radian to compare to
 * @return true if lhs is less than rhs
 */
inline bool operator<( Degree lhs, Radian rhs )
{
  return Radian(lhs).radian < rhs.radian;
}

/**
 * @brief Multiply Radian with a float
 *
 * @param[in] lhs Radian to multiply
 * @param[in] rhs float to multiply
 * @return result of the multiplication
 */
inline Radian operator*( Radian lhs, float rhs )
{
  return Radian( lhs.radian * rhs );
}

/**
 * @brief Negate the radian
 * @return The negative angle
 */
inline Radian operator-( Radian in )
{
   return Radian( -in.radian );
}

/**
 * @brief Clamp a radian value
 * @param angle to clamp
 * @param min value
 * @param max value
 * @return the resulting radian
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
