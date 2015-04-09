#ifndef DALI_SHADER_H
#define DALI_SHADER_H

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

// EXTERNAL INCLUDES
#include <string> // std::string

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX

/**
 * @brief DALI_COMPOSE_SHADER macro provides a convenient way to write shader source code.
 *
 * We normally use double quotation marks to write a string such as "Hello World".
 * However many symbols are needed to add multiple lines of string.
 * We don't need to write quotation marks using this macro at every line.
 *
 * [An example of double quotation marks usage]
 * <pre>
 * const string FRAGMENT_SHADER_SOURCE = \
 * "  void main()\n"
 * "  {\n"
 * "    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n"
 * "  }\n";
 * </pre><br/>
 * [An example of DALI_COMPOSE_SHADER usage]
 * <pre>
 * const string VERTEX_SHADER_SOURCE = DALI_COMPOSE_SHADER (
 *   void main()
 *   {
 *     gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);
 *     vTexCoord = aTexCoord;
 *   }
 * );
 * </pre>
 */
#define DALI_COMPOSE_SHADER(STR) #STR

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Shader;
}

/**
 * @brief Shaders allows custom vertex and color transformations in the GPU
 */
class DALI_IMPORT_API Shader : public Handle
{
public:
  /**
   * @brief Hints for rendering/subdividing geometry.
   */
  enum ShaderHints
  {
    HINT_NONE                     = 0x00, ///< no hints
    HINT_REQUIRES_SELF_DEPTH_TEST = 0x01, ///< Expects depth testing enabled
    HINT_OUTPUT_IS_TRANSPARENT    = 0x02, ///< Might generate transparent alpha from opaque inputs
    HINT_OUTPUT_IS_OPAQUE         = 0x04, ///< Outputs opaque colors even if the inputs are transparent
    HINT_MODIFIES_GEOMETRY        = 0x08, ///< Might change position of vertices,
                                          ///< this option disables any culling optimizations
  };

  /**
   * @brief An enumeration of properties belonging to the Shader class.
   */
  struct Property
  {
    enum
    {
      PROGRAM = DEFAULT_OBJECT_PROPERTY_START_INDEX,  ///< name "program",      type MAP; {"vertex-prefix":"","fragment-prefix":"","vertex":"","fragment":""}
      SHADER_HINTS,                                   ///< name "shader-hints", type UNSIGNED_INTEGER; (bitfield) values from enum GeometryHints
    };
  };

  /**
   * @brief Create Shader.
   *
   * @param vertexShader code for the effect. If you pass in an empty string, the default version will be used
   * @param fragmentShader code for the effect. If you pass in an empty string, the default version will be used
   * @param hints GeometryHints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static Shader New( const std::string& vertexShader,
                     const std::string& fragmentShader,
                     ShaderHints hints = ShaderHints(HINT_NONE) );

  /**
   * @brief Default constructor, creates an empty handle
   */
  Shader();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Shader();

  /**
   * @brief Copy constructor
   *
   * @param handle A handle to a Shader object
   */
  Shader( const Shader& handle );

  /**
   * @brief Downcast to a shader handle.
   *
   * If not a shader the returned shader handle is left uninitialized.
   * @param[in] handle to an object
   * @return shader handle or an uninitialized handle
   */
  static Shader DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Shader& operator=( const Shader& handle );

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   * @param [in] effect A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL Shader( Internal::Shader* effect );
};

} // namespace Dali

#endif // DALI_SHADER_H
