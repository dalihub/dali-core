#ifndef __DALI_DYNAMICS_WORLD_H__
#define __DALI_DYNAMICS_WORLD_H__

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

// EXTERNAL INCLUDES
#include <string>

// BASE CLASS INCLUDES
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class DynamicsWorld;
} // namespace Internal

class Actor;
class DynamicsBody;
class DynamicsCollision;
class DynamicsJoint;
class DynamicsShape;

/**
 * @brief DynamicsWorld gives the application developer an alternative method of moving and rotating.
 * actors in the DALi scene.
 *
 * Actors are represented by DynamicsBody objects in the dynamics simulation and are moved by
 * forces (eg gravity). Dynamics also allows collisions between objects to be
 * detected and responded to in signal handlers.
 *
 * DALi will update the physics simulation after animations and constraints, thus dynamics forces
 * will override positions and rotations applied by animations and constrints.
 *
 * Here is an example illustrating the basic steps to initialise the simulation and
 * add a rigid body.
 *
 * @code
 * // Initialize and get a handle to the Dali::DynamicsWorld
 * Dali::DynamicsWorldConfig worldConfig( Dali::DynamicsWorldConfig::New() );
 * Dali::DynamicsWorld dynamicsWorld( Dali::Stage::GetCurrent().InitializeDynamics( worldConfig ) );
 *
 * // Create an actor to represent the world and act as a parent to DynamicsBody instances
 * Dali::Actor dynamicsRootActor( Dali::Actor::New() );
 * dynamicsRootActor.SetParentOrigin( Dali::ParentOrigin::CENTER );
 * dynamicsWorld.SetRootActor( dynamicsRootActor );
 * Dali::Stage::GetCurrent().Add( dynamicsRootActor );
 *
 * // create an actor to represent a rigid body
 * Dali::Actor actor( Dali::Actor::New() );
 * actor.SetParentOrigin( Dali::ParentOrigin::CENTER );
 *
 * // Enable dynamics for the actor, creating a rigid body with default configuration
 * actor.EnableDynamics( Dali::DynamicsBodyConfig::New() );
 *
 * // Add the actor to the scene
 * dynamicsRootActor.Add( actor );
 * @endcode
 */
class DynamicsWorld : public BaseHandle
{
public:

  // Signal Names
  static const char* const SIGNAL_COLLISION; ///< name "collision"

  // signals
  typedef SignalV2< void (DynamicsWorld, const DynamicsCollision) > CollisionSignalV2; ///< Type of collision signal

public:
  /**
   * @brief Debug modes
   */
  enum  DEBUG_MODES
  {
    DEBUG_MODE_NONE               = 0,
    DEBUG_MODE_WIREFRAME          = (1<<0),
    DEBUG_MODE_FAST_WIREFRAME     = (1<<1),
    DEBUG_MODE_AABB               = (1<<2),
    DEBUG_MODE_CONTACT_POINTS     = (1<<3),
    DEBUG_MODE_NO_DEACTIVATION    = (1<<4),
    DEBUG_MODE_CONSTRAINTS        = (1<<5),
    DEBUG_MODE_CONSTRAINTS_LIMITS = (1<<6),
    DEBUG_MODES_NORMALS           = (1<<7),
  };

public:
  /**
   * @brief Create an uninitialized DynamicsWorld handle.
   */
  DynamicsWorld();

  /**
   * @brief Virtual destructor.
   */
  virtual ~DynamicsWorld();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

// Methods that modify the simulation
public:

  /**
   * @brief Set the gravity for the world.
   *
   * @param[in] gravity a Vector3 specifying the applicable gravity for the world.
   */
  void SetGravity( const Vector3& gravity );

  /**
   * @brief Get the gravity for the world.
   *
   * @return A Vector3 specifying the gravity that will be applied in the world.
   */
  const Vector3& GetGravity() const;

  /**
   * @brief Get the current debug drawmode.
   *
   * @return A combination of the flags in DEBUG_MODES or 0 if debug drawing is currently disabled.
   */
  int GetDebugDrawMode() const;

  /**
   * @brief Set the debug drawmode for the simulation.
   *
   * @param[in] mode A combination of the flags in DEBUG_MODES or 0 to disable debug drawing
   */
  void SetDebugDrawMode(int mode);

  /**
   * @brief Set the actor which will represent the dynamics world.
   *
   * All actors that will participate in the dynamics simulation must be direct children of this actor
   * @param[in] actor The root actor for the dynamics simulation
   */
  void SetRootActor(Actor actor);

  /**
   * @brief Get the root actor for the simulation.
   *
   * @return The root actor for the dynamics simulation
   */
  Actor GetRootActor() const;

public: // Signals

  /**
   * @brief This signal is emitted when a collision is detected between two DynamicsBodies.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCollisionHandler(Dali::DynamicsWorld world, const Dali::DynamicsCollision collisionData);
   * @endcode
   *
   * @return The signal to connect to.
   */
  CollisionSignalV2& CollisionSignal();

public: // Not intended for application developers

  /**
   * @brief This constructor is used internally by Dali.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL DynamicsWorld( Internal::DynamicsWorld* internal );
};

} // namespace Dali

#endif // __DALI_DYNAMICS_WORLD_H__
