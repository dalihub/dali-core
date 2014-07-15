#ifndef __DALI_INTERNAL_TAP_GESTURE_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_TAP_GESTURE_EVENT_PROCESSOR_H__

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
#include <dali/internal/event/events/tap-gesture-detector-impl.h>
#include <dali/internal/event/events/gesture-processor.h>

namespace Dali
{

namespace Integration
{
class GestureManager;
struct GestureEvent;
struct TapGestureEvent;
}

namespace Internal
{

class Stage;
class Actor;

/**
 * Tap Gesture Event Processing:
 *
 * When we receive a tap gesture event, we do the following:
 * - Find the actor that requires a tap where the tap occurred.
 * - Emit the gesture if the tap gesture event satisfies the detector conditions.
 */
class TapGestureProcessor : public GestureProcessor
{
public:

  /**
   * Create a tap gesture processor.
   * @param[in] stage The stage.
   * @param[in] gestureManager The gesture manager.
   */
  TapGestureProcessor(Stage& stage, Integration::GestureManager& gestureManager);

  /**
   * Non-virtual destructor; TapGestureProcessor is not a base class
   */
  ~TapGestureProcessor();

public: // To be called by GestureEventProcessor

  /**
   * This method is called whenever a tap gesture event occurs.
   * @param[in] tapEvent The event that has occurred.
   */
  void Process(const Integration::TapGestureEvent& tapEvent);

  /**
   * Adds a gesture detector to this gesture processor.
   * If this is the first gesture detector being added, then this method registers the required
   * gesture with the adaptor.
   * @param[in]  gestureDetector  The gesture detector being added.
   */
  void AddGestureDetector(TapGestureDetector* gestureDetector);

  /**
   * Removes the specified gesture detector from this gesture processor.  If, after removing this
   * gesture detector, there are no more gesture detectors registered, then this method unregisters
   * the gesture from the adaptor.
   * @param[in]  gestureDetector  The gesture detector being removed.
   */
  void RemoveGestureDetector(TapGestureDetector* gestureDetector);

  /**
   * This method updates the gesture detection parameters.
   * @param[in]  gestureDetector  The gesture detector that has been updated.
   */
  void GestureDetectorUpdated(TapGestureDetector* gestureDetector);

private:

  // Undefined
  TapGestureProcessor(const TapGestureProcessor&);
  TapGestureProcessor& operator=(const TapGestureProcessor& rhs);

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
  void OnGesturedActorStageDisconnection() { /* Nothing to do */ }

  /**
   * @copydoc GestureProcessor::CheckGestureDetector()
   */
  bool CheckGestureDetector( GestureDetector* detector, Actor* actor );

  /**
   * @copydoc GestureProcessor::EmitGestureSignal()
   */
  void EmitGestureSignal( Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates );

private:

  Stage& mStage;
  Integration::GestureManager& mGestureManager;
  TapGestureDetectorContainer mGestureDetectors;

  unsigned int mMinTapsRequired;
  unsigned int mMaxTapsRequired;
  unsigned int mMinTouchesRequired;
  unsigned int mMaxTouchesRequired;

  const Integration::TapGestureEvent* mCurrentTapEvent; ///< Pointer to current TapEvent, used when calling ProcessAndEmit()
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TAP_GESTURE_EVENT_PROCESSOR_H__
