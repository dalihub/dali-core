/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/manager/update-manager.h>

using Dali::Internal::SceneGraph::UpdateManager;

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogRenderList = Debug::Filter::New(Debug::Concise, false, "LOG_RENDER_TASK_LIST");
}
#endif

namespace Dali
{
namespace Internal
{

RenderTaskList* RenderTaskList::New( UpdateManager& updateManager, RenderTaskDefaults& defaults, bool systemLevel )
{
  RenderTaskList* taskList = new RenderTaskList( updateManager.GetEventToUpdate(), defaults, systemLevel );

  taskList->Initialize( updateManager );

  return taskList;
}

Dali::RenderTask RenderTaskList::CreateTask()
{
  RenderTask* taskImpl = RenderTask::New( mIsSystemLevel );

  Dali::RenderTask newTask( taskImpl );
  mTasks.push_back( newTask );

  if ( mSceneObject )
  {
    SceneGraph::RenderTask* sceneObject = taskImpl->CreateSceneObject();
    DALI_ASSERT_DEBUG( NULL != sceneObject );

    // Pass ownership to SceneGraph::RenderTaskList
    AddTaskMessage( mEventToUpdate, *mSceneObject, *sceneObject );
  }

  // Set the default source & camera actors
  taskImpl->SetSourceActor( &mDefaults.GetDefaultRootActor() );
  taskImpl->SetCameraActor( &mDefaults.GetDefaultCameraActor() );

  return newTask;
}

void RenderTaskList::RemoveTask( Dali::RenderTask task )
{
  for ( RenderTaskContainer::iterator iter = mTasks.begin(); mTasks.end() != iter; ++iter )
  {
    if ( *iter == task )
    {
      if ( mSceneObject )
      {
        RenderTask& taskImpl = GetImplementation( task );

        SceneGraph::RenderTask* sceneObject = taskImpl.GetRenderTaskSceneObject();
        DALI_ASSERT_DEBUG( NULL != sceneObject );

        // Send a message to remove the scene-graph RenderTask
        RemoveTaskMessage( mEventToUpdate, *mSceneObject, *sceneObject );

        // The scene-graph RenderTask will be destroyed soon; discard the raw-pointer
        taskImpl.DiscardSceneObject();
      }

      mTasks.erase( iter );
      break; // we're finished
    }
  }
}

unsigned int RenderTaskList::GetTaskCount() const
{
  return mTasks.size();
}

Dali::RenderTask RenderTaskList::GetTask( unsigned int index ) const
{
  DALI_ASSERT_ALWAYS( ( index < mTasks.size() ) && "RenderTask index out-of-range" );

  return mTasks[index];
}

void RenderTaskList::NotifyFinished()
{
  DALI_LOG_TRACE_METHOD(gLogRenderList);

  std::vector< Dali::RenderTask > finishedRenderTasks;

  // Since render tasks can be unreferenced during the signal emissions, iterators into render tasks pointers may be invalidated.
  // First copy the finished render tasks, then emit signals
  for ( std::vector<Dali::RenderTask>::iterator it = mTasks.begin(), endIt = mTasks.end(); it != endIt; ++it )
  {
    Dali::RenderTask& renderTask( *it );

    if( GetImplementation( renderTask ).HasFinished() )
    {
      finishedRenderTasks.push_back( Dali::RenderTask( renderTask ) );
    }
  }

  // Now it's safe to emit the signals
  for ( std::vector<Dali::RenderTask>::iterator it = finishedRenderTasks.begin(), endIt = finishedRenderTasks.end(); it != endIt; ++it )
  {
    Dali::RenderTask& handle( *it );

    GetImplementation(handle).EmitSignalFinish();
  }
}

RenderTaskList::RenderTaskList( EventToUpdate& eventToUpdate, RenderTaskDefaults& defaults, bool systemLevel )
: mEventToUpdate( eventToUpdate ),
  mDefaults( defaults ),
  mIsSystemLevel( systemLevel ),
  mSceneObject( NULL )
{
}

RenderTaskList::~RenderTaskList()
{
}

void RenderTaskList::Initialize( UpdateManager& updateManager )
{
  // This should only be called once, with no existing scene-object
  DALI_ASSERT_DEBUG( NULL == mSceneObject );

  // Get raw-pointer to render task list
  mSceneObject = updateManager.GetRenderTaskList( mIsSystemLevel );
}

} // namespace Internal

} // namespace Dali
