#ifndef __DALI_INTERNAL_SYSTEM_OVERLAY_H__
#define __DALI_INTERNAL_SYSTEM_OVERLAY_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/system-overlay.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>

namespace Dali
{

namespace Internal
{

class LayerList;
class RenderTaskList;
class EventThreadServices;

/**
 * Implementation of Dali::Integration::SystemOverlay
 */
class SystemOverlay : public RenderTaskDefaults
{
public:

  /**
   * Create the SystemOverlay; This should only be done once for each Dali core.
   * @param[in] eventThreadServices to send messaged to scene graph.
   * @return The newly allocated SystemOverlay.
   */
  static SystemOverlay* New( EventThreadServices& eventThreadServices );

  /**
   * Non-virtual destructor; not intended as a base class.
   */
  ~SystemOverlay();

  /**
   * @copydoc Dali::Integration::SystemOverlay::Add()
   */
  void Add( Actor& actor );

  /**
   * @copydoc Dali::Integration::SystemOverlay::Remove()
   */
  void Remove( Actor& actor );

  /**
   * @copydoc Dali::Integration::SystemOverlay::GetOverlayRenderTasks()
   */
  RenderTaskList& GetOverlayRenderTasks();

  /**
   * Forwarded from Stage::SetSize().
   * @param[in] width  The new width.
   * @param[in] height The new height.
   */
  void SetSize( float width, float height );

  /**
   * Retrieve the ordered list of system layers.
   * @return The layer-list.
   */
  LayerList& GetLayerList();

  /**
   * From RenderTaskDefaults.
   * Retrieve the default SystemOverlay root actor.
   * @note This is different to the root actor provided by Dali::Stage.
   */
  virtual Actor& GetDefaultRootActor();

  /**
   * From RenderTaskDefaults.
   * Retrieve the default SystemOverlay camera actor.
   * @note This is different to the default camera actor provided by Dali::Stage.
   */
  virtual CameraActor& GetDefaultCameraActor();

private:

  /**
   * Protected constructor; see also SystemOverlay::New().
   * @param[in] eventThreadServices to send messaged to the scene graph.
   */
  SystemOverlay( EventThreadServices& eventThreadServices );

  /**
   * Second-phase construction.
   */
  void Initialize();

  /**
   * Lazy initialization of the SystemOverlay root actor.
   */
  void CreateRootLayer();

  /**
   * Lazy initialization of the default camera actor.
   */
  void CreateDefaultCameraActor();

private:

  EventThreadServices& mEventThreadServices;

  Vector2 mSize;

  IntrusivePtr<RenderTaskList> mOverlayRenderTaskList;

  LayerPtr mRootLayer;

  CameraActorPtr mDefaultCameraActor;

  // Ordered list of currently on-stage layers
  OwnerPointer<LayerList> mLayerList;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SYSTEM_OVERLAY_H__
