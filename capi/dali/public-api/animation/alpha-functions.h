#ifndef __DALI_ALPHA_FUNCTIONS_H__
#define __DALI_ALPHA_FUNCTIONS_H__

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
 * @addtogroup CAPI_DALI_ANIMATION_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

typedef float (*AlphaFunction)(float progress); ///< Definition of an alpha function

/**
 * @brief Namespace containing a set of alpha functions.
 */
namespace AlphaFunctions
{

float Default(float progres);     ///< Linear
float Linear(float progress);     ///< Linear
float Square(float progress);     ///< Square (x^2)
float Reverse(float progress);    ///< Reverse linear

float EaseIn(float progress);     ///< Speeds up and comes to a sudden stop
float EaseOut(float progress);    ///< Sudden start and slows to a gradual stop
float EaseInOut(float progress);  ///< Speeds up and slows to a gradual stop

float EaseInSine(float progress);    ///< Speeds up and comes to a sudden stop
float EaseOutSine(float progress);   ///< Sudden start and slows to a gradual stop
float EaseInOutSine(float progress); ///< Speeds up and slows to a gradual stop

float EaseInSine33(float progress);          ///< Speeds up and comes to a sudden stop
float EaseOutSine33(float progress);         ///< Sudden start and slows to a gradual stop
float EaseInOutSine33(float progress);       ///< Speeds up and slows to a gradual stop
float EaseInOutSine50(float progress);       ///< Speeds up and slows to a gradual stop
float EaseInOutSine60(float progress);       ///< Speeds up and slows to a gradual stop
float EaseInOutSine70(float progress);       ///< Speeds up and slows to a gradual stop
float EaseInOutSine80(float progress);       ///< Speeds up and slows to a gradual stop
float EaseInOutSine90(float progress);       ///< Speeds up and slows to a gradual stop
float DoubleEaseInOutSine60(float progress); ///< Speeds up and slows to a gradual stop, then speeds up again and slows to a gradual stop

float EaseOutQuint50(float progress);        ///< Sudden start and slows to a gradual stop
float EaseOutQuint80(float progress);        ///< Sudden start and slows to a gradual stop

float Bounce(float progress);     ///< Sudden start, loses momentum and returns to start position
float BounceBack(float progress); ///< Sudden start, loses momentum and returns to exceed start position and gradual stop at start position

float EaseInBack(float progress);    ///< Slow start, exceed start position and quickly reach destination
float EaseOutBack(float progress);   ///< Sudden start, exceed end position and return to a gradual stop
float EaseInOutBack(float progress); ///< Slow start, exceed start position, fast middle, exceed end position and return to a gradual stop

float Sin(float progress);        ///< Single revolution
float Sin2x(float progress);      ///< Two revolutions

} // namespace AlphaFunctions

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_ALPHA_FUNCTIONS_H__
