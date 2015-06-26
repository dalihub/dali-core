#ifndef __TEST_DYNAMICS_H__
#define __TEST_DYNAMICS_H__

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

#include <dali/integration-api/dynamics/dynamics-body-intf.h>
#include <dali/integration-api/dynamics/dynamics-body-settings.h>
#include <dali/integration-api/dynamics/dynamics-joint-intf.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/integration-api/dynamics/dynamics-world-intf.h>
#include <dali/integration-api/dynamics/dynamics-world-settings.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>

#include <test-trace-call-stack.h>

namespace Dali
{

class TestDynamicsJoint : public Integration::DynamicsJoint
{
public:
  TestDynamicsJoint( TraceCallStack& trace );
  virtual ~TestDynamicsJoint();

public: // From Dali::Integration::DynamicsJoint

  void Initialize( Integration::DynamicsBody* bodyA, const Vector3& positionA, const Quaternion& rotationA, const Vector3& offsetA,
                   Integration::DynamicsBody* bodyB, const Vector3& positionB, const Quaternion& rotationB, const Vector3& offsetB );
  void SetLimit( const int axisIndex, const float lowerLimit, const float upperLimit );
  void EnableSpring( int axisIndex, bool flag );
  void SetSpringStiffness( int axisIndex, float stiffness );
  void SetSpringDamping( int axisIndex, float damping );
  void SetSpringCenterPoint( int axisIndex, float ratio );
  void EnableMotor( int axisIndex, bool flag );
  void SetMotorVelocity( int axisIndex, float velocity );
  void SetMotorForce( int axisIndex, float force );

private:

  TraceCallStack& mTrace;

}; // class TestDynamicsJoint

class TestDynamicsShape : public Integration::DynamicsShape
{
public:

  TestDynamicsShape( TraceCallStack& trace );
  virtual ~TestDynamicsShape();

public:
  void Initialize( int type, const Vector3& dimensions );
  void Initialize( int type, Geometry geometry );

private:

  TraceCallStack& mTrace;

}; // class TestDynamicsShape

class TestDynamicsBody : public Integration::DynamicsBody
{
public:
  TestDynamicsBody( TraceCallStack& trace );

  virtual ~TestDynamicsBody();

public: // From Dali::Integration::DynamicsBody

  inline void Initialize( const Integration::DynamicsBodySettings& bodySettings, Integration::DynamicsShape* shape,
                   Integration::DynamicsWorld& dynamicsWorld,
                   const Vector3& startPosition, const Quaternion& startRotation )
  {
    mTrace.PushCall( "DynamicsBody::~DynamicsBody", "" );
  }

  inline void SetMass( float mass )
  {
    mTrace.PushCall( "DynamicsBody::SetMass", "" );
  }

  inline void SetElasticity( float elasticity )
  {
    mTrace.PushCall( "DynamicsBody::SetElasticity", "" );
  }

  inline void SetLinearVelocity( const Vector3& velocity )
  {
    mTrace.PushCall( "DynamicsBody::SetLinearVelocity", "" );
  }

  inline Vector3 GetLinearVelocity() const
  {
    mTrace.PushCall( "DynamicsBody::GetLinearVelocity", "" );
    return Vector3::ZERO;
  }

  inline void SetAngularVelocity( const Vector3& velocity )
  {
    mTrace.PushCall( "DynamicsBody::SetAngularVelocity", "" );
  }

  inline Vector3 GetAngularVelocity() const
  {
    mTrace.PushCall( "DynamicsBody::GetAngularVelocity", "" );
    return Vector3::ZERO;
  }

  inline void SetKinematic( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::SetKinematic", "" );
  }

  inline bool IsKinematic() const
  {
    mTrace.PushCall( "DynamicsBody::IsKinematic", "" );
    return mKinematic;
  }

  inline void SetSleepEnabled( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::SetSleepEnabled", "" );
  }

  inline void WakeUp()
  {
    mTrace.PushCall( "DynamicsBody::WakeUp", "" );
  }

  inline void AddAnchor( unsigned int index, const Integration::DynamicsBody* anchorBody, bool collisions )
  {
    mTrace.PushCall( "DynamicsBody::AddAnchor", "" );
  }

  inline void ConserveVolume( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::ConserveVolume", "" );
  }

  inline void ConserveShape( bool flag )
  {
    mTrace.PushCall( "DynamicsBody::ConserveShape", "" );
  }

  inline short int GetCollisionGroup() const
  {
    mTrace.PushCall( "DynamicsBody::GetCollisionGroup", "" );
    return mCollisionGroup;
  }

  inline void SetCollisionGroup( short int collisionGroup )
  {
    mTrace.PushCall( "DynamicsBody::SetCollisionGroup", "" );
    mCollisionGroup = collisionGroup;
  }

  inline short int GetCollisionMask() const
  {
    mTrace.PushCall( "DynamicsBody::GetCollisionMask", "" );
    return mCollisionMask;
  }

  inline void SetCollisionMask( short int collisionMask )
  {
    mTrace.PushCall( "DynamicsBody::SetCollisionMask", "" );
    mCollisionMask = collisionMask;
  }

  inline int GetType() const
  {
    mTrace.PushCall( "DynamicsBody::GetType", "" );
    return mSettings->type;
  }

  inline bool IsActive() const
  {
    mTrace.PushCall( "DynamicsBody::IsActive", "" );
    return mActivationState;
  }

  inline void SetTransform( const Vector3& position, const Quaternion& rotation )
  {
    mTrace.PushCall( "DynamicsBody::SetTransform", "" );
  }

  inline void GetTransform( Vector3& position, Quaternion& rotation )
  {
    mTrace.PushCall( "DynamicsBody::GetTransform", "" );
  }

  inline void GetSoftVertices( Geometry geometry ) const
  {
    mTrace.PushCall( "DynamicsBody::GetSoftVertices", "" );
  }

private:
  Dali::Integration::DynamicsBodySettings* mSettings;
  bool mConserveVolume:1;
  bool mConserveShape:1;
  bool mKinematic:1;
  bool mActivationState:1;

  short int mCollisionGroup;
  short int mCollisionMask;

  TraceCallStack& mTrace;
}; // class TestDynamicsBody

class TestDynamicsWorld : public Dali::Integration::DynamicsWorld
{
public:

  TestDynamicsWorld( TraceCallStack& trace );

  virtual ~TestDynamicsWorld();

  inline void Initialize( const Dali::Integration::DynamicsWorldSettings& worldSettings )
  {

    mTrace.PushCall( "DynamicsWorld::Initialize", "" );
  }

  inline void AddBody( Dali::Integration::DynamicsBody* body )
  {
    mTrace.PushCall( "DynamicsWorld::AddBody", "" );
  }

  inline void RemoveBody( Dali::Integration::DynamicsBody* body )
  {
    mTrace.PushCall( "DynamicsWorld::RemoveBody", "" );
  }

  inline void AddJoint( Dali::Integration::DynamicsJoint* joint )
  {
    mTrace.PushCall( "DynamicsWorld::AddJoint", "" );
  }

  inline void RemoveJoint( Dali::Integration::DynamicsJoint* joint )
  {
    mTrace.PushCall( "DynamicsWorld::RemoveJoint", "" );
  }

  inline void SetGravity( const Vector3& gravity )
  {
    mTrace.PushCall( "DynamicsWorld::SetGravity", "" );
  }

  inline void SetDebugDrawMode(int mode)
  {
    mTrace.PushCall( "DynamicsWorld::SetDebugDrawMode", "" );
  }

  inline const Integration::DynamicsDebugVertexContainer& DebugDraw()
  {
    mTrace.PushCall( "DynamicsWorld::DebugDraw", "" );
    return mDebugVertices;
  }

  inline void Update( float elapsedSeconds )
  {
    mTrace.PushCall( "DynamicsWorld::Update", "" );
  }

  inline void CheckForCollisions( Integration::CollisionDataContainer& contacts )
  {
    mTrace.PushCall( "DynamicsWorld::CheckForCollisions", "" );
  }

private:
  Dali::Integration::DynamicsWorldSettings* mSettings;
  Dali::Integration::DynamicsDebugVertexContainer mDebugVertices;

  TraceCallStack& mTrace;

}; // class TestDynamicsWorld

class TestDynamicsFactory : public Integration::DynamicsFactory
{

public:

  TestDynamicsFactory( TraceCallStack& trace );

  virtual ~TestDynamicsFactory();

  virtual bool InitializeDynamics( const Integration::DynamicsWorldSettings& worldSettings );

  virtual void TerminateDynamics();

  virtual Integration::DynamicsWorld* CreateDynamicsWorld();
  virtual Integration::DynamicsBody* CreateDynamicsBody();

  virtual Integration::DynamicsJoint* CreateDynamicsJoint();

  virtual Integration::DynamicsShape* CreateDynamicsShape();

  TraceCallStack& mTrace;
}; // class TestDynamicsFactory

} // Dali

#endif // __TEST_DYNAMICS_H__
