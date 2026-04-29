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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/common/base-object-impl.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/type-registry-impl.h>

namespace Dali
{
BaseObject::BaseObject()
: mImpl(new Impl(*this))
{
}

BaseObject::~BaseObject()
{
  delete mImpl;
}

void BaseObject::OnDestroy()
{
  DALI_ASSERT_ALWAYS(mImpl && "~BaseObject OnDestroy should not be called twice!");

  // Notify BaseObject::Impl::Observer destroyed first.
  delete mImpl;
  mImpl = nullptr;
}

void BaseObject::RegisterObject()
{
  Internal::ThreadLocalStorage* tls = Internal::ThreadLocalStorage::GetInternal();
  if(tls)
  {
    tls->GetEventThreadServices().RegisterObject(this);
  }
}

void BaseObject::UnregisterObject()
{
  Internal::ThreadLocalStorage* tls = Internal::ThreadLocalStorage::GetInternal();

  // Guard to allow handle destruction after Core has been destroyed
  if(tls)
  {
    tls->GetEventThreadServices().UnregisterObject(this);
  }
}

bool BaseObject::DoAction(const Dali::StringView& actionName, const Property::Map& attributes)
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  if(registry)
  {
    return registry->DoActionTo(this, Integration::ToStdString(actionName), attributes);
  }

  return false;
}

Dali::String BaseObject::GetTypeName() const
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  if(registry)
  {
    Internal::TypeRegistry::TypeInfoPointer typeInfo = registry->GetTypeInfo(this);
    if(typeInfo)
    {
      return typeInfo->GetName();
    }
  }

  // Return an empty string if type-name not found.
  DALI_LOG_ERROR("TypeName Not Found\n");
  return Dali::String();
}

bool BaseObject::GetTypeInfo(Dali::TypeInfo& typeInfo) const
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  Internal::TypeRegistry::TypeInfoPointer info = registry->GetTypeInfo(this);
  if(info)
  {
    typeInfo = Dali::TypeInfo(info.Get());
    return true;
  }
  else
  {
    return false;
  }
}

bool BaseObject::DoConnectSignal(ConnectionTrackerInterface* connectionTracker, const Dali::StringView& signalName, FunctorDelegate* functorDelegate)
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  if(registry)
  {
    return registry->ConnectSignal(this, connectionTracker, Integration::ToStdString(signalName), functorDelegate);
  }

  return false;
}

} // namespace Dali
