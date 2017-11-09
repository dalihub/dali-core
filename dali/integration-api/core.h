#ifndef DALI_INTEGRATION_CORE_H
#define DALI_INTEGRATION_CORE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/view-mode.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/integration-api/core-enumerations.h>
#include <dali/integration-api/resource-policies.h>

namespace Dali
{

namespace Internal
{
class Core;
}

namespace Integration
{

class Core;
class GestureManager;
class GlAbstraction;
class GlSyncAbstraction;
class PlatformAbstraction;
class RenderController;
class SystemOverlay;
struct Event;
struct TouchData;


/**
 * The reasons why further updates are required.
 */
namespace KeepUpdating
{
enum Reasons
{
  NOT_REQUESTED           = 0,    ///< Zero means that no further updates are required
  STAGE_KEEP_RENDERING    = 1<<1, ///<  - Stage::KeepRendering() is being used
  ANIMATIONS_RUNNING      = 1<<2, ///< - Animations are ongoing
  MONITORING_PERFORMANCE  = 1<<3, ///< - The --enable-performance-monitor option is being used
  RENDER_TASK_SYNC        = 1<<4  ///< - A render task is waiting for render sync
};
}

/**
 * The status of the Core::Update operation.
 */
class UpdateStatus
{
public:

  /**
   * Constructor
   */
  UpdateStatus()
  : keepUpdating(false),
    needsNotification(false),
    secondsFromLastFrame( 0.0f )
  {
  }

public:

  /**
   * Query whether the Core has further frames to update & render e.g. when animations are ongoing.
   * @return A bitmask of KeepUpdating values
   */
  unsigned int KeepUpdating() { return keepUpdating; }

  /**
   * Query whether the Core requires an Notification event.
   * This should be sent through the same mechanism (e.g. event loop) as input events.
   * @return True if an Notification event should be sent.
   */
  bool NeedsNotification() { return needsNotification; }

  /**
   * This method is provided so that FPS can be easily calculated with a release version
   * of Core.
   * @return the seconds from last frame as float
   */
  float SecondsFromLastFrame() { return secondsFromLastFrame; }

public:

  unsigned int keepUpdating; ///< A bitmask of KeepUpdating values
  bool needsNotification;
  float secondsFromLastFrame;
};

/**
 * The status of the Core::Render operation.
 */
class RenderStatus
{
public:

  /**
   * Constructor
   */
  RenderStatus()
  : needsUpdate( false ),
    needsPostRender( false )
  {
  }

  /**
   * Set whether update needs to run following a render.
   * @param[in] updateRequired Set to true if an update is required to be run
   */
  void SetNeedsUpdate( bool updateRequired )
  {
    needsUpdate = updateRequired;
  }

  /**
   * Query the update status following rendering of a frame.
   * @return True if update is required to be run
   */
  bool NeedsUpdate() const
  {
    return needsUpdate;
  }

  /**
   * Sets if a post-render should be run.
   * If nothing is rendered this frame, we can skip post-render.
   * @param[in] postRenderRequired Set to True if post-render is required to be run
   */
  void SetNeedsPostRender( bool postRenderRequired )
  {
    needsPostRender = postRenderRequired;
  }

  /**
   * Queries if a post-render should be run.
   * @return True if post-render is required to be run
   */
  bool NeedsPostRender() const
  {
    return needsPostRender;
  }

private:

  bool needsUpdate      :1;  ///< True if update is required to be run
  bool needsPostRender  :1;  ///< True if post-render is required to be run.
};

/**
 * Integration::Core is used for integration with the native windowing system.
 * The following integration tasks must be completed:
 *
 * 1) Handle GL context creation, and notify the Core when this occurs.
 *
 * 2) Provide suspend/resume behaviour (see below for more details).
 *
 * 3) Run an event loop, for passing events to the Core e.g. multi-touch input events.
 * Notification events should be sent after a frame is updated (see UpdateStatus).
 *
 * 4) Run a rendering loop, instructing the Core to render each frame.
 * A separate rendering thread is recommended; see multi-threading options below.
 *
 * 5) Provide an implementation of the PlatformAbstraction interface, used to access platform specific services.
 *
 * 6) Provide an implementation of the GlAbstraction interface, used to access OpenGL services.
 *
 * 7) Provide an implementation of the GestureManager interface, used to register gestures provided by the platform.
 *
 * Multi-threading notes:
 *
 * The Dali API methods are not reentrant.  If you access the API from multiple threads simultaneously, then the results
 * are undefined. This means that your application might segfault, or behave unpredictably.
 *
 * Rendering strategies:
 *
 * 1) Single-threaded. Call every Core method from the same thread. Event handling and rendering will occur in the same thread.
 * This is not recommended, since processing input (slowly) can affect the smooth flow of animations.
 *
 * 2) Multi-threaded. The Core update & render operations can be processed in separate threads.
 * See the method descriptions in Core to see which thread they should be called from.
 * This is the recommended option, so that input processing will not affect the smoothness of animations.
 * Note that the rendering thread must be halted, before destroying the GL context.
 */
class DALI_IMPORT_API Core
{
public:

  /**
   * Create a new Core.
   * This object is used for integration with the native windowing system.
   * @param[in] renderController The interface to an object which controls rendering.
   * @param[in] platformAbstraction The interface providing platform specific services.
   * @param[in] glAbstraction The interface providing OpenGL services.
   * @param[in] glSyncAbstraction The interface providing OpenGL sync objects.
   * @param[in] gestureManager The interface providing gesture manager services.
   * @param[in] policy The data retention policy. This depends on application setting
   * and platform support. Dali should honour this policy when deciding to discard
   * intermediate resource data.
   * @param[in] renderToFboEnabled Whether rendering into the Frame Buffer Object is enabled.
   * @param[in] depthBufferAvailable Whether the depth buffer is available
   * @param[in] stencilBufferAvailable Whether the stencil buffer is available
   * @return A newly allocated Core.
   */
  static Core* New( RenderController& renderController,
                    PlatformAbstraction& platformAbstraction,
                    GlAbstraction& glAbstraction,
                    GlSyncAbstraction& glSyncAbstraction,
                    GestureManager& gestureManager,
                    ResourcePolicy::DataRetention policy,
                    RenderToFrameBuffer renderToFboEnabled,
                    DepthBufferAvailable depthBufferAvailable,
                    StencilBufferAvailable stencilBufferAvailable );

  /**
   * Non-virtual destructor. Core is not intended as a base class.
   */
  ~Core();

  // GL Context Lifecycle

  /**
   * Get the object that will notify the application/toolkit when context is lost/regained
   */
  ContextNotifierInterface* GetContextNotifier();

  /**
   * Notify the Core that the GL context has been created.
   * The context must be created before the Core can render.
   * Multi-threading note: this method should be called from the rendering thread only
   * @post The Core is aware of the GL context.
   */
  void ContextCreated();

  /**
   * Notify the Core that that GL context is about to be destroyed.
   * The Core will free any previously allocated GL resources.
   * Multi-threading note: this method should be called from the rendering thread only
   * @post The Core is unaware of any GL context.
   */
  void ContextDestroyed();

  /**
   * Notify the Core that the GL context has been re-created, e.g. after ReplaceSurface
   * or Context loss.
   *
   * In the case of ReplaceSurface, both ContextToBeDestroyed() and ContextCreated() will have
   * been called on the render thread before this is called on the event thread.
   *
   * Multi-threading note: this method should be called from the main thread
   */
  void RecoverFromContextLoss();

  /**
   * Notify the Core that the GL surface has been resized.
   * This should be done at least once i.e. after the first call to ContextCreated().
   * The Core will use the surface size for camera calculations, and to set the GL viewport.
   * Multi-threading note: this method should be called from the main thread
   * @param[in] width The new surface width.
   * @param[in] height The new surface height.
   */
  void SurfaceResized(unsigned int width, unsigned int height);

  /**
   * Notify the Core about the top margin size.
   * Available stage size is reduced by this size.
   * The stage is located below the size at the top of the display
   * It is mainly useful for indicator in mobile device
   * @param[in] margin margin size
   */
  void SetTopMargin( unsigned int margin );

  // Core setters

  /**
   * Notify the Core about the display's DPI values.
   * This should be done after the display is initialized and a Core instance is created.
   * The Core will use the DPI values for font rendering.
   * Multi-threading note: this method should be called from the main thread
   * @param[in] dpiHorizontal Horizontal DPI value.
   * @param[in] dpiVertical   Vertical DPI value.
   */
  void SetDpi(unsigned int dpiHorizontal, unsigned int dpiVertical);

  // Core Lifecycle

  /**
   * Notify Core that the scene has been created.
   */
  void SceneCreated();

  /**
   * Queue an event with Core.
   * Pre-processing of events may be beneficial e.g. a series of motion events could be throttled, so that only the last event is queued.
   * Multi-threading note: this method should be called from the main thread.
   * @param[in] event The new event.
   */
  void QueueEvent(const Event& event);

  /**
   * Process the events queued with QueueEvent().
   * Multi-threading note: this method should be called from the main thread.
   * @pre ProcessEvents should not be called during ProcessEvents.
   */
  void ProcessEvents();

  /**
   * The Core::Update() method prepares a frame for rendering. This method determines how many frames
   * may be prepared, ahead of the rendering.
   * For example if the maximum update count is 2, then Core::Update() for frame N+1 may be processed
   * whilst frame N is being rendered. However the Core::Update() for frame N+2 may not be called, until
   * the Core::Render() method for frame N has returned.
   * @return The maximum update count (>= 1).
   */
  unsigned int GetMaximumUpdateCount() const;

  /**
   * Update the scene for the next frame. This method must be called before each frame is rendered.
   * Multi-threading notes: this method should be called from a dedicated update-thread.
   * The update for frame N+1 may be processed whilst frame N is being rendered.
   * However the update-thread must wait until frame N has been rendered, before processing frame N+2.
   * After this method returns, messages may be queued internally for the main thread.
   * In order to process these messages, a notification is sent via the main thread's event loop.
   * @param[in] elapsedSeconds Number of seconds since the last call
   * @param[in] lastVSyncTimeMilliseconds The last vsync time in milliseconds
   * @param[in] nextVSyncTimeMilliseconds The time of the next predicted VSync in milliseconds
   * @param[out] status showing whether further updates are required. This also shows
   * whether a Notification event should be sent, regardless of whether the multi-threading is used.
   * @param[in] renderToFboEnabled Whether rendering into the Frame Buffer Object is enabled.
   * @param[in] isRenderingToFbo Whether this frame is being rendered into the Frame Buffer Object.
   */
  void Update( float elapsedSeconds,
               unsigned int lastVSyncTimeMilliseconds,
               unsigned int nextVSyncTimeMilliseconds,
               UpdateStatus& status,
               bool renderToFboEnabled,
               bool isRenderingToFbo );

  /**
   * Render the next frame. This method should be preceded by a call up Update.
   * Multi-threading note: this method should be called from a dedicated rendering thread.
   * @pre The GL context must have been created, and made current.
   * @param[out] status showing whether update is required to run.
   */
  void Render( RenderStatus& status );

  // System-level overlay

  /**
   * Use the SystemOverlay to draw content for system-level indicators, dialogs etc.
   * @return The SystemOverlay.
   */
  SystemOverlay& GetSystemOverlay();

  /**
   * Set the stereoscopic 3D view mode
   * @param[in] viewMode The new view mode
   */
  void SetViewMode( ViewMode viewMode );

  /**
   * Get the current view mode
   * @return The current view mode
   * @see SetViewMode.
   */
  ViewMode GetViewMode() const;

  /**
   * Set the stereo base (eye seperation) for stereoscopic 3D
   * @param[in] stereoBase The stereo base (eye seperation) for stereoscopic 3D (mm)
   */
  void SetStereoBase( float stereoBase );

  /**
   * Get the stereo base (eye seperation) for stereoscopic 3D
   * @return The stereo base (eye seperation) for stereoscopic 3D (mm)
   */
  float GetStereoBase() const;

private:

  /**
   * Private constructor; see also Core::New()
   */
  Core();

  /**
   * Undefined copy-constructor.
   * This avoids accidental calls to a default copy-constructor.
   * @param[in] core A reference to the object to copy.
   */
  Core(const Core& core);

  /**
   * Undefined assignment operator.
   * This avoids accidental calls to a default assignment operator.
   * @param[in] rhs A reference to the object to copy.
   */
  Core& operator=(const Core& rhs);

private:

  Internal::Core* mImpl;

};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_CORE_H
