#ifndef __DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H__
#define __DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H__

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
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/proxy-object.h>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

class EventToUpdate;
class ProxyObject;

namespace SceneGraph
{
class ConstraintBase;
}

/**
 * An abstract base class for active constraints.
 */
class ActiveConstraintBase : public ProxyObject
{
public:

  typedef Dali::Constraint::RemoveAction RemoveAction;
  typedef boost::any AnyFunction;

  /**
   * Constructor.
   * @param[in] messageController Used to send messages to the update-thread.
   * @param[in] targetPropertyIndex The index of the property being constrained.
   */
  ActiveConstraintBase( EventToUpdate& messageController, Property::Index targetPropertyIndex );

  /**
   * Virtual destructor.
   */
  virtual ~ActiveConstraintBase();

  /**
   * Clone an active-constraint.
   * @return A new active-constraint.
   */
  virtual ActiveConstraintBase* Clone() = 0;

  /**
   * Called when the ActiveConstraint is first applied.
   * @pre The active-constraint does not already have a parent.
   * @param[in] parent The parent object.
   * @param[in] applyTime The apply-time for this constraint.
   * @param[in] callback A pointer to a callback for the applied signal, or NULL.
   */
  void FirstApply( ProxyObject& parent, TimePeriod applyTime, ActiveConstraintCallbackType* callback );

  /**
   * Called when the ActiveConstraint is removed.
   */
  void BeginRemove();

  /**
   * Query whether the constraint is being removed.
   * This is only possible if mRemoveTime.durationSeconds is non-zero.
   * @return True if constraint is being removed.
   */
  bool IsRemoving();

  /**
   * Retrieve the parent of the active-constraint.
   * @return The parent object, or NULL.
   */
  ProxyObject* GetParent();

  /**
   * @copydoc Dali::Internal::Object::Supports()
   */
  virtual bool Supports( Object::Capability capability ) const;

  /**
   * @copydoc Dali::ActiveConstraint::GetTargetObject()
   */
  Dali::Handle GetTargetObject();

  /**
   * @copydoc Dali::ActiveConstraint::GetTargetProperty()
   */
  Property::Index GetTargetProperty();

  /**
   * @copydoc Dali::ActiveConstraint::SetWeight()
   */
  void SetWeight( float weight );

  /**
   * @copydoc Dali::ActiveConstraint::GetCurrentWeight()
   */
  float GetCurrentWeight() const;

  /**
   * @copydoc Dali::ActiveConstraint::AppliedSignal()
   */
  ActiveConstraintSignalV2& AppliedSignal();

  /**
   * @copydoc Dali::Constraint::SetRemoveTime()
   */
  void SetRemoveTime( TimePeriod timePeriod );

  /**
   * @copydoc Dali::Constraint::GetRemoveTime()
   */
  TimePeriod GetRemoveTime() const;

  /**
   * @copydoc Dali::Constraint::SetAlphaFunction()
   */
  void SetAlphaFunction(AlphaFunction func);

  /**
   * @copydoc Dali::Constraint::GetAlphaFunction()
   */
  AlphaFunction GetAlphaFunction() const;

  /**
   * @copydoc Dali::Constraint::SetRemoveAction()
   */
  void SetRemoveAction(RemoveAction action);

  /**
   * @copydoc Dali::Constraint::GetRemoveAction()
   */
  RemoveAction GetRemoveAction() const;

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

public: // Default property extensions from ProxyObject

  /**
   * @copydoc Dali::Internal::ProxyObject::IsSceneObjectRemovable()
   */
  virtual bool IsSceneObjectRemovable() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyName()
   */
  virtual const std::string& GetDefaultPropertyName( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndex()
   */
  virtual Property::Index GetDefaultPropertyIndex( const std::string& name ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyWritable()
   */
  virtual bool IsDefaultPropertyWritable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAnimatable()
   */
  virtual bool IsDefaultPropertyAnimatable( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Internal::ProxyObject::SetCustomProperty()
   */
  virtual void SetCustomProperty( Property::Index index, const CustomProperty& entry, const Property::Value& value );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::InstallSceneObjectProperty()
   */
  virtual void InstallSceneObjectProperty( SceneGraph::PropertyBase& newProperty, const std::string& name, unsigned int index );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObject()
   */
  virtual const SceneGraph::PropertyOwner* GetSceneObject() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectAnimatableProperty()
   */
  virtual const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetSceneObjectInputProperty()
   */
  virtual const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const;

private:

  /**
   * Helper called after the first apply animation.
   * @param [in] object The active constraint.
   */
  static void FirstApplyFinished( Object* object );

  /**
   * Helper called after the remove animation.
   * @param [in] object The active constraint.
   */
  static void OnRemoveFinished( Object* object );

  // To be implemented in derived classes

  /**
   * Set the parent of the active-constraint; called during OnFirstApply().
   * @param [in] parent The parent object.
   */
  virtual void OnFirstApply( ProxyObject& parent ) = 0;

  /**
   * Notification for the derived class, when BeginRemove() is called.
   */
  virtual void OnBeginRemove() = 0;

protected:

  EventToUpdate& mEventToUpdate;

  Property::Index mTargetPropertyIndex;

  ProxyObject* mTargetProxy; ///< The proxy-object owns the active-constraint.

  const SceneGraph::ConstraintBase* mSceneGraphConstraint;

  float mOffstageWeight;

  TimePeriod mRemoveTime;

  AlphaFunction mAlphaFunction;

  RemoveAction mRemoveAction;

private:

  ActiveConstraintSignalV2 mAppliedSignal;

  Dali::Animation mApplyAnimation;  ///< Used to automatically animate weight from 0.0f -> 1.0f
  Dali::Animation mRemoveAnimation; ///< Used to automatically animate weight back to 0.0f
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ActiveConstraintBase& GetImplementation(Dali::ActiveConstraint& constraint)
{
  DALI_ASSERT_ALWAYS( constraint && "ActiveConstraint handle is empty" );

  BaseObject& handle = constraint.GetBaseObject();

  return static_cast<Internal::ActiveConstraintBase&>(handle);
}

inline const Internal::ActiveConstraintBase& GetImplementation(const Dali::ActiveConstraint& constraint)
{
  DALI_ASSERT_ALWAYS( constraint && "ActiveConstraint handle is empty" );

  const BaseObject& handle = constraint.GetBaseObject();

  return static_cast<const Internal::ActiveConstraintBase&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ACTIVE_CONSTRAINT_BASE_H__
