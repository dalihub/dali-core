/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/system-overlay.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/system-overlay-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

namespace Dali
{

namespace Integration
{

SystemOverlay::~SystemOverlay()
{
  delete mImpl;
}

void SystemOverlay::Add( Actor actor )
{
  mImpl->Add( GetImplementation(actor) );
}

void SystemOverlay::Remove( Actor actor )
{
  mImpl->Remove( GetImplementation(actor) );
}

void SystemOverlay::SetOverlayRenderTasks(RenderTaskList& taskList)
{
  mImpl->SetOverlayRenderTasks( GetImplementation( taskList ) );
}

RenderTaskList SystemOverlay::GetOverlayRenderTasks()
{
  return RenderTaskList( &mImpl->GetOverlayRenderTasks() );
}

Actor SystemOverlay::GetDefaultRootActor()
{
  return Actor( &mImpl->GetDefaultRootActor() );
}

CameraActor SystemOverlay::GetDefaultCameraActor()
{
  return CameraActor( &mImpl->GetDefaultCameraActor() );
}

SystemOverlay::SystemOverlay( Internal::SystemOverlay* impl )
: mImpl( impl )
{
}

Internal::SystemOverlay* SystemOverlay::GetImpl()
{
  return mImpl;
}

} // namespace Integration

} // namespace Dali
