#ifndef DALI_INTERNAL_SCENE_GRAPH_SCENE_H
#define DALI_INTERNAL_SCENE_GRAPH_SCENE_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/integration-api/scene.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/common/render-instruction-container.h>

namespace Dali
{

namespace Internal
{

class Context;

namespace SceneGraph
{
class RenderInstructionContainer;

class Scene
{
public:

  /**
   * Constructor
   * @param[in] surface The render surface
   */
  Scene();

  /**
   * Destructor
   */
  virtual ~Scene();

  /**
   * Creates a scene object in the GPU.
   * @param[in] context The GL context
   */
  void Initialize( Context& context );

  /**
   * Called by RenderManager to inform the scene that the context has been destroyed
   */
  void GlContextDestroyed();

  /**
   * Gets the context holding the GL state of rendering for the scene
   * @return the context
   */
  Context* GetContext();

  /**
   * Gets the render instructions for the scene
   * @return the render instructions
   */
  RenderInstructionContainer& GetRenderInstructions();

private:

  Context*                    mContext;   ///< The context holding the GL state of rendering for the scene

  // Render instructions describe what should be rendered during RenderManager::RenderScene()
  // Update manager updates instructions for the next frame while we render the current one

  RenderInstructionContainer  mInstructions;   ///< Render instructions for the scene
};


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_SCENE_GRAPH_SCENE_H
