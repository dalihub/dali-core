#ifndef DALI_INTERNAL_RENDER_SHADER_H
#define DALI_INTERNAL_RENDER_SHADER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/shader-data.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>

namespace Dali
{
namespace Internal
{
class Program;
class ProgramCache;

namespace SceneGraph
{
class SceneController;

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
  /**
   * Constructor
   * @param hints Shader hints
   */
  explicit Shader(Dali::Shader::Hint::Value& hints);

  /**
   * Virtual destructor
   */
  ~Shader() override;

  /**
   * Query whether a shader hint is set.
   *
   * @warning This method is called from Update Algorithms.
   *
   * @pre The shader has been initialized.
   * @param[in] hint The hint to check.
   * @return True if the given hint is set.
   */
  [[nodiscard]] bool HintEnabled(Dali::Shader::Hint::Value hint) const
  {
    return mHints & hint;
  }

  /**
   * @brief Set the shader data for this shader.
   * @param[in] shaderData The program's vertex/fragment source and optionally pre-compiled shader binary.
   */
  void SetShaderData(ShaderDataPtr shaderData);

  /**
   * Get the shader data for this shader.
   * @return The shader data.
   */
  [[nodiscard]] ShaderDataPtr GetShaderData() const;

private: // Data
  Dali::Shader::Hint::Value mHints;

  ShaderDataPtr mShaderData;
};

inline void SetShaderDataMessage(EventThreadServices& eventThreadServices, const Shader& shader, ShaderDataPtr shaderData)
{
  using LocalType = MessageValue1<Shader, ShaderDataPtr>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&shader, &Shader::SetShaderData, shaderData);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_SHADER_H
