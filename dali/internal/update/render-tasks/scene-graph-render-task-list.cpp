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
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-object-allocator.h>

namespace //Unnamed namespace
{

//Memory pool used to allocate new RenderTaskLists. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderTaskList> gRenderTaskListMemoryPool;

} // unnamed namespace

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderTaskList* RenderTaskList::New()
{
  return new ( gRenderTaskListMemoryPool.AllocateRawThreadSafe() ) RenderTaskList();
}

RenderTaskList::RenderTaskList()
: mNotificationObject( nullptr ),
  mRenderMessageDispatcher( nullptr )
{
}

RenderTaskList::~RenderTaskList() = default;

void RenderTaskList::operator delete( void* ptr )
{
  gRenderTaskListMemoryPool.FreeThreadSafe( static_cast<RenderTaskList*>( ptr ) );
}

void RenderTaskList::SetRenderMessageDispatcher( RenderMessageDispatcher* renderMessageDispatcher )
{
  mRenderMessageDispatcher = renderMessageDispatcher;
}

void RenderTaskList::AddTask( OwnerPointer< RenderTask >& newTask )
{
  DALI_ASSERT_DEBUG( newTask != NULL && "SceneGraph RenderTask is null");
  DALI_ASSERT_DEBUG( mRenderMessageDispatcher != NULL && "RenderMessageDispatcher is null");

  newTask->Initialize( *mRenderMessageDispatcher );
  // mRenderTasks container takes ownership
  mRenderTasks.PushBack( newTask.Release() );
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

uint32_t RenderTaskList::GetTaskCount()
{
  return static_cast<uint32_t>( mRenderTasks.Count() );
}

RenderTaskList::RenderTaskContainer& RenderTaskList::GetTasks()
{
  return mRenderTasks;
}

const RenderTaskList::RenderTaskContainer& RenderTaskList::GetTasks() const
{
  return mRenderTasks;
}

void RenderTaskList::SetCompleteNotificationInterface( CompleteNotificationInterface* object )
{

  mNotificationObject = object;
}

CompleteNotificationInterface* RenderTaskList::GetCompleteNotificationInterface()
{

  return mNotificationObject;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
