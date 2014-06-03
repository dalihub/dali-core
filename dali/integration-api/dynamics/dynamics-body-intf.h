#ifndef __DALI_INTEGRATION_DYNAMICS_BODY_INTF_H__
#define __DALI_INTEGRATION_DYNAMICS_BODY_INTF_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/geometry/mesh-data.h>

namespace Dali
{

class Quaternion;
struct Vector3;

namespace Integration
{

struct DynamicsBodySettings;
class DynamicsShape;
class DynamicsWorld;

class DALI_IMPORT_API DynamicsBody
{
public:
  /**
   * Destructor
   */
  virtual ~DynamicsBody() {}

  /**
   * Initialize the body
   * @param[in] settings      Configuration parameters for the new body
   * @param[in] shape         The collision shape for the body
   * ~param[in] dynamicsWorld The dynamics world
   * @param[in] startPosition The initial position for the body
   * @param[in] startRotation The initial rotation of the body
   */
  virtual void Initialize( const DynamicsBodySettings& settings, DynamicsShape* shape,
                           Integration::DynamicsWorld& dynamicsWorld,
                           const Vector3& startPosition, const Quaternion& startRotation ) = 0;

  /**
   * Set the mass
   * @param[in] mass The new mass for the body
   */
  virtual void SetMass(const float mass) = 0;

  ///@ copydoc Dali::DynamicsBody::SetElasticity
  virtual  void SetElasticity(const float elasticity) = 0;

  /// @copydoc Dali::DynamicsBody::SetLinearVelocity
  virtual void SetLinearVelocity(const Vector3& velocity) = 0;

  /// @copydoc Dali::DynamicsBody::GetLinearVelocity
  virtual Vector3 GetLinearVelocity() const = 0;

  /// @copydoc Dali::DynamicsBody::SetAngularVelocity
  virtual void SetAngularVelocity(const Vector3& velocity) = 0;

  /// @copydoc Dali::DynamicsBody::SetLinearVelocity
  virtual Vector3 GetAngularVelocity() const = 0;

  /// @copydoc Dali::DynamicsBody::SetKinematic
  virtual void SetKinematic(bool flag ) = 0;

  /// @copydoc Dali::DynamicsBody::IsKinematic
  virtual bool IsKinematic() const = 0;

  /// @copydoc Dali::DynamicsBody::SetSleepEnabled
  virtual void SetSleepEnabled(bool flag) = 0;

  /// @copydoc Dali::DynamicsBody::WakeUp
  virtual void WakeUp() = 0;

  /// @copydoc Dali::DynamicsBody::AddAnchor
  virtual void AddAnchor( unsigned int index, const DynamicsBody* anchorBody, const bool collisions ) = 0;

  /// @copydoc Dali::DynamicsBody::ConserveVolume
  virtual void ConserveVolume( bool flag ) = 0;

  /// @copydoc Dali::DynamicsBody::ConserveShape
  virtual void ConserveShape( bool flag ) = 0;

  /// @copydoc Dali::DynamicsBody::GetCollisionGroup
  virtual short int GetCollisionGroup() const = 0;

  /// @copydoc Dali::DynamicsBody::SetCollisionGroup
  virtual void SetCollisionGroup( short int collisionGroup ) = 0;

  /// @copydoc Dali::DynamicsBody::GetCollisionMask
  virtual short int GetCollisionMask() const = 0;

  /// @copydoc Dali::DynamicsBody::SetCollisionMask
  virtual void SetCollisionMask( short int collisionMask ) = 0;

  virtual int GetType() const = 0;

  virtual bool IsActive() const = 0;

  virtual void SetTransform( const Vector3& position, const Quaternion& rotation ) = 0;
  virtual void GetTransform( Vector3& position, Quaternion& rotation ) = 0;

  /**
   * Get the vertices and normal from a soft body
   * @param[in,out] meshData A MeshData object with preallocated space for all the vertices.
   */
  virtual void GetSoftVertices( MeshData::VertexContainer& meshData ) const = 0;

}; // class DynamicsBody

 } //namespace Integration

} //namespace Dali

#endif // __DALI_INTEGRATION_DYNAMICS_BODY_INTF_H__
