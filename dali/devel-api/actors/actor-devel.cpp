/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/actor-coords.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{
namespace DevelActor
{
Vector2 CalculateScreenPosition(Actor actor)
{
  return GetImplementation(actor).CalculateScreenPosition();
}

Rect<> CalculateScreenExtents(Actor actor)
{
  return GetImplementation(actor).CalculateScreenExtents();
}

Rect<> CalculateCurrentScreenExtents(Actor actor)
{
  return GetImplementation(actor).CalculateCurrentScreenExtents();
}

VisibilityChangedSignalType& VisibilityChangedSignal(Actor actor)
{
  return GetImplementation(actor).VisibilityChangedSignal();
}

Actor GetVisiblityChangedActor()
{
  return Dali::Actor(Internal::Actor::GetVisiblityChangedActor().Get());
}

ChildChangedSignalType& ChildAddedSignal(Actor actor)
{
  return GetImplementation(actor).ChildAddedSignal();
}

ChildChangedSignalType& ChildRemovedSignal(Actor actor)
{
  return GetImplementation(actor).ChildRemovedSignal();
}

ChildOrderChangedSignalType& ChildOrderChangedSignal(Actor actor)
{
  return GetImplementation(actor).ChildOrderChangedSignal();
}

Actor::TouchEventSignalType& InterceptTouchedSignal(Actor actor)
{
  return GetImplementation(actor).InterceptTouchedSignal();
}

Actor::WheelEventSignalType& InterceptWheelSignal(Actor actor)
{
  return GetImplementation(actor).InterceptWheelSignal();
}

void SetNeedGesturePropagation(Actor actor, bool propagation)
{
  return GetImplementation(actor).SetNeedGesturePropagation(propagation);
}

void SwitchParent(Actor actor, Actor newParent)
{
  return GetImplementation(actor).SwitchParent(GetImplementation(newParent));
}

Actor::TouchEventSignalType& HitTestResultSignal(Actor actor)
{
  return GetImplementation(actor).HitTestResultSignal();
}

Matrix GetWorldTransform(Actor actor)
{
  return CalculateActorWorldTransform(GetImplementation(actor));
}

Vector4 GetWorldColor(Actor actor)
{
  return CalculateActorWorldColor(GetImplementation(actor));
}

bool IsEffectivelyVisible(Actor actor)
{
  return CalculateActorInheritedVisible(GetImplementation(actor));
}

void LookAt(Actor actor, Vector3 target, Vector3 up, Vector3 localForward, Vector3 localUp)
{
  auto orientation = CalculateActorLookAtOrientation(GetImplementation(actor), target, up, localForward, localUp);
  GetImplementation(actor).SetOrientation(orientation);
}

bool IsHittable(Actor actor)
{
  return GetImplementation(actor).IsHittable();
}

bool GetTouchRequired(Actor actor)
{
  return GetImplementation(actor).GetTouchRequired();
}

} // namespace DevelActor

} // namespace Dali
