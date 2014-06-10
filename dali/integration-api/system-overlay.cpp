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
#include <dali/integration-api/system-overlay.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/system-overlay-impl.h>

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

RenderTaskList SystemOverlay::GetOverlayRenderTasks()
{
  return RenderTaskList( &mImpl->GetOverlayRenderTasks() );
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
