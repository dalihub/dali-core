#ifndef DALI_KEY_FRAMES_DEVEL_H
#define DALI_KEY_FRAMES_DEVEL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/key-frames.h>

namespace Dali
{
namespace DevelKeyFrames
{
using ValueConvertor = Property::Value (*)(const Property::Value& input);

/**
 * @brief Get the number of key frames.
 * @param[in] keyFrames The KeyFrames object to perform this operation on
 * @return Total number of key frames
 */
DALI_CORE_API std::size_t GetKeyFrameCount(KeyFrames keyFrames);

/**
 * Get a key frame.
 * @param[in] keyFrames The KeyFrames object to perform this operation on
 * @param[in] index The index of the key frame to fetch
 * @param[out] time The progress of the given key frame
 * @param[out] value The value of the given key frame
 */
DALI_CORE_API void GetKeyFrame(KeyFrames keyFrames, std::size_t index, float& time, Property::Value& value);

/**
 * Set a key frame.
 * @param[in] keyFrames The KeyFrames object to perform this operation on
 * @param[in] index The index of the key frame to set
 * @param[in] value The value of the given key frame
 */
DALI_CORE_API void SetKeyFrameValue(KeyFrames keyFrames, std::size_t index, const Property::Value& value);

} // namespace DevelKeyFrames

} // namespace Dali

#endif // DALI_KEY_FRAMES_DEVEL_H
