#ifndef DALI_INTERNAL_RENDER_TASK_LIST_H
#define DALI_INTERNAL_RENDER_TASK_LIST_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/event/common/complete-notification-interface.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

class EventThreadServices;
class RenderTaskDefaults;
class Actor;
class CameraActor;

using RenderTaskPtr = IntrusivePtr<RenderTask>;
class RenderTaskList;
using RenderTaskListPtr = IntrusivePtr<RenderTaskList>;

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

  using RenderTaskContainer = std::vector< RenderTaskPtr >;

  struct Exclusive
  {
    RenderTask* renderTaskPtr;        ///< Pointer for comparison with current rendertask.
    Actor* actorPtr;                  ///< Pointer for comparison with current actor.
  };

  /**
   * Create a RenderTaskList.
   * @return A newly allocated RenderTaskList; the caller takes ownership.
   */
  static RenderTaskListPtr New();

  /**
   * @copydoc Dali::RenderTaskList::CreateTask()
   */
  RenderTaskPtr CreateTask();

  /**
   * @brief Creates a new RenderTask.
   *
   * This will be appended to the list of render-tasks.
   *
   * @param[in] sourceActor The actor and its children to be rendered for this render task.
   * @param[in] cameraActor The actor from which the scene is viewed for this render task.
   * @return A valid handle to a new RenderTask
   */
  RenderTaskPtr CreateTask( Actor* sourceActor, CameraActor* cameraActor);

  /**
   * @copydoc Dali::RenderTaskList::RemoveTask()
   */
  void RemoveTask( Internal::RenderTask& task );

  /**
   * @copydoc Dali::RenderTaskList::GetTaskCount()
   */
  uint32_t GetTaskCount() const;

  /**
   * @copydoc Dali::RenderTaskList::GetTask()
   */
  RenderTaskPtr GetTask( uint32_t index ) const;

  /**
   * Retrieve the container of render-tasks.
   * @return The container.
   */
  RenderTaskContainer& GetTasks()
  {
    return mTasks;
  }

  /**
   * @brief Mark a rendertask as having exclusive access to its source actor.
   *
   * @param[in] task Pointer to the rendertask.
   * @param[in] exclusive If a rendertask is to have exclusive acesss to its source actor.
   */
  void SetExclusive( RenderTask* task, bool exclusive );

  /**
   * @brief Return the list of rendertasks that exclusively own their source actor.
   *
   * @return [description]
   */
  const Vector< Exclusive >& GetExclusivesList() const
  {
    return mExclusives;
  }

  /**
   * Provide notification signals for a "Finished" render task.
   * This method should be called in the event-thread
   * Queue NotifyFinishedMessage() from update-thread
   * @param object pointer to this class instance
   */
  static void NotifyFinished( void* object );

  /**
   * This method refreshes all render tasks that have a frame buffer
   * and a refresh rate of RefreshOnce.
   */
  void RecoverFromContextLoss();

  /**
   * Retrieve the SceneGraph::RenderTaskList object.
   * @return The RenderTaskList.
   */
  const SceneGraph::RenderTaskList& GetSceneObject() const;

protected:

  /**
   * Construct a new RenderTaskList.
   */
  RenderTaskList();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~RenderTaskList();

  /**
   * 2nd-phase construction
   */
  void Initialize();

private: // from CompleteNotificationInterface

  /**
   * @copydoc CompleteNotificationInterface::NotifyCompleted()
   */
  virtual void NotifyCompleted();

private:

  EventThreadServices& mEventThreadServices;
  RenderTaskDefaults& mDefaults;

  SceneGraph::RenderTaskList* mSceneObject; ///< Raw-pointer to the scene-graph object; not owned.

  RenderTaskContainer mTasks;           ///< Reference counted render-tasks
  Vector< Exclusive > mExclusives;      ///< List of rendertasks with exclusively owned source actors.
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

#endif // DALI_INTERNAL_RENDER_TASK_LIST_H
