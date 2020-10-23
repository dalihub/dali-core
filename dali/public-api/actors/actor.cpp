/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>

namespace Dali
{
Actor::Actor() = default;

Actor Actor::New()
{
  Internal::ActorPtr internal = Internal::Actor::New();

  return Actor(internal.Get());
}

Actor Actor::DownCast(BaseHandle handle)
{
  return Actor(dynamic_cast<Dali::Internal::Actor*>(handle.GetObjectPtr()));
}

Actor::~Actor() = default;

Actor::Actor(const Actor& copy) = default;

Actor& Actor::operator=(const Actor& rhs) = default;

Actor::Actor(Actor&& rhs) = default;

Actor& Actor::operator=(Actor&& rhs) = default;

Layer Actor::GetLayer()
{
  return GetImplementation(*this).GetLayer();
}

void Actor::Add(Actor actor)
{
  GetImplementation(*this).Add(GetImplementation(actor));
}

void Actor::Remove(Actor actor)
{
  GetImplementation(*this).Remove(GetImplementation(actor));
}

void Actor::Unparent()
{
  GetImplementation(*this).Unparent();
}

uint32_t Actor::GetChildCount() const
{
  return GetImplementation(*this).GetChildCount();
}

Actor Actor::GetChildAt(uint32_t index) const
{
  Internal::ActorPtr child = GetImplementation(*this).GetChildAt(index);
  return Actor(child.Get());
}

Actor Actor::FindChildByName(const std::string& actorName)
{
  Internal::ActorPtr child = GetImplementation(*this).FindChildByName(actorName);
  return Actor(child.Get());
}

Actor Actor::FindChildById(const uint32_t id)
{
  Internal::ActorPtr child = GetImplementation(*this).FindChildById(id);
  return Actor(child.Get());
}

Actor Actor::GetParent() const
{
  Internal::Actor* parent = GetImplementation(*this).GetParent();

  return Actor(parent);
}

Vector3 Actor::GetTargetSize() const
{
  return GetImplementation(*this).GetTargetSize();
}

Vector3 Actor::GetNaturalSize() const
{
  return GetImplementation(*this).GetNaturalSize();
}

void Actor::TranslateBy(const Vector3& distance)
{
  GetImplementation(*this).TranslateBy(distance);
}

void Actor::RotateBy(const Radian& angle, const Vector3& axis)
{
  GetImplementation(*this).RotateBy(angle, axis);
}

void Actor::RotateBy(const Quaternion& relativeRotation)
{
  GetImplementation(*this).RotateBy(relativeRotation);
}

void Actor::ScaleBy(const Vector3& relativeScale)
{
  GetImplementation(*this).ScaleBy(relativeScale);
}

bool Actor::ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const
{
  return GetImplementation(*this).ScreenToLocal(localX, localY, screenX, screenY);
}

void Actor::Raise()
{
  GetImplementation(*this).Raise();
}

void Actor::Lower()
{
  GetImplementation(*this).Lower();
}

void Actor::RaiseToTop()
{
  GetImplementation(*this).RaiseToTop();
}

void Actor::LowerToBottom()
{
  GetImplementation(*this).LowerToBottom();
}

void Actor::RaiseAbove(Actor target)
{
  GetImplementation(*this).RaiseAbove(GetImplementation(target));
}

void Actor::LowerBelow(Actor target)
{
  GetImplementation(*this).LowerBelow(GetImplementation(target));
}

void Actor::SetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
  GetImplementation(*this).SetResizePolicy(policy, dimension);
}

ResizePolicy::Type Actor::GetResizePolicy(Dimension::Type dimension) const
{
  return GetImplementation(*this).GetResizePolicy(dimension);
}

float Actor::GetHeightForWidth(float width)
{
  return GetImplementation(*this).GetHeightForWidth(width);
}

float Actor::GetWidthForHeight(float height)
{
  return GetImplementation(*this).GetWidthForHeight(height);
}

float Actor::GetRelayoutSize(Dimension::Type dimension) const
{
  return GetImplementation(*this).GetRelayoutSize(dimension);
}

Actor::TouchEventSignalType& Actor::TouchedSignal()
{
  return GetImplementation(*this).TouchedSignal();
}

Actor::HoverSignalType& Actor::HoveredSignal()
{
  return GetImplementation(*this).HoveredSignal();
}

Actor::WheelEventSignalType& Actor::WheelEventSignal()
{
  return GetImplementation(*this).WheelEventSignal();
}

Actor::OnSceneSignalType& Actor::OnSceneSignal()
{
  return GetImplementation(*this).OnSceneSignal();
}

Actor::OffSceneSignalType& Actor::OffSceneSignal()
{
  return GetImplementation(*this).OffSceneSignal();
}

uint32_t Actor::AddRenderer(Renderer& renderer)
{
  return GetImplementation(*this).AddRenderer(GetImplementation(renderer));
}

uint32_t Actor::GetRendererCount() const
{
  return GetImplementation(*this).GetRendererCount();
}

Renderer Actor::GetRendererAt(uint32_t index)
{
  Internal::RendererPtr renderer = GetImplementation(*this).GetRendererAt(index);
  return Renderer(renderer.Get());
}

void Actor::RemoveRenderer(Renderer& renderer)
{
  GetImplementation(*this).RemoveRenderer(GetImplementation(renderer));
}

void Actor::RemoveRenderer(uint32_t index)
{
  GetImplementation(*this).RemoveRenderer(index);
}

Actor::OnRelayoutSignalType& Actor::OnRelayoutSignal()
{
  return GetImplementation(*this).OnRelayoutSignal();
}

Actor::LayoutDirectionChangedSignalType& Actor::LayoutDirectionChangedSignal()
{
  return GetImplementation(*this).LayoutDirectionChangedSignal();
}

Actor::Actor(Internal::Actor* internal)
: Handle(internal)
{
}

} // namespace Dali
