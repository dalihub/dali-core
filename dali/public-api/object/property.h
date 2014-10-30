#ifndef __DALI_PROPERTY_H__
#define __DALI_PROPERTY_H__

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
#include <string>
#include <utility>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

class Handle;

/**
 * @brief An object + property pair.
 */
struct DALI_IMPORT_API Property
{
  /**
   * @brief A valid property index is zero or greater.
   */
  typedef int Index;

  static const int INVALID_INDEX; ///< -1 is not a valid property index
  static const int INVALID_COMPONENT_INDEX; ///< -1 is not a valid property index

  typedef std::vector< Index > IndexContainer; ///< A vector of property indices

  /**
   * @brief A value-type representing a property value.
   */
  class Value;

  /**
   * @brief A pair of property values.
   */
  typedef std::pair<std::string, Value> StringValuePair;

  /**
   * @brief A Map of property values.
   */
  class Map;

  /**
   * @brief An Array of property values.
   */
  typedef std::vector<Value> Array;

  /**
   * @brief The property types supported.
   */
  enum Type
  {
    NONE,            ///< No type

    BOOLEAN,         ///< A boolean type
    FLOAT,           ///< A float type
    INTEGER,         ///< An integer type
    UNSIGNED_INTEGER,///< An unsigned integer type
    VECTOR2,         ///< a vector array of size=2 with float precision
    VECTOR3,         ///< a vector array of size=3 with float precision
    VECTOR4,         ///< a vector array of size=4 with float precision
    MATRIX3,         ///< a 3x3 matrix
    MATRIX,          ///< a 4x4 matrix
    RECTANGLE,       ///< an integer array of size=4
    ROTATION,        ///< either a quaternion or an axis angle rotation
    STRING,          ///< A string type
    ARRAY,           ///< an array of Property::Value
    MAP,             ///< a string key to Property:value mapping
    TYPE_COUNT       ///< The number of supported property types
  };

  /**
   * @brief The access mode for custom properties
   */
  enum AccessMode
  {
    READ_ONLY,          ///< if the property is read-only
    READ_WRITE,         ///< If the property is read/writeable
    ANIMATABLE,         ///< If the property can be animated or constrained
    ACCESS_MODE_COUNT   ///< The number of access modes
  };


  /**
   * @brief Create a Property instance.
   *
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyIndex The index of a property.
   */
  Property( Handle& object, Property::Index propertyIndex );


  /**
   * @brief Create a Property instance.
   *
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyIndex The index of a property.
   * @param [in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4. -1 for main property (default is -1)
   */
  Property( Handle& object, Property::Index propertyIndex, int componentIndex );

  /**
   * @brief Create a Property instance.
   *
   * @note This performs a property index query and is therefore slower than
   * constructing a Property directly with the index.
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyName The property name.
   */
  Property( Handle& object, const std::string& propertyName );

  /**
   * @brief Create a Property instance.
   *
   * @note This performs a property index query and is therefore slower than
   * constructing a Property directly with the index.
   * @param [in] object A valid handle to the target object.
   * @param [in] propertyName The property name.
   * @param [in] componentIndex Index to a sub component of a property, for use with Vector2, Vector3 and Vector4. -1 for main property (default is -1)
   */
  Property( Handle& object, const std::string& propertyName, int componentIndex );

  /**
   * @brief Non-virtual destructor; Property is not intended as a base class.
   */
  ~Property();

  Handle& object; ///< A valid handle to the target object.

  Index propertyIndex; ///< The index of a property provided by object.

  int componentIndex; ///< Index of a property sub component, for use with Vector2, Vector3 and Vector4, -1 if using main property
};

} // namespace Dali

#endif // __DALI_PROPERTY_H__
