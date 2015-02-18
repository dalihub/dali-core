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

// CLASS HEADER
#include <dali/internal/event/common/thread-local-storage.h>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

namespace
{
__thread ThreadLocalStorage* threadLocal = NULL;
}

ThreadLocalStorage::ThreadLocalStorage(Core* core)
: mCore(core)
{
  DALI_ASSERT_ALWAYS( threadLocal == NULL && "Cannot create more than one ThreadLocalStorage object" );

  // reset is used to store a new value associated with this thread
  threadLocal = this;
}

ThreadLocalStorage::~ThreadLocalStorage()
{
}

void ThreadLocalStorage::Remove()
{
  threadLocal = NULL;
}

ThreadLocalStorage& ThreadLocalStorage::Get()
{
  DALI_ASSERT_ALWAYS(threadLocal);

  return *threadLocal;
}

bool ThreadLocalStorage::Created()
{
  // see if the TLS has been set yet
  return (threadLocal != NULL);
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

ResourceManager& ThreadLocalStorage::GetResourceManager()
{
  return mCore->GetResourceManager();
}

ResourceClient& ThreadLocalStorage::GetResourceClient()
{
  return mCore->GetResourceClient();
}

ImageFactory& ThreadLocalStorage::GetImageFactory()
{
  return mCore->GetImageFactory();
}

ModelFactory& ThreadLocalStorage::GetModelFactory()
{
  return mCore->GetModelFactory();
}

FontFactory& ThreadLocalStorage::GetFontFactory()
{
  return mCore->GetFontFactory();
}

ShaderFactory& ThreadLocalStorage::GetShaderFactory()
{
  return mCore->GetShaderFactory();
}

StagePtr ThreadLocalStorage::GetCurrentStage()
{
  return mCore->GetCurrentStage();
}

EventToUpdate& ThreadLocalStorage::GetEventToUpdate()
{
  return GetUpdateManager().GetEventToUpdate();
}

GestureEventProcessor& ThreadLocalStorage::GetGestureEventProcessor()
{
  return mCore->GetGestureEventProcessor();
}

EmojiFactory& ThreadLocalStorage::GetEmojiFactory()
{
  return mCore->GetEmojiFactory();
}

} // namespace Internal

} // namespace Dali
