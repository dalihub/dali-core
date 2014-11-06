/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/active-constraint.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/active-constraint-base.h>

namespace Dali
{

const float ActiveConstraint::FINAL_WEIGHT   = 1.0f;
const float ActiveConstraint::DEFAULT_WEIGHT = 1.0f;

const char* const ActiveConstraint::SIGNAL_APPLIED = "applied";

ActiveConstraint::ActiveConstraint()
{
}

ActiveConstraint::ActiveConstraint( Internal::ActiveConstraintBase* constraint )
: Handle( constraint )
{
}

ActiveConstraint::~ActiveConstraint()
{
}

ActiveConstraint::ActiveConstraint(const ActiveConstraint& handle)
: Handle( handle )
{
}

ActiveConstraint& ActiveConstraint::operator=(const ActiveConstraint& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Handle ActiveConstraint::GetTargetObject()
{
  return GetImplementation(*this).GetTargetObject();
}

Property::Index ActiveConstraint::GetTargetProperty()
{
  return GetImplementation(*this).GetTargetProperty();
}

void ActiveConstraint::SetWeight( float weight )
{
  GetImplementation(*this).SetWeight( weight );
}

float ActiveConstraint::GetCurrentWeight() const
{
  return GetImplementation(*this).GetCurrentWeight();
}

ActiveConstraintSignalV2& ActiveConstraint::AppliedSignal()
{
  return GetImplementation(*this).AppliedSignal();
}

} // namespace Dali
