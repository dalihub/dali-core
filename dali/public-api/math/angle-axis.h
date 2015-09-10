#ifndef __DALI_ANGLE_AXIS_H__
#define __DALI_ANGLE_AXIS_H__

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
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{
/**
 * @addtogroup dali-core-object
 * @{
 */

struct Radian;

/**
 * @brief An angle & axis pair.
 *
 * This is slightly easier to understand than quaternions for handling rotations
 * of objects. Both elements should be non-zero to correctly describe a rotation.
 */
struct AngleAxis
{
  /**
   * @brief Create an angle-axis pair.
   */
  AngleAxis()
  : angle(0.0f),
    axis(0.0f, 0.0f, 0.0f)
  { }

  /**
   * @brief Create an angle-axis pair.
   *
   * @param[in] initialAngle The initial angle in radians.
   * @param[in] initialAxis The initial axis.
   */
  AngleAxis( Radian initialAngle, const Vector3& initialAxis )
  : angle( initialAngle ),
    axis( initialAxis )
  { }

  Radian angle; ///< The angle in radians
  Vector3 axis; ///< The axis

};

// compiler generated destructor, copy constructor and assignment operators are ok as this class is POD

/**
 * @brief Compare two angle axis for equality
 *
 * @param lhs angle axis
 * @param rhs angle axis
 * @return true if they are equal
 */
inline bool operator==( const Dali::AngleAxis& lhs, const Dali::AngleAxis& rhs )
{
  return (lhs.angle == rhs.angle) && (lhs.axis == rhs.axis);
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_ANGLE_AXIS_H__
