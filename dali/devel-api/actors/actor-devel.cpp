/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

namespace DevelActor
{

void Raise( Actor actor )
{
  GetImplementation( actor ).Raise();
}

void Lower( Actor actor )
{
  GetImplementation( actor ).Lower();
}

void RaiseToTop( Actor actor )
{
  GetImplementation( actor ).RaiseToTop();
}

void LowerToBottom( Actor actor )
{
  GetImplementation( actor ).LowerToBottom();
}

void RaiseAbove( Actor actor, Dali::Actor target )
{
  GetImplementation( actor ).RaiseAbove( target );
}

void LowerBelow( Actor actor, Actor target )
{
  GetImplementation( actor ).LowerBelow( target);
}

} // namespace DevelActor

} // namespace Dali
