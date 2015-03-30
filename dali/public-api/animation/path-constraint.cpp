/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/path-constraint.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/path-constraint-impl.h>
#include <dali/internal/event/animation/path-impl.h>

namespace Dali
{

PathConstraint PathConstraint::New( Dali::Path path, const Vector2& range )
{
  Internal::PathConstraint* internal = Internal::PathConstraint::New(GetImplementation(path), range);
  return PathConstraint(internal);
}

PathConstraint PathConstraint::DownCast( BaseHandle handle )
{
  return PathConstraint( dynamic_cast<Dali::Internal::PathConstraint*>(handle.GetObjectPtr()) );
}

PathConstraint::PathConstraint()
{
}

PathConstraint::~PathConstraint()
{
}

PathConstraint::PathConstraint(const PathConstraint& handle)
:Handle(handle)
{
}

PathConstraint::PathConstraint(Internal::PathConstraint* internal)
: Handle(internal)
{
}

PathConstraint& PathConstraint::operator=(const PathConstraint& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void PathConstraint::Apply( Dali::Property source, Dali::Property target, const Vector3& forward )
{
  GetImplementation(*this).Apply( source, target, forward );
}

void PathConstraint::Remove( Dali::Handle target )
{
  GetImplementation(*this).Remove( target );
}


} // Dali
