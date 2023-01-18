#ifndef DALI_INTERNAL_THREAD_LOCAL_STORAGE_H
#define DALI_INTERNAL_THREAD_LOCAL_STORAGE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>
#include <dali/internal/event/common/scene-impl.h>
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
class ShaderFactory;
class GestureEventProcessor;
class RelayoutController;
class ObjectRegistry;
class EventThreadServices;
class PropertyNotificationManager;
class AnimationPlaylist;

namespace SceneGraph
{
class UpdateManager;
}

/**
 * Class to store a pointer to core in thread local storage.
 *
 */
class ThreadLocalStorage : public Dali::BaseObject
{
public:
  /**
   * Constructor
   * Creates the TLS and adds a pointer to core
   * @param [in] core reference to core
   */
  ThreadLocalStorage(Core* core);

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
   * @copydoc Dali::SingletonService::Get()
   */
  static Dali::SingletonService GetSingletonService();

  /**
   * Checks if the TLS has been created
   * @return if the TLS has been created
   */
  static bool Created();

  /**
   * Checks if the system is shutting down
   * @return true if the system is shutting down
   */
  static bool IsShuttingDown();

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

  /**
   * Returns the Object registry.
   * @return A reference to the Object registry
   */
  ObjectRegistry& GetObjectRegistry();

  /**
   * @brief Gets the event thread services.
   * @return A reference to the event thread services
   */
  EventThreadServices& GetEventThreadServices();

  /**
   * @brief Gets the property notification manager.
   * @return A reference to the property notification manager
   */
  PropertyNotificationManager& GetPropertyNotificationManager();

  /**
   * @brief Gets the animation play list.
   * @return A reference to the animation play list
   */
  AnimationPlaylist& GetAnimationPlaylist();

  /**
   * @brief Returns whether the blend equation is supported in the system or not.
   * @param[in] blendEquation blend equation to be checked.
   * @return True if the blend equation supported.
   */
  bool IsBlendEquationSupported(DevelBlendEquation::Type blendEquation);

  /**
   * @brief Returns shader prefix of shading language version.
   */
  std::string GetShaderVersionPrefix();

  /**
   * @brief Returns vertex shader prefix including shading language version.
   */
  std::string GetVertexShaderPrefix();

  /**
   * @brief Returns fragment shader prefix including shading language version and extension information.
   */
  std::string GetFragmentShaderPrefix();

  /**
   * Add a Scene to the Core.
   * This is only used by the Scene to add itself to the core when the Scene is created.
   * @param[in] scene The Scene.
   */
  void AddScene(Scene* scene);

  /**
   * Remove a Scene from the Core.
   * This is only used by the Scene to remove itself from the core when the Scene is destroyed.
   * @param[in] scene The Scene.
   */
  void RemoveScene(Scene* scene);

  /**
   * @copydoc Dali::SingletonService::Register()
   */
  void Register(const std::type_info& info, BaseHandle singleton);

  /**
   * @copydoc Dali::SingletonService::UnregisterAll()
   */
  void UnregisterAll();

  /**
   * @copydoc Dali::SingletonService::GetSingleton()
   */
  BaseHandle GetSingleton(const std::type_info& info) const;

private:
  /**
   * Virtual Destructor
   */
  ~ThreadLocalStorage() override;

  // Undefined
  ThreadLocalStorage(const ThreadLocalStorage&);
  ThreadLocalStorage& operator=(ThreadLocalStorage&);

private:
  Core* mCore; ///< reference to core

  // using the address of the type name string as compiler will allocate these once per library
  // and we don't support un/re-loading of dali libraries while singleton service is alive
  using SingletonPair      = std::pair<const char*, BaseHandle>;
  using SingletonContainer = std::vector<SingletonPair>;
  using SingletonConstIter = SingletonContainer::const_iterator;

  SingletonContainer mSingletonContainer; ///< The container to look up singleton by its type name
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ThreadLocalStorage& GetImplementation(Dali::SingletonService& service)
{
  DALI_ASSERT_ALWAYS(service && "SingletonService handle is empty");

  BaseObject& handle = service.GetBaseObject();

  return static_cast<Internal::ThreadLocalStorage&>(handle);
}

inline const Internal::ThreadLocalStorage& GetImplementation(const Dali::SingletonService& service)
{
  DALI_ASSERT_ALWAYS(service && "SingletonService handle is empty");

  const BaseObject& handle = service.GetBaseObject();

  return static_cast<const Internal::ThreadLocalStorage&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_THREAD_LOCAL_STORAGE_H
