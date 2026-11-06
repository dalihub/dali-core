#ifndef DALI_INTERNAL_RENDER_TASK_LIST_H
#define DALI_INTERNAL_RENDER_TASK_LIST_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/render-tasks/render-task-list.h>

#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/common/complete-notification-interface.h>
#include <dali/internal/event/common/event-thread-services-holder.h>
#include <dali/internal/event/common/scene-graph-notifier-interface-mapper.h>
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace DALI_NAMESPACE
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
} // namespace SceneGraph

/**
 * A proxy for the scene-graph RenderTaskList.
 * A separate LayerList is maintained for actors added via the SystemLevel::Add().
 */
class RenderTaskList : public BaseObject, public CompleteNotificationInterface, public SceneGraphNotifierInterfaceMapper<RenderTask>, public EventThreadServicesHolder
{
public:
  using RenderTaskContainer = std::vector<RenderTaskPtr>;

  /**
   * Create a RenderTaskList.
   * @param[in] defaults Provider of the default root actor and camera actor.
   *                     If nullptr, the no-arg CreateTask() must not be called.
   * @return A newly allocated RenderTaskList; the caller takes ownership.
   */
  static RenderTaskListPtr New(RenderTaskDefaults* defaults = nullptr);

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
  RenderTaskPtr CreateTask(Actor* sourceActor, CameraActor* cameraActor, bool isOverlayTask = false);

  /**
   * @brief Creates a new RenderTask for overlay.
   * This will be appended to the end of render-task list.
   * @param[in] sourceActor The actor and its children to be rendered for this render task.
   * @param[in] cameraActor The actor from which the scene is viewed for this render task.
   * @return A valid handle to a new overlay RenderTask
   * @note The Overlay RenderTask will be rendered after all the other render tasks are rendered.
   */
  RenderTaskPtr CreateOverlayTask(Actor* sourceActor, CameraActor* cameraActor);

  /**
   * @copydoc Dali::RenderTaskList::RemoveTask()
   */
  void RemoveTask(Internal::RenderTask& task);

  /**
   * @copydoc Dali::RenderTaskList::GetTaskCount()
   */
  uint32_t GetTaskCount() const;

  /**
   * @copydoc Dali::RenderTaskList::GetTask()
   */
  RenderTaskPtr GetTask(uint32_t index) const;

  /**
   * @copydoc Dali::RenderTaskList::GetOverlayTask()
   */
  RenderTaskPtr GetOverlayTask() const;

  /**
   * Retrieve the container of render-tasks.
   * @return The container.
   */
  RenderTaskContainer& GetTasks()
  {
    return mTasks;
  }

  /**
   * @brief Synchronize a task's exclusive source and mapping registrations.
   *
   * Reflects changes to the source actor and screen-to-framebuffer mapping actor.
   * Removes both registrations when exclusive is false, including task removal.
   *
   * @param[in] task The task whose registrations should be updated.
   * @param[in] exclusive Whether the task should remain exclusively registered.
   */
  void UpdateExclusiveRegistration(RenderTask* task, bool exclusive);

  /**
   * @brief Queries whether an actor is the source of any exclusive render task.
   *
   * @param[in] actor The actor to query.
   * @return True if the actor is the source of an exclusive render task, false otherwise.
   */
  bool IsSourceActorOfExclusiveTask(const Actor& actor) const;

  /**
   * @brief Retrieves the first registered exclusive task whose source and mapping actor both match the actor.
   *
   * @param[in] actor The source and screen-to-framebuffer mapping actor to match.
   * @return A retained render task, or an empty pointer if no matching task exists.
   */
  RenderTaskPtr GetExclusiveMappingTask(const Actor& actor) const;

  /**
   * @brief Request to sort RenderTasks along OrderIndex
   */
  void RequestToSort()
  {
    mIsRequestedToSortTask = true;
  }

  /**
   * @brief Sort RenderTasks along OrderIndex
   */
  void SortTasks();

  /**
   * @brief Requests to reorder the RenderTasks of this RenderTaskList.
   */
  void RequestReorder()
  {
    mIsRequestedToReorderTask = true;
  }

  /**
   * @brief Retrieve whether Reordering is required or not.
   */
  bool IsReorderRequested() const
  {
    return mIsRequestedToReorderTask;
  }

  /**
   * @brief Reorder RenderTasks along the priority of OffScreenRenderableType of each Actor.
   *
   * @param[in] layerList layerList that used for tasks reordering.
   */
  void ReorderTasks(Dali::Internal::LayerList& layerList);

  /**
   * Provide notification signals for a "Finished" render task.
   * This method should be called in the event-thread
   * Queue NotifyFinishedMessage() from update-thread
   * @param object pointer to this class instance
   */
  static void NotifyFinished(void* object);

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
   * @param[in] defaults Provider of the default root actor and camera actor. May be nullptr.
   */
  explicit RenderTaskList(RenderTaskDefaults* defaults);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~RenderTaskList() override;

  /**
   * 2nd-phase construction
   */
  void Initialize();

private: // from CompleteNotificationInterface
  /**
   * @copydoc CompleteNotificationInterface::NotifyCompleted()
   */
  void NotifyCompleted(CompleteNotificationInterface::ParameterList notifierList) override;

private:
  struct ExclusiveRegistration
  {
    ActorObserver actor;                ///< Identifies the observed source lifetime, independently of its address.
    Actor*        source{nullptr};      ///< Hash bucket key only; never used as actor identity.
    bool          isMappingTask{false}; ///< Source and screen-to-framebuffer mapping actor match.
  };

  struct ExclusiveSource
  {
    std::vector<RenderTask*> tasks;
    std::vector<RenderTask*> mappingTasks;
  };

  /**
   * @brief Removes a task from its exclusive source and mapping task lists.
   *
   * Removes the source entry if no tasks remain and clears the mapping registration flag.
   *
   * @param[in] task The render task to remove.
   * @param[in,out] registration The task's source registration whose mapping flag is cleared.
   */
  void RemoveExclusiveSource(RenderTask* task, ExclusiveRegistration& registration);

private:
  std::unordered_map<RenderTask*, ExclusiveRegistration> mExclusiveTasks;   ///< Direct registration lookup.
  std::unordered_map<const Actor*, ExclusiveSource>      mExclusiveSources; ///< Tasks grouped by source; order is unspecified.

  RenderTaskDefaults* mDefaults; ///< Provider of default root/camera actors; may be nullptr for standalone lists.

  SceneGraph::RenderTaskList* mSceneObject; ///< Raw-pointer to the scene-graph object; not owned.

  RenderTaskContainer mTasks; ///< Reference counted render-tasks
  RenderTaskPtr       mOverlayRenderTask{nullptr};

  bool mIsRequestedToSortTask{false};
  bool mIsRequestedToReorderTask{false};
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

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_RENDER_TASK_LIST_H
