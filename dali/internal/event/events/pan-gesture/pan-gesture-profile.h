#ifndef DALI_INTERNAL_PAN_GESTURE_PROFILE_H
#define DALI_INTERNAL_PAN_GESTURE_PROFILE_H

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

// EXTERNAL INCLUDES
#include <cstdint>
#include <limits>
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/math/radian.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
/**
 * @brief One complete set of pan recognition options: the state behind Dali::PanGestureDetector::Options
 * and the default/per-device profiles kept by the pan detector.
 *
 * Angles are stored normalised: thresholds are positive and clamped to PI, angles are wrapped into
 * [-PI, PI]. Every angle mutation in the detector and in the public Options type goes through this
 * struct so the two cannot drift apart.
 */
struct PanGestureProfile
{
  using AngleThresholdPair = Dali::PanGestureDetector::AngleThresholdPair;
  using AngleContainer     = std::vector<AngleThresholdPair>;

  /**
   * @copydoc Dali::PanGestureDetector::AddAngle()
   */
  void AddAngle(Radian angle, Radian threshold);

  /**
   * @copydoc Dali::PanGestureDetector::AddDirection()
   */
  void AddDirection(Radian direction, Radian threshold);

  /**
   * @copydoc Dali::PanGestureDetector::RemoveAngle()
   */
  void RemoveAngle(Radian angle);

  /**
   * @copydoc Dali::PanGestureDetector::RemoveDirection()
   */
  void RemoveDirection(Radian direction);

  /**
   * @copydoc Dali::PanGestureDetector::ClearAngles()
   */
  void ClearAngles();

  /**
   * @copydoc Dali::PanGestureDetector::GetAngleCount()
   */
  uint32_t GetAngleCount() const;

  /**
   * @copydoc Dali::PanGestureDetector::GetAngle()
   */
  AngleThresholdPair GetAngle(uint32_t index) const;

  /**
   * @brief Checks whether a pan must be in one of the registered directions to be emitted.
   * @return true if at least one angle is registered
   */
  bool RequiresDirectionalPan() const;

  /**
   * @brief Checks whether the given pan angle is within one of the registered angle ranges.
   * @param[in] angle The angle of the pan in local actor coordinates
   * @return true if allowed, or if no angle is registered
   */
  bool IsAngleAllowed(Radian angle) const;

  /**
   * @brief Checks whether the profile is valid for registration: positive touch counts, minimum <= maximum.
   * @return true if valid
   */
  bool IsValid() const;

  uint32_t       minimumTouches{1u};                                          ///< The minimum number of touch points required.
  uint32_t       maximumTouches{1u};                                          ///< The maximum number of touch points allowed.
  uint32_t       maximumMotionEventAge{std::numeric_limits<uint32_t>::max()}; ///< Motion events older than this (ms) are skipped.
  AngleContainer angles;                                                      ///< Allowed pan angles; empty means any direction.
};

} // namespace Internal

} // namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_PAN_GESTURE_PROFILE_H
