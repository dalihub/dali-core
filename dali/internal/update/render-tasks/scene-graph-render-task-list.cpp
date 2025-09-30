/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>

namespace //Unnamed namespace
{
Dali::Internal::SceneGraph::MemoryPoolCollection*                                 gMemoryPoolCollection = nullptr;
static constexpr Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType gMemoryPoolType       = Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType::RENDER_TASK_LIST;
} // unnamed namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
RenderTaskList* RenderTaskList::New()
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "RenderTaskList::RegisterMemoryPoolCollection not called!");
  return new(gMemoryPoolCollection->AllocateRawThreadSafe(gMemoryPoolType)) RenderTaskList();
}

void RenderTaskList::RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection)
{
  gMemoryPoolCollection = &memoryPoolCollection;
}

void RenderTaskList::UnregisterMemoryPoolCollection()
{
  gMemoryPoolCollection = nullptr;
}

RenderTaskList::RenderTaskList()
: mNotificationObject(nullptr),
  mResetterManager(nullptr),
  mPropertyOwnerFlagManager(nullptr),
  mRenderMessageDispatcher(nullptr)
{
}

RenderTaskList::~RenderTaskList() = default;

void RenderTaskList::operator delete(void* ptr)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "RenderTaskList::RegisterMemoryPoolCollection not called!");
  gMemoryPoolCollection->FreeThreadSafe(gMemoryPoolType, ptr);
}

void RenderTaskList::Initialize(ResetterManager& resetterManager, PropertyOwnerFlagManager& propertyOwnerFlagManager, RenderMessageDispatcher& renderMessageDispatcher)
{
  mResetterManager          = &resetterManager;
  mPropertyOwnerFlagManager = &propertyOwnerFlagManager;
  mRenderMessageDispatcher  = &renderMessageDispatcher;
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
  auto iter = mRenderTasks.Find(task);
  if(iter != mRenderTasks.End())
  {
    // Discard the task. Move ownership to property owner manager.
    // DevNote : Since RenderTaskList itself could be removed at the same frame with
    //           RenderTask dirty, discarded memory ownership must be out of RenderTaskList.
    mPropertyOwnerFlagManager->DiscardPropertyOwner(mRenderTasks.Release(iter));
  }
}

void RenderTaskList::SortTasks(OwnerPointer<std::vector<const SceneGraph::RenderTask*>>& sortedTasks)
{
  DALI_ASSERT_ALWAYS(sortedTasks->size() == mRenderTasks.Count() && "SceneGraph RenderTask list is not matched with Event side RenderTask list!");

  auto iter = mRenderTasks.Begin();
  for(auto sortedIter = sortedTasks->begin(), sortedIterEnd = sortedTasks->end(); sortedIter != sortedIterEnd; ++sortedIter, ++iter)
  {
    const SceneGraph::RenderTask* task       = *sortedIter;
    SceneGraph::RenderTask*       castedTask = const_cast<SceneGraph::RenderTask*>(task);
    (*iter)                                  = castedTask;
  }

  // We should call Reorder after the order of container changed.
  mRenderTasks.ReorderCacheMap();
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

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
