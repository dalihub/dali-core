#ifndef __DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H__
#define __DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H__

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
#include <dali/internal/common/owner-pointer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/animation/constraint-source-impl.h>

namespace Dali
{

namespace Internal
{

class EventThreadServices;
class Object;
typedef Dali::Vector<Object*>     ObjectContainer;
typedef ObjectContainer::Iterator ObjectIter;

namespace SceneGraph
{
class ConstraintBase;

template <typename T>
class AnimatableProperty;
}

/**
 * An abstract base class for active constraints.
 */
class ConstraintBase : public BaseObject, public Object::Observer
{
public:

  typedef Dali::Constraint::RemoveAction RemoveAction;

  /**
   * Constructor.
   * @param[in] object The property owning object.
   * @param[in] messageController Used to send messages to the update-thread.
   * @param[in] targetPropertyIndex The index of the property being constrained.
   * @param[in] sources The sources of the input properties.
   */
  ConstraintBase( Object& object, Property::Index targetPropertyIndex, SourceContainer& sources );

  /**
   * Clone this constraint for another object.
   * @param[in]  object  The object to clone this constraint for
   * @return A new constraint.
   */
  virtual ConstraintBase* Clone( Object& object ) = 0;

  /**
   * Virtual destructor.
   */
  virtual ~ConstraintBase();

  /**
   * Adds a constraint source to the constraint
   *
   * @param[in] source The constraint source input to add
   */
  void AddSource( Source source );

  /**
   * @copydoc Dali::Constraint::Apply()
   */
  void Apply();

  /**
   * @copydoc Dali::Constraint::Remove()
   */
  void Remove();

  /**
   * Called when the Constraint is removed.
   *
   * @note This removes the scene-object as well but then does not call back into the target-object.
   */
  void RemoveInternal();

  /**
   * Retrieve the parent of the constraint.
   * @return The parent object, or NULL.
   */
  Object* GetParent();

  /**
   * @copydoc Dali::Constraint::GetTargetObject()
   */
  Dali::Handle GetTargetObject();

  /**
   * @copydoc Dali::Constraint::GetTargetProperty()
   */
  Property::Index GetTargetProperty();

  /**
   * @copydoc Dali::Constraint::SetRemoveAction()
   */
  void SetRemoveAction(RemoveAction action);

  /**
   * @copydoc Dali::Constraint::GetRemoveAction()
   */
  RemoveAction GetRemoveAction() const;

  /**
   * @copydoc Dali::Constraint::SetTag()
   */
  void SetTag(const unsigned int tag);

  /**
   * @copydoc Dali::Constraint::GetTag()
   */
  unsigned int GetTag() const;

private: // Object::Observer methods

  /**
   * @copydoc Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded( Object& object );

  /**
   * @copydoc Object::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved( Object& object );

  /**
   * @copydoc Object::Observer::ObjectDestroyed()
   */
  virtual void ObjectDestroyed( Object& object );

private:

  /**
   * Helper to observe an object, if not already observing it
   */
  void ObserveObject( Object& object );

  /**
   * Helper to stop observing objects
   */
  void StopObservation();

  // To be implemented in derived classes

  /**
   * Create and connect a constraint for a scene-object.
   */
  virtual void ConnectConstraint() = 0;

protected:

  /**
   * Get the event thread services object - used for sending messages to the scene graph
   * Assert if called from the wrong thread.
   * This is intentionally inline for performance reasons.
   *
   * @return The event thread services object
   */
  inline EventThreadServices& GetEventThreadServices()
  {
    DALI_ASSERT_DEBUG( EventThreadServices::IsCoreRunning() );
    return mEventThreadServices;
  }

  /**
   * Get the event thread services object - used for sending messages to the scene graph
   * Assert if called from the wrong thread
   * This is intentionally inline for performance reasons.
   *
   * @return The event thread services object
   */
  inline const EventThreadServices& GetEventThreadServices() const
  {
    DALI_ASSERT_DEBUG( EventThreadServices::IsCoreRunning() );
    return mEventThreadServices;
  }

protected:
  EventThreadServices& mEventThreadServices;
  Object* mTargetObject; ///< The object owns the constraint.
  const SceneGraph::ConstraintBase* mSceneGraphConstraint;
  SourceContainer mSources;
  ObjectContainer mObservedObjects; // We don't observe the same object twice
  Property::Index mTargetPropertyIndex;
  RemoveAction mRemoveAction;
  unsigned int mTag;
  bool mApplied:1; ///< Whether the constraint has been applied
  bool mSourceDestroyed:1; ///< Is set to true if any of our input source objects are destroyed
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ConstraintBase& GetImplementation(Dali::Constraint& constraint)
{
  DALI_ASSERT_ALWAYS( constraint && "Constraint handle is empty" );

  BaseObject& handle = constraint.GetBaseObject();

  return static_cast<Internal::ConstraintBase&>(handle);
}

inline const Internal::ConstraintBase& GetImplementation(const Dali::Constraint& constraint)
{
  DALI_ASSERT_ALWAYS( constraint && "Constraint handle is empty" );

  const BaseObject& handle = constraint.GetBaseObject();

  return static_cast<const Internal::ConstraintBase&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H__
