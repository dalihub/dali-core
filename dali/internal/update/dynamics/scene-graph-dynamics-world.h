#ifndef __SCENE_GRAPH_DYNAMICS_WORLD_H__
#define __SCENE_GRAPH_DYNAMICS_WORLD_H__

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

// INTERNAL HEADERS
#include <dali/public-api/common/map-wrapper.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-thread-services.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

class Matrix;

namespace Integration
{

struct DynamicsCollisionData;
class  DynamicsFactory;
class  DynamicsWorld;
struct DynamicsWorldSettings;

typedef std::map<void*, DynamicsCollisionData> CollisionDataContainer;

} // namespace Integration

namespace Internal
{

class NotificationManager;
class DynamicsWorldConfig;

namespace SceneGraph
{

class Node;
class Shader;
struct DynamicsCollisionData;
class DynamicsBody;
class DynamicsJoint;
class DynamicsShape;
class SceneController;

class DynamicsWorld
{
public:

  /**
   * Constructor.
   * @param[in] dynamicsNotifier    Notification object for dynamics events
   * @param[in] notificationManager The notification manager
   * @param[in] dynamicsFactory     A reference to the factory object which creates instances of dynamics objects
   */
  DynamicsWorld(DynamicsNotifier& dynamicsNotifier,
                NotificationManager& notificationManager,
                Integration::DynamicsFactory& dynamicsFactory );

  /**
   * Destructor.
   */
  virtual ~DynamicsWorld();

  /**
   * Initialize a dynamics world
   * @param[in] sceneController Allows access to the render message queue
   * @param[in] worldSettings   The configuration for the new DynamicsWorld
   */
  void Initialize(SceneController* sceneController, Integration::DynamicsWorldSettings* worldSettings, const SceneGraphBuffers* buffers);

  void AddBody(DynamicsBody& body);
  void RemoveBody(DynamicsBody& body);
  void DeleteBody(DynamicsBody& body);

  void AddJoint(DynamicsJoint& joint);
  void RemoveJoint(DynamicsJoint& joint);
  void DeleteJoint(DynamicsJoint& joint);

  void AddShape(DynamicsShape& shape);
  void DeleteShape(DynamicsShape& shape);

  /// @copydoc Dali::DynamicsWorld::SetGravity
  void SetGravity( const Vector3& gravity );

  /**
   * Step the simulation and check for collisions
   * @param[in] elapsedSeconds Time in seconds since last invocation
   * @return true if any body was translated or rotated
   */
  bool Update( float elapsedSeconds );

  /**
   * Update the simulation positions from the corresponding node position
   */
  void PreSimulationStep();

  /**
   * Update the node position from the corresponding simulation position
   * @return true if any body was translated or rotated
   */
  bool PostSimulationStep();

  /**
   * Check for collisions between simulation objects
   */
  void CheckForCollisions();

  /**
   * Set the root node for the simulation
   * @param[in] node The root node
   */
  void SetNode(Node* node);

  /**
   * Set the world scale
   * @param[in] scale The world scale
   */
  void SetWorldScale( const float scale );

  /**
   * Get the world scale
   * @return The world scale
   */
  float GetWorldScale() const;

  /**
   * Get the current update buffer index
   * @return The index to the current update buffer for double buffer values
   */
  BufferIndex GetBufferIndex() const;

  /**
   * Get the dynamics factory object
   * @return The dynamics factory object
   */
  Integration::DynamicsFactory& GetDynamicsFactory();

  /**
   * Get the dynamics world object
   * @return The dynamics world object
   */
  Integration::DynamicsWorld& GetDynamicsWorld();

private:

  // unimplemented copy constructor and assignment operator
  DynamicsWorld(const DynamicsWorld&);
  DynamicsWorld& operator=(const DynamicsWorld&);

private:

  typedef OwnerContainer< DynamicsBody* >  DynamicsBodyContainer;
  typedef OwnerContainer< DynamicsJoint* > DynamicsJointContainer;
  typedef OwnerContainer< DynamicsShape* > DynamicsShapeContainer;

  DynamicsBodyContainer     mBodies;          // container of active DynamicsBodies
  DynamicsBodyContainer     mInactiveBodies;  // container of discarded DynamicsBodies
  DynamicsBodyContainer     mDiscardedBodies; // container of discarded DynamicsBodies

  DynamicsJointContainer    mJoints;          // container of active DynamicsJoints
  DynamicsJointContainer    mInactiveJoints;  // container of inactive DynamicsJoints
  DynamicsJointContainer    mDiscardedJoints; // container of discarded DynamicsJoints

  DynamicsShapeContainer    mShapes;          // container of DynamicsShapes
  DynamicsShapeContainer    mDiscardedShapes; // container of discarded DynamicsShapes

  DynamicsNotifier&         mDynamicsNotifier;
  NotificationManager&      mNotificationManager;

  Node*                     mNode;
  SceneController*          mSceneController;
  const SceneGraphBuffers*  mBuffers;

  Integration::DynamicsFactory&       mDynamicsFactory;
  Integration::DynamicsWorld*         mDynamicsWorld;
  Integration::DynamicsWorldSettings* mSettings;
  Integration::CollisionDataContainer mContacts;

};

// Messages for DynamicsWorld

inline void SetGravityMessage( EventThreadServices& eventThreadServices, const DynamicsWorld& world, const Vector3& gravity )
{
  typedef MessageValue1< DynamicsWorld, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &world, &DynamicsWorld::SetGravity, gravity );
}

inline void SetRootActorMessage( EventThreadServices& eventThreadServices, const DynamicsWorld& world, const Node* node )
{
  typedef MessageValue1< DynamicsWorld, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &world, &DynamicsWorld::SetNode, const_cast<Node*>(node) );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_WORLD_H__
