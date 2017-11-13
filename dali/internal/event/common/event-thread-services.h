#ifndef __DALI_INTERNAL_EVENT_THREAD_SERVICES_H__
#define __DALI_INTERNAL_EVENT_THREAD_SERVICES_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>

namespace Dali
{

class BaseObject;

namespace Integration
{
class RenderController;
}

namespace Internal
{

namespace SceneGraph
{
class UpdateManager;
}

/**
 * Abstract interface of services for event-thread objects.
 * Used for registering objects, queueing messages during the event-thread for the next update.
 * Allows the event-thread to read double-buffered property values.
 */
class EventThreadServices
{
public:

  /**
   * Virtual destructor
   */
  virtual ~EventThreadServices()
  { }

  /**
   * @brief Registers the object as created with the Object registry.
   *
   * @param[in] object to register
   */
  virtual void RegisterObject( BaseObject* object) = 0;

  /**
   * @brief Unregisters the object from Object registry.
   *
   * @param[in] object to unregister
   */
  virtual void UnregisterObject( BaseObject* object) = 0;

  /**
   * @brief Get a reference to the UpdateManager
   *
   * @return the update manager
   */
  virtual SceneGraph::UpdateManager& GetUpdateManager() = 0;

  /**
   * @brief Get a reference to the RenderController
   *
   * @return the render controller
   */
  virtual Integration::RenderController& GetRenderController() = 0;

  /**
   * Reserve space for another message in the queue; this must then be initialized by the caller.
   * The message will be read from the update-thread after the next FlushMessages is called.
   * @post Calling this method may invalidate any previously returned slots.
   * @param[in] size The message size with respect to the size of type "char".
   * @param[in] updateScene A flag, when true denotes that the message will cause the scene-graph node tree to require an update.
   * @return A pointer to the first char allocated for the message.
   */
  virtual unsigned int* ReserveMessageSlot( std::size_t size, bool updateScene = true ) = 0;

  /**
   * @return the current event-buffer index.
   */
  virtual BufferIndex GetEventBufferIndex() const  = 0;

  /**
   * @brief Indicate that the next rendering is really required.
   */
  virtual void ForceNextUpdate() = 0;

  /**
   * @brief Check if the next rendering is really required.
   *
   * @return true if the next rendering is really required.
   */
  virtual bool IsNextUpdateForced() = 0;

  /**
   * @return true if core is still running and we can send messages
   */
  static bool IsCoreRunning();
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EVENT_THREAD_SERVICES_H__
