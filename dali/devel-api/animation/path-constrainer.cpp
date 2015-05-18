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
#include <dali/devel-api/animation/path-constrainer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/path-constrainer-impl.h>
#include <dali/internal/event/animation/path-impl.h>

namespace Dali
{

PathConstrainer PathConstrainer::New()
{
  Internal::PathConstrainer* internal = Internal::PathConstrainer::New();
  return PathConstrainer(internal);
}

PathConstrainer PathConstrainer::DownCast( BaseHandle handle )
{
  return PathConstrainer( dynamic_cast<Dali::Internal::PathConstrainer*>(handle.GetObjectPtr()) );
}

PathConstrainer::PathConstrainer()
{
}

PathConstrainer::~PathConstrainer()
{
}

PathConstrainer::PathConstrainer(const PathConstrainer& handle)
:Handle(handle)
{
}

PathConstrainer::PathConstrainer(Internal::PathConstrainer* internal)
: Handle(internal)
{
}

PathConstrainer& PathConstrainer::operator=(const PathConstrainer& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void PathConstrainer::Apply( Dali::Property target, Dali::Property source, const Vector2& range, const Vector2& wrap )
{
  GetImplementation(*this).Apply( target, source, range, wrap );
}

void PathConstrainer::Remove( Dali::Handle& target )
{
  GetImplementation(*this).Remove( target );
}


} // Dali
