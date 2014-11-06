#ifndef __DALI_RENDER_TASK_H__
#define __DALI_RENDER_TASK_H__

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
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/object/constrainable.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali
{

class Actor;
class CameraActor;
class FrameBufferImage;
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
 * later when targetting the screen.
 *
 * A RenderTask targetting a frame-buffer can still be hit-tested, provided that the
 * screen->frame-buffer coordinate conversion is successful; see SetScreenToFrameBufferFunction().
 *
 * If the refresh rate id REFRESH_ONCE and a "Finish" signal is connected, it will be emitted when the RenderTask is completed.
 * Note that all connected signals must be disconnected before the object is destroyed. This is typically done in the
 * object destructor, and requires either the Dali::Connection object or Dali::RenderTask handle to be stored.
 */
class DALI_IMPORT_API RenderTask : public Constrainable
{
public:
  /**
   * @brief Typedef for signals sent by this class.
   */
  typedef SignalV2< void (RenderTask& source) > RenderTaskSignalV2;

  // Default Properties
  static const Property::Index VIEWPORT_POSITION;    ///< Property  0, name "viewport-position",   type VECTOR2
  static const Property::Index VIEWPORT_SIZE;        ///< Property  1, name "viewport-size",       type VECTOR2
  static const Property::Index CLEAR_COLOR;          ///< Property  2, name "clear-color",         type VECTOR4

  //Signal Names
  static const char* const SIGNAL_FINISHED; ///< Name for Finished signal

  /**
   * @brief A pointer to a function for converting screen to frame-buffer coordinates.
   * @param[in,out] coordinates The screen coordinates to convert where (0,0) is the top-left of the screen.
   * @return True if the conversion was successful, otherwise coordinates should be unmodified.
   */
  typedef bool (* ScreenToFrameBufferFunction)( Vector2& coordinates );

  /**
   * @brief A pointer to a function for converting screen to frame-buffer coordinates.
   * @param[in,out] coordinates The screen coordinates to convert where (0,0) is the top-left of the screen.
   * @return True if the conversion was successful, otherwise coordinates should be unmodified.
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
   * @brief The refresh-rate of the RenderTask.
   */
  enum RefreshRate
  {
    REFRESH_ONCE   = 0, ///< Process once only e.g. take a snap-shot of the scene.
    REFRESH_ALWAYS = 1  ///< Process every frame.
  };

  static const bool         DEFAULT_EXCLUSIVE;     ///< false
  static const bool         DEFAULT_INPUT_ENABLED; ///< true
  static const Vector4      DEFAULT_CLEAR_COLOR;   ///< Color::BLACK
  static const bool         DEFAULT_CLEAR_ENABLED; ///< false
  static const bool         DEFAULT_CULL_MODE;     ///< true
  static const unsigned int DEFAULT_REFRESH_RATE;  ///< REFRESH_ALWAYS

  /**
   * @brief Create an empty RenderTask handle.
   *
   * This can be initialised with RenderTaskList::CreateRenderTask().
   */
  RenderTask();

  /**
   * @brief Downcast a handle to RenderTask handle.
   *
   * If handle points to a RenderTask the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle A handle to an object.
   * @return A handle to a RenderTask or an uninitialized handle.
   */
  static RenderTask DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RenderTask();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  RenderTask(const RenderTask& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  RenderTask& operator=(const RenderTask& rhs);

  /**
   * @brief Set the actors to be rendered.
   * @param[in] actor This actor and its children will be rendered.
   * If actor is an empty handle, then nothing will be rendered.
   */
  void SetSourceActor( Actor actor );

  /**
   * @brief Retrieve the actors to be rendered.
   * @return This actor and its children will be rendered.
   */
  Actor GetSourceActor() const;

  /**
   * @brief Set whether the RenderTask has exclusive access to the source actors; the default is false.
   * @param[in] exclusive True if the source actors will only be rendered by this render-task.
   */
  void SetExclusive( bool exclusive );

  /**
   * @brief Query whether the RenderTask has exclusive access to the source actors.
   * @return True if the source actors will only be rendered by this render-task.
   */
  bool IsExclusive() const;

  /**
   * @brief Set whether the render-task should be considered for input handling; the default is true.
   *
   * The task used for input handling will be last task in the RenderTaskList which has input enabled,
   * and has a valid source & camera actor.
   * A RenderTask targetting a frame-buffer can still be hit-tested, provided that the screen->frame-buffer
   * coordinate conversion is successful; see also SetScreenToFrameBufferFunction().
   * @param[in] enabled True if the render-task should be considered for input handling.
   */
  void SetInputEnabled( bool enabled );

  /**
   * @brief Query whether the render-task should be considered for input handling.
   * @return True if the render-task should be considered for input handling.
   */
  bool GetInputEnabled() const;

  /**
   * @brief Set the actor from which the scene is viewed.
   * @param[in] cameraActor The scene is viewed from the perspective of this actor.
   */
  void SetCameraActor( CameraActor cameraActor );

  /**
   * @brief Retrieve the actor from which the scene is viewed.
   * @return The scene is viewed from the perspective of this actor.
   */
  CameraActor GetCameraActor() const;

  /**
   * @brief Set the frame-buffer used as a render target.
   * @param[in] frameBuffer A valid frame-buffer handle to enable off-screen rendering, or an uninitialized handle to disable.
   */
  void SetTargetFrameBuffer( FrameBufferImage frameBuffer );

  /**
   * @brief Retrieve the frame-buffer used as a render target.
   * @return A valid frame-buffer handle, or an uninitialised handle if off-screen rendering is disabled.
   */
  FrameBufferImage GetTargetFrameBuffer() const;

  /**
   * @brief Set the function used to convert screen coordinates to frame-buffer coordinates.
   *
   * This is useful for hit-testing actors which are rendered off-screen.
   * @param[in] conversionFunction The conversion function.
   */
  void SetScreenToFrameBufferFunction( ScreenToFrameBufferFunction conversionFunction );

  /**
   * @brief Retrieve the function used to convert screen coordinates to frame-buffer coordinates.
   * @return The conversion function.
   */
  ScreenToFrameBufferFunction GetScreenToFrameBufferFunction() const;

  /**
   * @brief Set the actor used to convert screen coordinates to frame-buffer coordinates.
   *
   * The local coordinates of the actor are mapped as frame-buffer coordinates.
   * This is useful for hit-testing actors which are rendered off-screen.
   * Note: The mapping actor needs to be rendered by the default render task to make the mapping work properly.
   * @param[in] mappingActor The actor used for conversion.
   */
  void SetScreenToFrameBufferMappingActor( Actor mappingActor );

  /**
   * @brief Retrieve the actor used to convert screen coordinates to frame-buffer coordinates.
   * @return The actor used for conversion.
   */
  Actor GetScreenToFrameBufferMappingActor() const;

  /**
   * @brief Set the GL viewport position used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default this will match the target window or frame-buffer size.
   * @note Unlike the glViewport method, the x & y coordinates refer to the top-left of the viewport rectangle.
   * @param[in] position The viewports position (x,y)
   */
  void SetViewportPosition( Vector2 position );

  /**
   * @brief Retrieve the GL viewport position used when rendering.
   * @return The viewport.
   */
  Vector2 GetCurrentViewportPosition() const;

  /**
   * @brief Set the GL viewport size used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default this will match the target window or frame-buffer size.
   * @param[in] size The viewports size (width,height)
   */
  void SetViewportSize( Vector2 size );

  /**
   * @brief Retrieve the GL viewport size used when rendering.
   * @return The viewport.
   */
  Vector2 GetCurrentViewportSize() const;

  /**
   * @brief Set the GL viewport used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default this will match the target window or frame-buffer size.
   * @note Unlike the glViewport method, the x & y coordinates refer to the top-left of the viewport rectangle.
   * @param[in] viewport The new viewport.
   */
  void SetViewport( Viewport viewport );

  /**
   * @brief Retrieve the GL viewport used when rendering.
   * @return The viewport.
   */
  Viewport GetViewport() const;

  /**
   * @brief Set the clear color used when SetClearEnabled(true) is used.
   * @param[in] color The new clear color.
   */
  void SetClearColor( const Vector4& color );

  /**
   * @brief Retrieve the clear color used when SetClearEnabled(true) is used.
   * @note This property can be animated; the return value may not match the value written with SetClearColor().
   * @return The clear color.
   */
  Vector4 GetClearColor() const;

  /**
   * @brief Set whether the render-task will clear the results of previous render-tasks.
   *
   * The default is false.
   *
   * @note The default GL surface is cleared automatically at the
   * beginning of each frame; this setting is only useful when 2+
   * render-tasks are used, and the result of the first task needs to
   * be (partially) cleared before rendering the second.
   *
   * @param[in] enabled True if the render-task should clear.
   */
  void SetClearEnabled( bool enabled );

  /**
   * @brief Query whether the render-task will clear the results of previous render-tasks.
   * @return True if the render-task should clear.
   */
  bool GetClearEnabled() const;

  /**
   * @brief Set whether the render task will cull the actors to the camera's view frustum.
   *
   * Note that this will only affect image actors that use the default vertex shader.
   * The default mode is to cull actors.
   * @param[in] cullMode True if the renderers should be culled.
   */
  void SetCullMode( bool cullMode );

  /**
   * @brief Get the cull mode.
   *
   * @return True if the render task should cull the actors to the camera's view frustum
   */
  bool GetCullMode() const;

  /**
   * @brief Set the refresh-rate of the RenderTask.
   *
   * The default is REFRESH_ALWAYS (1), meaning that the RenderTask will be processed every frame.
   * It may be desirable to process less frequently e.g. SetRefreshRate(3) will process once every 3 frames.
   * The REFRESH_ONCE value means that the RenderTask will be processed once only, to take a snap-shot of the scene.
   * Repeatedly calling SetRefreshRate(REFRESH_ONCE) will cause more snap-shots to be taken.
   * @param[in] refreshRate The new refresh rate.
   */
  void SetRefreshRate( unsigned int refreshRate );

  /**
   * @brief Query the refresh-rate of the RenderTask.
   * @return The refresh-rate.
   */
  unsigned int GetRefreshRate() const;

public: // Signals

  /**
   * @brief If the refresh rate is REFRESH_ONCE, connect to this signal to be notified when a RenderTask has finished.
   */
  RenderTaskSignalV2& FinishedSignal();

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   * @param [in] renderTask A pointer to a newly allocated render-task
   */
  explicit DALI_INTERNAL RenderTask( Internal::RenderTask* renderTask );
};

} // namespace Dali

#endif //__DALI_RENDER_TASK_H__
