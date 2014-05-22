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
#include <dali/internal/event/common/object-registry-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/object/object-registry.h>

namespace Dali
{

namespace Internal
{

ObjectRegistryPtr ObjectRegistry::New()
{
  return ObjectRegistryPtr(new ObjectRegistry());
}

ObjectRegistry::ObjectRegistry()
{
}

ObjectRegistry::~ObjectRegistry()
{
}

void ObjectRegistry::RegisterObject( Dali::BaseObject* object )
{
  // Assert than an object is only registered once
  DALI_ASSERT_DEBUG( mDebugRegistry.end() == mDebugRegistry.find( object ) );

#ifdef DEBUG_ENABLED
  // This allows us to assert that an object is only registered once (debug builds only)
  mDebugRegistry.insert( object );
#endif // DEBUG_ENABLED

  if ( !mObjectCreatedSignalV2.Empty() )
  {
    Dali::BaseHandle handle( object );
    mObjectCreatedSignalV2.Emit( handle );
  }
}

void ObjectRegistry::UnregisterObject( Dali::BaseObject* object )
{
#ifdef DEBUG_ENABLED
  // This allows us to assert that an object is only registered once (debug builds only)
  mDebugRegistry.erase( object );
#endif // DEBUG_ENABLED

  mObjectDestroyedSignalV2.Emit( object );
}

bool ObjectRegistry::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  ObjectRegistry* objectRegistry = dynamic_cast<ObjectRegistry*>(object);

  if(Dali::ObjectRegistry::SIGNAL_OBJECT_CREATED == signalName)
  {
    objectRegistry->ObjectCreatedSignal().Connect( tracker, functor );
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
