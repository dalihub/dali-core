#ifndef __SCENE_GRAPH_DYNAMICS_JOINT_H__
#define __SCENE_GRAPH_DYNAMICS_JOINT_H__

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
#include <dali/internal/event/dynamics/dynamics-declarations.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>

class btGeneric6DofSpringConstraint;

namespace Dali
{

namespace Integration
{

class DynamicsJoint;

} // namespace Integration

namespace Internal
{

namespace SceneGraph
{

class DynamicsBody;
class DynamicsWorld;
class Node;

class DynamicsJoint
{
public:
  /**
   * Constructor.
   */
  DynamicsJoint(DynamicsWorld& world);

  /**
   * Destructor.
   */
  virtual ~DynamicsJoint();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsJoint(const DynamicsJoint&);
  DynamicsJoint& operator=(const DynamicsJoint&);

public:

  void Initialize( DynamicsBody* bodyA, DynamicsBody* bodyB, const Vector3& offsetA, const Vector3& offsetB );

  // @copydoc Dali::Internal::DynamicsJoint::SetLimit
  void SetLimit( int axisIndex, float lowerLimit, float upperLimit );

  /// @copydoc Dali::Internal::DynamicsJoint::EnableSpring
  void EnableSpring( int axisIndex, bool flag );

  /// @copydoc Dali::Internal::DynamicsJoint::SetSpringStiffness
  void SetSpringStiffness( int axisIndex, float stiffness );

  /// @copydoc Dali::Internal::DynamicsJoint::SetSpringDamping
  void SetSpringDamping( int axisIndex, float damping );

  /// @copydoc Dali::Internal::DynamicsJoint::SetSpringCenterPoint
  void SetSpringCenterPoint( int axisIndex, float ratio );

  /// @copydoc Dali::Internal::DynamicsJoint::EnableMotor
  void EnableMotor( int axisIndex, bool flag );

  /// @copydoc Dali::Internal::DynamicsJoint::SetMotorVelocity
  void SetMotorVelocity( int axisIndex, float velocity );

  /// @copydoc Dali::Internal::DynamicsJoint::SetMotorForce
  void SetMotorForce( int axisIndex, float force );

  void Connect();
  void Disconnect();
  void Delete();

  Integration::DynamicsJoint* GetJoint() const;

private:
  DynamicsWorld&  mWorld;
  Integration::DynamicsJoint* mJoint;
}; // class DynamicsJoint

// Messages for DynamicsJoint

inline void InitializeDynamicsJointMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, const DynamicsBody& bodyA, const DynamicsBody& bodyB, const Vector3& offsetA , const Vector3& offsetB )
{
  typedef MessageValue4< DynamicsJoint, DynamicsBody*, DynamicsBody*, Vector3, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::Initialize,
                        const_cast<DynamicsBody*>( &bodyA ),
                        const_cast<DynamicsBody*>( &bodyB ),
                        offsetA,
                        offsetB );
}

inline void SetLimitMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, float lowerLimit, float upperLimit )
{
  typedef MessageValue3< DynamicsJoint, int, float, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::SetLimit, axisIndex, lowerLimit, upperLimit );
}

inline void SetEnableSpringMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, bool flag )
{
  typedef MessageValue2< DynamicsJoint, int, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::EnableSpring, axisIndex, flag );
}

inline void SetSpringStiffnessMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, float stiffness )
{
  typedef MessageValue2< DynamicsJoint, int, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::SetSpringStiffness, axisIndex, stiffness );
}

inline void SetSpringDampingMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, float damping )
{
  typedef MessageValue2< DynamicsJoint, int, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::SetSpringDamping, axisIndex, damping );
}

inline void SetSpringCenterPointMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, float ratio )
{
  typedef MessageValue2< DynamicsJoint, int, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::SetSpringCenterPoint, axisIndex, ratio );
}

inline void SetEnableMotorMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, bool flag )
{
  typedef MessageValue2< DynamicsJoint, int, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::EnableMotor, axisIndex, flag );
}

inline void SetMotorVelocityMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, float velocity )
{
  typedef MessageValue2< DynamicsJoint, int, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::SetMotorVelocity, axisIndex, velocity );
}

inline void SetMotorForceMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint, int axisIndex, float force )
{
  typedef MessageValue2< DynamicsJoint, int, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::SetMotorForce, axisIndex, force );
}

inline void ConnectJointMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint )
{
  typedef Message< DynamicsJoint > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::Connect );
}

inline void DisconnectJointMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint )
{
  typedef Message< DynamicsJoint > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::Disconnect );
}

inline void DeleteJointMessage( EventToUpdate& eventToUpdate, const DynamicsJoint& joint )
{
  typedef Message< DynamicsJoint > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &joint, &DynamicsJoint::Delete );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __SCENE_GRAPH_DYNAMICS_JOINT_H__
