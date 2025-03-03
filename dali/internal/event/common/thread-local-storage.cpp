/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/event/common/thread-local-storage.h>

// INTERNAL INCLUDES
#include <dali/integration-api/processor-interface.h>
#include <dali/internal/common/core-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/public-api/common/dali-common.h>

#if defined(DEBUG_ENABLED)
#include <dali/integration-api/debug.h>
Debug::Filter* gSingletonServiceLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SINGLETON_SERVICE");

// Need to define own macro as the log function is not installed when this object is created so no logging is shown with DALI_LOG_INFO at construction and destruction
#define DALI_LOG_SINGLETON_SERVICE_DIRECT(level, message)                           \
  if(gSingletonServiceLogFilter && gSingletonServiceLogFilter->IsEnabledFor(level)) \
  {                                                                                 \
    std::string string(message);                                                    \
    Dali::TizenPlatform::LogMessage(Debug::INFO, string);                           \
  }

#define DALI_LOG_SINGLETON_SERVICE(level, format, ...) DALI_LOG_INFO(gSingletonServiceLogFilter, level, format, ##__VA_ARGS__)
#else

#define DALI_LOG_SINGLETON_SERVICE_DIRECT(level, message)
#define DALI_LOG_SINGLETON_SERVICE(level, format, ...)

#endif

namespace Dali
{
namespace Internal
{
namespace
{
thread_local ThreadLocalStorage* threadLocal    = nullptr;
thread_local bool                isShuttingDown = false;
} // namespace

ThreadLocalStorage::ThreadLocalStorage(Core* core)
: mCore(core),
  mSingletoneContainerChanging(false)
{
  DALI_ASSERT_ALWAYS(threadLocal == nullptr && "Cannot create more than one ThreadLocalStorage object");

  threadLocal    = this;
  isShuttingDown = false;
}

ThreadLocalStorage::~ThreadLocalStorage()
{
  if(DALI_LIKELY(!mSingletoneContainerChanging))
  {
    // Turn on this flag as true, and don't return to false agian.
    mSingletoneContainerChanging = true;
    mSingletonContainer.clear();
  }
}

void ThreadLocalStorage::Remove()
{
  threadLocal    = nullptr;
  isShuttingDown = true;
}

ThreadLocalStorage& ThreadLocalStorage::Get()
{
  DALI_ASSERT_ALWAYS(threadLocal);

  return *threadLocal;
}

Dali::SingletonService ThreadLocalStorage::GetSingletonService()
{
  Dali::SingletonService singletonService;
  if(threadLocal)
  {
    singletonService = Dali::SingletonService(threadLocal);
  }
  return singletonService;
}

bool ThreadLocalStorage::Created()
{
  // see if the TLS has been set yet
  return (threadLocal != nullptr);
}

bool ThreadLocalStorage::IsShuttingDown()
{
  return isShuttingDown;
}

ThreadLocalStorage* ThreadLocalStorage::GetInternal()
{
  return threadLocal;
}

Dali::Integration::PlatformAbstraction& ThreadLocalStorage::GetPlatformAbstraction()
{
  return mCore->GetPlatform();
}

SceneGraph::UpdateManager& ThreadLocalStorage::GetUpdateManager()
{
  return mCore->GetUpdateManager();
}

NotificationManager& ThreadLocalStorage::GetNotificationManager()
{
  return mCore->GetNotificationManager();
}

ShaderFactory& ThreadLocalStorage::GetShaderFactory()
{
  return mCore->GetShaderFactory();
}

StagePtr ThreadLocalStorage::GetCurrentStage()
{
  return mCore->GetCurrentStage();
}

GestureEventProcessor& ThreadLocalStorage::GetGestureEventProcessor()
{
  return mCore->GetGestureEventProcessor();
}

RelayoutController& ThreadLocalStorage::GetRelayoutController()
{
  return mCore->GetRelayoutController();
}

ObjectRegistry& ThreadLocalStorage::GetObjectRegistry()
{
  return mCore->GetObjectRegistry();
}

EventThreadServices& ThreadLocalStorage::GetEventThreadServices()
{
  return mCore->GetEventThreadServices();
}

PropertyNotificationManager& ThreadLocalStorage::GetPropertyNotificationManager()
{
  return mCore->GetPropertyNotificationManager();
}

AnimationPlaylist& ThreadLocalStorage::GetAnimationPlaylist()
{
  return mCore->GetAnimationPlaylist();
}

bool ThreadLocalStorage::IsBlendEquationSupported(DevelBlendEquation::Type blendEquation)
{
  return mCore->GetGraphicsConfig().IsBlendEquationSupported(blendEquation);
}

uint32_t ThreadLocalStorage::GetShaderLanguageVersion()
{
  return mCore->GetGraphicsConfig().GetShaderLanguageVersion();
}

std::string ThreadLocalStorage::GetShaderVersionPrefix()
{
  return mCore->GetGraphicsConfig().GetShaderVersionPrefix();
}

std::string ThreadLocalStorage::GetVertexShaderPrefix()
{
  return mCore->GetGraphicsConfig().GetVertexShaderPrefix();
}

std::string ThreadLocalStorage::GetFragmentShaderPrefix()
{
  return mCore->GetGraphicsConfig().GetFragmentShaderPrefix();
}

void ThreadLocalStorage::AddScene(Scene* scene)
{
  mCore->AddScene(scene);
}

void ThreadLocalStorage::RemoveScene(Scene* scene)
{
  mCore->RemoveScene(scene);
}

void ThreadLocalStorage::Register(const std::type_info& info, BaseHandle singleton)
{
  if(singleton)
  {
    if(DALI_LIKELY(!mSingletoneContainerChanging))
    {
      DALI_LOG_SINGLETON_SERVICE(Debug::General, "Singleton Added: %s\n", info.name());
      mSingletonContainer.push_back(SingletonPair(info.name(), singleton));
    }
  }
}

void ThreadLocalStorage::UnregisterAll()
{
  if(DALI_LIKELY(!mSingletoneContainerChanging))
  {
    mSingletoneContainerChanging = true;
    mSingletonContainer.clear();
    mSingletoneContainerChanging = false;
  }
}

BaseHandle ThreadLocalStorage::GetSingleton(const std::type_info& info) const
{
  BaseHandle object;

  if(DALI_LIKELY(!mSingletoneContainerChanging))
  {
    const SingletonContainer::const_iterator end = mSingletonContainer.end();
    for(SingletonContainer::const_iterator iter = mSingletonContainer.begin(); iter != end; ++iter)
    {
      // comparing the addresses as these are allocated statically per library
      if((*iter).first == info.name())
      {
        object = (*iter).second;
      }
    }
  }

  return object;
}

} // namespace Internal

} // namespace Dali
