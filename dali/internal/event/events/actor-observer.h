#ifndef __DALI_INTERNAL_ACTOR_OBSERVER_H__
#define __DALI_INTERNAL_ACTOR_OBSERVER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

namespace Internal
{

class Actor;

/**
 * Stores an actor pointer and connects/disconnects to any required signals appropriately when set/unset.
 */
struct ActorObserver : public Object::Observer
{
public:

  // Construction & Destruction

  /**
   * Constructor.
   */
  ActorObserver();

  /**
   * Non virtual destructor
   */
  ~ActorObserver();

  // Methods

  /**
   * Return the stored Actor pointer.
   * @return The Actor pointer.
   */
  Actor* GetActor();

  /**
   * Assignment operator.
   * This disconnects the required signals from the currently set actor and connects to the required
   * signals for the the actor specified (if set).
   */
  void SetActor( Actor* actor );

  /**
   * Resets the set actor and disconnects any connected signals.
   */
  void ResetActor();

private:

  // Undefined
  ActorObserver( const ActorObserver& );
  ActorObserver& operator=( const ActorObserver& );

private:

  /**
   * This will never get called as we do not observe objects that have not been added to the scene.
   * @param[in] object The object object.
   * @see Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded(Object& object) { }

  /**
   * This will be called when the actor is removed from the stage, we should clear and stop
   * observing it.
   * @param[in] object The object object.
   * @see Object::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved(Object& object);

  /**
   * This will be called when the actor is destroyed. We should clear the actor.
   * No need to stop observing as the object is being destroyed anyway.
   * @see Object::Observer::ObjectDestroyed()
   */
  virtual void ObjectDestroyed(Object& object);

private:
  Actor* mActor;              ///< Raw pointer to an Actor.
  bool  mActorDisconnected;   ///< Indicates whether the actor has been disconnected from the scene
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTOR_OBSERVER_H__

