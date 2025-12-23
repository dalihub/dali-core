#ifndef DALI_INTERNAL_SCENE_GRAPH_SCENE_H
#define DALI_INTERNAL_SCENE_GRAPH_SCENE_H

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

// EXTERNAL INCLUDE
#include <cstddef>
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
#include <dali/devel-api/common/map-wrapper.h>
#else
#include <unordered_map>
#endif

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/scene.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-target-graphics-objects.h>
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
  DirtyRectKey(const Node* node, Render::RendererKey renderer)
  : node(node),
    renderer(renderer)
  {
  }

  DirtyRectKey() = default;

  bool operator==(const DirtyRectKey& rhs) const
  {
    return node == rhs.node && renderer == rhs.renderer;
  }

#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  struct DirtyRectKeyCompareLess
  {
    bool operator()(const DirtyRectKey& lhs, const DirtyRectKey& rhs) const noexcept
    {
      return (lhs.node < rhs.node) || ((lhs.node == rhs.node) && lhs.renderer.Value() < rhs.renderer.Value());
    }
  };
#else
  struct DirtyRectKeyHash
  {
    // Reference by : https://stackoverflow.com/a/21062236
    std::size_t operator()(DirtyRectKey const& key) const noexcept
    {
      constexpr std::size_t nodeShift = Dali::Log<1 + sizeof(Node)>::value;

#if defined(__LP64__)
      constexpr std::size_t rendererShift = 0;
#else
      constexpr std::size_t rendererShift = Dali::Log<1 + sizeof(Render::Renderer)>::value;
#endif

      constexpr std::size_t zitterShift = sizeof(std::size_t) * 4; // zitter shift to avoid hash collision

      return ((reinterpret_cast<std::size_t>(key.node) >> nodeShift) << zitterShift) ^
             (key.renderer.Value() >> rendererShift);
    }
  };
#endif

  const Node*         node{nullptr};
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

/**
 * SceneGraph::Scene is a core object representing the window and it's scene-graph. It's used in both
 * UpdateManager and RenderManager, so has to live in both worlds, and has to know how to send
 * messages to itself to be run in RenderManager's Queue.
 *
 * Initialization is tricky - the Scene's render target has to be setup in RenderManager, so needs
 * mRenderTargetCreateInfo to be defined before Initialize is called in RenderManage's Queue.
 * Also, a scene's background color may also get set before Initialize is called.
 */
class Scene : public RenderTargetGraphicsObjects
{
public:
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
  using ItemsDirtyRectsContainer = std::map<DirtyRectKey, DirtyRectValue, DirtyRectKey::DirtyRectKeyCompareLess>;
#else
  using ItemsDirtyRectsContainer = std::unordered_map<DirtyRectKey, DirtyRectValue, DirtyRectKey::DirtyRectKeyHash>;
#endif

  /**
   * Constructor
   */
  Scene();

  /**
   * Destructor
   */
  virtual ~Scene();

  /**
   * Set the render target of the surface
   * Called from UpdateManager Queue, before Initialize
   *
   * @param[in] renderTarget The render target.
   */
  void SetSurfaceRenderTargetCreateInfo(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo);

  /**
   * Creates a scene object
   * Note, this is run inside RenderManager Queue, not UpdateManagerQueue.
   * So, other APIs must also run inside RenderManager Queue if they need
   * to run afterwards.
   *
   * @param[in] graphicsController The graphics controller
   * @param[in] depthBufferAvailable True if there is a depth buffer
   * @param[in] stencilBufferAvailable True if there is a stencil buffer
   */
  void Initialize(Graphics::Controller& graphicsController, Integration::DepthBufferAvailable depthBufferAvailable, Integration::StencilBufferAvailable stencilBufferAvailable);

  /**
   * @brief The graphics context is being shutdown. Clean down any outstanding graphics resources.
   */
  void ContextDestroyed();

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
   * Query how many times the surface rect changed.
   * @note It will reset surface rect changed count.
   * @return The count of the surface rect changed.
   */
  uint32_t GetSurfaceRectChangedCount();

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
   * @brief Set the clear color for the scene's render passes
   *
   * @note Normally, the render instruction's clear color is used (from the render task).
   *
   * @param[in] color The color to clear for the render passes
   */
  void SetClearColor(const Vector4& color);

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
   * @brief Sets whether the scene will update partial area or full area.
   *
   * @param[in] enabled True if the scene should update partial area
   * @note This doesn't change the global value which is set by the environment variable.
   * This works when partial update is enabled by the environment variable. If the partial update is disabled by the environment variable, it changes nothing.
   */
  void SetPartialUpdateEnabled(bool enabled);

  /**
   * @brief Queries whether the scene will update partial area.
   *
   * @return True if the scene should update partial area
   */
  bool IsPartialUpdateEnabled() const;

  /**
   * @brief Query if the scene needs full update
   * @note This will reset full-update flag
   * @return True if the scene needs full update
   */
  bool IsFullUpdateNeeded()
  {
    bool needFullUpdate = mNeedFullUpdate;
    mNeedFullUpdate     = false;
    return needFullUpdate;
  }

  void SetHasRenderInstructionToScene(bool renderInstructionExist)
  {
    mHasRenderInstructionToScene = renderInstructionExist;
  }

  bool HasRenderInstructionToScene() const
  {
    return mHasRenderInstructionToScene;
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

  /**
   * @brief Clear ItemsDirtyRects.
   * @note We will full-update next frame.
   */
  void ClearItemsDirtyRects();

public: // From RenderTargetGraphicsObjects
  /**
   * Get the render target created for the scene
   *
   * @return the render target
   */
  [[nodiscard]] Graphics::RenderTarget* GetSurfaceRenderTarget() const
  {
    return RenderTargetGraphicsObjects::GetGraphicsRenderTarget();
  }

  /**
   * Remove the render target of the surface
   */
  void RemoveSurfaceRenderTarget()
  {
    RenderTargetGraphicsObjects::NotifyRenderTargetDestroyed();
    mRenderTarget.reset();
  }

private:
  // Render instructions describe what should be rendered during RenderManager::RenderScene()
  // Update manager updates instructions for the next frame while we render the current one

  RenderInstructionContainer mInstructions; ///< Render instructions for the scene

  Dali::Integration::Scene::FrameCallbackContainer mFrameRenderedCallbacks;  ///< Frame rendered callbacks
  Dali::Integration::Scene::FrameCallbackContainer mFramePresentedCallbacks; ///< Frame presented callbacks

  Rect<int32_t> mSurfaceRect;        ///< The rectangle of surface which is related ot this scene.
  int32_t       mSurfaceOrientation; ///< The orientation of surface which is related of this scene
  int32_t       mScreenOrientation;  ///< The orientation of screen

  uint32_t mSurfaceRectChangedCount; ///< The numbero of surface's rectangle is changed when is resized or moved.

  float mKeepRenderingSeconds{0.0f}; ///< Time to keep rendering

  bool mRotationCompletedAcknowledgement : 1; ///< The flag of sending the acknowledgement to complete window rotation.
  bool mSkipRendering : 1;                    ///< A flag to skip rendering
  bool mNeedFullUpdate : 1;                   ///< A flag to update full area
  bool mPartialUpdateEnabled : 1;             ///< True if the partial update is enabled
  bool mHasRenderInstructionToScene : 1;      ///< True if has render instruction to the scene. Update at PreRender time.

  // Render target, command buffer and render passes

  Graphics::RenderTargetCreateInfo mRenderTargetCreateInfo; // Passed in by message before 2nd stage Initialization happens.

  SceneGraph::Layer* mRoot{nullptr}; ///< Root node

  ItemsDirtyRectsContainer mItemsDirtyRects{}; ///< Dirty rect list
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

inline void KeepRenderingMessage(EventThreadServices& eventThreadServices, const Scene& scene, float durationSeconds)
{
  using LocalType = MessageValue1<Scene, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::KeepRendering, durationSeconds);
}

inline void SetPartialUpdateEnabledMessage(EventThreadServices& eventThreadServices, const Scene& scene, bool enabled)
{
  using LocalType = MessageValue1<Scene, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetPartialUpdateEnabled, enabled);
}

inline void SetClearColorMessage(EventThreadServices& eventThreadServices, const Scene& scene, const Vector4& color)
{
  using LocalType = MessageValue1<Scene, Vector4>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&scene, &Scene::SetClearColor, color);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_SCENE_H
