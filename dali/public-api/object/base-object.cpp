/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/type-registry-impl.h>

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
  Internal::Stage* stage = Internal::Stage::GetCurrent();
  if( stage )
  {
    stage->RegisterObject( this );
  }
}

void BaseObject::UnregisterObject()
{
  // Guard to allow handle destruction after Core has been destroyed
  Internal::Stage* stage = Internal::Stage::GetCurrent();
  if( stage )
  {
    stage->UnregisterObject( this );
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

const std::string& BaseObject::GetTypeName() const
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  if( registry )
  {
    Dali::TypeInfo typeInfo = registry->GetTypeInfo(this);
    if( typeInfo )
    {
      return typeInfo.GetName();
    }
  }

  // Return an empty string if type-name not found.
  DALI_LOG_WARNING( "TypeName Not Found\n" );
  return String::EMPTY;
}

bool BaseObject::GetTypeInfo(Dali::TypeInfo& typeInfo) const
{
  Dali::Internal::TypeRegistry* registry = Dali::Internal::TypeRegistry::Get();

  Dali::TypeInfo info = registry->GetTypeInfo(this);
  if(info)
  {
    typeInfo = info;
    return true;
  }
  else
  {
    return false;
  }
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

