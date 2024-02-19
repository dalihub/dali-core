/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/common/resetter-manager.h> ///< For AddInitializeResetter

namespace //Unnamed namespace
{
//Memory pool used to allocate new RenderTaskLists. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderTaskList>& GetRenderTaskListMemoryPool()
{
  static Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderTaskList> gRenderTaskListMemoryPool;
  return gRenderTaskListMemoryPool;
}
} // unnamed namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
RenderTaskList* RenderTaskList::New()
{
  return new(GetRenderTaskListMemoryPool().AllocateRawThreadSafe()) RenderTaskList();
}

RenderTaskList::RenderTaskList()
: mNotificationObject(nullptr),
  mResetterManager(nullptr),
  mRenderMessageDispatcher(nullptr)
{
}

RenderTaskList::~RenderTaskList() = default;

void RenderTaskList::operator delete(void* ptr)
{
  GetRenderTaskListMemoryPool().FreeThreadSafe(static_cast<RenderTaskList*>(ptr));
}

void RenderTaskList::Initialize(ResetterManager& resetterManager, RenderMessageDispatcher& renderMessageDispatcher)
{
  mResetterManager         = &resetterManager;
  mRenderMessageDispatcher = &renderMessageDispatcher;
}

void RenderTaskList::AddTask(OwnerPointer<RenderTask>& newTask)
{
  DALI_ASSERT_DEBUG(newTask && "SceneGraph RenderTask is null");
  DALI_ASSERT_DEBUG(mRenderMessageDispatcher != NULL && "RenderMessageDispatcher is null");

  newTask->Initialize(*mResetterManager, *mRenderMessageDispatcher);

  mRenderTasks.PushBack(newTask.Release());
}

void RenderTaskList::RemoveTask(RenderTask* task)
{
  RenderTaskContainer::ConstIterator end = mRenderTasks.End();
  for(RenderTaskContainer::Iterator iter = mRenderTasks.Begin(); iter != end; ++iter)
  {
    if(*iter == task)
    {
      // Destroy the task
      mRenderTasks.Erase(iter);

      break; // we're finished
    }
  }
}

void RenderTaskList::SortTasks(OwnerPointer<std::vector<const SceneGraph::RenderTask*>>& sortedTasks)
{
  for(uint32_t i = 0; i < sortedTasks->size(); ++i)
  {
    const SceneGraph::RenderTask* task = (*sortedTasks)[i];
    SceneGraph::RenderTask* castedTask = const_cast<SceneGraph::RenderTask*>(task);
    mRenderTasks[i]                    = castedTask;
  }
}

uint32_t RenderTaskList::GetTaskCount()
{
  return static_cast<uint32_t>(mRenderTasks.Count());
}

RenderTaskList::RenderTaskContainer& RenderTaskList::GetTasks()
{
  return mRenderTasks;
}

const RenderTaskList::RenderTaskContainer& RenderTaskList::GetTasks() const
{
  return mRenderTasks;
}

void RenderTaskList::SetCompleteNotificationInterface(CompleteNotificationInterface* object)
{
  mNotificationObject = object;
}

CompleteNotificationInterface* RenderTaskList::GetCompleteNotificationInterface()
{
  return mNotificationObject;
}

uint32_t RenderTaskList::GetMemoryPoolCapacity()
{
  return GetRenderTaskListMemoryPool().GetCapacity();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
