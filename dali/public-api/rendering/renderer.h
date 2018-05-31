#ifndef DALI_RENDERER_H
#define DALI_RENDERER_H

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

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>                // Dali::Handle
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_RENDERER_PROPERTY_START_INDEX
#include <dali/public-api/rendering/geometry.h>           // Dali::Geometry
#include <dali/public-api/rendering/texture-set.h>        // Dali::TextureSet

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Renderer;
}

namespace FaceCullingMode
{

/**
 * @brief Enumeration for face culling mode.
 * @SINCE_1_1.43
 */
enum Type
{
  NONE,                ///< None of the faces should be culled                 @SINCE_1_1.43
  FRONT,               ///< Cull front face, front faces should never be shown @SINCE_1_1.43
  BACK,                ///< Cull back face, back faces should never be shown   @SINCE_1_1.43
  FRONT_AND_BACK,      ///< Cull front and back faces; if the geometry is composed of triangles none of the faces will be shown @SINCE_1_1.43
};

} // namespace FaceCullingMode

namespace BlendMode
{

/**
 * @brief Enumeration for blend mode.
 * @SINCE_1_1.43
 */
enum Type
{
  OFF,  ///< Blending is disabled.                                                    @SINCE_1_1.43
  AUTO, ///< Blending is enabled if there is alpha channel. This is the default mode. @SINCE_1_1.43
  ON    ///< Blending is enabled.                                                     @SINCE_1_1.43
};

} // namespace BlendMode

namespace BlendEquation
{

/**
 * @brief Enumeration for blend equation.
 * @SINCE_1_1.43
 */
enum Type
{
  ADD              = 0x8006,  ///< The source and destination colors are added to each other. @SINCE_1_1.43
  SUBTRACT         = 0x800A,  ///< Subtracts the destination from the source.                 @SINCE_1_1.43
  REVERSE_SUBTRACT = 0x800B   ///< Subtracts the source from the destination.                 @SINCE_1_1.43
};

} // namespace BlendEquation

namespace BlendFactor
{

/**
 * @brief Enumeration for blend factor.
 * @SINCE_1_1.43
 */
enum Type
{
  ZERO                     = 0,      ///< ZERO                     @SINCE_1_1.43
  ONE                      = 1,      ///< ONE                      @SINCE_1_1.43
  SRC_COLOR                = 0x0300, ///< SRC_COLOR                @SINCE_1_1.43
  ONE_MINUS_SRC_COLOR      = 0x0301, ///< ONE_MINUS_SRC_COLOR      @SINCE_1_1.43
  SRC_ALPHA                = 0x0302, ///< SRC_ALPHA                @SINCE_1_1.43
  ONE_MINUS_SRC_ALPHA      = 0x0303, ///< ONE_MINUS_SRC_ALPHA      @SINCE_1_1.43
  DST_ALPHA                = 0x0304, ///< DST_ALPHA                @SINCE_1_1.43
  ONE_MINUS_DST_ALPHA      = 0x0305, ///< ONE_MINUS_DST_ALPHA      @SINCE_1_1.43
  DST_COLOR                = 0x0306, ///< DST_COLOR                @SINCE_1_1.43
  ONE_MINUS_DST_COLOR      = 0x0307, ///< ONE_MINUS_DST_COLOR      @SINCE_1_1.43
  SRC_ALPHA_SATURATE       = 0x0308, ///< SRC_ALPHA_SATURATE       @SINCE_1_1.43
  CONSTANT_COLOR           = 0x8001, ///< CONSTANT_COLOR           @SINCE_1_1.43
  ONE_MINUS_CONSTANT_COLOR = 0x8002, ///< ONE_MINUS_CONSTANT_COLOR @SINCE_1_1.43
  CONSTANT_ALPHA           = 0x8003, ///< CONSTANT_ALPHA           @SINCE_1_1.43
  ONE_MINUS_CONSTANT_ALPHA = 0x8004  ///< ONE_MINUS_CONSTANT_ALPHA @SINCE_1_1.43
};

} // namespace BlendFactor

namespace DepthWriteMode
{

/**
 * @brief Enumeration for depth buffer write modes.
 * @SINCE_1_1.43
 */
enum Type
{
  OFF,  ///< Renderer doesn't write to the depth buffer              @SINCE_1_1.43
  AUTO, ///< Renderer only writes to the depth buffer if it's opaque @SINCE_1_1.43
  ON    ///< Renderer writes to the depth buffer                     @SINCE_1_1.43
};

} // namespace DepthWriteMode

namespace DepthTestMode
{

/**
 * @brief Enumeration for depth buffer test (read) modes.
 * @SINCE_1_1.43
 */
enum Type
{
  OFF,  ///< Renderer does not read from the depth buffer                    @SINCE_1_1.43
  AUTO, ///< Renderer only reads from the depth buffer if in a 3D layer      @SINCE_1_1.43
  ON    ///< Renderer reads from the depth buffer based on the DepthFunction @SINCE_1_1.43
};

} // namespace DepthTestMode

namespace DepthFunction
{

/**
 * @brief Enumeration for depth functions.
 * @SINCE_1_1.43
 */
enum Type
{
  NEVER,        ///< Depth test never passes                                                                          @SINCE_1_1.43
  ALWAYS,       ///< Depth test always passes                                                                         @SINCE_1_1.43
  LESS,         ///< Depth test passes if the incoming depth value is less than the stored depth value                @SINCE_1_1.43
  GREATER,      ///< Depth test passes if the incoming depth value is greater than the stored depth value             @SINCE_1_1.43
  EQUAL,        ///< Depth test passes if the incoming depth value is equal to the stored depth value                 @SINCE_1_1.43
  NOT_EQUAL,    ///< Depth test passes if the incoming depth value is not equal to the stored depth value             @SINCE_1_1.43
  LESS_EQUAL,   ///< Depth test passes if the incoming depth value is less than or equal to the stored depth value    @SINCE_1_1.43
  GREATER_EQUAL ///< Depth test passes if the incoming depth value is greater than or equal to the stored depth value @SINCE_1_1.43
};

} // namespace DepthFunction

namespace RenderMode
{

/**
 * @brief Enumeration for the controls of how this renderer uses its stencil properties and writes to the color buffer.
 * @SINCE_1_2_5
 */
enum Type
{
  NONE,         ///< Do not write to either color or stencil buffer (But will potentially render to depth buffer). @SINCE_1_2_5
  AUTO,         ///< Managed by the Actor Clipping API. This is the default.                                       @SINCE_1_2_5
  COLOR,        ///< Ingore stencil properties.  Write to the color buffer.                                        @SINCE_1_2_5
  STENCIL,      ///< Use the stencil properties. Do not write to the color buffer.                                 @SINCE_1_2_5
  COLOR_STENCIL ///< Use the stencil properties AND Write to the color buffer.                                     @SINCE_1_2_5
};

} // namespace RenderMode

namespace StencilFunction
{

/**
 * @brief Enumeration for the comparison function used on the stencil buffer.
 * @SINCE_1_1.43
 */
enum Type
{
  NEVER,         ///< Always fails                                         @SINCE_1_1.43
  LESS,          ///< Passes if ( reference & mask ) <  ( stencil & mask ) @SINCE_1_1.43
  EQUAL,         ///< Passes if ( reference & mask ) =  ( stencil & mask ) @SINCE_1_1.43
  LESS_EQUAL,    ///< Passes if ( reference & mask ) <= ( stencil & mask ) @SINCE_1_1.43
  GREATER,       ///< Passes if ( reference & mask ) >  ( stencil & mask ) @SINCE_1_1.43
  NOT_EQUAL,     ///< Passes if ( reference & mask ) != ( stencil & mask ) @SINCE_1_1.43
  GREATER_EQUAL, ///< Passes if ( reference & mask ) >= ( stencil & mask ) @SINCE_1_1.43
  ALWAYS,        ///< Always passes                                        @SINCE_1_1.43
};

} // namespace StencilFunction

namespace StencilOperation
{

/**
 * @brief Enumeration for specifying the action to take when the stencil (or depth) test fails during stencil test.
 * @SINCE_1_1.43
 */
enum Type
{
  ZERO,           ///< Sets the stencil buffer value to 0                                                              @SINCE_1_1.43
  KEEP,           ///< Keeps the current value                                                                         @SINCE_1_1.43
  REPLACE,        ///< Sets the stencil buffer value to ref, as specified by glStencilFunc                             @SINCE_1_1.43
  INCREMENT,      ///< Increments the current stencil buffer value. Clamps to the maximum representable unsigned value @SINCE_1_1.43
  DECREMENT,      ///< Decrements the current stencil buffer value. Clamps to 0                                        @SINCE_1_1.43
  INVERT,         ///< Bitwise inverts the current stencil buffer value                                                @SINCE_1_1.43
  INCREMENT_WRAP, ///< Increments the current stencil buffer value. Wraps stencil buffer value to zero when incrementing the maximum representable unsigned value @SINCE_1_1.43
  DECREMENT_WRAP  ///< Decrements the current stencil buffer value. Wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of zero @SINCE_1_1.43
};

} // namespace StencilOperation


/**
 * @brief Renderer is a handle to an object used to show content by combining a Geometry, a TextureSet and a shader.
 *
 * @SINCE_1_1.43
 */
class DALI_CORE_API Renderer : public Handle
{
public:

  /**
   * @brief Enumeration for instances of properties belonging to the Renderer class.
   * @SINCE_1_1.43
   */
  struct Property
  {
    /**
     * @brief Enumeration for instances of properties belonging to the Renderer class.
     * @SINCE_1_1.43
     */
    enum
    {
      /**
       * @brief Name "depthIndex", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is 0.
       */
      DEPTH_INDEX = DEFAULT_RENDERER_PROPERTY_START_INDEX,

      /**
       * @brief Name "faceCullingMode", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is FaceCullingMode::NONE.
       */
      FACE_CULLING_MODE,

      /**
       * @brief Name "blendMode", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is BlendMode::AUTO.
       */
      BLEND_MODE,

      /**
       * @brief Name "blendEquationRgb", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is BlendEquation::ADD.
       */
      BLEND_EQUATION_RGB,

      /**
       * @brief Name "blendEquationAlpha", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is BlendEquation::ADD.
       */
      BLEND_EQUATION_ALPHA,

      /**
       * @brief Name "blendFactorSrcRgb", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is BlendFactor::SRC_ALPHA.
       */
      BLEND_FACTOR_SRC_RGB,

      /**
       * @brief Name "blendFactorDestRgb", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is BlendFactor::ONE_MINUS_SRC_ALPHA.
       */
      BLEND_FACTOR_DEST_RGB,

      /**
       * @brief Name "blendFactorSrcAlpha", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is BlendFactor::ONE.
       */
      BLEND_FACTOR_SRC_ALPHA,

      /**
       * @brief Name "blendFactorDestAlpha", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is BlendFactor::ONE_MINUS_SRC_ALPHA.
       */
      BLEND_FACTOR_DEST_ALPHA,

      /**
       * @brief Name "blendColor", type VECTOR4.
       * @SINCE_1_1.43
       * @note The default value is Color::TRANSPARENT.
       */
      BLEND_COLOR,

      /**
       * @brief Name "blendPreMultipledAlpha", type BOOLEAN.
       * @SINCE_1_1.43
       * @note The default value is false.
       */
      BLEND_PRE_MULTIPLIED_ALPHA,

      /**
       * @brief Name "indexRangeFirst", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is 0.
       */
      INDEX_RANGE_FIRST,

      /**
       * @brief Name "indexRangeCount", type INTEGER.
       * @SINCE_1_1.43
       * @note The default (0) means that whole range of indices will be used.
       */
      INDEX_RANGE_COUNT,

      /**
       * @brief Name "depthWriteMode", type INTEGER.
       * @SINCE_1_1.43
       * @see DepthWriteMode
       * @note The default value is DepthWriteMode::AUTO.
       */
      DEPTH_WRITE_MODE,

      /**
       * @brief Name "depthFunction", type INTEGER.
       * @SINCE_1_1.43
       * @see DepthFunction
       * @note The default value is DepthFunction::LESS.
       */
      DEPTH_FUNCTION,

      /**
       * @brief Name "depthTestMode", type INTEGER.
       * @SINCE_1_1.43
       * @see DepthTestMode
       * @note The default value is DepthTestMode::AUTO.
       */
      DEPTH_TEST_MODE,

      /**
       * @brief Name "renderMode", type INTEGER.
       * @SINCE_1_2_5
       * @see RenderMode
       * @note The default value is RenderMode::AUTO.
       */
      RENDER_MODE,

      /**
       * @brief Name "stencilFunction", type INTEGER.
       * @SINCE_1_1.43
       * @see StencilFunction
       * @note The default value is StencilFunction::ALWAYS.
       */
      STENCIL_FUNCTION,

      /**
       * @brief Name "stencilFunctionMask", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is 0xFF.
       */
      STENCIL_FUNCTION_MASK,

      /**
       * @brief Name "stencilFunctionReference", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is 0.
       */
      STENCIL_FUNCTION_REFERENCE,

      /**
       * @brief Name "stencilMask", type INTEGER.
       * @SINCE_1_1.43
       * @note The default value is 0xFF.
       */
      STENCIL_MASK,

      /**
       * @brief Name "stencilOperationOnFail", type INTEGER.
       * @SINCE_1_1.43
       * @see StencilOperation
       * @note The default value is StencilOperation::KEEP
       */
      STENCIL_OPERATION_ON_FAIL,

      /**
       * @brief Name "stencilOperationOnZFail", type INTEGER.
       * @SINCE_1_1.43
       * @see StencilOperation
       * @note The default value is StencilOperation::KEEP.
       */
      STENCIL_OPERATION_ON_Z_FAIL,

      /**
       * @brief Name "stencilOperationOnZPass", type INTEGER.
       * @SINCE_1_1.43
       * @see StencilOperation
       * @note The default value is StencilOperation::KEEP.
       */
      STENCIL_OPERATION_ON_Z_PASS,
    };
  };

  /**
   * @brief Creates a new Renderer object.
   *
   * @SINCE_1_1.43
   * @param[in] geometry Geometry to be used by this renderer
   * @param[in] shader Shader to be used by this renderer
   * @return A handle to the Renderer
   */
  static Renderer New( Geometry& geometry, Shader& shader );

  /**
   * @brief Default constructor, creates an empty handle
   *
   * @SINCE_1_1.43
   */
  Renderer();

  /**
   * @brief Destructor.
   *
   * @SINCE_1_1.43
   */
  ~Renderer();

  /**
   * @brief Copy constructor, creates a new handle to the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   */
  Renderer( const Renderer& handle );

  /**
   * @brief Downcasts to a renderer handle.
   * If not, a renderer the returned renderer handle is left uninitialized.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Renderer handle or an uninitialized handle
   */
  static Renderer DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Renderer& operator=( const Renderer& handle );

  /**
   * @brief Sets the geometry to be used by this renderer.
   *
   * @SINCE_1_1.43
   * @param[in] geometry The geometry to be used by this renderer
   */
  void SetGeometry( Geometry& geometry );

  /**
   * @brief Gets the geometry used by this renderer.
   *
   * @SINCE_1_1.43
   * @return The geometry used by the renderer
   */
  Geometry GetGeometry() const;

  /**
   * @brief Sets effective range of indices to draw from bound index buffer.
   *
   * @SINCE_1_1.43
   * @param[in] firstElement The First element to draw
   * @param[in] elementsCount The number of elements to draw
   */
  inline void SetIndexRange( int firstElement, int elementsCount )
  {
    SetProperty( Property::INDEX_RANGE_FIRST, firstElement );
    SetProperty( Property::INDEX_RANGE_COUNT, elementsCount );
  }

  /**
   * @brief Sets the texture set to be used by this renderer.
   *
   * @SINCE_1_1.43
   * @param[in] textureSet The texture set to be used by this renderer
   */
  void SetTextures( TextureSet& textureSet );

  /**
   * @brief Gets the texture set used by this renderer.
   *
   * @SINCE_1_1.43
   * @return The texture set used by the renderer
   */
  TextureSet GetTextures() const;

  /**
   * @brief Sets the shader used by this renderer.
   *
   * @SINCE_1_1.43
   * @param[in] shader The shader to be used by this renderer
   */
  void SetShader( Shader& shader );

  /**
   * @brief Gets the shader used by this renderer.
   *
   * @SINCE_1_1.43
   * @return The shader used by the renderer
   */
  Shader GetShader() const;

public:

  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] pointer A pointer to a newly allocated Renderer
   */
  explicit DALI_INTERNAL Renderer( Internal::Renderer* pointer );
};

} //namespace Dali

#endif // DALI_RENDERER_H
