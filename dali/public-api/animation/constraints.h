#ifndef __DALI_CONSTRAINTS_H__
#define __DALI_CONSTRAINTS_H__

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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/object/property-input.h>

namespace Dali
{

/**
 * @brief EqualToConstraint
 *
 * f(current, property) = property
 */
struct EqualToConstraint
{
  /**
   * @brief Constructor.
   */
  EqualToConstraint()
  { }

  /**
   * @brief override functor for float properties
   *
   * @param[in, out] current The current property value, the constrained value is set
   * @param[in] inputs Contains the property to copy
   * @return The copy of the input property
   */
  void operator()( float& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetFloat();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value, the constrained value is set
   * @param[in] inputs Contains the property to copy
   * @return The copy of the input property
   */
  void operator()( Vector2& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetVector2();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in,out] current The current property value, the constrained value is set
   * @param[in] inputs Contains the property to copy
   * @return The copy of the input property
   */
  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetVector3();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in,out] current The current property value, the constrained value is set
   * @param[in] inputs Contains the property to copy
   * @return The copy of the input property
   */
  void operator()( Vector4& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetVector4();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in,out] current The current property value, the constrained value is set
   * @param[in] inputs Contains the property to copy
   * @return The copy of the input property
   */
  void operator()( Quaternion& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetQuaternion();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in,out] current The current property value
   * @param[in] inputs Contains the property to copy
   * @return The copy of the input property
   */
  void operator()( Matrix3& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetMatrix3();
  }

  /**
   * @brief override functor for float properties
   *
   * @param[in,out] current The current property value, the constrained value is set
   * @param[in] inputs Contains the property to copy
   * @return The copy of the input property
   */
  void operator()( Matrix& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetMatrix();
  }

};

/**
 * @brief RelativeToConstraint for Vector3 properties
 *
 * current = property * scale
 */
struct RelativeToConstraint
{
  /**
   * @brief Constructor.
   */
  RelativeToConstraint( float scale )
  : mScale( scale, scale, scale ) { }

  /**
   * @brief Constructor.
   */
  RelativeToConstraint( const Vector3& scale )
  : mScale( scale ) { }

  /**
   * @brief Functor.
   */
  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetVector3() * mScale;
  }

  Vector3 mScale; ///< Component-wise scale factor
};

/**
 * @brief RelativeToConstraint for float properties
 */
struct RelativeToConstraintFloat
{
  /**
   * @brief Constructor.
   */
  RelativeToConstraintFloat( float scale )
  : mScale( scale ) { }

  /**
   * @brief Functor.
   */
  void operator()( float& current, const PropertyInputContainer& inputs )
  {
    current = inputs[0]->GetFloat() * mScale;
  }

  float mScale; ///< Scale factor
};

/**
 * @brief Constraint function to aim a camera at a target.
 *
 * Constraint which sets camera's orientation given camera world position
 * and a target world position.  Uses target's up vector to orient the
 * constrained actor along the vector between camera position and
 * target position.
 *
 * @param[in,out] current The current orientation property value, the constrained value is set.
 * @param[in] inputs Contains the World position of the target, the World position of the camera, and the world orientation of the target
 * @return The orientation of the camera
 */
inline void LookAt( Quaternion& current, const PropertyInputContainer& inputs )
{
  const PropertyInput& targetPosition( *inputs[0] );
  const PropertyInput& cameraPosition( *inputs[1] );
  const PropertyInput& targetOrientation( *inputs[2] );

  Vector3 vForward = targetPosition.GetVector3() - cameraPosition.GetVector3();
  vForward.Normalize();

  const Quaternion& targetOrientationQ = targetOrientation.GetQuaternion();

  Vector3 targetY(targetOrientationQ.Rotate(Vector3::YAXIS));
  targetY.Normalize();

  // Camera Right vector is perpendicular to forward & target up
  Vector3 vX = targetY.Cross(vForward);
  vX.Normalize();

  // Camera Up vector is perpendicular to forward and right
  Vector3 vY = vForward.Cross(vX);
  vY.Normalize();

  current = Quaternion( vX, vY, vForward );
}

} // namespace Dali

#endif // __DALI_CONSTRAINTS_H__
