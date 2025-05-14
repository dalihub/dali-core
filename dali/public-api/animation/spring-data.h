#ifndef DALI_SPRING_DATA_H
#define DALI_SPRING_DATA_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

/**
 * @brief Structure for custom spring parameters.
 * This allows defining a custom spring-based easing curve using physics parameters.
 * @SINCE_2_4.17
 */
struct DALI_CORE_API SpringData
{
  /**
   * @brief Creates a spring data
   *
   * @SINCE_2_4.17
   * @param[in] stiffness Spring stiffness(Hooke's constant). Higher values make the spring snap back faster. Minimum value is 0.1.
   * @param[in] damping Damping coefficient. Controls oscillation and settling. Minimum value is 0.1.
   * @param[in] mass Mass of the object. Affects inertia and the duration of the motion. Minimum value is 0.1.
   */
  SpringData(float stiffness, float damping, float mass);

  float stiffness; ///< Spring stiffness(Hooke's constant). Higher values make the spring snap back faster. Minimum value is 0.1.
  float damping;   ///< Damping coefficient. Controls oscillation and settling. Minimum value is 0.1.
  float mass;      ///< Mass of the object. Affects inertia and the duration of the motion. Minimum value is 0.1.

  /**
   * @brief Returns the time in seconds it takes for a Spring Animation to converge based on the SpringData.
   * The maximum value for the returned duration is 100 seconds.
   * Since this value is calculated in an incremental manner, it may take longer if used frequently.
   *
   * @SINCE_2_4.18
   * @return Expected duration for input springData.
   */
  float GetDuration();
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_SPRING_DATA_H
