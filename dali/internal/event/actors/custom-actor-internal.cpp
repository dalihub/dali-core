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

// CLASS HEADER
#include <dali/internal/event/actors/custom-actor-internal.h>
#include <dali/public-api/object/type-info.h>

namespace Dali
{

namespace Internal
{

CustomActorPtr CustomActor::New(CustomActorImpl& extension)
{
  CustomActorPtr actor(new CustomActor(extension));

  // Second-phase construction
  extension.Initialize(*actor);
  actor->Initialize();

  return actor;
}

CustomActor::CustomActor(CustomActorImpl& extension)
: Actor( Actor::BASIC ),
  mImpl( &extension )
{
  mDerivedRequiresTouch = extension.RequiresTouchEvents();
  mDerivedRequiresHover = extension.RequiresHoverEvents();
  mDerivedRequiresWheelEvent = extension.RequiresWheelEvents();
  SetRelayoutEnabled( extension.IsRelayoutEnabled() );
}

CustomActor::~CustomActor()
{
}

Dali::TypeInfo CustomActor::GetTypeInfo()
{
  Dali::TypeInfo handle ( const_cast<Dali::Internal::TypeInfo*>(Object::GetTypeInfo()) );
  return handle;
}

} // namespace Internal

} // namespace Dali
