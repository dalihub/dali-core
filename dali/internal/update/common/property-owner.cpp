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

// CLASS HEADER
#include <dali/internal/update/common/property-owner.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

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
  DisconnectFromSceneGraph();
}

void PropertyOwner::AddObserver(Observer& observer)
{
  //Check for duplicates in debug builds
  DALI_ASSERT_DEBUG( mObservers.End() == std::find( mObservers.Begin(), mObservers.End(), &observer ) );

  mObservers.PushBack( &observer );
}

void PropertyOwner::RemoveObserver(Observer& observer)
{
  // Find the observer...
  const ConstObserverIter endIter =  mObservers.End();
  for( ObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    if( (*iter) == &observer)
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

void PropertyOwner::DisconnectFromSceneGraph()
{
  // Notification for observers
  const ConstObserverIter endIter = mObservers.End();
  for( ConstObserverIter iter = mObservers.Begin(); iter != endIter; ++iter)
  {
    (*iter)->PropertyOwnerDestroyed(*this);
  }

  mObservers.Clear();

  // Remove all constraints when disconnected from scene-graph
  mConstraints.Clear();
}

void PropertyOwner::InstallCustomProperty(PropertyBase* property)
{
  DALI_ASSERT_DEBUG( NULL != property );

  mCustomProperties.PushBack( property );
}

void PropertyOwner::ResetToBaseValues( BufferIndex updateBufferIndex )
{
  // Reset custom properties
  const OwnedPropertyIter endIter = mCustomProperties.End();
  for ( OwnedPropertyIter iter = mCustomProperties.Begin(); endIter != iter; ++iter )
  {
    (*iter)->ResetToBaseValue( updateBufferIndex );
  }

  // Reset constraint weights
  const ConstraintIter constraintEndIter = mConstraints.End();
  for( ConstraintIter iter = mConstraints.Begin(); constraintEndIter != iter; ++iter )
  {
    (*iter)->mWeight.ResetToBaseValue( updateBufferIndex );
  }

  // Notification for derived classes, to reset default properties
  ResetDefaultProperties( updateBufferIndex );
}

ConstraintOwnerContainer& PropertyOwner::GetConstraints()
{
  return mConstraints;
}

void PropertyOwner::ApplyConstraint( ConstraintBase* constraint )
{
  mConstraints.PushBack( constraint );

  constraint->OnConnect();
}

void PropertyOwner::RemoveConstraint( ConstraintBase* constraint )
{
  // Reset constraint weights
  const ConstraintIter constraintEndIter = mConstraints.End();
  for( ConstraintIter iter = mConstraints.Begin(); constraintEndIter != iter; ++iter )
  {
    if ( *iter == constraint )
    {
      mConstraints.Erase( iter );
      return; // We're finished
    }
  }

  // Should not come here
  DALI_ASSERT_DEBUG( false && "Constraint missing in RemoveConstraint" );
}

PropertyOwner::PropertyOwner()
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
