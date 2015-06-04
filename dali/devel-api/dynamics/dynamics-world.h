#ifndef __DALI_DYNAMICS_WORLD_H__
#define __DALI_DYNAMICS_WORLD_H__

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

// EXTERNAL INCLUDES
#include <string>

// BASE CLASS INCLUDES
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali/devel-api/dynamics/dynamics-world-config.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
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
 * will override positions and orientations applied by animations and constrints.
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
 *
 * Signals
 * | %Signal Name              | Method                     |
 * |---------------------------|----------------------------|
 * | collision                 | @ref CollisionSignal()     |
 */
class DALI_IMPORT_API DynamicsWorld : public BaseHandle
{
public:

  // signals
  typedef Signal< void ( DynamicsWorld, const DynamicsCollision ) > CollisionSignalType; ///< Type of collision signal

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
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DynamicsWorld();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  DynamicsWorld(const DynamicsWorld& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  DynamicsWorld& operator=(const DynamicsWorld& rhs);

  /**
   * @brief Static / singleton creator and getter.
   * Initialise the Dynamics simulation and create a DynamicsWorld object.
   *
   * Only one instance of DynamicsWorld will be created, so calling this method multiple times
   * will return the same DynamicsWorld object.
   *
   * If an instance already exists, it is returned regardless of configuration being passed in.
   *
   * @param[in] configuration A DynamicsWorldConfig object describing the required capabilities of the dynamics world.
   * @return A handle to the world object of the dynamics simulation, or an empty handle if Dynamics is not capable
   *         of supporting a requirement in the configuration as it is not available on the platform.
   */
  static DynamicsWorld GetInstance( DynamicsWorldConfig configuration );

  /**
   * @brief Static / singleton getter.
   * Get a handle to the world object of the dynamics simulation.
   *
   * Does not create an instance, use only if the instance is known to exist.
   *
   * @return A pointer to the dynamics world if it is installed.
   */
  static DynamicsWorld Get();

  /**
   * @brief Static instance cleanup.
   * Terminate the dynamics simulation.
   *
   * Calls Actor::DisableDynamics on all dynamics enabled actors,
   * all handles to any DynamicsBody or DynamicsJoint objects held by applications
   * will become detached from their actors and the simulation therefore should be discarded.
   */
  static void DestroyInstance();

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
  CollisionSignalType& CollisionSignal();

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
