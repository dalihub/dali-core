#ifndef __DALI_TOUCH_POINT_H__
#define __DALI_TOUCH_POINT_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief A TouchPoint represents a point on the screen that is currently being touched
 * or where touch has stopped.
 * @SINCE_1_0.0
 */
struct DALI_CORE_API TouchPoint
{
  // Enumerations

  /**
   * @brief Enumeration for Touch state.
   * @SINCE_1_0.0
   */
  enum State
  {
    Started,        /**< Touch or hover started */
    Finished,       /**< Touch or hover finished */
    Down = Started, /**< Screen touched */
    Up = Finished,  /**< Touch stopped */
    Motion,         /**< Finger dragged or hovered */
    Leave,          /**< Leave the boundary of an actor */
    Stationary,     /**< No change from last event.  Useful when a multi-point event occurs where
                         all points are sent but indicates that this particular point has not changed
                         since the last time */
    Interrupted,    /**< A system event has occurred which has interrupted the touch or hover event sequence. */
    Last            /**< Number of states. */
  };

  // Construction & Destruction

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] id      The touch device ID
   * @param[in] state   The state
   * @param[in] screenX The X co-ordinate relative to the screen's origin
   * @param[in] screenY The Y co-ordinate relative to the screen's origin
   */
  TouchPoint(int32_t id, State state, float screenX, float screenY);

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] id      The touch device ID
   * @param[in] state   The state
   * @param[in] screenX The X co-ordinate relative to the screen's origin
   * @param[in] screenY The Y co-ordinate relative to the screen's origin
   * @param[in] localX  The X co-ordinate relative to the top-left (0.0, 0.0, 0.5) of the actor
   * @param[in] localY  The Y co-ordinate relative to the top-left (0.0, 0.0, 0.5) of the actor
   */
  TouchPoint(int32_t id, State state, float screenX, float screenY, float localX, float localY);

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  ~TouchPoint();

  // Data

  /**
   * @brief Each touch point has a unique device ID which specifies the touch device for that point.
   */
  int32_t deviceId;

  /**
   * @brief State of the point.
   *
   * @see State
   */
  State state;

  /**
   * @brief The actor that was underneath the touch point.
   */
  Actor hitActor;

  /**
   * @brief The co-ordinates relative to the top-left of the hit-actor.
   *
   * @note The top-left of an actor is (0.0, 0.0, 0.5).
   * @note If you require the local coordinates of another actor (e.g the parent of the hit actor),
   * then you should use Actor::ScreenToLocal().
   */
  Vector2 local;

  /**
   * @brief The co-ordinates relative to the top-left of the screen.
   */
  Vector2 screen;
};

typedef std::vector<TouchPoint> TouchPointContainer; ///< Container of touch points. @SINCE_1_0.0
typedef TouchPointContainer::iterator TouchPointContainerIterator; ///< Iterator for Dali::TouchPointContainer @SINCE_1_0.0
typedef TouchPointContainer::const_iterator TouchPointContainerConstIterator; ///< Const iterator for Dali::TouchPointContainer @SINCE_1_0.0

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_TOUCH_POINT_H__
