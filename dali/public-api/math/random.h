#ifndef DALI_RANDOM_H
#define DALI_RANDOM_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector4.h>

// EXTERNAL INCLUDES
#include <cstdlib> ///< kept for applications that relied on this header for srand() and rand()
#include <ctime>   ///< kept for applications that relied on this header for time()

namespace DALI_NAMESPACE
{
/**
 * @addtogroup dali_core_math
 * @{
 */

/**
 * @brief Provides methods to generate and use random values.
 * @SINCE_1_0.0
 */
namespace Random
{
/**
 * @brief Returns a random number between f0 and f1.
 *
 * Note, uses a limited number of values.
 * @SINCE_1_0.0
 * @param[in] f0 The lower bound
 * @param[in] f1 The upper bound
 * @return A random value between the lower and upper bound
 */
DALI_CORE_API float Range(float f0, float f1);

/**
 * @brief Function to return a normalized axis in a random direction.
 *
 * @SINCE_1_0.0
 * @return The axis
 */
DALI_CORE_API Vector4 Axis();

} // namespace Random

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_RANDOM_H
