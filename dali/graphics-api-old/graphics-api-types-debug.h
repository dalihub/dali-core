#ifndef DALI_GRAPHICS_API_TYPES_DEBUG_H
#define DALI_GRAPHICS_API_TYPES_DEBUG_H

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
#include <dali/graphics-api/graphics-api-types.h>

namespace Dali
{
namespace Graphics
{

namespace Debug
{
#if defined(DEBUG_ENABLED)

#define CASE(TYPE, VALUE) case TYPE::VALUE: return std::string(#VALUE)

inline std::string str( const VertexInputRate& rate )
{
  switch( rate )
  {
    CASE( VertexInputRate, PER_INSTANCE );
    CASE( VertexInputRate, PER_VERTEX );
  }
  return "UNDEFINED";
}

inline std::string str( const PrimitiveTopology& topology )
{
  switch( topology )
  {
    CASE( PrimitiveTopology, LINE_STRIP);
    CASE( PrimitiveTopology, POINT_LIST);
    CASE( PrimitiveTopology, LINE_LIST);
    CASE( PrimitiveTopology, TRIANGLE_LIST);
    CASE( PrimitiveTopology, TRIANGLE_STRIP);
    CASE( PrimitiveTopology, TRIANGLE_FAN);
  }
  return "UNDEFINED";
}

inline std::string str( const CompareOp& op )
{
  switch( op )
  {
    CASE( CompareOp, NEVER);
    CASE( CompareOp, LESS);
    CASE( CompareOp, EQUAL);
    CASE( CompareOp, LESS_OR_EQUAL);
    CASE( CompareOp, GREATER);
    CASE( CompareOp, NOT_EQUAL);
    CASE( CompareOp, GREATER_OR_EQUAL);
    CASE( CompareOp, ALWAYS);
  }
  return "UNDEFINED";
}

inline std::string str( const CullMode& mode )
{
  switch( mode )
  {
    CASE( CullMode, NONE);
    CASE( CullMode, FRONT);
    CASE( CullMode, BACK);
    CASE( CullMode, FRONT_AND_BACK);
  }
  return "UNDEFINED";
}

inline std::string str( const PolygonMode& mode )
{
  switch( mode )
  {
    CASE( PolygonMode, FILL);
    CASE( PolygonMode, LINE);
    CASE( PolygonMode, POINT);
  }
  return "UNDEFINED";
}

inline std::string str( const FrontFace& mode )
{
  switch( mode )
  {
    CASE( FrontFace, COUNTER_CLOCKWISE);
    CASE( FrontFace, CLOCKWISE);
  }
  return "UNDEFINED";
}

inline std::string str( const BlendFactor& factor )
{
  switch( factor )
  {
    CASE( BlendFactor, ZERO );
    CASE( BlendFactor, ONE );
    CASE( BlendFactor, SRC_COLOR );
    CASE( BlendFactor, ONE_MINUS_SRC_COLOR );
    CASE( BlendFactor, DST_COLOR );
    CASE( BlendFactor, ONE_MINUS_DST_COLOR );
    CASE( BlendFactor, SRC_ALPHA );
    CASE( BlendFactor, ONE_MINUS_SRC_ALPHA );
    CASE( BlendFactor, DST_ALPHA );
    CASE( BlendFactor, ONE_MINUS_DST_ALPHA );
    CASE( BlendFactor, CONSTANT_COLOR );
    CASE( BlendFactor, ONE_MINUS_CONSTANT_COLOR );
    CASE( BlendFactor, CONSTANT_ALPHA );
    CASE( BlendFactor, ONE_MINUS_CONSTANT_ALPHA );
    CASE( BlendFactor, SRC_ALPHA_SATURATE );
    CASE( BlendFactor, SRC1_COLOR );
    CASE( BlendFactor, ONE_MINUS_SRC1_COLOR );
    CASE( BlendFactor, SRC1_ALPHA );
    CASE( BlendFactor, ONE_MINUS_SRC1_ALPHA );
  }
  return "UNDEFINED";
}

inline std::string str( const BlendOp& op )
{
  switch( op )
  {
    CASE( BlendOp, ADD );
    CASE( BlendOp, SUBTRACT );
    CASE( BlendOp, REVERSE_SUBTRACT );
    CASE( BlendOp, MIN );
    CASE( BlendOp, MAX );
  }
  return "UNDEFINED";
}

inline std::string str( const Format& format )
{
  switch( format )
  {
    CASE( Format, UNDEFINED );
    CASE( Format, L8 );
    CASE( Format, L8A8 );
    CASE( Format, R4G4_UNORM_PACK8 );
    CASE( Format, R4G4B4A4_UNORM_PACK16 );
    CASE( Format, B4G4R4A4_UNORM_PACK16 );
    CASE( Format, R5G6B5_UNORM_PACK16 );
    CASE( Format, B5G6R5_UNORM_PACK16 );
    CASE( Format, R5G5B5A1_UNORM_PACK16 );
    CASE( Format, B5G5R5A1_UNORM_PACK16 );
    CASE( Format, A1R5G5B5_UNORM_PACK16 );
    CASE( Format, R8_UNORM );
    CASE( Format, R8_SNORM );
    CASE( Format, R8_USCALED );
    CASE( Format, R8_SSCALED );
    CASE( Format, R8_UINT );
    CASE( Format, R8_SINT );
    CASE( Format, R8_SRGB );
    CASE( Format, R8G8_UNORM );
    CASE( Format, R8G8_SNORM );
    CASE( Format, R8G8_USCALED );
    CASE( Format, R8G8_SSCALED );
    CASE( Format, R8G8_UINT );
    CASE( Format, R8G8_SINT );
    CASE( Format, R8G8_SRGB );
    CASE( Format, R8G8B8_UNORM );
    CASE( Format, R8G8B8_SNORM );
    CASE( Format, R8G8B8_USCALED );
    CASE( Format, R8G8B8_SSCALED );
    CASE( Format, R8G8B8_UINT );
    CASE( Format, R8G8B8_SINT );
    CASE( Format, R8G8B8_SRGB );
    CASE( Format, B8G8R8_UNORM );
    CASE( Format, B8G8R8_SNORM );
    CASE( Format, B8G8R8_USCALED );
    CASE( Format, B8G8R8_SSCALED );
    CASE( Format, B8G8R8_UINT );
    CASE( Format, B8G8R8_SINT );
    CASE( Format, B8G8R8_SRGB );
    CASE( Format, R8G8B8A8_UNORM );
    CASE( Format, R8G8B8A8_SNORM );
    CASE( Format, R8G8B8A8_USCALED );
    CASE( Format, R8G8B8A8_SSCALED );
    CASE( Format, R8G8B8A8_UINT );
    CASE( Format, R8G8B8A8_SINT );
    CASE( Format, R8G8B8A8_SRGB );
    CASE( Format, B8G8R8A8_UNORM );
    CASE( Format, B8G8R8A8_SNORM );
    CASE( Format, B8G8R8A8_USCALED );
    CASE( Format, B8G8R8A8_SSCALED );
    CASE( Format, B8G8R8A8_UINT );
    CASE( Format, B8G8R8A8_SINT );
    CASE( Format, B8G8R8A8_SRGB );
    CASE( Format, A8B8G8R8_UNORM_PACK32 );
    CASE( Format, A8B8G8R8_SNORM_PACK32 );
    CASE( Format, A8B8G8R8_USCALED_PACK32 );
    CASE( Format, A8B8G8R8_SSCALED_PACK32 );
    CASE( Format, A8B8G8R8_UINT_PACK32 );
    CASE( Format, A8B8G8R8_SINT_PACK32 );
    CASE( Format, A8B8G8R8_SRGB_PACK32 );
    CASE( Format, A2R10G10B10_UNORM_PACK32 );
    CASE( Format, A2R10G10B10_SNORM_PACK32 );
    CASE( Format, A2R10G10B10_USCALED_PACK32 );
    CASE( Format, A2R10G10B10_SSCALED_PACK32 );
    CASE( Format, A2R10G10B10_UINT_PACK32 );
    CASE( Format, A2R10G10B10_SINT_PACK32 );
    CASE( Format, A2B10G10R10_UNORM_PACK32 );
    CASE( Format, A2B10G10R10_SNORM_PACK32 );
    CASE( Format, A2B10G10R10_USCALED_PACK32 );
    CASE( Format, A2B10G10R10_SSCALED_PACK32 );
    CASE( Format, A2B10G10R10_UINT_PACK32 );
    CASE( Format, A2B10G10R10_SINT_PACK32 );
    CASE( Format, R16_UNORM );
    CASE( Format, R16_SNORM );
    CASE( Format, R16_USCALED );
    CASE( Format, R16_SSCALED );
    CASE( Format, R16_UINT );
    CASE( Format, R16_SINT );
    CASE( Format, R16_SFLOAT );
    CASE( Format, R16G16_UNORM );
    CASE( Format, R16G16_SNORM );
    CASE( Format, R16G16_USCALED );
    CASE( Format, R16G16_SSCALED );
    CASE( Format, R16G16_UINT );
    CASE( Format, R16G16_SINT );
    CASE( Format, R16G16_SFLOAT );
    CASE( Format, R16G16B16_UNORM );
    CASE( Format, R16G16B16_SNORM );
    CASE( Format, R16G16B16_USCALED );
    CASE( Format, R16G16B16_SSCALED );
    CASE( Format, R16G16B16_UINT );
    CASE( Format, R16G16B16_SINT );
    CASE( Format, R16G16B16_SFLOAT );
    CASE( Format, R16G16B16A16_UNORM );
    CASE( Format, R16G16B16A16_SNORM );
    CASE( Format, R16G16B16A16_USCALED );
    CASE( Format, R16G16B16A16_SSCALED );
    CASE( Format, R16G16B16A16_UINT );
    CASE( Format, R16G16B16A16_SINT );
    CASE( Format, R16G16B16A16_SFLOAT );
    CASE( Format, R32_UINT );
    CASE( Format, R32_SINT );
    CASE( Format, R32_SFLOAT );
    CASE( Format, R32G32_UINT );
    CASE( Format, R32G32_SINT );
    CASE( Format, R32G32_SFLOAT );
    CASE( Format, R32G32B32_UINT );
    CASE( Format, R32G32B32_SINT );
    CASE( Format, R32G32B32_SFLOAT );
    CASE( Format, R32G32B32A32_UINT );
    CASE( Format, R32G32B32A32_SINT );
    CASE( Format, R32G32B32A32_SFLOAT );
    CASE( Format, R64_UINT );
    CASE( Format, R64_SINT );
    CASE( Format, R64_SFLOAT );
    CASE( Format, R64G64_UINT );
    CASE( Format, R64G64_SINT );
    CASE( Format, R64G64_SFLOAT );
    CASE( Format, R64G64B64_UINT );
    CASE( Format, R64G64B64_SINT );
    CASE( Format, R64G64B64_SFLOAT );
    CASE( Format, R64G64B64A64_UINT );
    CASE( Format, R64G64B64A64_SINT );
    CASE( Format, R64G64B64A64_SFLOAT );
    CASE( Format, B10G11R11_UFLOAT_PACK32 );
    CASE( Format, E5B9G9R9_UFLOAT_PACK32 );
    CASE( Format, D16_UNORM );
    CASE( Format, X8_D24_UNORM_PACK32 );
    CASE( Format, D32_SFLOAT );
    CASE( Format, S8_UINT );
    CASE( Format, D16_UNORM_S8_UINT );
    CASE( Format, D24_UNORM_S8_UINT );
    CASE( Format, D32_SFLOAT_S8_UINT );
    CASE( Format, BC1_RGB_UNORM_BLOCK );
    CASE( Format, BC1_RGB_SRGB_BLOCK );
    CASE( Format, BC1_RGBA_UNORM_BLOCK );
    CASE( Format, BC1_RGBA_SRGB_BLOCK );
    CASE( Format, BC2_UNORM_BLOCK );
    CASE( Format, BC2_SRGB_BLOCK );
    CASE( Format, BC3_UNORM_BLOCK );
    CASE( Format, BC3_SRGB_BLOCK );
    CASE( Format, BC4_UNORM_BLOCK );
    CASE( Format, BC4_SNORM_BLOCK );
    CASE( Format, BC5_UNORM_BLOCK );
    CASE( Format, BC5_SNORM_BLOCK );
    CASE( Format, BC6H_UFLOAT_BLOCK );
    CASE( Format, BC6H_SFLOAT_BLOCK );
    CASE( Format, BC7_UNORM_BLOCK );
    CASE( Format, BC7_SRGB_BLOCK );
    CASE( Format, ETC2_R8G8B8_UNORM_BLOCK );
    CASE( Format, ETC2_R8G8B8_SRGB_BLOCK );
    CASE( Format, ETC2_R8G8B8A1_UNORM_BLOCK );
    CASE( Format, ETC2_R8G8B8A1_SRGB_BLOCK );
    CASE( Format, ETC2_R8G8B8A8_UNORM_BLOCK );
    CASE( Format, ETC2_R8G8B8A8_SRGB_BLOCK );
    CASE( Format, EAC_R11_UNORM_BLOCK );
    CASE( Format, EAC_R11_SNORM_BLOCK );
    CASE( Format, EAC_R11G11_UNORM_BLOCK );
    CASE( Format, EAC_R11G11_SNORM_BLOCK );
    CASE( Format, ASTC_4x4_UNORM_BLOCK );
    CASE( Format, ASTC_4x4_SRGB_BLOCK );
    CASE( Format, ASTC_5x4_UNORM_BLOCK );
    CASE( Format, ASTC_5x4_SRGB_BLOCK );
    CASE( Format, ASTC_5x5_UNORM_BLOCK );
    CASE( Format, ASTC_5x5_SRGB_BLOCK );
    CASE( Format, ASTC_6x5_UNORM_BLOCK );
    CASE( Format, ASTC_6x5_SRGB_BLOCK );
    CASE( Format, ASTC_6x6_UNORM_BLOCK );
    CASE( Format, ASTC_6x6_SRGB_BLOCK );
    CASE( Format, ASTC_8x5_UNORM_BLOCK );
    CASE( Format, ASTC_8x5_SRGB_BLOCK );
    CASE( Format, ASTC_8x6_UNORM_BLOCK );
    CASE( Format, ASTC_8x6_SRGB_BLOCK );
    CASE( Format, ASTC_8x8_UNORM_BLOCK );
    CASE( Format, ASTC_8x8_SRGB_BLOCK );
    CASE( Format, ASTC_10x5_UNORM_BLOCK );
    CASE( Format, ASTC_10x5_SRGB_BLOCK );
    CASE( Format, ASTC_10x6_UNORM_BLOCK );
    CASE( Format, ASTC_10x6_SRGB_BLOCK );
    CASE( Format, ASTC_10x8_UNORM_BLOCK );
    CASE( Format, ASTC_10x8_SRGB_BLOCK );
    CASE( Format, ASTC_10x10_UNORM_BLOCK );
    CASE( Format, ASTC_10x10_SRGB_BLOCK );
    CASE( Format, ASTC_12x10_UNORM_BLOCK );
    CASE( Format, ASTC_12x10_SRGB_BLOCK );
    CASE( Format, ASTC_12x12_UNORM_BLOCK );
    CASE( Format, ASTC_12x12_SRGB_BLOCK );
    CASE( Format, PVRTC1_2BPP_UNORM_BLOCK_IMG );
    CASE( Format, PVRTC1_4BPP_UNORM_BLOCK_IMG );
    CASE( Format, PVRTC2_2BPP_UNORM_BLOCK_IMG );
    CASE( Format, PVRTC2_4BPP_UNORM_BLOCK_IMG );
    CASE( Format, PVRTC1_2BPP_SRGB_BLOCK_IMG );
    CASE( Format, PVRTC1_4BPP_SRGB_BLOCK_IMG );
    CASE( Format, PVRTC2_2BPP_SRGB_BLOCK_IMG );
    CASE( Format, PVRTC2_4BPP_SRGB_BLOCK_IMG );
  }
  return "UNDEFINED";
}
#endif

} // namespace Debug
} // namespace Graphics
} // namespace Dali

#endif //DALI_GRAPHICS_API_TYPES_DEBUG_H
