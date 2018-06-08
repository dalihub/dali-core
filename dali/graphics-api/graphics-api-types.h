#ifndef DALI_GRAPHICS_API_TYPES_H
#define DALI_GRAPHICS_API_TYPES_H

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

#include <dali/graphics-api/graphics-api-utility.h>
#include <vector>

namespace Dali
{
namespace Graphics
{
namespace API
{
class Shader;
using Extension = void*;

/**
 * Describes vertex attribute input rate
 */
enum class VertexInputRate
{
  PER_VERTEX, /// Attribute read per vertex
  PER_INSTANCE /// Attribute read per instance
};

/**
 * Vertex input format
 * When UNDEFINED, the reflection is used to determine what
 * the actual format is.
 */
enum class VertexInputFormat
{
  UNDEFINED,
  FVECTOR2,
  FVECTOR3,
  FVECTOR4,
  IVECTOR2,
  IVECTOR3,
  IVECTOR4,
  FLOAT,
  INTEGER,
};

/**
 * Logic operators
 */
enum class LogicOp
{};

/**
 * Blend factors
 */
enum class BlendFactor
{
  ZERO = 0,
  ONE = 1,
  SRC_COLOR = 2,
  ONE_MINUS_SRC_COLOR = 3,
  DST_COLOR = 4,
  ONE_MINUS_DST_COLOR = 5,
  SRC_ALPHA = 6,
  ONE_MINUS_SRC_ALPHA = 7,
  DST_ALPHA = 8,
  ONE_MINUS_DST_ALPHA = 9,
  CONSTANT_COLOR = 10,
  ONE_MINUS_CONSTANT_COLOR = 11,
  CONSTANT_ALPHA = 12,
  ONE_MINUS_CONSTANT_ALPHA = 13,
  SRC_ALPHA_SATURATE = 14,
  SRC1_COLOR = 15,
  ONE_MINUS_SRC1_COLOR = 16,
  SRC1_ALPHA = 17,
  ONE_MINUS_SRC1_ALPHA = 18,
};

/**
 * Blend operators
 */
enum class BlendOp
{
  ADD = 0,
  SUBTRACT = 1,
  REVERSE_SUBTRACT = 2,
  MIN = 3,
  MAX = 4,
};

/**
 * Compare operators
 */
enum class CompareOp
{
  NEVER,
  LESS,
  EQUAL,
  LESS_OR_EQUAL,
  GREATER,
  NOT_EQUAL,
  GREATER_OR_EQUAL,
  ALWAYS
};

/**
 * Stencil operators
 */
enum class StencilOp
{
  KEEP,
  ZERO,
  REPLACE,
  INCREMENT_AND_CLAMP,
  DECREMENT_AND_CLAMP,
  INVERT,
  INCREMENT_AND_WRAP,
  DECREMENT_AND_WRAP
};

/**
 * Backface culling modes
 */
enum class CullMode
{
  NONE,
  FRONT,
  BACK,
  FRONT_AND_BACK
};

/**
 * Polygon drawing modes
 */
enum class PolygonMode
{
  FILL,
  LINE,
  POINT
};

/**
 * Front face direction
 */
enum class FrontFace
{
  COUNTER_CLOCKWISE,
  CLOCKWISE
};

/**
 * Topology
 */
enum class PrimitiveTopology
{
  POINT_LIST,
  LINE_LIST,
  LINE_STRIP,
  TRIANGLE_LIST,
  TRIANGLE_STRIP,
  TRIANGLE_FAN
};

/**
 * Describes pipeline's color blend state
 */
struct ColorBlendState
{
  bool    logicOpEnable             = false;
  LogicOp logicOp                   = {};
  float   blendConstants[4]         = { 0.0f, 0.0f, 0.0f, 0.0f };
  bool        blendEnable           = false;
  BlendFactor srcColorBlendFactor   = BlendFactor::ZERO;
  BlendFactor dstColorBlendFactor   = BlendFactor::ZERO;
  BlendOp     colorBlendOp          = {};
  BlendFactor srcAlphaBlendFactor   = BlendFactor::ZERO;
  BlendFactor dstAlphaBlendFactor   = BlendFactor::ZERO;
  BlendOp     alphaBlendOp          = {};
  uint32_t    colorComponentWriteBits = { 0u };

  Extension   extension             = { nullptr };

  ColorBlendState& SetLogicOpEnable( bool value )
  {
    logicOpEnable = value;
    return *this;
  }

  ColorBlendState& SetLogicOp( LogicOp value )
  {
    logicOp = value;
    return *this;
  }

  ColorBlendState& SetBlendConstants( float value[4] )
  {
    std::copy( value, value+4, blendConstants );
    return *this;
  }

  ColorBlendState& SetBlendEnable( bool value )
  {
    blendEnable = value;
    return *this;
  }

  ColorBlendState& SetSrcColorBlendFactor( BlendFactor value )
  {
    srcColorBlendFactor = value;
    return *this;
  }

  ColorBlendState& SetDstColorBlendFactor( BlendFactor value )
  {
    dstColorBlendFactor = value;
    return *this;
  }

  ColorBlendState& SetColorBlendOp( BlendOp value )
  {
    colorBlendOp = value;
    return *this;
  }

  ColorBlendState& SetSrcAlphaBlendFactor( BlendFactor value )
  {
    srcAlphaBlendFactor = value;
    return *this;
  }

  ColorBlendState& SetDstAlphaBlendFactor( BlendFactor value )
  {
    dstAlphaBlendFactor = value;
    return *this;
  }

  ColorBlendState& SetAlphaBlendOp( BlendOp value )
  {
    alphaBlendOp = value;
    return *this;
  }

  ColorBlendState& SetColorComponentsWriteBits( uint32_t value )
  {
    colorComponentWriteBits = value;
    return *this;
  }
};

/**
 * Describes pipeline's shaading stages
 */
struct ShaderState
{
  Shader* shaderProgram { nullptr };

  Extension   extension             { nullptr };

  ShaderState& SetShaderProgram( Shader& value )
  {
    shaderProgram = &value;
    return *this;
  }
};

/**
 * Describes pipeline's viewport and scissor state
 */
struct ViewportState
{
  Viewport    viewport           { 0.0, 0.0, 0.0, 0.0 };
  Rect2D      scissor            { 0, 0, 0, 0 };
  bool        scissorTestEnable  { false };

  Extension   extension          { nullptr };


  ViewportState& SetViewport( const Viewport& value )
  {
    viewport = value;
    return *this;
  }

  ViewportState& SetScissor( const Rect2D& value )
  {
    scissor = value;
    return *this;
  }

  ViewportState& SetScissorTestEnable( bool value )
  {
    scissorTestEnable = value;
    return *this;
  }
};

/**
 * Describes pipeline's viewport and scissor state
 */
struct DepthStencilState
{
  bool      depthTestEnable   { false };
  bool      depthWriteEnable  { false };
  CompareOp depthCompareOp    {};
  bool      stencilTestEnable { false };


  Extension   extension       { nullptr };


  DepthStencilState& SetDepthTestEnable( bool value )
  {
    depthTestEnable = value;
    return *this;
  }

  DepthStencilState& SetDepthWriteEnable( bool value )
  {
    depthWriteEnable = value;
    return *this;
  }

  DepthStencilState& SetDepthCompareOp( CompareOp value)
  {
    depthCompareOp = value;
    return *this;
  }

};

/**
 * RasterizationState
 */
struct RasterizationState
{
  CullMode              cullMode        {};
  PolygonMode           polygonMode     {};
  PrimitiveTopology     topology        {};
  FrontFace             frontFace       {};

  Extension             extension       { nullptr };

  RasterizationState& SetCullMode( CullMode value )
  {
    cullMode = value;
    return *this;
  }

  RasterizationState& SetPolygonMode( PolygonMode value )
  {
    polygonMode = value;
    return *this;
  }

  RasterizationState& SetFrontFace( FrontFace value )
  {
    frontFace = value;
    return *this;
  }
};

/**
 *
 */
struct InputAssemblyState
{
  PrimitiveTopology topology               {};
  bool              primitiveRestartEnable { true };

  Extension         extension              { nullptr };

  InputAssemblyState& SetTopology( PrimitiveTopology value )
  {
    topology = value;
    return *this;
  }

  InputAssemblyState& SetPrimitiveRestartEnable( bool value )
  {
    primitiveRestartEnable = true;
    return *this;
  }
};

/**
 *
 */
struct VertexInputState
{
  VertexInputState() = default;

  struct Binding
  {
    Binding( uint32_t _stride, VertexInputRate _inputRate )
      : stride( _stride ), inputRate( _inputRate ) {}
    uint32_t stride;
    VertexInputRate inputRate;
  };

  struct Attribute
  {
    Attribute( uint32_t _location, uint32_t _binding, uint32_t _offset, VertexInputFormat _format )
      : location( _location ),
        binding( _binding ),
        offset( _offset ),
        format( _format )
    {}

    uint32_t            location;
    uint32_t            binding;
    uint32_t            offset;
    VertexInputFormat   format;
  };

  VertexInputState( std::vector<Binding> _bufferBindings, std::vector<Attribute> _attributes )
    : bufferBindings( _bufferBindings ), attributes( _attributes )
  {
  }

  std::vector<Binding>    bufferBindings {};
  std::vector<Attribute>  attributes     {};

  Extension   extension                  { nullptr };
};

} // API
} // Graphics
} // Dali

#endif //DALI_GRAPHICS_API_TYPES_H
