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
#include <dali/internal/update/common/property-owner.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/internal/common/const-string.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/property-owner-flag-manager.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
PropertyOwnerFlagManager* gPropertyOwnerFlagManager = nullptr;
} // unnamed namespace

PropertyOwner* PropertyOwner::New()
{
  return new PropertyOwner();
}

void PropertyOwner::RegisterPropertyOwnerFlagManager(PropertyOwnerFlagManager& manager)
{
  DALI_ASSERT_ALWAYS(gPropertyOwnerFlagManager == nullptr && "PropertyOwner::RegisterPropertyOwnerFlagManager called twice!");

  gPropertyOwnerFlagManager = &manager;
}

void PropertyOwner::UnregisterPropertyOwnerFlagManager()
{
  DALI_ASSERT_ALWAYS(gPropertyOwnerFlagManager && "PropertyOwner::RegisterPropertyOwnerFlagManager not be called before!");

  gPropertyOwnerFlagManager = nullptr;
}

PropertyOwner::~PropertyOwner()
{
  Destroy();
}

void PropertyOwner::AddObserver(Observer& observer)
{
  DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot add observer while notifying PropertyOwner::Observers");

  auto [iter, inserted] = mObservers.emplace(&observer, 1u);
  if(!inserted)
  {
    // Increase the number of observer count
    ++(iter->second);
  }
}

void PropertyOwner::RemoveObserver(Observer& observer)
{
  DALI_ASSERT_ALWAYS(!mObserverNotifying && "Cannot remove observer while notifying PropertyOwner::Observers");

  auto iter = mObservers.find(&observer);
  DALI_ASSERT_ALWAYS(iter != mObservers.end());

  if(--(iter->second) == 0u)
  {
    mObservers.erase(iter);
  }
}

bool PropertyOwner::IsObserved()
{
  return mObservers.size() != 0u;
}

void PropertyOwner::Destroy()
{
  // Guard Add/Remove observer during observer notifying.
  mObserverNotifying = true;

  // Notification for observers
  for(auto&& item : mObservers)
  {
    item.first->PropertyOwnerDestroyed(*this);
  }

  // Note : We don't need to restore mObserverNotifying to false as we are in delete the object.
  // If someone call AddObserver / RemoveObserver after this, assert.

  // Remove all observers
  mObservers.clear();

  // Remove all constraints when disconnected from scene-graph
  mConstraints.Clear();
  mPostConstraints.Clear();
}

void PropertyOwner::ConnectToSceneGraph()
{
  DALI_ASSERT_ALWAYS(!mObserverNotifying && "Should not be call ConnectToSceneGraph while notifying PropertyOwner::Observers");

  mIsConnectedToSceneGraph = true;
  SetUpdated(true);

  // Guard Add/Remove observer during observer notifying.
  mObserverNotifying = true;

  // Notification for observers
  for(auto&& item : mObservers)
  {
    item.first->PropertyOwnerConnected(*this);
  }

  mObserverNotifying = false;
}

void PropertyOwner::DisconnectFromSceneGraph()
{
  DALI_ASSERT_ALWAYS(!mObserverNotifying && "Should not be call DisconnectFromSceneGraph while notifying PropertyOwner::Observers");

  mIsConnectedToSceneGraph = false;

  // Guard Add/Remove observer during observer notifying.
  mObserverNotifying = true;

  // Notification for observers
  for(auto iter = mObservers.begin(); iter != mObservers.end();)
  {
    auto returnValue = (*iter).first->PropertyOwnerDisconnected(*this);
    if(returnValue == Observer::KEEP_OBSERVING)
    {
      ++iter;
    }
    else
    {
      iter = mObservers.erase(iter);
    }
  }

  mObserverNotifying = false;

  // Remove all constraints when disconnected from scene-graph
  mConstraints.Clear();
  mPostConstraints.Clear();
}

void PropertyOwner::ReserveProperties(int propertyCount)
{
  mCustomProperties.Reserve(propertyCount);
}

void PropertyOwner::InstallCustomProperty(OwnerPointer<PropertyBase>& property)
{
  mCustomProperties.PushBack(property.Release());
}

void PropertyOwner::RequestResetUpdated() const
{
  DALI_ASSERT_ALWAYS(gPropertyOwnerFlagManager && "PropertyOwner::SetUpdated called without RegisterPropertyOwnerFlagManager!!");

  gPropertyOwnerFlagManager->RequestResetUpdated(*this);
}

ConstraintContainer& PropertyOwner::GetConstraints()
{
  return mConstraints;
}

void PropertyOwner::ApplyConstraint(OwnerPointer<ConstraintBase>& constraint)
{
  constraint->OnConnect();
  mConstraints.PushBack(constraint.Release());
}

void PropertyOwner::RemoveConstraint(ConstraintBase* constraint)
{
  mConstraints.EraseObject(constraint);
  // it may be that the constraint has already been removed e.g. from disconnection from scene graph, so nothing needs to be done
}

void PropertyOwner::ConstraintApplyRateChanged(ConstraintBase* constraint)
{
  mConstraints.ApplyRateChanged(constraint);
}

ConstraintContainer& PropertyOwner::GetPostConstraints()
{
  return mPostConstraints;
}

uint32_t PropertyOwner::GetPostConstraintsActivatedCount() const
{
  return mPostConstraints.ActivateCount();
}

void PropertyOwner::ApplyPostConstraint(OwnerPointer<ConstraintBase>& constraint)
{
  constraint->OnConnect();
  mPostConstraints.PushBack(constraint.Release());
}

void PropertyOwner::RemovePostConstraint(ConstraintBase* constraint)
{
  mPostConstraints.EraseObject(constraint);
  // it may be that the constraint has already been removed e.g. from disconnection from scene graph, so nothing needs to be done
}

void PropertyOwner::PostConstraintApplyRateChanged(ConstraintBase* constraint)
{
  mPostConstraints.ApplyRateChanged(constraint);
}

PropertyOwner::PropertyOwner()
: mUpdated(false),
  mIsConnectedToSceneGraph(false),
  mObserverNotifying(false)
{
}

void PropertyOwner::AddUniformMapping(const UniformPropertyMapping& map)
{
  mUniformMaps.Add(map);
  OnMappingChanged();
}

void PropertyOwner::RemoveUniformMapping(const ConstString& uniformName)
{
  mUniformMaps.Remove(uniformName);
  OnMappingChanged();
}

const UniformMap& PropertyOwner::GetUniformMap() const
{
  return mUniformMaps;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
