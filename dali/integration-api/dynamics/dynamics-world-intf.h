#ifndef __DALI_INTEGRATION_DYNAMICS_WORLD_INTF_H__
#define __DALI_INTEGRATION_DYNAMICS_WORLD_INTF_H__

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
#include <dali/public-api/common/map-wrapper.h>
#include <dali/integration-api/dynamics/dynamics-debug-vertex.h>

namespace Dali
{

struct Vector3;

namespace Integration
{

struct DynamicsCollisionData;
class DynamicsBody;
class DynamicsJoint;
class DynamicsWorld;
struct DynamicsWorldSettings;
typedef std::map<void*, DynamicsCollisionData> CollisionDataContainer;

DynamicsWorld* InitializeDynamics( DynamicsWorldSettings* settings );

/**
 * @copydoc Dali::DynamicsWorld
 */
class DALI_IMPORT_API DynamicsWorld
{
public:
  /**
   * Destructor
   */
  virtual ~DynamicsWorld() {}

  virtual void Initialize( const DynamicsWorldSettings& worldSettings ) = 0;

  /**
   * Add a body to the simulation
   */
  virtual void AddBody(DynamicsBody* body) = 0;

  /**
   * Remove a body from the simulation
   */
  virtual void RemoveBody(DynamicsBody* body) = 0;

  /**
   * Add a joint to the simulation
   */
  virtual void AddJoint(DynamicsJoint* joint) = 0;

  /**
   * Remove body from the simulation
   */
  virtual void RemoveJoint(DynamicsJoint* joint) = 0;

  /// @copydoc Dali::DynamicsWorld::SetGravity
  virtual void SetGravity( const Vector3& gravity ) = 0;

  /// @copydoc Dali::DynamicsWorld::SetDebugDrawMode
  virtual void SetDebugDrawMode(int mode) = 0;

  /**
   * Allow dynamics engine to do its debug drawing
   */
  virtual const DynamicsDebugVertexContainer& DebugDraw() = 0;

  /**
   * Update the simulation
   * @param[in] elapsedSeconds The amount of time taht has elapsed since previous call
   */
  virtual void Update( float elapsedSeconds ) = 0;

  /**
   * Check for collisions between simulation objects
   * @param[out] contacts A container which will be filled with the current collision/contact data
   */
  virtual void CheckForCollisions( CollisionDataContainer& contacts ) = 0;

}; // class DynamicsWorld

} //namespace Integration

} //namespace Dali

#endif // __DALI_INTEGRATION_DYNAMICS_WORLD_INTF_H__
