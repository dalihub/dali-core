#ifndef __SCENE_GRAPH_DYNAMICS_BODY_H__
#define __SCENE_GRAPH_DYNAMICS_BODY_H__

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
#include <dali/internal/common/message.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

class MeshData;
class Quaternion;

namespace Integration
{

class DynamicsBody;
struct DynamicsBodySettings;

} // namespace Integration

namespace Internal
{

namespace SceneGraph
{

class Node;
class DynamicsBody;
class DynamicsShape;
class DynamicsWorld;

/**
 * @copydoc Dali::DynamicsBody
 */
class DynamicsBody
{
public:
  /**
   * Constructor.
   * @param[in] world The object encapsulation the simulation world.
   */
  DynamicsBody(DynamicsWorld& world, Node& node);

  /**
   * Destructor.
   */
  virtual ~DynamicsBody();

  /**
   * Initialize the body
   * @param[in] settings Configuration parameters for the new body
   */
  void Initialize(Integration::DynamicsBodySettings* settings, DynamicsShape* shape);

  /**
   * Set the mass
   * @param[in] mass The new mass for the body
   */
  void SetMass( float mass );

  ///@ copydoc Dali::DynamicsBody::SetElasticity
  void SetElasticity( float elasticity );

  /// @copydoc Dali::DynamicsBody::SetLinearVelocity
  void SetLinearVelocity( const Vector3& velocity );

  /**
   * @copydoc Dali::DynamicsBody::SetLinearVelocity
   * @param[in] bufferIndex Double buffered data index
   */
  Vector3 GetLinearVelocity( BufferIndex bufferIndex ) const;

  /// @copydoc Dali::DynamicsBody::SetAngularVelocity
  void SetAngularVelocity( const Vector3& velocity );

  /**
   * @copydoc Dali::DynamicsBody::SetLinearVelocity
   * @param[in] bufferIndex Double buffered data index
   */
  Vector3 GetAngularVelocity( BufferIndex bufferIndex ) const;

  /// @copydoc Dali::DynamicsBody::SetKinematic
  void SetKinematic( bool flag );

  /// @copydoc Dali::DynamicsBody::IsKinematic
  bool IsKinematic() const;

  /// @copydoc Dali::DynamicsBody::SetSleepEnabled
  void SetSleepEnabled( bool flag );

  /// @copydoc Dali::DynamicsBody::WakeUp
  void WakeUp();

  /// @copydoc Dali::DynamicsBody::AddAnchor
  void AddAnchor( unsigned int index, const DynamicsBody* anchorBody, bool collisions );

  /// @copydoc Dali::DynamicsBody::ConserveVolume
  void ConserveVolume( bool flag );

  /// @copydoc Dali::DynamicsBody::ConserveShape
  void ConserveShape( bool flag );

  /// @copydoc Dali::DynamicsBody::GetCollisionGroup
  short int GetCollisionGroup() const;

  /// @copydoc Dali::DynamicsBody::SetCollisionGroup
  void SetCollisionGroup( short int collisionGroup );

  /// @copydoc Dali::DynamicsBody::GetCollisionMask
  short int GetCollisionMask() const;

  /// @copydoc Dali::DynamicsBody::SetCollisionMask
  void SetCollisionMask( short int collisionMask );

  int GetType() const;

  /**
   * Get a pointer to the physics body object.
   * @return A pointer to the physics body object.
   */
  Integration::DynamicsBody* GetBody() const;

  /**
   * Add and connect DynamicsBody to DynamicsWorld
   */
  void Connect();

  /**
   * Disconnect DynamicsBody from DynamicsWorld
   */
  void Disconnect();

  /**
   * Delete DynamicsBody from DynamicsWorld
   */
  void Delete();

  /**
   * TODO comment us
   */
  void SetMotionState();
  void GetNodePositionAndRotation(Vector3& position, Quaternion& rotation);
  void SetNodePositionAndRotation(const Vector3& position, const Quaternion& rotation);
  void RefreshDynamics();
  bool RefreshNode(BufferIndex updateBufferIndex);
  void RefreshMesh(BufferIndex updateBufferIndex);

private:
  // unimplemented copy constructor and assignment operator
  DynamicsBody(const DynamicsBody&);
  DynamicsBody& operator=(const DynamicsBody&);

public:
  Dali::Integration::DynamicsBody* mBody;

private:
  Node&          mNode;      // weak reference to the Node
  DynamicsWorld& mWorld;

  DoubleBuffered<Vector3> mLinearVelocity;
  DoubleBuffered<Vector3> mAngularVelocity;

}; // class DynamicsBody

// Messages for DynamicsBody

inline void InitializeDynamicsBodyMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, Integration::DynamicsBodySettings* settings, const DynamicsShape& shape )
{
  typedef MessageValue2< DynamicsBody, Integration::DynamicsBodySettings*, DynamicsShape* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::Initialize, settings, &const_cast<DynamicsShape&>(shape) );
}

inline void SetMassMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, float mass )
{
  typedef MessageValue1< DynamicsBody, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetMass, mass );
}

inline void SetElasticityMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, float elasticity )
{
  typedef MessageValue1< DynamicsBody, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetElasticity, elasticity );
}

inline void SetLinearVelocityMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, const Vector3& velocity )
{
  typedef MessageValue1< DynamicsBody, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetLinearVelocity, velocity );
}

inline void SetAngularVelocityMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, const Vector3& velocity )
{
  typedef MessageValue1< DynamicsBody, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetAngularVelocity, velocity );
}

inline void SetKinematicMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, bool flag )
{
  typedef MessageValue1< DynamicsBody, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetKinematic, flag );
}

inline void SetSleepEnabledMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, bool flag )
{
  typedef MessageValue1< DynamicsBody, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetSleepEnabled, flag );
}

inline void WakeUpMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body )
{
  typedef Message< DynamicsBody > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::WakeUp );
}

inline void AddAnchorMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, unsigned int index, const DynamicsBody& anchorBody, bool collisions )
{
  typedef MessageValue3< DynamicsBody, unsigned int, const DynamicsBody*, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::AddAnchor, index, &anchorBody, collisions );
}

inline void ConserveVolumeMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, bool flag )
{
  typedef MessageValue1< DynamicsBody, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::ConserveVolume, flag );
}

inline void ConserveShapeMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, bool flag )
{
  typedef MessageValue1< DynamicsBody, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::ConserveShape, flag );
}

inline void SetCollisionGroupMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, short int collisionGroup )
{
  typedef MessageValue1< DynamicsBody, short int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetCollisionGroup, collisionGroup );
}

inline void SetCollisionMaskMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body, short int collisionMask )
{
  typedef MessageValue1< DynamicsBody, short int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::SetCollisionMask, collisionMask );
}

inline void ConnectMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body )
{
  typedef Message< DynamicsBody > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::Connect );
}

inline void DisconnectMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body )
{
  typedef Message< DynamicsBody > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::Disconnect );
}

inline void DeleteBodyMessage( EventThreadServices& eventThreadServices, const DynamicsBody& body )
{
  typedef Message< DynamicsBody > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &body, &DynamicsBody::Delete );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_BODY_H__
