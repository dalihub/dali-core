#ifndef __DALI_ANGLE_AXIS_H__
#define __DALI_ANGLE_AXIS_H__

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

/**
 * @addtogroup CAPI_DALI_MATH_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/vector3.h>

namespace Dali DALI_IMPORT_API
{

struct Radian;

/**
 * @brief An angle & axis pair.
 *
 * This is slightly easier to understand than quaternions for handling rotations
 * of objects. Both elements should be non-zero to correctly describe a rotation.
 */
struct DALI_IMPORT_API AngleAxis
{
  /**
   * @brief Create an angle-axis pair.
   *
   */
  AngleAxis();

  /**
   * @brief Create an angle-axis pair.
   *
   * @param[in] initialAngle The initial angle in degrees.
   * @param[in] initialAxis The initial axis.
   */
  AngleAxis( Degree initialAngle, Vector3 initialAxis );

  /**
   * @brief Create an angle-axis pair.
   *
   * @param[in] initialAngle The initial angle in radians.
   * @param[in] initialAxis The initial axis.
   */
  AngleAxis( Radian initialAngle, Vector3 initialAxis );

  Degree angle; ///< The angle in degrees
  Vector3 axis; ///< The axis
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ANGLE_AXIS_H__
