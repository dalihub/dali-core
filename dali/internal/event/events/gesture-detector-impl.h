#ifndef DALI_INTERNAL_GESTURE_DETECTOR_H
#define DALI_INTERNAL_GESTURE_DETECTOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/signals/slot-delegate.h>
#include <dali/public-api/events/gesture-detector.h>
#include <dali/internal/event/actors/actor-impl.h>

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

typedef IntrusivePtr<GestureDetector> GestureDetectorPtr;
typedef std::vector<GestureDetector*> GestureDetectorContainer;
typedef std::vector<Actor*> GestureDetectorActorContainer;

/**
 * This is a type trait that should be used by deriving gesture detectors for their container type.
 */
template< typename Detector >
struct DerivedGestureDetectorContainer
{
  typedef std::vector<Detector*> type;
};

/**
 * @copydoc Dali::GestureDetector
 */
class GestureDetector : public Object, public Object::Observer
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
  DevelGesture::Type GetType() const
  {
    return mType;
  }

  /**
   * Checks if the specified actor is still attached or pending attachment.
   * @param[in]  actor  The actor to check.
   * @return true, if the actor is attached or pending, false otherwise.
   */
  bool IsAttached(Actor& actor) const;

protected: // Creation & Destruction

  /**
   * Construct a new GestureDetector.
   * @param type the type of gesture
   * @param pointer to the scene object, nullptr if none
   * by default GestureDetectors don't have our own scene object
   */
  GestureDetector( DevelGesture::Type type, const SceneGraph::PropertyOwner* sceneObject  = nullptr );

  /**
   * @copydoc GestureDetector( DevelGesture::Type, const SceneGraph::PropertyOwner* )
   */
  GestureDetector( Gesture::Type type, const SceneGraph::PropertyOwner* sceneObject  = nullptr );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~GestureDetector();

private:

  // Undefined
  GestureDetector() = delete;
  GestureDetector(const GestureDetector&) = delete;
  GestureDetector& operator=(const GestureDetector& rhs) = delete;

  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded(Object& object);

  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectRemoved(Object& object) {}

  /**
   * @copydoc Dali::Internal::Object::Observer::ObjectDestroyed()
   */
  virtual void ObjectDestroyed(Object& object);

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

protected:

  DevelGesture::Type            mType;                  ///< The gesture detector will detect this type of gesture.
  GestureDetectorActorContainer mAttachedActors;        ///< Object::Observer is used to provide weak-pointer behaviour
  GestureDetectorActorContainer mPendingAttachActors;   ///< Object::Observer is used to provide weak-pointer behaviour
  GestureEventProcessor&        mGestureEventProcessor; ///< A reference to the gesture event processor.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::GestureDetector& GetImplementation(Dali::GestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "GestureDetector handle is empty" );

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::GestureDetector&>(handle);
}

inline const Internal::GestureDetector& GetImplementation(const Dali::GestureDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "GestureDetector handle is empty" );

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::GestureDetector&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_GESTURE_DETECTOR_H
