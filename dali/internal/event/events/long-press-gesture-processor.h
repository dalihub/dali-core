#ifndef __DALI_INTERNAL_LONG_PRESS_GESTURE_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_LONG_PRESS_GESTURE_EVENT_PROCESSOR_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/internal/event/events/long-press-gesture-detector-impl.h>
#include <dali/internal/event/events/gesture-processor.h>

namespace Dali
{

namespace Integration
{
class GestureManager;
struct GestureEvent;
struct LongPressGestureEvent;
}

namespace Internal
{

class Stage;

/**
 * Long Press Gesture Event Processing:
 *
 * When we receive a long press gesture event, we do the following:
 * - Determine the hit actor underneath the long press gesture event.
 * - Determine whether this actor is attached to any of the detectors or is a child of an actor
 *   attached to one of the detectors.
 * - Ensure the touches in the long press event match the requirements of the detector.
 * - Emit the gesture when all the above conditions are met.
 */
class LongPressGestureProcessor : public GestureProcessor
{
public:

  /**
   * Create a long press gesture processor.
   * @param[in] stage The stage.
   * @param[in] gestureManager The gesture manager.
   */
  LongPressGestureProcessor( Stage& stage, Integration::GestureManager& gestureManager );

  /**
   * Non-virtual destructor; LongPressGestureProcessor is not a base class
   */
  ~LongPressGestureProcessor();

public: // To be called by GestureEventProcessor

  /**
   * This method is called whenever a long press gesture event occurs.
   * @param[in] longPressEvent The event that has occurred.
   */
  void Process( const Integration::LongPressGestureEvent& longPressEvent );

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added.
   */
  void AddGestureDetector( LongPressGestureDetector* gestureDetector );

  /**
   * Removes the specified gesture detector from this gesture processor.  If, after removing this
   * gesture detector, there are no more gesture detectors registered, then this method unregisters
   * the gesture from the adaptor.
   * @param[in]  gestureDetector  The gesture detector being removed.
   */
  void RemoveGestureDetector( LongPressGestureDetector* gestureDetector );

  /**
   * This method updates the gesture detection parameters.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated(LongPressGestureDetector* gestureDetector);

private:

  // Undefined
  LongPressGestureProcessor( const LongPressGestureProcessor& );
  LongPressGestureProcessor& operator=( const LongPressGestureProcessor& rhs );

private:

  /**
   * Iterates through our GestureDetectors and determines if we need to ask the adaptor to update
   * its detection policy.  If it does, it sends the appropriate gesture update request to adaptor.
   */
  void UpdateDetection();

  // GestureProcessor overrides

  /**
   * @copydoc GestureProcessor::OnGesturedActorStageDisconnection()
   */
  void OnGesturedActorStageDisconnection();

private:

  Stage& mStage;
  Integration::GestureManager& mGestureManager;
  LongPressGestureDetectorContainer mGestureDetectors;

  LongPressGestureDetectorContainer mCurrentEmitters;
  Dali::RenderTask mCurrentRenderTask;

  unsigned int mMinTouchesRequired;
  unsigned int mMaxTouchesRequired;

  struct LongPressEventFunctor;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_LONG_PRESS_GESTURE_EVENT_PROCESSOR_H__
