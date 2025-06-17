#ifndef DALI_INTERNAL_ACTORS_ACTOR_RENDERER_CONTAINER_H
#define DALI_INTERNAL_ACTORS_ACTOR_RENDERER_CONTAINER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/internal/event/common/event-thread-services-holder.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali::Internal
{
using RendererPtr = IntrusivePtr<Renderer>;
class Renderer;

namespace SceneGraph
{
class Node;
}

/**
 * Class to contain an actor's renderers.
 * Enables actor to set the blending for all renderers at once.
 */
class RendererContainer : public EventThreadServicesHolder
{
public:
  /**
   * Constructor.
   *
   * @param[in] eventThreadServices - thread services for sending messages
   * @param[in] isCache - whether the container is for cache renderers(drawing offscreen rendering results)
   */
  RendererContainer(EventThreadServices& eventThreadServices, bool isCache);

  /**
   * Add a renderer to this actor & actor's node, with blending applied if necessary.
   * @param[in] node The node to add this renderer's scene graph object to
   * @param[in] renderer the renderer to add
   * @param[in] blending If blending equation should be applied to this renderer
   * @param[in] blendEquation The current blending equation
   */
  uint32_t Add(const SceneGraph::Node& node, Renderer& renderer, bool blending, DevelBlendEquation::Type blendEquation);

  /**
   * Remove the renderer, if present
   * @param[in] node The node to remove this renderer's scene graph object from
   * @param[in] renderer The renderer to remove
   */
  void Remove(const SceneGraph::Node& node, Renderer& renderer);

  /**
   * Remove the renderer at given index
   * @param[in] node The node to remove this renderer's scene graph object from
   * @param[in] index The index of the renderer to remove
   */
  void Remove(const SceneGraph::Node& node, uint32_t index);

  /**
   * Remove all renderers
   * @param[in] node The node to remove these renderer's scene graph object from
   */
  void RemoveAll(const SceneGraph::Node& node);

  /**
   * Return the number of renderers
   * @return the number of renderers
   */
  uint32_t GetCount() const;

  /**
   * @return true if there aren't any renderers
   */
  bool IsEmpty() const
  {
    return mRenderers.empty();
  }

  /**
   * Get the renderer at the given index, if in range.
   * @param[in] index The index of the renderer to return
   * @return A renderer at the given index, or nullptr if out of range
   */
  RendererPtr GetRendererAt(uint32_t index) const;

  /**
   * Set the blending equation for all renderers
   */
  void SetBlending(DevelBlendEquation::Type blendEquation);

private:
  std::vector<RendererPtr> mRenderers; ///< The contained renderers

  bool mIsCache : 1; /// The container is for cache renderers
};

} // namespace Dali::Internal

#endif //DALI_INTERNAL_ACTORS_ACTOR_RENDERER_CONTAINER_H
