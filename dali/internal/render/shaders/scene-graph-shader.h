#ifndef DALI_INTERNAL_SCENE_GRAPH_SHADER_H
#define DALI_INTERNAL_SCENE_GRAPH_SHADER_H

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
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>

namespace Dali
{
namespace Internal
{
class Program;
class ProgramCache;

namespace SceneGraph
{
class ConnectionObserver;
class SceneController;
/**
 * A holder class for Program; also enables sharing of uniform properties
 */
class Shader : public PropertyOwner, public UniformMap::Observer
{
public:
  /**
   * Constructor
   * @param hints Shader hints
   */
  Shader(Dali::Shader::Hint::Value& hints);

  /**
   * Virtual destructor
   */
  ~Shader() override;

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called during Update
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Query whether a shader hint is set.
   * @pre The shader has been initialized.
   * @param[in] hint The hint to check.
   * @return True if the given hint is set.
   */
  bool HintEnabled(Dali::Shader::Hint::Value hint) const
  {
    return mHints & hint;
  }

  /**
   * @return True if the fragment shader outputs only 1.0 on the alpha channel
   *
   * @note Shaders that can output any value on the alpha channel
   * including 1.0 should return false for this.
   */
  bool IsOutputOpaque();

  /**
   * @return True if the fragment shader can output any value but 1.0 on the alpha channel
   *
   * @note Shaders that can output any value on the alpha channel
   * including 1.0 should return false for this
   */
  bool IsOutputTransparent();

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties
   */
  virtual void ResetDefaultProperties(BufferIndex updateBufferIndex)
  {
    // no default properties
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called during Render
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Set the shader data for this shader.
   * @param[in] shaderData The program's vertex/fragment source and optionally pre-compiled shader binary.
   */
  void SetShaderData(ShaderDataPtr shaderData);

  /**
   * Get the shader data for this shader.
   * @return The shader data.
   */
  ShaderDataPtr GetShaderData() const;

public: // Implementation of ConnectionChangePropagator
  /**
   * @copydoc ConnectionChangePropagator::AddObserver
   */
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer);

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer);

public: // UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  void UniformMappingsChanged(const UniformMap& mappings) override;

private: // Data
  Dali::Shader::Hint::Value mHints;

  ShaderDataPtr mShaderData;

  ConnectionChangePropagator mConnectionObservers;
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

#endif // DALI_INTERNAL_SCENE_GRAPH_SHADER_H
