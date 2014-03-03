#ifndef __DALI_CONSTRAINTS_H__
#define __DALI_CONSTRAINTS_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>

namespace Dali DALI_IMPORT_API
{

class PropertyInput;
class Quaternion;
class Matrix;
class Matrix3;

/**
 * Scale To Fit constraint.
 *
 * Scales an Actor, such that it fits within it's Parent's Size.
 * f(current, size, parentSize) = parentSize / size
 */
struct DALI_IMPORT_API ScaleToFitConstraint
{
  ScaleToFitConstraint();

  /**
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty);

};

/**
 * Scale To Fit Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fits within its Parent's Size Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( min( parentSizeX / sizeX, min( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct DALI_IMPORT_API ScaleToFitKeepAspectRatioConstraint
{
  ScaleToFitKeepAspectRatioConstraint();

  /**
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty);
};

/**
 * Scale To Fill Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fill its Parent's Size Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( max( parentSizeX / sizeX, max( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct DALI_IMPORT_API ScaleToFillKeepAspectRatioConstraint
{
  ScaleToFillKeepAspectRatioConstraint();

  /**
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty);
};

/**
 * Scale To Fill XY Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fill its Parent's Size in the X and Y coordinates Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( max( parentSizeX / sizeX, max( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct DALI_IMPORT_API ScaleToFillXYKeepAspectRatioConstraint
{
  ScaleToFillXYKeepAspectRatioConstraint();

  /**
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty);
};

/**
 * Shrinks source size inside the target size maintaining aspect ratio of source.
 * If source is smaller than target it returns source
 */
struct ShrinkInsideKeepAspectRatioConstraint
{
  ShrinkInsideKeepAspectRatioConstraint();

  /**
   * @param[in] current The actor's current scale
   * @param[in] sizeProperty The actor's current size
   * @param[in] parentSizeProperty The parent's current size.
   * @return The actor's new scale
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& sizeProperty,
                     const PropertyInput& parentSizeProperty);
};

/**
 * MultiplyConstraint
 *
 * f(current, property) = current * property
 */
struct DALI_IMPORT_API MultiplyConstraint
{
  MultiplyConstraint();

  /**
   * @param[in] current The object's current property value
   * @param[in] property The property to multiply by.
   * @return The object's new property value
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& property);
};

/**
 * DivideConstraint
 *
 * f(current, property) = current / property
 */
struct DALI_IMPORT_API DivideConstraint
{
  DivideConstraint();

  /**
   * @param[in] current The object's current property value
   * @param[in] property The property to divide by.
   * @return The object's new property value
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& property);
};

/**
 * EqualToConstraint
 *
 * f(current, property) = property
 */
struct DALI_IMPORT_API EqualToConstraint
{
  EqualToConstraint();

  float operator()(const float current, const PropertyInput& property);
  Vector3 operator()(const Vector3& current, const PropertyInput& property);
  Vector4 operator()(const Vector4& current, const PropertyInput& property);
  Quaternion operator()(const Quaternion& current, const PropertyInput& property);
  Matrix3 operator()(const Matrix3& current, const PropertyInput& property);
  Matrix operator()(const Matrix& current, const PropertyInput& property);
};

/**
 * RelativeToConstraint
 *
 * f(current, property, scale) = property * scale
 */
struct DALI_IMPORT_API RelativeToConstraint
{
  RelativeToConstraint( float scale );
  RelativeToConstraint( const Vector3& scale );

  Vector3 operator()(const Vector3& current, const PropertyInput& property);

  Vector3 mScale;
};

struct DALI_IMPORT_API RelativeToConstraintFloat
{
  RelativeToConstraintFloat( float scale );

  float operator()(const float current, const PropertyInput& property);

  float mScale;
};

/**
 * InverseOfConstraint
 *
 * f(current, property) = 1 / property
 */
struct DALI_IMPORT_API InverseOfConstraint
{
  InverseOfConstraint();

  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& property);
};

/**
 * Constraint which sets width to be another actor's width,
 * and the height to a fixed height.
 */
struct DALI_IMPORT_API SourceWidthFixedHeight
{
  SourceWidthFixedHeight( float height );

  Vector3 operator()(const Vector3& current,
                     const PropertyInput& sourceSize);

  float mFixedHeight;
};

/**
 * Constraint which sets height to be another actor's height,
 * and the width to a fixed width.
 */
struct DALI_IMPORT_API SourceHeightFixedWidth
{
  SourceHeightFixedWidth( float width );

  Vector3 operator()(const Vector3& current,
                     const PropertyInput& sourceSize);

  float mFixedWidth;
};

/**
 * Constraint which sets camera's rotation given camera world position
 * and a target world position Uses target's up vector to orient the
 * constrained actor along the vector between camera position and
 * target position.
 *
 */
DALI_IMPORT_API Dali::Quaternion LookAt( const Dali::Quaternion& current,
                                         const PropertyInput& targetPosition,
                                         const PropertyInput& cameraPosition,
                                         const PropertyInput& targetRotation );


/**
 * Constraint which sets rotation given camera world position,
 * target world position (usually the looked at actor's world position)
 * and the angle parameter (how much the camera is offset with respect
 * to the target's up vector).
 */
struct DALI_IMPORT_API OrientedLookAt
{
  /**
   * Functor constructor
   *
   * @param[in] angle The angle of the camera's up vector with regards
   * to the target's up vector in radians. Positive angles rotate the
   * camera clockwise, negative angles rotate anti-clockwise.
   */
  OrientedLookAt( float angle );

  Dali::Quaternion operator()(const Dali::Quaternion& current,
                              const PropertyInput& targetPosition,
                              const PropertyInput& cameraPosition,
                              const PropertyInput& targetRotation );

  float mAngle;
};


} // namespace Dali

/**
 * @}
 */
#endif // __DALI_CONSTRAINTS_H__
