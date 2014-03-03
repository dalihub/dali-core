#ifndef __DALI_INTERNAL_LONG_PRESS_GESTURE_DETECTOR_H__
#define __DALI_INTERNAL_LONG_PRESS_GESTURE_DETECTOR_H__

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
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/internal/event/events/gesture-detector-impl.h>

namespace Dali
{

struct LongPressGesture;

namespace Internal
{

class LongPressGestureDetector;

typedef IntrusivePtr<LongPressGestureDetector> LongPressGestureDetectorPtr;
typedef DerivedGestureDetectorContainer<LongPressGestureDetector>::type LongPressGestureDetectorContainer;

/**
 * @copydoc Dali::LongPressGestureDetector
 */
class LongPressGestureDetector : public GestureDetector
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

public:

  /**
   * Called by the LongPressGestureProcessor when a tap gesture event occurs within the bounds of our
   * attached actor.
   * @param[in]  pressedActor  The pressed actor.
   * @param[in]  longPress     The long press
   */
  void EmitLongPressGestureSignal(Dali::Actor pressedActor, const LongPressGesture& longPress);

public: // Signals

  /**
   * @copydoc Dali::LongPressGestureDetector::DetectedSignal()
   */
  Dali::LongPressGestureDetector::DetectedSignalV2& DetectedSignal()
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
  virtual ~LongPressGestureDetector();

private:

  // Undefined
  LongPressGestureDetector(const LongPressGestureDetector&);
  LongPressGestureDetector& operator=(const LongPressGestureDetector& rhs);

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

  Dali::LongPressGestureDetector::DetectedSignalV2 mDetectedSignalV2;

  unsigned int mMinimumTouchesRequired;
  unsigned int mMaximumTouchesRequired;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::LongPressGestureDetector& GetImplementation(Dali::LongPressGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "LongPressGestureDetector handle is empty" );

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::LongPressGestureDetector&>(handle);
}

inline const Internal::LongPressGestureDetector& GetImplementation(const Dali::LongPressGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "LongPressGestureDetector handle is empty" );

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::LongPressGestureDetector&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_LONG_PRESS_GESTURE_DETECTOR_H__
