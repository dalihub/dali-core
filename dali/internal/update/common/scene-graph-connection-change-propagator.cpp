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
 */

#include "scene-graph-connection-change-propagator.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

ConnectionChangePropagator::ConnectionChangePropagator()
{
}

ConnectionChangePropagator::~ConnectionChangePropagator()
{
}

void ConnectionChangePropagator::Add(Observer& observer)
{
  bool foundObserver = false;
  for( ObserversIterator iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    if( *iter == &observer )
    {
      foundObserver = true;
      break;
    }
  }
  if( !foundObserver )
  {
    mObservers.PushBack( &observer );
  }
}

void ConnectionChangePropagator::Remove(Observer& observer)
{
  for( ObserversIterator iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    if( *iter == &observer )
    {
      mObservers.Erase(iter);
      return;
    }
  }
}

void ConnectionChangePropagator::ConnectionsChanged(PropertyOwner& object)
{
  // Inform observers that the object's children have changed
  for( ObserversIterator iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    Observer* observer = (*iter);
    observer->ConnectionsChanged( object );
  }
}

void ConnectionChangePropagator::ConnectedUniformMapChanged()
{
  // Inform observers that the object's uniform map has changed
  for( ObserversIterator iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    Observer* observer = (*iter);
    observer->ConnectedUniformMapChanged();
  }
}

void ConnectionChangePropagator::Destroy( PropertyOwner& object )
{
  // Inform observers that the object's children have changed
  for( ObserversIterator iter = mObservers.Begin(); iter != mObservers.End(); ++iter )
  {
    Observer* observer = (*iter);
    observer->ObservedObjectDestroyed( object );
  }
}

} // SceneGraph
} // Internal
} // Dali
