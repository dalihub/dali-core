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

// CLASS HEADER
#include <dali/internal/update/animation/scene-graph-constraint-container.h>

// INTERNAL INCLUDES
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

namespace Dali::Internal::SceneGraph
{
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
}

void ConstraintContainer::Clear()
{
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
}

void ConstraintContainer::Apply(BufferIndex updateBufferIndex)
{
  static std::vector<ConstraintIter> deactivatedIters;
  for(ConstraintIter iter = mActiveConstraints.Begin(), endIter = mActiveConstraints.End(); iter != endIter; ++iter)
  {
    ConstraintBase& constraint = **iter;
    constraint.Apply(updateBufferIndex);

    if(constraint.GetApplyRate() == Dali::Constraint::ApplyRate::APPLY_ONCE && constraint.GetAppliedCount() > 0u)
    {
      deactivatedIters.push_back(iter);
    }
  }

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
