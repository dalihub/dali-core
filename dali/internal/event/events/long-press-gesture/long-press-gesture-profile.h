#ifndef DALI_INTERNAL_LONG_PRESS_GESTURE_PROFILE_H
#define DALI_INTERNAL_LONG_PRESS_GESTURE_PROFILE_H

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
 * @brief One complete set of long press recognition options: the state behind
 * Dali::LongPressGestureDetector::Options and the default/per-device profiles kept by the detector.
 */
struct LongPressGestureProfile
{
  /**
   * @brief Checks whether the profile is valid for registration: positive touch counts, minimum <= maximum.
   * @return true if valid
   */
  bool IsValid() const
  {
    return minimumTouches > 0u && maximumTouches > 0u && minimumTouches <= maximumTouches;
  }

  uint32_t minimumTouches{1u}; ///< The minimum number of touch points required.
  uint32_t maximumTouches{1u}; ///< The maximum number of touch points allowed.
};

} // namespace Internal

} // namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_LONG_PRESS_GESTURE_PROFILE_H
