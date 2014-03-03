//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/update/dynamics/scene-graph-dynamics-motion-state.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/update/dynamics/scene-graph-dynamics-body.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

DynamicsMotionState::DynamicsMotionState(DynamicsBody& dynamicsBody)
: mDynamicsBody(dynamicsBody)
{
  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);
}

DynamicsMotionState::~DynamicsMotionState()
{
}

void DynamicsMotionState::getWorldTransform(btTransform& transform) const
{
//  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  // get node's world position and rotation
  Vector3 position;
  Quaternion rotation;
  Vector3 axis;
  float angle( 0.0f );

  mDynamicsBody.GetNodePositionAndRotation(position, rotation);
  rotation.ToAxisAngle( axis, angle );

  // modify parameters
  transform.setIdentity();
  transform.setOrigin(btVector3(position.x, position.y, position.z));
  if( axis != Vector3::ZERO )
  {
    transform.setRotation( btQuaternion(btVector3(axis.x, axis.y, axis.z), btScalar(angle)) );
  }
}

void DynamicsMotionState::setWorldTransform(const btTransform& transform)
{
//  DALI_LOG_INFO(Debug::Filter::gDynamics, Debug::Verbose, "%s\n", __PRETTY_FUNCTION__);

  // get updated parameters
  const btVector3& origin( transform.getOrigin() );
  const btQuaternion rotation( transform.getRotation() );
  const btVector3& axis( rotation.getAxis() );
  const btScalar& angle( rotation.getAngle() );

  Vector3 newPosition( origin.x(), origin.y(), origin.z() );
  const Vector3 newAxis( axis.x(), axis.y(), axis.z() );
  Quaternion newRotation( float(angle), newAxis );

  // set the nodes updated params
  mDynamicsBody.SetNodePositionAndRotation( newPosition, newRotation );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
