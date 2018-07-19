#ifndef DALI_GRAPHICS_API_SHADER_DETAILS_H
#define DALI_GRAPHICS_API_SHADER_DETAILS_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <string>
#include <vector>
#include <iostream>

namespace Dali
{
namespace Graphics
{
namespace API
{
namespace ShaderDetails
{

constexpr uint32_t ERROR_VERTEX_INPUT_ATTRIBUTE_NOT_FOUND( -1u );

enum class ShaderSourceType
{
  STRING,
  BINARY
};

struct ShaderSource
{
  /**
   * Creates shader source object from source string
   * @param sourceString
   */
  explicit ShaderSource( const std::string& sourceString )
  {
    source = sourceString;
    type = ShaderSourceType::STRING;
  }

  /**
   * Creates shader source object from vector
   * @param sourceBinary
   */
  template<class T>
  explicit ShaderSource( const std::vector<T>& sourceBinary )
  {
    if(!sourceBinary.empty())
    {
      code.resize(sourceBinary.size() * sizeof(T));
      auto begin = reinterpret_cast<const char *>(&*sourceBinary.begin());
      auto end   = reinterpret_cast<const char *>(&*sourceBinary.end());
      std::copy(begin, end, code.begin());
    }
    type = ShaderSourceType::BINARY;
  }

  template<char>
  explicit ShaderSource( const std::vector<char>& sourceBinary )
  {
    if( !sourceBinary.empty() )
    {
      code = sourceBinary;
    }
    type = ShaderSourceType::BINARY;
  }

  /**
   * Creates shader source object from C-Style binary data
   * @param pBinary
   * @param size
   */
  explicit ShaderSource( const void* pBinary, uint32_t size )
  {
    code.resize( size );
    auto begin = reinterpret_cast<const char*>(pBinary);
    auto end = begin + size;
    std::copy( begin, end, code.begin() );
    type = ShaderSourceType::BINARY;
  }

  ShaderSource( const ShaderSource& shaderSource ) = default;

  /**
   * Tests whether the shader module has been set
   * @return
   */
  bool IsSet() const
  {
    if( type == ShaderSourceType::BINARY )
    {
      return !code.empty();
    }

    return !source.empty();
  }

  bool operator==(const ShaderSource& rhs ) const
  {
    if( type == ShaderSourceType::BINARY )
    {
      return code == rhs.code;
    }
    else
    {
      return source == rhs.source;
    }
  }

  std::string       source;
  std::vector<char> code;
  ShaderSourceType type;
};

enum class Language {
  GLSL_1,
  GLSL_3_1,
  GLSL_3_2,
  SPIRV_1_0,
  SPIRV_1_1,
};

enum class PipelineStage {
  VERTEX,
  GEOMETRY,
  FRAGMENT,
  COMPUTE,
  TESSELATION_CONTROL,
  TESSELATION_EVALUATION,
};

enum class VertexInputAttributeFormat
{
  UNDEFINED,
  FLOAT,
  INTEGER,
  VEC2,
  VEC3,
  VEC4
};

enum class UniformClass
{
  SAMPLER,
  IMAGE,
  COMBINED_IMAGE_SAMPLER,
  UNIFORM_BUFFER,
  UNDEFINED
};

struct UniformInfo
{
  std::string   name{""};
  UniformClass  uniformClass{ UniformClass::UNDEFINED };
  uint32_t      binding { 0u };
  uint32_t      bufferIndex { 0u };
  uint32_t      offset { 0u };
  uint32_t      location { 0u };

  inline bool operator==( const UniformInfo& rhs )
  {
    return name == rhs.name &&
           uniformClass == rhs.uniformClass &&
           binding == rhs.binding &&
           bufferIndex == rhs.bufferIndex &&
           offset == rhs.offset &&
           location == rhs.location;
  }
};

struct UniformBlockInfo
{
  std::string name {""};
  uint32_t    descriptorSet {0u};
  uint32_t    binding {0u};
  uint32_t    size {0u};
  std::vector<UniformInfo> members {};
};

} // namespace TextureDetails
} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_SHADER_DETAILS_H
