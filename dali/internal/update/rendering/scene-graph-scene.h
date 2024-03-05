#ifndef DALI_INTERNAL_SCENE_GRAPH_SCENE_H
#define DALI_INTERNAL_SCENE_GRAPH_SCENE_H

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
 */

// EXTERNAL INCLUDE
#include <cstddef>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/compile-time-math.h>

#include <dali/integration-api/core.h>
#include <dali/integration-api/scene.h>

#include <dali/graphics-api/graphics-controller.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/rendering/render-instruction-container.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class RenderInstructionContainer;
class Node;

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
   * @param[in] graphicsController The graphics controller
   * @param[in] depthBufferAvailable True if there is a depth buffer
   * @param[in] stencilBufferAvailable True if there is a stencil buffer
   */
  void Initialize(Graphics::Controller& graphicsController, Integration::DepthBufferAvailable depthBufferAvailable, Integration::StencilBufferAvailable stencilBufferAvailable);

  /**
   * Gets the render instructions for the scene
   * @return the render instructions
   */
  RenderInstructionContainer& GetRenderInstructions();

  /**
   * Set the surface rectangle when surface is resized.
   *
   * @param[in] scene The resized scene.
   * @param[in] rect The retangle representing the surface.
   */
  void SetSurfaceRect(const Rect<int32_t>& rect);

  /**
   * Get the surface rectangle.
   *
   * @return the current surface rectangle
   */
  [[nodiscard]] const Rect<int32_t>& GetSurfaceRect() const;

  /**
   * Set the render target of the surface
   *
   * @param[in] renderTarget The render target.
   */
  void SetSurfaceRenderTargetCreateInfo(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo);

  /**
   * @brief Keep rendering for at least the given amount of time.
   *
   * @param[in] durationSeconds Time to keep rendering, 0 means render at least one more frame
   */
  void KeepRendering(float durationSeconds);

  /**
   * @brief Check whether rendering should keep going.
   *
   * @param[in] elapsedSeconds The time in seconds since the previous update.
   * @return True if rendering should keep going.
   */
  bool KeepRenderingCheck(float elapsedSeconds);

  /**
   * Get the render target created for the scene
   *
   * @return the render target
   */
  [[nodiscard]] Graphics::RenderTarget* GetSurfaceRenderTarget() const
  {
    return mRenderTarget.get();
  }

  /**
   * Remove the render target of the surface
   */
  void RemoveSurfaceRenderTarget()
  {
    mRenderTarget.reset();
  }

  /**
   * Get the graphics render pass created for the scene
   *
   * @return the graphics render pass
   */
  [[nodiscard]] Graphics::RenderPass* GetGraphicsRenderPass(Graphics::AttachmentLoadOp loadOp, Graphics::AttachmentStoreOp storeOp) const
  {
    if(loadOp == Graphics::AttachmentLoadOp::CLEAR)
    {
      return mRenderPass.get();
    }
    else
    {
      return mRenderPassNoClear.get();
    }
  }

  /**
   * Get an initialized array of clear values which then can be modified and accessed to BeginRenderPass() command.
   *
   * @return the array of clear values
   */
  [[nodiscard]] auto& GetGraphicsRenderPassClearValues()
  {
    return mClearValues;
  }

  /**
   * @brief Set a root of the Scene
   *
   * @param layer The root layer
   */
  void SetRoot(SceneGraph::Layer* layer)
  {
    mRoot = layer;
  }

  /**
   * @brief Get a root of the Scene
   *
   * @return The root layer
   */
  [[nodiscard]] SceneGraph::Layer* GetRoot() const
  {
    return mRoot;
  }

  void GetAvailableBuffers(Integration::DepthBufferAvailable& depthBufferAvailable,
                            Integration::StencilBufferAvailable& stencilBufferAvailable);

private:
  // Render instructions describe what should be rendered during RenderManager::RenderScene()
  // Update manager updates instructions for the next frame while we render the current one

  RenderInstructionContainer mInstructions; ///< Render instructions for the scene

  Graphics::Controller* mGraphicsController{nullptr}; ///< Graphics controller

  Rect<int32_t> mSurfaceRect;        ///< The rectangle of surface which is related ot this scene.

  float mKeepRenderingSeconds{0.0f}; ///< Time to keep rendering

  // Render pass and render target

  Graphics::RenderTargetCreateInfo mRenderTargetCreateInfo; // Passed in by message before 2nd stage Initialization happens.

  /**
   * Render pass is created on fly depending on Load and Store operations
   * The default render pass (most likely to be used) is the load = CLEAR
   * and store = STORE for color attachment.
   */
  Graphics::UniquePtr<Graphics::RenderPass>   mRenderPass{nullptr};        ///< The render pass created to render the surface
  Graphics::UniquePtr<Graphics::RenderPass>   mRenderPassNoClear{nullptr}; ///< The render pass created to render the surface without clearing color
  Graphics::UniquePtr<Graphics::RenderTarget> mRenderTarget{nullptr};      ///< This is created in Update/Render thread when surface is created/resized/replaced

  SceneGraph::Layer* mRoot{nullptr}; ///< Root node

  std::vector<Graphics::ClearValue> mClearValues{};     ///< Clear colors
  Integration::DepthBufferAvailable mDepthBufferAvailable{true};
  Integration::StencilBufferAvailable mStencilBufferAvailable{true};
};

inline void SetSurfaceRectMessage(EventThreadServices& eventThreadServices, const Scene& scene, const Rect<int32_t>& rect)
{
  using LocalType = MessageValue1<Scene, Rect<int32_t> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetSurfaceRect, rect);
}


inline void SetSurfaceRenderTargetCreateInfoMessage(EventThreadServices& eventThreadServices, const Scene& scene, const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo)
{
  using LocalType = MessageValue1<Scene, Graphics::RenderTargetCreateInfo>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetSurfaceRenderTargetCreateInfo, renderTargetCreateInfo);
}

inline void KeepRenderingMessage(EventThreadServices& eventThreadServices, const Scene& scene, float durationSeconds)
{
  using LocalType = MessageValue1<Scene, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::KeepRendering, durationSeconds);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SCENE_H
