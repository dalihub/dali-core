#ifndef GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
#define GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
#include <cstdint>
#include <vector>
#include <string>

extern std::vector<uint32_t> SHADER_BASIC_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_BASIC_SHADER_VERT;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_ANTI_ALIASING_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_ANTI_ALIASING_SHADER_VERT;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_BORDER_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_COLOR_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_COLOR_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_EXPERIMENTAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_ATLAS_CLAMP_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_ATLAS_VARIOUS_WRAP_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_NO_ATLAS_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_IMAGE_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_NORMAL_MAP_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_NORMAL_MAP_SHADER_VERT;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SIMPLE_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_MESH_VISUAL_SIMPLE_SHADER_VERT;
extern std::vector<uint32_t> SHADER_NPATCH_VISUAL_3X3_SHADER_VERT;
extern std::vector<uint32_t> SHADER_NPATCH_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_NPATCH_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_MULTI_COLOR_TEXT_WITH_STYLE_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SHADER_VERT;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_EMOJI_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_AND_EMOJI_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_TEXT_VISUAL_SINGLE_COLOR_TEXT_WITH_STYLE_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_WIREFRAME_VISUAL_SHADER_FRAG;
extern std::vector<uint32_t> SHADER_WIREFRAME_VISUAL_SHADER_VERT;
extern "C" std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag );
#define GraphicsGetBuiltinShaderId( x ) GraphicsGetBuiltinShader( #x )
#endif // GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
