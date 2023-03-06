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
#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/scene.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/renderers/render-renderer.h> // RendererKey
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/compile-time-math.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class RenderInstructionContainer;
class Node;

struct DirtyRectKey
{
  DirtyRectKey(Node* node, Render::RendererKey renderer)
  : node(node),
    renderer(renderer)
  {
  }

  DirtyRectKey() = default;

  bool operator==(const DirtyRectKey& rhs) const
  {
    return node == rhs.node && renderer == rhs.renderer;
  }

  struct DirtyRectKeyHash
  {
    // Reference by : https://stackoverflow.com/a/21062236
    std::size_t operator()(DirtyRectKey const& key) const noexcept
    {
      constexpr std::size_t nodeShift     = Dali::Log<1 + sizeof(Node)>::value;
      constexpr std::size_t rendererShift = Dali::Log<1 + sizeof(Render::Renderer)>::value;

      constexpr std::size_t zitterShift = sizeof(std::size_t) * 4; // zitter shift to avoid hash collision

      return ((reinterpret_cast<std::size_t>(key.node) >> nodeShift) << zitterShift) ^
             (key.renderer.Value() >> rendererShift);
    }
  };

  Node*               node{nullptr};
  Render::RendererKey renderer{};
};

struct DirtyRectValue
{
  DirtyRectValue(Rect<int>& rect)
  : rect(rect),
    visited(true)
  {
  }

  DirtyRectValue() = default;

  Rect<int32_t> rect{};
  bool          visited{true};
};

class Scene
{
public:
  using ItemsDirtyRectsContainer = std::unordered_map<DirtyRectKey, DirtyRectValue, DirtyRectKey::DirtyRectKeyHash>;

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
   * @brief Adds a callback that is called when the frame rendering is done by the graphics driver.
   *
   * @param[in] callback The function to call
   * @param[in] frameId The Id to specify the frame. It will be passed when the callback is called.
   *
   * @note A callback of the following type may be used:
   * @code
   *   void MyFunction( int frameId );
   * @endcode
   * This callback will be deleted once it is called.
   *
   * @note Ownership of the callback is passed onto this class.
   */
  void AddFrameRenderedCallback(CallbackBase* callback, int32_t frameId);

  /**
   * @brief Adds a callback that is called when the frame is displayed on the display.
   *
   * @param[in] callback The function to call
   * @param[in] frameId The Id to specify the frame. It will be passed when the callback is called.
   *
   * @note A callback of the following type may be used:
   * @code
   *   void MyFunction( int frameId );
   * @endcode
   * This callback will be deleted once it is called.
   *
   * @note Ownership of the callback is passed onto this class.
   */
  void AddFramePresentedCallback(CallbackBase* callback, int32_t frameId);

  /**
   * @brief Gets the callback list that is called when the frame rendering is done by the graphics driver.
   *
   * @param[out] callbacks The callback list
   */
  void GetFrameRenderedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks);

  /**
   * @brief Gets the callback list that is called when the frame is displayed on the display.
   *
   * @param[out] callbacks The callback list
   */
  void GetFramePresentedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks);

  /**
   * @brief Sets whether rendering should be skipped or not.
   * @param[in] skip true if rendering should be skipped.
   */
  void SetSkipRendering(bool skip);

  /**
   * @brief Query whether rendering should be skipped or not.
   * @return true if rendering should be skipped, false otherwise.
   */
  bool IsRenderingSkipped() const;

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
  const Rect<int32_t>& GetSurfaceRect() const;

  /**
   * Set the surface orientations when surface or screen is rotated.
   *
   * @param[in] windowOrientation The orientations value representing surface.
   * @param[in] screenOrienation The orientations value representing screen.
   */
  void SetSurfaceOrientations(int32_t windowOrientation, int32_t screenOrienation);

  /**
   * Get the surface orientation.
   *
   * @return the current surface orientation
   */
  int32_t GetSurfaceOrientation() const;

  /**
   * Get the screen orientation.
   *
   * @return the current screen orientation
   */
  int32_t GetScreenOrientation() const;

  /**
   * Query wheter the surface rect is changed or not.
   * @return true if the surface rect is changed.
   */
  bool IsSurfaceRectChanged();

  /**
   * @brief Set the internal flag to acknowledge surface rotation.
   */
  void SetRotationCompletedAcknowledgement();

  /**
   * @brief Query wheter is set to acknowledge for completing surface rotation.
   * @return true it should be acknowledged.
   */
  bool IsRotationCompletedAcknowledgementSet();

  /**
   * Set the render target of the surface
   *
   * @param[in] renderTarget The render target.
   */
  void SetSurfaceRenderTargetCreateInfo(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo);

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
  SceneGraph::Layer* GetRoot() const
  {
    return mRoot;
  }

  /**
   * @brief Get ItemsDirtyRects
   *
   * @return the ItemsDirtyRects
   */
  ItemsDirtyRectsContainer& GetItemsDirtyRects();

private:
  // Render instructions describe what should be rendered during RenderManager::RenderScene()
  // Update manager updates instructions for the next frame while we render the current one

  RenderInstructionContainer mInstructions; ///< Render instructions for the scene

  Graphics::Controller* mGraphicsController{nullptr}; ///< Graphics controller

  Dali::Integration::Scene::FrameCallbackContainer mFrameRenderedCallbacks;  ///< Frame rendered callbacks
  Dali::Integration::Scene::FrameCallbackContainer mFramePresentedCallbacks; ///< Frame presented callbacks

  bool mSkipRendering; ///< A flag to skip rendering

  Rect<int32_t> mSurfaceRect;                      ///< The rectangle of surface which is related ot this scene.
  int32_t       mSurfaceOrientation;               ///< The orientation of surface which is related of this scene
  int32_t       mScreenOrientation;                ///< The orientation of screen
  bool          mSurfaceRectChanged;               ///< The flag of surface's rectangle is changed when is resized or moved.
  bool          mRotationCompletedAcknowledgement; ///< The flag of sending the acknowledgement to complete window rotation.

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
  ItemsDirtyRectsContainer          mItemsDirtyRects{}; ///< Dirty rect list
};

/// Messages
inline void AddFrameRenderedCallbackMessage(EventThreadServices& eventThreadServices, const Scene& scene, const CallbackBase* callback, int32_t frameId)
{
  using LocalType = MessageValue2<Scene, CallbackBase*, int32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::AddFrameRenderedCallback, const_cast<CallbackBase*>(callback), frameId);
}

inline void AddFramePresentedCallbackMessage(EventThreadServices& eventThreadServices, const Scene& scene, const CallbackBase* callback, int32_t frameId)
{
  using LocalType = MessageValue2<Scene, CallbackBase*, int32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::AddFramePresentedCallback, const_cast<CallbackBase*>(callback), frameId);
}

inline void SetSurfaceRectMessage(EventThreadServices& eventThreadServices, const Scene& scene, const Rect<int32_t>& rect)
{
  using LocalType = MessageValue1<Scene, Rect<int32_t> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetSurfaceRect, rect);
}

inline void SetSurfaceOrientationsMessage(EventThreadServices& eventThreadServices, const Scene& scene, const int32_t windowOrientation, const int32_t screenOrientation)
{
  using LocalType = MessageValue2<Scene, int32_t, int32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetSurfaceOrientations, windowOrientation, screenOrientation);
}

inline void SetRotationCompletedAcknowledgementMessage(EventThreadServices& eventThreadServices, const Scene& scene)
{
  using LocalType = Message<Scene>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetRotationCompletedAcknowledgement);
}

inline void SetSurfaceRenderTargetCreateInfoMessage(EventThreadServices& eventThreadServices, const Scene& scene, const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo)
{
  using LocalType = MessageValue1<Scene, Graphics::RenderTargetCreateInfo>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetSurfaceRenderTargetCreateInfo, renderTargetCreateInfo);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SCENE_H
