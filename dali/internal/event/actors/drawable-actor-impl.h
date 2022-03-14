#ifndef DALI_INTERNAL_DRAWABLE_ACTOR_H
#define DALI_INTERNAL_DRAWABLE_ACTOR_H

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

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/public-api/rendering/renderer.h>

namespace Dali::Internal
{
namespace SceneGraph
{
class Node;
}

class DrawableActor;
using DrawableActorPtr = IntrusivePtr<DrawableActor>;

class DrawableActor : public Actor
{
public:
  /**
   * @brief Constructor of DrawableActor
   *
   * @param[in] node the scenegraph Node object
   * @param[in] renderCallback the pointer to the CallbackBase object
   */
  explicit DrawableActor(const Internal::SceneGraph::Node& node, RenderCallback* renderCallback);

  /**
   * @brief Creates new instance of the DrawableActor implementation
   *
   * @param[in] renderCallback Pointer to a valid CallbackBase object
   * @return managed pointer to the DrawableActor implementation
   */
  static DrawableActorPtr New(RenderCallback* renderCallback);

private:
  Dali::Renderer mRenderer; //< Drawable renderer with callback attached
};

} // namespace Dali::Internal

#endif // DALI_INTERNAL_DRAWABLE_ACTOR_H
