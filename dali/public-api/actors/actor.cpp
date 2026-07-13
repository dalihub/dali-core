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

// CLASS HEADER
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>

#include <dali/integration-api/string-utils.h>
#include <dali/internal/common/const-string.h>
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

Actor::Actor(Actor&& rhs) noexcept = default;

Actor& Actor::operator=(Actor&& rhs) noexcept = default;

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

Actor Actor::FindChildByName(Dali::StringView actorName)
{
  Internal::ActorPtr child = GetImplementation(*this).FindChildByName(Integration::ToStdStringView(actorName));
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

// The property accessor methods below follow two deliberate conventions:
//
// - Setters route through SetProperty() rather than calling the matching internal
//   Actor::SetXXX() directly. SetProperty() (via Object::SetProperty) is the single
//   point that fires OnPropertySet() and the PropertySetSignal, so routing here
//   guarantees that setting a property through this convenience API is observably
//   identical to setting it via the property system. Calling the internal setter
//   directly would silently skip those notifications. (SetResizePolicy() is an
//   exception: it takes two arguments and has no single matching property.)
//
// - Getters call the internal GetXXX() directly instead of GetProperty(). There is
//   no notification side-effect to centralise on the get side, so routing through
//   GetProperty() would only add needless Property::Value boxing and risk returning
//   the base value where a distinct current (animating) value is expected. The
//   current value, where relevant, is exposed via separate GetCurrentXXX() methods.
void Actor::SetParentOrigin(const Vector3& origin)
{
  GetImplementation(*this).SetProperty(Actor::Property::PARENT_ORIGIN, origin);
}

Vector3 Actor::GetParentOrigin() const
{
  return GetImplementation(*this).GetParentOrigin();
}

void Actor::SetParentOriginX(float x)
{
  GetImplementation(*this).SetProperty(Actor::Property::PARENT_ORIGIN_X, x);
}

float Actor::GetParentOriginX() const
{
  return GetImplementation(*this).GetParentOriginX();
}

void Actor::SetParentOriginY(float y)
{
  GetImplementation(*this).SetProperty(Actor::Property::PARENT_ORIGIN_Y, y);
}

float Actor::GetParentOriginY() const
{
  return GetImplementation(*this).GetParentOriginY();
}

void Actor::SetParentOriginZ(float z)
{
  GetImplementation(*this).SetProperty(Actor::Property::PARENT_ORIGIN_Z, z);
}

float Actor::GetParentOriginZ() const
{
  return GetImplementation(*this).GetParentOriginZ();
}

void Actor::SetPivot(const Vector3& pivot)
{
  GetImplementation(*this).SetProperty(Actor::Property::PIVOT, pivot);
}

Vector3 Actor::GetPivot() const
{
  return GetImplementation(*this).GetPivot();
}

void Actor::SetPivotX(float x)
{
  GetImplementation(*this).SetProperty(Actor::Property::PIVOT_X, x);
}

float Actor::GetPivotX() const
{
  return GetImplementation(*this).GetPivotX();
}

void Actor::SetPivotY(float y)
{
  GetImplementation(*this).SetProperty(Actor::Property::PIVOT_Y, y);
}

float Actor::GetPivotY() const
{
  return GetImplementation(*this).GetPivotY();
}

void Actor::SetPivotZ(float z)
{
  GetImplementation(*this).SetProperty(Actor::Property::PIVOT_Z, z);
}

float Actor::GetPivotZ() const
{
  return GetImplementation(*this).GetPivotZ();
}

void Actor::SetSize(const Vector3& size)
{
  GetImplementation(*this).SetProperty(Actor::Property::SIZE, size);
}

Vector3 Actor::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

Vector3 Actor::GetCurrentSize() const
{
  return GetImplementation(*this).GetCurrentSize();
}

void Actor::SetWidth(float width)
{
  GetImplementation(*this).SetProperty(Actor::Property::SIZE_WIDTH, width);
}

float Actor::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

void Actor::SetHeight(float height)
{
  GetImplementation(*this).SetProperty(Actor::Property::SIZE_HEIGHT, height);
}

float Actor::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

void Actor::SetDepth(float depth)
{
  GetImplementation(*this).SetProperty(Actor::Property::SIZE_DEPTH, depth);
}

float Actor::GetDepth() const
{
  return GetImplementation(*this).GetDepth();
}

void Actor::SetPosition(const Vector3& position)
{
  GetImplementation(*this).SetProperty(Actor::Property::POSITION, position);
}

void Actor::SetPositionX(float x)
{
  GetImplementation(*this).SetProperty(Actor::Property::POSITION_X, x);
}

float Actor::GetPositionX() const
{
  return GetImplementation(*this).GetX();
}

void Actor::SetPositionY(float y)
{
  GetImplementation(*this).SetProperty(Actor::Property::POSITION_Y, y);
}

float Actor::GetPositionY() const
{
  return GetImplementation(*this).GetY();
}

void Actor::SetPositionZ(float z)
{
  GetImplementation(*this).SetProperty(Actor::Property::POSITION_Z, z);
}

float Actor::GetPositionZ() const
{
  return GetImplementation(*this).GetZ();
}

Vector3 Actor::GetPosition() const
{
  return GetImplementation(*this).GetPosition();
}

Vector3 Actor::GetCurrentPosition() const
{
  return GetImplementation(*this).GetCurrentPosition();
}

Vector3 Actor::GetWorldPosition() const
{
  return GetImplementation(*this).GetWorldPosition();
}

float Actor::GetWorldPositionX() const
{
  return GetImplementation(*this).GetWorldPositionX();
}

float Actor::GetWorldPositionY() const
{
  return GetImplementation(*this).GetWorldPositionY();
}

float Actor::GetWorldPositionZ() const
{
  return GetImplementation(*this).GetWorldPositionZ();
}

void Actor::SetOrientation(const Quaternion& orientation)
{
  GetImplementation(*this).SetProperty(Actor::Property::ORIENTATION, orientation);
}

Quaternion Actor::GetOrientation() const
{
  return GetImplementation(*this).GetOrientation();
}

Quaternion Actor::GetWorldOrientation() const
{
  return GetImplementation(*this).GetWorldOrientation();
}

void Actor::SetScale(const Vector3& scale)
{
  GetImplementation(*this).SetProperty(Actor::Property::SCALE, scale);
}

Vector3 Actor::GetScale() const
{
  return GetImplementation(*this).GetScale();
}

void Actor::SetScaleX(float scaleX)
{
  GetImplementation(*this).SetProperty(Actor::Property::SCALE_X, scaleX);
}

float Actor::GetScaleX() const
{
  return GetImplementation(*this).GetScaleX();
}

void Actor::SetScaleY(float scaleY)
{
  GetImplementation(*this).SetProperty(Actor::Property::SCALE_Y, scaleY);
}

float Actor::GetScaleY() const
{
  return GetImplementation(*this).GetScaleY();
}

void Actor::SetScaleZ(float scaleZ)
{
  GetImplementation(*this).SetProperty(Actor::Property::SCALE_Z, scaleZ);
}

float Actor::GetScaleZ() const
{
  return GetImplementation(*this).GetScaleZ();
}

Vector3 Actor::GetWorldScale() const
{
  return GetImplementation(*this).GetWorldScale();
}

void Actor::SetVisible(bool visible)
{
  GetImplementation(*this).SetProperty(Actor::Property::VISIBLE, visible);
}

bool Actor::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

void Actor::SetColor(const Vector4& color)
{
  GetImplementation(*this).SetProperty(Actor::Property::COLOR, color);
}

Vector4 Actor::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

Vector4 Actor::GetCurrentColor() const
{
  return GetImplementation(*this).GetCurrentColor();
}

void Actor::SetColorRed(float red)
{
  GetImplementation(*this).SetProperty(Actor::Property::COLOR_RED, red);
}

float Actor::GetColorRed() const
{
  return GetImplementation(*this).GetColorRed();
}

void Actor::SetColorGreen(float green)
{
  GetImplementation(*this).SetProperty(Actor::Property::COLOR_GREEN, green);
}

float Actor::GetColorGreen() const
{
  return GetImplementation(*this).GetColorGreen();
}

void Actor::SetColorBlue(float blue)
{
  GetImplementation(*this).SetProperty(Actor::Property::COLOR_BLUE, blue);
}

float Actor::GetColorBlue() const
{
  return GetImplementation(*this).GetColorBlue();
}

void Actor::SetColorAlpha(float alpha)
{
  GetImplementation(*this).SetProperty(Actor::Property::COLOR_ALPHA, alpha);
}

float Actor::GetColorAlpha() const
{
  return GetImplementation(*this).GetColorAlpha();
}

Vector4 Actor::GetWorldColor() const
{
  return GetImplementation(*this).GetWorldColor();
}

Matrix Actor::GetWorldMatrix() const
{
  return GetImplementation(*this).GetWorldMatrix();
}

void Actor::SetName(Dali::StringView name)
{
  GetImplementation(*this).SetProperty(Actor::Property::NAME, Dali::Property::Value(Dali::String(name)));
}

Dali::String Actor::GetName() const
{
  return Dali::String(GetImplementation(*this).GetName().data());
}

void Actor::SetSensitive(bool sensitive)
{
  GetImplementation(*this).SetProperty(Actor::Property::SENSITIVE, sensitive);
}

bool Actor::IsSensitive() const
{
  return GetImplementation(*this).IsSensitive();
}

void Actor::SetLeaveRequired(bool required)
{
  GetImplementation(*this).SetProperty(Actor::Property::LEAVE_REQUIRED, required);
}

bool Actor::GetLeaveRequired() const
{
  return GetImplementation(*this).GetLeaveRequired();
}

void Actor::SetTouchHitAreaMargin(const Extents& margin)
{
  GetImplementation(*this).SetProperty(Actor::Property::TOUCH_HIT_AREA_MARGIN, margin);
}

Extents Actor::GetTouchHitAreaMargin() const
{
  return GetImplementation(*this).GetTouchHitAreaMargin();
}

void Actor::SetAllowSelfInitiatedTouchOnlyEnabled(bool enabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::ALLOW_SELF_INITIATED_TOUCH_ONLY, enabled);
}

bool Actor::IsAllowSelfInitiatedTouchOnlyEnabled() const
{
  return GetImplementation(*this).IsAllowSelfInitiatedTouchOnlyEnabled();
}

void Actor::SetDispatchTouchMotionEnabled(bool enabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::DISPATCH_TOUCH_MOTION, enabled);
}

bool Actor::IsDispatchTouchMotionEnabled() const
{
  return GetImplementation(*this).IsDispatchTouchMotionEnabled();
}

void Actor::SetDispatchHoverMotionEnabled(bool enabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::DISPATCH_HOVER_MOTION, enabled);
}

bool Actor::IsDispatchHoverMotionEnabled() const
{
  return GetImplementation(*this).IsDispatchHoverMotionEnabled();
}

void Actor::SetInheritOrientationEnabled(bool inherit)
{
  GetImplementation(*this).SetProperty(Actor::Property::INHERIT_ORIENTATION, inherit);
}

bool Actor::IsInheritOrientationEnabled() const
{
  return GetImplementation(*this).IsOrientationInherited();
}

void Actor::SetInheritScaleEnabled(bool inherit)
{
  GetImplementation(*this).SetProperty(Actor::Property::INHERIT_SCALE, inherit);
}

bool Actor::IsInheritScaleEnabled() const
{
  return GetImplementation(*this).IsScaleInherited();
}

void Actor::SetInheritPositionEnabled(bool inherit)
{
  GetImplementation(*this).SetProperty(Actor::Property::INHERIT_POSITION, inherit);
}

bool Actor::IsInheritPositionEnabled() const
{
  return GetImplementation(*this).IsPositionInherited();
}

void Actor::SetColorMode(ColorMode colorMode)
{
  GetImplementation(*this).SetProperty(Actor::Property::COLOR_MODE, colorMode);
}

ColorMode Actor::GetColorMode() const
{
  return GetImplementation(*this).GetColorMode();
}

void Actor::SetDrawMode(DrawMode::Type drawMode)
{
  GetImplementation(*this).SetProperty(Actor::Property::DRAW_MODE, drawMode);
}

DrawMode::Type Actor::GetDrawMode() const
{
  return GetImplementation(*this).GetDrawMode();
}

void Actor::SetBlendEquation(Dali::BlendEquation::Type blendEquation)
{
  GetImplementation(*this).SetProperty(Actor::Property::BLEND_EQUATION, blendEquation);
}

Dali::BlendEquation::Type Actor::GetBlendEquation() const
{
  return GetImplementation(*this).GetBlendEquation();
}

void Actor::SetClippingMode(ClippingMode::Type clippingMode)
{
  GetImplementation(*this).SetProperty(Actor::Property::CLIPPING_MODE, clippingMode);
}

ClippingMode::Type Actor::GetClippingMode() const
{
  return GetImplementation(*this).GetClippingMode();
}

void Actor::SetLayoutDirection(LayoutDirection::Type layoutDirection)
{
  GetImplementation(*this).SetProperty(Actor::Property::LAYOUT_DIRECTION, layoutDirection);
}

LayoutDirection::Type Actor::GetLayoutDirection() const
{
  return GetImplementation(*this).GetLayoutDirection();
}

void Actor::SetInheritLayoutDirectionEnabled(bool enabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::INHERIT_LAYOUT_DIRECTION, enabled);
}

bool Actor::IsInheritLayoutDirectionEnabled() const
{
  return GetImplementation(*this).IsInheritLayoutDirectionEnabled();
}

void Actor::SetOpacity(float opacity)
{
  GetImplementation(*this).SetProperty(Actor::Property::OPACITY, opacity);
}

float Actor::GetOpacity() const
{
  return GetImplementation(*this).GetOpacity();
}

Vector2 Actor::GetScreenPosition() const
{
  return GetImplementation(*this).GetScreenPosition();
}

void Actor::SetPositionUsesPivotEnabled(bool enabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::POSITION_USES_PIVOT, enabled);
}

bool Actor::IsPositionUsesPivotEnabled() const
{
  return GetImplementation(*this).IsPositionUsesPivotEnabled();
}

bool Actor::IsCulled() const
{
  return GetImplementation(*this).IsCulled();
}

uint32_t Actor::GetId() const
{
  return GetImplementation(*this).GetId();
}

bool Actor::IsRoot() const
{
  return GetImplementation(*this).IsRoot();
}

bool Actor::IsLayer() const
{
  return GetImplementation(*this).IsLayer();
}

bool Actor::IsConnectedToScene() const
{
  return GetImplementation(*this).IsConnectedToScene();
}

void Actor::SetFocusable(bool focusable)
{
  GetImplementation(*this).SetProperty(Actor::Property::FOCUSABLE, focusable);
}

bool Actor::IsFocusable() const
{
  return GetImplementation(*this).IsFocusable();
}

void Actor::SetFocusOnTouchEnabled(bool focusOnTouchEnabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::FOCUS_ON_TOUCH, focusOnTouchEnabled);
}

bool Actor::IsFocusOnTouchEnabled() const
{
  return GetImplementation(*this).IsFocusOnTouchEnabled();
}

void Actor::SetAllowDescendantFocusEnabled(bool allowDescendantFocusEnabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::ALLOW_DESCENDANT_FOCUS, allowDescendantFocusEnabled);
}

bool Actor::IsAllowDescendantFocusEnabled() const
{
  return GetImplementation(*this).IsAllowDescendantFocusEnabled();
}

bool Actor::HasAncestorBlockingFocus() const
{
  return GetImplementation(*this).HasAncestorBlockingFocus();
}

void Actor::SetEnabled(bool enabled)
{
  GetImplementation(*this).SetProperty(Actor::Property::ENABLED, enabled);
}

bool Actor::IsEnabled() const
{
  return GetImplementation(*this).IsEnabled();
}

void Actor::SetUpdateAreaHint(const Vector4& hint)
{
  GetImplementation(*this).SetProperty(Actor::Property::UPDATE_AREA_HINT, hint);
}

Vector4 Actor::GetUpdateAreaHint() const
{
  return GetImplementation(*this).GetUpdateAreaHint();
}

bool Actor::ScreenToLocal(float& localX, float& localY, float screenX, float screenY) const
{
  return GetImplementation(*this).ScreenToLocal(localX, localY, screenX, screenY);
}

Vector2 Actor::CalculateScreenPosition() const
{
  return GetImplementation(*this).CalculateScreenPosition();
}

Bounds Actor::CalculateScreenExtents() const
{
  return GetImplementation(*this).CalculateScreenExtents();
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

void Actor::SetIgnored(bool ignored)
{
  GetImplementation(*this).SetProperty(Actor::Property::IGNORED, ignored);
}

bool Actor::IsIgnored() const
{
  return GetImplementation(*this).IsIgnored();
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

uint32_t Actor::AddCacheRenderer(Renderer& renderer)
{
  return GetImplementation(*this).AddCacheRenderer(GetImplementation(renderer));
}

uint32_t Actor::GetCacheRendererCount() const
{
  return GetImplementation(*this).GetCacheRendererCount();
}

void Actor::RemoveCacheRenderer(Renderer& renderer)
{
  GetImplementation(*this).RemoveCacheRenderer(GetImplementation(renderer));
}

Actor::TouchEventSignalType& Actor::TouchEventSignal()
{
  return GetImplementation(*this).TouchEventSignal();
}

Actor::HoverEventSignalType& Actor::HoverEventSignal()
{
  return GetImplementation(*this).HoverEventSignal();
}

Actor::WheelEventSignalType& Actor::WheelEventSignal()
{
  return GetImplementation(*this).WheelEventSignal();
}

Actor::TouchEventSignalType& Actor::InterceptTouchEventSignal()
{
  return GetImplementation(*this).InterceptTouchEventSignal();
}

Actor::WheelEventSignalType& Actor::InterceptWheelEventSignal()
{
  return GetImplementation(*this).InterceptWheelEventSignal();
}

Actor::SceneConnectedSignalType& Actor::SceneConnectedSignal()
{
  return GetImplementation(*this).SceneConnectedSignal();
}

Actor::SceneDisconnectedSignalType& Actor::SceneDisconnectedSignal()
{
  return GetImplementation(*this).SceneDisconnectedSignal();
}

Actor::ChildAddedSignalType& Actor::ChildAddedSignal()
{
  return GetImplementation(*this).ChildAddedSignal();
}

Actor::ChildRemovedSignalType& Actor::ChildRemovedSignal()
{
  return GetImplementation(*this).ChildRemovedSignal();
}

Actor::VisibilityChangedSignalType& Actor::VisibilityChangedSignal()
{
  return GetImplementation(*this).VisibilityChangedSignal();
}

Actor::EffectiveVisibilityChangedSignalType& Actor::EffectiveVisibilityChangedSignal()
{
  return GetImplementation(*this).EffectiveVisibilityChangedSignal();
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
