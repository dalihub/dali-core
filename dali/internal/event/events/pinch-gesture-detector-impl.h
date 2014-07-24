#ifndef __DALI_INTERNAL_PINCH_GESTURE_DETECTOR_H__
#define __DALI_INTERNAL_PINCH_GESTURE_DETECTOR_H__

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
#include <dali/public-api/events/pinch-gesture-detector.h>
#include <dali/internal/event/events/gesture-detector-impl.h>

namespace Dali
{

struct PinchGesture;

namespace Internal
{

class PinchGestureDetector;

typedef IntrusivePtr<PinchGestureDetector> PinchGestureDetectorPtr;
typedef DerivedGestureDetectorContainer<PinchGestureDetector>::type PinchGestureDetectorContainer;

/**
 * @copydoc Dali::PinchGestureDetector
 */
class PinchGestureDetector : public GestureDetector
{
public: // Creation

  /**
   * Create a new gesture detector.
   * @return A smart-pointer to the newly allocated detector.
   */
  static PinchGestureDetectorPtr New();

  /**
   * Construct a new GestureDetector.
   */
  PinchGestureDetector();

public:

  /**
   * Called by the PinchGestureProcessor when a pinch gesture event occurs within the bounds of our
   * attached actor.
   * @param[in]  actor  The pinched actor.
   * @param[in]  pinch  The pinch gesture.
   */
  void EmitPinchGestureSignal(Dali::Actor actor, const PinchGesture& pinch);

public: // Signals

  /**
   * @copydoc Dali::PinchGestureDetector::DetectedSignal()
   */
  Dali::PinchGestureDetector::DetectedSignalV2& DetectedSignal()
  {
    return mDetectedSignalV2;
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
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PinchGestureDetector();

private:

  // Undefined
  PinchGestureDetector(const PinchGestureDetector&);
  PinchGestureDetector& operator=(const PinchGestureDetector& rhs);

private: // GestureDetector overrides

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorAttach(Actor&)
   */
  virtual void OnActorAttach(Actor& actor);

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorDetach(Actor&)
   */
  virtual void OnActorDetach(Actor& actor);

  /**
   * @copydoc Dali::Internal::GestureDetector::OnActorDestroyed(Object&)
   */
  virtual void OnActorDestroyed(Object& object);

private:

  Dali::PinchGestureDetector::DetectedSignalV2 mDetectedSignalV2;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::PinchGestureDetector& GetImplementation(Dali::PinchGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "PinchGestureDetector handle is empty" );

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::PinchGestureDetector&>(handle);
}

inline const Internal::PinchGestureDetector& GetImplementation(const Dali::PinchGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "PinchGestureDetector handle is empty" );

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::PinchGestureDetector&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_PINCH_GESTURE_DETECTOR_H__
