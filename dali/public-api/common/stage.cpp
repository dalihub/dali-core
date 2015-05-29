/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/stage.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/common/core-impl.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/object-registry.h>

#include <dali/devel-api/dynamics/dynamics-world.h>
#include <dali/devel-api/dynamics/dynamics-world-config.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#endif

namespace Dali
{

using Internal::Core;

const Vector4 Stage::DEFAULT_BACKGROUND_COLOR(0.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Stage::DEBUG_BACKGROUND_COLOR(0.2f, 0.5f, 0.2f, 1.0f);

Stage::Stage()
{
}

Stage::~Stage()
{
}

Stage::Stage(const Stage& handle)
: BaseHandle(handle)
{
}

Stage& Stage::operator=(const Stage& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

Stage::Stage(Internal::Stage* internal)
: BaseHandle(internal)
{
}

Stage Stage::GetCurrent()
{
  Internal::Stage* stage = Internal::Stage::GetCurrent();
  DALI_ASSERT_ALWAYS( stage && "Stage doesn't exist" );
  return Stage( stage );
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
  return RenderTaskList( &GetImplementation(*this).GetRenderTaskList() );
}

Vector2 Stage::GetSize() const
{
  return GetImplementation(*this).GetSize();
}

unsigned int Stage::GetLayerCount() const
{
  return GetImplementation(*this).GetLayerCount();
}

Layer Stage::GetLayer(unsigned int depth) const
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
  Internal::ObjectRegistry& internal = Internal::Stage::GetCurrent()->GetObjectRegistry();

  return ObjectRegistry(&internal);
}

DynamicsWorld Stage::InitializeDynamics(DynamicsWorldConfig config)
{
#ifdef DYNAMICS_SUPPORT
  Internal::DynamicsWorldConfigPtr configImpl( &(GetImplementation(config)) );

  return DynamicsWorld( GetImplementation(*this).InitializeDynamics(configImpl).Get() );
#else
  return DynamicsWorld();
#endif
}

DynamicsWorld Stage::GetDynamicsWorld()
{
#ifdef DYNAMICS_SUPPORT
  return DynamicsWorld( GetImplementation(*this).GetDynamicsWorld().Get() );
#else
  return DynamicsWorld();
#endif
}

void Stage::TerminateDynamics()
{
#ifdef DYNAMICS_SUPPORT
  GetImplementation(*this).TerminateDynamics();
#endif
}

void Stage::KeepRendering( float durationSeconds )
{
  GetImplementation(*this).KeepRendering( durationSeconds );
}

Stage::KeyEventSignalType& Stage::KeyEventSignal()
{
  return GetImplementation(*this).KeyEventSignal();
}

Stage::EventProcessingFinishedSignalType& Stage::EventProcessingFinishedSignal()
{
  return GetImplementation(*this).EventProcessingFinishedSignal();
}

Stage::TouchedSignalType& Stage::TouchedSignal()
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
