#ifndef DALI_INTERNAL_ANIMATION_PLAYLIST_DECLARATIONS_H
#define DALI_INTERNAL_ANIMATION_PLAYLIST_DECLARATIONS_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/owner-pointer.h>

namespace Dali
{
namespace Internal
{
class AnimationPlaylist;

using AnimationPlaylistOwner = OwnerPointer<AnimationPlaylist>;

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_ANIMATION_PLAYLIST_DECLARATIONS_H
