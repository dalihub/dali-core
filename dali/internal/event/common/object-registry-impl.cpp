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

// CLASS HEADER
#include <dali/internal/event/common/object-registry-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/object/object-registry.h>

using Dali::Integration::ToStdStringView;

namespace Dali
{
namespace Internal
{
namespace
{
// Signals

const char* const SIGNAL_OBJECT_CREATED   = "objectCreated";
const char* const SIGNAL_OBJECT_DESTROYED = "objectDestroyed";

TypeRegistration mType(typeid(Dali::ObjectRegistry), typeid(Dali::BaseHandle), nullptr);

SignalConnectorType signalConnector1(mType, Dali::String(SIGNAL_OBJECT_CREATED), &ObjectRegistry::DoConnectSignal);
SignalConnectorType signalConnector2(mType, Dali::String(SIGNAL_OBJECT_DESTROYED), &ObjectRegistry::DoConnectSignal);

} // namespace

ObjectRegistryPtr ObjectRegistry::New()
{
  return ObjectRegistryPtr(new ObjectRegistry());
}

ObjectRegistry::ObjectRegistry() = default;

ObjectRegistry::~ObjectRegistry() = default;

void ObjectRegistry::RegisterObject(Dali::BaseObject* object)
{
  if(!mObjectCreatedSignal.Empty())
  {
    Dali::BaseHandle handle(object);
    mObjectCreatedSignal.Emit(handle);
  }
}

void ObjectRegistry::UnregisterObject(Dali::BaseObject* object)
{
  mObjectDestroyedSignal.Emit(object);
}

bool ObjectRegistry::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor)
{
  bool            connected(true);
  ObjectRegistry* objectRegistry = static_cast<ObjectRegistry*>(object); // TypeRegistry guarantees that this is the correct type.

  std::string_view name = ToStdStringView(signalName);
  if(name == SIGNAL_OBJECT_CREATED)
  {
    objectRegistry->ObjectCreatedSignal().Connect(tracker, functor);
  }
  else if(name == SIGNAL_OBJECT_DESTROYED)
  {
    objectRegistry->ObjectDestroyedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

} // namespace Internal

} // namespace Dali
