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
#include <dali/internal/update/animation/scene-graph-constraint.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gConstraintFilter = Debug::Filter::New(Debug::NoLogging, false, "DALI_LOG_CONSTRAINT");

uint32_t mCurrentInstanceCount = 0;
uint32_t mTotalInstanceCount   = 0;
#endif

#if defined(DEBUG_ENABLED)
#define DALI_LOG_CONSTRAINT_INFO(format, ...) \
  DALI_LOG_INFO(gConstraintFilter, Debug::Verbose, format, ##__VA_ARGS__)
#else
#define DALI_LOG_CONSTRAINT_INFO(format, ...)
#endif
} // namespace

ConstraintBase::ConstraintBase(PropertyOwnerContainer& ownerSet, RemoveAction removeAction, uint32_t applyRate)
: mRemoveAction(removeAction),
  mApplyRate(applyRate),
  mAppliedCount(0u),
  mFirstApply(true),
  mDisconnected(true),
  mObservedOwners(ownerSet),
  mLifecycleObserver(nullptr)
{
  DALI_LOG_CONSTRAINT_INFO("SG[%p](r:%d, c:%d)\n", this, mApplyRate, mAppliedCount);
#ifdef DEBUG_ENABLED
  ++mCurrentInstanceCount;
  ++mTotalInstanceCount;
#endif
}

ConstraintBase::~ConstraintBase()
{
  DALI_LOG_CONSTRAINT_INFO("~SG[%p](r:%d, c:%d)\n", this, mApplyRate, mAppliedCount);
  if(!mDisconnected)
  {
    StopObservation();
  }

  if(mLifecycleObserver != nullptr)
  {
    mLifecycleObserver->ObjectDestroyed();
  }

#ifdef DEBUG_ENABLED
  --mCurrentInstanceCount;
#endif
}

uint32_t ConstraintBase::GetCurrentInstanceCount()
{
#ifdef DEBUG_ENABLED
  return mCurrentInstanceCount;
#else
  return 0u;
#endif
}

uint32_t ConstraintBase::GetTotalInstanceCount()
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
