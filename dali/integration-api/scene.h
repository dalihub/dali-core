#ifndef DALI_SCENE_H
#define DALI_SCENE_H

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
 *
 */

// EXTERNAL INCLUDES
#include <memory>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-render-target-create-info.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/gesture-enumerations.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/handle.h>

namespace Dali
{
class Actor;
class KeyEvent;
class Layer;
class RenderTaskList;
class TouchEvent;
class WheelEvent;

namespace Internal DALI_INTERNAL
{
class Scene;
}

namespace Graphics
{
class RenderTarget;
}

namespace Integration
{
struct Event;

/**
 * @brief
 *
 * Scene creates a "world" that can be bound to a surface for rendering.
 *
 */
class DALI_CORE_API Scene : public BaseHandle
{

public:

  /**
   * @brief Touch event propagation way
   */
  enum TouchPropagationType
  {
    PARENT,     ///< events are propagated to parents in the traditional way
    GEOMETRY    ///< events are propagated based on geometry.
  };

  using EventProcessingFinishedSignalType = Signal<void()>;                        ///< Event Processing finished signal type
  using KeyEventSignalType                = Signal<void(const Dali::KeyEvent&)>;   ///< Key event signal type
  using KeyEventGeneratedSignalType       = Signal<bool(const Dali::KeyEvent&)>;   ///< key event generated signal type
  using TouchEventSignalType              = Signal<void(const Dali::TouchEvent&)>; ///< Touch signal type
  using WheelEventSignalType              = Signal<void(const Dali::WheelEvent&)>; ///< WheelEvent signal type
  using WheelEventGeneratedSignalType     = Signal<bool(const Dali::WheelEvent&)>; ///< WheelEvent generated signal type

  using FrameCallbackContainer = std::vector<std::pair<std::unique_ptr<CallbackBase>, int32_t> >;

  /**
   * @brief Create an initialized Scene handle.
   *
   * @param[in] size The size of the set surface for this scene
   * @param[in] windowOrientation The rotated angle of the set surface for this scene
   * @param[in] screenOrientation The rotated angle of the screen
   *
   * @return a handle to a newly allocated Dali resource.
   */
  static Scene New(Size size, int32_t windowOrientation = 0, int32_t screenOrientation = 0);

  /**
   * @brief Downcast an Object handle to Scene handle.
   *
   * If handle points to a Scene object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Scene object or an uninitialized handle
   */
  static Scene DownCast(BaseHandle handle);

  /**
   * @brief Create an uninitialized Scene handle.
   *
   * This can be initialized with Scene::New(). Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Scene();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Scene();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Scene(const Scene& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Scene& operator=(const Scene& rhs);

  /**
   * @brief This move constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the moved handle
   */
  Scene(Scene&& handle) noexcept;

  /**
   * @brief This move assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the moved handle
   * @return A reference to this
   */
  Scene& operator=(Scene&& rhs) noexcept;

  /**
   * @brief Adds a child Actor to the Scene.
   *
   * The child will be referenced.
   * @param[in] actor The child
   * @pre The actor has been initialized.
   * @pre The actor does not have a parent.
   */
  void Add(Actor actor);

  /**
   * @brief Removes a child Actor from the Scene.
   *
   * The child will be unreferenced.
   * @param[in] actor The child
   * @pre The actor has been added to the stage.
   */
  void Remove(Actor actor);

  /**
   * @brief Returns the size of the Scene in pixels as a Vector.
   *
   * The x component will be the width of the Scene in pixels.
   * The y component will be the height of the Scene in pixels.
   *
   * @return The size of the Scene as a Vector
   */
  Size GetSize() const;

  /**
   * Sets horizontal and vertical pixels per inch value that is used by the display
   * @param[in] dpi Horizontal and vertical dpi value
   */
  void SetDpi(Vector2 dpi);

  /**
   * @brief Retrieves the DPI of the display device to which the scene is connected.
   *
   * @return The horizontal and vertical DPI
   */
  Vector2 GetDpi() const;

  /**
   * @brief Sets the background color.
   *
   * @param[in] color The new background color
   */
  void SetBackgroundColor(const Vector4& color);

  /**
   * @brief Gets the background color of the render　surface.
   *
   * @return The background color
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @brief Retrieves the list of render-tasks.
   *
   * @return A valid handle to a RenderTaskList
   */
  Dali::RenderTaskList GetRenderTaskList() const;

  /**
   * @brief Returns the Scene's Root Layer.
   *
   * @return The root layer
   */
  Layer GetRootLayer() const;

  /**
   * @brief Returns the Scene's Overlay Layer.
   * If there is no overlay layer yet, this creates the layer and an associated render task.
   *
   * @return The overlay layer
   */
  Layer GetOverlayLayer();

  /**
   * @brief Queries the number of on-stage layers.
   *
   * Note that a default layer is always provided (count >= 1).
   * @return The number of layers
   */
  uint32_t GetLayerCount() const;

  /**
   * @brief Retrieves the layer at a specified depth.
   *
   * @param[in] depth The depth
   * @return The layer found at the given depth
   * @pre Depth is less than layer count; see GetLayerCount().
   */
  Layer GetLayer(uint32_t depth) const;

  /**
   * @brief Informs the scene that the set surface has been resized.
   *
   * @param[in] width The new width of the set surface
   * @param[in] height The new height of the set surface
   */
  void SurfaceResized(float width, float height);

  /**
   * @brief Informs the scene that the surface has been replaced.
   */
  void SurfaceReplaced();

  /**
   * @brief Removes the scene graph object.
   */
  void RemoveSceneObject();

  /**
   * @brief Discards this Scene from the Core.
   */
  void Discard();

  /**
   * @brief Sets the render target for the surface.
   *
   * @param[in] renderTarget The render target create info for the surface
   */
  void SetSurfaceRenderTarget(const Graphics::RenderTargetCreateInfo& createInfo);

  /**
   * @brief Retrieve the Scene that the given actor belongs to.
   * @return The Scene.
   */
  static Integration::Scene Get(Actor actor);

  /**
   * This function is called when an event is queued.
   * @param[in] event A event to queue.
   */
  void QueueEvent(const Integration::Event& event);

  /**
   * This function is called by Core when events are processed.
   */
  void ProcessEvents();

  /**
   * @brief Adds a callback that is called when the frame rendering is done by the graphics driver.
   *
   * @param[in] callback The function to call
   * @param[in] frameId The Id to specify the frame. It will be passed when the callback is called.
   *
   * @note A callback of the following type may be used:
   * @code
   *   void MyFunction( int32_t frameId );
   * @endcode
   * This callback will be deleted once it is called.
   *
   * @note Ownership of the callback is passed onto this class.
   */
  void AddFrameRenderedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId);

  /**
   * @brief Adds a callback that is called when the frame is displayed on the display.
   *
   * @param[in] callback The function to call
   * @param[in] frameId The Id to specify the frame. It will be passed when the callback is called.
   *
   * @note A callback of the following type may be used:
   * @code
   *   void MyFunction( int32_t frameId );
   * @endcode
   * This callback will be deleted once it is called.
   *
   * @note Ownership of the callback is passed onto this class.
   */
  void AddFramePresentedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId);

  /**
   * @brief Gets the callback list that is called when the frame rendering is done by the graphics driver.
   *
   * @param[out] callbacks The callback list
   *
   * @note This is called in the update thread.
   */
  void GetFrameRenderedCallback(FrameCallbackContainer& callbacks);

  /**
   * @brief Gets the callback list that is called when the frame is displayed on the display.
   *
   * @param[out] callbacks The callback list
   *
   * @note This is called in the update thread.
   */
  void GetFramePresentedCallback(FrameCallbackContainer& callbacks);

  /**
   * @brief Informs the scene that the set surface has been rotated.
   *
   * @param[in] width The width of rotated surface
   * @param[in] height The height of rotated surface
   * @param[in] windowOrientation the current window orientation
   * @param[in] screenOrientation the current screen orientation
   */
  void SurfaceRotated(float width, float height, int32_t windowOrientation, int32_t screenOrientation);

  /**
   * @brief Gets the current surface orientation. It gets the value from the scene object.
   *
   * @return The current surface orientation.
   */
  int32_t GetCurrentSurfaceOrientation() const;

  /**
   * @brief Gets the current screen orientation. It gets the value from the scene object.
   *
   * @return The current screen orientation.
   */
  int32_t GetCurrentScreenOrientation() const;

  /**
   * @brief Gets the current surface rectangle. It gets the value from the scene object.
   *
   * @return The current surface rectangle
   */
  const Rect<int32_t>& GetCurrentSurfaceRect() const;

  /**
   * Query how many times the surface rect changed.
   * @note It will reset surface rect changed count.
   * @return The count of the surface rect changed.
   */
  uint32_t GetSurfaceRectChangedCount() const;

  /**
   * @brief Send message to acknowledge for completing window rotation with current window orientation.
   *
   * If this function is called, the message is sent to render thread, then mSurfaceRectChanged in scene-graph-scene is set with true.
   * After that, render thread checks whether window rotation event is received, mSurfaceRectChanged and the neccessary flag are set.
   * If they are all true, rotation done function is called to complete window rotation.
   */
  void SetRotationCompletedAcknowledgement();

  /**
   * @brief Query wheter is set to acknowledge for completing surface rotation.
   * @return true it should be acknowledged.
   *
   * If SetRotationCompletedAcknowledgement() is called and the related message is received to scene-graph-scene,
   * then mSurfaceRectChanged in scene-graph-scene is set with true.
   *
   * When this function is called, the mSurfaceRectChanged in scene-graph-scene is return.
   * Then, the flag will be reset.
   *
   * @note This function should be not called the application's main thread.
   * Because this function should be called in windwow surface's postrender() function to complete window rotation manually.
   */
  bool IsRotationCompletedAcknowledgementSet() const;

  /**
   * @brief Keep rendering for at least the given amount of time.
   *
   * By default, Dali will stop rendering when no Actor positions are being set, and when no animations are running etc.
   * This method is useful to force screen refreshes.
   *
   * @param[in] durationSeconds Time to keep rendering, 0 means render at least one more frame
   */
  void KeepRendering(float durationSeconds);

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
   * @brief Sets whether the processes using geometry event propagation touch and hover events.
   *
   * @param[in] enabled True if the processes using geometry event propagation touch and hover events.
   */
  void SetGeometryHittestEnabled(bool enabled);

  /**
   * @brief Queries whether the scene using geometry event propagation touch and hover events.
   *
   * @return True if the scene using geometry event propagation touch and hover events.
   */
  bool IsGeometryHittestEnabled();

  /**
   * @brief Get the Touch Propagation Type
   *
   * @return TouchPropagationType
   */
  TouchPropagationType GetTouchPropagationType();

  /**
   * @brief Sets the native window id
   *
   * @param nativeId The native window id
   */
  void SetNativeId(int32_t nativeId);

  /**
   * @brief Gets the native window id
   *
   * @return The native window id
   */
  int32_t GetNativeId() const;

  /**
   * @brief Gets the last pan gesture state
   *
   * @return Dali::GestureState
   */
  Dali::GestureState GetLastPanGestureState();

  /**
   * @brief This signal is emitted just after the event processing is finished.
   *
   * @return The signal to connect to
   */
  EventProcessingFinishedSignalType& EventProcessingFinishedSignal();

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const KeyEvent& event);
   * @endcode
   * @return The signal to connect to
   */
  KeyEventSignalType& KeyEventSignal();

  /**
   * @brief The user would connect to this signal to get a KeyEvent when KeyEvent is generated.
   *
   * If the control already consumed key event, KeyEventProcessor do not need to Emit keyEvent.
   * Therefore, KeyinputManager first checks whether KeyEvent is generated as KeyEventGeneratedSignal.
   * After that keyEventProcessor must invoke KeyEvent only if KeyEventGeneratedSignal () is not consumed.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(const KeyEvent& event);
   * @endcode
   *
   * @return The return is true if KeyEvent is consumed, otherwise false.
   */
  KeyEventGeneratedSignalType& KeyEventGeneratedSignal();

  /**
   * @brief The user would connect to this signal to intercept a KeyEvent at window.
   *
   * Intercepts KeyEvents in the window before dispatching KeyEvents to the control.
   * If a KeyEvent is consumed, no KeyEvent is delivered to the control.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(const KeyEvent& event);
   * @endcode
   *
   * @return The return is true if KeyEvent is consumed, otherwise false.
   */
  KeyEventGeneratedSignalType& InterceptKeyEventSignal();

  /**
   * @brief This signal is emitted when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
   *
   * If there are multiple touch points, then this will be emitted when the first touch occurs and
   * then when the last finger is lifted.
   * An interrupted event will also be emitted (if it occurs).
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( TouchEvent event );
   * @endcode
   *
   * @return The touch signal to connect to
   * @note Motion events are not emitted.
   */
  TouchEventSignalType& TouchedSignal();

  /**
   * @brief This signal is emitted when wheel event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const WheelEvent& event);
   * @endcode
   * @return The signal to connect to
   */
  WheelEventSignalType& WheelEventSignal();

  /**
   * @brief When a custom wheel event occurs, it need to process the focused actor first.
   *
   * Therefore, KeyboardFocusManager first checks whether WheelEvent is generated as WheelEventGeneratedSignal.
   * After that wheelEventProcessor must invoke WheelEvent only if wheelEventGeneratedSignal () is not consumed.
   *
   * This is only valid for custom wheel events.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(const WheelEvent& event);
   * @endcode
   *
   * @return The return is true if WheelEvent is consumed, otherwise false.
   */
  WheelEventGeneratedSignalType& WheelEventGeneratedSignal();

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali::New() methods.
   *
   * @param[in] scene A pointer to an internal Scene resource
   */
  explicit DALI_INTERNAL Scene(Internal::Scene* scene);
};

} // namespace Integration

} // namespace Dali

#endif // DALI_SCENE_H
