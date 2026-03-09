#ifndef DALI_SPRING_DATA_H
#define DALI_SPRING_DATA_H

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
class DALI_CORE_API SpringData
{
public:
  /**
   * @brief Creates a spring data
   *
   * @SINCE_2_4.17
   * @param[in] stiffness Spring stiffness(Hooke's constant). Higher values make the spring snap back faster. Minimum value is 0.1.
   * @param[in] damping Damping coefficient. Controls oscillation and settling. Minimum value is 0.1.
   * @param[in] mass Mass of the object. Affects inertia and the duration of the motion. Minimum value is 0.1.
   */
  SpringData(float stiffness, float damping, float mass);

  /**
   * @brief Default constructor.
   * @SINCE_2_5.14
   */
  SpringData();

  /**
   * @brief Destructor.
   * @SINCE_2_5.14
   */
  ~SpringData();

  /**
   * @brief Copy constructor.
   * Creates a new SpringData object by copying the contents of another.
   * @SINCE_2_5.14
   * @param[in] rhs The SpringData object to copy from.
   */
  SpringData(const SpringData& rhs);

  /**
   * @brief Move constructor.
   * Creates a new SpringData object by moving the contents of another.
   * @SINCE_2_5.14
   * @param[in] rhs The SpringData object to move from.
   */
  SpringData(SpringData&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * Copies the contents of another SpringData object to this one.
   * @SINCE_2_5.14
   * @param[in] rhs The SpringData object to copy from.
   * @return A reference to this object.
   */
  SpringData& operator=(const SpringData& rhs);

  /**
   * @brief Move assignment operator.
   * Moves the contents of another SpringData object to this one.
   * @SINCE_2_5.14
   * @param[in] rhs The SpringData object to move from.
   * @return A reference to this object.
   */
  SpringData& operator=(SpringData&& rhs) noexcept;

  /**
   * @brief Sets the stiffness.
   * @SINCE_2_5.14
   * @param[in] stiffness Spring stiffness(Hooke's constant). Higher values make the spring snap back faster. Minimum value is 0.1.
   */
  void SetStiffness(float stiffness);

  /**
   * @brief Returns the stiffness.
   * @SINCE_2_5.14
   * @return Spring stiffness(Hooke's constant). Higher values make the spring snap back faster. Minimum value is 0.1.
   */
  float GetStiffness() const;

  /**
   * @brief Sets the damping.
   * @SINCE_2_5.14
   * @param[in] damping Damping coefficient. Controls oscillation and settling. Minimum value is 0.1.
   */
  void SetDamping(float damping);

  /**
   * @brief Returns the damping.
   * @SINCE_2_5.14
   * @return Damping coefficient. Controls oscillation and settling. Minimum value is 0.1.
   */
  float GetDamping() const;

  /**
   * @brief Sets the mass.
   * @SINCE_2_5.14
   * @param[in] mass Mass of the object. Affects inertia and the duration of the motion. Minimum value is 0.1.
   */
  void SetMass(float mass);

  /**
   * @brief Returns the mass.
   * @SINCE_2_5.14
   * @return Mass of the object. Affects inertia and the duration of the motion. Minimum value is 0.1.
   */
  float GetMass() const;

  /**
   * @brief Returns the time in seconds it takes for a Spring Animation to converge based on the SpringData.
   * The maximum value for the returned duration is 100 seconds.
   * Since this value is calculated in an incremental manner, it may take longer if used frequently.
   *
   * @SINCE_2_4.18
   * @return Expected duration for input springData.
   */
  float GetDuration() const;

private:
  /**
   * @brief Internal implementation structure.
   */
  struct Impl;

  Impl* mImpl; ///< Pointer to the implementation
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_SPRING_DATA_H
