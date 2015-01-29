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
#include <dali/internal/event/common/proxy-object.h>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/animation/constraint-source-impl.h>

namespace Dali
{

namespace Internal
{

class EventToUpdate;
typedef Dali::Vector<ProxyObject*>     ProxyObjectContainer;
typedef ProxyObjectContainer::Iterator ProxyObjectIter;

namespace SceneGraph
{
class ConstraintBase;

template <typename T>
class AnimatableProperty;
}

/**
 * An abstract base class for active constraints.
 */
class ActiveConstraintBase : public ProxyObject, public ProxyObject::Observer
{
public:

  typedef Dali::Constraint::RemoveAction RemoveAction;
  typedef Any AnyFunction;

  /**
   * Constructor.
   * @param[in] messageController Used to send messages to the update-thread.
   * @param[in] targetPropertyIndex The index of the property being constrained.
   * @param[in] sources The sources of the input properties.
   * @param[in] sourceCount The original number of sources; this may not match sources.size() if objects have died.
   */
  ActiveConstraintBase( EventToUpdate& messageController, Property::Index targetPropertyIndex, SourceContainer& sources, unsigned int sourceCount );

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
   * Set a custom "weight" property.
   * @param[in] weightObject An object with a "weight" float property.
   * @param[in] weightIndex The index of the weight property.
   */
  void SetCustomWeightObject( ProxyObject& weightObject, Property::Index weightIndex );

  /**
   * Called when the ActiveConstraint is first applied.
   * @pre The active-constraint does not already have a parent.
   * @param[in] parent The parent object.
   * @param[in] applyTime The apply-time for this constraint.
   */
  void FirstApply( ProxyObject& parent, TimePeriod applyTime );

  /**
   * Called when the ActiveConstraint is removed.
   */
  void BeginRemove();

  /**
   * Called when the target object is destroyed.
   */
  void OnParentDestroyed();

  /**
   * Called when the target object is connected to the scene-graph
   */
  void OnParentSceneObjectAdded();

  /**
   * Called when the target object is disconnected from the scene-graph
   */
  void OnParentSceneObjectRemoved();

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
   * @copydoc Dali::Constraint::SetTag()
   */
  void SetTag(const unsigned int tag);

  /**
   * @copydoc Dali::Constraint::GetTag()
   */
  unsigned int GetTag() const;

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
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyCount()
   */
  virtual unsigned int GetDefaultPropertyCount() const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyIndices()
   */
  virtual void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyName()
   */
  virtual const char* GetDefaultPropertyName( Property::Index index ) const;

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
   * @copydoc Dali::Internal::ProxyObject::IsDefaultPropertyAConstraintInput()
   */
  virtual bool IsDefaultPropertyAConstraintInput( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultPropertyType()
   */
  virtual Property::Type GetDefaultPropertyType( Property::Index index ) const;

  /**
   * @copydoc Dali::Internal::ProxyObject::SetDefaultProperty()
   */
  virtual void SetDefaultProperty( Property::Index index, const Property::Value& propertyValue );

  /**
   * @copydoc Dali::Internal::ProxyObject::GetDefaultProperty()
   */
  virtual Property::Value GetDefaultProperty( Property::Index index ) const;

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

public: // ProxyObject::Observer methods

  /**
   * @copydoc ProxyObject::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded( ProxyObject& proxy );

  /**
   * @copydoc ProxyObject::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved( ProxyObject& proxy );

  /**
   * @copydoc ProxyObject::Observer::ProxyDestroyed()
   */
  virtual void ProxyDestroyed( ProxyObject& proxy );

private:

  /**
   * Helper to observe a proxy, if not already observing it
   */
  void ObserveProxy( ProxyObject& proxy );

  /**
   * Helper to stop observing proxies
   */
  void StopObservation();

  /**
   * Helper called after the first apply animation.
   * @param [in] object The active constraint.
   */
  static void FirstApplyFinished( Object* object );

  // To be implemented in derived classes

  /**
   * Create and connect a constraint for a scene-object.
   */
  virtual void ConnectConstraint() = 0;

protected:

  EventToUpdate& mEventToUpdate;

  Property::Index mTargetPropertyIndex;
  SourceContainer mSources;
  const unsigned int mSourceCount;

  ProxyObject* mTargetProxy; ///< The proxy-object owns the active-constraint.
  ProxyObjectContainer mObservedProxies; // We don't observe the same object twice

  const SceneGraph::ConstraintBase* mSceneGraphConstraint;

  const SceneGraph::AnimatableProperty<float>* mCustomWeight;

  float mOffstageWeight;

  AlphaFunction mAlphaFunction;

  RemoveAction mRemoveAction;
  unsigned int mTag;

private:

  ActiveConstraintSignalV2 mAppliedSignal;

  Dali::Animation mApplyAnimation;  ///< Used to automatically animate weight from 0.0f -> 1.0f

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
