#ifndef DALI_INTERNAL_RENDER_SHADER_H
#define DALI_INTERNAL_RENDER_SHADER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/internal/common/shader-data.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>

namespace Dali
{
namespace Internal
{
class Program;

namespace Render
{
class UniformBlock;
}

namespace SceneGraph
{

/**
 * This PropertyOwner enables registration of properties as uniforms.
 * It holds a ShaderData, which can be read from Render side.
 *
 * Any renderer that uses this shader also registers to it's UniformMap
 * observer, so that it can be notified when properties are registered
 * after being linked. (DALi public API allows Renderer to be created
 * with a shader, or a shader to be set on a renderer _before_ uniform
 * properties are defined on a shader. This connection ensures that
 * all the uniforms are loaded into GPU at render time).
 *
 * //@todo Move back to scene graph
 */
class Shader : public PropertyOwner
{
public:
  using UniformBlockContainer = std::unordered_map<size_t, Render::UniformBlock*>;

  Shader() = default;

  /**
   * Virtual destructor
   */
  ~Shader() override;

  /**
   * @brief Set the shader data for this shader.
   * @param[in] shaderData The program's vertex/fragment source and optionally pre-compiled shader binary.
   */
  void UpdateShaderData(ShaderDataPtr shaderData);

  /**
   * Get the shader data for this shader.
   * @return The shader data.
   */
  [[nodiscard]] const ShaderDataPtr& GetShaderData(uint32_t renderPassTag) const;

  /**
   * @brief Connect uniform blocks.
   * @param[in] uniformBlock Uniform block to be connected.
   */
  void ConnectUniformBlock(Render::UniformBlock* uniformBlock);

  /**
   * @brief Connect uniform blocks.
   * @param[in] uniformBlock Uniform block to be disconnected.
   */
  void DisconnectUniformBlock(Render::UniformBlock* uniformBlock);

  /**
   * @brief Get the set of connected uniform blocks.
   * @return The set of connected uniform blocks.
   */
  const UniformBlockContainer& GetConnectedUniformBlocks() const;

  /**
   * @brief Get the hash value of connected uniform blocks name.
   * @return Hash value of all connected uniform blocks name.
   */
  std::size_t GetSharedUniformNamesHash() const;

  /**
   * @brief Check whether it's property, or one of UniformBlock's property was changed.
   */
  bool IsUpdated() const;

  /**
   * Update the result of Query of IsUpdated() result.
   * The result will be updated only 1 times per each frame.
   */
  void CheckUpdated() const;

public: // From PropertyOwner
  /**
   * @copydoc PropertyOwner::ResetUpdated
   */
  void ResetUpdated() override;

private: // Data
  ShaderDataPtr              mDefaultShaderData{nullptr};
  std::vector<ShaderDataPtr> mShaderDataList{};
  UniformBlockContainer      mBlocks{};           ///< List of connected uniform blocks (not owned)
  std::size_t                mBlockNamesHash{0u}; ///< Simple hash of all connected uniform blocks name.

  mutable uint8_t mDirtyUpdated{0u}; ///< Dirty flag that we can change 1 times per each frame.
};

inline void UpdateShaderDataMessage(EventThreadServices& eventThreadServices, const Shader& shader, ShaderDataPtr shaderData)
{
  using LocalType = MessageValue1<Shader, ShaderDataPtr>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&shader, &Shader::UpdateShaderData, shaderData);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_SHADER_H
