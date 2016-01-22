#ifndef __DALI_SHADER_EFFECT_H__
#define __DALI_SHADER_EFFECT_H__

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
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{
/**
 * @addtogroup dali_core_shader_effects
 * @{
 */

/**
 * @brief DALI_COMPOSE_SHADER macro provides a convenient way to write shader source code.
 *
 * We normally use double quotation marks to write a string such as "Hello World".
 * However many symbols are needed to add multiple lines of string.
 * We don't need to write quotation marks using this macro at every line.
 *
 * [An example of double quotation marks usage]
 * const string FRAGMENT_SHADER_SOURCE = \
 * "  void main()\n"
 * "  {\n"
 * "    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
 * "  }\n";
 *
 * [An example of DALI_COMPOSE_SHADER usage]
 * const string VERTEX_SHADER_SOURCE = DALI_COMPOSE_SHADER (
 *   void main()
 *   {
 *     gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy, 0.0, 1.0);
 *     vTexCoord = mix( uTextureRect.xy, uTextureRect.zw, aPosition + vec2(0.5) );
 *   }
 * );
 * @SINCE_1_0.0
 */
#define DALI_COMPOSE_SHADER(STR) #STR

class Image;
struct Vector2;
struct Vector3;
struct Vector4;

namespace Internal DALI_INTERNAL
{
class ShaderEffect;
}

/**
 * @deprecated DALi 1.0.47
 *
 * @brief Shader effects provide a visual effect for image actors.
 *
 * For a Custom shader you can provide the vertex and fragment shader code as strings.
 * These shader snippets get concatenated with the default attributes and uniforms.
 * For a vertex shader this part contains the following code:
 * <pre>
 * precision highp float;
 * attribute vec3  aPosition;
 * attribute vec2  aTexCoord;
 * uniform   mat4  uMvpMatrix;
 * uniform   mat4  uModelMatrix;
 * uniform   mat4  uViewMatrix;
 * uniform   mat4  uModelView;
 * uniform   mat3  uNormalMatrix;
 * uniform   mat4  uProjection;
 * uniform   vec4  uColor;
 * varying   vec2  vTexCoord;
 * </pre>
 * The custom shader part is expected to output the vertex position and texture coordinate.
 * A basic custom vertex shader would contain the following code:
 * <pre>
 * void main()
 * {
 *     gl_Position = uMvpMatrix * vec4(aPosition*uSize.xy, 0.0, 1.0);
 *     vTexCoord = mix( uTextureRect.xy, uTextureRect.zw, aPosition + vec2(0.5) );
 * }
 * </pre>
 * For fragment shader the default part for images contains the following code:
 * <pre>
 * precision mediump float;
 * uniform   sampler2D sTexture;
 * uniform   sampler2D sEffect;
 * uniform   vec4      uColor;
 * varying   vec2      vTexCoord;
 * </pre>
 * <BR>
 * <B>
 * Note: In order for fade and color animations to work, the fragment shader needs to multiply the fragment color
 * with the uniform color "uColor" of the node
 * </B>
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API ShaderEffect : public Handle
{
public:

  // Default Properties
  /**
   * @brief An enumeration of properties belonging to the ShaderEffect class.
   * Grid Density defines the spacing of vertex coordinates in world units.
   * ie a larger actor will have more grids at the same spacing.
   *
   *  +---+---+         +---+---+---+
   *  |   |   |         |   |   |   |
   *  +---+---+         +---+---+---+
   *  |   |   |         |   |   |   |
   *  +---+---+         +---+---+---+
   *                    |   |   |   |
   *                    +---+---+---+
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      GRID_DENSITY = DEFAULT_ACTOR_PROPERTY_START_INDEX, ///< name "gridDensity",    type float @SINCE_1_0.0
      IMAGE,                                             ///< name "image",          type Map {"filename":"", "loadPolicy":...} @SINCE_1_0.0
      PROGRAM,                                           ///< name "program",        type Map {"vertexPrefix":"","fragmentPrefix":"","vertex":"","fragment":""} @SINCE_1_0.0
      GEOMETRY_HINTS                                     ///< name "geometryHints",  type int (bitfield) values from enum GeometryHints @SINCE_1_0.0
    };
  };

  static const float DEFAULT_GRID_DENSITY;              ///< The default density is 40 pixels

  /**
   * @brief Hints for rendering/subdividing geometry.
   * @SINCE_1_0.0
   */
  enum GeometryHints
  {
    HINT_NONE           = 0x00,   ///< no hints @SINCE_1_0.0
    HINT_GRID_X         = 0x01,   ///< Geometry must be subdivided in X @SINCE_1_0.0
    HINT_GRID_Y         = 0x02,   ///< Geometry must be subdivided in Y @SINCE_1_0.0
    HINT_GRID           = (HINT_GRID_X | HINT_GRID_Y),
    HINT_DEPTH_BUFFER   = 0x04,   ///< Needs depth buffering turned on @SINCE_1_0.0
    HINT_BLENDING       = 0x08,   ///< Notifies the actor to use blending even if it's fully opaque. Needs actor's blending set to BlendingMode::AUTO @SINCE_1_0.0
    HINT_DOESNT_MODIFY_GEOMETRY = 0x10 ///< Notifies that the vertex shader will not change geometry (enables bounding box culling) @SINCE_1_0.0
  };

  /**
   * @brief Coordinate type of the shader uniform.
   *
   * Viewport coordinate types will convert from viewport to view space.
   * Use this coordinate type if your are doing a transformation in view space.
   * The texture coordinate type converts a value in actor local space to texture coodinates.
   * This is useful for pixel shaders and accounts for texture atlas.
   * @SINCE_1_0.0
   */
  enum UniformCoordinateType
  {
    COORDINATE_TYPE_DEFAULT,           ///< Default, No transformation to be applied @SINCE_1_0.0
    COORDINATE_TYPE_VIEWPORT_POSITION, ///< @deprecated Dali 1.1.11 The uniform is a position vector in viewport coordinates that needs to be converted to GL view space coordinates. @SINCE_1_0.0
    COORDINATE_TYPE_VIEWPORT_DIRECTION ///< @deprecated Dali 1.1.11 The uniform is a directional vector in viewport coordinates that needs to be converted to GL view space coordinates. @SINCE_1_0.0
  };

  /**
   * @brief Create an empty ShaderEffect.
   *
   * This can be initialised with ShaderEffect::New(...)
   * @SINCE_1_0.0
   */
  ShaderEffect();

  /**
   * @brief Create ShaderEffect.
   *
   * @SINCE_1_0.0
   * @param vertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param fragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect New( const std::string& vertexShader,
                           const std::string& fragmentShader,
                           GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Create ShaderEffect.
   * @SINCE_1_0.0
   * @param vertexShaderPrefix code for the effect. It will be inserted before the default uniforms (ideal for \#defines)
   * @param vertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param fragmentShaderPrefix code for the effect. It will be inserted before the default uniforms (ideal for \#defines)
   * @param fragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect NewWithPrefix(const std::string& vertexShaderPrefix,
                                    const std::string& vertexShader,
                                    const std::string& fragmentShaderPrefix,
                                    const std::string& fragmentShader,
                                    GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Downcast an Object handle to ShaderEffect.
   *
   * If handle points to a ShaderEffect the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a ShaderEffect object or an uninitialized handle
   */
  static ShaderEffect DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ShaderEffect();

  /**
   * @brief Copy constructor
   *
   * @SINCE_1_0.0
   * @param object A reference to a ShaderEffect object
   */
  ShaderEffect(const ShaderEffect& object);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  ShaderEffect& operator=(const ShaderEffect& rhs);

  /**
   * @brief Sets image for using as effect texture.
   *
   * This image texture will be bound to the "sEffect" sampler
   * so it can be used in fragment shader for effects
   *
   * @SINCE_1_0.0
   * @param[in] image to use as effect texture
   */
  void SetEffectImage( Image image );

  /**
   * @brief Set a uniform value.
   * This will register a property of type Property::FLOAT; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @SINCE_1_0.0
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   */
  void SetUniform( const std::string& name,
                   float value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::VECTOR2; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @SINCE_1_0.0
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   */
  void SetUniform( const std::string& name,
                   Vector2 value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::VECTOR3; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @SINCE_1_0.0
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   */
  void SetUniform( const std::string& name,
                   Vector3 value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::VECTOR4; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @SINCE_1_0.0
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   */
  void SetUniform( const std::string& name,
                   Vector4 value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::MATRIX; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @SINCE_1_0.0
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   */
  void SetUniform( const std::string& name,
                   const Matrix& value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::MATRIX3; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @SINCE_1_0.0
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   */
  void SetUniform( const std::string& name,
                   const Matrix3& value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   * @SINCE_1_0.0
   * @param [in] effect A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL ShaderEffect(Internal::ShaderEffect* effect);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_SHADER_EFFECT_H__
