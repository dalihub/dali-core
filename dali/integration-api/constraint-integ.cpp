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

// FILE HEADER
#include <dali/integration-api/constraint-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/object-impl.h>

namespace Dali::Integration
{
void ConstraintSetInternalTag(Dali::Constraint& constraint, const uint32_t tag)
{
  DALI_ASSERT_ALWAYS(ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START <= tag && tag <= ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_MAX && "Out of tag range!");
  GetImplementation(constraint).SetTag(tag);
}

void HandleRemoveAllConstraints(Dali::Handle& handle)
{
  GetImplementation(handle).RemoveConstraints();
}

void HandleRemoveConstraints(Dali::Handle& handle, uint32_t tag)
{
  DALI_ASSERT_ALWAYS(ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START <= tag && tag <= ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_MAX && "Out of tag range!");
  GetImplementation(handle).RemoveConstraints(tag);
}

void HandleRemoveConstraints(Dali::Handle& handle, uint32_t tagBegin, uint32_t tagEnd)
{
  DALI_ASSERT_ALWAYS(ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_START <= tagBegin && tagEnd <= ConstraintTagRanges::INTERNAL_CONSTRAINT_TAG_MAX + 1u && "Out of tag range!");
  GetImplementation(handle).RemoveConstraints(tagBegin, tagEnd);
}
} // namespace Dali::Integration
