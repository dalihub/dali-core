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
#include <dali/public-api/animation/linear-constrainer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/linear-constrainer-impl.h>


namespace Dali
{

LinearConstrainer LinearConstrainer::New()
{
  Internal::LinearConstrainer* internal = Internal::LinearConstrainer::New();
  return LinearConstrainer(internal);
}

LinearConstrainer LinearConstrainer::DownCast( BaseHandle handle )
{
  return LinearConstrainer( dynamic_cast<Dali::Internal::LinearConstrainer*>(handle.GetObjectPtr()) );
}

LinearConstrainer::LinearConstrainer()
{
}

LinearConstrainer::~LinearConstrainer()
{
}

LinearConstrainer::LinearConstrainer(const LinearConstrainer& handle)
:Handle(handle)
{
}

LinearConstrainer::LinearConstrainer(Internal::LinearConstrainer* internal)
: Handle(internal)
{
}

LinearConstrainer& LinearConstrainer::operator=(const LinearConstrainer& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void LinearConstrainer::Apply( Dali::Property target, Dali::Property source, const Vector2& range, const Vector2& wrap )
{
  GetImplementation(*this).Apply( target, source, range, wrap );
}

void LinearConstrainer::Remove( Dali::Handle& target )
{
  GetImplementation(*this).Remove( target );
}


} // Dali
