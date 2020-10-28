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
#include <typeinfo>

// CLASS HEADER
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{
BaseHandle::BaseHandle(Dali::BaseObject* handle)
: mObjectHandle(handle)
{
}

BaseHandle::BaseHandle() = default;

BaseHandle::~BaseHandle() = default;

BaseHandle::BaseHandle(const BaseHandle& handle) = default;

BaseHandle& BaseHandle::operator=(const BaseHandle& rhs) = default;

BaseHandle::BaseHandle(BaseHandle&& rhs) = default;

BaseHandle& BaseHandle::operator=(BaseHandle&& rhs) = default;

bool BaseHandle::DoAction(const std::string& command, const Property::Map& attributes)
{
  return GetImplementation(*this).DoAction(command, attributes);
}

const std::string& BaseHandle::GetTypeName() const
{
  return GetImplementation(*this).GetTypeName();
}

bool BaseHandle::GetTypeInfo(Dali::TypeInfo& typeInfo) const
{
  return GetImplementation(*this).GetTypeInfo(typeInfo);
}

BaseObject& BaseHandle::GetBaseObject()
{
  return static_cast<BaseObject&>(*mObjectHandle);
}

const BaseObject& BaseHandle::GetBaseObject() const
{
  return static_cast<const BaseObject&>(*mObjectHandle);
}

void BaseHandle::Reset()
{
  mObjectHandle = nullptr;
}

BaseHandle::operator BaseHandle::BooleanType() const
{
  return mObjectHandle ? &BaseHandle::ThisIsSaferThanReturningVoidStar : nullptr;
}

bool BaseHandle::operator==(const BaseHandle& rhs) const
{
  return this->mObjectHandle == rhs.mObjectHandle;
}

bool BaseHandle::operator!=(const BaseHandle& rhs) const
{
  return this->mObjectHandle != rhs.mObjectHandle;
}

Dali::RefObject* BaseHandle::GetObjectPtr() const
{
  return mObjectHandle.Get();
}

bool BaseHandle::DoConnectSignal(ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor)
{
  return GetImplementation(*this).DoConnectSignal(connectionTracker, signalName, functor);
}

} // namespace Dali
