#ifndef __DALI_RENDER_TASK_H__
#define __DALI_RENDER_TASK_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/**
 * @addtogroup dali_core_rendering_effects
 * @{
 */

class Actor;
class CameraActor;
class FrameBufferImage;
class FrameBuffer;
struct Vector4;

namespace Internal DALI_INTERNAL
{
class RenderTask;
}

/**
 * @brief RenderTasks describe how the Dali scene should be rendered.
 *
 * The Stage::GetRenderTaskList() method provides access to an ordered list of render-tasks.
 *
 * Each RenderTask must specify the source actors to be rendered, and a camera actor from
 * which the scene is viewed.
 *
 * RenderTasks may optionally target a frame-buffer, otherwise the default GL surface is used;
 * typically this is a window provided by the native system.
 *
 * By default Dali provides a single RenderTask, which renders the entire actor hierachy using
 * a default camera actor and GL surface. If stereoscopic rendering is enabled, Dali will create
 * two additional render tasks, on for each eye. Each render task will have its own camera parented
 * to the default camera actor.
 *
 * The first RenderTask used for input handling will be the last one rendered, which also has input enabled,
 * and has a valid source & camera actor; see SetInputEnabled().
 *
 * If none of the actors are hit in the last RenderTask rendered, then input handling will continue
 * with the second last RenderTask rendered, and so on.
 *
 * All RenderTasks which target a frame-buffer (i.e. off screen) will be rendered before all RenderTasks
 * which target the default GL surface. This allows the user to render intermediate targets which are used
 * later when targeting the screen.
 *
 * A RenderTask targeting a frame-buffer can still be hit-tested, provided that the
 * screen->frame-buffer coordinate conversion is successful; see SetScreenToFrameBufferFunction().
 *
 * If the refresh rate id REFRESH_ONCE and a "Finish" signal is connected, it will be emitted when the RenderTask is completed.
 * Note that all connected signals must be disconnected before the object is destroyed. This is typically done in the
 * object destructor, and requires either the Dali::Connection object or Dali::RenderTask handle to be stored.
 *
 * Signals
 * | %Signal Name | Method                |
 * |--------------|-----------------------|
 * | finished     | @ref FinishedSignal() |
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API RenderTask : public Handle
{
public:

  /**
   * @brief Enumeration for instances of properties belonging to the RenderTask class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for instances of properties belonging to the RenderTask class.
     * @SINCE_1_0.0
     */
    enum
    {
      /**
       * @brief name "viewportPosition", type Vector2
       * @SINCE_1_0.0
       */
      VIEWPORT_POSITION = DEFAULT_OBJECT_PROPERTY_START_INDEX,
      /**
       * @brief name "viewportSize", type Vector2
       * @SINCE_1_0.0
       */
      VIEWPORT_SIZE,
      /**
       * @brief name "clearColor", type Vector4
       * @SINCE_1_0.0
       */
      CLEAR_COLOR,
      /**
       * @brief name "requiresSync", type BOOLEAN
       * @details By default, the sync object is not created.
       *  When native image source is used as render target, in order to track when the render to pixmap is completed, the GL sync should be enabled.
       *  Thus the RENDER_ONCE finished signal can be emit at the correct timing.
       * @SINCE_1_1.29
       * @note The use of GL sync might cause deadlock with multiple access to the single pixmap happening in the same time.
       */
      REQUIRES_SYNC,
    };
  };

  /**
   * @brief Typedef for signals sent by this class.
   * @SINCE_1_0.0
   */
  typedef Signal< void (RenderTask& source) > RenderTaskSignalType;

  /**
   * @brief A pointer to a function for converting screen to frame-buffer coordinates.
   * @SINCE_1_0.0
   * @param[in,out] coordinates The screen coordinates to convert where (0,0) is the top-left of the screen
   * @return True if the conversion was successful, otherwise coordinates should be unmodified
   */
  typedef bool (* ScreenToFrameBufferFunction)( Vector2& coordinates );

  /**
   * @brief A pointer to a function for converting screen to frame-buffer coordinates.
   * @SINCE_1_0.0
   * @param[in,out] coordinates The screen coordinates to convert where (0,0) is the top-left of the screen
   * @return True if the conversion was successful, otherwise coordinates should be unmodified
   */
  typedef bool (* const ConstScreenToFrameBufferFunction)( Vector2& coordinates );

  /**
   * @brief The default conversion function returns false for any screen coordinates.
   *
   * This effectively disables hit-testing for RenderTasks rendering to a frame buffer.
   * See also FULLSCREEN_FRAMEBUFFER_FUNCTION below.
   */
  static ConstScreenToFrameBufferFunction DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION;

  /**
   * @brief This conversion function outputs the (unmodified) screen coordinates as frame-buffer coordinates.
   *
   * Therefore the contents of an off-screen image is expected to be rendered "full screen".
   */
  static ConstScreenToFrameBufferFunction FULLSCREEN_FRAMEBUFFER_FUNCTION;

  /**
   * @brief Enumeration for the refresh-rate of the RenderTask.
   * @SINCE_1_0.0
   */
  enum RefreshRate
  {
    REFRESH_ONCE   = 0, ///< Process once only e.g. take a snap-shot of the scene. @SINCE_1_0.0
    REFRESH_ALWAYS = 1  ///< Process every frame. @SINCE_1_0.0
  };

  static const bool         DEFAULT_EXCLUSIVE;     ///< false
  static const bool         DEFAULT_INPUT_ENABLED; ///< true
  static const Vector4      DEFAULT_CLEAR_COLOR;   ///< Color::BLACK
  static const bool         DEFAULT_CLEAR_ENABLED; ///< false
  static const bool         DEFAULT_CULL_MODE;     ///< true
  static const unsigned int DEFAULT_REFRESH_RATE;  ///< REFRESH_ALWAYS

  /**
   * @brief Creates an empty RenderTask handle.
   *
   * This can be initialised with RenderTaskList::CreateRenderTask().
   * @SINCE_1_0.0
   */
  RenderTask();

  /**
   * @brief Downcasts a handle to RenderTask handle.
   *
   * If handle points to a RenderTask, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle A handle to an object
   * @return A handle to a RenderTask or an uninitialized handle
   */
  static RenderTask DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~RenderTask();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  RenderTask(const RenderTask& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  RenderTask& operator=(const RenderTask& rhs);

  /**
   * @brief Sets the actors to be rendered.
   * @SINCE_1_0.0
   * @param[in] actor This actor and its children will be rendered.
   * If actor is an empty handle, then nothing will be rendered
   */
  void SetSourceActor( Actor actor );

  /**
   * @brief Retrieves the actors to be rendered.
   * @SINCE_1_0.0
   * @return This actor and its children will be rendered
   */
  Actor GetSourceActor() const;

  /**
   * @brief Sets whether the RenderTask has exclusive access to the source actors; the default is false.
   * @SINCE_1_0.0
   * @param[in] exclusive True if the source actors will only be rendered by this render-task
   */
  void SetExclusive( bool exclusive );

  /**
   * @brief Queries whether the RenderTask has exclusive access to the source actors.
   * @SINCE_1_0.0
   * @return True if the source actors will only be rendered by this render-task
   */
  bool IsExclusive() const;

  /**
   * @brief Sets whether the render-task should be considered for input handling; the default is true.
   *
   * The task used for input handling will be last task in the RenderTaskList which has input enabled,
   * and has a valid source & camera actor.
   * A RenderTask targetting a frame-buffer can still be hit-tested, provided that the screen->frame-buffer
   * coordinate conversion is successful; see also SetScreenToFrameBufferFunction().
   * @SINCE_1_0.0
   * @param[in] enabled True if the render-task should be considered for input handling
   */
  void SetInputEnabled( bool enabled );

  /**
   * @brief Queries whether the render-task should be considered for input handling.
   * @SINCE_1_0.0
   * @return True if the render-task should be considered for input handling
   */
  bool GetInputEnabled() const;

  /**
   * @brief Sets the actor from which the scene is viewed.
   * @SINCE_1_0.0
   * @param[in] cameraActor The scene is viewed from the perspective of this actor
   */
  void SetCameraActor( CameraActor cameraActor );

  /**
   * @brief Retrieves the actor from which the scene is viewed.
   * @SINCE_1_0.0
   * @return The scene is viewed from the perspective of this actor
   */
  CameraActor GetCameraActor() const;

  /**
   * @brief Sets the frame-buffer used as a render target.
   * @SINCE_1_0.0
   * @param[in] frameBuffer A valid frame-buffer handle to enable off-screen rendering, or an uninitialized handle to disable
   */
  void SetTargetFrameBuffer( FrameBufferImage frameBuffer );

  /**
   * @brief Retrieves the frame-buffer used as a render target.
   * @SINCE_1_0.0
   * @return A valid frame-buffer handle, or an uninitialised handle if off-screen rendering is disabled
   */
  FrameBufferImage GetTargetFrameBuffer() const;

  /**
   * @brief Sets the frame-buffer used as a render target.
   * @SINCE_1_1.38
   * @param[in] frameBuffer er A valid FrameBuffer handle to enable off-screen rendering, or an uninitialized handle to disable it
   */
  void SetFrameBuffer( FrameBuffer frameBuffer );

  /**
   * @brief Retrieves the frame-buffer used as a render target.
   * @SINCE_1_1.38
   * @return The framebuffer
   */
  FrameBuffer GetFrameBuffer() const;

  /**
   * @brief Sets the function used to convert screen coordinates to frame-buffer coordinates.
   *
   * This is useful for hit-testing actors which are rendered off-screen.
   * @SINCE_1_0.0
   * @param[in] conversionFunction The conversion function
   */
  void SetScreenToFrameBufferFunction( ScreenToFrameBufferFunction conversionFunction );

  /**
   * @brief Retrieves the function used to convert screen coordinates to frame-buffer coordinates.
   * @SINCE_1_0.0
   * @return The conversion function
   */
  ScreenToFrameBufferFunction GetScreenToFrameBufferFunction() const;

  /**
   * @brief Sets the actor used to convert screen coordinates to frame-buffer coordinates.
   *
   * The local coordinates of the actor are mapped as frame-buffer coordinates.
   * This is useful for hit-testing actors which are rendered off-screen.
   * @SINCE_1_0.0
   * @param[in] mappingActor The actor used for conversion
   * @note The mapping actor needs to be rendered by the default render task to make the mapping work properly.
   */
  void SetScreenToFrameBufferMappingActor( Actor mappingActor );

  /**
   * @brief Retrieves the actor used to convert screen coordinates to frame-buffer coordinates.
   * @SINCE_1_0.0
   * @return The actor used for conversion
   */
  Actor GetScreenToFrameBufferMappingActor() const;

  /**
   * @brief Sets the GL viewport position used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default this will match the target window or frame-buffer size.
   * @SINCE_1_0.0
   * @param[in] position The viewports position (x,y)
   * @note Unlike the glViewport method, the x & y coordinates refer to the top-left of the viewport rectangle.
   */
  void SetViewportPosition( Vector2 position );

  /**
   * @brief Retrieves the GL viewport position used when rendering.
   * @SINCE_1_0.0
   * @return The viewport
   */
  Vector2 GetCurrentViewportPosition() const;

  /**
   * @brief Sets the GL viewport size used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default this will match the target window or frame-buffer size.
   * @SINCE_1_0.0
   * @param[in] size The viewports size (width,height)
   */
  void SetViewportSize( Vector2 size );

  /**
   * @brief Retrieves the GL viewport size used when rendering.
   * @SINCE_1_0.0
   * @return The viewport
   */
  Vector2 GetCurrentViewportSize() const;

  /**
   * @brief Sets the GL viewport used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default this will match the target window or frame-buffer size.
   * @SINCE_1_0.0
   * @param[in] viewport The new viewport
   * @note Unlike the glViewport method, the x & y coordinates refer to the top-left of the viewport rectangle.
   */
  void SetViewport( Viewport viewport );

  /**
   * @brief Retrieves the GL viewport used when rendering.
   * @SINCE_1_0.0
   * @return The viewport
   */
  Viewport GetViewport() const;

  /**
   * @brief Sets the clear color used when SetClearEnabled(true) is used.
   * @SINCE_1_0.0
   * @param[in] color The new clear color
   */
  void SetClearColor( const Vector4& color );

  /**
   * @brief Retrieves the clear color used when SetClearEnabled(true) is used.
   * @SINCE_1_0.0
   * @return The clear color
   * @note This property can be animated; the return value may not match the value written with SetClearColor().
   */
  Vector4 GetClearColor() const;

  /**
   * @brief Sets whether the render-task will clear the results of previous render-tasks.
   *
   * The default is false.
   *
   * @SINCE_1_0.0
   * @param[in] enabled True if the render-task should clear
   * @note The default GL surface is cleared automatically at the
   * beginning of each frame; this setting is only useful when 2+
   * render-tasks are used, and the result of the first task needs to
   * be (partially) cleared before rendering the second.
   *
   */
  void SetClearEnabled( bool enabled );

  /**
   * @brief Queries whether the render-task will clear the results of previous render-tasks.
   * @SINCE_1_0.0
   * @return True if the render-task should clear
   */
  bool GetClearEnabled() const;

  /**
   * @brief Sets whether the render task will cull the actors to the camera's view frustum.
   *
   * @SINCE_1_0.0
   * @param[in] cullMode True if the renderers should be culled
   * @note The default mode is to cull actors.
   * @note If the shader uses @ref Shader::Hint::MODIFIES_GEOMETRY then culling optimizations are disabled.
   * @see Shader::Hint
   */
  void SetCullMode( bool cullMode );

  /**
   * @brief Gets the cull mode.
   *
   * @SINCE_1_0.0
   * @return True if the render task should cull the actors to the camera's view frustum
   */
  bool GetCullMode() const;

  /**
   * @brief Sets the refresh-rate of the RenderTask.
   *
   * The default is REFRESH_ALWAYS (1), meaning that the RenderTask
   * will be processed every frame if the scene graph is changing.  It
   * may be desirable to process less frequently. For example,
   * SetRefreshRate(3) will process once every 3 frames if the scene
   * graph is changing. If the scene graph is not changing, then the
   * render task will not be rendered, regardless of this value.
   *
   * The REFRESH_ONCE value means that the RenderTask will be
   * processed once only, to take a snap-shot of the scene.
   * Repeatedly calling SetRefreshRate(REFRESH_ONCE) will cause more
   * snap-shots to be taken.
   *
   * @SINCE_1_0.0
   * @param[in] refreshRate The new refresh rate
   */
  void SetRefreshRate( unsigned int refreshRate );

  /**
   * @brief Queries the refresh-rate of the RenderTask.
   * @SINCE_1_0.0
   * @return The refresh-rate
   */
  unsigned int GetRefreshRate() const;

  /*
   * @brief Gets viewport coordinates for given world position.
   *
   * @SINCE_1_1.13
   * @param[in] position The world position
   * @param[out] viewportX The viewport x position
   * @param[out] viewportY The viewport y position
   * @return true if the position has a screen coordinate
   */
  bool WorldToViewport(const Vector3 &position, float& viewportX, float& viewportY) const;

  /*
   * @brief Gets actor local coordinates for given viewport coordinates.
   *
   * @SINCE_1_1.13
   * @param[in] actor The actor describing local coordinate system
   * @param[in] viewportX The viewport x position
   * @param[in] viewportY The viewport y position
   * @param[out] localX The local x position
   * @param[out] localY The local y position
   * @return true if the screen position has a local coordinate
   */
  bool ViewportToLocal(Actor actor, float viewportX, float viewportY, float &localX, float &localY) const;

public: // Signals

  /**
   * @brief If the refresh rate is REFRESH_ONCE, connect to this signal to be notified when a RenderTask has finished.
   * @SINCE_1_0.0
   * @return The signal to connect to
   */
  RenderTaskSignalType& FinishedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   * @SINCE_1_0.0
   * @param[in] renderTask A pointer to a newly allocated render-task
   */
  explicit DALI_INTERNAL RenderTask( Internal::RenderTask* renderTask );
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif //__DALI_RENDER_TASK_H__
