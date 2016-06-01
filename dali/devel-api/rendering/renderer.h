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

namespace FaceCullingMode
{

/**
 * @brief Set face culling mode.
 * @SINCE_1_1.33
 */
enum Type
{
  NONE,                ///< None of the faces should be culled
  FRONT,               ///< Cull front face, back face should never be shown
  BACK,                ///< Cull back face, back face should never be shown
  FRONT_AND_BACK,      ///< Cull front and back faces; if the geometry is composed of triangles none of the faces will be shown
};

} // namespace FaceCullingMode

namespace BlendMode
{

/**
 * @brief Blend mode.
 * @SINCE_1_1.33
 */
enum Type
{
  OFF,  ///< Blending is disabled. @SINCE_1_1.33
  AUTO, ///< Blending is enabled if there is alpha channel. This is the default mode. @SINCE_1_1.33
  ON    ///< Blending is enabled. @SINCE_1_1.33
};

} // namespace BlendMode

namespace BlendEquation
{
/**
 * @brief Blend Equation.
 *
 * @SINCE_1_0.0
 * @remarks This is an experimental feature and might not be supported in the next release.
 * We do recommend not to use it.
 */
enum Type
{
  ADD              = 0x8006,  ///< The source and destination colors are added to each other. @SINCE_1_0.0
  SUBTRACT         = 0x800A,  ///< Subtracts the destination from the source. @SINCE_1_0.0
  REVERSE_SUBTRACT = 0x800B  ///< Subtracts the source from the destination. @SINCE_1_0.0
};

} // namespace BlendEquation

namespace BlendFactor
{
/**
 * @brief Blend Factor.
 *
 * @SINCE_1_0.0
 * @remarks This is an experimental feature and might not be supported in the next release.
 * We do recommend not to use it.
 */
enum Type
{
  ZERO                     = 0,  ///< ZERO @SINCE_1_0.0
  ONE                      = 1,  ///< ONE @SINCE_1_0.0
  SRC_COLOR                = 0x0300,  ///< SRC_COLOR @SINCE_1_0.0
  ONE_MINUS_SRC_COLOR      = 0x0301,  ///< ONE_MINUS_SRC_COLOR @SINCE_1_0.0
  SRC_ALPHA                = 0x0302,  ///< SRC_ALPHA @SINCE_1_0.0
  ONE_MINUS_SRC_ALPHA      = 0x0303,  ///< ONE_MINUS_SRC_ALPHA @SINCE_1_0.0
  DST_ALPHA                = 0x0304,  ///< DST_ALPHA @SINCE_1_0.0
  ONE_MINUS_DST_ALPHA      = 0x0305,  ///< ONE_MINUS_DST_ALPHA @SINCE_1_0.0
  DST_COLOR                = 0x0306,  ///< DST_COLOR @SINCE_1_0.0
  ONE_MINUS_DST_COLOR      = 0x0307,  ///< ONE_MINUS_DST_COLOR @SINCE_1_0.0
  SRC_ALPHA_SATURATE       = 0x0308,  ///< SRC_ALPHA_SATURATE @SINCE_1_0.0
  CONSTANT_COLOR           = 0x8001,  ///< CONSTANT_COLOR @SINCE_1_0.0
  ONE_MINUS_CONSTANT_COLOR = 0x8002,  ///< ONE_MINUS_CONSTANT_COLOR @SINCE_1_0.0
  CONSTANT_ALPHA           = 0x8003,  ///< CONSTANT_ALPHA @SINCE_1_0.0
  ONE_MINUS_CONSTANT_ALPHA = 0x8004  ///< ONE_MINUS_CONSTANT_ALPHA @SINCE_1_0.0
};

} // namespace BlendFactor

namespace DepthWriteMode
{

/**
 * @brief Depth buffer write modes
 */
enum Type
{
  OFF,  ///< Renderer doesn't write to the depth buffer
  AUTO, ///< Renderer only writes to the depth buffer if it's opaque
  ON    ///< Renderer writes to the depth buffer
};

} // namespace DepthWriteMode

namespace DepthTestMode
{

/**
 * @brief Depth buffer test (reading) modes.
 */
enum Type
{
  OFF,  ///< Renderer does not read from the depth buffer @SINCE_1_1.38
  AUTO, ///< Renderer only reads from the depth buffer if in a 3D layer @SINCE_1_1.38
  ON    ///< Renderer reads from the depth buffer based on the DepthFunction @SINCE_1_1.38
};

} // namespace DepthTestMode

namespace DepthFunction
{

/**
 * @brief Depth functions
 */
enum Type
{
  NEVER,        ///< Depth test never passes
  ALWAYS,       ///< Depth test always passes
  LESS,         ///< Depth test passes if the incoming depth value is less than the stored depth value
  GREATER,      ///< Depth test passes if the incoming depth value is greater than the stored depth value
  EQUAL,        ///< Depth test passes if the incoming depth value is equal to the stored depth value
  NOT_EQUAL,    ///< Depth test passes if the incoming depth value is not equal to the stored depth value
  LESS_EQUAL,   ///< Depth test passes if the incoming depth value is less than or equal to the stored depth value
  GREATER_EQUAL ///< Depth test passes if the incoming depth value is greater than or equal to the stored depth value
};

} // namespace DepthFunction

/**
 * @brief Renderer is a handle to an object used to show content by combining a Geometry, a TextureSet and a shader
 */
class DALI_IMPORT_API Renderer : public Handle
{
public:

  /**
   * @brief An enumeration of properties belonging to the Renderer class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief name "depthIndex", type INTEGER
       * @note The default value is 0
       */
      DEPTH_INDEX = DEFAULT_OBJECT_PROPERTY_START_INDEX,

      /**
       * @brief name "faceCullingMode", type INTEGER
       * @note The default value is FaceCullingMode::NONE
       */
      FACE_CULLING_MODE,

      /**
       * @brief name "blendMode", type INTEGER
       * @note The default value is BlendMode::AUTO
       */
      BLEND_MODE,

      /**
       * @brief name "blendEquationRgb", type INTEGER
       * @note The default value is BlendEquation::ADD
       */
      BLEND_EQUATION_RGB,

      /**
       * @brief name "blendEquationAlpha", type INTEGER
       * @note The default value is BlendEquation::ADD
       */
      BLEND_EQUATION_ALPHA,

      /**
       * @brief name "blendFactorSrcRgb", type INTEGER
       * @note The default value is BlendFactor::SRC_ALPHA
       */
      BLEND_FACTOR_SRC_RGB,

      /**
       * @brief name "blendFactorDestRgb", type INTEGER
       * @note The default value is BlendFactor::ONE_MINUS_SRC_ALPHA
       */
      BLEND_FACTOR_DEST_RGB,

      /**
       * @brief name "blendFactorSrcAlpha", type INTEGER
       * @note The default value is BlendFactor::ONE
       */
      BLEND_FACTOR_SRC_ALPHA,

      /**
       * @brief name "blendFactorDestAlpha", type INTEGER
       * @note The default value is BlendFactor::ONE_MINUS_SRC_ALPHA
       */
      BLEND_FACTOR_DEST_ALPHA,

      /**
       * @brief name "blendColor", type VECTOR4
       * @note The default value is Color::TRANSPARENT
       */
      BLEND_COLOR,

      /**
       * @brief name "blendPreMultipledAlpha", type BOOLEAN
       * @note The default value is false
       */
      BLEND_PRE_MULTIPLIED_ALPHA,

      /**
       * @brief name "indexRangeFirst", type INTEGER
       * @note The default value is 0
       */
      INDEX_RANGE_FIRST,

      /**
       * @brief name "indexRangeCount", type INTEGER
       * @note The default (0) means that whole range of indices will be used
       */
      INDEX_RANGE_COUNT,

      /**
       * @brief name "depthWriteMode", type INTEGER
       * @see DepthWriteMode
       * @note The default value is DepthWriteMode::AUTO
       */
      DEPTH_WRITE_MODE,

      /**
       * @brief name "depthFunction", type INTEGER
       * @see DepthFunction
       * @note The default value is DepthFunction::LESS
       */
      DEPTH_FUNCTION,

      /**
       * @brief name "depthTestMode", type INTEGER
       * @see DepthTestMode
       * @note The default value is DepthTestMode::AUTO
       * @SINCE_1_1.38
       */
      DEPTH_TEST_MODE
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
