#ifndef DALI_DRAWABLE_ACTOR_H
#define DALI_DRAWABLE_ACTOR_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/signals/render-callback.h>

// EXTERNAL INCLUDES
#include <memory>

namespace Dali
{
namespace Internal
{
class DrawableActor;
}

/**
 * @class DrawableActor
 *
 * DrawableActor is a special type of Actor that allows attaching
 * DrawableCallback in order to pass custom native graphics API
 * calls (like GLES).
 *
 * @SINCE_2_1.15
 */
class DALI_CORE_API DrawableActor : public Actor
{
public:
  /**
   * @brief Creates new DrawableActor instance
   *
   * @SINCE_2_1.15
   * @param[in] callback Reference to the valid DrawableCallback object
   * @return Handle to the new DrawableActor
   */
  static DrawableActor New(RenderCallback& callback);

  /**
   * @brief Constructor
   *
   * @SINCE_2_1.15
   */
  DrawableActor() = default;

private:
  explicit DrawableActor(Internal::DrawableActor* internal);
};
} // namespace Dali

#endif // DALI_DRAWABLE_ACTOR_H
