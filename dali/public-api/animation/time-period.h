#ifndef __DALI_TIME_PERIOD_H__
#define __DALI_TIME_PERIOD_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief A value-type representing a period of time within an animation.
 */
struct DALI_IMPORT_API TimePeriod
{
  /**
   * @brief Create a time period.
   *
   * @param [in] durationSeconds The duration of the time period in seconds.
   */
  TimePeriod(float durationSeconds);

  /**
   * @brief Create a time period.
   *
   * @param [in] delaySeconds A delay before the time period in seconds.
   * @param [in] durationSeconds The duration of the time period in seconds.
   */
  TimePeriod(float delaySeconds, float durationSeconds);

  /**
   * @brief Non-virtual destructor; TimePeriod is not intended as a base class.
   */
  ~TimePeriod();

  float delaySeconds;    ///< A delay before the time period in seconds
  float durationSeconds; ///< The duration of the time period in seconds
};

} // namespace Dali

#endif // __DALI_TIME_PERIOD_H__
