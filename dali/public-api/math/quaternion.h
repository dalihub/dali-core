#ifndef __DALI_QUATERNION_H__
#define __DALI_QUATERNION_H__

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

// EXTERNAL INCLUDES
#include <iosfwd>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{

// Forward declaration
class Matrix;

/**
 * @brief The Quaternion class encapsulates the mathematics of the quaternion.
 */
class DALI_IMPORT_API Quaternion
{
public:

  /**
   * @brief Default Constructor
   */
  Quaternion();

  /**
   * @brief Construct from a quaternion represented by floats.
   *
   * @param[in] cosThetaBy2
   * @param[in] iBySineTheta
   * @param[in] jBySineTheta
   * @param[in] kBySineTheta
   */
  Quaternion(float cosThetaBy2, float iBySineTheta, float jBySineTheta, float kBySineTheta);

  /**
   * @brief Construct from a quaternion represented by a vector.
   *
   * @param[in] vector - x,y,z fields represent i,j,k coefficients, w represents cos(theta/2)
   */
  explicit Quaternion(const Vector4& vector);

  /**
   * @brief Constructor from an axis and angle.
   *
   * @param[in] angle - the angle around the axis
   * @param[in] axis  - the vector of the axis
   */
  Quaternion(float angle, const Vector3 &axis);

  /**
   * @brief Constructor from an axis and angle.
   *
   * @param[in] theta - the angle of the axis
   * @param[in] axis  - the unit vector of the axis
   */
  Quaternion(float theta, const Vector4 &axis);

  /**
   * @brief Construct from Euler angles.
   *
   * @param[in] x - the X axis euler angle (pitch)
   * @param[in] y - the Y axis euler angle (yaw)
   * @param[in] z - the Z axis euler angle (roll)
   */
  Quaternion(float x, float y, float z);

  /**
   * @brief Construct from a matrix.
   *
   * @param[in] matrix
   */
  explicit Quaternion(const Matrix& matrix);

  /**
   * @brief Construct from 3 orthonormal axes.
   *
   * @param[in] xAxis The X axis
   * @param[in] yAxis The Y axis
   * @param[in] zAxis The Z axis
   */
  explicit Quaternion( const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis );

  /**
   * @brief Construct quaternion which describes minimum rotation to align v0 with v1
   * @pre v0 and v1 should be normalized
   *
   * @param[in] v0 First normalized vector
   * @param[in] v1 Second normalized vector
   */
  explicit Quaternion( const Vector3& v0, const Vector3& v1 );

  /**
   * @brief Converts an axis + angle pair rotation to a Quaternion.
   *
   * @param[in] axis
   * @param[in] angle
   * @return the represented quaternion
   */
  static Quaternion FromAxisAngle(const Vector4 &axis, float angle);

  /**
   * @brief Destructor, nonvirtual as this is not a base class.
   *
   */
  ~Quaternion();

// Constants

  static const Quaternion IDENTITY; ///< (0.0f,0.0f,0.0f,1.0f)

  /**
   * @brief Helper to check if this is an identity quaternion
   *
   * @return true if this is identity quaternion
   */
  bool IsIdentity() const;

  /**
   * @brief Convert the quaternion to an axis/angle pair.
   *
   * @param[out] axis
   * @param[out] angle
   * @return true if converted correctly
   */
  bool ToAxisAngle(Vector3 &axis, float &angle) const;

  /**
   * @brief Convert the quaternion to an axis/angle pair.
   *
   * @param[out] axis
   * @param[out] angle
   * @return true if converted correctly
   */
  bool ToAxisAngle(Vector4 &axis, float &angle) const;

  /**
   * @brief Return the quaternion as a vector.
   *
   * @return the vector representation of the quaternion
   */
  const Vector4& AsVector() const;

  /**
   * @brief SetEuler sets the quaternion from the Euler angles applied in x, y, z order.
   *
   * @param[in] x - the X axis euler angle (pitch)
   * @param[in] y - the Y axis euler angle (yaw)
   * @param[in] z - the Z axis euler angle (roll)
   */
  void SetEuler(float x, float y, float z);

  /**
   * @brief returns the Euler angles from a rotation Quaternion.
   *
   * @return a vector of Euler angles (x == pitch, y == yaw, z == roll)
   */
  Vector4 EulerAngles() const;

  /**
   * @brief Addition operator.
   *
   * @param[in] other The quaternion to add
   * @return A quaternion containing the result of the addition
   */
  const Quaternion operator +(const Quaternion &other) const;

  /**
   * @brief Subtraction operator.
   *
   * @param[in] other The quaternion to subtract
   * @return A quaternion containing the result of the subtract
   */
  const Quaternion operator -(const Quaternion &other) const;

  /**
   * @brief Multiplication operator.
   *
   * @param[in] other The quaternion to multiply
   * @return A quaternion containing the result
   */
  const Quaternion operator *(const Quaternion &other) const;

  /**
   * @brief Multiplication operator.
   *
   * @param[in] v The vector to multiply
   * @return A vector containing the result of the multiplication
   */
  Vector3 operator *(const Vector3& v) const;

  /**
   * @brief Division operator.
   *
   * @param[in] other a quaternion to divide by
   * @return A quaternion containing the result
   */
  const Quaternion operator /(const Quaternion &other) const;

  /**
   * @brief Scale operator.
   *
   * @param[in] scale A value to scale by
   * @return A quaternion containing the result
   */
  const Quaternion operator *(float scale) const;

  /**
   * @brief Scale operator.
   *
   * @param[in] scale A value to scale by
   * @return A quaternion containing the result
   */
  const Quaternion operator /(float scale) const;

  /**
   * @brief Unary Negation operator.
   *
   * @return A quaternion containing the negated result
   */
  Quaternion operator -() const;

  /**
   * @brief Addition with Assignment operator.
   *
   * @param[in] other The quaternion to add
   * @return itself
   */
  const Quaternion &operator +=(const Quaternion &other);

  /**
   * @brief Subtraction with Assignment operator.
   *
   * @param[in] other The quaternion to subtract
   * @return itself
   */
  const Quaternion &operator -=(const Quaternion &other);

  /**
   * @brief Multiplication with Assignment operator.
   *
   * @param[in] other The quaternion to multiply
   * @return itself
   */
  const Quaternion &operator *=(const Quaternion &other);

  /**
   * @brief Scale with Assignment operator.
   *
   * @param[in] scale the value to scale by
   * @return itself
   */
  const Quaternion &operator *= (float scale);

  /**
   * @brief Scale with Assignment operator.
   *
   * @param[in] scale the value to scale by
   * @return itself
   */
  const Quaternion &operator /= (float scale);

  /**
   * @brief Equality operator.
   *
   * @param[in] rhs The quaterion to compare with.
   * @return True if the quaternions are equal.
   */
  bool operator== (const Quaternion& rhs) const;

  /**
   * @brief Inequality operator.
   *
   * @param[in] rhs The quaterion to compare with.
   * @return True if the quaternions are not equal.
   */
  bool operator!= (const Quaternion& rhs) const;

  /**
   * @brief Return the length of the quaternion.
   *
   * @return the length of the quaternion
   */
  float Length() const;

  /**
   * @brief Return the squared length of the quaternion.
   *
   * @return the squared length of the quaternion
   */
  float LengthSquared() const;

  /**
   * @brief Normalize this to unit length.
   *
   */
  void Normalize();

  /**
   * @brief Normalized.
   *
   * @return a normalized version of this quaternion
   */
  Quaternion Normalized() const;

  /**
   * @brief Conjugate this quaternion.
   *
   */
  void Conjugate();

  /**
   * @brief Invert this quaternion.
   *
   */
  void Invert();

  /**
   * @brief Performs the logarithm of a Quaternion = v*a where q = (cos(a),v*sin(a)).
   *
   * @return a quaternion representing the logarithm
   */
  Quaternion Log() const;

  /**
   * @brief Performs an exponent e^Quaternion = Exp(v*a) = (cos(a),vsin(a)).
   *
   * @return a quaternion representing the exponent
   */
  Quaternion Exp() const;

  /**
   * @brief Return the dot product of two quaternions.
   *
   * @param[in] q1 - the first quaternion
   * @param[in] q2 - the second quaternion
   * @return the dot product of the two quaternions
   */
  static float Dot(const Quaternion &q1, const Quaternion &q2);

  /**
   * @brief Linear Interpolation (using a straight line between the two quaternions).
   *
   * @param[in] q1 - the start quaternion
   * @param[in] q2 - the end quaternion
   * @param[in] t  - a progress value between 0 and 1
   * @return the interpolated quaternion
   */
  static Quaternion Lerp(const Quaternion &q1, const Quaternion &q2, float t);

  /**
   * @brief Spherical Linear Interpolation (using the shortest arc of a great circle between
   * the two quaternions).
   *
   * @param[in] q1 - the start quaternion
   * @param[in] q2 - the end quaternion
   * @param[in] progress  - a progress value between 0 and 1
   * @return the interpolated quaternion
   */
  static Quaternion Slerp(const Quaternion &q1, const Quaternion &q2, float progress);

  /**
   * @brief This version of Slerp, used by Squad, does not check for theta > 90.
   *
   * @param[in] q1 - the start quaternion
   * @param[in] q2 - the end quaternion
   * @param[in] t  - a progress value between 0 and 1
   * @return the interpolated quaternion
   */
  static Quaternion SlerpNoInvert(const Quaternion &q1, const Quaternion &q2, float t);

  /**
   * @brief Spherical Cubic Interpolation.
   *
   * @param[in] q1 - the start quaternion
   * @param[in] q2 - the end quaternion
   * @param[in] a  - the control quaternion for q1
   * @param[in] b  - the control quaternion for q2
   * @param[in] t  - a progress value between 0 and 1
   * @return the interpolated quaternion
   */
  static Quaternion Squad(const Quaternion &q1,const Quaternion &q2,const Quaternion &a,const Quaternion &b,float t);

  /**
   * @brief Returns the shortest angle between two quaternions in Radians.
   *
   * @param[in] q1 - the first quaternion
   * @param[in] q2 - the second quaternion
   * @return the angle between the two quaternions.
   */
  static float AngleBetween(const Quaternion &q1, const Quaternion &q2);

  /**
   * @brief Rotate v by this Quaternion (Quaternion must be unit).
   *
   * @param[in] v - a vector to rotate
   * @return the rotated vector
   */
  Vector4 Rotate(const Vector4 &v) const;

  /**
   * @brief Rotate v by this Quaternion (Quaternion must be unit).
   *
   * @param[in] v - a vector to rotate
   * @return the rotated vector
   */
  Vector3 Rotate(const Vector3 &v) const;

private:

  /**
   * @brief Set the quaternion from 3 orthonormal axes.
   *
   * @param[in] xAxis The X axis
   * @param[in] yAxis The Y axis
   * @param[in] zAxis The Z axis
   */
  DALI_INTERNAL void SetFromAxes( const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis );

public:

  Vector4 mVector;   ///< w component is s ( = cos(theta/2.0) )
};

/**
 * @brief Print a Quaternion.
 *
 * @param [in] o The output stream operator.
 * @param [in] quaternion The quaternion to print.
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<< (std::ostream& o, const Quaternion& quaternion);

} // namespace Dali

#endif // __DALI_QUATERNION_H__
