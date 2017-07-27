#ifndef DALI_ANIMATION_DEVEL_H
#define DALI_ANIMATION_DEVEL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 * @brief Enumeration for what looping mode is in.
 */
enum LoopingMode
{
  RESTART,      ///< When the animation arrives at the end in looping mode, the animation restarts from the beginning.
  AUTO_REVERSE  ///< When the animation arrives at the end in looping mode, the animation reverses direction and runs backwards again.
};

/**
 * @brief Set progress percentage marker to trigger ProgressHasBeenReachedSignal
 *
 * @param[in] animation the animation object to perform this operation on
 * @param[in] progress the progress percentage to trigger the signal at, e.g .3 for 30%.
 */
DALI_IMPORT_API void SetProgressNotification( Animation animation, float progress );

/**
 * @brief Get progress percentage marker that has been set to trigger ProgressHasBeenReachedSignal
 *
 * @param[in] animation the animation object to perform this operation on
 * @return the percentage to trigger at eg 0.3 for 30%
 */
DALI_IMPORT_API float GetProgressNotification( Animation animation );

/**
 * @brief Connects to this signal to be notified when an Animation's animations have reached set progress.
 *
 * @return A signal object to connect with
 *
 */
DALI_IMPORT_API Animation::AnimationSignalType& ProgressReachedSignal( Animation animation );

/**
 * @brief Play the animation after a given delay time.
 *
 * The delay time is not included in the looping time.
 * When the delay time is negative value, it would treat as play immediately.
 * @param[in] animation The animation object to perform this operation on
 * @param[in] delaySeconds The delay time
 */
DALI_IMPORT_API void PlayAfter( Animation animation, float delaySeconds );

/**
 * @brief Sets the looping mode.
 *
 * Animation plays forwards and then restarts from the beginning or runs backwards again.
 * @param[in] animation The animation object to perform this operation on
 * @param[in] loopingMode The looping mode is one of RESTART and AUTO_REVERSE
 */
DALI_IMPORT_API void SetLoopingMode( Animation animation, LoopingMode loopingMode );

/**
 * @brief Gets one of the current looping mode.
 *
 * @param[in] animation The animation object to perform this operation on
 * @return The current looping mode
 */
DALI_IMPORT_API LoopingMode GetLoopingMode( Animation animation );

} // namespace DevelAnimation

} // namespace Dali

#endif // DALI_ANIMATION_DEVEL_H
