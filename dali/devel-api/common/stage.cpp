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

// CLASS HEADER
#include <dali/devel-api/common/stage.h>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace
{
// The Stage handle no longer wraps an internal object; these helpers reach the
// current Core (owned by the thread-local storage) directly. Returns nullptr
// when the Core is not installed (e.g. during or after shutdown).
Internal::Core* GetCurrentCore()
{
  Internal::ThreadLocalStorage* tls = Internal::ThreadLocalStorage::GetInternal();
  if(tls)
  {
    // Core is the concrete EventThreadServices held by the thread-local storage.
    return &static_cast<Internal::Core&>(tls->GetEventThreadServices());
  }
  return nullptr;
}
} // unnamed namespace

Stage::Stage() = default;

Stage::~Stage() = default;

Stage::Stage(const Stage& handle) = default;

Stage& Stage::operator=(const Stage& rhs) = default;

Stage::Stage(Stage&& handle) noexcept = default;

Stage& Stage::operator=(Stage&& rhs) noexcept = default;

Stage Stage::GetCurrent()
{
  // The Stage implementation has been removed; GetDpi() and KeepRendering()
  // operate on the current Core directly, so an empty handle is sufficient.
  return Stage();
}

Vector2 Stage::GetDpi() const
{
  Internal::Core* core = GetCurrentCore();
  if(core)
  {
    const Internal::Core::SceneContainer& scenes = core->GetScenes();
    if(!scenes.empty())
    {
      return scenes[0]->GetDpi();
    }
  }
  return Vector2::ZERO;
}

void Stage::KeepRendering(float durationSeconds)
{
  Internal::Core* core = GetCurrentCore();
  if(core)
  {
    core->KeepRendering(durationSeconds);
  }
}

} // namespace Dali
