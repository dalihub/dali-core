/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/event/animation/constraint-base.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner-messages.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/type-registry.h>

using Dali::Internal::SceneGraph::AnimatableProperty;

namespace Dali
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gConstraintFilter = Debug::Filter::New(Debug::NoLogging, false, "DALI_LOG_CONSTRAINT");
#endif

#if defined(DEBUG_ENABLED)
#define DALI_LOG_CONSTRAINT_INFO(format, ...) \
  DALI_LOG_INFO(gConstraintFilter, Debug::Verbose, format, ##__VA_ARGS__)
#else
#define DALI_LOG_CONSTRAINT_INFO(format, ...)
#endif

/**
 * Helper to add only unique entries to the propertyOwner container
 * @param propertyOwners to add the entries to
 * @param object to add
 */
inline void AddUnique(SceneGraph::PropertyOwnerContainer& propertyOwners, SceneGraph::PropertyOwner* object)
{
  const SceneGraph::PropertyOwnerIter iter = std::find(propertyOwners.Begin(), propertyOwners.End(), object);
  if(iter == propertyOwners.End())
  {
    // each owner should only be added once
    propertyOwners.PushBack(object);
  }
}
} // unnamed namespace

ConstraintBase::ConstraintBase(Object& object, Property::Index targetPropertyIndex, SourceContainer& sources)
: EventThreadServicesHolder(EventThreadServices::Get()),
  mTargetObject(&object),
  mSceneGraphConstraint(nullptr),
  mSources(sources),
  mObservedObjects(),
  mTargetPropertyIndex(targetPropertyIndex),
  mRemoveAction(Dali::Constraint::DEFAULT_REMOVE_ACTION),
  mApplyRate(Dali::Constraint::APPLY_ALWAYS),
  mTag(0),
  mApplied(false),
  mSourceDestroyed(false),
  mIsPreConstraint(true),
  mConstraintResetterApplied(false)
{
  DALI_LOG_CONSTRAINT_INFO("Constraint[%p] index[%u]\n", this, mTargetPropertyIndex);
  ObserveObject(object);
}

ConstraintBase* ConstraintBase::Clone(Object& object)
{
  DALI_ASSERT_ALWAYS(!mSourceDestroyed && "An input source object has been destroyed");

  // create the type specific object
  ConstraintBase* clone = DoClone(object);
  clone->SetRemoveAction(mRemoveAction);
  clone->SetApplyRate(mApplyRate);
  clone->SetTag(mTag);
  return clone;
}

ConstraintBase::~ConstraintBase()
{
  DALI_LOG_CONSTRAINT_INFO("~Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] with sources[%zu]\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate, mSources.size());
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~ConstraintBase[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  StopObservation();

  RemoveInternal();
}

void ConstraintBase::AddSource(Source source)
{
  mSources.push_back(source);

  // Observe the object providing this property
  if(OBJECT_PROPERTY == source.sourceType)
  {
    if(source.object != nullptr)
    {
      ObserveObject(*source.object);
    }
    else
    {
      DALI_LOG_ERROR("Constraint source object not found\n");
    }
  }
}

void ConstraintBase::Apply(bool isPreConstraint)
{
  if(mTargetObject && !mApplied && !mSourceDestroyed)
  {
    mApplied         = true;
    mIsPreConstraint = isPreConstraint;
    ConnectConstraint(mIsPreConstraint);

    DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] with sources[%zu] Apply(%d)\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate, mSources.size(), isPreConstraint);
    mTargetObject->ApplyConstraint(*this);
  }
  else
  {
    DALI_LOG_ERROR("Fail to apply constraint\n");
  }
}

void ConstraintBase::ApplyPost()
{
  Apply(false);
}

void ConstraintBase::Remove()
{
  RemoveInternal();

  if(mTargetObject)
  {
    mTargetObject->RemoveConstraint(*this);
  }
  mIsPreConstraint = true;
}

void ConstraintBase::RemoveInternal()
{
  if(mApplied)
  {
    DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] with sources[%zu] Remove(%d)\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate, mSources.size(), mIsPreConstraint);
    mApplied = false;

    mConstraintResetterApplied = false;

    // Guard against constraint sending messages during core destruction
    if(Stage::IsInstalled())
    {
      if(mTargetObject && mSceneGraphConstraint)
      {
        const SceneGraph::PropertyOwner& propertyOwner = mTargetObject->GetSceneObject();
        // Remove from scene-graph
        if(mIsPreConstraint)
        {
          RemoveConstraintMessage(GetEventThreadServices(), propertyOwner, *(mSceneGraphConstraint));
        }
        else
        {
          RemovePostConstraintMessage(GetEventThreadServices(), propertyOwner, *(mSceneGraphConstraint));
        }
        // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
        mSceneGraphConstraint = nullptr;
      }
    }
  }
}

Object* ConstraintBase::GetParent()
{
  return mTargetObject;
}

Dali::Handle ConstraintBase::GetTargetObject()
{
  return Dali::Handle(mTargetObject);
}

Property::Index ConstraintBase::GetTargetProperty()
{
  return mTargetPropertyIndex;
}

void ConstraintBase::SetRemoveAction(ConstraintBase::RemoveAction action)
{
  DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] with sources[%zu] SetRemoveAction() %d -> %d\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate, mSources.size(), static_cast<int>(mRemoveAction), static_cast<int>(action));
  mRemoveAction = action;
}

ConstraintBase::RemoveAction ConstraintBase::GetRemoveAction() const
{
  return mRemoveAction;
}

void ConstraintBase::SetApplyRate(uint32_t applyRate)
{
  DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] with sources[%zu] SetApplyRate() %u -> %u\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate, mSources.size(), mApplyRate, applyRate);
  mApplyRate = applyRate;

  // Always send message to support apply whenever we call SetApplyRate(APPLY_ONCE)
  if(mSceneGraphConstraint)
  {
    SetApplyRateMessage(GetEventThreadServices(), *(mSceneGraphConstraint), mApplyRate);
    if(DALI_LIKELY(mTargetObject))
    {
      SceneGraph::PropertyOwner& targetObject = const_cast<SceneGraph::PropertyOwner&>(mTargetObject->GetSceneObject());

      if(mIsPreConstraint)
      {
        ConstraintApplyRateChangedMessage(GetEventThreadServices(), targetObject, *(mSceneGraphConstraint));
      }
      else
      {
        PostConstraintApplyRateChangedMessage(GetEventThreadServices(), targetObject, *(mSceneGraphConstraint));
      }

      const SceneGraph::PropertyBase* targetProperty = mTargetObject->GetSceneObjectAnimatableProperty(mTargetPropertyIndex);
      // The targetProperty should exist, when targetObject exists
      DALI_ASSERT_ALWAYS(nullptr != targetProperty && "Constraint target property does not exist");

      if(!targetProperty->IsTransformManagerProperty())
      {
        if(mApplyRate == Dali::Constraint::APPLY_ONCE)
        {
          mConstraintResetterApplied = false;
          auto resetter              = SceneGraph::BakerResetter::New(targetObject, *targetProperty, mRemoveAction == Dali::Constraint::RemoveAction::BAKE ? SceneGraph::BakerResetter::Lifetime::BAKE : SceneGraph::BakerResetter::Lifetime::SET);
          AddResetterMessage(GetEventThreadServices().GetUpdateManager(), resetter);
        }
        else if(!mConstraintResetterApplied)
        {
          mConstraintResetterApplied = true;
          auto resetter              = SceneGraph::ConstraintResetter::New(targetObject, *targetProperty, *mSceneGraphConstraint);
          AddResetterMessage(GetEventThreadServices().GetUpdateManager(), resetter);
        }
      }
    }
  }
}

uint32_t ConstraintBase::GetApplyRate() const
{
  return mApplyRate;
}

void ConstraintBase::SetTag(uint32_t tag)
{
  if(mTag != ConstraintTagRanges::DEFAULT_TAG && tag != ConstraintTagRanges::DEFAULT_TAG)
  {
    // Assert if application try to set tag to internal constraints,
    // or internal engine try to set tag what application using.
    DALI_ASSERT_ALWAYS(
      (((ConstraintTagRanges::CUSTOM_CONSTRAINT_TAG_START <= mTag && mTag <= ConstraintTagRanges::CUSTOM_CONSTRAINT_TAG_MAX) &&
        (ConstraintTagRanges::CUSTOM_CONSTRAINT_TAG_START <= tag && tag <= ConstraintTagRanges::CUSTOM_CONSTRAINT_TAG_MAX)) ||
       ((ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START <= mTag && mTag <= ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_MAX) &&
        (ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START <= tag && tag <= ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_MAX))) &&
      "Cross tag setting is not allowed!");
  }
  DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] SetTag() %u\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate, tag);
  mTag = tag;
}

uint32_t ConstraintBase::GetTag() const
{
  return mTag;
}

Dali::Constraint::State ConstraintBase::GetState() const
{
  if(DALI_UNLIKELY(mSourceDestroyed))
  {
    return Dali::Constraint::State::OBJECT_DESTROYED;
  }
  if(!mApplied)
  {
    return Dali::Constraint::State::INITIALIZED;
  }

  // Applied cases
  if(mSceneGraphConstraint)
  {
    return mIsPreConstraint ? Dali::Constraint::State::APPLIED : Dali::Constraint::State::POST_APPLIED;
  }
  else
  {
    return mIsPreConstraint ? Dali::Constraint::State::APPLIED_OBJECT_OFF_SCENE : Dali::Constraint::State::POST_APPLIED_OBJECT_OFF_SCENE;
  }
}

uint32_t ConstraintBase::GetSourceCount() const
{
  return static_cast<uint32_t>(mSources.size());
}

Dali::ConstraintSource ConstraintBase::GetSourceAt(uint32_t index) const
{
  DALI_ASSERT_ALWAYS(index < static_cast<uint32_t>(mSources.size()) && "Constraint GetSourceAt index out of bounds");

  const Source& internalSource = mSources[index];

  switch(internalSource.sourceType)
  {
    case Dali::SourceType::OBJECT_PROPERTY:
    {
      auto handle = Dali::Handle(internalSource.object);
      return Dali::ConstraintSource(Dali::Source(handle, internalSource.propertyIndex));
    }
    case Dali::SourceType::LOCAL_PROPERTY:
    default: ///< Never ever comes here, but for coverage.
    {
      return Dali::ConstraintSource(Dali::LocalSource(internalSource.propertyIndex));
    }
    case Dali::SourceType::PARENT_PROPERTY:
    {
      return Dali::ConstraintSource(Dali::ParentSource(internalSource.propertyIndex));
    }
  }
}

void ConstraintBase::SceneObjectAdded(Object& object)
{
  DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] SceneObjectAdded()\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate);
  if(mApplied &&
     (nullptr == mSceneGraphConstraint) &&
     mTargetObject)
  {
    ConnectConstraint(mIsPreConstraint);
  }
}

void ConstraintBase::SceneObjectRemoved(Object& object)
{
  DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] SceneObjectRemoved()\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate);
  if(mSceneGraphConstraint)
  {
    // An input property owning source has been deleted, need to tell the scene-graph-constraint owner to remove it
    if(&object != mTargetObject)
    {
      if(mTargetObject)
      {
        const SceneGraph::PropertyOwner& propertyOwner = mTargetObject->GetSceneObject();
        // Remove from scene-graph
        RemoveConstraintMessage(GetEventThreadServices(), propertyOwner, *(mSceneGraphConstraint));
      }
    }

    // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
    mSceneGraphConstraint = nullptr;
  }
}

void ConstraintBase::ObjectDestroyed(Object& object)
{
  DALI_LOG_CONSTRAINT_INFO("Constraint[%p] SG[%p] tag[%u] index[%u] rate[%u] ObjectDestroyed()\n", this, mSceneGraphConstraint, mTag, mTargetPropertyIndex, mApplyRate);

  // Remove object pointer from observation set
  ObjectIter iter = std::find(mObservedObjects.Begin(), mObservedObjects.End(), &object);
  DALI_ASSERT_DEBUG(mObservedObjects.End() != iter);
  mObservedObjects.Erase(iter);

  // Constraint is not useful anymore as an input-source has been destroyed
  mSourceDestroyed = true;

  // Stop observing the remaining objects
  StopObservation();

  // Clear our sources as well
  mSources.clear();

  // Discard all object & scene-graph pointers
  mSceneGraphConstraint = nullptr;
  mTargetObject         = nullptr;
}

void ConstraintBase::ObserveObject(Object& object)
{
  ObjectIter iter = std::find(mObservedObjects.Begin(), mObservedObjects.End(), &object);
  if(mObservedObjects.End() == iter)
  {
    object.AddObserver(*this);
    mObservedObjects.PushBack(&object);
  }
}

void ConstraintBase::StopObservation()
{
  const ObjectIter end = mObservedObjects.End();
  for(ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter)
  {
    (*iter)->RemoveObserver(*this);
  }

  mObservedObjects.Clear();
}

PropertyInputImpl* ConstraintBase::AddInputProperty(Source& source, SceneGraph::PropertyOwnerContainer& propertyOwners, int32_t& componentIndex)
{
  PropertyInputImpl* inputProperty = nullptr;

  if(OBJECT_PROPERTY == source.sourceType)
  {
    DALI_ASSERT_ALWAYS(source.object->IsPropertyAConstraintInput(source.propertyIndex));

    SceneGraph::PropertyOwner& owner = const_cast<SceneGraph::PropertyOwner&>(source.object->GetSceneObject());

    AddUnique(propertyOwners, &owner);
    inputProperty  = const_cast<PropertyInputImpl*>(source.object->GetSceneObjectInputProperty(source.propertyIndex));
    componentIndex = source.object->GetPropertyComponentIndex(source.propertyIndex);

    // The scene-object property should exist, when the property owner exists
    DALI_ASSERT_ALWAYS(inputProperty && "Constraint source property does not exist");
  }
  else if(LOCAL_PROPERTY == source.sourceType)
  {
    DALI_ASSERT_ALWAYS(mTargetObject->IsPropertyAConstraintInput(source.propertyIndex));

    inputProperty  = const_cast<PropertyInputImpl*>(mTargetObject->GetSceneObjectInputProperty(source.propertyIndex));
    componentIndex = mTargetObject->GetPropertyComponentIndex(source.propertyIndex);

    // The target scene-object should provide this property
    DALI_ASSERT_ALWAYS(inputProperty && "Constraint source property does not exist");
  }
  else
  {
    DALI_ASSERT_ALWAYS(PARENT_PROPERTY == source.sourceType && "Constraint source property type is invalid");

    Object* objectParent = dynamic_cast<Actor&>(*mTargetObject).GetParent();

    // This will not exist, if the target object is off-stage
    if(objectParent)
    {
      DALI_ASSERT_ALWAYS(objectParent->IsPropertyAConstraintInput(source.propertyIndex));

      SceneGraph::PropertyOwner& owner = const_cast<SceneGraph::PropertyOwner&>(objectParent->GetSceneObject());

      AddUnique(propertyOwners, &owner);
      inputProperty  = const_cast<PropertyInputImpl*>(objectParent->GetSceneObjectInputProperty(source.propertyIndex));
      componentIndex = objectParent->GetPropertyComponentIndex(source.propertyIndex);

      // The scene-object property should exist, when the property owner exists
      DALI_ASSERT_ALWAYS(inputProperty && "Constraint source property does not exist");
    }
  }
  return inputProperty;
}

} // namespace Internal

} // namespace Dali
