#ifndef __DALI_SCRIPTING_H__
#define __DALI_SCRIPTING_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/devel-api/animation/animation-data.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{

class Actor;

/**
 * @brief Utilities for scripting support.
 */
namespace Scripting
{

/**
 * @brief Structure which stores an enumeration and its string equivalent.
 */
struct StringEnum
{
  const char* string; ///< The string representation
  const int value;    ///< The enumeration value wrapped in int
};

/**
 * @brief Find the given enum index from the table
 *
 * @param[in]  value       The string equivalent (case-insensitive).
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 * @return     The index of the enumeration. If enumeration is not found, logs an error and returns tableCount.
 */
DALI_IMPORT_API unsigned int FindEnumIndex( const char* value, const StringEnum* table, unsigned int tableCount );

/**
 * @brief Chooses the appropriate enumeration for the provided string from the given table.
 *
 * @param[in]  value       The string equivalent (case-insensitive).
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 * @param[out] result      The enum value
 *
 * @return     True if the value was found from the table
 */
template< typename T >
bool GetEnumeration( const char* value, const StringEnum* table, unsigned int tableCount, T& result )
{
  bool retVal( false );
  if( table )
  {
    unsigned int index = FindEnumIndex( value, table, tableCount );
    // check to avoid crash, not asserting on purpose, error is logged instead
    if( index < tableCount )
    {
      result = static_cast<T>( table[ index ].value );
      retVal = true;
    }
  }
  return retVal;
}

/**
 * @brief Chooses the appropriate string for the provided enumeration from the given table.
 *
 * @param[in]  value       The enumeration.
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 *
 * @return     The equivalent enumeration for the given string. Will return NULL if the value does not exist
 *
 * @note The caller is NOT responsible for cleaning up the returned pointer as it is statically allocated.
 */
template< typename T >
const char* GetEnumerationName( T value, const StringEnum* table, unsigned int tableCount )
{
  if( table )
  {
    for ( unsigned int i = 0; i < tableCount; ++i )
    {
      if ( value == T(table[ i ].value) )
      {
        return table[ i ].string;
      }
    }
  }
  return NULL;
}

/**
 * @brief Chooses the appropriate string for the provided enumeration from the given table.
 * This is an optimised version that handles enumerations that start at 0 and are linear only.
 *
 * @param[in]  value       The enumeration.
 * @param[in]  table       A pointer to an array with the enumeration to string equivalents.
 * @param[in]  tableCount  Number of items in the array.
 *
 * @return     The equivalent enumeration for the given string. Will return NULL if the value does not exist
 *
 * @note The caller is NOT responsible for cleaning up the returned pointer as it is statically allocated.
 */
template< typename T >
const char * GetLinearEnumerationName( T value, const StringEnum* table, unsigned int tableCount )
{
  if ( table && ( value > 0 || value <= (int)tableCount ) )
  {
    return table[value].string;
  }
  return NULL;
}

/**
 * @brief Takes a string and returns the appropriate color mode.
 *
 * @param[in] value The input string
 * @return    The corresponding color-mode.
 */
DALI_IMPORT_API ColorMode GetColorMode( const std::string& value );

/**
 * @brief Takes a color mode and returns the appropriate string equivalent.
 *
 * @param[in] value The color mode
 * @return    The corresponding string.
 */
DALI_IMPORT_API std::string GetColorMode( ColorMode value );

/**
 * @brief Takes a string and returns the appropriate position inheritance mode.
 *
 * @param[in] value The input string
 * @return    The corresponding position-inheritance-mode.
 */
DALI_IMPORT_API PositionInheritanceMode GetPositionInheritanceMode( const std::string& value );

/**
 * @brief Takes a position inheritance mode and returns the string equivalent.
 *
 * @param[in] value The position-inheritance-mode.
 * @return    The corresponding string.
 */
DALI_IMPORT_API std::string GetPositionInheritanceMode( PositionInheritanceMode value );

/**
 * @brief Takes a string and returns the appropriate draw mode.
 *
 * @param[in] value The input string
 * @return    The corresponding draw-mode.
 */
DALI_IMPORT_API DrawMode::Type GetDrawMode( const std::string& value );

/**
 * @brief Takes a draw-mode and returns the string equivalent.
 *
 * @param[in] value The draw-mode.
 * @return    The corresponding string.
 */
DALI_IMPORT_API std::string GetDrawMode( DrawMode::Type value );

/**
 * @brief Takes a string and returns the appropriate anchor-point or parent-origin constant.
 *
 * @param[in] value The input string
 * @return    The corresponding anchor-point or parent-origin constant.
 */
DALI_IMPORT_API Vector3 GetAnchorConstant( const std::string& value );

/**
 * @brief Creates object with data from the property value map.
 *
 * @param[in] property The property value map with the following valid fields:
 * @code
 * "filename":       type std::string
 * "load-policy"     type std::string (enum)
 * "release-policy"  type std::string (enum)
 * "width"           type float
 * "height"          type float
 * "pixel-format"    type std::string (enum)
 * "fitting-mode"    type std::string (enum)
 * "sampling-mode"   type std::string (enum)
 * "orientation"     type bool
 * "type"            type std::string (FrameBufferImage|BufferImage|ResourceImage(default))
 * @endcode
 * Some fields are optional and some only pertain to a specific type.
 *
 * @return A pointer to a newly created object.
 */
DALI_IMPORT_API Image NewImage( const Property::Value& property );

/**
 * @brief Creates object with data from the property value map.
 *
 * @param[in] property The property value map with the following valid fields:
 * @code
 * // a program can be specified as string or a filename.
 * // some fields may be ignored depending on the geometry-type
 * "program":        type Map
 * {
 *   "vertex":                   type std::string
 *   "fragment":                 type std::string
 *   "vertex-prefix":            type std::string
 *   "fragment-prefix":          type std::string
 *   "text-vertex":              type std::string
 *   "text-fragment":            type std::string
 *   "vertex-filename":          type std::string
 *   "fragment-filename":        type std::string
 *   "vertex-prefix-filename":   type std::string
 *   "fragment-prefix-filename": type std::string
 *   "text-vertex-filename":     type std::string
 *   "text-fragment-filename":   type std::string
 *   "geometry-type":            type std::string (enum)
 *   "geometry-hints":           type std::string (enum)
 * }
 * // uniforms must be specified to be registered
 * "uUniform1":       type float,
 * "uUniform2":       type float, etc
 * @endcode
 *
 * @return A pointer to a newly created object.
 */
DALI_IMPORT_API ShaderEffect NewShaderEffect( const Property::Value& property );

/**
 * @brief Creates an actor with the date from the property value map.
 *
 * @param[in] map The property value map with the properties (and hierarchy) of the actor required
 *                 For example:
 * @code
 * {
 *   "type": "ImageActor",
 *   "image":
 *   {
 *     "filename":"my-image-path.png"
 *   },
 *   "actors":
 *   [
 *     {
 *       "type":"Actor",
 *       "position":[0,0,0]
 *     }
 *   ]
 * }
 * @endcode
 *
 * @return A handle to the newly created actor.
 */
DALI_IMPORT_API Actor NewActor( const Property::Map& map );

/**
 * @brief Creates a Property::Map from the actor provided.
 *
 * @param[in]  actor The base-actor from which a Property::Map should be created
 * @param[out] map This map is cleared and a property map of actor and its children is filled in
 */
DALI_IMPORT_API void CreatePropertyMap( Actor actor, Property::Map& map );

/**
 * @brief Creates a Property::Map from the image provided.
 *
 * @param[in]  image The image from which a Property::Map should be created
 * @param[out] map This map is cleared and a property map of the image is filled in
 */
DALI_IMPORT_API void CreatePropertyMap( Image image, Property::Map& map );

/**
 * @brief Creates description data required to create an Animation object from a property map.
 *
 * @param[in]  map The property value map containing the animation description
 * @param[out] outputAnimationData Resultant data retrieved from the property map is written here
 */
DALI_IMPORT_API void NewAnimation( const Property::Map& map, Dali::AnimationData& outputAnimationData );

} // namespace Scripting

} // namespace Dali

#endif // __DALI_SCRIPTING_H__
