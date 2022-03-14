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

#include "drawable-actor-impl.h"
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali::Internal
{
DrawableActorPtr DrawableActor::New(RenderCallback* renderCallback)
{
  return DrawableActorPtr(new DrawableActor(*CreateNode(), renderCallback));
}

DrawableActor::DrawableActor(const Internal::SceneGraph::Node& node, RenderCallback* renderCallback)
: Actor(Actor::BASIC, node)
{
  auto rendererImpl = Internal::Renderer::New();

  rendererImpl->SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);

  mRenderer = Dali::Renderer(rendererImpl.Get());
  mRenderer.SetRenderCallback(renderCallback);
  AddRenderer(*rendererImpl.Get());
}

} // namespace Dali::Internal