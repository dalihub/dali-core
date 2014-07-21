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
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>

namespace Dali DALI_IMPORT_API
{

class PropertyInput;
class Quaternion;
class Matrix;
class Matrix3;

/**
 * @brief Scale To Fit constraint.
 *
 * Scales an Actor, such that it fits within it's Parent's Size.
 * f(current, size, parentSize) = parentSize / size
 */
struct DALI_IMPORT_API ScaleToFitConstraint
{
  /**
   * @brief Constructor.
   */
  ScaleToFitConstraint();

  /**
   * @brief Functor operator
   *
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
 * @brief Scale To Fit Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fits within its Parent's Size Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( min( parentSizeX / sizeX, min( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct DALI_IMPORT_API ScaleToFitKeepAspectRatioConstraint
{
  /**
   * @brief Constructor.
   */
  ScaleToFitKeepAspectRatioConstraint();

  /**
   * @brief Functor operator
   *
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
 * @brief Scale To Fill Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fill its Parent's Size Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( max( parentSizeX / sizeX, max( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct DALI_IMPORT_API ScaleToFillKeepAspectRatioConstraint
{
  /**
   * @brief Constructor.
   */
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
 * @brief Scale To Fill XY Keep Aspect Ratio constraint.
 *
 * Scales an Actor, such that it fill its Parent's Size in the X and Y coordinates Keeping the aspect ratio.
 * f(current, size, parentSize) = Vector3( max( parentSizeX / sizeX, max( parentSizeY / sizeY, parentSizeZ / sizeZ ) )
 */
struct DALI_IMPORT_API ScaleToFillXYKeepAspectRatioConstraint
{
  /**
   * @brief Constructor.
   */
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
 * @brief Shrinks source size inside the target size maintaining aspect ratio of source.
 * If source is smaller than target it returns source
 */
struct ShrinkInsideKeepAspectRatioConstraint
{
  /**
   * @brief Constructor.
   */
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
 * @brief MultiplyConstraint
 *
 * f(current, property) = current * property
 */
struct DALI_IMPORT_API MultiplyConstraint
{
  /**
   * @brief Constructor.
   */
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
 * @brief DivideConstraint
 *
 * f(current, property) = current / property
 */
struct DALI_IMPORT_API DivideConstraint
{
  /**
   * @brief Constructor.
   */
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
 * @brief EqualToConstraint
 *
 * f(current, property) = property
 */
struct DALI_IMPORT_API EqualToConstraint
{
  /**
   * @brief Constructor.
   */
  EqualToConstraint();

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  float operator()(const float current, const PropertyInput& property);

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Vector3 operator()(const Vector3& current, const PropertyInput& property);

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Vector4 operator()(const Vector4& current, const PropertyInput& property);

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Quaternion operator()(const Quaternion& current, const PropertyInput& property);

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Matrix3 operator()(const Matrix3& current, const PropertyInput& property);

  /**
   * @brief override functor for float properties
   *
   * @param[in] current The current property value
   * @param[in] property The property to copy
   * @return The copy of the input property
   */
  Matrix operator()(const Matrix& current, const PropertyInput& property);
};

/**
 * @brief RelativeToConstraint for Vector3 properties
 *
 * f(current, property, scale) = property * scale
 */
struct DALI_IMPORT_API RelativeToConstraint
{
  /**
   * @brief Constructor.
   */
  RelativeToConstraint( float scale );

  /**
   * @brief Constructor.
   */
  RelativeToConstraint( const Vector3& scale );

  /**
   * @brief Functor.
   */
  Vector3 operator()(const Vector3& current, const PropertyInput& property);

  Vector3 mScale; ///< Component-wise scale factor
};

/**
 * @brief RelativeToConstraint for float properties
 */
struct DALI_IMPORT_API RelativeToConstraintFloat
{
  /**
   * @brief Constructor.
   */
  RelativeToConstraintFloat( float scale );

  /**
   * @brief Functor.
   */
  float operator()(const float current, const PropertyInput& property);

  float mScale; ///< Scale factor
};

/**
 * @brief InverseOfConstraint
 *
 * f(current, property) = 1 / property
 */
struct DALI_IMPORT_API InverseOfConstraint
{
  /**
   * @brief Constructor.
   */
  InverseOfConstraint();

  /**
   * @brief Functor.
   */
  Vector3 operator()(const Vector3&    current,
                     const PropertyInput& property);
};

/**
 * @brief Constraint which sets width to be another actor's width,
 * and the height to a fixed height.
 */
struct DALI_IMPORT_API SourceWidthFixedHeight
{
  /**
   * @brief Constructor.
   */
  SourceWidthFixedHeight( float height );

  /**
   * @brief Functor.
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& sourceSize);

  float mFixedHeight; ///< the fixed height
};

/**
 * @brief Constraint which sets height to be another actor's height,
 * and the width to a fixed width.
 */
struct DALI_IMPORT_API SourceHeightFixedWidth
{
  /**
   * @brief Constructor
   */
  SourceHeightFixedWidth( float width );

  /**
   * @brief Functor
   */
  Vector3 operator()(const Vector3& current,
                     const PropertyInput& sourceSize);

  float mFixedWidth; ///< the fixed width
};

/**
 * @brief Constraint function to aim a camera at a target.
 *
 * Constraint which sets camera's rotation given camera world position
 * and a target world position.  Uses target's up vector to orient the
 * constrained actor along the vector between camera position and
 * target position.
 *
 * @param[in] current The current rotation property value
 * @param[in] targetPosition World position of target
 * @param[in] cameraPosition World position of camera
 * @param[in] targetRotation World rotation of the target
 * @return The orientation of the camera
 */
DALI_IMPORT_API Dali::Quaternion LookAt( const Dali::Quaternion& current,
                                         const PropertyInput& targetPosition,
                                         const PropertyInput& cameraPosition,
                                         const PropertyInput& targetRotation );


/**
 * @brief Constraint functor to aim a camera at a target.
 *
 * Constraint which sets rotation given camera world position,
 * target world position (usually the looked at actor's world position)
 * and the angle parameter (how much the camera is offset with respect
 * to the target's up vector).
 */
struct DALI_IMPORT_API OrientedLookAt
{
  /**
   * @brief Functor constructor.
   *
   * @param[in] angle The angle of the camera's up vector with regards
   * to the target's up vector in radians. Positive angles rotate the
   * camera clockwise, negative angles rotate anti-clockwise.
   */
  OrientedLookAt( float angle );

  /**
   * @brief Functor.
   *
   * @param[in] current The current rotation property value
   * @param[in] targetPosition World position of target
   * @param[in] cameraPosition World position of camera
   * @param[in] targetRotation World rotation of the target
   * @return The orientation of the camera
   */
  Dali::Quaternion operator()(const Dali::Quaternion& current,
                              const PropertyInput& targetPosition,
                              const PropertyInput& cameraPosition,
                              const PropertyInput& targetRotation );

  float mAngle; ///< camera angle offset
};


} // namespace Dali

#endif // __DALI_CONSTRAINTS_H__
