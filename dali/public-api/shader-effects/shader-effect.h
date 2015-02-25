#ifndef __DALI_SHADER_EFFECT_H__
#define __DALI_SHADER_EFFECT_H__

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
#include <dali/public-api/animation/active-constraint-declarations.h>
#include <dali/public-api/object/handle.h>

namespace Dali
{

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
 *     gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
 *     vTexCoord = aTexCoord;
 *   }
 * );
 */
#define DALI_COMPOSE_SHADER(STR) #STR

class Constraint;
class Image;
struct Vector2;
struct Vector3;
struct Vector4;

namespace Internal DALI_INTERNAL
{
class ShaderEffect;
}

/**
 * @brief GeometryType determines how geometry is shaped.
 */
enum GeometryType
{
  GEOMETRY_TYPE_IMAGE = 0x01,         ///< image, with flat color or texture
  GEOMETRY_TYPE_TEXT = 0x02,          ///< text, with flat color or texture
  GEOMETRY_TYPE_UNTEXTURED_MESH = 0x04,///< Complex meshes, with flat color
  GEOMETRY_TYPE_TEXTURED_MESH = 0x08, ///< Complex meshes, with texture
  GEOMETRY_TYPE_LAST = 0x10
};

/**
 * @brief Shader effects provide a visual effect for actors.
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
 *   gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
 *   vTexCoord = aTexCoord;
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
 * and for text:
 * <pre>
 * \#extension GL_OES_standard_derivatives : enable
 * uniform   mediump sampler2D sTexture;
 * uniform   lowp    vec4      uColor;
 * uniform   lowp    vec4      uTextColor;
 * uniform   mediump float     uSmoothing;
 * varying   mediump vec2      vTexCoord;
 * </pre>
 * and the custom shader is expected to output the fragment color.
 * The basic fragment shader for images would contain:
 * <pre>
 * void main()
 * {
 *   gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;
 * }
 * </pre>
 * and for text::
 * <pre>
 *  void main()
 *  {
 *    // sample distance field
 *    mediump float distance = texture2D(sTexture, vTexCoord).a;
 *    mediump float smoothWidth = fwidth(distance);
 *    // set fragment color
 *    lowp vec4 color = uTextColor;
 *    // adjust alpha by sampled distance
 *    color.a *= smoothstep(uSmoothing - smoothWidth, uSmoothing + smoothWidth, distance);
 *    // fragment color multiplied with uColor.
 *    glFragColor = color * uColor;
 *  }
 * </pre>
 * <BR>
 * <B>
 * Note: In order for fade and color animations to work, the fragment shader needs to multiply the fragment color
 * with the uniform color "uColor" of the node
 * </B>
 */
class DALI_IMPORT_API ShaderEffect : public Handle
{
public:
  /**
   * @brief The Extension class is a base class for objects that can be attached to the
   * ShaderEffects as extensions.
   *
   * Extensions are useful to create pimpled implementations of custom shaders.
   * The shader effect will hold an intrusive pointer to the extension.
   */
  class Extension : public RefObject
  {
  protected:
    /**
     * @brief Disable default constructor. This a base class is not meant to be initialised on its own.
     */
    Extension();

    /**
     * @brief Virtual destructor.
     */
    virtual ~Extension();
  };

  // Default Properties
  /* Grid Density defines the spacing of vertex coordinates in world units.
   * ie a larger actor will have more grids at the same spacing.
   *
   *  +---+---+         +---+---+---+
   *  |   |   |         |   |   |   |
   *  +---+---+         +---+---+---+
   *  |   |   |         |   |   |   |
   *  +---+---+         +---+---+---+
   *                    |   |   |   |
   *                    +---+---+---+
   */
  static const Property::Index GRID_DENSITY;       ///< name "grid-density",   type FLOAT
  static const Property::Index IMAGE;              ///< name "image",          type MAP; {"filename":"", "load-policy":...}
  static const Property::Index PROGRAM;            ///< name "program",        type MAP; {"vertex-prefix":"","fragment-prefix":"","vertex":"","fragment":""}
  static const Property::Index GEOMETRY_HINTS;     ///< name "geometry-hints", type INT (bitfield) values from enum GeometryHints

  static const float DEFAULT_GRID_DENSITY;         ///< The default density is 40 pixels

  /**
   * @brief Hints for rendering/subdividing geometry.
   */
  enum GeometryHints
  {
    HINT_NONE           = 0x00,   ///< no hints
    HINT_GRID_X         = 0x01,   ///< Geometry must be subdivided in X
    HINT_GRID_Y         = 0x02,   ///< Geometry must be subdivided in Y
    HINT_GRID           = (HINT_GRID_X | HINT_GRID_Y),
    HINT_DEPTH_BUFFER   = 0x04,   ///< Needs depth buffering turned on
    HINT_BLENDING       = 0x08,   ///< Notifies the actor to use blending even if it's fully opaque. Needs actor's blending set to BlendingMode::AUTO
    HINT_DOESNT_MODIFY_GEOMETRY = 0x10 ///< Notifies that the vertex shader will not change geometry (enables bounding box culling)
  };

  /**
   * @brief Coordinate type of the shader uniform.
   *
   * Viewport coordinate types will convert from viewport to view space.
   * Use this coordinate type if your are doing a transformation in view space.
   * The texture coordinate type converts a value in actor local space to texture coodinates.
   * This is useful for pixel shaders and accounts for texture atlas.
   */
  enum UniformCoordinateType
  {
    COORDINATE_TYPE_DEFAULT,           ///< Default, No transformation to be applied
    COORDINATE_TYPE_VIEWPORT_POSITION, ///< The uniform is a position vector in viewport coordinates that needs to be converted to GL view space coordinates.
    COORDINATE_TYPE_VIEWPORT_DIRECTION ///< The uniform is a directional vector in viewport coordinates that needs to be converted to GL view space coordinates.
  };

  /**
   * @brief Create an empty ShaderEffect.
   *
   * This can be initialised with ShaderEffect::New(...)
   */
  ShaderEffect();

  /**
   * @brief Create ShaderEffect.
   *
   * @param vertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param fragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param type GeometryType to define the shape of the geometry
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect New( const std::string& vertexShader,
                           const std::string& fragmentShader,
                           GeometryType type = GeometryType(GEOMETRY_TYPE_IMAGE),
                           GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Create ShaderEffect.
   * @param vertexShaderPrefix code for the effect. It will be inserted before the default uniforms (ideal for \#defines)
   * @param vertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param fragmentShaderPrefix code for the effect. It will be inserted before the default uniforms (ideal for \#defines)
   * @param fragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param type GeometryType to define the shape of the geometry
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect NewWithPrefix(const std::string& vertexShaderPrefix,
                                    const std::string& vertexShader,
                                    const std::string& fragmentShaderPrefix,
                                    const std::string& fragmentShader,
                                    GeometryType type = GeometryType(GEOMETRY_TYPE_IMAGE),
                                    GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Create ShaderEffect.
   * @param imageVertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param imageFragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param textVertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param textFragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect New( const std::string& imageVertexShader,
                           const std::string& imageFragmentShader,
                           const std::string& textVertexShader,
                           const std::string& textFragmentShader,
                           GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Create ShaderEffect.
   * @param imageVertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param imageFragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param textVertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param textFragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param texturedMeshVertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param texturedMeshFragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param meshVertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param meshFragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static ShaderEffect New( const std::string& imageVertexShader,
                           const std::string& imageFragmentShader,
                           const std::string& textVertexShader,
                           const std::string& textFragmentShader,
                           const std::string& texturedMeshVertexShader,
                           const std::string& texturedMeshFragmentShader,
                           const std::string& meshVertexShader,
                           const std::string& meshFragmentShader,
                           GeometryHints hints = GeometryHints(HINT_NONE) );

  /**
   * @brief Downcast an Object handle to ShaderEffect.
   *
   * If handle points to a ShaderEffect the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ShaderEffect object or an uninitialized handle
   */
  static ShaderEffect DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ShaderEffect();

  /**
   * @brief Copy constructor
   *
   * @param object A reference to a ShaderEffect object
   */
  ShaderEffect(const ShaderEffect& object);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
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
   * @param[in] image to use as effect texture
   */
  void SetEffectImage( Image image );

  /**
   * @brief Set a uniform value.
   * This will register a property of type Property::FLOAT; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   */
  void SetUniform( const std::string& name,
                   float value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::VECTOR2; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   */
  void SetUniform( const std::string& name,
                   Vector2 value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::VECTOR3; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   */
  void SetUniform( const std::string& name,
                   Vector3 value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::VECTOR4; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   */
  void SetUniform( const std::string& name,
                   Vector4 value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::MATRIX; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   */
  void SetUniform( const std::string& name,
                   const Matrix& value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Set a uniform value.
   *
   * This will register a property of type Property::MATRIX3; see Object::RegisterProperty() for more details.
   * If name matches a uniform in the shader source, this value will be uploaded when rendering.
   * @pre Either the property name is not in use, or a property exists with the correct name & type.
   * @param name The name of the uniform.
   * @param value The value to to set.
   * @param uniformCoordinateType The coordinate type of the uniform.
   */
  void SetUniform( const std::string& name,
                   const Matrix3& value,
                   UniformCoordinateType uniformCoordinateType = UniformCoordinateType(COORDINATE_TYPE_DEFAULT) );

  /**
   * @brief Attach an extension object.
   *
   * This object is reference counted and will be automatically deleted.
   * This object can be retrieved back with the GetExtension function.
   * @param object Pointer to a Extension.
   * @pre extension is not NULL
   */
  void AttachExtension( Extension *object );

  /**
   * @brief Retrieve the attached extension object.
   *
   * This object can be set with the AttachExtension function.
   * @return implementation Pointer to a Extension.
   * @pre An extension needs to be attached previously.
   */
  Extension& GetExtension();

  /**
   * @brief Retrieve the attached extension object.
   *
   * This object can be set with the AttachExtension function.
   * @return implementation Pointer to a Extension.
   * @pre An extension needs to be attached previously.
   */
  const Extension& GetExtension() const;


public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   * @param [in] effect A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL ShaderEffect(Internal::ShaderEffect* effect);
};

} // namespace Dali

#endif // __DALI_SHADER_EFFECT_H__
