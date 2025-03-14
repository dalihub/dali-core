/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/rendering/frame-buffer.h>

namespace Dali
{
static bool DefaultScreenToFrameBufferFunction(Vector2& coordinates)
{
  return false;
}

static bool FullScreenFrameBufferFunction(Vector2& coordinates)
{
  // Don't need to modify frameBufferCoords
  return true;
}

RenderTask::ConstScreenToFrameBufferFunction RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION = DefaultScreenToFrameBufferFunction;
RenderTask::ConstScreenToFrameBufferFunction RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION        = FullScreenFrameBufferFunction;

const bool     RenderTask::DEFAULT_EXCLUSIVE     = false;
const bool     RenderTask::DEFAULT_INPUT_ENABLED = true;
const Vector4  RenderTask::DEFAULT_CLEAR_COLOR   = Vector4(0.0f, 0.0f, 0.0f, 1.0f); // cannot use Color::Black because it may or may not be initialized yet
const bool     RenderTask::DEFAULT_CLEAR_ENABLED = false;
const bool     RenderTask::DEFAULT_CULL_MODE     = true;
const uint32_t RenderTask::DEFAULT_REFRESH_RATE  = REFRESH_ALWAYS;

static constexpr int32_t MIN_ORDER_INDEX = -1000;
static constexpr int32_t MAX_ORDER_INDEX = 1000;

RenderTask::RenderTask() = default;

RenderTask RenderTask::DownCast(BaseHandle handle)
{
  return RenderTask(dynamic_cast<Dali::Internal::RenderTask*>(handle.GetObjectPtr()));
}

RenderTask::~RenderTask() = default;

RenderTask::RenderTask(const RenderTask& handle) = default;

RenderTask& RenderTask::operator=(const RenderTask& rhs) = default;

RenderTask::RenderTask(RenderTask&& rhs) noexcept = default;

RenderTask& RenderTask::operator=(RenderTask&& rhs) noexcept = default;

void RenderTask::SetSourceActor(Actor actor)
{
  // NULL handle is allowed
  Internal::Actor* actorImpl(nullptr);
  if(actor)
  {
    actorImpl = &GetImplementation(actor);
  }

  GetImplementation(*this).SetSourceActor(actorImpl);
}

Actor RenderTask::GetSourceActor() const
{
  return Dali::Actor(GetImplementation(*this).GetSourceActor());
}

Actor RenderTask::GetStopperActor() const
{
  return Dali::Actor(GetImplementation(*this).GetStopperActor());
}

void RenderTask::SetExclusive(bool exclusive)
{
  GetImplementation(*this).SetExclusive(exclusive);
}

bool RenderTask::IsExclusive() const
{
  return GetImplementation(*this).IsExclusive();
}

void RenderTask::SetCameraActor(CameraActor cameraActor)
{
  // NULL handle is allowed
  Internal::CameraActor* actorImpl(nullptr);
  if(cameraActor)
  {
    actorImpl = &GetImplementation(cameraActor);
  }

  GetImplementation(*this).SetCameraActor(actorImpl);
}

CameraActor RenderTask::GetCameraActor() const
{
  return Dali::CameraActor(GetImplementation(*this).GetCameraActor());
}

void RenderTask::SetFrameBuffer(FrameBuffer frameBuffer)
{
  Internal::FrameBuffer* frameBufferPtr(nullptr);
  if(frameBuffer)
  {
    frameBufferPtr = &GetImplementation(frameBuffer);
  }

  GetImplementation(*this).SetFrameBuffer(frameBufferPtr);
}

FrameBuffer RenderTask::GetFrameBuffer() const
{
  Internal::FrameBuffer* frameBufferPtr(GetImplementation(*this).GetFrameBuffer());
  return FrameBuffer(frameBufferPtr);
}

void RenderTask::SetScreenToFrameBufferFunction(ScreenToFrameBufferFunction conversionFunction)
{
  GetImplementation(*this).SetScreenToFrameBufferFunction(conversionFunction);
}

RenderTask::ScreenToFrameBufferFunction RenderTask::GetScreenToFrameBufferFunction() const
{
  return GetImplementation(*this).GetScreenToFrameBufferFunction();
}

void RenderTask::SetScreenToFrameBufferMappingActor(Dali::Actor mappingActor)
{
  GetImplementation(*this).SetScreenToFrameBufferMappingActor(mappingActor);
}

Dali::Actor RenderTask::GetScreenToFrameBufferMappingActor() const
{
  return GetImplementation(*this).GetScreenToFrameBufferMappingActor();
}

void RenderTask::SetViewportGuideActor(Actor actor)
{
  // NULL handle is allowed
  Internal::Actor* actorImpl(nullptr);
  if(actor)
  {
    actorImpl = &GetImplementation(actor);
  }

  GetImplementation(*this).SetViewportGuideActor(actorImpl);
}

Actor RenderTask::GetViewportGuideActor() const
{
  return Dali::Actor(GetImplementation(*this).GetViewportGuideActor());
}

void RenderTask::ResetViewportGuideActor()
{
  GetImplementation(*this).ResetViewportGuideActor();
}

void RenderTask::SetViewportPosition(Vector2 position)
{
  GetImplementation(*this).SetViewportPosition(position);
}

Vector2 RenderTask::GetCurrentViewportPosition() const
{
  return GetImplementation(*this).GetCurrentViewportPosition();
}

void RenderTask::SetViewportSize(Vector2 size)
{
  GetImplementation(*this).SetViewportSize(size);
}

Vector2 RenderTask::GetCurrentViewportSize() const
{
  return GetImplementation(*this).GetCurrentViewportSize();
}

void RenderTask::SetViewport(Viewport viewport)
{
  GetImplementation(*this).SetViewport(viewport);
}

Viewport RenderTask::GetViewport() const
{
  Viewport result;
  GetImplementation(*this).GetViewport(result);
  return result;
}

void RenderTask::SetClearColor(const Vector4& color)
{
  GetImplementation(*this).SetClearColor(color);
}

Vector4 RenderTask::GetClearColor() const
{
  return GetImplementation(*this).GetClearColor();
}

void RenderTask::SetClearEnabled(bool enabled)
{
  GetImplementation(*this).SetClearEnabled(enabled);
}

bool RenderTask::GetClearEnabled() const
{
  return GetImplementation(*this).GetClearEnabled();
}

void RenderTask::SetCullMode(bool mode)
{
  GetImplementation(*this).SetCullMode(mode);
}

bool RenderTask::GetCullMode() const
{
  return GetImplementation(*this).GetCullMode();
}

void RenderTask::SetRefreshRate(uint32_t refreshRate)
{
  GetImplementation(*this).SetRefreshRate(refreshRate);
}

uint32_t RenderTask::GetRefreshRate() const
{
  return GetImplementation(*this).GetRefreshRate();
}

RenderTask::RenderTaskSignalType& RenderTask::FinishedSignal()
{
  return GetImplementation(*this).FinishedSignal();
}

void RenderTask::SetInputEnabled(bool enabled)
{
  GetImplementation(*this).SetInputEnabled(enabled);
}

bool RenderTask::GetInputEnabled() const
{
  return GetImplementation(*this).GetInputEnabled();
}

bool RenderTask::WorldToViewport(const Vector3& position, float& viewportX, float& viewportY) const
{
  return GetImplementation(*this).WorldToViewport(position, viewportX, viewportY);
}

bool RenderTask::ViewportToLocal(Actor actor, float viewportX, float viewportY, float& localX, float& localY) const
{
  if(actor)
  {
    Internal::Actor* actorImpl(&GetImplementation(actor));
    return GetImplementation(*this).ViewportToLocal(actorImpl, viewportX, viewportY, localX, localY);
  }
  else
  {
    return false;
  }
}

void RenderTask::SetRenderPassTag(uint32_t renderPassTag)
{
  GetImplementation(*this).SetRenderPassTag(renderPassTag);
}

uint32_t RenderTask::GetRenderPassTag() const
{
  return GetImplementation(*this).GetRenderPassTag();
}

void RenderTask::SetOrderIndex(int32_t orderIndex)
{
  if(orderIndex < MIN_ORDER_INDEX || orderIndex > MAX_ORDER_INDEX)
  {
    DALI_LOG_ERROR("OrderIndex value can be available between [-1000, 1000].\n");
    orderIndex = std::min(orderIndex, MAX_ORDER_INDEX);
    orderIndex = std::max(orderIndex, MIN_ORDER_INDEX);
  }
  GetImplementation(*this).SetOrderIndex(orderIndex);
}

int32_t RenderTask::GetOrderIndex() const
{
  return GetImplementation(*this).GetOrderIndex();
}

uint32_t RenderTask::GetRenderTaskId() const
{
  return GetImplementation(*this).GetRenderTaskId();
}

void RenderTask::RenderUntil(Actor stopperActor)
{
  // NULL handle is allowed
  Internal::Actor* actorImpl(nullptr);
  if(stopperActor)
  {
    actorImpl = &GetImplementation(stopperActor);
  }
  return GetImplementation(*this).RenderUntil(actorImpl);
}

void RenderTask::KeepRenderResult()
{
  GetImplementation(*this).KeepRenderResult();
}

void RenderTask::ClearRenderResult()
{
  GetImplementation(*this).ClearRenderResult();
}

PixelData RenderTask::GetRenderResult()
{
  return GetImplementation(*this).GetRenderResult();
}

RenderTask::RenderTask(Internal::RenderTask* internal)
: Handle(internal)
{
}

} // namespace Dali
