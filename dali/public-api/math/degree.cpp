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

// CLASS HEADER
#include <dali/public-api/math/degree.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/radian.h>

namespace Dali
{

Degree::Degree( Radian radian )
: degree( radian * Math::ONE80_OVER_PI )
{
}

const Radian ANGLE_360 = Radian( Math::PI * 2.f     ); ///< 360 degree turn in radians
const Radian ANGLE_180 = Radian( Math::PI           ); ///< 180 degree turn in radians
const Radian ANGLE_120 = Radian( Math::PI * 2.f/3.f ); ///< 120 degree turn in radians
const Radian ANGLE_90  = Radian( Math::PI_2         ); ///< 90 degree turn in radians
const Radian ANGLE_60  = Radian( Math::PI / 3.f     ); ///< 60 degree turn in radians
const Radian ANGLE_45  = Radian( Math::PI_4         ); ///< 45 degree turn in radians
const Radian ANGLE_30  = Radian( Math::PI / 6.f     ); ///< 30 degree turn in radians
const Radian ANGLE_0   = Radian( 0.0f               ); ///< 0 degree turn in radians

} // namespace Dali
