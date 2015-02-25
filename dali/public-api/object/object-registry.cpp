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
#include <dali/public-api/object/object-registry.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-registry-impl.h>

namespace Dali
{

ObjectRegistry::ObjectRegistry()
{
}

ObjectRegistry::~ObjectRegistry()
{
}

ObjectRegistry::ObjectRegistry(const ObjectRegistry& copy)
: BaseHandle(copy)
{
}

ObjectRegistry& ObjectRegistry::operator=(const ObjectRegistry& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

ObjectRegistry::ObjectCreatedSignalType& ObjectRegistry::ObjectCreatedSignal()
{
  return GetImplementation(*this).ObjectCreatedSignal();
}

ObjectRegistry::ObjectDestroyedSignalType& ObjectRegistry::ObjectDestroyedSignal()
{
  return GetImplementation(*this).ObjectDestroyedSignal();
}

ObjectRegistry::ObjectRegistry(Internal::ObjectRegistry* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
