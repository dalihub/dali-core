#ifndef __DALI_PROPERTY_TYPES_H__
#define __DALI_PROPERTY_TYPES_H__

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
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/angle-axis.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

/**
 * @brief Template function instances for property getters.
 */
namespace PropertyTypes
{

/**
 * @brief Retrieve the name of a property type.
 *
 * @param [in] type The property type.
 * @return The name of this type.
 */
DALI_IMPORT_API const char* GetName(Property::Type type);

/**
 * @brief Retrieve an enumerated property type.
 *
 * New versions of this templated function must be defined for future types.
 * @return The property type.
 */
template <typename T> Property::Type Get();
template <>           Property::Type Get<bool>();
template <>           Property::Type Get<float>();
template <>           Property::Type Get<int>();
template <>           Property::Type Get<unsigned int>();
template <>           Property::Type Get<Vector2>();
template <>           Property::Type Get<Vector3>();
template <>           Property::Type Get<Vector4>();
template <>           Property::Type Get<Matrix3>();
template <>           Property::Type Get<Matrix>();
template <>           Property::Type Get<AngleAxis>();
template <>           Property::Type Get<Quaternion>();
template <>           Property::Type Get<std::string>();
template <>           Property::Type Get<Rect<int> >();
template <>           Property::Type Get<Property::Map >();
template <>           Property::Type Get<Property::Array >();

}; // namespace PropertyTypes

} // namespace Dali

#endif // __DALI_PROPERTY_TYPES_H__
