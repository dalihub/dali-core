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
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>

// INTERNAL INCLUDES
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/resources/complete-status-manager.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderTaskList::RenderTaskList(CompleteStatusManager& completeStatusManager)
: mCompleteStatusManager( completeStatusManager )
{
}

RenderTaskList::~RenderTaskList()
{
}

void RenderTaskList::AddTask( RenderTask* newTask )
{
  DALI_ASSERT_DEBUG( newTask != NULL && "SceneGraph RenderTask is null");

  // mRenderTasks container takes ownership
  mRenderTasks.PushBack( newTask );
  newTask->SetCompleteStatusManager( &mCompleteStatusManager );
}

void RenderTaskList::RemoveTask( RenderTask* task )
{
  RenderTaskContainer::ConstIterator end = mRenderTasks.End();
  for ( RenderTaskContainer::Iterator iter = mRenderTasks.Begin(); iter != end; ++iter )
  {
    if ( *iter == task )
    {
      // Destroy the task
      mRenderTasks.Erase( iter );

      break; // we're finished
    }
  }
}

RenderTaskList::RenderTaskContainer& RenderTaskList::GetTasks()
{
  return mRenderTasks;
}

const RenderTaskList::RenderTaskContainer& RenderTaskList::GetTasks() const
{
  return mRenderTasks;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
