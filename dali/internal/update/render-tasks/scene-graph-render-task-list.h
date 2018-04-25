#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_LIST_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_LIST_H__

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

// INTERNAL INCLUDES
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>

namespace Dali
{

namespace Internal
{

class CompleteNotificationInterface;

namespace SceneGraph
{

class RenderTask;

/**
 * An ordered list of render-tasks.
 */
class RenderTaskList
{
public:

  typedef OwnerContainer< RenderTask* > RenderTaskContainer;

  /**
   * Constructor
   * @param renderMessageDispatcher to send messages
   */
  RenderTaskList( /*RenderMessageDispatcher& renderMessageDispatcher*/ );

  /**
   * Destructor
   */
  ~RenderTaskList();

  /**
   * Add a new RenderTask to the list.
   * @param[in] newTask The RenderTaskList takes ownership of this task.
   */
  void AddTask( OwnerPointer< RenderTask >& newTask );

  /**
   * Remove a RenderTask from the list.
   * @param[in] task The RenderTaskList will destroy this task.
   */
  void RemoveTask( RenderTask* task );

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
  void SetCompleteNotificationInterface( CompleteNotificationInterface* object );

  /**
   * Get the Notification interface for when 1+ render tasks have finished
   */
  CompleteNotificationInterface* GetCompleteNotificationInterface();

private:

  // Undefined
  RenderTaskList(const RenderTaskList&);

  // Undefined
  RenderTaskList& operator=(const RenderTaskList&);

private:

  CompleteNotificationInterface* mNotificationObject; ///< object to pass in to the complete notification
  RenderTaskContainer mRenderTasks; ///< A container of owned RenderTasks
};

// Messages for RenderTaskList

inline void AddTaskMessage( EventThreadServices& eventThreadServices, RenderTaskList& list, OwnerPointer< RenderTask >& task )
{
  // Message has ownership of the RenderTask while in transit from event -> update
  typedef MessageValue1< RenderTaskList, OwnerPointer< RenderTask > > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &list, &RenderTaskList::AddTask, task );
}

inline void RemoveTaskMessage( EventThreadServices& eventThreadServices, RenderTaskList& list, const RenderTask& constTask )
{
  // Scene graph thread can destroy this object.
  RenderTask& task = const_cast< RenderTask& >( constTask );

  typedef MessageValue1< RenderTaskList, RenderTask* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &list, &RenderTaskList::RemoveTask, &task );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_LIST_H__
