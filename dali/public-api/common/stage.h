#ifndef __DALI_STAGE_H__
#define __DALI_STAGE_H__

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

namespace Internal DALI_INTERNAL
{
class Stage;
}

class Actor;
class Layer;
class ObjectRegistry;
class TouchData;
class RenderTaskList;
struct Vector2;
struct Vector3;
struct Vector4;
struct KeyEvent;
struct TouchEvent;
struct WheelEvent;

/**
 * @brief The Stage is a top-level object used for displaying a tree of Actors.
 *
 * Stage is a top-level object that represents the entire screen.
 * It is used for displaying a hierarchy of actors managed by the scene graph structure,
 * which means an actor inherits a position relative to its parent,
 * and can be moved in relation to this point.
 *
 * The stage instance is a singleton object (the only instance of its class during the
 * lifetime of the program). You can get it using a static function.
 *
 * To display the contents of an actor, it must be added to a stage.
 * The following example shows how to connect a new actor to the stage:
 * @code
 * Actor actor = Actor::New();
 * Stage::GetCurrent().Add( actor );
 * @endcode
 *
 * The stage has a 2D size that matches the size of the application window.
 * The default unit 1 is 1 pixel with the default camera.
 *
 * Multiple stage/window support is not currently provided.
 *
 * Signals
 * | %Signal Name            | Method                               |
 * |-------------------------|--------------------------------------|
 * | keyEvent                | @ref KeyEventSignal()                |
 * | eventProcessingFinished | @ref EventProcessingFinishedSignal() |
 * | touched                 | @ref TouchedSignal()                 |
 * | wheelEvent              | @ref WheelEventSignal()              |
 * | contextLost             | @ref ContextLostSignal()             |
 * | contextRegained         | @ref ContextRegainedSignal()         |
 * | sceneCreated            | @ref SceneCreatedSignal()            |
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Stage : public BaseHandle
{
public:

  typedef Signal< void (const KeyEvent&) > KeyEventSignalType;       ///< Key event signal type @SINCE_1_0.0
  typedef Signal< void () > EventProcessingFinishedSignalType;       ///< Event Processing finished signal type @SINCE_1_0.0
  typedef Signal< void (const TouchEvent&) > TouchedSignalType;      ///< @DEPRECATED_1_1.37 @brief Touched signal type @SINCE_1_0.0
  typedef Signal< void (const TouchData&) > TouchSignalType;                ///< Touch signal type @SINCE_1_1.37
  typedef Signal< void (const WheelEvent&) > WheelEventSignalType;   ///< Touched signal type @SINCE_1_0.0
  typedef Signal< void () > ContextStatusSignal;                     ///< Context status signal type @SINCE_1_0.0
  typedef Signal< void () > SceneCreatedSignalType;                  ///< Scene created signal type @SINCE_1_0.0

  static const Vector4 DEFAULT_BACKGROUND_COLOR; ///< Default black background.
  static const Vector4 DEBUG_BACKGROUND_COLOR;   ///< Green background, useful when debugging.

  /**
   * @brief Allows the creation of an empty stage handle.
   *
   * To retrieve the current stage, this handle can be set using Stage::GetCurrent().
   * @SINCE_1_0.0
   */
  Stage();

  /**
   * @brief Get the current Stage.
   *
   * @SINCE_1_0.0
   * @return The current stage or an empty handle if the internal core has not been created or has been already destroyed.
   */
  static Stage GetCurrent();

  /**
   * @brief Query whether the Stage exists; this should only return false during or after destruction of Dali core.
   *
   * @SINCE_1_0.0
   * @return True when it's safe to call Stage::GetCurrent().
   */
  static bool IsInstalled();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Stage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  Stage(const Stage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Stage& operator=(const Stage& rhs);

  // Containment

  /**
   * @brief Adds a child Actor to the Stage.
   *
   * The child will be referenced.
   * @SINCE_1_0.0
   * @param [in] actor The child.
   * @pre The actor has been initialized.
   * @pre The actor does not have a parent.
   */
  void Add(Actor& actor);

  /**
   * @brief Removes a child Actor from the Stage.
   *
   * The child will be unreferenced.
   * @SINCE_1_0.0
   * @param [in] actor The child.
   * @pre The actor has been added to the stage.
   */
  void Remove(Actor& actor);

  /**
   * @brief Returns the size of the Stage in pixels as a Vector.
   *
   * The x component will be the width of the Stage in pixels
   * The y component will be the height of the Stage in pixels
   * The z component will be the distance between far and near planes
   * @SINCE_1_0.0
   * @return The size of the Stage as a Vector.
   */
  Vector2 GetSize() const;

  // Render Tasks

  /**
   * @brief Retrieve the list of render-tasks.
   *
   * @SINCE_1_0.0
   * @return A valid handle to a RenderTaskList.
   */
  RenderTaskList GetRenderTaskList() const;

  // Layers

  /**
   * @brief Query the number of on-stage layers.
   *
   * Note that a default layer is always provided (count >= 1).
   * @SINCE_1_0.0
   * @return The number of layers.
   */
  unsigned int GetLayerCount() const;

  /**
   * @brief Retrieve the layer at a specified depth.
   *
   * @SINCE_1_0.0
   * @param[in] depth The depth.
   * @return The layer found at the given depth.
   * @pre Depth is less than layer count; see GetLayerCount().
   */
  Layer GetLayer(unsigned int depth) const;

  /**
   * @brief Returns the Stage's Root Layer.
   *
   * @SINCE_1_0.0
   * @return The root layer.
   */
  Layer GetRootLayer() const;

  // Background color

  /**
   * @brief Set the background color of the stage.
   *
   * @SINCE_1_0.0
   * @param[in] color The new background color.
   */
  void SetBackgroundColor(Vector4 color);

  /**
   * @brief Retrieve the background color of the stage.
   *
   * @SINCE_1_0.0
   * @return The background color.
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @brief Retrieve the DPI of the display device to which the stage is connected.
   *
   * @SINCE_1_0.0
   * @return The horizontal and vertical DPI
   */
  Vector2 GetDpi() const;

  /**
   * @brief Get the Object registry.
   *
   * @SINCE_1_0.0
   * @return The object registry.
   */
  ObjectRegistry GetObjectRegistry() const;

  // Rendering

  /**
   * @brief Keep rendering for at least the given amount of time.
   *
   * By default Dali will stop rendering when no Actor positions are being set, and when no animations are running etc.
   * This method is useful to force screen refreshes e.g. when updating a NativeImage.
   * @SINCE_1_0.0
   * @param[in] durationSeconds Time to keep rendering, 0 means render at least one more frame
   */
  void KeepRendering( float durationSeconds );

  // Signals

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const KeyEvent& event);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   */
  KeyEventSignalType& KeyEventSignal();

  /**
   * @brief This signal is emitted just after the event processing is finished.
   *
   * @SINCE_1_0.0
   * @return The signal to connect to.
   */
  EventProcessingFinishedSignalType& EventProcessingFinishedSignal();

  /**
   * @DEPRECATED_1_1.37 Use TouchSignal() instead.
   * @brief This signal is emitted when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
   *
   * If there are multiple touch points, then this will be emitted when the first touch occurs and
   * then when the last finger is lifted.
   * An interrupted event will also be emitted (if it occurs).
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const TouchEvent& event);
   * @endcode
   *
   * @SINCE_1_0.0
   * @return The touch signal to connect to.
   * @note Motion events are not emitted.
   */
  TouchedSignalType& TouchedSignal();

  /**
   * @brief This signal is emitted when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
   *
   * If there are multiple touch points, then this will be emitted when the first touch occurs and
   * then when the last finger is lifted.
   * An interrupted event will also be emitted (if it occurs).
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( TouchData event );
   * @endcode
   *
   * @SINCE_1_1.37
   * @return The touch signal to connect to.
   * @note Motion events are not emitted.
   */
  TouchSignalType& TouchSignal();

  /**
   * @brief This signal is emitted when wheel event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const WheelEvent& event);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   */
  WheelEventSignalType& WheelEventSignal();

  /**
   * @brief This signal is emitted when the GL context is lost (Platform specific behaviour).
   *
   * If the application is responsible for handling context loss, it should listen to
   * this signal and tear down UI components when recieved.
   * @SINCE_1_0.0
   * @return The context lost signal to connect to.
   */
  ContextStatusSignal& ContextLostSignal();

  /**
   * @brief This signal is emitted when the GL context is regained (Platform specific
   * behaviour).
   *
   * If the application is responsible for handling context loss, it should listen to
   * this signal and rebuild UI components on receipt.
   * @SINCE_1_0.0
   * @return The context regained signal to connect to.
   */
  ContextStatusSignal& ContextRegainedSignal();

  /**
   * @brief This signal is emitted after the initial scene is created.
   *
   * It will be triggered after the
   * application init signal.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName();
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to.
   */
  SceneCreatedSignalType& SceneCreatedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Stage::GetCurrent() methods.
   *
   * @SINCE_1_0.0
   * @param [in] stage A pointer to a Dali resource
   */
  explicit DALI_INTERNAL Stage(Internal::Stage* stage);
  /// @endcond
};

/**
 * @}
 */

} // namespace Dali

#endif // __DALI_STAGE_H__
