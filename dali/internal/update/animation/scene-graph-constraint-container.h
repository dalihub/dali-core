#ifndef DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_CONTAINER_H
#define DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_CONTAINER_H

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

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>

namespace Dali::Internal::SceneGraph
{
class ConstraintBase;

/**
 * @brief Owner container of SceneGraph::ConstraintBase.
 * Help to control activated constraints and deactivated constraints.
 * (Deactivate mean, ApplyRate is APPLY_ONCE and we already applied it.)
 */
class ConstraintContainer
{
public: // Behavior of normal container
  /**
   * Constructor
   */
  ConstraintContainer() = default;

  /**
   * Destructor
   */
  ~ConstraintContainer() = default;

  void PushBack(ConstraintBase* constraint);

  void EraseObject(ConstraintBase* constraint);

  void Clear();

public:
  void ApplyRateChanged(ConstraintBase* constraint);

  uint32_t ActivateCount() const
  {
    return mActiveConstraints.Count();
  }

  void Apply();

private:
  ConstraintContainer(const ConstraintContainer&)            = delete;
  ConstraintContainer& operator=(const ConstraintContainer&) = delete;

  ConstraintContainer(ConstraintContainer&&)            = delete;
  ConstraintContainer& operator=(ConstraintContainer&&) = delete;

private:
  using ConstraintOwnerContainer = OwnerContainer<ConstraintBase*>;
  using ConstraintIter           = ConstraintOwnerContainer::Iterator;

  ConstraintOwnerContainer mActiveConstraints;   ///< List of constraints that should be constraint this frame.
  ConstraintOwnerContainer mDeactiveConstraints; ///< List of APPLY_ONCE applied constraints.
};

} // namespace Dali::Internal::SceneGraph

#endif // DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_CONTAINER_H
