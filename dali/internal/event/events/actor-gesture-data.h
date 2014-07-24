#ifndef __DALI_INTERNAL_ACTOR_GESTURE_DATA_H__
#define __DALI_INTERNAL_ACTOR_GESTURE_DATA_H__

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
#include <dali/internal/event/events/gesture-detector-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * Holds gesture specific data for an Actor
 */
class ActorGestureData
{
public:

  /**
   * Constructor
   */
  ActorGestureData();

  /**
   * Non-virtual Destructor
   */
  ~ActorGestureData();

  /**
   * Adds a gesture detector to the data so that the owning actor is aware that it requires this
   * type of gesture.
   * @param[in] detector The detector being added.
   * @note A raw pointer to the detector is stored, so the detector MUST remove itself when it is
   * destroyed using RemoveGestureDetector()
   */
  void AddGestureDetector( GestureDetector& detector );

  /**
   * Removes a previously added gesture detector from the data. If no more gesture detectors of
   * this type are registered then the actor owning this data will no longer be hit-tested for that
   * gesture.
   * @param[in] detector The detector to remove.
   */
  void RemoveGestureDetector( GestureDetector& detector );

  /**
   * Queries whether the actor requires the gesture type.
   * @param[in] type The gesture type.
   * @return true if the gesture is required, false otherwise.
   */
  inline bool IsGestureRequred( Gesture::Type type ) const
  {
    return type & gesturesRequired;
  }

  /**
   * Retrieve a reference to the detectors for the given type.
   * @param[in] type The container type required
   * @pre Ensure IsGestureRequired() is used to check if the container is actually available.
   */
  GestureDetectorContainer& GetGestureDetectorContainer( Gesture::Type type );

private:

  /**
   * Helper to retrieve the appropriate container type.
   * @param[in] type The container type required.
   */
  inline GestureDetectorContainer*& GetContainerPtr( Gesture::Type type );

private:

  Gesture::Type gesturesRequired; ///< Stores which gestures are required

  GestureDetectorContainer* panDetectors;       ///< Pointer to a container of pan-detectors
  GestureDetectorContainer* pinchDetectors;     ///< Pointer to a container of pinch-detectors
  GestureDetectorContainer* longPressDetectors; ///< Pointer to a container of long-press-detectors
  GestureDetectorContainer* tapDetectors;       ///< Pointer to a container of tap-detectors
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTOR_GESTURE_DATA_H__

