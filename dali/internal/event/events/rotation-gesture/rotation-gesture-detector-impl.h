#ifndef DALI_INTERNAL_ROTATION_GESTURE_DETECTOR_H
#define DALI_INTERNAL_ROTATION_GESTURE_DETECTOR_H

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
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-event.h>
#include <dali/public-api/events/rotation-gesture-detector.h>
#include <dali/public-api/events/rotation-gesture.h>

namespace Dali
{
namespace Internal
{
class RotationGestureDetector;

using RotationGestureDetectorPtr       = IntrusivePtr<RotationGestureDetector>;
using RotationGestureDetectorContainer = DerivedGestureDetectorContainer<RotationGestureDetector>::type;

/**
 * @copydoc Dali::RotationGestureDetector
 */
class RotationGestureDetector : public GestureDetector, public RecognizerObserver<RotationGestureEvent>
{
public: // Creation
  /**
   * Create a new gesture detector.
   * @return A smart-pointer to the newly allocated detector.
   */
  static RotationGestureDetectorPtr New();

  /**
   * Construct a new GestureDetector.
   */
  RotationGestureDetector();

  // Not copyable

  RotationGestureDetector(const RotationGestureDetector&)                = delete; ///< Deleted copy constructor
  RotationGestureDetector& operator=(const RotationGestureDetector& rhs) = delete; ///< Deleted copy assignment operator

public:
  /**
   * Called by the RotationGestureProcessor when a rotation gesture event occurs within the bounds of our
   * attached actor.
   * @param[in]  actor     The rotated actor
   * @param[in]  rotation  The rotation gesture
   */
  void EmitRotationGestureSignal(Dali::Actor actor, const Dali::RotationGesture& rotation);

public: // Signals
  /**
   * @copydoc Dali::RotationGestureDetector::DetectedSignal()
   */
  Dali::RotationGestureDetector::DetectedSignalType& DetectedSignal()
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

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~RotationGestureDetector() override = default;

private: // GestureDetector overrides
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
  void OnActorDestroyed(Object& object) override
  { /* Nothing to do */
  }

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
  bool CheckGestureDetector(const GestureEvent* longPressEvent, Actor* actor, RenderTaskPtr renderTask) override
  {
    // No special case required for rotation.
    return true;
  }

  /**
   * @copydoc Dali::Internal::GestureDetector::CancelProcessing()
   */
  void CancelProcessing() override;

  /**
   * This method is called whenever a tap gesture event occurs.
   * @param[in] scene The scene the tap gesture event occurs in.
   * @param[in] tapEvent The event that has occurred.
   */
  void Process(Scene& scene, const RotationGestureEvent& tapEvent) override;

  /**
   * Get the current gestured actor
   * @return The current gestured actor, or nullptr if no actor is currently gestured
   */
  Actor* GetCurrentGesturedActor() override;

  /**
   * Creates a RotationGesture emit its detected signal.
   * @param[in]  actor         The actor that has been rotationed.
   * @param[in]  rotationEvent The rotationEvent received from the adaptor.
   * @param[in]  localCenter   Relative to the actor attached to the detector.
   */
  void EmitRotationSignal(Actor* actor, const RotationGestureEvent& rotationEvent, Vector2 localCenter);

private:
  Dali::RotationGestureDetector::DetectedSignalType mDetectedSignal;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::RotationGestureDetector& GetImplementation(Dali::RotationGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "RotationGestureDetector handle is empty");

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::RotationGestureDetector&>(handle);
}

inline const Internal::RotationGestureDetector& GetImplementation(const Dali::RotationGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "RotationGestureDetector handle is empty");

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::RotationGestureDetector&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_ROTATION_GESTURE_DETECTOR_H
