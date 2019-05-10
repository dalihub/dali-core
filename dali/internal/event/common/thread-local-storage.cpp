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

// CLASS HEADER
#include <dali/internal/event/common/thread-local-storage.h>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/event-thread-services.h>

namespace Dali
{

namespace Internal
{

namespace
{
thread_local ThreadLocalStorage* threadLocal = nullptr;
}

ThreadLocalStorage::ThreadLocalStorage(Core* core)
: mCore( core )
{
  DALI_ASSERT_ALWAYS( threadLocal == nullptr && "Cannot create more than one ThreadLocalStorage object" );

  threadLocal = this;
}

ThreadLocalStorage::~ThreadLocalStorage()
{
}

void ThreadLocalStorage::Remove()
{
  threadLocal = nullptr;
}

ThreadLocalStorage& ThreadLocalStorage::Get()
{
  DALI_ASSERT_ALWAYS(threadLocal);

  return *threadLocal;
}

bool ThreadLocalStorage::Created()
{
  // see if the TLS has been set yet
  return (threadLocal != nullptr);
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

void ThreadLocalStorage::AddScene( Scene* scene )
{
  mCore->AddScene( scene );
}

void ThreadLocalStorage::RemoveScene( Scene* scene )
{
  mCore->RemoveScene( scene );
}

} // namespace Internal

} // namespace Dali
