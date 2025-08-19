#ifndef DALI_RENDERING_UNIFORM_BLOCK_H
#define DALI_RENDERING_UNIFORM_BLOCK_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/object/handle.h>
#include <dali/public-api/rendering/shader.h>

namespace Dali
{
/**
 * @addtogroup dali_core_rendering_effects
 * @{
 */

namespace Internal DALI_INTERNAL
{
class UniformBlock;
}

/**
 * Uniform block is a property owning object that can hold a number
 * of properties mapping onto a uniform block in the shader.
 *
 * Mapping is done automatically through shader reflection, the structure
 * doesn't need to be defined here. (Order of property registration is not
 * relevant)
 *
 * Uniform Properties can be animated / constrained as normal.
 *
 * The underlying code requires the name of the uniform block to match;
 * consequently, it's a construct only property.
 *
 * When the uniform block object is connected to a shader, it will be
 * used to populate the uniforms for that shader, no matter what renderer
 * + actor that shader is connected to. Consequently, the client does not
 * need to declare the block's properties on the shader, renderer or actor.
 *
 * Furthermore, any such declared properties will not override those
 * in the Uniform Block.
 *
 * @SINCE_2_4.14
 */
class DALI_CORE_API UniformBlock : public Handle
{
public:
  /**
   * Construct a named uniform block.
   *
   * @SINCE_2_4.14
   * @param[in] blockName The block name must match a named structure in the shader
   */
  static UniformBlock New(std::string blockName);

  /**
   * @brief Constructor
   *
   * @SINCE_2_4.14
   */
  UniformBlock();

  /**
   * @brief Destructor
   *
   * @SINCE_2_4.14
   */
  ~UniformBlock();

  /**
   * @brief Copy Constructor
   *
   * @SINCE_2_4.14
   * @param[in] handle The handle to copy
   */
  UniformBlock(const UniformBlock& handle);

  /**
   * @brief Assignment operator
   *
   * @SINCE_2_4.14
   * @param[in] handle The handle to copy
   */
  UniformBlock& operator=(const UniformBlock& handle);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_4.14
   * @param[in] handle The handle to move
   */
  UniformBlock(UniformBlock&& handle) noexcept;

  /**
   * @brief Move assign
   *
   * @SINCE_2_4.14
   * @param[in] handle The handle to move
   * @return this handle
   */
  UniformBlock& operator=(UniformBlock&& handle) noexcept;

  /**
   * @brief Attempt to downcast from the base type
   *
   * @SINCE_2_4.14
   * @param[in] handle The handle to downcast
   * @return A UniformBlock handle to a valid resource, or empty handle if invalid.
   */
  static UniformBlock DownCast(BaseHandle handle);

public:
  /**
   * @brief Retrieve the block name.
   *
   * @SINCE_2_4.14
   * @return The block name.
   */
  std::string_view GetUniformBlockName() const;

  /**
   * @brief Connect to a shader.
   *
   * Return false if the UniformBlock cannot be connected to the shader.
   * (e.g. Shader is invalid, or the UniformBlock has already been connected to given shader)
   *
   * @SINCE_2_4.14
   * @param[in] shader A shader to connect to. Multiple shaders can be connected.
   * @param[in] strongConnection If true, a strong connection is made to the shader. False as default, which means a weak connection.
   * @return True if we successfully connected to the shader, false otherwise.
   */
  bool ConnectToShader(Shader shader, bool strongConnection = false);

  /**
   * @brief Disconnect from a shader.
   *
   * @SINCE_2_4.14
   * @param[in] shader A shader to disconnect from.
   */
  void DisconnectFromShader(Shader shader);

public:
  /// @cond internal
  /**
   * @brief Constructor
   * @note Not intended for application developers
   *
   * @param[in] object A pointer to a newly allocated UniformBlock
   */
  explicit DALI_INTERNAL UniformBlock(Internal::UniformBlock* object);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_RENDERING_UNIFORM_BLOCK_H
