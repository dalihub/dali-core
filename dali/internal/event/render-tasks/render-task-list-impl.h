#ifndef __DALI_INTERNAL_RENDER_TASK_LIST_H__
#define __DALI_INTERNAL_RENDER_TASK_LIST_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/common/complete-notification-interface.h>

namespace Dali
{

namespace Internal
{

class EventToUpdate;
class RenderTaskDefaults;

namespace SceneGraph
{
class RenderTaskList;
class UpdateManager;
}

/**
 * A proxy for the scene-graph RenderTaskList.
 * A separate LayerList is maintained for actors added via the SystemLevel::Add().
 */
class RenderTaskList : public BaseObject, public CompleteNotificationInterface
{
public:

  typedef std::vector< Dali::RenderTask > RenderTaskContainer;

  /**
   * Create a RenderTaskList.
   * @param[in] updateManager Used for creating render-tasks in the scene graph.
   * @param[in] defaults Provides the default source & camera actors.
   * @param[in] systemLevel True if this is the LayerList for actors added via the SystemLevel::Add().
   * @return A newly allocated RenderTaskList; the caller takes ownership.
   */
  static RenderTaskList* New( SceneGraph::UpdateManager& updateManager, RenderTaskDefaults& defaults, bool systemLevel );

  /**
   * @copydoc Dali::RenderTaskList::CreateTask()
   */
  Dali::RenderTask CreateTask();

  /**
   * @copydoc Dali::RenderTaskList::RemoveTask()
   */
  void RemoveTask( Dali::RenderTask task );

  /**
   * @copydoc Dali::RenderTaskList::GetTaskCount()
   */
  unsigned int GetTaskCount() const;

  /**
   * @copydoc Dali::RenderTaskList::GetTask()
   */
  Dali::RenderTask GetTask( unsigned int index ) const;

  /**
   * Retrieve the container of render-tasks.
   * @return The container.
   */
  RenderTaskContainer& GetTasks()
  {
    return mTasks;
  }

  /**
   * Provide notification signals for a "Finished" render task.
   * This method should be called in the event-thread
   * Queue NotifyFinishedMessage() from update-thread
   * @param object pointer to this class instance
   */
  static void NotifyFinished( void* object );

protected:

  /**
   * Construct a new RenderTaskList.
   * @param[in] eventToUpdate Used for creating render-tasks in the scene graph.
   * @param[in] defaults Provides the default source & camera actors.
   * @param[in] systemLevel True if this is the system-level list.
   */
  RenderTaskList( EventToUpdate& eventToUpdate, RenderTaskDefaults& defaults, bool systemLevel );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RenderTaskList();

  /**
   * 2nd-phase construction
   */
  void Initialize( SceneGraph::UpdateManager& updateManager );

private: // from CompleteNotificationInterface

  /**
   * @copydoc CompleteNotificationInterface::NotifyCompleted()
   */
  virtual void NotifyCompleted();

private:

  EventToUpdate& mEventToUpdate;
  RenderTaskDefaults& mDefaults;

  bool mIsSystemLevel; ///< True if the layers are added via the SystemLevel API

  SceneGraph::RenderTaskList* mSceneObject; ///< Raw-pointer to the scene-graph object; not owned.

  RenderTaskContainer mTasks; ///< Reference counted render-tasks
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::RenderTaskList& GetImplementation(Dali::RenderTaskList& taskList)
{
  DALI_ASSERT_ALWAYS(taskList && "RenderTaskList handle is empty");

  BaseObject& handle = taskList.GetBaseObject();

  return static_cast<Internal::RenderTaskList&>(handle);
}

inline const Internal::RenderTaskList& GetImplementation(const Dali::RenderTaskList& taskList)
{
  DALI_ASSERT_ALWAYS(taskList && "RenderTaskList handle is empty");

  const BaseObject& handle = taskList.GetBaseObject();

  return static_cast<const Internal::RenderTaskList&>(handle);
}

} // namespace Dali

#endif //__DALI_INTERNAL_RENDER_TASK_LIST_H__
