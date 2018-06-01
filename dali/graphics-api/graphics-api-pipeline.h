#ifndef DALI_GRAPHICS_API_PIPELINE_H
#define DALI_GRAPHICS_API_PIPELINE_H

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
class Buffer;
class Shader;

class Pipeline
{

public:

  /*
   * Enums
   */

  /**
   *
   */
  enum class VertexInputRate
  {
    PER_VERTEX,
    PER_INSTANCE
  };

  /**
   *
   */
  enum class VertexInputFormat
  {
    UNDEFINED, // undefined will be pulled out of reflection
    FVECTOR2,
    FVECTOR3,
    FVECTOR4,
    IVECTOR2,
    IVECTOR3,
    IVECTOR4,
    FLOAT,
    INTEGER,
  };

  enum class LogicOp
  {};

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

  enum class BlendOp
  {
    ADD = 0,
    SUBTRACT = 1,
    REVERSE_SUBTRACT = 2,
    MIN = 3,
    MAX = 4,
  };

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

  enum class CullMode
  {
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK
  };

  enum class PolygonMode
  {
    FILL,
    LINE,
    POINT
  };

  enum class FrontFace
  {
    COUNTER_CLOCKWISE,
    CLOCKWISE
  };

  /**
   *
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

public:
  // not copyable
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  virtual ~Pipeline() = default;

protected:

  Pipeline(Pipeline&&) = default;
  Pipeline& operator=(Pipeline&&) = default;

  Pipeline() = default;

public:

  struct ColorBlendState
  {
    bool    logicOpEnable             = false;
    ColorBlendState& SetLogicOpEnable( bool value )
    {
      logicOpEnable = value;
      return *this;
    }

    LogicOp logicOp                   = {};
    ColorBlendState& SetLogicOp( LogicOp value )
    {
      logicOp = value;
      return *this;
    }

    float   blendConstants[4]         = { 0.0f, 0.0f, 0.0f, 0.0f };
    ColorBlendState& SetLogicOp( float value[4] )
    {
      std::copy( value, value+4, blendConstants );
      return *this;
    }

    // blending state
    bool        blendEnable           = false;
    ColorBlendState& SetBlendEnable( bool value )
    {
      blendEnable = value;
      return *this;
    }

    BlendFactor srcColorBlendFactor   = BlendFactor::ZERO;
    ColorBlendState& SetSrcColorBlendFactor( BlendFactor value )
    {
      srcColorBlendFactor = value;
      return *this;
    }

    BlendFactor dstColorBlendFactor   = BlendFactor::ZERO;
    ColorBlendState& SetDstColorBlendFactor( BlendFactor value )
    {
      dstColorBlendFactor = value;
      return *this;
    }

    BlendOp     colorBlendOp          = {};
    ColorBlendState& SetColorBlendOp( BlendOp value )
    {
      colorBlendOp = value;
      return *this;
    }

    BlendFactor srcAlphaBlendFactor   = BlendFactor::ZERO;
    ColorBlendState& SetSrcAlphaBlendFactor( BlendFactor value )
    {
      srcAlphaBlendFactor = value;
      return *this;
    }

    BlendFactor dstAlphaBlendFactor   = BlendFactor::ZERO;
    ColorBlendState& SetDstAlphaBlendFactor( BlendFactor value )
    {
      dstAlphaBlendFactor = value;
      return *this;
    }

    BlendOp     alphaBlendOp          = {};
    ColorBlendState& SetAlphaBlendOp( BlendOp value )
    {
      alphaBlendOp = value;
      return *this;
    }

    uint32_t    colorComponentWriteBits = { 0u };
    ColorBlendState& SetColorComponentsWriteBits( uint32_t value )
    {
      colorComponentWriteBits = value;
      return *this;
    }
  };

  struct ShaderState
  {
    Shader* shaderProgram;
    ShaderState& SetShaderProgram( Shader& value )
    {
      shaderProgram = &value;
      return *this;
    }
  };

  struct ViewportState
  {
    Viewport viewport;
    Rect2D   scissor;
    bool     scissorTestEnable;

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

  struct DepthStencilState
  {
    bool      depthTestEnable;
    bool      depthWriteEnable;
    CompareOp depthCompareOp;

    // todo: support stencil
    bool      stencilTestEnable;

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
    CullMode  cullMode{};
    RasterizationState& SetCullMode( CullMode value )
    {
      cullMode = value;
      return *this;
    }

    PolygonMode polygonMode{};
    RasterizationState& SetPolygonMode( PolygonMode value )
    {
      polygonMode = value;
      return *this;
    }

    FrontFace frontFace{};
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
    PrimitiveTopology topology {};
    bool              primitiveRestartEnable { false };

    InputAssemblyState& SetTopology( PrimitiveTopology value )
    {
      topology = value;
      return *this;
    }

    InputAssemblyState& SetPrimitiveRestartEnable( bool value )
    {
      primitiveRestartEnable = value;
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
      Binding( Buffer* _buffer, uint32_t _stride, VertexInputRate _inputRate )
      : buffer( _buffer ), stride( _stride ), inputRate( _inputRate ) {}
      Graphics::API::Buffer* buffer;
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

    std::vector<Binding>    bufferBindings;
    std::vector<Attribute>  attributes;
  };



};
}
}
}
#endif // DALI_GRAPHICS_API_PIPELINE_H
