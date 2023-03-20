#ifndef DALI_RENDER_TASK_LIST_H
#define DALI_RENDER_TASK_LIST_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
/**
 * @addtogroup dali_core_rendering_effects
 * @{
 */

class RenderTask;

namespace Internal DALI_INTERNAL
{
class RenderTaskList;
}

/**
 * @brief An ordered list of Dali::RenderTask%s.
 *
 * These tasks describe how the Dali scene should be rendered; @see Dali::RenderTask for more details.
 * @SINCE_1_0.0
 */
class DALI_CORE_API RenderTaskList : public BaseHandle
{
public:
  /**
   * @brief Creates an empty RenderTaskList handle.
   *
   * This can be initialised with Stage::GetRenderTaskList().
   * @SINCE_1_0.0
   */
  RenderTaskList();

  /**
   * @brief Downcasts a handle to RenderTaskList handle.
   *
   * If handle points to a RenderTaskList the downcast produces valid
   * handle. If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle A handle to an object
   * @return A handle to a RenderTaskList or an uninitialized handle
   */
  static RenderTaskList DownCast(BaseHandle handle);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~RenderTaskList();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  RenderTaskList(const RenderTaskList& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs A reference to the copied handle
   * @return A reference to this
   */
  RenderTaskList& operator=(const RenderTaskList& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   */
  RenderTaskList(RenderTaskList&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  RenderTaskList& operator=(RenderTaskList&& rhs) noexcept;

  /**
   * @brief Creates a new RenderTask.
   *
   * This will be appended to the list of render-tasks.
   * @SINCE_1_0.0
   * @return A valid handle to a new RenderTask
   */
  RenderTask CreateTask();

  /**
   * @brief Removes a RenderTask from the list of render-tasks.
   * @SINCE_1_0.0
   * @param[in] task The render-task to remove.
   */
  void RemoveTask(RenderTask task);

  /**
   * @brief Queries the number of render-tasks in the list.
   *
   * This is ordered i.e. the task with index 0 is the first to be processed each frame.
   * @SINCE_1_0.0
   * @return The number of render-tasks
   */
  uint32_t GetTaskCount() const;

  /**
   * @brief Retrieves a render-task.
   *
   * @SINCE_1_0.0
   * @param[in] index The index of the render task to retrieve
   * @return A handle to the render-task
   * @pre index should be in range i.e. less than GetTaskCount().
   */
  RenderTask GetTask(uint32_t index) const;

  /**
   * @brief Retrieves a RenderTask for Overlay
   * @SINCE_2_2.10
   * @return A handle to the overlay RenderTask.
   * If the scene has not created an overlay render task, this returns empty handle.
   */
  RenderTask GetOverlayTask() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @SINCE_1_0.0
   * @param [in] renderTask A pointer to a newly allocated render-task
   */
  explicit DALI_INTERNAL RenderTaskList(Internal::RenderTaskList* renderTask);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif //DALI_RENDER_TASK_LIST_H
