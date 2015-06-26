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

#include "test-dynamics.h"

using namespace Dali;

namespace Dali
{


TestDynamicsJoint::TestDynamicsJoint( TraceCallStack& trace )
: mTrace( trace )
{
  mTrace.PushCall( "DynamicsJoint::DynamicsJoint", "" );
}

TestDynamicsJoint::~TestDynamicsJoint()
{
  mTrace.PushCall( "DynamicsJoint::~DynamicsJoint", "" );
}

void TestDynamicsJoint::Initialize(
  Integration::DynamicsBody* bodyA, const Vector3& positionA, const Quaternion& rotationA, const Vector3& offsetA,
  Integration::DynamicsBody* bodyB, const Vector3& positionB, const Quaternion& rotationB, const Vector3& offsetB )
{
  mTrace.PushCall( "DynamicsJoint::Initialize", "" );
}

void TestDynamicsJoint::SetLimit( const int axisIndex, const float lowerLimit, const float upperLimit )
{
  mTrace.PushCall( "DynamicsJoint::SetLimit", "" );
}

void TestDynamicsJoint::EnableSpring( int axisIndex, bool flag )
{
  mTrace.PushCall( "DynamicsJoint::EnableSpring", "" );
}

void TestDynamicsJoint::SetSpringStiffness( int axisIndex, float stiffness )
{
  mTrace.PushCall( "DynamicsJoint::SetSpringStiffness", "" );
}

void TestDynamicsJoint::SetSpringDamping( int axisIndex, float damping )
{
  mTrace.PushCall( "DynamicsJoint::SetSpringDamping", "" );
}

void TestDynamicsJoint::SetSpringCenterPoint( int axisIndex, float ratio )
{
  mTrace.PushCall( "DynamicsJoint::SetSpringCenterPoint", "" );
}

void TestDynamicsJoint::EnableMotor( int axisIndex, bool flag )
{
  mTrace.PushCall( "DynamicsJoint::EnableMotor", "" );
}

void TestDynamicsJoint::SetMotorVelocity( int axisIndex, float velocity )
{
  mTrace.PushCall( "DynamicsJoint::SetMotorVelocity", "" );
}

void TestDynamicsJoint::SetMotorForce( int axisIndex, float force )
{
  mTrace.PushCall( "DynamicsJoint::SetMotorForce", "" );
}


TestDynamicsShape::TestDynamicsShape( TraceCallStack& trace )
: mTrace( trace )
{
  mTrace.PushCall( "DynamicsShape::DynamicsShape", "" );
}

TestDynamicsShape::~TestDynamicsShape()
{
  mTrace.PushCall( "DynamicsShape::~DynamicsShape", "" );
}

void TestDynamicsShape::Initialize( int type, const Vector3& dimensions )
{
  mTrace.PushCall( "DynamicsShape::Initialize", "" );
}

void TestDynamicsShape::Initialize( int type, Geometry geometry )
{
  mTrace.PushCall( "DynamicsShape::Initialize", "" );
}

TestDynamicsBody::TestDynamicsBody( TraceCallStack& trace )
: mSettings( NULL ),
  mConserveVolume( false ),
  mConserveShape( false ),
  mKinematic( false ),
  mActivationState( true ),
  mCollisionGroup( 0 ),
  mCollisionMask( 0 ),
  mTrace( trace )
{
  mTrace.PushCall( "DynamicsBody::DynamicsBody", "" );
}

TestDynamicsBody::~TestDynamicsBody()
{
  mTrace.PushCall( "DynamicsBody::~DynamicsBody", "" );
}

TestDynamicsWorld::TestDynamicsWorld( TraceCallStack& trace )
: mSettings( NULL ),
  mTrace( trace )
{
  mTrace.PushCall( "DynamicsWorld::DynamicsWorld", "" );
}

TestDynamicsWorld::~TestDynamicsWorld()
{
  mTrace.PushCall( "DynamicsWorld::~DynamicsWorld", "" );
}


TestDynamicsFactory::TestDynamicsFactory( TraceCallStack& trace ) : mTrace( trace )
{
}

TestDynamicsFactory::~TestDynamicsFactory()
{
}

bool TestDynamicsFactory::InitializeDynamics( const Integration::DynamicsWorldSettings& worldSettings )
{
  mTrace.PushCall( "DynamicsFactory::InitializeDynamics", "" );
  return true;
}

void TestDynamicsFactory::TerminateDynamics()
{
  mTrace.PushCall( "DynamicsFactory::InitializeDynamics", "" );
}

Integration::DynamicsWorld* TestDynamicsFactory::CreateDynamicsWorld()
{
  mTrace.PushCall( "DynamicsFactory::CreateDynamicsWorld", "" );
  return new TestDynamicsWorld( mTrace );
}

Integration::DynamicsBody* TestDynamicsFactory::CreateDynamicsBody()
{
  mTrace.PushCall( "DynamicsFactory::CreateDynamicsBody", "" );
  return new TestDynamicsBody( mTrace );
}

Integration::DynamicsJoint* TestDynamicsFactory::CreateDynamicsJoint()
{
  mTrace.PushCall( "DynamicsFactory::CreateDynamicsJoint", "" );
  return new TestDynamicsJoint( mTrace );
}

Integration::DynamicsShape* TestDynamicsFactory::CreateDynamicsShape()
{
  mTrace.PushCall( "DynamicsFactory::CreateDynamicsShape", "" );
 return new TestDynamicsShape( mTrace );
}


} //namespace Dali
