/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/scene.h>

// EXTERNAL INCLUDES
#include <dali/graphics-api/graphics-render-target.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/render-tasks/render-task-list.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/scene-impl.h>

namespace Dali
{
namespace Integration
{
Scene Scene::New(Size size, int32_t windowOrientation, int32_t screenOrientation)
{
  Internal::ScenePtr internal = Internal::Scene::New(size, windowOrientation, screenOrientation);
  return Scene(internal.Get());
}

Scene Scene::DownCast(BaseHandle handle)
{
  return Scene(dynamic_cast<Dali::Internal::Scene*>(handle.GetObjectPtr()));
}

Scene::Scene() = default;

Scene::~Scene() = default;

Scene::Scene(const Scene& handle) = default;

Scene& Scene::operator=(const Scene& rhs) = default;

Scene::Scene(Scene&& handle) noexcept = default;

Scene& Scene::operator=(Scene&& rhs) noexcept = default;

Scene::Scene(Internal::Scene* internal)
: BaseHandle(internal)
{
}

void Scene::Add(Actor actor)
{
  GetImplementation(*this).Add(GetImplementation(actor));
}

void Scene::Remove(Actor actor)
{
  GetImplementation(*this).Remove(GetImplementation(actor));
}

void Scene::Show()
{
  GetImplementation(*this).Show();
}

void Scene::Hide()
{
  GetImplementation(*this).Hide();
}

bool Scene::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

Size Scene::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

void Scene::SetDpi(Vector2 dpi)
{
  GetImplementation(*this).SetDpi(dpi);
}

Vector2 Scene::GetDpi() const
{
  return GetImplementation(*this).GetDpi();
}

void Scene::SetBackgroundColor(const Vector4& color)
{
  GetImplementation(*this).SetBackgroundColor(color);
}

Vector4 Scene::GetBackgroundColor() const
{
  return GetImplementation(*this).GetBackgroundColor();
}

RenderTaskList Scene::GetRenderTaskList() const
{
  return RenderTaskList(&GetImplementation(*this).GetRenderTaskList());
}

Layer Scene::GetRootLayer() const
{
  return GetImplementation(*this).GetRootLayer();
}

Layer Scene::GetOverlayLayer()
{
  return GetImplementation(*this).GetOverlayLayer();
}

uint32_t Scene::GetLayerCount() const
{
  return GetImplementation(*this).GetLayerCount();
}

Layer Scene::GetLayer(uint32_t depth) const
{
  return GetImplementation(*this).GetLayer(depth);
}

void Scene::SurfaceResized(float width, float height)
{
  GetImplementation(*this).SurfaceResized(width, height);
}

void Scene::SurfaceReplaced()
{
  GetImplementation(*this).SurfaceReplaced();
}

void Scene::RemoveSceneObject()
{
  GetImplementation(*this).RemoveSceneObject();
}

void Scene::Discard()
{
  GetImplementation(*this).Discard();
}

void Scene::SetSurfaceRenderTarget(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo)
{
  GetImplementation(*this).SetSurfaceRenderTarget(renderTargetCreateInfo);
}

Integration::Scene Scene::Get(Actor actor)
{
  return Dali::Integration::Scene(&GetImplementation(actor).GetScene());
}

void Scene::QueueEvent(const Integration::Event& event)
{
  GetImplementation(*this).QueueEvent(event);
}

void Scene::ProcessEvents()
{
  GetImplementation(*this).ProcessEvents();
}

void Scene::AddFrameRenderedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId)
{
  GetImplementation(*this).AddFrameRenderedCallback(std::move(callback), frameId);
}

void Scene::AddFramePresentedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId)
{
  GetImplementation(*this).AddFramePresentedCallback(std::move(callback), frameId);
}

void Scene::GetFrameRenderedCallback(FrameCallbackContainer& callbacks)
{
  GetImplementation(*this).GetFrameRenderedCallback(callbacks);
}

void Scene::GetFramePresentedCallback(FrameCallbackContainer& callbacks)
{
  GetImplementation(*this).GetFramePresentedCallback(callbacks);
}

void Scene::SurfaceRotated(float width, float height, int32_t windowOrientation, int32_t screenOrientation)
{
  GetImplementation(*this).SurfaceRotated(width, height, windowOrientation, screenOrientation);
}

int32_t Scene::GetCurrentSurfaceOrientation() const
{
  return GetImplementation(*this).GetCurrentSurfaceOrientation();
}

int32_t Scene::GetCurrentScreenOrientation() const
{
  return GetImplementation(*this).GetCurrentScreenOrientation();
}

const Rect<int32_t>& Scene::GetCurrentSurfaceRect() const
{
  return GetImplementation(*this).GetCurrentSurfaceRect();
}

uint32_t Scene::GetSurfaceRectChangedCount() const
{
  return GetImplementation(*this).GetSurfaceRectChangedCount();
}

void Scene::SetRotationCompletedAcknowledgement()
{
  GetImplementation(*this).SetRotationCompletedAcknowledgement();
}

bool Scene::IsRotationCompletedAcknowledgementSet() const
{
  return GetImplementation(*this).IsRotationCompletedAcknowledgementSet();
}

void Scene::KeepRendering(float durationSeconds)
{
  GetImplementation(*this).KeepRendering(durationSeconds);
}

void Scene::SetPartialUpdateEnabled(bool enabled)
{
  GetImplementation(*this).SetPartialUpdateEnabled(enabled);
}

bool Scene::IsPartialUpdateEnabled() const
{
  return GetImplementation(*this).IsPartialUpdateEnabled();
}

void Scene::SetGeometryHittestEnabled(bool enabled)
{
  GetImplementation(*this).SetGeometryHittestEnabled(enabled);
}

bool Scene::IsGeometryHittestEnabled()
{
  return GetImplementation(*this).IsGeometryHittestEnabled();
}

Scene::TouchPropagationType Scene::GetTouchPropagationType()
{
  return GetImplementation(*this).GetTouchPropagationType();
}

void Scene::SetNativeId(int32_t nativeId)
{
  return GetImplementation(*this).SetNativeId(nativeId);
}

int32_t Scene::GetNativeId() const
{
  return GetImplementation(*this).GetNativeId();
}

Dali::GestureState Scene::GetLastPanGestureState()
{
  return GetImplementation(*this).GetLastPanGestureState();
}

Scene::EventProcessingFinishedSignalType& Scene::EventProcessingFinishedSignal()
{
  return GetImplementation(*this).EventProcessingFinishedSignal();
}

Scene::KeyEventSignalType& Scene::KeyEventSignal()
{
  return GetImplementation(*this).KeyEventSignal();
}

Scene::KeyEventGeneratedSignalType& Scene::KeyEventGeneratedSignal()
{
  return GetImplementation(*this).KeyEventGeneratedSignal();
}

Scene::KeyEventGeneratedSignalType& Scene::InterceptKeyEventSignal()
{
  return GetImplementation(*this).InterceptKeyEventSignal();
}

Scene::KeyEventSignalType& Scene::KeyEventMonitorSignal()
{
  return GetImplementation(*this).KeyEventMonitorSignal();
}

Scene::TouchEventSignalType& Scene::TouchedSignal()
{
  return GetImplementation(*this).TouchedSignal();
}

Scene::WheelEventSignalType& Scene::WheelEventSignal()
{
  return GetImplementation(*this).WheelEventSignal();
}

Scene::WheelEventGeneratedSignalType& Scene::WheelEventGeneratedSignal()
{
  return GetImplementation(*this).WheelEventGeneratedSignal();
}

} // namespace Integration

} // namespace Dali
