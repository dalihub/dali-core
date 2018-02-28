/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/animation/scene-graph-constraint.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

#ifdef DEBUG_ENABLED
  unsigned int ConstraintBase::mCurrentInstanceCount = 0;
  unsigned int ConstraintBase::mTotalInstanceCount   = 0;
#endif

ConstraintBase::ConstraintBase( PropertyOwnerContainer& ownerSet, RemoveAction removeAction )
: mRemoveAction( removeAction ),
  mFirstApply( true ),
  mDisconnected( true ),
  mObservedOwners( ownerSet ),
  mLifecycleObserver( nullptr )
{
#ifdef DEBUG_ENABLED
  ++mCurrentInstanceCount;
  ++mTotalInstanceCount;
#endif
}

ConstraintBase::~ConstraintBase()
{
  if ( !mDisconnected )
  {
    StopObservation();
  }

  if( mLifecycleObserver != nullptr )
  {
    mLifecycleObserver->ObjectDestroyed();
  }

#ifdef DEBUG_ENABLED
  --mCurrentInstanceCount;
#endif
}

unsigned int ConstraintBase::GetCurrentInstanceCount()
{
#ifdef DEBUG_ENABLED
  return mCurrentInstanceCount;
#else
  return 0u;
#endif
}

unsigned int ConstraintBase::GetTotalInstanceCount()
{
#ifdef DEBUG_ENABLED
  return mTotalInstanceCount;
#else
  return 0u;
#endif
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
