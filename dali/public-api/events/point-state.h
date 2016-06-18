#ifndef __DALI_POINT_STATE_H__
#define __DALI_POINT_STATE_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief Point state
 * @SINCE_1_1.37
 */
namespace PointState
{

/**
 * @brief Point state type
 * @SINCE_1_1.37
 */
enum Type
{
  STARTED,        /**< Touch or hover started */
  FINISHED,       /**< Touch or hover finished */
  DOWN = STARTED, /**< Screen touched */
  UP = FINISHED,  /**< Touch stopped */
  MOTION,         /**< Finger dragged or hovered */
  LEAVE,          /**< Leave the boundary of an actor */
  STATIONARY,     /**< No change from last event.  Useful when a multi-point event occurs where
                       all points are sent but indicates that this particular point has not changed
                       since the last time */
  INTERRUPTED,    /**< A system event has occurred which has interrupted the touch or hover event sequence. */
};
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_POINT_H__
