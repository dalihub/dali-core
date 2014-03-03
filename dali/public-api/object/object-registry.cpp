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
#include <dali/public-api/object/object-registry.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-registry-impl.h>

namespace Dali
{

const char* const ObjectRegistry::SIGNAL_OBJECT_CREATED = "object-created";
const char* const ObjectRegistry::SIGNAL_OBJECT_DESTROYED = "object-destroyed";

ObjectRegistry::ObjectRegistry()
{
}

ObjectRegistry::~ObjectRegistry()
{
}

ObjectRegistry::ObjectCreatedSignalV2& ObjectRegistry::ObjectCreatedSignal()
{
  return GetImplementation(*this).ObjectCreatedSignal();
}

ObjectRegistry::ObjectDestroyedSignalV2& ObjectRegistry::ObjectDestroyedSignal()
{
  return GetImplementation(*this).ObjectDestroyedSignal();
}

ObjectRegistry::ObjectRegistry(Internal::ObjectRegistry* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
