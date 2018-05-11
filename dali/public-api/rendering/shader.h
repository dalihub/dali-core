#ifndef DALI_SHADER_H
#define DALI_SHADER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/handle.h>                // Dali::Handle
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
 *
 * @SINCE_1_1.43
 */
#define DALI_COMPOSE_SHADER(STR) #STR

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Shader;
}

/**
 * @brief Shaders allows custom vertex and color transformations in the GPU.
 *
 * @SINCE_1_1.43
 */
class DALI_CORE_API Shader : public Handle
{
public:

  /**
   * @brief Hints for rendering.
   * @SINCE_1_1.45
   */
  struct Hint
  {
    /**
     * @brief Enumeration for the hint value.
     * @SINCE_1_1.45
     */
    enum Value
    {
      NONE                     = 0x00, ///< No hints                                                                          @SINCE_1_1.45
      OUTPUT_IS_TRANSPARENT    = 0x01, ///< Might generate transparent alpha from opaque inputs                               @SINCE_1_1.45
      MODIFIES_GEOMETRY        = 0x02, ///< Might change position of vertices, this option disables any culling optimizations @SINCE_1_1.45
    };
  };

  /**
   * @brief Enumeration for instances of properties belonging to the Shader class.
   * @SINCE_1_1.43
   */
  struct Property
  {
    /**
     * @brief Enumeration for instances of properties belonging to the Shader class.
     * @SINCE_1_1.43
     */
    enum
    {
      /**
       * @brief Name: "program", Type: MAP.
       * @note The default value is empty.
       * @note Format: {"vertex":"","fragment":"",hints:"","vertexPrefix":"","fragmentPrefix":""}
       * @SINCE_1_1.43
       */
      PROGRAM = DEFAULT_OBJECT_PROPERTY_START_INDEX
    };
  };

  /**
   * @brief Creates Shader.
   *
   * @SINCE_1_1.43
   * @param[in] vertexShader Vertex shader code for the effect.
   * @param[in] fragmentShader Fragment Shader code for the effect.
   * @param[in] hints Hints to define the geometry of the rendered object
   * @return A handle to a shader effect
   */
  static Shader New( const std::string& vertexShader,
                     const std::string& fragmentShader,
                     Hint::Value hints = Hint::NONE );

  /**
   * @brief Default constructor, creates an empty handle.
   *
   * @SINCE_1_1.43
   */
  Shader();

  /**
   * @brief Destructor.
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_1_1.43
   */
  ~Shader();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_1.43
   * @param[in] handle A handle to a Shader object
   */
  Shader( const Shader& handle );

  /**
   * @brief Downcasts to a shader handle.
   * If not, a shader the returned shader handle is left uninitialized.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Shader handle or an uninitialized handle
   */
  static Shader DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Shader& operator=( const Shader& handle );

public:

  /**
   * @brief This constructor is used by Dali New() methods.
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] effect A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL Shader( Internal::Shader* effect );
};

} // namespace Dali

#endif // DALI_SHADER_H
