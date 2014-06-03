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
protected: // Construction & Destruction

  /**
   * Protected constructor.  Cannot create an instance of GestureProcessor
   */
  GestureProcessor( Gesture::Type type );

  /**
   * Virtual protected destructor.
   */
  virtual ~GestureProcessor();

  // Templates and types for deriving classes

  /**
   * Given a container of derived pointer types, this populates an equivalent container of base pointer types.
   * @param[in]   derivedContainer  A const reference to the container with pointers to the derived class.
   * @param[out]  baseContainer     A reference to the container to populate with equivalent pointers to the base class.
   * @pre Ensure the baseContainer is empty.
   */
  template< typename Detector >
  static void UpCastContainer( const typename DerivedGestureDetectorContainer<Detector>::type& derivedContainer, GestureDetectorContainer& baseContainer )
  {
    baseContainer.assign( derivedContainer.begin(), derivedContainer.end() );
  }

  /**
   * Given a container of base pointer types, this populates an equivalent container of derived pointer types.
   * @param[in]   baseContainer     A const reference to the container with pointers to the base class.
   * @param[out]  derivedContainer  A reference to the container to populate with equivalent pointers to the derived class.
   * @pre Ensure the derivedContainer is empty.
   */
  template< typename Detector >
  static void DownCastContainer( const GestureDetectorContainer& baseContainer, typename DerivedGestureDetectorContainer<Detector>::type& derivedContainer )
  {
    for ( GestureDetectorContainer::const_iterator iter = baseContainer.begin(), endIter = baseContainer.end(); iter != endIter; ++iter )
    {
      derivedContainer.push_back( static_cast< Detector* >( *iter ) );
    }
  }

  /**
   * Functor to use in GetGesturedActor() and ProcessAndEmit() methods.
   */
  struct Functor
  {
    /**
     * This operator should be overridden to check if the gesture detector meets the parameters of the current gesture.
     * @param[in]  detector  The gesture detector to check.
     * @param[in]  actor     The actor that has been gestured.
     * @return true, if it meets the parameters, false otherwise.
     */
    virtual bool operator() ( GestureDetector* detector, Actor* actor ) = 0;

    /**
     * This operator should be overridden to emit the gesture signal on the provided container of gesture detectors along with the actor
     * the gesture has occurred on.
     * @param[in]  actor             The actor which has been gestured.
     * @param[in]  gestureDetectors  The detectors that should emit the signal.
     * @param[in]  actorCoordinates  The local actor coordinates where the gesture took place.
     */
    virtual void operator() ( Dali::Actor actor, const GestureDetectorContainer& gestureDetectors, Vector2 actorCoordinates ) = 0;
  };

  // Methods for deriving classes

  /**
   * Given the hit actor and attached detectors, this walks up the actor tree to determine the actor that is connected to one (or several) gesture
   * detectors. The functor is used to check whether the gesture falls within the gesture detector's parameters.
   * Derived classes need to provide this functor.
   *
   * @param[in,out]  actor               The hit actor. When this function returns, this is the actor that has been hit by the gesture.
   * @param[in]      connectedDetectors  Reference to the detectors connected to the derived processor
   * @param[out]     gestureDetectors    A container containing all the gesture detectors that have the hit actor attached and satisfy the functor parameters.
   * @param[in]      functor             A reference to Functor.  The operator() (GestureDetector*) should be used to check if the connected gesture detector
   *                                     meets the current gesture's parameters.
   */
  void GetGesturedActor( Dali::Actor& actor, const GestureDetectorContainer& connectedDetectors, GestureDetectorContainer& gestureDetectors, Functor& functor );

  /**
   * This does what GetGesturedActor() does but also starts emission of the gesture (using the functor).
   *
   * @param[in]  hitTestResults      The Hit Test Results.
   * @param[in]  connectedDetectors  The detectors attached to the gesture processor.
   * @param[in]  functor             A reference to the functor which should provide an operator() to check if detector satisfies current gesture and another
   *                                 operator() which will be called when all conditions are met and the gesture should be emitted for the actor and detectors.
   *
   * @pre Hit Testing should already be done.
   */
  void ProcessAndEmit( const HitTestAlgorithm::Results& hitTestResults, const GestureDetectorContainer& connectedDetectors, Functor& functor );

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
  void SetActor( Dali::Actor actor );

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

  // For derived classes to override

  /**
   * Called when the gestured actor is removed from the stage.
   */
  virtual void OnGesturedActorStageDisconnection() = 0;

private:

  // Undefined
  GestureProcessor( const GestureProcessor& );
  GestureProcessor& operator=( const GestureProcessor& );

private:

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

  // Signal Handlers

  /**
   * Signal handler called when the actor is removed from the stage.
   * @param[in]  actor  The actor removed from the stage.
   */
  void OnStageDisconnection( Dali::Actor actor );

private: // Data

  Gesture::Type mType;                 ///< Type of GestureProcessor
  Actor* mCurrentGesturedActor;        ///< The current actor that has been gestured.
  bool   mGesturedActorDisconnected:1; ///< Indicates whether the gestured actor has been disconnected from the scene
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GESTURE_PROCESSOR_H__
