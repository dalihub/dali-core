#ifndef __DALI_PROPERTY_TYPES_H__
#define __DALI_PROPERTY_TYPES_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/extents.h>
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
 * @addtogroup dali_core_object
 * @{
 */

/**
 * @brief Template function instances for property getters.
 * @SINCE_1_0.0
 */
namespace PropertyTypes
{

/**
 * @brief Retrieves the name of a property type.
 *
 * @SINCE_1_0.0
 * @param[in] type The property type
 * @return The name of this type
 */
DALI_IMPORT_API const char* GetName(Property::Type type);

/**
 * @brief Retrieves an enumerated property type.
 *
 * New versions of this templated function must be defined for future types.
 * @SINCE_1_0.0
 * @return The property type
 */
template <typename T>
inline Property::Type Get()                   { return Property::NONE;  }
template <>
inline Property::Type Get<bool>()             { return Property::BOOLEAN;  }
template <>
inline Property::Type Get<float>()            { return Property::FLOAT;    }
template <>
inline Property::Type Get<int>()              { return Property::INTEGER;  }
template <>
inline Property::Type Get<Vector2>()          { return Property::VECTOR2;  }
template <>
inline Property::Type Get<Vector3>()          { return Property::VECTOR3;  }
template <>
inline Property::Type Get<Vector4>()          { return Property::VECTOR4;  }
template <>
inline Property::Type Get<Matrix3>()          { return Property::MATRIX3;  }
template <>
inline Property::Type Get<Matrix>()           { return Property::MATRIX;  }
template <>
inline Property::Type Get<AngleAxis>()        { return Property::ROTATION; } // Rotation has two representations
template <>
inline Property::Type Get<Quaternion>()       { return Property::ROTATION; } // Rotation has two representations
template <>
inline Property::Type Get<std::string>()      { return Property::STRING; }
template <>
inline Property::Type Get<Dali::Rect<int> >() { return Property::RECTANGLE; }
template <>
inline Property::Type Get<Property::Map>()    { return Property::MAP; }
template <>
inline Property::Type Get<Property::Array>()  { return Property::ARRAY; }
template <>
inline Property::Type Get<Extents>()          { return Property::EXTENTS; }


}; // namespace PropertyTypes

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_TYPES_H__
