//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/event/common/thread-local-storage.h>

// EXTERNAL INCLUDES
#include <boost/thread/tss.hpp>

// INTERNAL INCLUDES
#include <dali/internal/common/core-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Internal
{

namespace
{
boost::thread_specific_ptr<ThreadLocalStorage> threadLocal;
}

ThreadLocalStorage::ThreadLocalStorage(Core* core)
:mCore(core)
{
  DALI_ASSERT_ALWAYS( threadLocal.get() == NULL && "Cannot create more than one ThreadLocalStorage object" );

  // reset is used to store a new value associated with this thread
  threadLocal.reset(this);

}

ThreadLocalStorage::~ThreadLocalStorage()
{
}

void ThreadLocalStorage::Remove()
{
  threadLocal.reset();
}

ThreadLocalStorage& ThreadLocalStorage::Get()
{
  ThreadLocalStorage* tls = threadLocal.get();

  DALI_ASSERT_ALWAYS(tls);

  return *tls;
}

bool ThreadLocalStorage::Created()
{
  // see if the TLS has been set yet
  return (threadLocal.get() != NULL);
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

} // namespace Internal

} // namespace Dali
