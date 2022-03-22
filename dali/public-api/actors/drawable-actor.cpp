/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/drawable-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/drawable-actor-impl.h>

using DrawableActorImpl = Dali::Internal::DrawableActor;

namespace Dali
{
DrawableActor DrawableActor::New(RenderCallback& callback)
{
  auto internal = Internal::DrawableActor::New(&callback);
  return DrawableActor(internal.Get());
}

DrawableActor::DrawableActor(Internal::DrawableActor* internal)
: Actor(internal)
{
}

} // namespace Dali