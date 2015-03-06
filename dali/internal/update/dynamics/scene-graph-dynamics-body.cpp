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

// CLASS HEADER
#include <dali/internal/update/dynamics/scene-graph-dynamics-body.h>

// INTERNAL HEADERS
#include <dali/integration-api/dynamics/dynamics-body-intf.h>
#include <dali/integration-api/dynamics/dynamics-body-settings.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-shape-intf.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-shape.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-mesh-shape.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-world.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>
#include <dali/internal/update/nodes/node.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsBody::DynamicsBody(DynamicsWorld& world, Node& node )
: mBody(NULL),
  mNode(node),
  mWorld(world),
  mLinearVelocity(),
  mAngularVelocity()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsBody::~DynamicsBody()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__ );

  delete mBody;
}


void DynamicsBody::Initialize( Integration::DynamicsBodySettings* settings, DynamicsShape* shape )
{
  DALI_ASSERT_DEBUG( shape && "NULL shape passed into DynamicsBody" );

  mBody = mWorld.GetDynamicsFactory().CreateDynamicsBody();

  if( Dali::DynamicsBodyConfig::SOFT == settings->type )
  {
    Mesh* mesh( static_cast<DynamicsMeshShape&>(*shape).GetMesh() );
    DALI_ASSERT_DEBUG( mesh );
    mMesh = mesh;
  }

  Vector3 position;
  Quaternion rotation;
  GetNodePositionAndRotation( position, rotation );
  mBody->Initialize( *settings, shape->GetShape(), mWorld.GetDynamicsWorld(), position, rotation );
  delete settings;
}

void DynamicsBody::SetMass(const float mass)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - mass:%.2f\n", __PRETTY_FUNCTION__, mass);

  mBody->SetMass( mass );
}

void DynamicsBody::SetElasticity(const float elasticity)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - elasticity:%.1f\n", __PRETTY_FUNCTION__, elasticity);

  mBody->SetElasticity( elasticity );
}

void DynamicsBody::SetLinearVelocity(const Vector3& velocity)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (velocity:%f %f %f)\n", __PRETTY_FUNCTION__, velocity.x, velocity.y, velocity.z);

  mBody->SetLinearVelocity( velocity / mWorld.GetWorldScale() );
  mLinearVelocity[ mWorld.GetBufferIndex() ] = velocity;
}

Vector3 DynamicsBody::GetLinearVelocity( BufferIndex bufferIndex ) const
{
  const Vector3& velocity( mLinearVelocity[ bufferIndex ] );

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (velocity:%f %f %f)\n", __PRETTY_FUNCTION__,
                velocity.x, velocity.y, velocity.z);

  return velocity;
}

void DynamicsBody::SetAngularVelocity(const Vector3& velocity)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (velocity:%f %f %f)\n", __PRETTY_FUNCTION__, velocity.x, velocity.y, velocity.z);

  mBody->SetAngularVelocity( velocity );

  mAngularVelocity[ mWorld.GetBufferIndex() ] = velocity;
}

Vector3 DynamicsBody::GetAngularVelocity( BufferIndex bufferIndex ) const
{
  const Vector3& velocity( mAngularVelocity[ bufferIndex ] );

  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (velocity:%f %f %f)\n", __PRETTY_FUNCTION__,
                velocity.x, velocity.y, velocity.z);

  return velocity;
}

void DynamicsBody::SetKinematic(const bool flag)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - %s\n", __PRETTY_FUNCTION__, flag ? "true" : "false" );

  mBody->SetKinematic( flag );
}

bool DynamicsBody::IsKinematic() const
{
  return mBody->IsKinematic();
}

void DynamicsBody::SetSleepEnabled( const bool flag)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - %s\n", __PRETTY_FUNCTION__, flag ? "true" : "false" );

  mBody->SetSleepEnabled( flag );
}

void DynamicsBody::WakeUp()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  mBody->WakeUp();
}

void DynamicsBody::AddAnchor( const unsigned int index, const DynamicsBody* anchorBody, const bool collisions )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (index: %d)\n", __PRETTY_FUNCTION__, index);
  DALI_ASSERT_DEBUG( NULL != mBody );
  DALI_ASSERT_DEBUG( Dali::DynamicsBodyConfig::SOFT == mBody->GetType() );
  DALI_ASSERT_DEBUG( NULL != anchorBody && NULL != anchorBody->mBody );
  DALI_ASSERT_DEBUG( Dali::DynamicsBodyConfig::RIGID == anchorBody->GetType() );

  mBody->AddAnchor( index, anchorBody->GetBody(), collisions );
}

void DynamicsBody::ConserveVolume( const bool flag )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (%s)\n", __PRETTY_FUNCTION__, flag ? "yes" : "no" );

  mBody->ConserveVolume( flag );
}

void DynamicsBody::ConserveShape( const bool flag )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (%s)\n", __PRETTY_FUNCTION__, flag ? "yes" : "no" );

  mBody->ConserveShape( flag );
}

short int DynamicsBody::GetCollisionGroup() const
{
  return mBody->GetCollisionGroup();
}

void DynamicsBody::SetCollisionGroup( const short int collisionGroup )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (0x%04X)\n", __PRETTY_FUNCTION__, collisionGroup );
  mBody->SetCollisionGroup( collisionGroup );
}

short int DynamicsBody::GetCollisionMask() const
{
  return mBody->GetCollisionMask();
}

void DynamicsBody::SetCollisionMask( const short int collisionMask )
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s - (0x%04X)\n", __PRETTY_FUNCTION__, collisionMask );
  mBody->SetCollisionMask( collisionMask );
}

int DynamicsBody::GetType() const
{
  return mBody->GetType();
}

Integration::DynamicsBody* DynamicsBody::GetBody() const
{
  return mBody;
}

void DynamicsBody::Connect()
{
  int bodyType( mBody->GetType() );
  if( Dali::DynamicsBodyConfig::RIGID == bodyType )
  {
//    SetMotionState();
  }
  else if( Dali::DynamicsBodyConfig::SOFT == bodyType )
  {
    /*
     * Soft body vertices get local transformation applied twice due to
     * physics engine directly transforming all the points in a soft body's mesh
     * then the vertices getting the transformation applied again in the shader
     */
    mNode.SetInhibitLocalTransform(true);
  }

  RefreshDynamics();

  mWorld.AddBody(*this);
}

void DynamicsBody::Disconnect()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s (body: %p)\n", __PRETTY_FUNCTION__, mBody);

  if( Dali::DynamicsBodyConfig::SOFT == GetType() )
  {
    /*
     * Soft body vertices get local transformation applied twice due to
     * physics engine directly transforming all the points in a soft body's mesh
     * then the vertices getting the transformation applied again in the shader
     */
    mNode.SetInhibitLocalTransform(false);
  }

  mWorld.RemoveBody(*this);
}

void DynamicsBody::Delete()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::General, "%s (body: %p)\n", __PRETTY_FUNCTION__, mBody);

  mWorld.DeleteBody(*this);
}

void DynamicsBody::SetMotionState()
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

void DynamicsBody::GetNodePositionAndRotation(Vector3& position, Quaternion& rotation)
{
  const BufferIndex bufferIndex = mWorld.GetBufferIndex();
  position = mNode.GetPosition(bufferIndex) / mWorld.GetWorldScale();
  rotation = mNode.GetRotation(bufferIndex);
}

void DynamicsBody::SetNodePositionAndRotation(const Vector3& position, const Quaternion& rotation)
{
  const Vector3 scaledPosition(position * mWorld.GetWorldScale());

  const BufferIndex bufferIndex( mWorld.GetBufferIndex() );
  mNode.BakePosition(bufferIndex, scaledPosition);
  mNode.BakeOrientation(bufferIndex, rotation);

  if( Dali::DynamicsBodyConfig::RIGID == mBody->GetType() )
  {
    mLinearVelocity[bufferIndex] = mBody->GetLinearVelocity();
    mAngularVelocity[bufferIndex] = mBody->GetAngularVelocity();
  }
}

void DynamicsBody::RefreshDynamics()
{
  // get node's world position and rotation
  Vector3 position;
  Quaternion rotation;
  GetNodePositionAndRotation(position, rotation);

  mBody->SetTransform( position, rotation );
}

bool DynamicsBody::RefreshNode(BufferIndex updateBufferIndex)
{
  // get updated parameters
  if( Dali::DynamicsBodyConfig::SOFT == mBody->GetType() )
  {
    RefreshMesh(updateBufferIndex);
  }
  else
  {
    if( !mBody->IsKinematic() )
    {
      // get updated parameters
      Vector3 position;
      Quaternion rotation;
      mBody->GetTransform( position, rotation );
      SetNodePositionAndRotation( position, rotation );
    }
  }

  // TODO: Add activation state change notification
  // interrogate dynamics body for it's activation state
  bool activationState( mBody->IsActive() );

  return activationState;
}

void DynamicsBody::RefreshMesh(BufferIndex updateBufferIndex)
{
  Internal::MeshData& meshData( mMesh->GetMeshData( Mesh::UPDATE_THREAD ) );

  mBody->GetSoftVertices( meshData.GetVertices() );

  mMesh->MeshDataUpdated( updateBufferIndex, Mesh::UPDATE_THREAD, NULL );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
