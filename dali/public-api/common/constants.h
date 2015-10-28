#ifndef __DALI_CONSTANTS_H__
#define __DALI_CONSTANTS_H__

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

// EXTERNAL INCLUDES
#include <math.h> // M_PI

// INTERNAL INCLUDES
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief ParentOrigin constants
 *
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f)
 */
namespace ParentOrigin
{
extern const Vector3 DEFAULT; ///< TOP_LEFT
static const float TOP = 0.0f;
static const float BOTTOM = 1.0f;
static const float LEFT = 0.0f;
static const float RIGHT = 1.0f;
static const float MIDDLE = 0.5f;

DALI_IMPORT_API extern const Vector3 TOP_LEFT;           ///< 0.0f, 0.0f, 0.5f
DALI_IMPORT_API extern const Vector3 TOP_CENTER;         ///< 0.5f, 0.0f, 0.5f
DALI_IMPORT_API extern const Vector3 TOP_RIGHT;          ///< 1.0f, 0.0f, 0.5f
DALI_IMPORT_API extern const Vector3 CENTER_LEFT;        ///< 0.0f, 0.5f, 0.5f
DALI_IMPORT_API extern const Vector3 CENTER;             ///< 0.5f, 0.5f, 0.5f
DALI_IMPORT_API extern const Vector3 CENTER_RIGHT;       ///< 1.0f, 0.5f, 0.5f
DALI_IMPORT_API extern const Vector3 BOTTOM_LEFT;        ///< 0.0f, 1.0f, 0.5f
DALI_IMPORT_API extern const Vector3 BOTTOM_CENTER;      ///< 0.5f, 1.0f, 0.5f
DALI_IMPORT_API extern const Vector3 BOTTOM_RIGHT;       ///< 1.0f, 1.0f, 0.5f

}

/**
 * @brief AnchorPoint constants
 *
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f)
 */
namespace AnchorPoint
{
static const float TOP = 0.0f;
static const float BOTTOM = 1.0f;
static const float LEFT = 0.0f;
static const float RIGHT = 1.0f;
static const float MIDDLE = 0.5f;

extern const Vector3 DEFAULT; ///< CENTER
DALI_IMPORT_API extern const Vector3 TOP_LEFT;           ///< 0.0f, 0.0f, 0.5f
DALI_IMPORT_API extern const Vector3 TOP_CENTER;         ///< 0.5f, 0.0f, 0.5f
DALI_IMPORT_API extern const Vector3 TOP_RIGHT;          ///< 1.0f, 0.0f, 0.5f
DALI_IMPORT_API extern const Vector3 CENTER_LEFT;        ///< 0.0f, 0.5f, 0.5f
DALI_IMPORT_API extern const Vector3 CENTER;             ///< 0.5f, 0.5f, 0.5f
DALI_IMPORT_API extern const Vector3 CENTER_RIGHT;       ///< 1.0f, 0.5f, 0.5f
DALI_IMPORT_API extern const Vector3 BOTTOM_LEFT;        ///< 0.0f, 1.0f, 0.5f
DALI_IMPORT_API extern const Vector3 BOTTOM_CENTER;      ///< 0.5f, 1.0f, 0.5f
DALI_IMPORT_API extern const Vector3 BOTTOM_RIGHT;       ///< 1.0f, 1.0f, 0.5f

}


/**
 * @brief Color Constants.
 *
 * Color is represented by the Vector4 class (see vector4.h).
 */
namespace Color
{
DALI_IMPORT_API extern const Vector4 BLACK;       ///< Pure black (0.0f, 0.0f, 0.0f, 1.0f);
DALI_IMPORT_API extern const Vector4 WHITE;       ///< Pure white (1.0f, 1.0f, 1.0f, 1.0f);

DALI_IMPORT_API extern const Vector4 RED;         ///< Pure red   (1.0f, 0.0f, 0.0f, 1.0f);
DALI_IMPORT_API extern const Vector4 GREEN;       ///< Pure green (0.0f, 1.0f, 0.0f, 1.0f);
DALI_IMPORT_API extern const Vector4 BLUE;        ///< Pure blue  (0.0f, 0.0f, 1.0f, 1.0f);

DALI_IMPORT_API extern const Vector4 YELLOW;      ///< Pure yellow  (1.0f, 1.0f, 0.0f, 1.0f);
DALI_IMPORT_API extern const Vector4 MAGENTA;     ///< Pure magenta (1.0f, 0.0f, 1.0f, 1.0f);
DALI_IMPORT_API extern const Vector4 CYAN;        ///< Pure cyan    (0.0f, 1.0f, 1.0f, 1.0f);

DALI_IMPORT_API extern const Vector4 TRANSPARENT; ///< Black transparent (0.0f, 0.0f, 0.0f, 0.0f);

} // namespace Color

/**
 * @brief Math constants
 */
namespace Math
{
DALI_IMPORT_API extern const float MACHINE_EPSILON_0;      ///< Epsilon for values near zero
DALI_IMPORT_API extern const float MACHINE_EPSILON_1;      ///< Epsilon for values near 1
DALI_IMPORT_API extern const float MACHINE_EPSILON_10;     ///< Epsilon for values near 10
DALI_IMPORT_API extern const float MACHINE_EPSILON_100;    ///< Epsilon for values near 100
DALI_IMPORT_API extern const float MACHINE_EPSILON_1000;   ///< Epsilon for values near 1000
DALI_IMPORT_API extern const float MACHINE_EPSILON_10000;  ///< Epsilon for values near 10000

// float is preferred to double for performance on ARM targets
static const float PI   = static_cast<float>(M_PI);       ///< Constant representing PI
static const float PI_2 = static_cast<float>(M_PI_2);     ///< Constant representing PI/2
static const float PI_4 = static_cast<float>(M_PI_4);     ///< Constant representing PI/4
static const float PI_OVER_180 = Dali::Math::PI/180.0f;   ///< Constant used to convert degree to radian
static const float ONE80_OVER_PI = 180.0f/Dali::Math::PI; ///< Constant used to convert radian to degree

} // namespace Math

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_CONSTANTS_H__
