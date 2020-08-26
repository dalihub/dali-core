#ifndef DALI_CONSTANTS_H
#define DALI_CONSTANTS_H

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
#include <math.h> // M_PI

// INTERNAL INCLUDES
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief ParentOrigin constants.
 *
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f).
 * @SINCE_1_0.0
 */
namespace ParentOrigin
{
static constexpr float TOP = 0.0f;
static constexpr float BOTTOM = 1.0f;
static constexpr float LEFT = 0.0f;
static constexpr float RIGHT = 1.0f;
static constexpr float MIDDLE = 0.5f;
static constexpr Vector3 TOP_LEFT     (0.0f, 0.0f, 0.5f);
static constexpr Vector3 TOP_CENTER   (0.5f, 0.0f, 0.5f);
static constexpr Vector3 TOP_RIGHT    (1.0f, 0.0f, 0.5f);
static constexpr Vector3 CENTER_LEFT  (0.0f, 0.5f, 0.5f);
static constexpr Vector3 CENTER       (0.5f, 0.5f, 0.5f);
static constexpr Vector3 CENTER_RIGHT (1.0f, 0.5f, 0.5f);
static constexpr Vector3 BOTTOM_LEFT  (0.0f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_CENTER(0.5f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_RIGHT (1.0f, 1.0f, 0.5f);
static constexpr Vector3 DEFAULT( TOP_LEFT );
}

/**
 * @brief AnchorPoint constants.
 *
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f).
 * @SINCE_1_0.0
 */
namespace AnchorPoint
{
static constexpr float TOP = 0.0f;
static constexpr float BOTTOM = 1.0f;
static constexpr float LEFT = 0.0f;
static constexpr float RIGHT = 1.0f;
static constexpr float MIDDLE = 0.5f;
static constexpr Vector3 TOP_LEFT     (0.0f, 0.0f, 0.5f);
static constexpr Vector3 TOP_CENTER   (0.5f, 0.0f, 0.5f);
static constexpr Vector3 TOP_RIGHT    (1.0f, 0.0f, 0.5f);
static constexpr Vector3 CENTER_LEFT  (0.0f, 0.5f, 0.5f);
static constexpr Vector3 CENTER       (0.5f, 0.5f, 0.5f);
static constexpr Vector3 CENTER_RIGHT (1.0f, 0.5f, 0.5f);
static constexpr Vector3 BOTTOM_LEFT  (0.0f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_CENTER(0.5f, 1.0f, 0.5f);
static constexpr Vector3 BOTTOM_RIGHT (1.0f, 1.0f, 0.5f);
static constexpr Vector3 DEFAULT( CENTER );
}


/**
 * @brief Color Constants.
 *
 * Color is represented by the Vector4 class (see vector4.h).
 * @SINCE_1_0.0
 */
namespace Color
{
static constexpr Vector4 BLACK( 0.0f, 0.0f, 0.0f, 1.0f );
static constexpr Vector4 WHITE( 1.0f, 1.0f, 1.0f, 1.0f );
static constexpr Vector4 RED(   1.0f, 0.0f, 0.0f, 1.0f );
static constexpr Vector4 GREEN( 0.0f, 1.0f, 0.0f, 1.0f );
static constexpr Vector4 BLUE(  0.0f, 0.0f, 1.0f, 1.0f );
static constexpr Vector4 YELLOW(  1.0f, 1.0f, 0.0f, 1.0f );
static constexpr Vector4 MAGENTA( 1.0f, 0.0f, 1.0f, 1.0f );
static constexpr Vector4 CYAN(    0.0f, 1.0f, 1.0f, 1.0f );
static constexpr Vector4 TRANSPARENT( 0.0f, 0.0f, 0.0f, 0.0f );

} // namespace Color

/**
 * @brief Math constants.
 * @SINCE_1_0.0
 */
namespace Math
{
static constexpr float MACHINE_EPSILON_0     = Epsilon<0>::value;
static constexpr float MACHINE_EPSILON_1     = Epsilon<1>::value;
static constexpr float MACHINE_EPSILON_10    = Epsilon<10>::value;
static constexpr float MACHINE_EPSILON_100   = Epsilon<100>::value;
static constexpr float MACHINE_EPSILON_1000  = Epsilon<1000>::value;
static constexpr float MACHINE_EPSILON_10000 = Epsilon<10000>::value;

// float is preferred to double for performance on ARM targets
static constexpr float PI   = static_cast<float>(M_PI);       ///< Pi
static constexpr float PI_2 = static_cast<float>(M_PI_2);     ///< Pi/2
static constexpr float PI_4 = static_cast<float>(M_PI_4);     ///< Pi/4
static constexpr float PI_OVER_180 = Dali::Math::PI/180.0f;   ///< Constant used to convert degree to radian
static constexpr float ONE80_OVER_PI = 180.0f/Dali::Math::PI; ///< Constant used to convert radian to degree

} // namespace Math

/**
 * @}
 */
} // namespace Dali

#endif // DALI_CONSTANTS_H
