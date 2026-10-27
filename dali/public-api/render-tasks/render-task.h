#ifndef DALI_RENDER_TASK_H
#define DALI_RENDER_TASK_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/signals/dali-signal.h>

namespace DALI_NAMESPACE
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
 * The Scene provides access to an ordered list of RenderTasks.
 *
 * Each RenderTask must specify the source actors to be rendered, and a camera actor from
 * which the scene is viewed.
 *
 * RenderTasks may optionally target a frame-buffer, otherwise the default surface is used;
 * typically this is a window provided by the native system.
 *
 * By default Dali provides a single RenderTask, which renders the entire actor hierarchy using
 * a default camera actor and the default surface.
 *
 * The first RenderTask used for input handling will be the last one rendered, which also has input enabled,
 * and has a valid source & camera actor; see SetInputEnabled().
 *
 * If none of the actors are hit in the last RenderTask rendered, then input handling will continue
 * with the second last RenderTask rendered, and so on.
 *
 * All RenderTasks which target a frame-buffer (i.e. off screen) will be rendered before all RenderTasks
 * which target the default surface. This allows the user to render intermediate targets which are used
 * later when targeting the screen.
 *
 * A RenderTask targeting a frame-buffer can still be hit-tested, provided that the
 * screen->frame-buffer coordinate conversion is successful; see SetScreenToFrameBufferFunction().
 *
 * If the refresh rate is REFRESH_ONCE and the finished signal is connected, it will be emitted when the RenderTask is completed.
 * Note that all connected signals must be disconnected before the object is destroyed. This is typically done in the
 * object destructor, and requires either the Dali::Connection object or Dali::RenderTask handle to be stored.
 *
 * Signals
 * | %Signal Name | Method                |
 * |--------------|-----------------------|
 * | finished     | @ref FinishedSignal() |
 * @SINCE_1_0.0
 */
class DALI_CORE_API RenderTask : public Handle
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
       * @brief name "viewportPosition", type Property::VECTOR2
       * @SINCE_1_0.0
       */
      VIEWPORT_POSITION = DEFAULT_OBJECT_PROPERTY_START_INDEX,
      /**
       * @brief name "viewportSize", type Property::VECTOR2
       * @SINCE_1_0.0
       */
      VIEWPORT_SIZE,
      /**
       * @brief name "clearColor", type Property::VECTOR4
       * @SINCE_1_0.0
       */
      CLEAR_COLOR,
      /**
       * @brief name "requiresSync", type Property::BOOLEAN
       * @details By default, the sync object is not created.
       *  When native image is used as render target, in order to track when the render to pixmap is completed, the GL sync should be enabled.
       *  Thus the RENDER_ONCE finished signal can be emitted at the correct timing.
       * @SINCE_1_1.29
       * @note The use of GL sync may cause a deadlock if a single pixmap is accessed by multiple parties at the same time.
       */
      REQUIRES_SYNC,
      /**
       * @brief name "renderedScaleFactor", type Property::VECTOR2 or Property::FLOAT
       * @details The scale factor between source scene and rendered target.
       *  For example, if we render a 200x200 scene into a 50x400 FBO, the rendered scale factor is Vector2(0.5f, 2.0f).
       *  If the property is set as a float, the value is applied to both components.
       * @SINCE_2_4.36
       */
      RENDERED_SCALE_FACTOR,
    };
  };

  /**
   * @brief Typedef for signals sent by this class.
   * @SINCE_1_0.0
   */
  using RenderTaskSignalType = Signal<void(RenderTask)>;

  /**
   * @brief A pointer to a function for converting screen to frame-buffer coordinates.
   * @SINCE_1_0.0
   * @param[in,out] coordinates The screen coordinates to convert where (0,0) is the top-left of the screen
   * @return True if the conversion was successful, otherwise coordinates should be unmodified
   */
  using ScreenToFrameBufferFunction = bool (*)(Vector2&);

  /**
   * @brief A pointer to a function for converting screen to frame-buffer coordinates.
   * @SINCE_1_0.0
   * @param[in,out] coordinates The screen coordinates to convert where (0,0) is the top-left of the screen
   * @return True if the conversion was successful, otherwise coordinates should be unmodified
   */
  using ConstScreenToFrameBufferFunction = bool (*const)(Vector2&);

  /**
   * @brief The default conversion function returns false for any screen coordinates.
   *
   * This effectively disables hit-testing for RenderTasks rendering to a frame buffer.
   * See also FULLSCREEN_FRAMEBUFFER_FUNCTION below.
   * @SINCE_2_5.46
   */
  static ConstScreenToFrameBufferFunction DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION;

  /**
   * @brief This conversion function outputs the (unmodified) screen coordinates as frame-buffer coordinates.
   *
   * Therefore the contents of an off-screen image are expected to be rendered "full screen".
   * @SINCE_2_5.46
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

  /**
   * @brief Enumeration for the type of built-in camera actor.
   * @SINCE_2_5.21
   * @see SetBuiltinCameraActor()
   */
  enum class BuiltinCameraType
  {
    ATTACHED_TO_SCENE,         ///< Builtin camera attached to scene. @SINCE_2_5.21
    ATTACHED_TO_SOURCE_ACTOR,  ///< Builtin camera attached to source actor. @SINCE_2_5.21
    ATTACHED_TO_STOPPER_ACTOR, ///< Builtin camera attached to stopper actor, or source actor if stopper node not applied. @SINCE_2_5.21
  };

  /**
   * @brief Enumeration for the type of camera actor used by the RenderTask.
   * @SINCE_2_5.21
   * @see GetCameraActorType()
   */
  enum class CameraActorType
  {
    DEFAULT,               ///< Default camera from RenderTaskList. @SINCE_2_5.21
    USER,                  ///< CameraActor set by SetCameraActor. @SINCE_2_5.21
    BUILTIN_SCENE,         ///< CameraActor set by SetBuiltinCameraActor with BuiltinCameraType::ATTACHED_TO_SCENE. @SINCE_2_5.21
    BUILTIN_SOURCE_ACTOR,  ///< CameraActor set by SetBuiltinCameraActor with BuiltinCameraType::ATTACHED_TO_SOURCE_ACTOR. @SINCE_2_5.21
    BUILTIN_STOPPER_ACTOR, ///< CameraActor set by SetBuiltinCameraActor with BuiltinCameraType::ATTACHED_TO_STOPPER_ACTOR. @SINCE_2_5.21
  };

  static const bool     DEFAULT_EXCLUSIVE;     ///< Default exclusive setting, false. @SINCE_2_5.46
  static const bool     DEFAULT_INPUT_ENABLED; ///< Default input enabled setting, true. @SINCE_2_5.46
  static const Vector4  DEFAULT_CLEAR_COLOR;   ///< Default clear color, Color::BLACK. @SINCE_2_5.46
  static const bool     DEFAULT_CLEAR_ENABLED; ///< Default clear enabled setting, false. @SINCE_2_5.46
  static const bool     DEFAULT_CULL_MODE;     ///< Default cull mode, true. @SINCE_2_5.46
  static const uint32_t DEFAULT_REFRESH_RATE;  ///< Default refresh rate, REFRESH_ALWAYS. @SINCE_2_5.46

  /**
   * @brief Creates an empty RenderTask handle.
   *
   * This can be initialised with RenderTaskList::CreateTask().
   * @SINCE_1_0.0
   */
  RenderTask();

  /**
   * @brief Downcasts a handle to RenderTask handle.
   *
   * If handle points to a RenderTask, the
   * downcast produces a valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle A handle to an object
   * @return A handle to a RenderTask or an uninitialized handle
   */
  static RenderTask DownCast(BaseHandle handle);

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
   * @brief Move constructor.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   */
  RenderTask(RenderTask&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  RenderTask& operator=(RenderTask&& rhs) noexcept;

  /**
   * @brief Sets the actors to be rendered.
   * @SINCE_1_0.0
   * @param[in] actor This actor and its children will be rendered.
   * If actor is an empty handle, then nothing will be rendered
   */
  void SetSourceActor(Actor actor);

  /**
   * @brief Retrieves the actors to be rendered.
   * @SINCE_1_0.0
   * @return This actor and its children will be rendered
   */
  Actor GetSourceActor() const;

  /**
   * @brief Retrieves the stopper actor.
   * @SINCE_2_3.23
   * @return The actor that marks where to stop rendering.
   */
  Actor GetStopperActor() const;

  /**
   * @brief Sets whether the RenderTask has exclusive access to the source actors; the default is false.
   * @SINCE_1_0.0
   * @param[in] exclusive True if the source actors will only be rendered by this RenderTask
   */
  void SetExclusive(bool exclusive);

  /**
   * @brief Queries whether the RenderTask has exclusive access to the source actors.
   * @SINCE_1_0.0
   * @return True if the source actors will only be rendered by this RenderTask
   */
  bool IsExclusive() const;

  /**
   * @brief Sets whether the RenderTask should be considered for input handling; the default is true.
   *
   * The task used for input handling will be the last task in the RenderTaskList which has input enabled,
   * and has a valid source & camera actor.
   * A RenderTask targeting a frame-buffer can still be hit-tested, provided that the screen->frame-buffer
   * coordinate conversion is successful; see also SetScreenToFrameBufferFunction().
   * @SINCE_1_0.0
   * @param[in] enabled True if the RenderTask should be considered for input handling
   */
  void SetInputEnabled(bool enabled);

  /**
   * @brief Queries whether the RenderTask should be considered for input handling.
   * @SINCE_1_0.0
   * @return True if the RenderTask should be considered for input handling
   */
  bool GetInputEnabled() const;

  /**
   * @brief Sets the actor from which the scene is viewed.
   *
   * This sets the camera actor type to CameraActorType::USER.
   * This is exclusive with SetBuiltinCameraActor - calling this will clear any built-in camera actor.
   *
   * @SINCE_1_0.0
   * @param[in] cameraActor The scene is viewed from the perspective of this actor
   */
  void SetCameraActor(CameraActor cameraActor);

  /**
   * @brief Sets an internal built-in camera actor.
   *
   * This method creates an internal camera actor that looks at the center of its parent.
   * The camera uses the provided size for perspective projection via SetPerspectiveProjection.
   *
   * BuiltinCameraType decides where the camera actor is attached:
   * - BuiltinCameraType::ATTACHED_TO_SCENE attached to the root layer of the source actor.
   * - BuiltinCameraType::ATTACHED_TO_SOURCE_ACTOR attached to the source actor.
   * - BuiltinCameraType::ATTACHED_TO_STOPPER_ACTOR attached to the stopper actor.
   *
   * This sets the camera actor type to CameraActorType::BUILTIN_SCENE, CameraActorType::BUILTIN_SOURCE_ACTOR or CameraActorType::BUILTIN_STOPPER_ACTOR accordingly.
   * This is exclusive with SetCameraActor - calling this will clear any user-set camera actor.
   *
   * @SINCE_2_5.21
   * @param[in] builtinCameraType The type of built-in camera actor. It decides where the camera is attached.
   * @param[in] screenSize The size used for SetPerspectiveProjection of the internal camera.
   * @param[in] cameraPropertyMap The extra properties for the internal camera. For example, InvertY, or offset from center of parent.
   */
  void SetBuiltinCameraActor(BuiltinCameraType builtinCameraType, Dali::Size screenSize, const Dali::Property::Map& cameraPropertyMap = Dali::Property::Map());

  /**
   * @brief Retrieves the actor from which the scene is viewed.
   * @SINCE_1_0.0
   * @return The scene is viewed from the perspective of this actor
   */
  CameraActor GetCameraActor() const;

  /**
   * @brief Retrieves the type of camera actor.
   *
   * @SINCE_2_5.21
   * @return The type of camera actor for the RenderTask.
   */
  CameraActorType GetCameraActorType() const;

  /**
   * @brief Sets the frame-buffer used as a render target.
   * @SINCE_1_1.38
   * @param[in] frameBuffer A valid FrameBuffer handle to enable off-screen rendering, or an uninitialized handle to disable it
   */
  void SetFrameBuffer(FrameBuffer frameBuffer);

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
  void SetScreenToFrameBufferFunction(ScreenToFrameBufferFunction conversionFunction);

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
   * @note The mapping actor needs to be rendered by the default RenderTask to make the mapping work properly.
   * @note An Actor can be MappingActor for only one RenderTask.
   * @note If mappingActor is an empty handle, this RenderTask does not use a mapping actor.
   * @note The mapping actor cannot be used as a source actor of any RenderTask.
   */
  void SetScreenToFrameBufferMappingActor(Actor mappingActor);

  /**
   * @brief Retrieves the actor used to convert screen coordinates to frame-buffer coordinates.
   * @SINCE_1_0.0
   * @return The actor used for conversion
   */
  Actor GetScreenToFrameBufferMappingActor() const;

  /**
   * @brief Sets the actor used to compute the viewport of this RenderTask.
   * The actor should be on the scene.
   * @SINCE_2_1.36
   * @param[in] actor This actor is used to compute the viewport of the RenderTask.
   * @note If the default camera of the window is rotated and the actor is no longer a rectangle on the screen, the viewport may be computed incorrectly.
   * The values of VIEWPORT_POSITION and VIEWPORT_SIZE are kept while the viewport guide actor is used; only their current values change.
   */
  void SetViewportGuideActor(Actor actor);

  /**
   * @brief Retrieves the actor used to compute the viewport of this RenderTask.
   * @SINCE_2_1.36
   * @return The actor used to compute the viewport of the RenderTask.
   */
  Actor GetViewportGuideActor() const;

  /**
   * @brief Resets the actor used to compute the viewport of this RenderTask.
   * @SINCE_2_1.36
   * @note The viewport returns to the values of VIEWPORT_POSITION and VIEWPORT_SIZE that were last set.
   */
  void ResetViewportGuideActor();

  /**
   * @brief Sets the scale factor for rendered output.
   * @details The scale factor between source scene and rendered target.
   *  For example, if we render a 200x200 scene into a 50x400 FBO, the rendered scale factor is Vector2(0.5f, 2.0f).
   * @SINCE_2_4.36
   * @param[in] scaleFactor The scaling factor to apply to the rendered content
   */
  void SetRenderedScaleFactor(const Vector2& scaleFactor);

  /**
   * @brief Gets the rendered output scale factor.
   * @SINCE_2_4.36
   * @return The scale factor that was last set
   */
  Vector2 GetRenderedScaleFactor() const;

  /**
   * @brief Sets the viewport position used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default, the viewport covers the whole target window or frame-buffer.
   * @SINCE_1_0.0
   * @param[in] position The viewport's position (x,y)
   * @note Unlike the glViewport method, the x & y coordinates refer to the top-left of the viewport rectangle.
   */
  void SetViewportPosition(Vector2 position);

  /**
   * @brief Retrieves the viewport position.
   * @SINCE_2_5.46
   * @return The viewport position that was last set (the target of any ongoing animation), which may differ from the position currently used for rendering
   * @note While a viewport guide actor is set, this returns the position that was last set, not the one computed from the guide actor.
   * @see Property::VIEWPORT_POSITION
   */
  Vector2 GetViewportPosition() const;

  /**
   * @brief Retrieves the viewport position used when rendering.
   * @SINCE_1_0.0
   * @DEPRECATED_2_5.46 Use GetViewportPosition() instead. For the value used for rendering, use GetCurrentProperty() with Property::VIEWPORT_POSITION.
   * @return The viewport
   */
  Vector2 GetCurrentViewportPosition() const;

  /**
   * @brief Sets the viewport size used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default, the viewport covers the whole target window or frame-buffer.
   * @SINCE_1_0.0
   * @param[in] size The viewport's size (width,height)
   */
  void SetViewportSize(Vector2 size);

  /**
   * @brief Retrieves the viewport size.
   * @SINCE_2_5.46
   * @return The viewport size that was last set (the target of any ongoing animation), which may differ from the size currently used for rendering
   * @note While a viewport guide actor is set, this returns the size that was last set, not the one computed from the guide actor.
   * @note Returns Vector2::ZERO if the viewport has not been set; the target window or frame-buffer size is then used for rendering.
   * @see Property::VIEWPORT_SIZE
   */
  Vector2 GetViewportSize() const;

  /**
   * @brief Retrieves the viewport size used when rendering.
   * @SINCE_1_0.0
   * @DEPRECATED_2_5.46 Use GetViewportSize() instead. For the value used for rendering, use GetCurrentProperty() with Property::VIEWPORT_SIZE.
   * @return The viewport
   */
  Vector2 GetCurrentViewportSize() const;

  /**
   * @brief Sets the viewport used when rendering.
   *
   * This specifies the transformation between normalized device coordinates and target window (or frame-buffer) coordinates.
   * By default this will match the target window or frame-buffer size.
   * @SINCE_1_0.0
   * @DEPRECATED_2_5.46 Use SetViewportPosition and SetViewportSize instead.
   * @param[in] viewport The new viewport
   * @note Unlike the glViewport method, the x & y coordinates refer to the top-left of the viewport rectangle.
   */
  void SetViewport(Viewport viewport);

  /**
   * @brief Retrieves the viewport used when rendering.
   * @SINCE_1_0.0
   * @DEPRECATED_2_5.46 Use GetViewportPosition and GetViewportSize instead.
   * @return The viewport
   */
  Viewport GetViewport() const;

  /**
   * @brief Sets the clear color used when SetClearEnabled(true) is used.
   * @SINCE_1_0.0
   * @param[in] color The new clear color
   */
  void SetClearColor(const Vector4& color);

  /**
   * @brief Retrieves the clear color used when SetClearEnabled(true) is used.
   * @SINCE_1_0.0
   * @return The clear color that was last set (the target of any ongoing animation), which may differ from the color currently used for rendering
   */
  Vector4 GetClearColor() const;

  /**
   * @brief Sets whether the RenderTask will clear the results of previous RenderTasks.
   *
   * The default is false.
   *
   * @SINCE_1_0.0
   * @param[in] enabled True if the RenderTask should clear
   * @note The default surface is cleared automatically at the
   * beginning of each frame; this setting is only useful when 2+
   * RenderTasks are used, and the result of the first task needs to
   * be (partially) cleared before rendering the second.
   *
   */
  void SetClearEnabled(bool enabled);

  /**
   * @brief Queries whether the RenderTask will clear the results of previous RenderTasks.
   * @SINCE_1_0.0
   * @return True if the RenderTask should clear
   */
  bool GetClearEnabled() const;

  /**
   * @brief Sets whether the RenderTask will cull the actors to the camera's view frustum.
   *
   * @SINCE_1_0.0
   * @param[in] cullMode True if the renderers should be culled
   * @note The default mode is to cull actors.
   * @note If the shader uses @ref Shader::Hint::MODIFIES_GEOMETRY then culling optimizations are disabled.
   * @see Shader::Hint
   */
  void SetCullMode(bool cullMode);

  /**
   * @brief Gets the cull mode.
   *
   * @SINCE_1_0.0
   * @return True if the RenderTask should cull the actors to the camera's view frustum
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
   * RenderTask will not be rendered, regardless of this value.
   *
   * The REFRESH_ONCE value means that the RenderTask will be
   * processed once only, to take a snap-shot of the scene.
   * Repeatedly calling SetRefreshRate(REFRESH_ONCE) will cause more
   * snap-shots to be taken.
   *
   * @SINCE_1_0.0
   * @param[in] refreshRate The new refresh rate
   */
  void SetRefreshRate(uint32_t refreshRate);

  /**
   * @brief Queries the refresh-rate of the RenderTask.
   * @SINCE_1_0.0
   * @return The refresh-rate
   */
  uint32_t GetRefreshRate() const;

  /**
   * @brief Gets viewport coordinates for given world position.
   *
   * @SINCE_1_1.13
   * @param[in] position The world position
   * @param[out] viewportX The viewport x position
   * @param[out] viewportY The viewport y position
   * @return True if the position has a screen coordinate
   */
  bool WorldToViewport(const Vector3& position, float& viewportX, float& viewportY) const;

  /**
   * @brief Gets actor local coordinates for given viewport coordinates.
   *
   * @SINCE_1_1.13
   * @param[in] actor The actor describing local coordinate system
   * @param[in] viewportX The viewport x position
   * @param[in] viewportY The viewport y position
   * @param[out] localX The local x position
   * @param[out] localY The local y position
   * @return True if the screen position has a local coordinate
   */
  bool ViewportToLocal(Actor actor, float viewportX, float viewportY, float& localX, float& localY) const;

  /**
   * @brief Sets the render pass tag for this RenderTask.
   *
   * Shader code that matches this render pass tag is used for rendering.
   * If no matching shader is found, the code with a render pass tag of 0 is used.
   * In other cases, operation is not guaranteed.
   * @SINCE_2_5.46
   * @param[in] renderPassTag RenderPassTag value for this RenderTask.
   * @note The RenderPassTag of the default RenderTask is 0u.
   */
  void SetRenderPassTag(uint32_t renderPassTag);

  /**
   * @brief Gets the render pass tag for this RenderTask.
   * @SINCE_2_5.46
   * @return RenderPassTag value for this RenderTask.
   */
  uint32_t GetRenderPassTag() const;

  /**
   * @brief Sets the order index to define the rendering order of this RenderTask.
   *
   * In DALi, off-screen RenderTasks are rendered earlier than on-screen RenderTasks.
   * Within each category of off-screen and on-screen RenderTasks,
   * a RenderTask with a smaller order index is rendered first.
   * The RenderTasks in the RenderTaskList are always sorted in ascending order of the order index.
   * The order index must be in the range [-1000, 1000].
   * The default order index is 0.
   * @SINCE_2_5.46
   * @param[in] orderIndex The order index for this RenderTask.
   * @note The order among RenderTasks whose order index has not changed follows the order in which they were created.
   * @note The rendering order among RenderTasks that have the same order index cannot be guaranteed after the order index is changed.
   */
  void SetOrderIndex(int32_t orderIndex);

  /**
   * @brief Gets the order index of this RenderTask.
   * @SINCE_2_5.46
   * @return The order index of this RenderTask.
   */
  int32_t GetOrderIndex() const;

  /**
   * @brief Gets the unique id of the RenderTask. It is 0 if the RenderTask is invalid.
   *
   * @SINCE_2_3.10
   * @return The unique id of RenderTask, or 0 if invalid.
   */
  uint32_t GetRenderTaskId() const;

  /**
   * @brief Stops rendering at the given actor. The actor is not included.
   * @SINCE_2_3.23
   * @param[in] stopperActor A marker to stop rendering.
   * If stopperActor is an empty handle, or is not a child of the source actor, then the whole source actor tree is rendered
   */
  void RenderUntil(Actor stopperActor);

  /**
   * @brief Requests to keep the rendering result of this frame.
   * @SINCE_2_3.51
   * @note This method is only available when the refresh rate is REFRESH_ONCE.
   * @note The result can be used after the FinishedSignal is emitted.
   */
  void KeepRenderResult();

  /**
   * @brief Requests to clear the rendered result.
   * @SINCE_2_3.51
   * @note Since the rendered result is kept in the render thread resource, this method asynchronously clears the result.
   */
  void ClearRenderResult();

  /**
   * @brief Retrieves the rendered result as PixelData.
   * @SINCE_2_3.51
   * @return Dali::PixelData that contains rendered result.
   * @note This method is only available after the FinishedSignal is emitted.
   * If this method is called before the FinishedSignal is emitted, an empty handle is returned.
   */
  Dali::PixelData GetRenderResult();

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
   * @param[in] renderTask A pointer to a newly allocated RenderTask
   */
  explicit DALI_INTERNAL RenderTask(Internal::RenderTask* renderTask);
  /// @endcond
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif // DALI_RENDER_TASK_H
