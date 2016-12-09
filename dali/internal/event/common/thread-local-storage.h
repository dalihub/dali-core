#ifndef __DALI_INTERNAL_THREAD_LOCAL_STORAGE_H__
#define __DALI_INTERNAL_THREAD_LOCAL_STORAGE_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/stage-def.h>

namespace Dali
{

struct Vector2;

namespace Integration
{
class PlatformAbstraction;
}

namespace Internal
{

class Core;
class NotificationManager;
class ResourceClient;
class ResourceManager;
class ShaderFactory;
class GestureEventProcessor;
class RelayoutController;

namespace SceneGraph
{
class UpdateManager;
}

/**
 * Class to store a pointer to core in thread local storage.
 *
 */
class ThreadLocalStorage
{
public:

  /**
   * Constructor
   * Creates the TLS and adds a pointer to core
   * @param [in] core reference to core
   */
  ThreadLocalStorage(Core* core);

  /**
   * Destructor.
   */
  ~ThreadLocalStorage();

  /**
   * Remove core pointer.
   * Prevents the core pointer being automatically deleted when the thread exits.
   */
  void Remove();

  /**
   * Get the TLS
   * @return reference to the TLS
   */
  static ThreadLocalStorage& Get();

  /**
   * Checks if the TLS has been created
   * @return if the TLS has been created
   */
  static bool Created();

  /**
   * Get a pointer to the TLS or NULL if not initialized
   * @return pointer to the TLS
   */
  static ThreadLocalStorage* GetInternal();

  /**
   * get platform abstraction
   * @return reference to core
   */
  Dali::Integration::PlatformAbstraction& GetPlatformAbstraction();

  /**
   * Retrieve the update manager
   * @return reference to update manager
   */
  SceneGraph::UpdateManager& GetUpdateManager();

  /**
   * Returns the Notification Manager
   * @return reference to the Notification Manager
   */
  NotificationManager& GetNotificationManager();

  /**
   * Returns the Resource Manager
   * @return reference to the Resource Manager
   */
  ResourceManager& GetResourceManager();

  /**
   * Returns the Resource Client
   * @return reference to the Resource Client
   */
  ResourceClient& GetResourceClient();

  /**
   * Returns the Shader Factory
   * @return reference to the Shader Factory
   */
  ShaderFactory& GetShaderFactory();

  /**
   * Returns the current stage.
   * @return A pointer to the current stage.
   */
  StagePtr GetCurrentStage();

  /**
   * Returns the gesture event processor.
   * @return A reference to the gesture event processor.
   */
  GestureEventProcessor& GetGestureEventProcessor();

  /**
   * Return the relayout controller
   * @Return Return a reference to the relayout controller
   */
  RelayoutController& GetRelayoutController();

private:

  Core* mCore;                                              ///< reference to core

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_THREAD_LOCAL_STORAGE_H__
