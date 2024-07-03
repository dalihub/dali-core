/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/public-api/common/dali-common.h>

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
namespace
{
static constexpr uint32_t ORDER_INDEX_OVERLAY_RENDER_TASK = INT32_MAX;
}

RenderTaskListPtr RenderTaskList::New()
{
  RenderTaskListPtr taskList = new RenderTaskList();

  taskList->Initialize();

  return taskList;
}

RenderTaskPtr RenderTaskList::CreateTask()
{
  return CreateTask(&mDefaults.GetDefaultRootActor(), &mDefaults.GetDefaultCameraActor());
}

RenderTaskPtr RenderTaskList::CreateTask(Actor* sourceActor, CameraActor* cameraActor, bool isOverlayTask)
{
  RenderTaskPtr task = RenderTask::New(sourceActor, cameraActor, *this);
  mTasks.push_back(task);

  if(isOverlayTask)
  {
    task->SetOrderIndex(ORDER_INDEX_OVERLAY_RENDER_TASK);
  }

  // Setup mapping infomations between scenegraph rendertask
  this->MapNotifier(task->GetRenderTaskSceneObject(), *task);

  return task;
}

RenderTaskPtr RenderTaskList::CreateOverlayTask(Actor* sourceActor, CameraActor* cameraActor)
{
  if(!mOverlayRenderTask)
  {
    mOverlayRenderTask = CreateTask(sourceActor, cameraActor, true);
  }
  return mOverlayRenderTask;
}

void RenderTaskList::RemoveTask(Internal::RenderTask& task)
{
  for(RenderTaskContainer::iterator iter = mTasks.begin(); mTasks.end() != iter; ++iter)
  {
    RenderTask* ptr = iter->Get();

    if(ptr == &task)
    {
      if(mOverlayRenderTask == &task)
      {
        mOverlayRenderTask.Reset();
      }

      // Remove mapping infomations
      this->UnmapNotifier(task.GetRenderTaskSceneObject());

      // delete the task
      mTasks.erase(iter);

      task.RemoveRenderTaskSceneObject(*this);

      Exclusive                     exclusive{ptr, ActorObserver()};
      ExclusivesContainer::iterator exclusiveIter = find(mExclusives.begin(), mExclusives.end(), exclusive);
      if(exclusiveIter != mExclusives.end())
      {
        mExclusives.erase(exclusiveIter);
      }
      break; // we're finished
    }
  }
}

uint32_t RenderTaskList::GetTaskCount() const
{
  return static_cast<uint32_t>(mTasks.size()); // only 4,294,967,295 render tasks supported
}

RenderTaskPtr RenderTaskList::GetTask(uint32_t index) const
{
  DALI_ASSERT_ALWAYS((index < mTasks.size()) && "RenderTask index out-of-range");

  return mTasks[index];
}

RenderTaskPtr RenderTaskList::GetOverlayTask() const
{
  RenderTaskPtr overlayRenderTask;
  if(mOverlayRenderTask)
  {
    overlayRenderTask = mOverlayRenderTask;
  }
  return overlayRenderTask;
}

void RenderTaskList::SetExclusive(RenderTask* task, bool exclusive)
{
  // Check to see if this rendertask has an entry?
  for(auto exclusiveIt = mExclusives.begin(); exclusiveIt != mExclusives.end(); ++exclusiveIt)
  {
    if(exclusiveIt->renderTaskPtr == task)
    {
      if(!exclusive)
      {
        mExclusives.erase(exclusiveIt);
        break;
      }
      else
      {
        exclusiveIt->actor.SetActor(task->GetSourceActor());
        exclusive = false;
        break;
      }
    }
  }
  if(exclusive)
  {
    Exclusive exclusiveSlot;
    exclusiveSlot.renderTaskPtr = task;
    exclusiveSlot.actor.SetActor(task->GetSourceActor());
    mExclusives.emplace_back(std::move(exclusiveSlot));
  }
}

void RenderTaskList::SortTasks()
{
  if(!mIsRequestedToSortTask)
  {
    return;
  }

  std::stable_sort(mTasks.begin(), mTasks.end(), [](RenderTaskPtr first, RenderTaskPtr second) -> bool { return first->GetOrderIndex() < second->GetOrderIndex(); });

  OwnerPointer<std::vector<const SceneGraph::RenderTask*>> sortedTasks(new std::vector<const SceneGraph::RenderTask*>());
  for(auto&& task : mTasks)
  {
    sortedTasks->push_back(task->GetRenderTaskSceneObject());
  }
  SortTasksMessage(mEventThreadServices, *mSceneObject, sortedTasks);
  mIsRequestedToSortTask = false;
}

RenderTaskList::RenderTaskList()
: mEventThreadServices(EventThreadServices::Get()),
  mDefaults(*Stage::GetCurrent()),
  mSceneObject(nullptr)
{
}

RenderTaskList::~RenderTaskList()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~RenderTaskList[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mSceneObject))
  {
    // Remove the render task list using a message to the update manager
    RemoveRenderTaskListMessage(mEventThreadServices.GetUpdateManager(), *mSceneObject);
  }
}

void RenderTaskList::Initialize()
{
  // Create a new render task list, Keep a const pointer to the render task list.
  mSceneObject = SceneGraph::RenderTaskList::New();

  OwnerPointer<SceneGraph::RenderTaskList> transferOwnership(const_cast<SceneGraph::RenderTaskList*>(mSceneObject));
  AddRenderTaskListMessage(mEventThreadServices.GetUpdateManager(), transferOwnership);

  // set the callback to call us back when tasks are completed
  mSceneObject->SetCompleteNotificationInterface(this);
}

void RenderTaskList::NotifyCompleted(CompleteNotificationInterface::ParameterList notifierIdList)
{
  DALI_LOG_TRACE_METHOD(gLogRenderList);

  RenderTaskContainer finishedRenderTasks;

  for(const auto& notifierId : notifierIdList)
  {
    auto* renderTask = GetEventObject(notifierId);
    if(DALI_LIKELY(renderTask))
    {
      // Check if this render task hold inputed scenegraph render task.
      DALI_ASSERT_DEBUG(renderTask->GetRenderTaskSceneObject()->GetNotifyId() == notifierId);

      if(renderTask->HasFinished())
      {
        finishedRenderTasks.push_back(renderTask);
      }
    }
  }

  // Now it's safe to emit the signals
  for(auto&& item : finishedRenderTasks)
  {
    item->EmitSignalFinish();
  }
}

void RenderTaskList::RecoverFromContextLoss()
{
  for(auto&& item : mTasks)
  {
    // If the render target renders only once to an offscreen, re-render the render task
    if(item->GetRefreshRate() == Dali::RenderTask::REFRESH_ONCE && item->GetFrameBuffer())
    {
      item->SetRefreshRate(Dali::RenderTask::REFRESH_ONCE);
    }
  }
}

const SceneGraph::RenderTaskList& RenderTaskList::GetSceneObject() const
{
  return *mSceneObject;
}

} // namespace Internal

} // namespace Dali
