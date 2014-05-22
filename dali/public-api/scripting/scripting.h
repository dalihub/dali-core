#ifndef __DALI_SCRIPTING_H__
#define __DALI_SCRIPTING_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/actors/draw-mode.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/object/property-value.h>

namespace Dali DALI_IMPORT_API
{

class Actor;

/**
 * @brief Utilities for scripting support.
 */
namespace Scripting
{

/**
 * @brief Permissive comparison for string enums.
 *
 * Case insensitive and ignores '_', '-' in either string when comparing.
 * If both strings are empty return true; ie like if(a==b) but not like a.compare(b)
 * @param[in] input The input string
 * @param[in] enumString The enum string
 * @return true if the strings are equal as defined above
 */
bool CompareEnums(const std::string& input, const std::string& enumString);

/**
 * @brief Set the value if strings pass a permissive compare.
 *
 * @param[in] a The input string
 * @param[in] b The input string
 * @param[in] set The variable to set
 * @param[in] value The value to set
 * @return true if the strings pass the permissve compare
 */
template <typename T>
bool SetIfEqual(const std::string& a, const std::string& b, T& set, T value)
{
  if( CompareEnums(a, b ) )
  {
    set = value;
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * @brief Takes a string and returns the appropriate color mode.
 *
 * @param[in] value The input string
 * @return The corresponding color-mode.
 */
ColorMode GetColorMode( const std::string& value );

/**
 * @brief Takes a color mode and returns the appropriate string equivalent.
 *
 * @param[in] value The color mode
 * @return The corresponding string.
 */
const std::string& GetColorMode( ColorMode value );

/**
 * @brief Takes a string and returns the appropriate position inheritance mode.
 *
 * @param[in] value The input string
 * @return The corresponding position-inheritance-mode.
 */
PositionInheritanceMode GetPositionInheritanceMode( const std::string& value );

/**
 * @brief Takes a position inheritance mode and returns the string equivalent.
 *
 * @param[in] value The position-inheritance-mode.
 * @return The corresponding string.
 */
const std::string& GetPositionInheritanceMode( PositionInheritanceMode value );

/**
 * @brief Takes a string and returns the appropriate draw mode.
 *
 * @param[in] value The input string
 * @return The corresponding draw-mode.
 */
DrawMode::Type GetDrawMode( const std::string& value );

/**
 * @brief Takes a draw-mode and returns the string equivalent.
 *
 * @param[in] value The draw-mode.
 * @return The corresponding string.
 */
const std::string& GetDrawMode( DrawMode::Type value );

/**
 * @brief Takes a string and returns the appropriate anchor-point or parent-origin constant.
 *
 * @param[in] value The input string
 * @return The corresponding anchor-point or parent-origin constant.
 */
Vector3 GetAnchorConstant( const std::string& value );

/**
 * @brief Creates object with data from the property value map.
 *
 * @param[in] map The property value map with the following valid fields:
 * @code
 * "filename":       type STRING
 * "load-policy"     type STRING (enum)
 * "release-policy"  type STRING (enum)
 * "width"           type FLOAT
 * "height"          type FLOAT
 * "pixel-format"    type STRING (enum)
 * "scaling-mode"    type STRING (enum)
 * "crop"            type VECTOR4
 * "type"            type STRING (FrameBufferImage|BitMapImage|Image(default))
 * @endcode
 * Some fields are optional and some only pertain to a specific type.
 *
 * @return a pointer to a newly created object.
 */
Image NewImage( const Property::Value& map );

/**
 * @brief Creates object with data from the property value map.
 *
 * @param[in] map The property value map with the following valid fields:
 * @code
 * // a program can be specified as string or a filename.
 * // some fields may be ignored depending on the geometry-type
 * "program":        type MAP
 * {
 *   "vertex":                  type STRING
 *   "fragment":                type STRING
 *   "vertex-prefix":           type STRING
 *   "fragment-prefix":         type STRING
 *   "text-vertex":             type STRING
 *   "text-fragment":           type STRING
 *   "vertex-filename":         type STRING
 *   "fragment-filename":       type STRING
 *   "vertex-prefix-filename":  type STRING
 *   "fragment-prefix-filename":type STRING
 *   "text-vertex-filename":    type STRING
 *   "text-fragment-filename":  type STRING
 *   "geometry-type":           type STRING (enum)
 *   "geometry-hints":          type STRING (enum)
 * }
 * // uniforms must be specified to be registered
 * "uUniform1":       type FLOAT,
 * "uUniform2":       type FLOAT,  etc
 * @endcode
 *
 * @return a pointer to a newly created object.
 */
ShaderEffect NewShaderEffect( const Property::Value& map );

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
 * @return Handle to the newly created actor.
 */
Actor NewActor( const Property::Map& map );

/**
 * @brief Creates a Property::Map from the actor provided.
 *
 * @param[in] actor The base-actor from which a Property::Map should be created
 * @param[out] map This map is cleared and a property map of actor and its children is filled in
 */
void CreatePropertyMap( Actor actor, Property::Map& map );

/**
 * @brief Creates a Property::Map from the image provided.
 *
 * @param[in] image The image from which a Property::Map should be created
 * @param[out] map This map is cleared and a property map of the image is filled in
 */
void CreatePropertyMap( Image image, Property::Map& map );

}

} // namespace Dali

#endif // __DALI_SCRIPTING_H__
