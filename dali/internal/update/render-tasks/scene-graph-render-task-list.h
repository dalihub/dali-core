#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_LIST_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_LIST_H

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

// INTERNAL INCLUDES
#include <dali/integration-api/ordered-set.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner-flag-manager.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>

namespace Dali
{
namespace Internal
{
class CompleteNotificationInterface;

namespace SceneGraph
{
class RenderMessageDispatcher;
class RenderTask;
class ResetterManager;

/**
 * An ordered list of render-tasks.
 */
class RenderTaskList
{
public:
  using RenderTaskContainer = Dali::Integration::OrderedSet<RenderTask>;

  /**
   * Construct a new RenderTaskList.
   * @return A new RenderTaskList
   */
  static RenderTaskList* New();

  /**
   * Clear memory pool of render task list.
   * This should be called at the begin of Core.
   * (Since Core could be recreated, we need to reset the memory pool.)
   * After this API call, all SceneGraph::RenderTaskList classes are invalid.
   */
  static void ResetMemoryPool();

  /**
   * Destructor
   */
  ~RenderTaskList();

  /**
   * Overriden delete operator
   * Deletes the RenderTaskList from its global memory pool
   */
  void operator delete(void* ptr);

  /**
   * Set the resetterManager and renderMessageDispatcher to send message.
   * @param[in] resetterManager to send resetter
   * @param[in] propertyOwnerFlagManager to discard render task
   * @param[in] renderMessageDispatcher The renderMessageDispatcher to send messages.
   */
  void Initialize(ResetterManager& resetterManager, PropertyOwnerFlagManager& propertyOwnerFlagManager, RenderMessageDispatcher& renderMessageDispatcher);

  /**
   * Add a new RenderTask to the list.
   * @param[in] newTask The RenderTaskList takes ownership of this task.
   */
  void AddTask(OwnerPointer<RenderTask>& newTask);

  /**
   * Remove a RenderTask from the list.
   * @param[in] task The RenderTaskList will destroy this task.
   */
  void RemoveTask(RenderTask* task);

  /**
   * Retrieve the count of RenderTasks.
   * @return The count.
   */
  uint32_t GetTaskCount();

  /**
   * Retrieve the container of RenderTasks.
   * @return The container.
   */
  RenderTaskContainer& GetTasks();

  /**
   * Retrieve the container of RenderTasks.
   * @return The container.
   */
  const RenderTaskContainer& GetTasks() const;

  /**
   * Set the notification method to package in the NotifyFinishedMessage
   * @param object to store in notification managers queue
   */
  void SetCompleteNotificationInterface(CompleteNotificationInterface* object);

  /**
   * Get the Notification interface for when 1+ render tasks have finished
   */
  CompleteNotificationInterface* GetCompleteNotificationInterface();

  /**
   * Get the capacity of the memory pools
   */
  static uint32_t GetMemoryPoolCapacity();

  /**
   * @brief Sort RenderTasks along OrderIndex
   */
  void SortTasks(OwnerPointer<std::vector<const SceneGraph::RenderTask*>>& sortedTasks);

protected:
  /**
   * Protected constructor. See New()
   */
  RenderTaskList();

private:
  // Undefined
  RenderTaskList(const RenderTaskList&);

  // Undefined
  RenderTaskList& operator=(const RenderTaskList&);

private:
  CompleteNotificationInterface* mNotificationObject;       ///< object to pass in to the complete notification
  ResetterManager*               mResetterManager;          ///< for sending bake resetter if rendertask initalized
  PropertyOwnerFlagManager*      mPropertyOwnerFlagManager; ///< for sending discard it rendertask destroyed
  RenderMessageDispatcher*       mRenderMessageDispatcher;  ///< for sending messages to render thread
  RenderTaskContainer            mRenderTasks;              ///< A container of owned RenderTasks
};

// Messages for RenderTaskList

inline void AddTaskMessage(EventThreadServices& eventThreadServices, const RenderTaskList& list, OwnerPointer<RenderTask>& task)
{
  // Message has ownership of the RenderTask while in transit from event -> update
  using LocalType = MessageValue1<RenderTaskList, OwnerPointer<RenderTask>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&list, &RenderTaskList::AddTask, task);
}

inline void RemoveTaskMessage(EventThreadServices& eventThreadServices, const RenderTaskList& list, const RenderTask& constTask)
{
  // Scene graph thread can destroy this object.
  RenderTask& task = const_cast<RenderTask&>(constTask);

  using LocalType = MessageValue1<RenderTaskList, RenderTask*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&list, &RenderTaskList::RemoveTask, &task);
}

inline void SortTasksMessage(EventThreadServices& eventThreadServices, const RenderTaskList& list, OwnerPointer<std::vector<const SceneGraph::RenderTask*>>& sortedTasks)
{
  using LocalType = MessageValue1<RenderTaskList, OwnerPointer<std::vector<const SceneGraph::RenderTask*>>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&list, &RenderTaskList::SortTasks, sortedTasks);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_LIST_H
