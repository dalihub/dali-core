#ifndef DALI_RENDERER_H
#define DALI_RENDERER_H

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/blending.h> // Dali::BlendingMode, Dali::BlendingEquation, Dali::BlendingFactor
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_OBJECT_PROPERTY_START_INDEX
#include <dali/devel-api/rendering/geometry.h> // Dali::Geometry
#include <dali/devel-api/rendering/texture-set.h> // Dali::TextureSet

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Renderer;
}

/**
 * @brief Renderer is a handle to an object used to show content by combining a Geometry, a TextureSet and a shader
 */
class DALI_IMPORT_API Renderer : public Handle
{
public:

  /**
   * @brief Set face culling mode.
   */
  enum FaceCullingMode
  {
    NONE,                     ///< None of the faces should be culled
    CULL_FRONT,               ///< Cull front face, back face should never be shown
    CULL_BACK,                ///< Cull back face, back face should never be shown
    CULL_BACK_AND_FRONT,      ///< Cull back and front faces, if the geometry is composed of triangles none of the faces will be shown
  };

  /**
   * @brief An enumeration of properties belonging to the Renderer class.
   */
  struct Property
  {
    enum
    {
      DEPTH_INDEX = DEFAULT_OBJECT_PROPERTY_START_INDEX,  ///< name "depthIndex",                     type INTEGER
      FACE_CULLING_MODE,                                  ///< name "faceCullingMode",                type INTEGER
      BLENDING_MODE,                                      ///< name "blendingMode",                   type INTEGER
      BLEND_EQUATION_RGB,                                 ///< name "blendEquationRgb",               type INTEGER
      BLEND_EQUATION_ALPHA,                               ///< name "blendEquationAlpha",             type INTEGER
      BLENDING_SRC_FACTOR_RGB,                            ///< name "sourceBlendFactorRgb",           type INTEGER
      BLENDING_DEST_FACTOR_RGB,                           ///< name "destinationBlendFactorRgb",      type INTEGER
      BLENDING_SRC_FACTOR_ALPHA,                          ///< name "sourceBlendFactorAlpha",         type INTEGER
      BLENDING_DEST_FACTOR_ALPHA,                         ///< name "destinationBlendFactorAlpha",    type INTEGER
      BLENDING_COLOR,                                     ///< name "blendingColor",                  type VECTOR4
      BLEND_PRE_MULTIPLIED_ALPHA,                         ///< name "blendPreMultipledAlpha",         type BOOLEAN
      INDEX_RANGE_FIRST,                                  ///< name "indexRangeFirst",                type INTEGER
      INDEX_RANGE_COUNT                                   ///< name "indexRangeCount",                type INTEGER
    };
  };

  /**
   * @brief Creates a new Renderer object
   *
   * @param[in] geometry Geometry to be used by this renderer
   * @param[in] shader Shader to be used by this renderer
   */
  static Renderer New( Geometry& geometry, Shader& shader );

  /**
   * @brief Default constructor, creates an empty handle
   */
  Renderer();

  /**
   * @brief Destructor
   */
  ~Renderer();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  Renderer( const Renderer& handle );

  /**
   * @brief Downcast to a renderer handle.
   *
   * If not a renderer the returned renderer handle is left uninitialized.
   * @param[in] handle to an object
   * @return renderer handle or an uninitialized handle
   */
  static Renderer DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Renderer& operator=( const Renderer& handle );

  /**
   * @brief Sets the geometry to be used by this renderer
   *
   * @param[in] geometry The geometry to be used by this renderer
   */
  void SetGeometry( Geometry& geometry );

  /**
   * @brief Gets the geometry used by this renderer
   *
   * @return The geometry used by the renderer
   */
  Geometry GetGeometry() const;

  /**
   * @brief Sets effective range of indices to draw from bound index buffer
   * @param [in] firstElement first element to draw
   * @param [in] elementsCount number of elements to draw
   */
  inline void SetIndexRange( int firstElement, int elementsCount )
  {
    SetProperty( Property::INDEX_RANGE_FIRST, firstElement );
    SetProperty( Property::INDEX_RANGE_COUNT, elementsCount );
  }

  /**
   * @brief Sets the texture set to be used by this renderer
   *
   * @param[in] textureSet The texture set to be used by this renderer
   */
  void SetTextures( TextureSet& textureSet );

  /**
   * @brief Gets the texture set used by this renderer
   *
   * @return The texture set used by the renderer
   */
  TextureSet GetTextures() const;

  /**
   * @brief Set the shader used by this renderer
   * @param[in] shader The shader to be used by this renderer
   */
  void SetShader( Shader& shader );

  /**
   * @brief Get the shader used by this renderer
   * @return THe shader used by the renderer
   */
  Shader GetShader() const;

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

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated Renderer
   */
  explicit DALI_INTERNAL Renderer( Internal::Renderer* pointer );
};

} //namespace Dali

#endif // DALI_RENDERER_H
