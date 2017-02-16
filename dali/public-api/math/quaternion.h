#ifndef __DALI_QUATERNION_H__
#define __DALI_QUATERNION_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <iosfwd>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

// Forward declaration
class Matrix;

/**
 * @brief The Quaternion class encapsulates the mathematics of the quaternion.
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API Quaternion
{
public:

  /**
   * @brief Default Constructor.
   * @SINCE_1_0.0
   */
  Quaternion();

  /**
   * @brief Constructs from a quaternion represented by floats.
   *
   * @SINCE_1_0.0
   * @param[in] cosThetaBy2
   * @param[in] iBySineTheta
   * @param[in] jBySineTheta
   * @param[in] kBySineTheta
   */
  Quaternion( float cosThetaBy2, float iBySineTheta, float jBySineTheta, float kBySineTheta );

  /**
   * @brief Constructs from a quaternion represented by a vector.
   *
   * @SINCE_1_0.0
   * @param[in] vector x,y,z fields represent i,j,k coefficients, w represents cos(theta/2)
   */
  explicit Quaternion( const Vector4& vector );

  /**
   * @brief Constructor from an axis and angle.
   *
   * @SINCE_1_0.0
   * @param[in] angle The angle around the axis
   * @param[in] axis  The vector of the axis
   */
  Quaternion( Radian angle, const Vector3& axis );

  /**
   * @brief Constructs from Euler angles.
   *
   * @SINCE_1_0.0
   * @param[in] pitch
   * @param[in] yaw
   * @param[in] roll
   */
  Quaternion( Radian pitch, Radian yaw, Radian roll );

  /**
   * @brief Constructs from a matrix.
   *
   * @SINCE_1_0.0
   * @param[in] matrix
   */
  explicit Quaternion(const Matrix& matrix);

  /**
   * @brief Constructs from 3 orthonormal axes.
   *
   * @SINCE_1_0.0
   * @param[in] xAxis The X axis
   * @param[in] yAxis The Y axis
   * @param[in] zAxis The Z axis
   */
  explicit Quaternion( const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis );

  /**
   * @brief Constructs quaternion which describes minimum rotation to align v0 with v1.
   * @SINCE_1_0.0
   * @param[in] v0 First normalized vector
   * @param[in] v1 Second normalized vector
   * @pre v0 and v1 should be normalized
   *
   */
  explicit Quaternion( const Vector3& v0, const Vector3& v1 );

  /**
   * @brief Destructor, nonvirtual as this is not a base class.
   *
   * @SINCE_1_0.0
   */
  ~Quaternion();

// Constants

  static const Quaternion IDENTITY; ///< (0.0f,0.0f,0.0f,1.0f)

  /**
   * @brief Helper to check if this is an identity quaternion.
   *
   * @SINCE_1_0.0
   * @return True if this is identity quaternion
   */
  bool IsIdentity() const;

  /**
   * @brief Converts the quaternion to an axis/angle pair.
   *
   * @SINCE_1_0.0
   * @param[out] axis
   * @param[out] angle Angle in radians
   * @return True if converted correctly
   */
  bool ToAxisAngle( Vector3& axis, Radian& angle ) const;

  /**
   * @brief Returns the quaternion as a vector.
   *
   * @SINCE_1_0.0
   * @return The vector representation of the quaternion
   */
  const Vector4& AsVector() const;

  /**
   * @brief SetEuler sets the quaternion from the Euler angles applied in x, y, z order.
   *
   * @SINCE_1_0.0
   * @param[in] pitch
   * @param[in] yaw
   * @param[in] roll
   */
  void SetEuler( Radian pitch, Radian yaw, Radian roll );

  /**
   * @brief Returns the Euler angles from a rotation Quaternion.
   *
   * @SINCE_1_0.0
   * @return A vector of Euler angles (x == pitch, y == yaw, z == roll)
   */
  Vector4 EulerAngles() const;

  /**
   * @brief Addition operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The quaternion to add
   * @return A quaternion containing the result of the addition
   */
  const Quaternion operator+( const Quaternion& other ) const;

  /**
   * @brief Subtraction operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The quaternion to subtract
   * @return A quaternion containing the result of the subtract
   */
  const Quaternion operator-( const Quaternion& other ) const;

  /**
   * @brief Multiplication operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The quaternion to multiply
   * @return A quaternion containing the result
   */
  const Quaternion operator*( const Quaternion& other ) const;

  /**
   * @brief Multiplication operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The vector to multiply
   * @return A vector containing the result of the multiplication
   */
  Vector3 operator*( const Vector3& other ) const;

  /**
   * @brief Division operator.
   *
   * @SINCE_1_0.0
   * @param[in] other A quaternion to divide by
   * @return A quaternion containing the result
   */
  const Quaternion operator/( const Quaternion& other ) const;

  /**
   * @brief Scale operator.
   *
   * @SINCE_1_0.0
   * @param[in] scale A value to scale by
   * @return A quaternion containing the result
   */
  const Quaternion operator*( float scale ) const;

  /**
   * @brief Scale operator.
   *
   * @SINCE_1_0.0
   * @param[in] scale A value to scale by
   * @return A quaternion containing the result
   */
  const Quaternion operator/( float scale ) const;

  /**
   * @brief Unary Negation operator.
   *
   * @SINCE_1_0.0
   * @return A quaternion containing the negated result
   */
  Quaternion operator-() const;

  /**
   * @brief Addition with Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The quaternion to add
   * @return A reference to this
   */
  const Quaternion& operator+=( const Quaternion& other );

  /**
   * @brief Subtraction with Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The quaternion to subtract
   * @return A reference to this
   */
  const Quaternion& operator-=( const Quaternion& other );

  /**
   * @brief Multiplication with Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] other The quaternion to multiply
   * @return A reference to this
   */
  const Quaternion& operator*=( const Quaternion& other );

  /**
   * @brief Scale with Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] scale the value to scale by
   * @return A reference to this
   */
  const Quaternion& operator*=( float scale );

  /**
   * @brief Scale with Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] scale The value to scale by
   * @return A reference to this
   */
  const Quaternion& operator/=( float scale );

  /**
   * @brief Equality operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The quaternion to compare with
   * @return True if the quaternions are equal
   */
  bool operator==( const Quaternion& rhs ) const;

  /**
   * @brief Inequality operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The quaternion to compare with
   * @return True if the quaternions are not equal
   */
  bool operator!=( const Quaternion& rhs ) const;

  /**
   * @brief Returns the length of the quaternion.
   *
   * @SINCE_1_0.0
   * @return The length of the quaternion
   */
  float Length() const;

  /**
   * @brief Returns the squared length of the quaternion.
   *
   * @SINCE_1_0.0
   * @return The squared length of the quaternion
   */
  float LengthSquared() const;

  /**
   * @brief Normalizes this to unit length.
   *
   * @SINCE_1_0.0
   */
  void Normalize();

  /**
   * @brief Normalized.
   *
   * @SINCE_1_0.0
   * @return A normalized version of this quaternion
   */
  Quaternion Normalized() const;

  /**
   * @brief Conjugates this quaternion.
   *
   * @SINCE_1_0.0
   */
  void Conjugate();

  /**
   * @brief Inverts this quaternion.
   *
   * @SINCE_1_0.0
   */
  void Invert();

  /**
   * @brief Performs the logarithm of a Quaternion = v*a where q = (cos(a),v*sin(a)).
   *
   * @SINCE_1_0.0
   * @return A quaternion representing the logarithm
   */
  Quaternion Log() const;

  /**
   * @brief Performs an exponent e^Quaternion = Exp(v*a) = (cos(a),vsin(a)).
   *
   * @SINCE_1_0.0
   * @return A quaternion representing the exponent
   */
  Quaternion Exp() const;

  /**
   * @brief Returns the dot product of two quaternions.
   *
   * @SINCE_1_0.0
   * @param[in] q1 The first quaternion
   * @param[in] q2 The second quaternion
   * @return The dot product of the two quaternions
   */
  static float Dot( const Quaternion &q1, const Quaternion &q2 );

  /**
   * @brief Linear Interpolation (using a straight line between the two quaternions).
   *
   * @SINCE_1_0.0
   * @param[in] q1 The start quaternion
   * @param[in] q2 The end quaternion
   * @param[in] t A progress value between 0 and 1
   * @return The interpolated quaternion
   */
  static Quaternion Lerp( const Quaternion &q1, const Quaternion &q2, float t );

  /**
   * @brief Spherical Linear Interpolation (using the shortest arc of a great circle between
   * the two quaternions).
   *
   * @SINCE_1_0.0
   * @param[in] q1 The start quaternion
   * @param[in] q2 The end quaternion
   * @param[in] progress A progress value between 0 and 1
   * @return The interpolated quaternion
   */
  static Quaternion Slerp( const Quaternion &q1, const Quaternion &q2, float progress );

  /**
   * @brief This version of Slerp, used by Squad, does not check for theta > 90.
   *
   * @SINCE_1_0.0
   * @param[in] q1 The start quaternion
   * @param[in] q2 The end quaternion
   * @param[in] t A progress value between 0 and 1
   * @return The interpolated quaternion
   */
  static Quaternion SlerpNoInvert( const Quaternion &q1, const Quaternion &q2, float t );

  /**
   * @brief Spherical Cubic Interpolation.
   *
   * @SINCE_1_0.0
   * @param[in] start The start quaternion
   * @param[in] end The end quaternion
   * @param[in] ctrl1 The control quaternion for q1
   * @param[in] ctrl2 The control quaternion for q2
   * @param[in] t A progress value between 0 and 1
   * @return The interpolated quaternion
   */
  static Quaternion Squad( const Quaternion& start, const Quaternion& end,  const Quaternion& ctrl1,  const Quaternion& ctrl2, float t );

  /**
   * @brief Returns the shortest angle between two quaternions in Radians.
   *
   * @SINCE_1_0.0
   * @param[in] q1 The first quaternion
   * @param[in] q2 The second quaternion
   * @return The angle between the two quaternions
   */
  static float AngleBetween( const Quaternion& q1, const Quaternion& q2 );

  /**
   * @brief Rotates v by this Quaternion (Quaternion must be unit).
   *
   * @SINCE_1_0.0
   * @param[in] vector A vector to rotate
   * @return The rotated vector
   */
  Vector4 Rotate( const Vector4& vector ) const;

  /**
   * @brief Rotates v by this Quaternion (Quaternion must be unit).
   *
   * @SINCE_1_0.0
   * @param[in] vector A vector to rotate
   * @return The rotated vector
   */
  Vector3 Rotate( const Vector3& vector ) const;

private:

  /// @cond internal
  /**
   * @brief Sets the quaternion from 3 orthonormal axes.
   *
   * @SINCE_1_0.0
   * @param[in] xAxis The X axis
   * @param[in] yAxis The Y axis
   * @param[in] zAxis The Z axis
   */
  DALI_INTERNAL void SetFromAxes( const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis );
  /// @endcond

public:

  Vector4 mVector;   ///< w component is s ( = cos(theta/2.0) )
};

/**
 * @brief Prints a Quaternion.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] quaternion The quaternion to print
 * @return The output stream operator
 */
DALI_IMPORT_API std::ostream& operator<< (std::ostream& o, const Quaternion& quaternion);

// Allow Quaternion to be treated as a POD type
template <> struct TypeTraits< Quaternion > : public BasicTypes< Quaternion > { enum { IS_TRIVIAL_TYPE = true }; };

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_QUATERNION_H__
