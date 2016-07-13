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
#include <dali/public-api/object/handle.h>                // Dali::Handle
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_OBJECT_PROPERTY_START_INDEX
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
 * @brief Face culling mode.
 * @SINCE_1_1.43
 */
enum Type
{
  NONE,                ///< None of the faces should be culled               @SINCE_1_1.43
  FRONT,               ///< Cull front face, back face should never be shown @SINCE_1_1.43
  BACK,                ///< Cull back face, back face should never be shown  @SINCE_1_1.43
  FRONT_AND_BACK,      ///< Cull front and back faces; if the geometry is composed of triangles none of the faces will be shown @SINCE_1_1.43
};

} // namespace FaceCullingMode

namespace BlendMode
{

/**
 * @brief Blend mode.
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
 * @brief Blend Equation.
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
 * @brief Blend Factor.
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
 * @brief Depth buffer write modes.
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
 * @brief Depth buffer test (read) modes.
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
 * @brief Depth functions.
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

namespace StencilFunction
{

/**
 * @brief The comparison function used on the stencil buffer.
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

namespace StencilMode
{

/**
 * @brief How the stencil buffer will be managed.
 * @SINCE_1_1.43
 */
enum Type
{
  OFF,       ///< Off for this renderer                                  @SINCE_1_1.43
  AUTO,      ///< Managed by the Actor clipping API. This is the default @SINCE_1_1.43
  ON         ///< On for this renderer. Select this to use the Renderer stencil properties to manage behavior. Note that Actor clipping management is bypassed for this renderer @SINCE_1_1.43
};

} // namespace StencilMode

namespace StencilOperation
{

/**
 * @brief Specifies the action to take when the stencil (or depth) test fails during stencil test.
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
 * @brief Renderer is a handle to an object used to show content by combining a Geometry, a TextureSet and a shader
 *
 * @SINCE_1_1.43
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
       * @SINCE_1_1.43
       */
      DEPTH_INDEX = DEFAULT_OBJECT_PROPERTY_START_INDEX,

      /**
       * @brief name "faceCullingMode", type INTEGER
       * @note The default value is FaceCullingMode::NONE
       * @SINCE_1_1.43
       */
      FACE_CULLING_MODE,

      /**
       * @brief name "blendMode", type INTEGER
       * @note The default value is BlendMode::AUTO
       * @SINCE_1_1.43
       */
      BLEND_MODE,

      /**
       * @brief name "blendEquationRgb", type INTEGER
       * @note The default value is BlendEquation::ADD
       * @SINCE_1_1.43
       */
      BLEND_EQUATION_RGB,

      /**
       * @brief name "blendEquationAlpha", type INTEGER
       * @note The default value is BlendEquation::ADD
       * @SINCE_1_1.43
       */
      BLEND_EQUATION_ALPHA,

      /**
       * @brief name "blendFactorSrcRgb", type INTEGER
       * @note The default value is BlendFactor::SRC_ALPHA
       * @SINCE_1_1.43
       */
      BLEND_FACTOR_SRC_RGB,

      /**
       * @brief name "blendFactorDestRgb", type INTEGER
       * @note The default value is BlendFactor::ONE_MINUS_SRC_ALPHA
       * @SINCE_1_1.43
       */
      BLEND_FACTOR_DEST_RGB,

      /**
       * @brief name "blendFactorSrcAlpha", type INTEGER
       * @note The default value is BlendFactor::ONE
       * @SINCE_1_1.43
       */
      BLEND_FACTOR_SRC_ALPHA,

      /**
       * @brief name "blendFactorDestAlpha", type INTEGER
       * @note The default value is BlendFactor::ONE_MINUS_SRC_ALPHA
       * @SINCE_1_1.43
       */
      BLEND_FACTOR_DEST_ALPHA,

      /**
       * @brief name "blendColor", type VECTOR4
       * @note The default value is Color::TRANSPARENT
       * @SINCE_1_1.43
       */
      BLEND_COLOR,

      /**
       * @brief name "blendPreMultipledAlpha", type BOOLEAN
       * @note The default value is false
       * @SINCE_1_1.43
       */
      BLEND_PRE_MULTIPLIED_ALPHA,

      /**
       * @brief name "indexRangeFirst", type INTEGER
       * @note The default value is 0
       * @SINCE_1_1.43
       */
      INDEX_RANGE_FIRST,

      /**
       * @brief name "indexRangeCount", type INTEGER
       * @note The default (0) means that whole range of indices will be used
       * @SINCE_1_1.43
       */
      INDEX_RANGE_COUNT,

      /**
       * @brief name "depthWriteMode", type INTEGER
       * @see DepthWriteMode
       * @note The default value is DepthWriteMode::AUTO
       * @SINCE_1_1.43
       */
      DEPTH_WRITE_MODE,

      /**
       * @brief name "depthFunction", type INTEGER
       * @see DepthFunction
       * @note The default value is DepthFunction::LESS
       * @SINCE_1_1.43
       */
      DEPTH_FUNCTION,

      /**
       * @brief name "depthTestMode", type INTEGER
       * @see DepthTestMode
       * @note The default value is DepthTestMode::AUTO
       * @SINCE_1_1.43
       */
      DEPTH_TEST_MODE,

      /**
       * @brief name "stencilFunction", type INTEGER
       * @see StencilFunction
       * @note The default value is StencilFunction::ALWAYS
       * @SINCE_1_1.43
       */
      STENCIL_FUNCTION,

      /**
       * @brief name "stencilFunctionMask", type INTEGER
       * @note The default value is 0xFF
       * @SINCE_1_1.43
       */
      STENCIL_FUNCTION_MASK,

      /**
       * @brief name "stencilFunctionReference", type INTEGER
       * @note The default value is 0
       * @SINCE_1_1.43
       */
      STENCIL_FUNCTION_REFERENCE,

      /**
       * @brief name "stencilMask", type INTEGER
       * @note The default value is 0xFF
       * @SINCE_1_1.43
       */
      STENCIL_MASK,

      /**
       * @brief name "stencilMode", type INTEGER
       * @see StencilMode
       * @note The default value is StencilMode::AUTO
       * @SINCE_1_1.43
       */
      STENCIL_MODE,

      /**
       * @brief name "stencilOperationOnFail", type INTEGER
       * @see StencilOperation
       * @note The default value is StencilOperation::KEEP
       * @SINCE_1_1.43
       */
      STENCIL_OPERATION_ON_FAIL,

      /**
       * @brief name "stencilOperationOnZFail", type INTEGER
       * @see StencilOperation
       * @note The default value is StencilOperation::KEEP
       * @SINCE_1_1.43
       */
      STENCIL_OPERATION_ON_Z_FAIL,

      /**
       * @brief name "stencilOperationOnZPass", type INTEGER
       * @see StencilOperation
       * @note The default value is StencilOperation::KEEP
       * @SINCE_1_1.43
       */
      STENCIL_OPERATION_ON_Z_PASS,

      /**
       * @brief name "writeToColorBuffer", type BOOLEAN
       * This allows per-renderer control of writing to the color buffer.
       * For example: This can be turned off to write to the stencil or depth buffers only.
       * @note The default value is True
       * @SINCE_1_1.43
       */
      WRITE_TO_COLOR_BUFFER
    };
  };

  /**
   * @brief Creates a new Renderer object
   *
   * @SINCE_1_1.43
   * @param[in] geometry Geometry to be used by this renderer
   * @param[in] shader Shader to be used by this renderer
   */
  static Renderer New( Geometry& geometry, Shader& shader );

  /**
   * @brief Default constructor, creates an empty handle
   *
   * @SINCE_1_1.43
   */
  Renderer();

  /**
   * @brief Destructor
   *
   * @SINCE_1_1.43
   */
  ~Renderer();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   */
  Renderer( const Renderer& handle );

  /**
   * @brief Downcast to a renderer handle.
   * If not a renderer the returned renderer handle is left uninitialized.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Renderer handle or an uninitialized handle
   */
  static Renderer DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Renderer& operator=( const Renderer& handle );

  /**
   * @brief Sets the geometry to be used by this renderer
   *
   * @SINCE_1_1.43
   * @param[in] geometry The geometry to be used by this renderer
   */
  void SetGeometry( Geometry& geometry );

  /**
   * @brief Gets the geometry used by this renderer
   *
   * @SINCE_1_1.43
   * @return The geometry used by the renderer
   */
  Geometry GetGeometry() const;

  /**
   * @brief Sets effective range of indices to draw from bound index buffer
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
   * @brief Sets the texture set to be used by this renderer
   *
   * @SINCE_1_1.43
   * @param[in] textureSet The texture set to be used by this renderer
   */
  void SetTextures( TextureSet& textureSet );

  /**
   * @brief Gets the texture set used by this renderer
   *
   * @SINCE_1_1.43
   * @return The texture set used by the renderer
   */
  TextureSet GetTextures() const;

  /**
   * @brief Set the shader used by this renderer
   *
   * @SINCE_1_1.43
   * @param[in] shader The shader to be used by this renderer
   */
  void SetShader( Shader& shader );

  /**
   * @brief Get the shader used by this renderer
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
