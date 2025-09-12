#ifndef DALI_INTERNAL_PAN_GESTURE_DETECTOR_H
#define DALI_INTERNAL_PAN_GESTURE_DETECTOR_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/pan-gesture/pan-gesture-event.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
class TouchEvent;
struct Radian;

namespace Internal
{
class PanGestureDetector;
using PanGestureDetectorPtr       = IntrusivePtr<PanGestureDetector>;
using PanGestureDetectorContainer = DerivedGestureDetectorContainer<PanGestureDetector>::type;

namespace SceneGraph
{
class PanGesture;
}

/**
 * @copydoc Dali::PanGestureDetector
 */
class PanGestureDetector : public GestureDetector, public RecognizerObserver<PanGestureEvent>
{
public: // Typedefs
  using AngleThresholdPair = Dali::PanGestureDetector::AngleThresholdPair;
  using AngleContainer     = std::vector<AngleThresholdPair>;

public: // Creation
  /**
   * Create a new gesture detector.
   * @return A smart-pointer to the newly allocated detector.
   */
  static PanGestureDetectorPtr New();

public:
  /**
   * @copydoc Dali::PanGestureDetector::SetMinimumTouchesRequired(uint32_t)
   */
  void SetMinimumTouchesRequired(uint32_t minimum);

  /**
   * @copydoc Dali::PanGestureDetector::SetMaximumTouchesRequired(uint32_t)
   */
  void SetMaximumTouchesRequired(uint32_t maximum);

  /**
   * @copydoc Dali::PanGestureDetector::SetMaximumMotionEventAge(uint32_t)
   */
  void SetMaximumMotionEventAge(uint32_t maximumAge);

  /**
   * @copydoc Dali::PanGestureDetector::GetMinimumTouchesRequired() const
   */
  uint32_t GetMinimumTouchesRequired() const;

  /**
   * @copydoc Dali::PanGestureDetector::GetMaximumTouchesRequired() const
   */
  uint32_t GetMaximumTouchesRequired() const;

  /**
   * @copydoc Dali::PanGestureDetector::GetMaximumMotionEventAge() const
   */
  uint32_t GetMaximumMotionEventAge() const;

  /**
   * @copydoc Dali::PanGestureDetector::AddAngle()
   */
  void AddAngle(Radian angle, Radian threshold);

  /**
   * @copydoc Dali::PanGestureDetector::AddDirection()
   */
  void AddDirection(Radian direction, Radian threshold);

  /**
   * @copydoc Dali::PanGestureDetector::GetAngleCount()
   */
  uint32_t GetAngleCount() const;

  /**
   * @copydoc Dali::PanGestureDetector::GetAngle()
   */
  AngleThresholdPair GetAngle(uint32_t index) const;

  /**
   * @copydoc Dali::PanGestureDetector::ClearAngles()
   */
  void ClearAngles();

  /**
   * @copydoc Dali::PanGestureDetector::RemoveAngle()
   */
  void RemoveAngle(Radian angle);

  /**
   * @copydoc Dali::PanGestureDetector::RemoveDirection()
   */
  void RemoveDirection(Radian direction);

  /**
   * Checks whether the pan gesture detector requires a directional pan for emission.
   * @return true, if directional panning required, false otherwise.
   */
  bool RequiresDirectionalPan() const;

  /**
   * Checks whether the given pan angle is allowed for this gesture detector.
   * @param[in]  angle  The angle to check.
   */
  bool CheckAngleAllowed(Radian angle) const;

  /**
   * pan gesture-detector meets the parameters of the current gesture.
   *
   * @param[in]  gestureEvent        The gesture event.
   * @param[in]  actor               The actor that has been gestured.
   * @param[in]  renderTask          The renderTask.
   * @param[in]  possiblePanPosition The possiblePanPosition.
   *
   * @return true, if the detector meets the parameters, false otherwise.
   */
  bool CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask, Vector2 possiblePanPosition);

public:
  /**
   * Called by the PanGestureProcessor when a pan gesture event occurs within the bounds of our
   * attached actor.
   * @param[in]  actor  The panned actor.
   * @param[in]  pan    The pan gesture.
   */
  void EmitPanGestureSignal(Dali::Actor actor, const Dali::PanGesture& pan);

public: // Signals
  /**
   * @copydoc Dali::PanGestureDetector::DetectedSignal()
   */
  Dali::PanGestureDetector::DetectedSignalType& DetectedSignal()
  {
    return mDetectedSignal;
  }

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

public: // Override Pan Gesture
  /**
   * @copydoc Dali::PanGestureDetector::SetPanGestureProperties()
   */
  static void SetPanGestureProperties(const Dali::PanGesture& pan);

protected:
  /**
   * Construct a new PanGestureDetector.
   * @param sceneObject the scene object
   */
  PanGestureDetector(const SceneGraph::PanGesture& sceneObject);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~PanGestureDetector() override;

private:
  // Undefined
  PanGestureDetector()                                         = delete;
  PanGestureDetector(const PanGestureDetector&)                = delete;
  PanGestureDetector& operator=(const PanGestureDetector& rhs) = delete;

  /**
   * @return the pan gesture scene object
   */
  const SceneGraph::PanGesture& GetPanGestureSceneObject() const;

  // From GestureDetector

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorAttach(Actor&)
   */
  void OnActorAttach(Actor& actor) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorDetach(Actor&)
   */
  void OnActorDetach(Actor& actor) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorDestroyed(Object&)
   */
  void OnActorDestroyed(Object& object) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::OnTouchEvent(Dali::Actor, const Dali::TouchEvent&)
   */
  bool OnTouchEvent(Dali::Actor actor, const Dali::TouchEvent& touch) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::ProcessTouchEvent(Scene&, const Integration::TouchEvent&)
   */
  void ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::CheckGestureDetector(const GestureEvent*, Actor*, RenderTaskPtr)
   */
  bool CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask) override
  {
    // pan gesture use CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask, Vector2 possiblePanPosition) api
    return true;
  }

  /**
   * @copydoc Dali::Internal::GestureDetector::CancelProcessing()
   */
  void CancelProcessing() override;

  /**
   * This method is called whenever a pan gesture event occurs.
   * @param[in] scene The scene the pan gesture event occurs in.
   * @param[in] panEvent The event that has occurred.
   */
  void Process(Scene& scene, const PanGestureEvent& panEvent) override;

  /**
   * Creates a PanGesture and emit its detected signal.
   * @param[in]  actor             The actor that has been panned.
   * @param[in]  panEvent          The panEvent received from the adaptor.
   * @param[in]  localCurrent      Current position relative to the actor attached to the detector.
   * @param[in]  state             The state of the gesture.
   * @param[in]  renderTask        The renderTask to use.
   * @param[in]  scene             The scene the pan gesture event occurs in.
   */
  void EmitPanSignal(Actor* actor, const PanGestureEvent& panEvent, Vector2 localCurrent, GestureState state, RenderTaskPtr renderTask, Scene& scene);

  // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty(Property::Index index) const override;

private:
  Dali::PanGestureDetector::DetectedSignalType mDetectedSignal;

  uint32_t mMinimumTouches;        ///< The minimum number of fingers required to be touching for pan.
  uint32_t mMaximumTouches;        ///< The maximum number of fingers required to be touching for pan.
  uint32_t mMaximumMotionEventAge; ///< The maximum age of motion events as milliseconds.

  AngleContainer mAngleContainer; ///< A container of all angles allowed for pan to occur.

  Vector2                 mPossiblePanPosition; ///< The Position when possible state.
  Vector2                 mLastVelocity;        ///< The last recorded velocity in local actor coordinates.
  Vector2                 mLastScreenVelocity;  ///< The last recorded velocity in screen coordinates.
  ActorObserver           mCurrentPanActor;     ///< The current actor that has been gestured.
  SceneGraph::PanGesture* mSceneObject;         ///< Not owned, but we write to it directly (Owned by UpdateManager)
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::PanGestureDetector& GetImplementation(Dali::PanGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "PanGestureDetector handle is empty");

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::PanGestureDetector&>(handle);
}

inline const Internal::PanGestureDetector& GetImplementation(const Dali::PanGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "PanGestureDetector handle is empty");

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::PanGestureDetector&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_PAN_GESTURE_DETECTOR_H
