#ifndef __DALI_INTERNAL_GESTURE_PROCESSOR_H__
#define __DALI_INTERNAL_GESTURE_PROCESSOR_H__

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
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/common/proxy-object.h>

namespace Dali
{

class Actor;

namespace Internal
{

/**
 * Base class for the different Gesture Processors.
 */
class GestureProcessor : public ProxyObject::Observer
{
protected:

  // Construction & Destruction

  /**
   * Protected constructor.  Cannot create an instance of GestureProcessor
   */
  GestureProcessor( Gesture::Type type );

  /**
   * Virtual protected destructor.
   */
  virtual ~GestureProcessor();

  // Methods to be used by deriving classes

  /**
   * Given the hit actor, this walks up the actor tree to determine the actor that is connected to one (or several) gesture detectors.
   *
   * @param[in,out]  actor               The gestured actor. When this function returns, this is the actor that has been hit by the gesture.
   * @param[out]     gestureDetectors    A container containing all the gesture detectors that have the hit actor attached and satisfy the functor parameters.
   *
   * @note Uses CheckGestureDetector() to check if a the current gesture matches the criteria the gesture detector requires.
   * @pre gestureDetectors should be empty.
   */
  void GetGesturedActor( Actor*& actor, GestureDetectorContainer& gestureDetectors );

  /**
   * Calls the emission method in the deriving class for matching gesture-detectors with the hit-actor (or one of its parents).
   *
   * @param[in]  hitTestResults      The Hit Test Results.
   *
   * @note Uses the CheckGestureDetector() to check if the gesture matches the criteria of the given gesture detector
   *       and EmitGestureSignal() to emit the signal.
   * @pre Hit Testing should already be done.
   */
  void ProcessAndEmit( HitTestAlgorithm::Results& hitTestResults );

  /**
   * Hit test the screen coordinates, and place the results in hitTestResults.
   * @param[in] stage Stage.
   * @param[in] screenCoordinates The screen coordinates to test.
   * @param[out] hitTestResults Structure to write results into.
   * @return false if the system overlay was hit or no actor was hit.
   */
  virtual bool HitTest(Stage& stage, Vector2 screenCoordinates, HitTestAlgorithm::Results& hitTestResults);

  /**
   * Sets the mCurrentGesturedActor and connects to the required signals.
   * @actor  actor  The actor so set.
   */
  void SetActor( Actor* actor );

  /**
   * Resets the set actor and disconnects any connected signals.
   */
  void ResetActor();

  /**
   * Returns the current gestured actor if it is on stage
   *
   * @return The current gestured actor
   */
  Actor* GetCurrentGesturedActor();

private:

  // For derived classes to override

  /**
   * Called when the gestured actor is removed from the stage.
   */
  virtual void OnGesturedActorStageDisconnection() = 0;

  /**
   * Called by the ProcessAndEmit() & GetGesturedActor() methods to check if the provided
   * gesture-detector meets the parameters of the current gesture.
   *
   * @param[in]  detector  The gesture detector to check.
   * @param[in]  actor     The actor that has been gestured.
   *
   * @return true, if the detector meets the parameters, false otherwise.
   */
  virtual bool CheckGestureDetector( GestureDetector* detector, Actor* actor ) = 0;

  /**
   * Called by the ProcessAndEmit() method when the gesture meets all applicable criteria and
   * should be overridden by deriving classes to emit the gesture signal on gesture-detectors
   * provided for the actor the gesture has occurred on.
   *
   * @param[in]  actor             The actor which has been gestured.
   * @param[in]  gestureDetectors  The detectors that should emit the signal.
   * @param[in]  actorCoordinates  The local actor coordinates where the gesture took place.
   */
  virtual void EmitGestureSignal( Actor* actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates ) = 0;

  // Undefined

  GestureProcessor( const GestureProcessor& );
  GestureProcessor& operator=( const GestureProcessor& );

  // SceneObject overrides

  /**
   * This will never get called as we do not observe objects that have not been added to the scene.
   * @param[in] proxy The proxy object.
   * @see ProxyObject::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded(ProxyObject& proxy) { }

  /**
   * This will be called when the actor is removed from the stage, we should clear and stop
   * observing it.
   * @param[in] proxy The proxy object.
   * @see ProxyObject::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved(ProxyObject& proxy);

  /**
   * This will be called when the actor is destroyed. We should clear the actor.
   * No need to stop observing as the object is being destroyed anyway.
   * @see ProxyObject::Observer::ProxyDestroyed()
   */
  virtual void ProxyDestroyed(ProxyObject& proxy);

private: // Data

  Gesture::Type mType;                 ///< Type of GestureProcessor
  Actor* mCurrentGesturedActor;        ///< The current actor that has been gestured.
  bool   mGesturedActorDisconnected:1; ///< Indicates whether the gestured actor has been disconnected from the scene
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GESTURE_PROCESSOR_H__
