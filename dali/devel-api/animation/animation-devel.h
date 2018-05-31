#ifndef DALI_ANIMATION_DEVEL_H
#define DALI_ANIMATION_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/animation.h>

namespace Dali
{

namespace DevelAnimation
{

/**
 * @brief Set progress percentage marker to trigger ProgressHasBeenReachedSignal
 *
 * @param[in] animation the animation object to perform this operation on
 * @param[in] progress the progress percentage to trigger the signal at, e.g .3 for 30%.
 */
DALI_CORE_API void SetProgressNotification( Animation animation, float progress );

/**
 * @brief Get progress percentage marker that has been set to trigger ProgressHasBeenReachedSignal
 *
 * @param[in] animation the animation object to perform this operation on
 * @return the percentage to trigger at eg 0.3 for 30%
 */
DALI_CORE_API float GetProgressNotification( Animation animation );

/**
 * @brief Connects to this signal to be notified when an Animation's animations have reached set progress.
 *
 * @return A signal object to connect with
 *
 */
DALI_CORE_API Animation::AnimationSignalType& ProgressReachedSignal( Animation animation );

} // namespace DevelAnimation

} // namespace Dali

#endif // DALI_ANIMATION_DEVEL_H
