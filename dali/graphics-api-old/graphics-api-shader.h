#ifndef DALI_GRAPHICS_API_SHADER_H
#define DALI_GRAPHICS_API_SHADER_H

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

#include <dali/graphics-api/graphics-api-shader-details.h>

#include <cstdint>
#include <string>

namespace Dali
{
namespace Graphics
{

/**
 * @brief Interface class for Shader types in the graphics API.
 */
class Shader
{
public:
  // not copyable
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  virtual ~Shader() = default;

  // Reflection, optional, may be not supported
  virtual bool IsReflectionSupported() const = 0;

  virtual uint32_t GetVertexAttributeLocation(const std::string &name) const
  {
    return 0;
  }

  virtual ShaderDetails::VertexInputAttributeFormat GetVertexAttributeFormat( uint32_t location ) const
  {
    return ShaderDetails::VertexInputAttributeFormat::UNDEFINED;
  }

  virtual std::string GetVertexAttributeName(uint32_t location) const
  {
    return {};
  }

  virtual std::vector<uint32_t> GetVertexAttributeLocations() const
  {
    return {};
  }

  virtual std::vector<ShaderDetails::UniformInfo> GetSamplers() const
  {
    return {};
  }

  virtual bool GetNamedUniform( const std::string& name, ShaderDetails::UniformInfo& out ) const
  {
    return false;
  }

  /**
   * Returns number of uniform blocks from all the shaders
   * @return
   */
  virtual uint32_t GetUniformBlockCount() const
  {
    return 0u;
  }

  virtual uint32_t GetUniformBlockBinding( uint32_t index ) const
  {
    return 0u;
  }

  virtual uint32_t GetUniformBlockSize( uint32_t index ) const
  {
    return 0u;
  }

  virtual bool GetUniformBlock( uint32_t index, ShaderDetails::UniformBlockInfo& out ) const
  {
    return false;
  }

protected:
  // derived types should not be moved direcly to prevent slicing
  Shader(Shader&&) = default;
  Shader& operator=(Shader&&) = default;

  /**
   * Objects of this type should not be directly instantiated.
   */
  Shader() = default;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_SHADER_H
