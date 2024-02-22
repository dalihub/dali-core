#ifndef DALI_GRAPHICS_API_TYPES_H
#define DALI_GRAPHICS_API_TYPES_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <vector>
#include <string>

namespace Dali
{
namespace Graphics
{
class Texture;
class Buffer;
class Shader;
class Framebuffer;
using Extension = void*;

/**
 *
 */
struct Offset2D
{
  int32_t x = 0;
  int32_t y = 0;
};

struct Extent2D
{
  uint32_t width;
  uint32_t height;
};

/**
 *
 */
struct Rect2D
{
  int32_t x = 0;
  int32_t y = 0;
  uint32_t width = 0;
  uint32_t height = 0;
};

/**
 * Structure represents area of viewport
 */
struct Viewport
{
  float x = 0.0f;
  float y = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
  float minDepth = 0.0f;
  float maxDepth = 0.0f;
};

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
 * Sampler address ( wrapping ) mode
 */
enum class SamplerAddressMode
{
  REPEAT,
  MIRRORED_REPEAT,
  CLAMP_TO_EDGE,
  CLAMP_TO_BORDER,
  MIRROR_CLAMP_TO_EDGE
};

/**
 * Filtering
 */
enum class SamplerFilter
{
  NEAREST,
  LINEAR
};

/**
 * Mipmap mode
 */
enum class SamplerMipmapMode
{
  NEAREST,
  LINEAR
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
  const Shader* shaderProgram { nullptr };

  Extension   extension             { nullptr };

  ShaderState& SetShaderProgram( const Shader& value )
  {
    shaderProgram = &value;
    return *this;
  }
};

struct FramebufferState
{
  const Framebuffer *framebuffer{ nullptr };

  Extension extension{ nullptr };

  FramebufferState& SetFramebuffer( const Framebuffer& value )
  {

    framebuffer = &value;
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
 * Describes stencil operation state
 */
struct StencilOpState
{
  StencilOp failOp        {};
  StencilOp passOp        {};
  StencilOp depthFailOp   {};
  CompareOp compareOp     {};
  uint32_t  compareMask   { 0u };
  uint32_t  writeMask     { 0u };
  uint32_t  reference     { 0u };

  StencilOpState& SetFailOp( StencilOp value )
  {
    failOp = value;
    return *this;
  }

  StencilOpState& SetPassOp( StencilOp value )
  {
    failOp = value;
    return *this;
  }

  StencilOpState& SetDepthFailOp( StencilOp value )
  {
    failOp = value;
    return *this;
  }

  StencilOpState& SetCompareOp( CompareOp value )
  {
    compareOp = value;
    return *this;
  }

  StencilOpState& SetCompareMask( uint32_t value )
  {
    compareMask = value;
    return *this;
  }

  StencilOpState& SetWriteMask( uint32_t value )
  {
    writeMask = value;
    return *this;
  }

  StencilOpState& SetReference( uint32_t value )
  {
    reference = value;
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
  StencilOpState front {};
  StencilOpState back {};

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

  DepthStencilState& SetFront( StencilOpState value)
  {
    front = value;
    return *this;
  }

  DepthStencilState& SetBack( StencilOpState value)
  {
    back = value;
    return *this;
  }

  DepthStencilState& SetStencilTestEnable( bool value)
  {
    stencilTestEnable = value;
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
 * Pipeline dynamic state bits
 */
namespace PipelineDynamicStateBits
{
const uint32_t VIEWPORT_BIT                   = 1 << 0;
const uint32_t SCISSOR_BIT                    = 1 << 1;
const uint32_t LINE_WIDTH_BIT                 = 1 << 2;
const uint32_t DEPTH_BIAS_BIT                 = 1 << 3;
const uint32_t BLEND_CONSTANTS_BIT            = 1 << 4;
const uint32_t DEPTH_BOUNDS_BIT               = 1 << 5;
const uint32_t STENCIL_COMPARE_MASK_BIT       = 1 << 6;
const uint32_t STENCIL_WRITE_MASK_BIT         = 1 << 7;
const uint32_t STENCIL_REFERENCE_BIT          = 1 << 8;
}

const uint32_t PIPELINE_DYNAMIC_STATE_COUNT( 9u );

using PipelineDynamicStateMask = uint32_t;

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

/**
 * List of all possible formats
 * Not all formats may be supported
 */
enum class Format
{
  UNDEFINED,
  // GLES compatible, luminance doesn't exist in Vulkan
  L8,
  L8A8,

  // Vulkan compatible
  R4G4_UNORM_PACK8,
  R4G4B4A4_UNORM_PACK16,
  B4G4R4A4_UNORM_PACK16,
  R5G6B5_UNORM_PACK16,
  B5G6R5_UNORM_PACK16,
  R5G5B5A1_UNORM_PACK16,
  B5G5R5A1_UNORM_PACK16,
  A1R5G5B5_UNORM_PACK16,
  R8_UNORM,
  R8_SNORM,
  R8_USCALED,
  R8_SSCALED,
  R8_UINT,
  R8_SINT,
  R8_SRGB,
  R8G8_UNORM,
  R8G8_SNORM,
  R8G8_USCALED,
  R8G8_SSCALED,
  R8G8_UINT,
  R8G8_SINT,
  R8G8_SRGB,
  R8G8B8_UNORM,
  R8G8B8_SNORM,
  R8G8B8_USCALED,
  R8G8B8_SSCALED,
  R8G8B8_UINT,
  R8G8B8_SINT,
  R8G8B8_SRGB,
  B8G8R8_UNORM,
  B8G8R8_SNORM,
  B8G8R8_USCALED,
  B8G8R8_SSCALED,
  B8G8R8_UINT,
  B8G8R8_SINT,
  B8G8R8_SRGB,
  R8G8B8A8_UNORM,
  R8G8B8A8_SNORM,
  R8G8B8A8_USCALED,
  R8G8B8A8_SSCALED,
  R8G8B8A8_UINT,
  R8G8B8A8_SINT,
  R8G8B8A8_SRGB,
  B8G8R8A8_UNORM,
  B8G8R8A8_SNORM,
  B8G8R8A8_USCALED,
  B8G8R8A8_SSCALED,
  B8G8R8A8_UINT,
  B8G8R8A8_SINT,
  B8G8R8A8_SRGB,
  A8B8G8R8_UNORM_PACK32,
  A8B8G8R8_SNORM_PACK32,
  A8B8G8R8_USCALED_PACK32,
  A8B8G8R8_SSCALED_PACK32,
  A8B8G8R8_UINT_PACK32,
  A8B8G8R8_SINT_PACK32,
  A8B8G8R8_SRGB_PACK32,
  A2R10G10B10_UNORM_PACK32,
  A2R10G10B10_SNORM_PACK32,
  A2R10G10B10_USCALED_PACK32,
  A2R10G10B10_SSCALED_PACK32,
  A2R10G10B10_UINT_PACK32,
  A2R10G10B10_SINT_PACK32,
  A2B10G10R10_UNORM_PACK32,
  A2B10G10R10_SNORM_PACK32,
  A2B10G10R10_USCALED_PACK32,
  A2B10G10R10_SSCALED_PACK32,
  A2B10G10R10_UINT_PACK32,
  A2B10G10R10_SINT_PACK32,
  R16_UNORM,
  R16_SNORM,
  R16_USCALED,
  R16_SSCALED,
  R16_UINT,
  R16_SINT,
  R16_SFLOAT,
  R16G16_UNORM,
  R16G16_SNORM,
  R16G16_USCALED,
  R16G16_SSCALED,
  R16G16_UINT,
  R16G16_SINT,
  R16G16_SFLOAT,
  R16G16B16_UNORM,
  R16G16B16_SNORM,
  R16G16B16_USCALED,
  R16G16B16_SSCALED,
  R16G16B16_UINT,
  R16G16B16_SINT,
  R16G16B16_SFLOAT,
  R16G16B16A16_UNORM,
  R16G16B16A16_SNORM,
  R16G16B16A16_USCALED,
  R16G16B16A16_SSCALED,
  R16G16B16A16_UINT,
  R16G16B16A16_SINT,
  R16G16B16A16_SFLOAT,
  R32_UINT,
  R32_SINT,
  R32_SFLOAT,
  R32G32_UINT,
  R32G32_SINT,
  R32G32_SFLOAT,
  R32G32B32_UINT,
  R32G32B32_SINT,
  R32G32B32_SFLOAT,
  R32G32B32A32_UINT,
  R32G32B32A32_SINT,
  R32G32B32A32_SFLOAT,
  R64_UINT,
  R64_SINT,
  R64_SFLOAT,
  R64G64_UINT,
  R64G64_SINT,
  R64G64_SFLOAT,
  R64G64B64_UINT,
  R64G64B64_SINT,
  R64G64B64_SFLOAT,
  R64G64B64A64_UINT,
  R64G64B64A64_SINT,
  R64G64B64A64_SFLOAT,
  B10G11R11_UFLOAT_PACK32,
  E5B9G9R9_UFLOAT_PACK32,
  D16_UNORM,
  X8_D24_UNORM_PACK32,
  D32_SFLOAT,
  S8_UINT,
  D16_UNORM_S8_UINT,
  D24_UNORM_S8_UINT,
  D32_SFLOAT_S8_UINT,
  BC1_RGB_UNORM_BLOCK,
  BC1_RGB_SRGB_BLOCK,
  BC1_RGBA_UNORM_BLOCK,
  BC1_RGBA_SRGB_BLOCK,
  BC2_UNORM_BLOCK,
  BC2_SRGB_BLOCK,
  BC3_UNORM_BLOCK,
  BC3_SRGB_BLOCK,
  BC4_UNORM_BLOCK,
  BC4_SNORM_BLOCK,
  BC5_UNORM_BLOCK,
  BC5_SNORM_BLOCK,
  BC6H_UFLOAT_BLOCK,
  BC6H_SFLOAT_BLOCK,
  BC7_UNORM_BLOCK,
  BC7_SRGB_BLOCK,
  ETC2_R8G8B8_UNORM_BLOCK,
  ETC2_R8G8B8_SRGB_BLOCK,
  ETC2_R8G8B8A1_UNORM_BLOCK,
  ETC2_R8G8B8A1_SRGB_BLOCK,
  ETC2_R8G8B8A8_UNORM_BLOCK,
  ETC2_R8G8B8A8_SRGB_BLOCK,
  EAC_R11_UNORM_BLOCK,
  EAC_R11_SNORM_BLOCK,
  EAC_R11G11_UNORM_BLOCK,
  EAC_R11G11_SNORM_BLOCK,
  ASTC_4x4_UNORM_BLOCK,
  ASTC_4x4_SRGB_BLOCK,
  ASTC_5x4_UNORM_BLOCK,
  ASTC_5x4_SRGB_BLOCK,
  ASTC_5x5_UNORM_BLOCK,
  ASTC_5x5_SRGB_BLOCK,
  ASTC_6x5_UNORM_BLOCK,
  ASTC_6x5_SRGB_BLOCK,
  ASTC_6x6_UNORM_BLOCK,
  ASTC_6x6_SRGB_BLOCK,
  ASTC_8x5_UNORM_BLOCK,
  ASTC_8x5_SRGB_BLOCK,
  ASTC_8x6_UNORM_BLOCK,
  ASTC_8x6_SRGB_BLOCK,
  ASTC_8x8_UNORM_BLOCK,
  ASTC_8x8_SRGB_BLOCK,
  ASTC_10x5_UNORM_BLOCK,
  ASTC_10x5_SRGB_BLOCK,
  ASTC_10x6_UNORM_BLOCK,
  ASTC_10x6_SRGB_BLOCK,
  ASTC_10x8_UNORM_BLOCK,
  ASTC_10x8_SRGB_BLOCK,
  ASTC_10x10_UNORM_BLOCK,
  ASTC_10x10_SRGB_BLOCK,
  ASTC_12x10_UNORM_BLOCK,
  ASTC_12x10_SRGB_BLOCK,
  ASTC_12x12_UNORM_BLOCK,
  ASTC_12x12_SRGB_BLOCK,
  PVRTC1_2BPP_UNORM_BLOCK_IMG,
  PVRTC1_4BPP_UNORM_BLOCK_IMG,
  PVRTC2_2BPP_UNORM_BLOCK_IMG,
  PVRTC2_4BPP_UNORM_BLOCK_IMG,
  PVRTC1_2BPP_SRGB_BLOCK_IMG,
  PVRTC1_4BPP_SRGB_BLOCK_IMG,
  PVRTC2_2BPP_SRGB_BLOCK_IMG,
  PVRTC2_4BPP_SRGB_BLOCK_IMG,
};

enum class BufferUsage
{
  TRANSFER_SRC = 1 << 0,
  TRANSFER_DST = 1 << 1,
  UNIFORM_TEXEL_BUFFER = 1 << 2,
  STORAGE_TEXEL_BUFFER = 1 << 3,
  UNIFORM_BUFFER = 1 << 4,
  STORAGE_BUFFER = 1 << 5,
  INDEX_BUFFER = 1 << 6,
  VERTEX_BUFFER = 1 << 7,
  INDIRECT_BUFFER = 1 << 8
};

using BufferUsageFlags = uint32_t;

inline BufferUsageFlags operator|( BufferUsageFlags flags, BufferUsage usage )
{
  flags |= static_cast<uint32_t>( usage );
  return flags;
}

struct MemoryRequirements
{
  size_t size;
  size_t alignment;
};

using TextureUpdateFlags = uint32_t;
enum class TextureUpdateFlagBits
{
  KEEP_SOURCE = 1 << 0,
};

struct TextureUpdateInfo
{
  Texture*      dstTexture;
  Offset2D      dstOffset2D;
  uint32_t      layer;
  uint32_t      level;

  uint32_t      srcReference;
  Extent2D      srcExtent2D;
  uint32_t      srcOffset;
  uint32_t      srcSize;
};

struct TextureUpdateSourceInfo
{
  enum class Type
  {
    Buffer,
    Memory,
    Texture
  };

  Type sourceType;

  struct BufferSource
  {
    Buffer* buffer;
  } bufferSource;

  struct MemorySource
  {
    void* pMemory;
  } memorySource;

  struct TextureSource
  {
    Texture* texture;
  } textureSource;
};

struct TextureProperties
{
  Format    format;
  Format    format1;
  bool      emulated;
  bool      compressed;
  bool      packed;
  Extent2D  extent2D;
  bool      directWriteAccessEnabled;
};

/**
 * Texture tiling that directly refers to the tiling
 * mode supported by the Vulkan. Other implementations
 * of the backend may ignore the value.
 */
enum class TextureTiling
{
  OPTIMAL,
  LINEAR
};

} // Graphics
} // Dali

#endif //DALI_GRAPHICS_API_TYPES_H
