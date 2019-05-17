#ifndef DALI_INTEGRATION_RENDER_TASK_LIST_H
#define DALI_INTEGRATION_RENDER_TASK_LIST_H

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
#include <dali/public-api/render-tasks/render-task-list.h>

namespace Dali
{

class Actor;
class CameraActor;

namespace Integration
{

namespace RenderTaskList
{

  /**
   * @brief Helper function to create a new RenderTaskList object.
   *
   * A RenderTaskList is an ordered list of render tasks that describes
   * how the Dali scene should be rendered.
   *
   * @return A handle to a newly allocated RenderTaskList
   */
  DALI_CORE_API Dali::RenderTaskList New();

  /**
   * @brief Helper function to create a new RenderTask.
   *
   * The created task will be appended to the list of render-tasks.
   *
   * @param[in] taskList 　　　The render task list containing all the render-tasks.
   * @param[in] sourceActor The actor and its children to be rendered for this render task.
   * @param[in] cameraActor The actor from which the scene is viewed for this render task.
   * @return A valid handle to a new RenderTask
   */
  DALI_CORE_API Dali::RenderTask CreateTask( Dali::RenderTaskList& taskList, Dali::Actor& sourceActor, Dali::CameraActor& cameraActor);

} // namespace RenderTaskList

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_RENDER_TASK_LIST_H
