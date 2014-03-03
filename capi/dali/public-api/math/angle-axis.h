#ifndef __DALI_ANGLE_AXIS_H__
#define __DALI_ANGLE_AXIS_H__

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
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/vector3.h>

namespace Dali DALI_IMPORT_API
{

struct Radian;

/**
 * An angle & axis pair.
 */
struct DALI_IMPORT_API AngleAxis
{
  /**
   * Create an angle-axis pair.
   */
  AngleAxis();

  /**
   * Create an angle-axis pair.
   * @param[in] initialAngle The initial angle in degrees.
   * @param[in] initialAxis The initial axis.
   */
  AngleAxis( Degree initialAngle, Vector3 initialAxis );

  /**
   * Create an angle-axis pair.
   * @param[in] initialAngle The initial angle in radians.
   * @param[in] initialAxis The initial axis.
   */
  AngleAxis( Radian initialAngle, Vector3 initialAxis );

  Degree angle;
  Vector3 axis;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ANGLE_AXIS_H__
