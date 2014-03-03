#ifndef __DALI_TOUCH_POINT_H__
#define __DALI_TOUCH_POINT_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

/**
 * A TouchPoint represents a point on the screen that is currently being touched or where touch
 * has stopped.
 */
struct TouchPoint
{
  // Enumerations

  enum State
  {
    Down,        /**< Screen touched */
    Up,          /**< Touch stopped */
    Motion,      /**< Finger dragged */
    Leave,       /**< Leave the boundary of an actor */
    Stationary,  /**< No change from last event.  Useful when a multi-touch event occurs where
                      all points are sent but indicates that this particular point has not changed
                      since the last time */
    Interrupted, /**< A system event has occurred which has interrupted the touch event sequence. */
    Last
  };

  // Construction & Destruction

  /**
   * Constructor
   * @param[in]  id       The touch device ID.
   * @param[in]  state    The state.
   * @param[in]  screenX  The X co-ordinate relative to the screen's origin.
   * @param[in]  screenY  The Y co-ordinate relative to the screen's origin.
   */
  TouchPoint(int id, State state, float screenX, float screenY);

  /**
   * Constructor
   * @param[in]  id       The touch device ID.
   * @param[in]  state    The state.
   * @param[in]  screenX  The X co-ordinate relative to the screen's origin.
   * @param[in]  screenY  The Y co-ordinate relative to the screen's origin.
   * @param[in]  localX   The X co-ordinate relative to the top-left (0.0, 0.0, 0.5) of the actor.
   * @param[in]  localY   The Y co-ordinate relative to the top-left (0.0, 0.0, 0.5) of the actor.
   */
  TouchPoint(int id, State state, float screenX, float screenY, float localX, float localY);

  /**
   * Destructor
   */
  ~TouchPoint();

  // Data

  /**
   * Each touch point has a unique device ID which specifies the touch device for that point.
   */
  int deviceId;

  /**
   * State of the point.
   * @see State
   */
  State state;

  /**
   * The actor that was underneath the touch point.
   */
  Actor hitActor;

  /**
   * The co-ordinates relative to the top-left of the hit-actor
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   * @note If you require the local coordinates of another actor (e.g the parent of the hit actor),
   * then you should use Actor::ScreenToLocal().
   */
  Vector2 local;

  /**
   * The co-ordinates relative to the top-left of the screen.
   */
  Vector2 screen;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TOUCH_POINT_H__
