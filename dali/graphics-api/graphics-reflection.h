#ifndef DALI_GRAPHICS_REFLECTION_H
#define DALI_GRAPHICS_REFLECTION_H

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
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief The Reflection class represents a single shader that can be bound
 * to the specific stage of pipeline (vertex, fragment, compute etc.).
 *
 * Reflections are linked together when pipeline is created.
 *
 */
class Reflection
{
public:
  Reflection()          = default;
  virtual ~Reflection() = default;

  // not copyable
  Reflection(const Reflection&) = delete;
  Reflection& operator=(const Reflection&) = delete;

  // Vertex attributes

  /**
   * @brief Gets the location of a vertex attribute.
   *
   * @param [in] name The name of vertex attribute
   * @return The index of the vertex attribute in the shader
   */
  virtual uint32_t GetVertexAttributeLocation(const std::string& name) const = 0;

  /**
   * @brief Gets the format of a vertex attribute.
   *
   * @param [in] location The location of vertex attribute
   * @return The format of a vertex attribute
   */
  virtual Dali::Graphics::VertexInputAttributeFormat GetVertexAttributeFormat(uint32_t location) const = 0;

  /**
   * @brief Gets the name of a vertex attribute.
   *
   * @param [in] location The location of vertex attribute
   * @return The name of the vertex attribute
   */
  virtual std::string GetVertexAttributeName(uint32_t location) const = 0;

  /**
   * @brief Gets the locations of all the vertex attribute in the shader.
   *
   * @return A vector of the locations of all the vertex attributes in the shader
   */
  virtual std::vector<uint32_t> GetVertexAttributeLocations() const = 0;

  // Uniform blocks

  /**
   * @brief Gets the number of uniform blocks in the shader
   *
   * @return The number of uniform blocks
   */
  virtual uint32_t GetUniformBlockCount() const = 0;

  /**
   * @brief Gets the binding point to which the uniform block with the given index is binded.
   *
   * @param [in] index The index of the uniform block
   * @return The binding point
   */
  virtual uint32_t GetUniformBlockBinding(uint32_t index) const = 0;

  /**
   * @brief Gets the size of the uniform block with the given index.
   *
   * @param [in] index The index of the uniform block
   * @return The size of the uniform block
   */
  virtual uint32_t GetUniformBlockSize(uint32_t index) const = 0;

  /**
   * @brief Retrieves the information of the uniform block with the given index.
   *
   * The information includes the name, binding point, size, uniforms inside the uniform block, etc.
   *
   * @param [in] index The index of the uniform block
   * @param [out] out A structure that contains the information of the uniform block
   * @return Whether the uniform block exists or not
   */
  virtual bool GetUniformBlock(uint32_t index, Dali::Graphics::UniformBlockInfo& out) const = 0;

  /**
   * @brief Gets the binding points of all the uniform blocks in the shader.
   *
   * @return A vector of binding points
   */
  virtual std::vector<uint32_t> GetUniformBlockLocations() const = 0;

  /**
   * @brief Gets the name of uniform block with the given index.
   *
   * @param [in] blockIndex The index of the uniform block
   * @return The name of the uniform block
   */
  virtual std::string GetUniformBlockName(uint32_t blockIndex) const = 0;

  /**
   * @brief Gets the number of uniforms in the uniform block with the given index.
   *
   * @param [in] blockIndex The index of the uniform block
   * @return The number of uniforms in the uniform block
   */
  virtual uint32_t GetUniformBlockMemberCount(uint32_t blockIndex) const = 0;

  /**
   * @brief Gets the name of the uniform in the given location within the uniform block.
   *
   * @param [in] blockIndex The index of the uniform block
   * @param [in] memberLocation The location of the uniform within the uniform block
   * @return The name of the uniform
   */
  virtual std::string GetUniformBlockMemberName(uint32_t blockIndex, uint32_t memberLocation) const = 0;

  /**
   * @brief Gets the byte offset of the uniform in the given location within the uniform block.
   *
   * @param [in] blockIndex The index of the uniform block
   * @param [in] memberLocation The location of the uniform within the uniform block
   * @return The byte offset of the uniform
   */
  virtual uint32_t GetUniformBlockMemberOffset(uint32_t blockIndex, uint32_t memberLocation) const = 0;

  // Named uniforms

  /**
   * @brief Gets the information of the uniform by its name.
   *
   * @param [in] name The name of the uniform
   * @param [out] out The information of the uniform
   * @return Whether the uniform exists or not
   */
  virtual bool GetNamedUniform(const std::string& name, Dali::Graphics::UniformInfo& out) const = 0;

  // Sampler

  /**
   * @brief Gets all the sampler uniforms. In the case of arrays of samplers,
   * it contains only the name of the sampler array without the [N] size, but,
   * the element count is set to N.
   *
   * @return A vector of the sampler uniforms
   */
  virtual const std::vector<Dali::Graphics::UniformInfo>& GetSamplers() const = 0;

  // Language

  /**
   * @brief Retrieves the language of the shader
   *
   * @return The language of the shader
   */
  virtual Graphics::ShaderLanguage GetLanguage() const = 0;

protected:
  Reflection(Reflection&&) = default;
  Reflection& operator=(Reflection&&) = default;
};

} // Namespace Graphics
} // Namespace Dali

#endif // DALI_GRAPHICS_REFLECTION_H
