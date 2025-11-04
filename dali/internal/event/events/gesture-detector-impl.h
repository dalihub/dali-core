#ifndef DALI_INTERNAL_GESTURE_DETECTOR_H
#define DALI_INTERNAL_GESTURE_DETECTOR_H

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
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/events/gesture-event.h>
#include <dali/internal/event/events/gesture-recognizer.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/signals/slot-delegate.h>

namespace Dali
{
namespace Integration
{
struct GestureEvent;
}

namespace Internal
{
class GestureDetector;
class GestureEventProcessor;

using GestureDetectorPtr            = IntrusivePtr<GestureDetector>;
using GestureDetectorContainer      = std::vector<GestureDetector*>;
using GestureDetectorActorContainer = std::vector<Actor*>;
using GestureRecognizerPtr          = IntrusivePtr<GestureRecognizer>;

/**
 * This is a type trait that should be used by deriving gesture detectors for their container type.
 */
template<typename Detector>
struct DerivedGestureDetectorContainer
{
  using type = std::vector<Detector*>;
};

/**
 * @copydoc Dali::GestureDetector
 */
class GestureDetector : public Object, public Object::Observer, public ConnectionTracker
{
public:
  /**
   * @copydoc Dali::GestureDetector::Attach()
   */
  void Attach(Actor& actor);

  /**
   * @copydoc Dali::GestureDetector::Detach()
   */
  void Detach(Actor& actor);

  /**
   * @copydoc Dali::GestureDetector::DetachAll()
   */
  void DetachAll();

  /**
   * @copydoc Dali::GestureDetector::GetAttachedActorCount() const
   */
  size_t GetAttachedActorCount() const;

  /**
   * @copydoc Dali::GestureDetector::GetAttachedActor() const
   */
  Dali::Actor GetAttachedActor(size_t index) const;

  /**
   * @copydoc Dali::GestureDetector::HandleEvent()
   */
  bool HandleEvent(Dali::Actor& actor, Dali::TouchEvent& touch);

  /**
   * Returns a const reference to the container of attached actor pointers.
   * @return A const reference to the attached internal actors.
   */
  const GestureDetectorActorContainer& GetAttachedActorPointers() const
  {
    return mAttachedActors;
  }

  /**
   * Retrieves the type of GestureDetector
   * @return The GestureDetector Type
   */
  GestureType::Value GetType() const
  {
    return mType;
  }

  /**
   * Checks if the specified actor is still attached or pending attachment.
   * @param[in]  actor  The actor to check.
   * @return true, if the actor is attached or pending, false otherwise.
   */
  bool IsAttached(Actor& actor) const;

  /**
   * @brief This method that cancels all other gesture detectors except the current gesture detector
   */
  void CancelAllOtherGestureDetectors();

  /**
   * @brief Returns whether the gesture was detected.
   * @return true, if the gesture was detected, false otherwise.
   */
  bool IsDetected() const;

  /**
   * @brief Sets whether the gesture was detected.
   * @param detected Whether gesture detected.
   */
  void SetDetected(bool detected);

  /**
   * Get the current gestured actor
   * @return The current gestured actor, or nullptr if no actor is currently gestured
   */
  Actor* GetCurrentGesturedActor();

  /**
   * gesture-detector meets the parameters of the current gesture.
   *
   * @param[in]  gestureEvent The gesture event.
   * @param[in]  actor        The actor that has been gestured.
   * @param[in]  renderTask   The renderTask.
   *
   * @return true, if the detector meets the parameters, false otherwise.
   */
  virtual bool CheckGestureDetector(const GestureEvent* gestureEvent, Actor* actor, RenderTaskPtr renderTask) = 0;

  /**
   * @brief This is canceling the ongoing gesture recognition process.
   */
  virtual void CancelProcessing() = 0;

protected: // Creation & Destruction
  /**
   * Construct a new GestureDetector.
   * @param type the type of gesture
   * @param pointer to the scene object, nullptr if none
   * by default GestureDetectors don't have our own scene object
   */
  GestureDetector(GestureType::Value type, const SceneGraph::PropertyOwner* sceneObject = nullptr);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~GestureDetector() override;

private:
  // Undefined
  GestureDetector()                                      = delete;
  GestureDetector(const GestureDetector&)                = delete;
  GestureDetector& operator=(const GestureDetector& rhs) = delete;

  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  void SceneObjectAdded(Object& object) override;

  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  void SceneObjectRemoved(Object& object) override
  {
  }

  /**
   * @copydoc Dali::Internal::Object::Observer::ObjectDestroyed()
   */
  void ObjectDestroyed(Object& object) override;

  /**
   * For use in derived classes, called after an actor is attached.
   * @param[in]  actor  The actor that is being attached.
   */
  virtual void OnActorAttach(Actor& actor) = 0;

  /**
   * For use in derived classes, called after an actor is detached.
   * @param[in] actor The actor that is being detached.
   */
  virtual void OnActorDetach(Actor& actor) = 0;

  /**
   * For use in derived classes, called when an attached actor is destroyed.
   * @param[in] object The object (Actor's base class) that has been destroyed.
   * @note Derived classes should not call any Actor specific APIs in this method as the Actor's
   *       destructor would have already been called.
   */
  virtual void OnActorDestroyed(Object& object) = 0;

  /**
   * @brief Callback for TouchedSignal signal
   *
   * @param actor The actor
   * @param touch The touch event that has occurred
   */
  virtual bool OnTouchEvent(Dali::Actor actor, const Dali::TouchEvent& touch) = 0;

  /**
   * @brief Delivers touch events to each detector.
   * This is an API that is called by FeedTouch and recognizes gestures directly from the Detector without going through the Reconizer.
   *
   * @param scene The scene
   * @param event The touch event that has occurred
   */
  virtual void ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event) = 0;

  /**
   * @brief Clears the detector.
   */
  void Clear();

protected:
  GestureType::Value            mType;                  ///< The gesture detector will detect this type of gesture.
  GestureDetectorActorContainer mAttachedActors;        ///< Object::Observer is used to provide weak-pointer behaviour
  GestureDetectorActorContainer mPendingAttachActors;   ///< Object::Observer is used to provide weak-pointer behaviour
  GestureEventProcessor&        mGestureEventProcessor; ///< A reference to the gesture event processor.
  ActorObserver                 mFeededActor;           ///< The Actor that feeds touch events
  RenderTaskPtr                 mRenderTask;            ///< The render task used to generate this touch event.
  GestureRecognizerPtr          mGestureRecognizer;     ///< The gesture recognizer
  bool                          mIsDetected : 1;        ///< Whether gesture detected.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::GestureDetector& GetImplementation(Dali::GestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "GestureDetector handle is empty");

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::GestureDetector&>(handle);
}

inline const Internal::GestureDetector& GetImplementation(const Dali::GestureDetector& detector)
{
  DALI_ASSERT_ALWAYS(detector && "GestureDetector handle is empty");

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::GestureDetector&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_GESTURE_DETECTOR_H
