#ifndef DALI_INTERNAL_LONG_PRESS_GESTURE_DETECTOR_IMPL_H
#define DALI_INTERNAL_LONG_PRESS_GESTURE_DETECTOR_IMPL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-event.h>

namespace Dali
{
namespace Internal
{
class LongPressGestureDetector;

using LongPressGestureDetectorPtr       = IntrusivePtr<LongPressGestureDetector>;
using LongPressGestureDetectorContainer = DerivedGestureDetectorContainer<LongPressGestureDetector>::type;

/**
 * @copydoc Dali::LongPressGestureDetector
 */
class LongPressGestureDetector : public GestureDetector, public RecognizerObserver<LongPressGestureEvent>
{
public: // Creation
  /**
   * Create a new gesture detector.
   * @return A smart-pointer to the newly allocated detector.
   */
  static LongPressGestureDetectorPtr New();

  /**
   * Create a new gesture detector with the specified touches.
   * @param[in]  touchesRequired  The number of touches required.
   * @return A smart-pointer to the newly allocated detector.
   */
  static LongPressGestureDetectorPtr New(unsigned int touchesRequired);

  /**
   * Create a new gesture detector with the specified minimum and maximum touches.
   * @param[in]  minTouches  The minimum number of touches required.
   * @param[in]  maxTouches  The maximum number of touches required.
   * @return A smart-pointer to the newly allocated detector.
   */
  static LongPressGestureDetectorPtr New(unsigned int minTouches, unsigned int maxTouches);

  /**
   * Construct a new GestureDetector.
   */
  LongPressGestureDetector();

  /**
   * Create a new gesture detector with the specified minimum and maximum touches.
   * @param[in]  minTouches  The minimum number of touches required.
   * @param[in]  maxTouches  The maximum number of touches required.
   */
  LongPressGestureDetector(unsigned int minTouches, unsigned int maxTouches);

public:
  /**
   * @copydoc Dali::LongPressGestureDetector::SetTouchesRequired(unsigned int)
   */
  void SetTouchesRequired(unsigned int touches);

  /**
   * @copydoc Dali::LongPressGestureDetector::SetTouchesRequired(unsigned int, unsigned int)
   */
  void SetTouchesRequired(unsigned int minTouches, unsigned int maxTouches);

  /**
   * @copydoc Dali::LongPressGestureDetector::GetMinimumTouchesRequired()
   */
  unsigned int GetMinimumTouchesRequired() const;

  /**
   * @copydoc Dali::LongPressGestureDetector::GetMaximumTouchesRequired()
   */
  unsigned int GetMaximumTouchesRequired() const;

  /**
   * @return The minimum holding time required to be recognized as a long press gesture in milliseconds
   */
  uint32_t GetMinimumHoldingTime() const;

public:
  /**
   * Called by the LongPressGestureProcessor when a tap gesture event occurs within the bounds of our
   * attached actor.
   * @param[in]  pressedActor  The pressed actor.
   * @param[in]  longPress     The long press
   */
  void EmitLongPressGestureSignal(Dali::Actor pressedActor, const Dali::LongPressGesture& longPress);

public: // Signals
  /**
   * @copydoc Dali::LongPressGestureDetector::DetectedSignal()
   */
  Dali::LongPressGestureDetector::DetectedSignalType& DetectedSignal()
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
  ~LongPressGestureDetector() override;

private:
  // Undefined
  LongPressGestureDetector(const LongPressGestureDetector&);
  LongPressGestureDetector& operator=(const LongPressGestureDetector& rhs);

  /**
   * Creates a LongPressGesture and asks the specified detector to emit its detected signal.
   * @param[in]  actor             The actor on which the long press gesture has occurred.
   * @param[in]  gestureDetectors  A reference to gesture detectors that should emit the signal.
   * @param[in]  longPressEvent    The longPressEvent received from the adaptor.
   * @param[in]  localPoint        Relative to the actor attached to the detector.
   */
  void EmitLongPressSignal(Actor* actor, const LongPressGestureEvent& longPressEvent, Vector2 localPoint);

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
  bool CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask) override;

  /**
   * @copydoc Dali::Internal::GestureDetector::CancelProcessing()
   */
  void CancelProcessing() override;

  /**
   * This method is called whenever a long press gesture event occurs.
   * @param[in] scene The scene the long press gesture event occurs in.
   * @param[in] longPressEvent The event that has occurred.
   */
  void Process(Scene& scene, const LongPressGestureEvent& longPressEvent) override;

private:
  Dali::LongPressGestureDetector::DetectedSignalType mDetectedSignal;

  unsigned int mMinimumTouchesRequired;
  unsigned int mMaximumTouchesRequired;

  ActorObserver          mCurrentLongPressActor;  ///< Current actor for which the long press gesture has been recognized.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::LongPressGestureDetector& GetImplementation(Dali::LongPressGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "LongPressGestureDetector handle is empty");

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::LongPressGestureDetector&>(handle);
}

inline const Internal::LongPressGestureDetector& GetImplementation(const Dali::LongPressGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "LongPressGestureDetector handle is empty");

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::LongPressGestureDetector&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_LONG_PRESS_GESTURE_DETECTOR_IMPL_H
