#ifndef DALI_INTERNAL_SCENE_GRAPH_CONNECTION_CHANGE_PROPAGATOR_H
#define DALI_INTERNAL_SCENE_GRAPH_CONNECTION_CHANGE_PROPAGATOR_H

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

#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class PropertyOwner;

/**
 * This class maintains a list of observers that are informed when
 * it's owner changes it's connections.
 */
class ConnectionChangePropagator
{
public:
  class Observer
  {
  public:
    /**
     * Inform the observer of the object that it's connections have changed
     * @param[in] object The connection owner
     */
    virtual void ConnectionsChanged(PropertyOwner& object) = 0;

    /**
     * Inform the observer of the object that a connected object has
     * changed it's uniform map in some way.
     */
    virtual void ConnectedUniformMapChanged() = 0;

    /**
     * Inform the observer of the object that the object is about to be destroyed
     */
    virtual void ObservedObjectDestroyed(PropertyOwner& object){}
  };

  /**
   * Constructor
   */
  ConnectionChangePropagator();

  /**
   * Destructor
   */
  ~ConnectionChangePropagator();

  /**
   * Add an observer
   */
  void Add(Observer& observer);

  /**
   * Remove an observer
   */
  void Remove(Observer& observer);

  /**
   * Inform the observers that the connections to the object have changed
   * @param[in] object The connection owner
   */
  void ConnectionsChanged(PropertyOwner& object);

  /**
   * Inform the observers that the uniform map of this or a connected object
   * has changed.
   */
  void ConnectedUniformMapChanged();

  /**
   * Inform the observers that the object is about to be destroyed
   * @param[in] object The connection owner
   */
  void Destroy( PropertyOwner& object );

private:
  typedef Dali::Vector<Observer*> Observers;
  typedef Observers::Iterator ObserversIterator;

  Observers mObservers;
};


} // SceneGraph
} // Internal
} // Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_CONNECTION_CHANGE_PROPAGATOR_H
