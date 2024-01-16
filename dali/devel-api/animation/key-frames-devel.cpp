/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/animation/key-frames-devel.h>
#include <dali/internal/event/animation/key-frames-impl.h>

namespace Dali
{
namespace DevelKeyFrames
{
std::size_t GetKeyFrameCount(KeyFrames keyFrames)
{
  return GetImplementation(keyFrames).GetKeyFrameCount();
}

void GetKeyFrame(KeyFrames keyFrames, std::size_t index, float& time, Property::Value& value)
{
  GetImplementation(keyFrames).GetKeyFrame(index, time, value);
}

void SetKeyFrameValue(KeyFrames keyFrames, std::size_t index, const Property::Value& value)
{
  GetImplementation(keyFrames).SetKeyFrameValue(index, value);
}

bool OptimizeKeyFramesLinear(KeyFrames keyFrames)
{
  return GetImplementation(keyFrames).OptimizeKeyFramesLinear();
}

} // namespace DevelKeyFrames

} // namespace Dali
