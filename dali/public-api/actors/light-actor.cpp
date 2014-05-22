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
#include <dali/public-api/actors/light-actor.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/light-actor-impl.h>
#include <dali/public-api/common/light.h>

namespace Dali
{

LightActor::LightActor()
{
}

LightActor LightActor::New()
{
  Internal::LightActorPtr internal = Internal::LightActor::New();

  return LightActor(internal.Get());
}

LightActor LightActor::DownCast( BaseHandle handle )
{
  return LightActor( dynamic_cast<Dali::Internal::LightActor*>(handle.GetObjectPtr()) );
}

LightActor::~LightActor()
{
}

void LightActor::SetLight(Light light)
{
  GetImplementation(*this).SetLight(light);
}

Light LightActor::GetLight()
{
  return GetImplementation(*this).GetLight();
}

void LightActor::SetActive(bool active)
{
  GetImplementation(*this).SetActive(active);
}

bool LightActor::GetActive()
{
  return GetImplementation(*this).GetActive();
}

LightActor::LightActor(Internal::LightActor* internal)
: Actor(internal)
{
}

} // namespace Dali
