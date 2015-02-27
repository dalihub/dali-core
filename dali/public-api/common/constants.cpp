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

// CLASS HEADER
#include <dali/public-api/common/constants.h>

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/degree.h>

namespace Dali
{

// epsilon constants
const float Math::MACHINE_EPSILON_0     = Epsilon<0>::value;
const float Math::MACHINE_EPSILON_1     = Epsilon<1>::value;
const float Math::MACHINE_EPSILON_10    = Epsilon<10>::value;
const float Math::MACHINE_EPSILON_100   = Epsilon<100>::value;
const float Math::MACHINE_EPSILON_1000  = Epsilon<1000>::value;
const float Math::MACHINE_EPSILON_10000 = Epsilon<10000>::value;

const std::string String::EMPTY("");

} // namespace Dali
