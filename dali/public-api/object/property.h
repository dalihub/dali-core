#ifndef __DALI_PROPERTY_H__
#define __DALI_PROPERTY_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <string>
#include <utility>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

class Handle;

/**
 * @brief An object + property pair.
 * @SINCE_1_0.0
 */
struct DALI_IMPORT_API Property
{
  /**
   * @brief A valid property index is zero or greater.
   * @SINCE_1_0.0
   */
  typedef int Index;

  static const int INVALID_INDEX; ///< -1 is not a valid property index
  static const int INVALID_KEY;   ///< -1 is not a valid property key
  static const int INVALID_COMPONENT_INDEX; ///< -1 is not a valid property index

  typedef Dali::Vector< Index > IndexContainer; ///< A vector of property indices @SINCE_1_0.0

  /**
   * @brief A value-type representing a property value.
   */
  class Value;

  /**
   * @brief A Map of property values.
   */
  class Map;

  /**
   * @brief An Array of property values.
   */
  class Array;

  /**
   * @brief The property types supported.
   * @SINCE_1_0.0
   */
  enum Type
  {
    NONE,            ///< No type @SINCE_1_0.0

    BOOLEAN,         ///< A boolean type @SINCE_1_0.0
    FLOAT,           ///< A float type @SINCE_1_0.0
    INTEGER,         ///< An integer type @SINCE_1_0.0
    VECTOR2,         ///< a vector array of size=2 with float precision @SINCE_1_0.0
    VECTOR3,         ///< a vector array of size=3 with float precision @SINCE_1_0.0
    VECTOR4,         ///< a vector array of size=4 with float precision @SINCE_1_0.0
    MATRIX3,         ///< a 3x3 matrix @SINCE_1_0.0
    MATRIX,          ///< a 4x4 matrix @SINCE_1_0.0
    RECTANGLE,       ///< an integer array of size=4 @SINCE_1_0.0
    ROTATION,        ///< either a quaternion or an axis angle rotation @SINCE_1_0.0
    STRING,          ///< A string type @SINCE_1_0.0
    ARRAY,           ///< an array of Property::Value @SINCE_1_0.0
    MAP              ///< a string key to Property:value mapping @SINCE_1_0.0
  };

  /**
   * @brief The access mode for custom properties
   * @SINCE_1_0.0
   */
  enum AccessMode
  {
    READ_ONLY,          ///< if the property is read-only @SINCE_1_0.0
    READ_WRITE,         ///< If the property is read/writeable @SINCE_1_0.0
    ANIMATABLE,         ///< If the property can be animated or constrained @SINCE_1_0.0
    ACCESS_MODE_COUNT   ///< The number of access modes @SINCE_1_0.0
  };


  /**
   * @brief Create a Property instance.
   *
   * @SINCE_1_0.0
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyIndex The index of a property.
   */
  Property( Handle& object, Property::Index propertyIndex );


  /**
   * @brief Create a Property instance.
   *
   * @SINCE_1_0.0
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyIndex The index of a property.
   * @param [in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4. -1 for main property (default is -1)
   */
  Property( Handle& object, Property::Index propertyIndex, int componentIndex );

  /**
   * @brief Create a Property instance.
   *
   * @SINCE_1_0.0
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyName The property name.
   * @note This performs a property index query and is therefore slower than
   * constructing a Property directly with the index.
   */
  Property( Handle& object, const std::string& propertyName );

  /**
   * @brief Create a Property instance.
   *
   * @SINCE_1_0.0
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyName The property name.
   * @param [in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4. -1 for main property (default is -1)
   * @note This performs a property index query and is therefore slower than
   * constructing a Property directly with the index.
   */
  Property( Handle& object, const std::string& propertyName, int componentIndex );

  /**
   * @brief Non-virtual destructor; Property is not intended as a base class.
   * @SINCE_1_0.0
   */
  ~Property();

  Handle& object; ///< A valid handle to the target object.

  Index propertyIndex; ///< The index of a property provided by object.

  int componentIndex; ///< Index of a property sub component, for use with Vector2, Vector3 and Vector4, -1 if using main property
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PROPERTY_H__
