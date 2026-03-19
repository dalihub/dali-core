#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include <iosfwd>

#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/math/angle-axis.h> //incl vec3
#include <dali/public-api/math/matrix3.h>    // incl vec3,mat
#include <dali/public-api/math/quaternion.h> //incl vec4
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{
class String;

/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Print a String
 *
 * @SINCE_2.5.15
 * @param[in] o The output stream operator
 * @param[in] string The string to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& outStream, const String& string);
/**
 * @}
 */

/**
 * @addtogroup dali_core_math
 * @{
 */

/**
 * @brief Print a Vector2.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] vector The vector to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& o, const Vector2& vector);

/**
 * @brief Prints a Vector3.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] vector The vector to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& o, const Vector3& vector);

/**
 * @brief Print a Vector4.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] vector The vector to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& o, const Vector4& vector);

/**
 * @brief Prints a Quaternion.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] quaternion The quaternion to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& o, const Quaternion& quaternion);

/**
 * @brief Prints a 3x3 matrix.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] matrix The matrix to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& o, const Matrix3& matrix);

/**
 * @brief Prints a 4x4 matrix.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] matrix The matrix to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& o, const Matrix& matrix);

/**
 * @brief Prints an angle axis.
 *
 * @SINCE_1_1.33
 * @param[in] o The output stream operator
 * @param[in] angleAxis The angle axis to print
 * @return The output stream operator
 */
std::ostream& operator<<(std::ostream& o, const Dali::AngleAxis& angleAxis);

/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Converts the value of the extents into a string and insert in to an output stream.
 *
 * @SINCE_1_2.62
 * @param[in] stream The output stream operator
 * @param[in] extents The Extents to output
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Extents& extents);

/**
 * @}
 */

/**
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Converts the value of the property into a string and appends to an output stream.
 *
 * @SINCE_1_0.0
 * @param[in] ouputStream The output stream operator
 * @param[in] value The value to insert
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& ouputStream, const Property::Value& value);

/**
 * @brief Converts the key/value pairs of the property map into a string and append to an output stream.
 *
 * @SINCE_1_1.28
 * @param[in] stream The output stream operator
 * @param[in] map The map to insert
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Property::Map& map);

/**
 * @brief Converts the values of the property array into a string and append to an output stream.
 *
 * @SINCE_1_1.28
 * @param[in] stream The output stream operator
 * @param[in] array The array to insert
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Property::Array& array);

/**
 * @brief Convert the key into a string and append to an output stream.
 *
 * @SINCE_1_2.7
 * @param [in] stream The output stream operator.
 * @param [in] key the key to convert
 * @return The output stream operator.
 */
DALI_CORE_API std::ostream& operator<<(std::ostream& stream, const Property::Key& key);

/**
 * @}
 */

} //namespace Dali
