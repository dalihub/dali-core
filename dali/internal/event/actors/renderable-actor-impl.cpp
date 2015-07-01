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
#include <dali/internal/event/actors/renderable-actor-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>

namespace // unnamed namespace
{

using namespace Dali;

/*
 * This may look like a no-op but maintains TypeRegistry chain of classes.
 * ie if a child actor declares its base as RenderableActor, RenderableActor must exist
 * in TypeRegistry otherwise it doesnt know that child is related to Actor.
 */
BaseHandle Create()
{
  return BaseHandle();
}

TypeRegistration mType( typeid(Dali::RenderableActor), typeid(Dali::Actor), Create );

} // unnamed namespace

namespace Dali
{

namespace Internal
{

RenderableActor::RenderableActor()
: Actor( Actor::RENDERABLE )
{
}

RenderableActor::~RenderableActor()
{
}


} // namespace Internal

} // namespace Dali
