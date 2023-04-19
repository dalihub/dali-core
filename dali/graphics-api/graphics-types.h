#ifndef DALI_GRAPHICS_API_TYPES
#define DALI_GRAPHICS_API_TYPES

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <dali/public-api/signals/callback.h>

namespace Dali
{
namespace Graphics
{
class Buffer;
class CommandBuffer;
class Framebuffer;
class Program;
class Shader;
class Texture;

/**
 * @brief Structure describes 2D offset
 */
struct Offset2D
{
  int32_t x = 0;
  int32_t y = 0;
};

/**
 * @brief Structure describes 2D dimensions
 */
struct Extent2D
{
  uint32_t width;
  uint32_t height;
};

/**
 * @brief Structure describes 2D rectangle (offset, extent)
 */
struct Rect2D
{
  int32_t  x      = 0;
  int32_t  y      = 0;
  uint32_t width  = 0;
  uint32_t height = 0;
};

/**
 * @brief Structure represents area of viewport
 */
struct Viewport
{
  float x        = 0.0f;
  float y        = 0.0f;
  float width    = 0.0f;
  float height   = 0.0f;
  float minDepth = 0.0f;
  float maxDepth = 0.0f;
};

/**
 * @brief Describes vertex attribute input rate
 */
enum class VertexInputRate
{
  PER_VERTEX,  ///< Attribute read per vertex
  PER_INSTANCE ///< Attribute read per instance
};

/**
 * @brief Vertex input format
 *
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
 * @brief Logic operators used by color blending state
 * when logicOpEnable is set.
 */
enum class LogicOp
{
  CLEAR         = 0,
  AND           = 1,
  AND_REVERSE   = 2,
  COPY          = 3,
  AND_INVERTED  = 4,
  NO_OP         = 5,
  XOR           = 6,
  OR            = 7,
  NOR           = 8,
  EQUIVALENT    = 9,
  INVERT        = 10,
  OR_REVERSE    = 11,
  COPY_INVERTED = 12,
  OR_INVERTED   = 13,
  NAND          = 14,
  SET           = 15,
};

/**
 * @brief Blend factors
 */
enum class BlendFactor
{
  ZERO                     = 0,
  ONE                      = 1,
  SRC_COLOR                = 2,
  ONE_MINUS_SRC_COLOR      = 3,
  DST_COLOR                = 4,
  ONE_MINUS_DST_COLOR      = 5,
  SRC_ALPHA                = 6,
  ONE_MINUS_SRC_ALPHA      = 7,
  DST_ALPHA                = 8,
  ONE_MINUS_DST_ALPHA      = 9,
  CONSTANT_COLOR           = 10,
  ONE_MINUS_CONSTANT_COLOR = 11,
  CONSTANT_ALPHA           = 12,
  ONE_MINUS_CONSTANT_ALPHA = 13,
  SRC_ALPHA_SATURATE       = 14,
  SRC1_COLOR               = 15,
  ONE_MINUS_SRC1_COLOR     = 16,
  SRC1_ALPHA               = 17,
  ONE_MINUS_SRC1_ALPHA     = 18,
};

/**
 * @brief Blend operators
 */
enum class BlendOp
{
  ADD              = 0,
  SUBTRACT         = 1,
  REVERSE_SUBTRACT = 2,
  MIN              = 3,
  MAX              = 4,
  MULTIPLY         = 5,
  SCREEN           = 6,
  OVERLAY          = 7,
  DARKEN           = 8,
  LIGHTEN          = 9,
  COLOR_DODGE      = 10,
  COLOR_BURN       = 11,
  HARD_LIGHT       = 12,
  SOFT_LIGHT       = 13,
  DIFFERENCE       = 14,
  EXCLUSION        = 15,
  HUE              = 16,
  SATURATION       = 17,
  COLOR            = 18,
  LUMINOSITY       = 19,
};

const BlendOp ADVANCED_BLEND_OPTIONS_START(BlendOp::MULTIPLY);

/**
 * @brief Compare operators
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
 * @brief Stencil operators
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
 * @brief Backface culling modes
 */
enum class CullMode
{
  NONE,
  FRONT,
  BACK,
  FRONT_AND_BACK
};

/**
 * @brief Polygon drawing modes
 */
enum class PolygonMode
{
  FILL,
  LINE,
  POINT
};

/**
 * @brief Front face direction
 */
enum class FrontFace
{
  COUNTER_CLOCKWISE,
  CLOCKWISE
};

/**
 * @brief Primitive geometry topology
 */
enum class PrimitiveTopology
{
  POINT_LIST,
  LINE_LIST,
  LINE_LOOP,
  LINE_STRIP,
  TRIANGLE_LIST,
  TRIANGLE_STRIP,
  TRIANGLE_FAN
};

/**
 * @brief Sampler address ( wrapping ) mode
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
 * @brief Filtering mode
 */
enum class SamplerFilter
{
  NEAREST,
  LINEAR
};

/**
 * @brief Mipmap mode
 */
enum class SamplerMipmapMode
{
  NONE,
  NEAREST,
  LINEAR
};

/**
 * @brief Describes pipeline's color blend state
 */
struct ColorBlendState
{
  bool        logicOpEnable           = false;
  LogicOp     logicOp                 = {};
  float       blendConstants[4]       = {0.0f, 0.0f, 0.0f, 0.0f};
  bool        blendEnable             = false;
  BlendFactor srcColorBlendFactor     = BlendFactor::ZERO;
  BlendFactor dstColorBlendFactor     = BlendFactor::ZERO;
  BlendOp     colorBlendOp            = {};
  BlendFactor srcAlphaBlendFactor     = BlendFactor::ZERO;
  BlendFactor dstAlphaBlendFactor     = BlendFactor::ZERO;
  BlendOp     alphaBlendOp            = {};
  uint32_t    colorComponentWriteBits = {0u};

  auto& SetLogicOpEnable(bool value)
  {
    logicOpEnable = value;
    return *this;
  }

  auto& SetLogicOp(LogicOp value)
  {
    logicOp = value;
    return *this;
  }

  auto& SetBlendConstants(float value[4])
  {
    std::copy(value, value + 4, blendConstants);
    return *this;
  }

  auto& SetBlendEnable(bool value)
  {
    blendEnable = value;
    return *this;
  }

  auto& SetSrcColorBlendFactor(BlendFactor value)
  {
    srcColorBlendFactor = value;
    return *this;
  }

  auto& SetDstColorBlendFactor(BlendFactor value)
  {
    dstColorBlendFactor = value;
    return *this;
  }

  auto& SetColorBlendOp(BlendOp value)
  {
    colorBlendOp = value;
    return *this;
  }

  auto& SetSrcAlphaBlendFactor(BlendFactor value)
  {
    srcAlphaBlendFactor = value;
    return *this;
  }

  auto& SetDstAlphaBlendFactor(BlendFactor value)
  {
    dstAlphaBlendFactor = value;
    return *this;
  }

  auto& SetAlphaBlendOp(BlendOp value)
  {
    alphaBlendOp = value;
    return *this;
  }

  auto& SetColorComponentsWriteBits(uint32_t value)
  {
    colorComponentWriteBits = value;
    return *this;
  }
};

/**
 * @brief Program State
 */
struct ProgramState
{
  const Program* program{nullptr};

  auto& SetProgram(const Program& value)
  {
    program = &value;
    return *this;
  }
};

/**
 * @brief  Framebuffer state.
 */
struct FramebufferState
{
  const Framebuffer* framebuffer{nullptr};

  auto& SetFramebuffer(const Framebuffer& value)
  {
    framebuffer = &value;
    return *this;
  }
};

/**
 * @brief Describes pipeline's viewport and scissor state
 */
struct ViewportState
{
  Viewport viewport{0.0, 0.0, 0.0, 0.0};
  Rect2D   scissor{0, 0, 0, 0};
  bool     scissorTestEnable{false};

  auto& SetViewport(const Viewport& value)
  {
    viewport = value;
    return *this;
  }

  auto& SetScissor(const Rect2D& value)
  {
    scissor = value;
    return *this;
  }

  auto& SetScissorTestEnable(bool value)
  {
    scissorTestEnable = value;
    return *this;
  }
};

/**
 * @brief Describes stencil operation state
 */
struct StencilOpState
{
  StencilOp failOp{};
  StencilOp passOp{};
  StencilOp depthFailOp{};
  CompareOp compareOp{};
  uint32_t  compareMask{0u};
  uint32_t  writeMask{0u};
  uint32_t  reference{0u};

  auto& SetFailOp(StencilOp value)
  {
    failOp = value;
    return *this;
  }

  auto& SetPassOp(StencilOp value)
  {
    failOp = value;
    return *this;
  }

  auto& SetDepthFailOp(StencilOp value)
  {
    failOp = value;
    return *this;
  }

  auto& SetCompareOp(CompareOp value)
  {
    compareOp = value;
    return *this;
  }

  auto& SetCompareMask(uint32_t value)
  {
    compareMask = value;
    return *this;
  }

  auto& SetWriteMask(uint32_t value)
  {
    writeMask = value;
    return *this;
  }

  auto& SetReference(uint32_t value)
  {
    reference = value;
    return *this;
  }
};

/**
 * @brief Describes pipeline's viewport and scissor state
 */
struct DepthStencilState
{
  bool      depthTestEnable{false};
  bool      depthWriteEnable{false};
  CompareOp depthCompareOp{};

  bool           stencilTestEnable{false};
  StencilOpState front{};
  StencilOpState back{};

  auto& SetDepthTestEnable(bool value)
  {
    depthTestEnable = value;
    return *this;
  }

  auto& SetDepthWriteEnable(bool value)
  {
    depthWriteEnable = value;
    return *this;
  }

  auto& SetDepthCompareOp(CompareOp value)
  {
    depthCompareOp = value;
    return *this;
  }

  auto& SetFront(StencilOpState value)
  {
    front = value;
    return *this;
  }

  auto& SetBack(StencilOpState value)
  {
    back = value;
    return *this;
  }

  auto& SetStencilTestEnable(bool value)
  {
    stencilTestEnable = value;
    return *this;
  }
};

/**
 * @brief Rasterization state descriptor
 */
struct RasterizationState
{
  CullMode    cullMode{};
  PolygonMode polygonMode{};
  FrontFace   frontFace{};

  auto& SetCullMode(CullMode value)
  {
    cullMode = value;
    return *this;
  }

  auto& SetPolygonMode(PolygonMode value)
  {
    polygonMode = value;
    return *this;
  }

  auto& SetFrontFace(FrontFace value)
  {
    frontFace = value;
    return *this;
  }
};

/**
 * @brief Input assembly state descriptor.
 *
 * Structure describes the topology for submitted
 * geometry.
 */
struct InputAssemblyState
{
  PrimitiveTopology topology{};
  bool              primitiveRestartEnable{true};

  auto& SetTopology(PrimitiveTopology value)
  {
    topology = value;
    return *this;
  }

  auto& SetPrimitiveRestartEnable(bool value)
  {
    primitiveRestartEnable = true;
    return *this;
  }
};

/**
 * @brief Pipeline dynamic state bits
 */
namespace PipelineDynamicStateBits
{
const uint32_t VIEWPORT_BIT             = 1 << 0;
const uint32_t SCISSOR_BIT              = 1 << 1;
const uint32_t LINE_WIDTH_BIT           = 1 << 2;
const uint32_t DEPTH_BIAS_BIT           = 1 << 3;
const uint32_t BLEND_CONSTANTS_BIT      = 1 << 4;
const uint32_t DEPTH_BOUNDS_BIT         = 1 << 5;
const uint32_t STENCIL_COMPARE_MASK_BIT = 1 << 6;
const uint32_t STENCIL_WRITE_MASK_BIT   = 1 << 7;
const uint32_t STENCIL_REFERENCE_BIT    = 1 << 8;
} // namespace PipelineDynamicStateBits

const uint32_t PIPELINE_DYNAMIC_STATE_COUNT(9u);

using PipelineDynamicStateMask = uint32_t;

/**
 * @brief Structure describes vertex input state of the pipeline.
 * It specifies buffer binding and attribute format to be used.
 */
struct VertexInputState
{
  VertexInputState() = default;

  /**
   * @brief Vertex buffer binding
   */
  struct Binding
  {
    Binding(uint32_t _stride, VertexInputRate _inputRate)
    : stride(_stride),
      inputRate(_inputRate)
    {
    }
    uint32_t        stride;
    VertexInputRate inputRate;
  };

  /**
   * @brief Attribute description
   */
  struct Attribute
  {
    Attribute(uint32_t _location, uint32_t _binding, uint32_t _offset, VertexInputFormat _format)
    : location(_location),
      binding(_binding),
      offset(_offset),
      format(_format)
    {
    }

    uint32_t          location;
    uint32_t          binding;
    uint32_t          offset;
    VertexInputFormat format;
  };

  VertexInputState(std::vector<Binding> _bufferBindings, std::vector<Attribute> _attributes)
  : bufferBindings(std::move(_bufferBindings)),
    attributes(std::move(_attributes))
  {
  }

  std::vector<Binding>   bufferBindings{};
  std::vector<Attribute> attributes{};
};

/**
 * @brief List of all possible formats
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
  R11G11B10_UFLOAT_PACK32,
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

/**
 * @brief Flags specifying a buffer usage
 */
enum class BufferUsage
{
  TRANSFER_SRC         = 1 << 0,
  TRANSFER_DST         = 1 << 1,
  UNIFORM_TEXEL_BUFFER = 1 << 2,
  STORAGE_TEXEL_BUFFER = 1 << 3,
  UNIFORM_BUFFER       = 1 << 4,
  STORAGE_BUFFER       = 1 << 5,
  INDEX_BUFFER         = 1 << 6,
  VERTEX_BUFFER        = 1 << 7,
  INDIRECT_BUFFER      = 1 << 8,
};

using BufferUsageFlags = uint32_t;

inline BufferUsageFlags operator|(BufferUsageFlags flags, BufferUsage usage)
{
  flags |= static_cast<uint32_t>(usage);
  return flags;
}

/**
 * @brief Buffer property bits
 *
 * Use these bits to set BufferPropertiesFlags.
 */
enum class BufferPropertiesFlagBit : uint32_t
{
  CPU_ALLOCATED    = 1 << 0, ///< Buffer is allocated on the CPU side
  TRANSIENT_MEMORY = 1 << 1, ///< Buffer memory will be short-lived
};

/**
 * @brief BufferPropetiesFlags alters behaviour of implementation
 */
using BufferPropertiesFlags = uint32_t;

inline BufferPropertiesFlags operator|(BufferPropertiesFlags flags, BufferPropertiesFlagBit usage)
{
  flags |= static_cast<uint32_t>(usage);
  return flags;
}

/**
 * @brief The structure describes memory requirements of GPU resource (texture, buffer)
 */
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

/**
 * @brief Texture update info
 *
 * Describes the texture update to be executed by
 * Controller::UpdateTextures()
 */
struct TextureUpdateInfo
{
  Texture* dstTexture{};
  Offset2D dstOffset2D;
  uint32_t layer{};
  uint32_t level{};

  uint32_t srcReference{};
  Extent2D srcExtent2D{};
  uint32_t srcOffset{};
  uint32_t srcSize{};
  uint32_t srcStride{};
  Format   srcFormat{}; ///< Should match dstTexture's format, otherwise conversion may occur
};

/**
 * @brief Texture update source info
 *
 * Describes the source of data (memory, buffer or another texture)
 * to be used when updating textures using Controller::UpdateTextures().
 */
struct TextureUpdateSourceInfo
{
  enum class Type
  {
    BUFFER,
    MEMORY,
    TEXTURE
  };

  Type sourceType;

  struct BufferSource
  {
    Buffer* buffer;
  } bufferSource;

  struct MemorySource
  {
    void* memory;
  } memorySource;

  struct TextureSource
  {
    Texture* texture;
  } textureSource;
};

/**
 * @brief Properties of texture
 */
struct TextureProperties
{
  Format   format;                   ///< Texture format
  Format   format1;                  ///< Texture format (if emulated)
  bool     emulated;                 ///< Format is emulated (for example RGB as RGBA)
  bool     compressed;               ///< Texture is compressed
  bool     packed;                   ///< Texture is packed
  Extent2D extent2D;                 ///< Size of texture
  bool     directWriteAccessEnabled; ///< Direct write access (mappable textures)
  uint32_t nativeHandle;             ///< Native texture handle
};

/**
 * @brief Texture tiling that directly refers to the tiling
 * mode supported by the Vulkan. Other implementations
 * of the backend may ignore the value.
 */
enum class TextureTiling
{
  OPTIMAL,
  LINEAR
};

/**
 * @brief Texture color attachment used by FramebufferCreateInfo
 */
struct ColorAttachment
{
  uint32_t attachmentId;
  Texture* texture;
  uint32_t layerId;
  uint32_t levelId;
};

/**
 * @brief Depth stencil attachment used by FramebufferCreateInfo
 */
struct DepthStencilAttachment
{
  enum class Usage
  {
    WRITE, // If no texture, will create a RenderBuffer instead
    NONE   // If no attachment/RenderBuffer required
  };
  Texture* depthTexture{nullptr};
  Texture* stencilTexture{nullptr};
  uint32_t depthLevel{0};
  uint32_t stencilLevel{0};
  Usage    depthUsage{Usage::NONE};
  Usage    stencilUsage{Usage::NONE};
};

/**
 * @brief Submit flags
 */
using SubmitFlags = uint32_t;

/**
 * Submit flag bits
 */
enum class SubmitFlagBits : uint32_t
{
  FLUSH         = 1 << 0, // Flush immediately
  DONT_OPTIMIZE = 1 << 1  // Tells controller not to optimize commands
};

template<typename T>
inline SubmitFlags operator|(T flags, SubmitFlagBits bit)
{
  return static_cast<SubmitFlags>(flags) | static_cast<SubmitFlags>(bit);
}

/**
 * @brief Describes command buffers submission
 */
struct SubmitInfo
{
  std::vector<CommandBuffer*> cmdBuffer;
  SubmitFlags                 flags;
};

/**
 * @brief Shader language enum
 */
enum class ShaderLanguage
{
  GLSL_1,
  GLSL_3_1,
  GLSL_3_2,
  SPIRV_1_0,
  SPIRV_1_1,
};

/**
 * @brief Pipeline stages
 */
enum class PipelineStage
{
  TOP_OF_PIPELINE,
  VERTEX_SHADER,
  GEOMETRY_SHADER,
  FRAGMENT_SHADER,
  COMPUTE_SHADER,
  TESSELATION_CONTROL,
  TESSELATION_EVALUATION,
  BOTTOM_OF_PIPELINE
};

/**
 * @brief Vertex attribute format
 *
 * TODO: to be replaced with Format
 */
enum class VertexInputAttributeFormat
{
  UNDEFINED,
  FLOAT,
  INTEGER,
  VEC2,
  VEC3,
  VEC4
};

/**
 * @brief Uniform class
 */
enum class UniformClass
{
  SAMPLER,
  IMAGE,
  COMBINED_IMAGE_SAMPLER,
  UNIFORM_BUFFER,
  UNIFORM,
  UNDEFINED
};

/**
 * @brief Type of texture
 */
enum class TextureType
{
  TEXTURE_2D,
  TEXTURE_3D,
  TEXTURE_CUBEMAP,
};

/**
 * @brief The information of the uniform
 */
struct UniformInfo
{
  std::string  name{""};
  UniformClass uniformClass{UniformClass::UNDEFINED};
  uint32_t     binding{0u};
  uint32_t     bufferIndex{0u};
  uint32_t     offset{0u};
  uint32_t     location{0u};
  uint32_t     elementCount{0u};

  bool operator==(const UniformInfo& rhs)
  {
    return name == rhs.name &&
           uniformClass == rhs.uniformClass &&
           binding == rhs.binding &&
           bufferIndex == rhs.bufferIndex &&
           offset == rhs.offset &&
           location == rhs.location &&
           elementCount == rhs.elementCount;
  }
};

/**
 * @brief The information of the uniform block
 */
struct UniformBlockInfo
{
  std::string              name{""};
  uint32_t                 descriptorSet{0u};
  uint32_t                 binding{0u};
  uint32_t                 size{0u};
  std::vector<UniformInfo> members{};
};

/**
 * @brief Describes pipeline's shading stages
 *
 * Shader state binds shader and pipeline stage that the
 * shader will be executed. The shader may be created with
 * pipeline stage and the pipelineStage member may be ignored
 * by setting inheritPipelineStage to true.
 */
struct ShaderState
{
  const Shader* shader{nullptr};             // shader to attach
  PipelineStage pipelineStage{};             // pipeline stage to execute the shader
  bool          inheritPipelineStage{false}; // stage inheritance

  auto& SetShader(const Shader& value)
  {
    shader = &value;
    return *this;
  }

  auto& SetPipelineStage(PipelineStage value)
  {
    pipelineStage = value;
    return *this;
  }

  auto& SetInheritPipelineStage(bool value)
  {
    inheritPipelineStage = value;
    return *this;
  }
};

/**
 * @brief Flag determining usage of texture
 */
using TextureUsageFlags = uint32_t;
enum class TextureUsageFlagBits : uint32_t
{
  SAMPLE                   = 1 << 0,
  COLOR_ATTACHMENT         = 1 << 1,
  DEPTH_STENCIL_ATTACHMENT = 1 << 2,
  DONT_CARE                = 1 << 4,
};

template<typename T>
inline TextureUsageFlags operator|(T flags, TextureUsageFlagBits bit)
{
  return static_cast<TextureUsageFlags>(flags) | static_cast<TextureUsageFlags>(bit);
}

using TextureFormat = Dali::Graphics::Format;

/**
 * @brief Texture mipmap disable/enable enum
 */
enum class TextureMipMapFlag
{
  ENABLED,
  DISABLED,
};

/**
 * @brief Depth/stencil attachment flag
 */
enum class TextureDepthStencilFlag
{
  NONE,
  DEPTH,
  STENCIL,
  DEPTH_STENCIL,
};

/**
 * @brief Layout of texture
 *
 * Specifies how the memory will be allocated, organized and accessed.
 */
enum class TextureLayout
{
  LINEAR, ///< Creates linear memory, mapping possible
  OPTIMAL ///< Usually, read-only memory, driver-optimal layout
};

/**
 * @brief Level of command buffer
 */
enum class CommandBufferLevel
{
  PRIMARY,  ///< Primary buffer can be executed on its own
  SECONDARY ///< Secondary buffer must be executed within scope of Primary buffer
};

/**
 * @brief Enum indicating whether shader source
 * is text-based or binary.
 */
enum class ShaderSourceMode
{
  TEXT,
  BINARY
};

/**
 * @brief Memory usage flags to be set when mapping the buffer
 */
using MemoryUsageFlags = uint32_t;
enum class MemoryUsageFlagBits : uint32_t
{
  WRITE        = 1 << 0,
  READ         = 1 << 1,
  PERSISTENT   = 1 << 2,
  ASYNCHRONOUS = 1 << 3,
  DONT_CARE    = 1 << 4,
};

template<typename T>
inline MemoryUsageFlags operator|(T flags, MemoryUsageFlagBits bit)
{
  return static_cast<MemoryUsageFlags>(flags) | static_cast<MemoryUsageFlags>(bit);
}

/**
 * @brief Describes buffer mapping details
 */
struct MapBufferInfo
{
  Buffer*          buffer;
  MemoryUsageFlags usage;
  uint32_t         offset;
  uint32_t         size;
};

/**
 * @brief Describes buffer mapping details
 * TODO: mapping by texture level and layer
 */
struct MapTextureInfo
{
  Texture*         texture;
  MemoryUsageFlags usage;
  uint32_t         offset;
  uint32_t         size;
};

/**
 * @brief GraphicsStructureType enum is used by all create info structures
 * in order to identify by the implementation which structure it is
 * dealing with.
 */
enum class GraphicsStructureType : uint32_t
{
  BUFFER_CREATE_INFO_STRUCT,
  COMMAND_BUFFER_CREATE_INFO_STRUCT,
  FRAMEBUFFER_CREATE_INFO_STRUCT,
  PROGRAM_CREATE_INFO_STRUCT,
  PIPELINE_CREATE_INFO_STRUCT,
  RENDERPASS_CREATE_INFO_STRUCT,
  SAMPLER_CREATE_INFO_STRUCT,
  SHADER_CREATE_INFO_STRUCT,
  TEXTURE_CREATE_INFO_STRUCT,
  RENDER_TARGET_CREATE_INFO_STRUCT,
  SYNC_OBJECT_CREATE_INFO_STRUCT
};

/**
 * @brief Enum describes load operation associated
 * with particular framebuffer attachment
 */
enum class AttachmentLoadOp
{
  LOAD,     ///< Load previous content
  CLEAR,    ///< Clear the attachment
  DONT_CARE ///< Let driver decide
};

/**
 * @brief Enum describes store operation associated
 * with particular framebuffer attachment
 */
enum class AttachmentStoreOp
{
  STORE,    ///< Store content (color attachemnts)
  DONT_CARE ///< Let driver decide (depth/stencil attachemnt with no intention of reading)
};

/**
 * @brief The structure describes the read/write
 * modes of a single framebuffer attachment
 *
 * The attachment description specifies what is going to
 * happen to the attachment at the beginning and end of the
 * render pass.
 *
 * The stencil operation is separated as it may be set
 * independent from the depth component (use loadOp, storeOp
 * to set up the depth component and stencilLoadOp, stencilStoreOp
 * for stencil component).
 */
struct AttachmentDescription
{
  /**
   * @brief Sets load operation for the attachment
   *
   * @param value Load operation
   * @return this structure
   */
  auto& SetLoadOp(AttachmentLoadOp value)
  {
    loadOp = value;
    return *this;
  }

  /**
   * @brief Sets store operation for the attachment
   *
   * @param value Store operation
   * @return this structure
   */
  auto& SetStoreOp(AttachmentStoreOp value)
  {
    storeOp = value;
    return *this;
  }

  /**
   * @brief Sets load operation for the stencil part of attachment
   *
   * @param value load operation
   * @return this structure
   */
  auto& SetStencilLoadOp(AttachmentLoadOp value)
  {
    stencilLoadOp = value;
    return *this;
  }

  /**
   * @brief Sets store operation for the stencil part of attachment
   *
   * @param value store operation
   * @return this structure
   */
  auto& SetStencilStoreOp(AttachmentStoreOp value)
  {
    stencilStoreOp = value;
    return *this;
  }

  AttachmentLoadOp  loadOp{};
  AttachmentStoreOp storeOp{};
  AttachmentLoadOp  stencilLoadOp{};
  AttachmentStoreOp stencilStoreOp{};
};

/**
 * @brief Helper function to be used by the extension developers
 *
 * The value of custom type must be unique and recognizable by the
 * implementation.
 *
 * @param customValue Custom value of GraphicsStructureType
 * @return Integer converted to GraphicsStructureType
 */
inline GraphicsStructureType GraphicsExtensionType(uint32_t customValue)
{
  return static_cast<GraphicsStructureType>(customValue);
}

/**
 * @brief The allocation callbacks may be passed when the object is created.
 */
struct AllocationCallbacks
{
  void* userData                                                                          = nullptr; ///< User data passed to the allocator
  void* (*allocCallback)(size_t size, size_t alignment, void* userData)                   = nullptr;
  void* (*reallocCallback)(void* original, size_t size, size_t alignment, void* userData) = nullptr;
  void (*freeCallback)(void* memory, void* userData)                                      = nullptr;
};

/**
 * @brief The ExtensionCreateInfo structure should be a base of any
 * extension create info structure. The structure isn't virtual
 * so the implementation must prevent slicing it.
 */
struct ExtensionCreateInfo
{
  GraphicsStructureType type{};
  ExtensionCreateInfo*  nextExtension{};
};

/**
 * @brief Default deleter for graphics unique pointers
 *
 * Returned unique_ptr may require custom deleter. To get it working
 * with std::unique_ptr the custom type is used with polymorphic deleter
 */
template<class T>
struct DefaultDeleter
{
  DefaultDeleter() = default;

  /**
   * @brief Conversion constructor
   *
   * This constructor will set the lambda for type passed
   * as an argument.
   */
  template<class P, template<typename> typename U>
  DefaultDeleter(const U<P>& deleter)
  {
    deleteFunction = [](T* object) { U<P>()(static_cast<P*>(object)); };
  }

  /**
   * @brief Conversion constructor from DefaultDelete<P>
   *
   * This constructor transfers deleteFunction only
   */
  template<class P>
  explicit DefaultDeleter(const DefaultDeleter<P>& deleter)
  {
    deleteFunction = decltype(deleteFunction)(deleter.deleteFunction);
  }

  /**
   * @brief Default deleter
   *
   * Default deleter will use standard 'delete' call in order
   * to discard graphics objects unless a custom deleter was
   * used.
   *
   * @param[in] object Object to delete
   */
  void operator()(T* object)
  {
    if(deleteFunction)
    {
      deleteFunction(object);
    }
    else
    {
      delete object;
    }
  }

  void (*deleteFunction)(T* object){nullptr}; ///< Custom delete function
};

/**
 * Surface type is just a void* to any native object.
 */
using Surface = void;

/**
 * @brief Enum describing preTransform of render target
 */
enum class RenderTargetTransformFlagBits
{
  TRANSFORM_IDENTITY_BIT           = 0x00000001,
  ROTATE_90_BIT                    = 0x00000002,
  ROTATE_180_BIT                   = 0x00000004,
  ROTATE_270_BIT                   = 0x00000008,
  HORIZONTAL_MIRROR_BIT            = 0x00000010,
  HORIZONTAL_MIRROR_ROTATE_90_BIT  = 0x00000020,
  HORIZONTAL_MIRROR_ROTATE_180_BIT = 0x00000040,
  HORIZONTAL_MIRROR_ROTATE_270_BIT = 0x00000080,
};

using RenderTargetTransformFlags = uint32_t;

template<typename T>
inline RenderTargetTransformFlags operator|(T flags, RenderTargetTransformFlagBits bit)
{
  return static_cast<RenderTargetTransformFlags>(flags) | static_cast<RenderTargetTransformFlags>(bit);
}

/**
 * unique_ptr defined in the Graphics scope
 */
template<class T, class D = DefaultDeleter<T>>
using UniquePtr = std::unique_ptr<T, D>;

/**
 * @brief MakeUnique<> version that returns Graphics::UniquePtr
 * @param[in] args Arguments for construction
 * @return
 */
template<class T, class Deleter = DefaultDeleter<T>, class... Args>
std::enable_if_t<!std::is_array<T>::value, Graphics::UniquePtr<T>>
MakeUnique(Args&&... args)
{
  return UniquePtr<T>(new T(std::forward<Args>(args)...), Deleter());
}

} // namespace Graphics
} // namespace Dali

#endif //DALI_GRAPHICS_API_TYPES_H
