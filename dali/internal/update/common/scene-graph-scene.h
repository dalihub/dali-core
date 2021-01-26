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
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/public-api/common/vector-wrapper.h>

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
   * Gets the context holding the GL state of rendering for the scene
   * @return the context
   */
  Context* GetContext();

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
  void AddFrameRenderedCallback( CallbackBase* callback, int32_t frameId );

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
  void AddFramePresentedCallback( CallbackBase* callback, int32_t frameId );

  /**
   * @brief Gets the callback list that is called when the frame rendering is done by the graphics driver.
   *
   * @param[out] callbacks The callback list
   */
  void GetFrameRenderedCallback( Dali::Integration::Scene::FrameCallbackContainer& callbacks );

  /**
   * @brief Gets the callback list that is called when the frame is displayed on the display.
   *
   * @param[out] callbacks The callback list
   */
  void GetFramePresentedCallback( Dali::Integration::Scene::FrameCallbackContainer& callbacks );

  /**
   * @brief Sets whether rendering should be skipped or not.
   * @param[in] skip true if rendering should be skipped.
   */
  void SetSkipRendering( bool skip );

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
  void SetSurfaceRect( const Rect<int32_t>& rect );

  /**
   * Get the surface rectangle.
   *
   * @return the current surface rectangle
   */
  const Rect<int32_t>& GetSurfaceRect() const;

  /**
   * Set the surface orientation when surface is rotated.
   *
   * @param[in] scene The rotated scene.
   * @param[in] orientation The orientation value representing the surface.
   */
  void SetSurfaceOrientation( int32_t orientation );

  /**
   * Get the surface orientation.
   *
   * @return the current surface orientation
   */
  int32_t GetSurfaceOrientation() const;

  /**
   * Query wheter the surface rect is changed or not.
   * @return true if the surface rect is changed.
   */
  bool IsSurfaceRectChanged();

private:

  Context*                    mContext;   ///< The context holding the GL state of rendering for the scene, not owned

  // Render instructions describe what should be rendered during RenderManager::RenderScene()
  // Update manager updates instructions for the next frame while we render the current one

  RenderInstructionContainer  mInstructions;   ///< Render instructions for the scene

  Dali::Integration::Scene::FrameCallbackContainer mFrameRenderedCallbacks;   ///< Frame rendered callbacks
  Dali::Integration::Scene::FrameCallbackContainer mFramePresentedCallbacks;  ///< Frame presented callbacks

  bool                        mSkipRendering;  ///< A flag to skip rendering

  Rect<int32_t>               mSurfaceRect;           ///< The rectangle of surface which is related ot this scene.
  int32_t                     mSurfaceOrientation;    ///< The orientation of surface which is related of this scene
  bool                        mSurfaceRectChanged;    ///< The flag of surface's rectangle is changed when is resized, moved or rotated.
};

/// Messages
inline void AddFrameRenderedCallbackMessage( EventThreadServices& eventThreadServices, const Scene& scene, const CallbackBase* callback, int32_t frameId )
{
  using LocalType = MessageValue2<Scene, CallbackBase*, int32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &scene, &Scene::AddFrameRenderedCallback, const_cast< CallbackBase* >( callback ), frameId );
}

inline void AddFramePresentedCallbackMessage( EventThreadServices& eventThreadServices, const Scene& scene, const CallbackBase* callback, int32_t frameId )
{
  using LocalType = MessageValue2<Scene, CallbackBase*, int32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &scene, &Scene::AddFramePresentedCallback, const_cast< CallbackBase* >( callback ), frameId );
}

inline void SetSurfaceRectMessage( EventThreadServices& eventThreadServices, const Scene& scene, const Rect<int32_t>& rect )
{
  using LocalType = MessageValue1<Scene, Rect<int32_t> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &scene, &Scene::SetSurfaceRect, rect );
}

inline void SetSurfaceOrientationMessage( EventThreadServices& eventThreadServices, const Scene& scene, int32_t orientation )
{
  using LocalType = MessageValue1<Scene, int32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &scene, &Scene::SetSurfaceOrientation, orientation );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_SCENE_GRAPH_SCENE_H
