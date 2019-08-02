#ifndef DALI_INTERNAL_ACTOR_OBSERVER_H
#define DALI_INTERNAL_ACTOR_OBSERVER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/signals/callback.h>
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
   * Constructor with a callback which is called when the observed actor is removed from the scene.
   *
   * The callback should have the following signature:
   * @code
   * void MyCallback( Actor* actor );
   * @endcode
   * Where actor is a pointer to the object that has been removed from the scene.
   *
   * @param[in]  callback  The callback to connect to.
   *
   * @note Ownership of callback is passed onto this class.
   */
  ActorObserver( CallbackBase* callback );

  /**
   * Non virtual destructor
   */
  ~ActorObserver();

  // Movable

  /**
   * Move constructor.
   *
   * @param[in] other The object to move the data from
   *
   * @note The other's actor is appropriately disconnected.
   * @note Ownership of callback is passed onto this class.
   */
  ActorObserver( ActorObserver&& other );

  /**
   * Move assignment operator.
   *
   * @param[in] other The object to move the data from
   *
   * @note The other's actor is appropriately disconnected.
   * @note Ownership of callback is passed onto this class.
   */
  ActorObserver& operator=( ActorObserver&& other );

  // Not copyable

  ActorObserver( const ActorObserver& ) = delete;   ///< Deleted copy constructor.
  ActorObserver& operator=( const ActorObserver& ) = delete;   ///< Deleted copy assignment operator.

  // Methods

  /**
   * Return the stored Actor pointer.
   * @return The Actor pointer.
   * @note Returns nullptr while the observed actor is not on the scene.
   */
  Actor* GetActor() const;

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

  /**
   * This will be called if an actor is added to the scene.
   * @param[in] object The object object.
   * @see Object::Observer::SceneObjectAdded()
   */
  virtual void SceneObjectAdded( Object& object );

  /**
   * This will be called when the actor is removed from the scene.
   * @param[in] object The object object.
   * @see Object::Observer::SceneObjectRemoved()
   */
  virtual void SceneObjectRemoved( Object& object );

  /**
   * This will be called when the actor is destroyed. We should clear the actor.
   * No need to stop observing as the object is being destroyed anyway.
   * @see Object::Observer::ObjectDestroyed()
   */
  virtual void ObjectDestroyed( Object& object );

private:
  Actor* mActor;                 ///< Raw pointer to an Actor.
  bool  mActorDisconnected;      ///< Indicates whether the actor has been disconnected from the scene
  CallbackBase* mRemoveCallback; ///< Callback to call when the observed actor is removed from the scene
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_ACTOR_OBSERVER_H

