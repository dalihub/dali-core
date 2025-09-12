#ifndef DALI_DEGREE_H
#define DALI_DEGREE_H

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
   * @brief Default constructor, initialises to 0.
   * @SINCE_1_0.0
   */
  Degree()
  : degree(0.f)
  {
  }

  /**
   * @brief Creates an angle in degrees.
   *
   * @SINCE_1_0.0
   * @param[in] value The initial value in degrees
   */
  explicit constexpr Degree(float value)
  : degree(value)
  {
  }

  /**
   * @brief Creates an angle in degrees from a Radian.
   *
   * @SINCE_1_0.0
   * @param[in] value The initial value in Radians
   */
  DALI_CORE_API Degree(Radian value);

public:
  Degree(const Degree&)                = default; ///< Default copy constructor
  Degree(Degree&&) noexcept            = default; ///< Default move constructor
  Degree& operator=(const Degree&)     = default; ///< Default copy assignment operator
  Degree& operator=(Degree&&) noexcept = default; ///< Default move assignment operator

public:
  // member data
  float degree; ///< The value in degrees
};

// compiler generated destructor, copy constructor and assignment operators are ok as this class is POD

/**
 * @brief Compares equality between two degrees.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Degree to compare
 * @param[in] rhs Degree to compare to
 * @return True if the values are identical
 */
inline bool operator==(const Degree& lhs, const Degree& rhs)
{
  return fabsf(lhs.degree - rhs.degree) < Math::MACHINE_EPSILON_1000; // expect degree angles to be between 0 and 1000
}

/**
 * @brief Compares inequality between two degrees.
 *
 * @SINCE_1_0.0
 * @param[in] lhs Degree to compare
 * @param[in] rhs Degree to compare to
 * @return True if the values are not identical
 */
inline bool operator!=(const Degree& lhs, const Degree& rhs)
{
  return !(operator==(lhs, rhs));
}

/**
 * @brief Clamps a radian value.
 * @SINCE_1_0.0
 * @param angle to clamp
 * @param min value
 * @param max value
 * @return The resulting radian
 */
inline Degree Clamp(Degree angle, float min, float max)
{
  return Degree(Clamp<float>(angle.degree, min, max));
}

/**
 * @}
 */
} // namespace Dali

#endif // DALI_DEGREE_H
