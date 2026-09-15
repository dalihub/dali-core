#ifndef DALI_INTERNAL_TAP_GESTURE_PROFILE_H
#define DALI_INTERNAL_TAP_GESTURE_PROFILE_H

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

namespace DALI_NAMESPACE
{
namespace Internal
{
/**
 * @brief One complete set of tap recognition options: the state behind Dali::TapGestureDetector::Options
 * and the default/per-device profiles kept by the tap detector.
 */
struct TapGestureProfile
{
  /**
   * @brief Checks whether the profile is valid for registration: positive tap counts, minimum <= maximum.
   * @return true if valid
   */
  bool IsValid() const
  {
    return minimumTaps > 0u && maximumTaps > 0u && minimumTaps <= maximumTaps;
  }

  uint32_t minimumTaps{1u};            ///< The minimum number of taps required to emit the signal.
  uint32_t maximumTaps{1u};            ///< The number of taps at which the signal is emitted immediately.
  bool     receiveAllTapEvents{false}; ///< Emit every tap between minimum and maximum instead of waiting.
};

} // namespace Internal

} // namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_TAP_GESTURE_PROFILE_H
