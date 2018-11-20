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
#include <dali/integration-api/render-task-list-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>

namespace Dali
{

namespace Integration
{

namespace RenderTaskList
{

Dali::RenderTaskList New()
{
  Dali::Internal::RenderTaskListPtr renderTaskList = Dali::Internal::RenderTaskList::New();
  return Dali::RenderTaskList( renderTaskList.Get() );
}

Dali::RenderTask CreateTask( Dali::RenderTaskList& taskList, Dali::Actor& sourceActor, Dali::CameraActor& cameraActor)
{
  return GetImplementation(taskList).CreateTask( &GetImplementation( sourceActor), &GetImplementation(cameraActor) );
}

} // namespace RenderTaskList

} // namespace Integration

} // namespace Dali
