#ifndef DALI_MATERIAL_H
#define DALI_MATERIAL_H

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
#include <cstddef> // std::size_t
#include <string> // std::string

// INTERNAL INCLUDES
#include <dali/public-api/actors/blending.h> // Dali::BlendingMode, Dali::BlendingEquation, Dali::BlendingFactor
#include <dali/public-api/images/image.h> // Dali::Image
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX
#include <dali/public-api/shader-effects/sampler.h> // Dali::Sampler
#include <dali/public-api/shader-effects/shader.h> // Dali::Shader

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Material;
}

/**
 * @brief Material is a handle to an object that specifies the visual properties of the renderer.
 */
class DALI_IMPORT_API Material : public Handle
{
public:

  /**
   * @brief Set face culling mode.
   */
  enum FaceCullingMode
  {
    NONE,                     ///< None of the faces should be culled
    CULL_BACK,                ///< Cull back face, back face should never be shown
    CULL_BACK_HINT,           ///< Cull back face hinting, will still display correctly if no culling is done
    CULL_FRONT,               ///< Cull front face, back face should never be shown
    CULL_FRONT_HINT,          ///< Cull front face hinting, will still display correctly if no culling is done
    CULL_BACK_AND_FRONT,      ///< Cull back and front faces, if the geometry is composed of triangles none of the faces will be shown
    CULL_BACK_AND_FRONT_HINT, ///< Cull back and front hint, will still display correctly if no culling is done
  };

  /**
   * @brief An enumeration of properties belonging to the Material class.
   */
  struct Property
  {
    enum
    {
      COLOR = DEFAULT_OBJECT_PROPERTY_START_INDEX,  ///< name "color",                          type VECTOR4
      FACE_CULLING_MODE,                            ///< name "face-culling-mode",              type STRING
      BLENDING_MODE,                                ///< name "blending-mode",                  type STRING
      BLEND_EQUATION_RGB,                           ///< name "blend-equation-rgb",             type STRING
      BLEND_EQUATION_ALPHA,                         ///< name "blend-equation-alpha",           type STRING
      BLENDING_SRC_FACTOR_RGB,                      ///< name "source-blend-factor-rgb",        type STRING
      BLENDING_DEST_FACTOR_RGB,                     ///< name "destination-blend-factor-rgb",   type STRING
      BLENDING_SRC_FACTOR_ALPHA,                    ///< name "source-blend-factor-alpha",      type STRING
      BLENDING_DEST_FACTOR_ALPHA,                   ///< name "destination-blend-factor-alpha", type STRING
      BLEND_COLOR,                                  ///< name "blend-color",                    type VECTOR4
    };
  };

  /// @name Uniform Mapping
  /** @{ */
       ///< property "COLOR", uniform "uMaterialColor"
  /** @} */

  /**
   * @brief Creates a new Material object
   *
   * @return A handle to a newly allocated Material
   */
  static Material New( Shader shader );

  /**
   * @brief Default constructor, creates an empty handle
   */
  Material();

  /**
   * @brief Destructor
   */
  ~Material();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  Material( const Material& handle );

  /**
   * @brief Downcast to a material handle.
   *
   * If handle is not a material, the returned handle is left uninitialized.
   * @param[in] handle to an object
   * @return material handle or an uninitialized handle
   */
  static Material DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   */
  Material& operator=( const Material& handle );

  /**
   * @brief Sets the Shader used by this material
   *
   * @param[in] shader Handle to a shader
   */
  void SetShader( Shader& shader );

  /**
   * @brief Gets the shader used by this material
   *
   * @return The shader used by the material
   */
  Shader GetShader() const;

  /**
   * @brief Add a sampler to this material
   *
   * param[in] sampler The sampler to add to this material
   */
  void AddSampler( Sampler& sampler );

  /**
   * @brief Get the number of samplers
   *
   * @return The number of samplers
   */
  std::size_t GetNumberOfSamplers() const;

  /**
   * @brief Remove a sampler
   *
   * The index must be between 0 and GetNumberOfSamplers()-1
   *
   * @param[in] index The index of the sampler to remove
   */
  void RemoveSampler( std::size_t index );

  /**
   * @brief Get the sampler at the given index for this material
   *
   * The index must be between 0 and GetNumberOfSamplers()-1
   *
   * @param[in] index The index of the sampler
   * @return The sampler at that index
   */
  Sampler GetSamplerAt( unsigned int index ) const;

  /**
   * @brief Set the culling mode for this material
   *
   * Calling this function sets the properties CULLING_MODE
   *
   * @param[in] cullingMode The culling mode for this material
   */
  void SetFaceCullingMode( FaceCullingMode cullingMode );

  /**
   * @brief Sets the blending mode.
   *
   * Possible values are: BlendingMode::OFF, BlendingMode::AUTO and BlendingMode::ON. Default is BlendingMode::AUTO.
   *
   * If blending is disabled (BlendingMode::OFF) fade in and fade out animations do not work.
   *
   * <ul>
   *   <li> \e OFF Blending is disabled.
   *   <li> \e AUTO Blending is enabled only if the renderable actor has alpha channel.
   *   <li> \e ON Blending is enabled.
   * </ul>
   *
   * @param[in] mode The blending mode.
   */
  void SetBlendMode( BlendingMode::Type mode );

  /**
   * @brief Retrieves the blending mode.
   *
   * @return The blending mode, one of BlendingMode::OFF, BlendingMode::AUTO or BlendingMode::ON.
   */
  BlendingMode::Type GetBlendMode() const;

  /**
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @param[in] srcFactorRgba Specifies how the red, green, blue, and alpha source blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE.
   *
   * @param[in] destFactorRgba Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, and GL_ONE_MINUS_CONSTANT_ALPHA.
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgba, BlendingFactor::Type destFactorRgba );

  /**
   * @brief Specify the pixel arithmetic used when the actor is blended.
   *
   * @param[in] srcFactorRgb Specifies how the red, green, and blue source blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, and GL_SRC_ALPHA_SATURATE.
   *
   * @param[in] destFactorRgb Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * The options are BlendingFactor::ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR,
   * SRC_ALPHA, ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR,
   * GL_CONSTANT_ALPHA, and GL_ONE_MINUS_CONSTANT_ALPHA.
   *
   * @param[in] srcFactorAlpha Specifies how the alpha source blending factor is computed.
   * The options are the same as for srcFactorRgb.
   *
   * @param[in] destFactorAlpha Specifies how the alpha source blending factor is computed.
   * The options are the same as for destFactorRgb.
   */
  void SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                     BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha );

  /**
   * @brief Query the pixel arithmetic used when the actor is blended.
   *
   * @param[out] srcFactorRgb Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorRgb Specifies how the red, green, blue, and alpha destination blending factors are computed.
   * @param[out] srcFactorAlpha Specifies how the red, green, blue, and alpha source blending factors are computed.
   * @param[out] destFactorAlpha Specifies how the red, green, blue, and alpha destination blending factors are computed.
   */
  void GetBlendFunc( BlendingFactor::Type& srcFactorRgb,   BlendingFactor::Type& destFactorRgb,
                     BlendingFactor::Type& srcFactorAlpha, BlendingFactor::Type& destFactorAlpha ) const;

  /**
   * @brief Specify the equation used when the actor is blended.
   *
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   * @param[in] equationRgba The equation used for combining red, green, blue, and alpha components.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgba );

  /**
   * @brief Specify the equation used when the actor is blended.
   *
   * @param[in] equationRgb The equation used for combining red, green, and blue components.
   * @param[in] equationAlpha The equation used for combining the alpha component.
   * The options are BlendingEquation::ADD, SUBTRACT, or REVERSE_SUBTRACT.
   */
  void SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha );

  /**
   * @brief Query the equation used when the actor is blended.
   *
   * @param[out] equationRgb The equation used for combining red, green, and blue components.
   * @param[out] equationAlpha The equation used for combining the alpha component.
   */
  void GetBlendEquation( BlendingEquation::Type& equationRgb, BlendingEquation::Type& equationAlpha ) const;

  /**
   * @brief Specify the color used when the actor is blended; the default is Vector4::ZERO.
   *
   * @param[in] color The blend color.
   */
  void SetBlendColor( const Vector4& color );

  /**
   * @brief Query the color used when the actor is blended.
   *
   * @return The blend color.
   */
  const Vector4& GetBlendColor() const;

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated Material
   */
  explicit DALI_INTERNAL Material( Internal::Material* pointer );
};

} //namespace Dali



#endif // DALI_MATERIAL_H
