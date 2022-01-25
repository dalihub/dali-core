/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
PropertyOwner* PropertyOwner::New()
{
  return new PropertyOwner();
}

PropertyOwner::~PropertyOwner()
{
  Destroy();
}

void PropertyOwner::AddObserver(Observer& observer)
{
  //Check for duplicates in debug builds
  DALI_ASSERT_DEBUG(mObservers.End() == std::find(mObservers.Begin(), mObservers.End(), &observer));

  mObservers.PushBack(&observer);
}

void PropertyOwner::RemoveObserver(Observer& observer)
{
  // Find the observer...
  const ConstObserverIter endIter = mObservers.End();
  for(ObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    if((*iter) == &observer)
    {
      // erase it
      mObservers.Erase(iter);
      break;
    }
  }
}

bool PropertyOwner::IsObserved()
{
  return mObservers.Count() != 0u;
}

void PropertyOwner::Destroy()
{
  // Notification for observers
  const ConstObserverIter endIter = mObservers.End();
  for(ConstObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    (*iter)->PropertyOwnerDestroyed(*this);
  }

  mObservers.Clear();

  // Remove all constraints when disconnected from scene-graph
  mConstraints.Clear();
}

void PropertyOwner::ConnectToSceneGraph()
{
  mIsConnectedToSceneGraph = true;
  SetUpdated(true);

  // Notification for observers
  const ConstObserverIter endIter = mObservers.End();
  for(ConstObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    (*iter)->PropertyOwnerConnected(*this);
  }
}

void PropertyOwner::DisconnectFromSceneGraph(BufferIndex updateBufferIndex)
{
  mIsConnectedToSceneGraph = false;

  // Notification for observers
  const ConstObserverIter endIter = mObservers.End();
  for(ConstObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    (*iter)->PropertyOwnerDisconnected(updateBufferIndex, *this);
  }

  // Remove all constraints when disconnected from scene-graph
  mConstraints.Clear();
}

void PropertyOwner::ReserveProperties(int propertyCount)
{
  mCustomProperties.Reserve(propertyCount);
}

void PropertyOwner::InstallCustomProperty(OwnerPointer<PropertyBase>& property)
{
  mCustomProperties.PushBack(property.Release());
}

ConstraintOwnerContainer& PropertyOwner::GetConstraints()
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
  const ConstraintIter constraintEndIter = mConstraints.End();
  for(ConstraintIter iter = mConstraints.Begin(); constraintEndIter != iter; ++iter)
  {
    if(*iter == constraint)
    {
      mConstraints.Erase(iter);
      return; // We're finished
    }
  }

  //it may be that the constraint has already been removed e.g. from disconnection from scene graph, so nothing needs to be done
}

PropertyOwner::PropertyOwner()
: mUpdated(false),
  mIsConnectedToSceneGraph(false)
{
}

void PropertyOwner::AddUniformMapping(const UniformPropertyMapping& map)
{
  mUniformMaps.Add(map);
}

void PropertyOwner::RemoveUniformMapping(const ConstString& uniformName)
{
  mUniformMaps.Remove(uniformName);
}

const UniformMap& PropertyOwner::GetUniformMap() const
{
  return mUniformMaps;
}

void PropertyOwner::AddUniformMapObserver(UniformMap::Observer& observer)
{
  mUniformMaps.AddObserver(observer);
}

void PropertyOwner::RemoveUniformMapObserver(UniformMap::Observer& observer)
{
  mUniformMaps.RemoveObserver(observer);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
