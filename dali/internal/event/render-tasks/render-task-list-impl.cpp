/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/event-thread-services.h>
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

RenderTaskList* RenderTaskList::New( EventThreadServices& eventServices, RenderTaskDefaults& defaults, bool systemLevel )
{
  RenderTaskList* taskList = new RenderTaskList( eventServices, defaults, systemLevel );

  taskList->Initialize( eventServices.GetUpdateManager() );

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

    OwnerPointer< SceneGraph::RenderTask > transferOwnership( sceneObject );
    AddTaskMessage( mEventThreadServices, *mSceneObject, transferOwnership );
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
      RenderTask& taskImpl = GetImplementation( task );
      if ( mSceneObject )
      {
        SceneGraph::RenderTask* sceneObject = taskImpl.GetRenderTaskSceneObject();
        DALI_ASSERT_DEBUG( NULL != sceneObject );

        // Send a message to remove the scene-graph RenderTask
        RemoveTaskMessage( mEventThreadServices, *mSceneObject, *sceneObject );

        // The scene-graph RenderTask will be destroyed soon; discard the raw-pointer
        taskImpl.DiscardSceneObject();
      }

      mTasks.erase( iter );

      for ( Vector< Exclusive >::Iterator exclusiveIt = mExclusives.Begin(); exclusiveIt != mExclusives.End(); ++exclusiveIt )
      {
        if ( exclusiveIt->renderTaskPtr == &taskImpl )
        {
          mExclusives.Erase( exclusiveIt );
          break;
        }
      }
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

void RenderTaskList::SetExclusive( RenderTask* task, bool exclusive )
{
  // Check to see if this rendertask has an entry?
  for ( Vector< Exclusive >::Iterator exclusiveIt = mExclusives.Begin(); exclusiveIt != mExclusives.End(); ++exclusiveIt )
  {
    if ( exclusiveIt->renderTaskPtr == task )
    {
      if ( !exclusive )
      {
        mExclusives.Erase( exclusiveIt );
        break;
      }
      else
      {
        exclusiveIt->actorPtr = task->GetSourceActor();
        exclusive = false;
        break;
      }
    }
  }
  if ( exclusive )
  {
    Exclusive exclusiveSlot;
    exclusiveSlot.renderTaskPtr = task;
    exclusiveSlot.actorPtr = task->GetSourceActor();
    mExclusives.PushBack( exclusiveSlot );
  }
}

RenderTaskList::RenderTaskList( EventThreadServices& eventThreadServices, RenderTaskDefaults& defaults, bool systemLevel )
: mEventThreadServices( eventThreadServices ),
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
  // set the callback to call us back when tasks are completed
  mSceneObject->SetCompleteNotificationInterface( this );
}

void RenderTaskList::NotifyCompleted()
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

void RenderTaskList::RecoverFromContextLoss()
{
  for ( RenderTaskContainer::iterator iter = mTasks.begin(); mTasks.end() != iter; ++iter )
  {
    Dali::RenderTask task = *iter;

    // If the render target renders only once to an offscreen, re-render the render task
    if( task.GetRefreshRate() == Dali::RenderTask::REFRESH_ONCE && task.GetTargetFrameBuffer() )
    {
      task.SetRefreshRate( Dali::RenderTask::REFRESH_ONCE );
    }
  }
}

} // namespace Internal

} // namespace Dali
