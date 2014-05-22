#ifndef __DALI_INTERPOLATOR_FUNCTIONS_H__
#define __DALI_INTERPOLATOR_FUNCTIONS_H__

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
 * @addtogroup CAPI_DALI_ANIMATION_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/object/any.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/object/property.h>

namespace Dali DALI_IMPORT_API
{

// Interpolation functions used by Constraints

/**
 * @brief Interpolate linearly between two boolean values.
 * @param [in] current The current value.
 * @param [in] target The target value.
 * @param [in] progress The current progress (between 0 & 1).
 * @return The interpolated value.
 */
bool LerpBoolean( const bool& current, const bool& target, float progress );

/**
 * @brief Interpolate linearly between two float values.
 *
 * @param [in] current The current value.
 * @param [in] target The target value.
 * @param [in] progress The current progress (between 0 & 1).
 * @return The interpolated value.
 */
float LerpFloat( const float& current, const float& target, float progress );

/**
 * @brief Interpolate linearly between two Vector2 values.
 *
 * @param [in] current The current value.
 * @param [in] target The target value.
 * @param [in] progress The current progress (between 0 & 1).
 * @return The interpolated value.
 */
Vector2 LerpVector2( const Vector2& current, const Vector2& target, float progress );

/**
 * @brief Interpolate linearly between two Vector3 values.
 *
 * @param [in] current The current value.
 * @param [in] target The target value.
 * @param [in] progress The current progress (between 0 & 1).
 * @return The interpolated value.
 */
Vector3 LerpVector3( const Vector3& current, const Vector3& target, float progress );

/**
 * @brief Interpolate linearly between two Vector4 values.
 *
 * @param [in] current The current value.
 * @param [in] target The target value.
 * @param [in] progress The current progress (between 0 & 1).
 * @return The interpolated value.
 */
Vector4 LerpVector4( const Vector4& current, const Vector4& target, float progress );

/**
 * @brief Spherical linear interpolation between two Quaternion values.
 *
 * @param [in] current The current value.
 * @param [in] target The target value.
 * @param [in] progress The current progress (between 0 & 1).
 * @return The interpolated value.
 */
Quaternion SlerpQuaternion( const Quaternion& current, const Quaternion& target, float progress );

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<bool (const bool& start, const bool& target, float progress)> BoolInterpolator;

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<float (const float& start, const float& target, float progress)> FloatInterpolator;

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<Vector2 (const Vector2& current, const Vector2& target, float progress)> Vector2Interpolator;

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<Vector3 (const Vector3& current, const Vector3& target, float progress)> Vector3Interpolator;

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<Vector4 (const Vector4& current, const Vector4& target, float progress)> Vector4Interpolator;

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<Quaternion (const Quaternion& current, const Quaternion& target, float progress)> QuaternionInterpolator;

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<Matrix3 (const Matrix3& current, const Matrix3& target, float progress)> Matrix3Interpolator;

/**
 * @brief A function which interpolates between a start and target value.
 *
 * @param[in] start The start value.
 * @param[in] target The target value.
 * @param[in] progress The current progress (between 0 and 1).
 * @return The interpolated value.
 */
typedef boost::function<Matrix (const Matrix& current, const Matrix& target, float progress)> MatrixInterpolator;

/**
 * @brief Any interpolator function.
 */
typedef Any AnyInterpolator;

/**
 * @brief Retrieve an interpolator function for a property.
 *
 * This can be cast to boost::function<P (const P&, const P&, float)> where P corresponds to the property type.
 * @param[in] type The property type for which to get an interpolator
 * @return The interpolator function.
 */
AnyInterpolator GetDefaultInterpolator(Property::Type type);

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_INTERPOLATOR_FUNCTIONS_H__
