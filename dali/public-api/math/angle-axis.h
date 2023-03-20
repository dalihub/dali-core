#ifndef DALI_ANGLE_AXIS_H
#define DALI_ANGLE_AXIS_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <iosfwd>
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

struct Radian;

/**
 * @brief An angle & axis pair.
 *
 * This is slightly easier to understand than quaternions for handling rotations
 * of objects. Both elements should be non-zero to correctly describe a rotation.
 * @SINCE_1_0.0
 */
struct AngleAxis
{
  /**
   * @brief Creates an angle-axis pair.
   * @SINCE_1_0.0
   */
  AngleAxis()
  : angle(0.0f),
    axis(0.0f, 0.0f, 0.0f)
  {
  }

  /**
   * @brief Creates an angle-axis pair.
   *
   * @SINCE_1_0.0
   * @param[in] initialAngle The initial angle in radians
   * @param[in] initialAxis The initial axis
   */
  AngleAxis(Radian initialAngle, const Vector3& initialAxis)
  : angle(initialAngle),
    axis(initialAxis)
  {
  }

public:
  AngleAxis(const AngleAxis&) = default;            ///< Default copy constructor
  AngleAxis(AngleAxis&&) noexcept = default;            ///< Default move constructor
  AngleAxis& operator=(const AngleAxis&) = default; ///< Default copy assignment operator
  AngleAxis& operator=(AngleAxis&&) noexcept = default;      ///< Default move assignment operator

public:
  Radian  angle; ///< The angle in radians
  Vector3 axis;  ///< The axis
};

// Compiler generated destructor, copy constructor and assignment operators are ok as this class is POD

/**
 * @brief Compares two angle axis for equality.
 *
 * @SINCE_1_0.0
 * @param lhs angle axis
 * @param rhs angle axis
 * @return True if they are equal
 */
inline bool operator==(const Dali::AngleAxis& lhs, const Dali::AngleAxis& rhs)
{
  return (lhs.angle == rhs.angle) && (lhs.axis == rhs.axis);
}

/**
 * @brief Prints an angle axis.
 *
 * @SINCE_1_1.33
 * @param[in] o The output stream operator
 * @param[in] angleAxis The angle axis to print
 * @return The output stream operator
 */
inline std::ostream& operator<<(std::ostream& o, const Dali::AngleAxis& angleAxis)
{
  return o << "[ Axis: [" << angleAxis.axis.x << ", " << angleAxis.axis.y << ", " << angleAxis.axis.z << "], Angle: " << Degree(angleAxis.angle).degree << " degrees ]";
}

/**
 * @}
 */
} // namespace Dali

#endif // DALI_ANGLE_AXIS_H
