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
#include <dali/public-api/animation/constraints.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/object/property-input.h>

namespace Dali
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// ScaleToFitConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

ScaleToFitConstraint::ScaleToFitConstraint()
{

}

Vector3 ScaleToFitConstraint::operator()(const Vector3&    current,
                   const PropertyInput& sizeProperty,
                   const PropertyInput& parentSizeProperty)
{
  const Vector3 size = sizeProperty.GetVector3();
  const Vector3 parentSize = parentSizeProperty.GetVector3();

  return Vector3( fabsf(size.x) > Math::MACHINE_EPSILON_1 ? (parentSize.x / size.x) : 0.0f,
                  fabsf(size.y) > Math::MACHINE_EPSILON_1 ? (parentSize.y / size.y) : 0.0f,
                  fabsf(size.z) > Math::MACHINE_EPSILON_1 ? (parentSize.z / size.z) : 0.0f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ScaleToFitKeepAspectRatioConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

ScaleToFitKeepAspectRatioConstraint::ScaleToFitKeepAspectRatioConstraint()
{

}

Vector3 ScaleToFitKeepAspectRatioConstraint::operator() (
    const Vector3&    current,
    const PropertyInput& sizeProperty,
    const PropertyInput& parentSizeProperty )
{
  return FitKeepAspectRatio( parentSizeProperty.GetVector3(), sizeProperty.GetVector3() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ScaleToFillKeepAspectRatioConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

ScaleToFillKeepAspectRatioConstraint::ScaleToFillKeepAspectRatioConstraint()
{

}

Vector3 ScaleToFillKeepAspectRatioConstraint::operator() (
    const Vector3&    current,
    const PropertyInput& sizeProperty,
    const PropertyInput& parentSizeProperty )
{
  return FillKeepAspectRatio( parentSizeProperty.GetVector3(), sizeProperty.GetVector3() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ScaleToFillXYKeepAspectRatioConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

ScaleToFillXYKeepAspectRatioConstraint::ScaleToFillXYKeepAspectRatioConstraint()
{

}

Vector3 ScaleToFillXYKeepAspectRatioConstraint::operator() (
    const Vector3&    current,
    const PropertyInput& sizeProperty,
    const PropertyInput& parentSizeProperty )
{
  return FillXYKeepAspectRatio( parentSizeProperty.GetVector3(), sizeProperty.GetVector3() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// ShrinkInsideKeepAspectRatioConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

ShrinkInsideKeepAspectRatioConstraint::ShrinkInsideKeepAspectRatioConstraint()
{
}

Vector3 ShrinkInsideKeepAspectRatioConstraint::operator() (
    const Vector3&    current,
    const PropertyInput& sizeProperty,
    const PropertyInput& parentSizeProperty )
{
  return ShrinkInsideKeepAspectRatio( parentSizeProperty.GetVector3(), sizeProperty.GetVector3() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// MultiplyConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

MultiplyConstraint::MultiplyConstraint()
{

}

Vector3 MultiplyConstraint::operator()(const Vector3&    current,
                   const PropertyInput& property)
{
  return current * property.GetVector3();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// DivideConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

DivideConstraint::DivideConstraint()
{

}

Vector3 DivideConstraint::operator()(const Vector3&    current,
                   const PropertyInput& property)
{
  const Vector3 value = property.GetVector3();

  return Vector3( fabsf(value.x) > Math::MACHINE_EPSILON_1 ? (current.x / value.x) : 0.0f,
                  fabsf(value.y) > Math::MACHINE_EPSILON_1 ? (current.y / value.y) : 0.0f,
                  fabsf(value.z) > Math::MACHINE_EPSILON_1 ? (current.z / value.z) : 0.0f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EqualToConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

EqualToConstraint::EqualToConstraint()
{
}

float EqualToConstraint::operator()(const float current, const PropertyInput& property)
{
  return property.GetFloat();
}

Vector3 EqualToConstraint::operator()(const Vector3& current, const PropertyInput& property)
{
  return property.GetVector3();
}

Vector4 EqualToConstraint::operator()(const Vector4& current, const PropertyInput& property)
{
  return property.GetVector4();
}

Quaternion EqualToConstraint::operator()(const Quaternion& current, const PropertyInput& property)
{
  return property.GetQuaternion();
}

Matrix3 EqualToConstraint::operator()(const Matrix3& current, const PropertyInput& property)
{
  return property.GetMatrix3();
}

Matrix EqualToConstraint::operator()(const Matrix& current, const PropertyInput& property)
{
  return property.GetMatrix();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// RelativeToConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

RelativeToConstraint::RelativeToConstraint( float scale )
: mScale( scale, scale, scale )
{
}

RelativeToConstraint::RelativeToConstraint( const Vector3& scale )
: mScale( scale )
{
}

Vector3 RelativeToConstraint::operator()( const Vector3& current, const PropertyInput& property )
{
  return property.GetVector3() * mScale;
}

RelativeToConstraintFloat::RelativeToConstraintFloat( float scale )
: mScale( scale )
{
}

float RelativeToConstraintFloat::operator()( const float current, const PropertyInput& property )
{
  return property.GetFloat() * mScale;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InverseOfConstraint
///////////////////////////////////////////////////////////////////////////////////////////////////

InverseOfConstraint::InverseOfConstraint()
{

}

Vector3 InverseOfConstraint::operator()(const Vector3&    current,
                   const PropertyInput& property)
{
  const Vector3& value = property.GetVector3();

  return Vector3( fabsf(value.x) > Math::MACHINE_EPSILON_1 ? (1.0f / value.x) : 0.0f,
                  fabsf(value.y) > Math::MACHINE_EPSILON_1 ? (1.0f / value.y) : 0.0f,
                  fabsf(value.z) > Math::MACHINE_EPSILON_1 ? (1.0f / value.z) : 0.0f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SourceWidthFixedHeight
///////////////////////////////////////////////////////////////////////////////////////////////////

SourceWidthFixedHeight::SourceWidthFixedHeight( float height )
: mFixedHeight( height )
{
}

Vector3 SourceWidthFixedHeight::operator()( const Vector3& current, const PropertyInput& sourceSize )
{
  return Vector3( sourceSize.GetVector3().width, mFixedHeight, current.depth );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SourceHeightFixedWidth
///////////////////////////////////////////////////////////////////////////////////////////////////

SourceHeightFixedWidth::SourceHeightFixedWidth( float width )
: mFixedWidth( width )
{
}

Vector3 SourceHeightFixedWidth::operator()( const Vector3& current, const PropertyInput& sourceSize )
{
  return Vector3( mFixedWidth, sourceSize.GetVector3().height, current.depth );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// LookAt
///////////////////////////////////////////////////////////////////////////////////////////////////

static Quaternion LookAtTarget( const Vector3& vForward, const Quaternion& targetRotation )
{
  Vector3 vRight;
  Vector3 vUp;

  Vector3 targetUp(targetRotation.Rotate(-Vector3::YAXIS));
  targetUp.Normalize();

  // Camera Right vector is perpendicular to forward & target up
  vRight = vForward.Cross(targetUp);
  vRight.Normalize();

  // Camera Up vector is perpendicular to forward and right
  vUp = vForward.Cross(vRight);
  vUp.Normalize();

  return Quaternion( vRight, vUp, vForward );
}

Dali::Quaternion LookAt( const Dali::Quaternion& current,
                         const PropertyInput& targetPosition,
                         const PropertyInput& cameraPosition,
                         const PropertyInput& targetRotation )
{
  Vector3 vForward = targetPosition.GetVector3() - cameraPosition.GetVector3();
  vForward.Normalize();

  return LookAtTarget(vForward, targetRotation.GetQuaternion());
}


OrientedLookAt::OrientedLookAt( float angle )
: mAngle (angle)
{
}

Quaternion OrientedLookAt::operator()(const Dali::Quaternion& current,
                                      const PropertyInput& targetPosition,
                                      const PropertyInput& cameraPosition,
                                      const PropertyInput& targetRotation )
{
  Vector3 vForward = targetPosition.GetVector3() - cameraPosition.GetVector3();
  vForward.Normalize();

  // Calculate world vector of target's local up rotated by mAngle around z axis,
  // multiplied by target's world rotation
  Quaternion worldRot = targetRotation.GetQuaternion();
  Quaternion localRot( mAngle, Vector3::ZAXIS );
  Quaternion targetOrientation = worldRot * localRot;

  return LookAtTarget(vForward, targetOrientation);
}

} // namespace Dali
