#ifndef DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H
#define DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H

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
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/animation/constraint-source-impl.h>
#include <dali/internal/event/common/event-thread-services-holder.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Internal
{
class EventThreadServices;
class Object;
using ObjectContainer = Dali::Vector<Object*>;
using ObjectIter      = ObjectContainer::Iterator;

namespace SceneGraph
{
class ConstraintBase;

template<typename T>
class AnimatableProperty;
} // namespace SceneGraph

/**
 * An abstract base class for active constraints.
 */
class ConstraintBase : public BaseObject, public Object::Observer, public EventThreadServicesHolder
{
public:
  using RemoveAction = Dali::Constraint::RemoveAction;

  /**
   * Constructor.
   * @param[in] object The property owning object.
   * @param[in] messageController Used to send messages to the update-thread.
   * @param[in] targetPropertyIndex The index of the property being constrained.
   * @param[in] sources The sources of the input properties.
   */
  ConstraintBase(Object& object, Property::Index targetPropertyIndex, SourceContainer& sources);

  /**
   * Clone this constraint for another object.
   * @param[in]  object  The object to clone this constraint for
   * @return A new constraint.
   */
  ConstraintBase* Clone(Object& object);

  /**
   * Virtual destructor.
   */
  ~ConstraintBase() override;

  /**
   * Adds a constraint source to the constraint
   *
   * @param[in] source The constraint source input to add
   */
  void AddSource(Source source);

  /**
   * @copydoc Dali::Constraint::Apply()
   */
  void Apply(bool isPreConstraint = true);

  /**
   * @copydoc Dali::Constraint::ApplyPost()
   */
  void ApplyPost();

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
   * @copydoc Dali::Constraint::SetApplyRate()
   */
  void SetApplyRate(uint32_t applyRate);

  /**
   * @copydoc Dali::Constraint::GetApplyRate()
   */
  uint32_t GetApplyRate() const;

  /**
   * @copydoc Dali::Constraint::SetTag()
   */
  void SetTag(uint32_t tag);

  /**
   * @copydoc Dali::Constraint::GetTag()
   */
  uint32_t GetTag() const;

private: // Object::Observer methods
  /**
   * @copydoc Object::Observer::SceneObjectAdded()
   */
  void SceneObjectAdded(Object& object) override;

  /**
   * @copydoc Object::Observer::SceneObjectRemoved()
   */
  void SceneObjectRemoved(Object& object) override;

  /**
   * @copydoc Object::Observer::ObjectDestroyed()
   */
  void ObjectDestroyed(Object& object) override;

private:
  /**
   * Helper to observe an object, if not already observing it
   */
  void ObserveObject(Object& object);

  /**
   * Helper to stop observing objects
   */
  void StopObservation();

  // To be implemented in derived classes

  /**
   * Clone the actual constraint
   *
   * @param object to clone to
   * @return pointer to the clone
   */
  virtual ConstraintBase* DoClone(Object& object) = 0;

  /**
   * Connect the constraint
   */
  virtual void ConnectConstraint(bool isPreConstraint = true) = 0;

protected:
  /**
   * Helper to Add an input property to the container of property owners
   * @param source constraint[in] source used to determine the type and locate the property on the object
   * @param propertyOwners[out] reference to the container to add
   * @param componentIndex[out] component index
   * @return pointer to input property if it was found, nullptr otherwise
   */
  PropertyInputImpl* AddInputProperty(Source& source, SceneGraph::PropertyOwnerContainer& propertyOwners, int32_t& componentIndex);

protected:
  Object*                           mTargetObject; ///< The object owns the constraint.
  const SceneGraph::ConstraintBase* mSceneGraphConstraint;
  SourceContainer                   mSources;
  ObjectContainer                   mObservedObjects; // We don't observe the same object twice
  Property::Index                   mTargetPropertyIndex;
  RemoveAction                      mRemoveAction;
  uint32_t                          mApplyRate;
  uint32_t                          mTag;
  bool                              mApplied : 1;         ///< Whether the constraint has been applied
  bool                              mSourceDestroyed : 1; ///< Is set to true if any of our input source objects are destroyed
  bool                              mIsPreConstraint : 1; ///< Is set to true if this constraint is run before transform.

  bool mConstraintResetterApplied : 1;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ConstraintBase& GetImplementation(Dali::Constraint& constraint)
{
  DALI_ASSERT_ALWAYS(constraint && "Constraint handle is empty");

  BaseObject& handle = constraint.GetBaseObject();

  return static_cast<Internal::ConstraintBase&>(handle);
}

inline const Internal::ConstraintBase& GetImplementation(const Dali::Constraint& constraint)
{
  DALI_ASSERT_ALWAYS(constraint && "Constraint handle is empty");

  const BaseObject& handle = constraint.GetBaseObject();

  return static_cast<const Internal::ConstraintBase&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H
