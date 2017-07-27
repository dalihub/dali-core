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
#include <dali/devel-api/animation/animation-devel.h>
#include <dali/internal/event/animation/animation-impl.h>

namespace Dali
{

namespace DevelAnimation
{

void SetProgressNotification( Animation animation, float progress )
{
  GetImplementation(animation).SetProgressNotification( progress );
}

float GetProgressNotification( Animation animation )
{
  return GetImplementation(animation).GetProgressNotification();
}

Animation::AnimationSignalType& ProgressReachedSignal( Animation animation )
{
  return GetImplementation( animation ).ProgressReachedSignal();
}

void PlayAfter( Animation animation, float delaySeconds )
{
  GetImplementation( animation ).PlayAfter( delaySeconds );
}

void SetLoopingMode( Animation animation, LoopingMode loopingMode )
{
  GetImplementation( animation ).SetLoopingMode( loopingMode );
}

LoopingMode GetLoopingMode( Animation animation )
{
  return GetImplementation(animation).GetLoopingMode();
}

} // namespace DevelAnimation

} // namespace Dali
