#ifndef DALI_INTERNAL_PAN_GESTURE_DETECTOR_H
#define DALI_INTERNAL_PAN_GESTURE_DETECTOR_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/events/gesture-detector-impl.h>
#include <dali/public-api/events/pan-gesture.h>

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
class PanGestureDetector : public GestureDetector
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
   * @copydoc Dali::PanGestureDetector::SetMinimumTouchesRequired(unsigned int)
   */
  void SetMinimumTouchesRequired(unsigned int minimum);

  /**
   * @copydoc Dali::PanGestureDetector::SetMaximumTouchesRequired(unsigned int)
   */
  void SetMaximumTouchesRequired(unsigned int maximum);

  /**
   * @copydoc Dali::PanGestureDetector::GetMinimumTouchesRequired() const
   */
  uint32_t GetMinimumTouchesRequired() const;

  /**
   * @copydoc Dali::PanGestureDetector::GetMaximumTouchesRequired() const
   */
  uint32_t GetMaximumTouchesRequired() const;

  /**
   * @copydoc Dali::PanGestureDetector::AddAngle()
   */
  void AddAngle( Radian angle, Radian threshold );

  /**
   * @copydoc Dali::PanGestureDetector::AddDirection()
   */
  void AddDirection( Radian direction, Radian threshold );

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
  void RemoveAngle( Radian angle );

  /**
   * @copydoc Dali::PanGestureDetector::RemoveDirection()
   */
  void RemoveDirection( Radian direction );

  /**
   * Checks whether the pan gesture detector requires a directional pan for emission.
   * @return true, if directional panning required, false otherwise.
   */
  bool RequiresDirectionalPan() const;

  /**
   * Checks whether the given pan angle is allowed for this gesture detector.
   * @param[in]  angle  The angle to check.
   */
  bool CheckAngleAllowed( Radian angle ) const;

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
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

public: // Override Pan Gesture

  /**
   * @copydoc Dali::PanGestureDetector::SetPanGestureProperties()
   */
  static void SetPanGestureProperties( const Dali::PanGesture& pan );

protected:

  /**
   * Construct a new PanGestureDetector.
   * @param sceneObject the scene object
   */
  PanGestureDetector( const SceneGraph::PanGesture& sceneObject );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~PanGestureDetector() override;

private:

  // Undefined
  PanGestureDetector() = delete;
  PanGestureDetector(const PanGestureDetector&) = delete;
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


  // Default property extensions from Object

  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty( Property::Index index ) const override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue( Property::Index index ) const override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const override;

private:

  Dali::PanGestureDetector::DetectedSignalType mDetectedSignal;

  unsigned int mMinimumTouches; ///< The minimum number of fingers required to be touching for pan.
  unsigned int mMaximumTouches; ///< The maximum number of fingers required to be touching for pan.

  AngleContainer mAngleContainer; ///< A container of all angles allowed for pan to occur.

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::PanGestureDetector& GetImplementation(Dali::PanGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "PanGestureDetector handle is empty" );

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::PanGestureDetector&>(handle);
}

inline const Internal::PanGestureDetector& GetImplementation(const Dali::PanGestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "PanGestureDetector handle is empty" );

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::PanGestureDetector&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_PAN_GESTURE_DETECTOR_H
