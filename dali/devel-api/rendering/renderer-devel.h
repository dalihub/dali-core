#ifndef DALI_RENDERER_DEVEL_H
#define DALI_RENDERER_DEVEL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/renderer.h>

namespace Dali
{
namespace DevelBlendEquation
{
/**
 * @brief Enumeration for blend equation.
 */
enum Type
{
  ADD              = Dali::BlendEquation::ADD,
  SUBTRACT         = Dali::BlendEquation::SUBTRACT,
  REVERSE_SUBTRACT = Dali::BlendEquation::REVERSE_SUBTRACT,

  // OpenGL es 3.0 enumeration
  MIN = 0x8007,
  MAX = 0x8008,

  // OpenGL es 3.2 or KHR_Blend_Equation_Advanced enumeration
  MULTIPLY    = 0x9294,
  SCREEN      = 0x9295,
  OVERLAY     = 0x9296,
  DARKEN      = 0x9297,
  LIGHTEN     = 0x9298,
  COLOR_DODGE = 0x9299,
  COLOR_BURN  = 0x929A,
  HARD_LIGHT  = 0x929B,
  SOFT_LIGHT  = 0x929C,
  DIFFERENCE  = 0x929E,
  EXCLUSION   = 0x92A0,
  HUE         = 0x92AD,
  SATURATION  = 0x92AE,
  COLOR       = 0x92AF,
  LUMINOSITY  = 0x92B0
};

} // namespace DevelBlendEquation

namespace DevelRenderer
{
/**
 * The index of render queue used by the DrawCommand
 */
using RenderQueueIndex = uint32_t;

constexpr RenderQueueIndex RENDER_QUEUE_OPAQUE      = 0; ///< Queue for opaque elements
constexpr RenderQueueIndex RENDER_QUEUE_TRANSPARENT = 1; ///<Queue for transparent elements
constexpr RenderQueueIndex RENDER_QUEUE_MAX         = 2;

/**
 * Enum describing way of rendering the primitives (indexed draw, array draw)
 */
enum class DrawType
{
  INDEXED,
  ARRAY,
};

/**
 * Draw command can be attached to the Dali::Renderer and override the default
 * rendering functionality. Renderer may have several DrawCommands attached to it
 * and the will be executed sequentially in the order of the 'queue' index.
 */
struct DrawCommand
{
  DrawType         drawType;     ///< Type of drawing (indexed, array)
  uint32_t         firstIndex;   ///< First index into the geometry array
  uint32_t         elementCount; ///< Number of elements to draw
  RenderQueueIndex queue;        ///< Queue index
};

/**
 * @brief Adds a draw command to the renderer
 * Once the draw command is added, the default Renderer's behaviour is overriden.
 * @param[in] renderer a valid Renderer object
 * @param[in] drawCommand Valid DrawCommand to add to the Renderer
 */
DALI_CORE_API void AddDrawCommand(Dali::Renderer renderer, const DrawCommand& drawCommand);

namespace Property
{
enum Type
{
  DEPTH_INDEX                 = Dali::Renderer::Property::DEPTH_INDEX,
  FACE_CULLING_MODE           = Dali::Renderer::Property::FACE_CULLING_MODE,
  BLEND_MODE                  = Dali::Renderer::Property::BLEND_MODE,
  BLEND_EQUATION_RGB          = Dali::Renderer::Property::BLEND_EQUATION_RGB,
  BLEND_EQUATION_ALPHA        = Dali::Renderer::Property::BLEND_EQUATION_ALPHA,
  BLEND_FACTOR_SRC_RGB        = Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB,
  BLEND_FACTOR_DEST_RGB       = Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB,
  BLEND_FACTOR_SRC_ALPHA      = Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA,
  BLEND_FACTOR_DEST_ALPHA     = Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA,
  BLEND_COLOR                 = Dali::Renderer::Property::BLEND_COLOR,
  BLEND_PRE_MULTIPLIED_ALPHA  = Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA,
  INDEX_RANGE_FIRST           = Dali::Renderer::Property::INDEX_RANGE_FIRST,
  INDEX_RANGE_COUNT           = Dali::Renderer::Property::INDEX_RANGE_COUNT,
  DEPTH_WRITE_MODE            = Dali::Renderer::Property::DEPTH_WRITE_MODE,
  DEPTH_FUNCTION              = Dali::Renderer::Property::DEPTH_FUNCTION,
  DEPTH_TEST_MODE             = Dali::Renderer::Property::DEPTH_TEST_MODE,
  RENDER_MODE                 = Dali::Renderer::Property::RENDER_MODE,
  STENCIL_FUNCTION            = Dali::Renderer::Property::STENCIL_FUNCTION,
  STENCIL_FUNCTION_MASK       = Dali::Renderer::Property::STENCIL_FUNCTION_MASK,
  STENCIL_FUNCTION_REFERENCE  = Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE,
  STENCIL_MASK                = Dali::Renderer::Property::STENCIL_MASK,
  STENCIL_OPERATION_ON_FAIL   = Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL,
  STENCIL_OPERATION_ON_Z_FAIL = Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL,
  STENCIL_OPERATION_ON_Z_PASS = Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS,

  /**
   * @brief The opacity of the renderer.
   * @details Name "opacity", type Property::FLOAT.
   */
  OPACITY = STENCIL_OPERATION_ON_Z_PASS + 1,

  /**
   * @brief The rendering behavior of the renderer.
   * @details Name "renderingBehavior", type Property::INTEGER.
   */
  RENDERING_BEHAVIOR = STENCIL_OPERATION_ON_Z_PASS + 2,

  /**
   * @brief name "blendEquation", type INTEGER
   * @note The default value is BlendEquation::ADD
   */
  BLEND_EQUATION = STENCIL_OPERATION_ON_Z_PASS + 3,

  /**
   * @brief Which vertex to render first from a vertex buffer without indices
   * @details name "vertexRangeFirst", type INTEGER
   */
  VERTEX_RANGE_FIRST = INDEX_RANGE_FIRST,

  /**
   * @brief How many vertices to render to render from a vertex buffer without indices
   * @details name "vertexRangeCount", type INTEGER
   */
  VERTEX_RANGE_COUNT = INDEX_RANGE_COUNT,

  /**
   * @brief How many instances to render from a vertex buffer with non-zero divisor.
   * @note Overrides computed instance count on vertex buffer.
   * @details name "instanceCount", type INTEGER
   */
  INSTANCE_COUNT = STENCIL_OPERATION_ON_Z_PASS + 4,
};
} // namespace Property

namespace Rendering
{
/**
 * @brief Enumeration for the rendering behavior
 */
enum Type
{
  IF_REQUIRED, ///< Default. Will only render if required to do so.
  CONTINUOUSLY ///< Will render continuously.
};

} // namespace Rendering

/**
 * @brief Query whether current blend equation is advanced option.
 * @param[in] renderer to be checked whether it has been applied advanced blend equation or not
 * @return True if current blend equation is advanced.
 */
DALI_CORE_API bool IsAdvancedBlendEquationApplied(const Renderer& renderer);

} // namespace DevelRenderer

} // namespace Dali

#endif // DALI_RENDERER_DEVEL_H
