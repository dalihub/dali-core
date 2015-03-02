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
#include <dali/public-api/actors/renderable-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/renderable-actor-impl.h>
#include <dali/internal/event/effects/shader-effect-impl.h>

namespace Dali
{

const BlendingMode::Type RenderableActor::DEFAULT_BLENDING_MODE = BlendingMode::AUTO;

RenderableActor::RenderableActor()
{
}

RenderableActor RenderableActor::DownCast( BaseHandle handle )
{
  return RenderableActor( dynamic_cast<Dali::Internal::RenderableActor*>(handle.GetObjectPtr()) );
}

RenderableActor::~RenderableActor()
{
}

RenderableActor::RenderableActor(const RenderableActor& copy)
: Actor(copy)
{
}

RenderableActor& RenderableActor::operator=(const RenderableActor& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}


RenderableActor::RenderableActor(Internal::RenderableActor* internal)
: Actor(internal)
{
}


} // namespace Dali
