//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/common/stage.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/common/core-impl.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/object/object-registry.h>

using namespace std;

namespace Dali
{

using Internal::Core;

const Vector4 Stage::DEFAULT_BACKGROUND_COLOR(0.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Stage::DEBUG_BACKGROUND_COLOR(0.2f, 0.5f, 0.2f, 1.0f);

const char* const Stage::SIGNAL_KEY_EVENT = "key-event";
const char* const Stage::SIGNAL_EVENT_PROCESSING_FINISHED = "event-processing-finished";
const char* const Stage::SIGNAL_TOUCHED = "touched";

Stage::Stage()
{
}

Stage::~Stage()
{
}

Stage::Stage(Internal::Stage* internal)
: BaseHandle(internal)
{
}

Stage Stage::GetCurrent()
{
  return Stage(Internal::Stage::GetCurrent());
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
  Internal::DynamicsWorldConfigPtr configImpl( &(GetImplementation(config)) );

  return DynamicsWorld( GetImplementation(*this).InitializeDynamics(configImpl).Get() );
}

DynamicsWorld Stage::GetDynamicsWorld()
{
  return DynamicsWorld( GetImplementation(*this).GetDynamicsWorld().Get() );
}

void Stage::TerminateDynamics()
{
  GetImplementation(*this).TerminateDynamics();
}

void Stage::KeepRendering( float durationSeconds )
{
  GetImplementation(*this).KeepRendering( durationSeconds );
}

Stage::KeyEventSignalV2& Stage::KeyEventSignal()
{
  return GetImplementation(*this).KeyEventSignal();
}

Stage::EventProcessingFinishedSignalV2& Stage::EventProcessingFinishedSignal()
{
  return GetImplementation(*this).EventProcessingFinishedSignal();
}

Stage::TouchedSignalV2& Stage::TouchedSignal()
{
  return GetImplementation(*this).TouchedSignal();
}

} // namespace Dali
