#ifndef DALI_SCENE_H
#define DALI_SCENE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/handle.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

class Actor;
struct KeyEvent;
class Layer;
class RenderTaskList;
class TouchData;
struct WheelEvent;

namespace Internal DALI_INTERNAL
{
  class Scene;
}

namespace Integration
{

class RenderSurface;
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
  typedef Signal< void () > EventProcessingFinishedSignalType; ///< Event Processing finished signal type
  typedef Signal< void (const Dali::KeyEvent&) > KeyEventSignalType; ///< Key event signal type
  typedef Signal< void (const Dali::TouchData&) > TouchSignalType; ///< Touch signal type
  typedef Signal< void (const Dali::WheelEvent&) > WheelEventSignalType; ///< Touched signal type

  /**
   * @brief Create an initialized Scene handle.
   *
   * @param[in] size The size of the scene in pixels as a Vector
   *
   * @return a handle to a newly allocated Dali resource.
   */
  static Scene New( const Size& size );

  /**
   * @brief Downcast an Object handle to Scene handle.
   *
   * If handle points to a Scene object the downcast produces
   * valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a Scene object or an uninitialized handle
   */
  static Scene DownCast( BaseHandle handle );

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
   * @brief Adds a child Actor to the Scene.
   *
   * The child will be referenced.
   * @param[in] actor The child
   * @pre The actor has been initialized.
   * @pre The actor does not have a parent.
   */
  void Add(Actor& actor);

  /**
   * @brief Removes a child Actor from the Scene.
   *
   * The child will be unreferenced.
   * @param[in] actor The child
   * @pre The actor has been added to the stage.
   */
  void Remove(Actor& actor);

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
  void SetDpi( Vector2 dpi );

  /**
   * @brief Retrieves the DPI of the display device to which the scene is connected.
   *
   * @return The horizontal and vertical DPI
   */
  Vector2 GetDpi() const;

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
  Layer GetLayer( uint32_t depth ) const;

  /**
   * @brief Binds the rendering surface to the scene
   *
   * @return The root layer
   */
  void SetSurface( Integration::RenderSurface& surface );

  /**
   * @brief Gets the rendering surface bound to the scene
   *
   * @return The render surface
   */
  Integration::RenderSurface* GetSurface() const;

  /**
   * This function is called when an event is queued.
   * @param[in] event A event to queue.
   */
  void QueueEvent( const Integration::Event& event );

  /**
   * This function is called by Core when events are processed.
   */
  void ProcessEvents();

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
   * @return The touch signal to connect to
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
   * @return The signal to connect to
   */
  WheelEventSignalType& WheelEventSignal();

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
