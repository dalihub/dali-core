#ifndef __DYNAMICS_UNIVERSAL_JOINT_IMPL_H__
#define __DYNAMICS_UNIVERSAL_JOINT_IMPL_H__

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

// BASE CLASS HEADER
#include <dali/internal/event/dynamics/dynamics-joint-impl.h>

// INTERNAL HEADERS
#include <dali/internal/event/dynamics/dynamics-declarations.h>

namespace Dali
{

class Quaternion;
struct Vector3;

namespace Internal
{

namespace SceneGraph
{
class DynamicsUniversalJoint;
} // namespace SceneGraph

/// @copydoc Dali::DynamicsUniversalJoint
class DynamicsUniversalJoint : public DynamicsJoint
{
public:

  /**
   * Constructor.
   * @copydoc Dali::DynamicsJoint::NewUniversal
   */
  DynamicsUniversalJoint(DynamicsWorldPtr world,
                         DynamicsBodyPtr bodyA, DynamicsBodyPtr bodyB,
                         const Vector3& pointA, const Quaternion& orientationA,
                         const Vector3& pointB, const Quaternion& orientationB );

protected:
  /**
   * Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~DynamicsUniversalJoint();

private:
  // unimplemented copy constructor and assignment operator
  DynamicsUniversalJoint(const DynamicsUniversalJoint&);
  DynamicsUniversalJoint& operator=(const DynamicsUniversalJoint&);

  // Methods
public:
  /// @copydoc Dali::DynamicsJoint::GetTranslationLowerLimit
  Vector3 GetTranslationLowerLimit() const;

  /// @copydoc Dali::DynamicsJoint::SetTranslationLowerLimit
  void SetTranslationLowerLimit( const Vector3& limit );

  /// @copydoc Dali::DynamicsJoint::GetTranslationUpperLimit
  Vector3 GetTranslationUpperLimit() const;

  /// @copydoc Dali::DynamicsJoint::SetTranslationUpperLimit
  void SetTranslationUpperLimit( const Vector3& limit );

  /// @copydoc Dali::DynamicsJoint::GetRotationLowerLimit
  Vector3 GetRotationLowerLimit() const;

  /// @copydoc Dali::DynamicsJoint::SetRotationLowerLimit
  void SetRotationLowerLimit( const Vector3& limit );

  /// @copydoc Dali::DynamicsJoint::GetRotationUpperLimit
  Vector3 GetRotationUpperLimit() const;

  /// @copydoc Dali::DynamicsJoint::SetRotationUpperLimit
  void SetRotationUpperLimit( const Vector3& limit );

  SceneGraph::DynamicsUniversalJoint* GetSceneObject() const;

private:
  Vector3     mTranslationLowerLimit;
  Vector3     mTranslationUpperLimit;
  Vector3     mRotationLowerLimit;
  Vector3     mRotationUpperLimit;
}; // class DynamicsUniversalJoint

} // namespace Internal

} // namespace Dali

#endif // __DYNAMICS_UNIVERSAL_JOINT_IMPL_H__
