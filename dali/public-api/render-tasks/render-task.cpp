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
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

static bool DefaultScreenToFrameBufferFunction( Vector2& coordinates )
{
  return false;
}

static bool FullScreenFrameBufferFunction( Vector2& coordinates )
{
  // Don't need to modify frameBufferCoords
  return true;
}

const char* const RenderTask::SIGNAL_FINISHED = "finished";

RenderTask::ConstScreenToFrameBufferFunction RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION = DefaultScreenToFrameBufferFunction;
RenderTask::ConstScreenToFrameBufferFunction RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION        = FullScreenFrameBufferFunction;

const bool         RenderTask::DEFAULT_EXCLUSIVE     = false;
const bool         RenderTask::DEFAULT_INPUT_ENABLED = true;
const Vector4      RenderTask::DEFAULT_CLEAR_COLOR   = Vector4( 0.0f, 0.0f, 0.0f, 1.0f ); // cannot use Color::Black because it may or may not be initialized yet
const bool         RenderTask::DEFAULT_CLEAR_ENABLED = false;
const unsigned int RenderTask::DEFAULT_REFRESH_RATE  = REFRESH_ALWAYS;

RenderTask::RenderTask()
{
}

RenderTask RenderTask::DownCast( BaseHandle handle )
{
  return RenderTask( dynamic_cast<Dali::Internal::RenderTask*>( handle.GetObjectPtr() ) );
}

RenderTask::~RenderTask()
{
}

void RenderTask::SetSourceActor( Actor actor )
{
  // NULL handle is allowed
  Internal::Actor* actorImpl( NULL );
  if ( actor )
  {
    actorImpl = &GetImplementation( actor );
  }

  GetImplementation(*this).SetSourceActor( actorImpl );
}

Actor RenderTask::GetSourceActor() const
{
  return Dali::Actor(GetImplementation(*this).GetSourceActor());
}

void RenderTask::SetExclusive( bool exclusive )
{
  GetImplementation(*this).SetExclusive( exclusive );
}

bool RenderTask::IsExclusive() const
{
  return GetImplementation(*this).IsExclusive();
}

void RenderTask::SetCameraActor( CameraActor cameraActor )
{
  // NULL handle is allowed
  Internal::CameraActor* actorImpl( NULL );
  if ( cameraActor )
  {
    actorImpl = &GetImplementation( cameraActor );
  }

  GetImplementation(*this).SetCameraActor( actorImpl );
}

CameraActor RenderTask::GetCameraActor() const
{
  return Dali::CameraActor(GetImplementation(*this).GetCameraActor());
}

void RenderTask::SetTargetFrameBuffer( FrameBufferImage frameBuffer )
{
  GetImplementation(*this).SetTargetFrameBuffer( frameBuffer );
}

FrameBufferImage RenderTask::GetTargetFrameBuffer() const
{
  return GetImplementation(*this).GetTargetFrameBuffer();
}

void RenderTask::SetScreenToFrameBufferFunction( ScreenToFrameBufferFunction conversionFunction )
{
  GetImplementation(*this).SetScreenToFrameBufferFunction( conversionFunction );
}

RenderTask::ScreenToFrameBufferFunction RenderTask::GetScreenToFrameBufferFunction() const
{
  return GetImplementation(*this).GetScreenToFrameBufferFunction();
}

void RenderTask::SetScreenToFrameBufferMappingActor( Dali::Actor mappingActor )
{
  // NULL handle is allowed
  Internal::Actor* actorImpl( NULL );
  if ( mappingActor )
  {
    actorImpl = &GetImplementation( mappingActor );
  }

  GetImplementation(*this).SetScreenToFrameBufferMappingActor( actorImpl );
}

Dali::Actor RenderTask::GetScreenToFrameBufferMappingActor() const
{
  return Dali::Actor(GetImplementation(*this).GetScreenToFrameBufferMappingActor());
}

void RenderTask::SetViewportPosition( Vector2 position )
{
  GetImplementation(*this).SetViewportPosition( position );
}

Vector2 RenderTask::GetCurrentViewportPosition() const
{
  return GetImplementation(*this).GetCurrentViewportPosition();
}

void RenderTask::SetViewportSize( Vector2 size )
{
  GetImplementation(*this).SetViewportSize( size );
}

Vector2 RenderTask::GetCurrentViewportSize() const
{
  return GetImplementation(*this).GetCurrentViewportSize();
}

void RenderTask::SetViewport( Viewport viewport )
{
  GetImplementation(*this).SetViewport( viewport );
}

Viewport RenderTask::GetViewport() const
{
  Viewport result;
  GetImplementation(*this).GetViewport( result );
  return result;
}

void RenderTask::SetClearColor( const Vector4& color )
{
  GetImplementation(*this).SetClearColor( color );
}

Vector4 RenderTask::GetClearColor() const
{
  return GetImplementation(*this).GetClearColor();
}

void RenderTask::SetClearEnabled( bool enabled )
{
  GetImplementation(*this).SetClearEnabled( enabled );
}

bool RenderTask::GetClearEnabled() const
{
  return GetImplementation(*this).GetClearEnabled();
}

void RenderTask::SetRefreshRate( unsigned int refreshRate )
{
  GetImplementation(*this).SetRefreshRate( refreshRate );
}

unsigned int RenderTask::GetRefreshRate() const
{
  return GetImplementation(*this).GetRefreshRate();
}

RenderTask::RenderTaskSignalV2& RenderTask::FinishedSignal()
{
  return GetImplementation(*this).FinishedSignal();
}

void RenderTask::SetInputEnabled( bool enabled )
{
  GetImplementation(*this).SetInputEnabled( enabled );
}

bool RenderTask::GetInputEnabled() const
{
  return GetImplementation(*this).GetInputEnabled();
}

RenderTask::RenderTask( Internal::RenderTask* internal )
: Constrainable(internal)
{
}

} // namespace Dali

