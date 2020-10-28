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
#include <dali/devel-api/common/stage.h>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>

namespace Dali
{
using Internal::Core;

Stage::Stage() = default;

Stage::~Stage() = default;

Stage::Stage(const Stage& handle) = default;

Stage& Stage::operator=(const Stage& rhs) = default;

Stage::Stage(Internal::Stage* internal)
: BaseHandle(internal)
{
}

Stage Stage::GetCurrent()
{
  Internal::Stage* stage = Internal::Stage::GetCurrent();
  DALI_ASSERT_ALWAYS(stage && "Stage doesn't exist");
  return Stage(stage);
}

bool Stage::IsInstalled()
{
  return Internal::Stage::IsInstalled();
}

void Stage::Add(Actor& actor)
{
  GetImplementation(*this).Add(GetImplementation(actor));
}

void Stage::Remove(Actor& actor)
{
  GetImplementation(*this).Remove(GetImplementation(actor));
}

RenderTaskList Stage::GetRenderTaskList() const
{
  return RenderTaskList(&GetImplementation(*this).GetRenderTaskList());
}

Vector2 Stage::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

uint32_t Stage::GetLayerCount() const
{
  return GetImplementation(*this).GetLayerCount();
}

Layer Stage::GetLayer(uint32_t depth) const
{
  return GetImplementation(*this).GetLayer(depth);
}

Layer Stage::GetRootLayer() const
{
  return GetImplementation(*this).GetRootLayer();
}

void Stage::SetBackgroundColor(Vector4 color)
{
  GetImplementation(*this).SetBackgroundColor(color);
}

Vector4 Stage::GetBackgroundColor() const
{
  return GetImplementation(*this).GetBackgroundColor();
}

Vector2 Stage::GetDpi() const
{
  return GetImplementation(*this).GetDpi();
}

ObjectRegistry Stage::GetObjectRegistry() const
{
  Internal::Stage* stage = Internal::Stage::GetCurrent();
  DALI_ASSERT_ALWAYS(stage && "GetObjectRegistry() : Stage is null");

  Internal::ObjectRegistry& internal = stage->GetObjectRegistry();
  return ObjectRegistry(&internal);
}

void Stage::KeepRendering(float durationSeconds)
{
  GetImplementation(*this).KeepRendering(durationSeconds);
}

Stage::KeyEventSignalType& Stage::KeyEventSignal()
{
  return GetImplementation(*this).KeyEventSignal();
}

Stage::EventProcessingFinishedSignalType& Stage::EventProcessingFinishedSignal()
{
  return GetImplementation(*this).EventProcessingFinishedSignal();
}

Stage::TouchEventSignalType& Stage::TouchedSignal()
{
  return GetImplementation(*this).TouchedSignal();
}

Stage::WheelEventSignalType& Stage::WheelEventSignal()
{
  return GetImplementation(*this).WheelEventSignal();
}

Stage::ContextStatusSignal& Stage::ContextLostSignal()
{
  return GetImplementation(*this).ContextLostSignal();
}

Stage::ContextStatusSignal& Stage::ContextRegainedSignal()
{
  return GetImplementation(*this).ContextRegainedSignal();
}

Stage::SceneCreatedSignalType& Stage::SceneCreatedSignal()
{
  return GetImplementation(*this).SceneCreatedSignal();
}

} // namespace Dali
