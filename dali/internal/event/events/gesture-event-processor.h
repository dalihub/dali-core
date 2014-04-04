#ifndef __DALI_INTERNAL_GESTURE_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_GESTURE_EVENT_PROCESSOR_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/internal/event/events/gesture-detector-impl.h>
#include <dali/internal/event/events/long-press-gesture-processor.h>
#include <dali/internal/event/events/pan-gesture-processor.h>
#include <dali/internal/event/events/pinch-gesture-processor.h>
#include <dali/internal/event/events/tap-gesture-processor.h>

namespace Dali
{

struct Gesture;

namespace Integration
{
struct GestureEvent;
class GestureManager;
class RenderController;
}

namespace Internal
{

class Stage;

/**
 * Gesture Event Processing:
 *
 * The GestureEventProcessor determines what type of gesture event we have received and sends it to
 * the appropriate gesture processor for processing.
 */
class GestureEventProcessor
{
public:

  /**
   * Create a gesture event processor.
   * @param[in] stage The stage.
   * @param[in] gestureManager The gesture manager
   * @param[in] renderController The render controller
   */
  GestureEventProcessor(Stage& stage, Integration::GestureManager& gestureManager, Integration::RenderController& renderController);

  /**
   * Non-virtual destructor; GestureProcessor is not a base class
   */
  ~GestureEventProcessor();

public: // To be called by EventProcessor

  /**
   * This function is called by Core whenever a gesture event occurs.
   * @param[in] event The event that has occurred.
   */
  void ProcessGestureEvent(const Integration::GestureEvent& event);

public: // To be called by gesture detectors

  /**
   * This method adds the specified gesture detector to the relevant gesture processor.
   * @param[in]  gestureDetector  The gesture detector to add
   */
  void AddGestureDetector(GestureDetector* gestureDetector);

  /**
   * This method removes the specified gesture detector from the relevant gesture processor.
   * @param[in]  gestureDetector  The gesture detector to remove.
   */
  void RemoveGestureDetector(GestureDetector* gestureDetector);

  /**
   * This method informs the appropriate processor that the gesture detector has been updated.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated(GestureDetector* gestureDetector);

  /**
   * This method is called by GestureDetectors on Started or Continue state events.
   * Status is queried and reset by Core in ProcessEvents
   */
  void SetUpdateRequired();

  /**
   * Called by GestureDetectors to set the gesture properties in the update thread.
   * @param[in]  gesture  The gesture whose values will be used in the Update object.
   * @note If we are in the middle of processing the gesture being set, then this call is ignored.
   */
  void SetGestureProperties( const Gesture& gesture );

public: // Called by Core

  /**
   * Returns true if any GestureDetector requires a Core::Update. Clears
   * the state flag after reading.
   *
   * @return true if any GestureDetector requires a Core::Update
   */
  bool NeedsUpdate();

  /**
   * Called to provide pan-gesture profiling information.
   */
  void EnablePanGestureProfiling();

private:

  // Undefined
  GestureEventProcessor(const GestureEventProcessor&);
  GestureEventProcessor& operator=(const GestureEventProcessor& rhs);

private:

  Stage& mStage;
  Integration::GestureManager& mGestureManager;

  LongPressGestureProcessor mLongPressGestureProcessor;
  PanGestureProcessor mPanGestureProcessor;
  PinchGestureProcessor mPinchGestureProcessor;
  TapGestureProcessor mTapGestureProcessor;
  Integration::RenderController& mRenderController;

  bool mUpdateRequired;     ///< set to true by gesture detectors if they require a Core::Update
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GESTURE_EVENT_PROCESSOR_H__
