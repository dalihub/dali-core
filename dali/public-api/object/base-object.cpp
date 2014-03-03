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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/type-registry-impl.h>

using namespace std;

namespace Dali
{

BaseObject::BaseObject()
{
}

BaseObject::~BaseObject()
{
}

void BaseObject::RegisterObject()
{
  if( Internal::Stage::IsInstalled() )
  {
    Internal::Stage::GetCurrent()->GetObjectRegistry().RegisterObject( this );
  }
}

void BaseObject::UnregisterObject()
{
  // Guard to allow handle destruction after Core has been destroyed
  if( Internal::Stage::IsInstalled() )
  {
    Internal::Stage::GetCurrent()->GetObjectRegistry().UnregisterObject( this );
  }
}

bool BaseObject::DoAction(const std::string& actionName, const std::vector<Property::Value>& attributes)
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  if( registry )
  {
    return registry->DoActionTo(this, actionName, attributes);
  }

  return false;
}

std::string BaseObject::GetTypeName()
{
  std::string name;

  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  if( registry )
  {
    Dali::TypeInfo typeInfo = registry->GetTypeInfo(this);
    if( typeInfo )
    {
      name = typeInfo.GetName();
    }
  }

  return name;
}

bool BaseObject::DoConnectSignal( ConnectionTrackerInterface* connectionTracker, const std::string& signalName, FunctorDelegate* functor )
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  if( registry )
  {
    return registry->ConnectSignal( this, connectionTracker, signalName, functor );
  }

  return false;
}

} // namespace Dali

