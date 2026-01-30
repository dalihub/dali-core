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
#include <dali/internal/update/animation/scene-graph-constraint-container.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

namespace Dali::Internal::SceneGraph
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
} // namespace

void ConstraintContainer::PushBack(ConstraintBase* constraint)
{
  if(constraint->GetApplyRate() == Dali::Constraint::ApplyRate::APPLY_ONCE && constraint->GetAppliedCount() > 0u)
  {
    // Already activated constraint.
    mDeactiveConstraints.PushBack(constraint);
  }
  else
  {
    mActiveConstraints.PushBack(constraint);
  }
  DALI_LOG_CONSTRAINT_INFO("[%p] Add SG[%p](r:%d, c:%d). act[%zu] deact[%zu]\n", this, constraint, constraint->GetApplyRate(), constraint->GetAppliedCount(), mActiveConstraints.Count(), mDeactiveConstraints.Count());
}

void ConstraintContainer::EraseObject(ConstraintBase* constraint)
{
  if(constraint->GetApplyRate() == Dali::Constraint::ApplyRate::APPLY_ONCE && constraint->GetAppliedCount() > 0u)
  {
    mDeactiveConstraints.EraseObject(constraint);
  }
  else
  {
    mActiveConstraints.EraseObject(constraint);
  }
  DALI_LOG_CONSTRAINT_INFO("[%p] Remove SG[%p](r:%d, c:%d). act[%zu] deact[%zu]\n", this, constraint, constraint->GetApplyRate(), constraint->GetAppliedCount(), mActiveConstraints.Count(), mDeactiveConstraints.Count());
}

void ConstraintContainer::Clear()
{
  DALI_LOG_CONSTRAINT_INFO("[%p] Clear, act[%zu] deact[%zu]\n", this, mActiveConstraints.Count(), mDeactiveConstraints.Count());
  mActiveConstraints.Clear();
  mDeactiveConstraints.Clear();
}

void ConstraintContainer::ApplyRateChanged(ConstraintBase* constraint)
{
  // Make it always activated
  auto endIter = mDeactiveConstraints.End();
  auto iter    = std::find(mDeactiveConstraints.Begin(), endIter, constraint);

  if(iter != endIter)
  {
    mActiveConstraints.PushBack(mDeactiveConstraints.Release(iter));
  }
  DALI_LOG_CONSTRAINT_INFO("[%p] Activate SG[%p](r:%d, c:%d). act[%zu] deact[%zu]\n", this, constraint, constraint->GetApplyRate(), constraint->GetAppliedCount(), mActiveConstraints.Count(), mDeactiveConstraints.Count());
}

void ConstraintContainer::Apply(BufferIndex updateBufferIndex)
{
  if(mActiveConstraints.Empty())
  {
    return;
  }

  static std::vector<ConstraintIter> deactivatedIters;
  for(ConstraintIter iter = mActiveConstraints.Begin(), endIter = mActiveConstraints.End(); iter != endIter; ++iter)
  {
    ConstraintBase& constraint = **iter;
    DALI_LOG_CONSTRAINT_INFO("[%p] Apply SG[%p](r:%d, c:%d).\n", this, &constraint, constraint.GetApplyRate(), constraint.GetAppliedCount());
    constraint.Apply(updateBufferIndex);

    if(constraint.GetApplyRate() == Dali::Constraint::ApplyRate::APPLY_ONCE && constraint.GetAppliedCount() > 0u)
    {
      deactivatedIters.push_back(iter);
    }
  }

  DALI_LOG_CONSTRAINT_INFO("[%p] Apply(%d) act[%zu] deact[%zu] deact now[%zu]\n", this, updateBufferIndex, mActiveConstraints.Count(), mDeactiveConstraints.Count(), deactivatedIters.size());

  // FILO ordered iterator release.
  // Since OwnerContainer is linear container, we could keep iter.
  while(!deactivatedIters.empty())
  {
    auto iter = deactivatedIters.back();
    deactivatedIters.pop_back();

    mDeactiveConstraints.PushBack(mActiveConstraints.Release(iter));
  }
}
} // namespace Dali::Internal::SceneGraph
