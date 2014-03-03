#ifndef __DALI_CONSTANTS_H__
#define __DALI_CONSTANTS_H__

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

// EXTERNAL INCLUDES
#include <math.h> // M_PI

// INTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/radian.h>

namespace Dali DALI_IMPORT_API
{

namespace ParentOrigin
{

/**
 * ParentOrigin constants
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f)
 */

extern const Vector3 DEFAULT; // TOP_LEFT

extern const float TOP;    // 0.0f
extern const float BOTTOM; // 1.0f
extern const float LEFT;   // 0.0f
extern const float RIGHT;  // 1.0f
extern const float MIDDLE; // 0.5f

extern const Vector3 BACK_TOP_LEFT;      ///< 0.0f, 0.0f, 0.0f
extern const Vector3 BACK_TOP_CENTER;    ///< 0.5f, 0.0f, 0.0f
extern const Vector3 BACK_TOP_RIGHT;     ///< 1.0f, 0.0f, 0.0f
extern const Vector3 BACK_CENTER_LEFT;   ///< 0.0f, 0.5f, 0.0f
extern const Vector3 BACK_CENTER;        ///< 0.5f, 0.5f, 0.0f
extern const Vector3 BACK_CENTER_RIGHT;  ///< 1.0f, 0.5f, 0.0f
extern const Vector3 BACK_BOTTOM_LEFT;   ///< 0.0f, 1.0f, 0.0f
extern const Vector3 BACK_BOTTOM_CENTER; ///< 0.5f, 1.0f, 0.0f
extern const Vector3 BACK_BOTTOM_RIGHT;  ///< 1.0f, 1.0f, 0.0f

extern const Vector3 TOP_LEFT;           ///< 0.0f, 0.0f, 0.5f
extern const Vector3 TOP_CENTER;         ///< 0.5f, 0.0f, 0.5f
extern const Vector3 TOP_RIGHT;          ///< 1.0f, 0.0f, 0.5f
extern const Vector3 CENTER_LEFT;        ///< 0.0f, 0.5f, 0.5f
extern const Vector3 CENTER;             ///< 0.5f, 0.5f, 0.5f
extern const Vector3 CENTER_RIGHT;       ///< 1.0f, 0.5f, 0.5f
extern const Vector3 BOTTOM_LEFT;        ///< 0.0f, 1.0f, 0.5f
extern const Vector3 BOTTOM_CENTER;      ///< 0.5f, 1.0f, 0.5f
extern const Vector3 BOTTOM_RIGHT;       ///< 1.0f, 1.0f, 0.5f

extern const Vector3 FRONT_TOP_LEFT;     ///< 0.0f, 0.0f, 1.0f
extern const Vector3 FRONT_TOP_CENTER;   ///< 0.5f, 0.0f, 1.0f
extern const Vector3 FRONT_TOP_RIGHT;    ///< 1.0f, 0.0f, 1.0f
extern const Vector3 FRONT_CENTER_LEFT;  ///< 0.0f, 0.5f, 1.0f
extern const Vector3 FRONT_CENTER;       ///< 0.5f, 0.5f, 1.0f
extern const Vector3 FRONT_CENTER_RIGHT; ///< 1.0f, 0.5f, 1.0f
extern const Vector3 FRONT_BOTTOM_LEFT;  ///< 0.0f, 1.0f, 1.0f
extern const Vector3 FRONT_BOTTOM_CENTER;///< 0.5f, 1.0f, 1.0f
extern const Vector3 FRONT_BOTTOM_RIGHT; ///< 1.0f, 1.0f, 1.0f

}

namespace AnchorPoint
{

/**
 * AnchorPoint constants
 * Note that for 2D layouting, the typical Z position is 0.5f (not 0.0f)
 */

extern const float TOP;    // 0.0f
extern const float BOTTOM; // 1.0f
extern const float LEFT;   // 0.0f
extern const float RIGHT;  // 1.0f
extern const float MIDDLE; // 0.5f

extern const Vector3 DEFAULT; // CENTER

extern const Vector3 BACK_TOP_LEFT;      ///< 0.0f, 0.0f, 0.0f
extern const Vector3 BACK_TOP_CENTER;    ///< 0.5f, 0.0f, 0.0f
extern const Vector3 BACK_TOP_RIGHT;     ///< 1.0f, 0.0f, 0.0f
extern const Vector3 BACK_CENTER_LEFT;   ///< 0.0f, 0.5f, 0.0f
extern const Vector3 BACK_CENTER;        ///< 0.5f, 0.5f, 0.0f
extern const Vector3 BACK_CENTER_RIGHT;  ///< 1.0f, 0.5f, 0.0f
extern const Vector3 BACK_BOTTOM_LEFT;   ///< 0.0f, 1.0f, 0.0f
extern const Vector3 BACK_BOTTOM_CENTER; ///< 0.5f, 1.0f, 0.0f
extern const Vector3 BACK_BOTTOM_RIGHT;  ///< 1.0f, 1.0f, 0.0f

extern const Vector3 TOP_LEFT;           ///< 0.0f, 0.0f, 0.5f
extern const Vector3 TOP_CENTER;         ///< 0.5f, 0.0f, 0.5f
extern const Vector3 TOP_RIGHT;          ///< 1.0f, 0.0f, 0.5f
extern const Vector3 CENTER_LEFT;        ///< 0.0f, 0.5f, 0.5f
extern const Vector3 CENTER;             ///< 0.5f, 0.5f, 0.5f
extern const Vector3 CENTER_RIGHT;       ///< 1.0f, 0.5f, 0.5f
extern const Vector3 BOTTOM_LEFT;        ///< 0.0f, 1.0f, 0.5f
extern const Vector3 BOTTOM_CENTER;      ///< 0.5f, 1.0f, 0.5f
extern const Vector3 BOTTOM_RIGHT;       ///< 1.0f, 1.0f, 0.5f

extern const Vector3 FRONT_TOP_LEFT;     ///< 0.0f, 0.0f, 1.0f
extern const Vector3 FRONT_TOP_CENTER;   ///< 0.5f, 0.0f, 1.0f
extern const Vector3 FRONT_TOP_RIGHT;    ///< 1.0f, 0.0f, 1.0f
extern const Vector3 FRONT_CENTER_LEFT;  ///< 0.0f, 0.5f, 1.0f
extern const Vector3 FRONT_CENTER;       ///< 0.5f, 0.5f, 1.0f
extern const Vector3 FRONT_CENTER_RIGHT; ///< 1.0f, 0.5f, 1.0f
extern const Vector3 FRONT_BOTTOM_LEFT;  ///< 0.0f, 1.0f, 1.0f
extern const Vector3 FRONT_BOTTOM_CENTER;///< 0.5f, 1.0f, 1.0f
extern const Vector3 FRONT_BOTTOM_RIGHT; ///< 1.0f, 1.0f, 1.0f

}

namespace Color
{

/**
 * Color Constants.
 * Color is represented by the Vector4 class (see vector4.h).
 */

extern const Vector4 BLACK;       ///< Pure black (0.0f, 0.0f, 0.0f, 1.0f);
extern const Vector4 WHITE;       ///< Pure white (1.0f, 1.0f, 1.0f, 1.0f);

extern const Vector4 RED;         ///< Pure red   (1.0f, 0.0f, 0.0f, 1.0f);
extern const Vector4 GREEN;       ///< Pure green (0.0f, 1.0f, 0.0f, 1.0f);
extern const Vector4 BLUE;        ///< Pure blue  (0.0f, 0.0f, 1.0f, 1.0f);

extern const Vector4 YELLOW;      ///< Pure yellow  (1.0f, 1.0f, 0.0f, 1.0f);
extern const Vector4 MAGENTA;     ///< Pure magenta (1.0f, 0.0f, 1.0f, 1.0f);
extern const Vector4 CYAN;        ///< Pure cyan    (0.0f, 1.0f, 1.0f, 1.0f);

extern const Vector4 TRANSPARENT; ///< Black transparent (0.0f, 0.0f, 0.0f, 0.0f);

} // namespace Color

namespace Math
{

extern const float MACHINE_EPSILON_0;
extern const float MACHINE_EPSILON_1;
extern const float MACHINE_EPSILON_10;
extern const float MACHINE_EPSILON_100;
extern const float MACHINE_EPSILON_1000;
extern const float MACHINE_EPSILON_10000;

// float is preferred to double for performance on ARM targets
const float PI   = static_cast<float>(M_PI);
const float PI_2 = static_cast<float>(M_PI_2);
const float PI_4 = static_cast<float>(M_PI_4);

} // namespace Math

namespace String
{

extern const std::string EMPTY;

}


} // namespace Dali

/**
 * @}
 */
#endif // __DALI_CONSTANTS_H__
