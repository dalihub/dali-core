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

// CLASS HEADER
#include <dali/devel-api/common/singleton-service.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/thread-local-storage.h>

namespace Dali
{
SingletonService::SingletonService() = default;

SingletonService SingletonService::Get()
{
  return Internal::ThreadLocalStorage::GetSingletonService();
}

SingletonService::~SingletonService() = default;

void SingletonService::Register(const std::type_info& info, BaseHandle singleton)
{
  GetImplementation(*this).Register(info, singleton);
}

void SingletonService::UnregisterAll()
{
  GetImplementation(*this).UnregisterAll();
}

BaseHandle SingletonService::GetSingleton(const std::type_info& info) const
{
  return GetImplementation(*this).GetSingleton(info);
}

SingletonService::SingletonService(Internal::ThreadLocalStorage* tls)
: BaseHandle(tls)
{
}

} // namespace Dali
