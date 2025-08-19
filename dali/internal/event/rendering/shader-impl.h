#ifndef DALI_INTERNAL_SHADER_H
#define DALI_INTERNAL_SHADER_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali/internal/common/shader-data.h>            // ShaderPtr
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h>      // Dali::Internal::Object
#include <dali/public-api/common/dali-common.h>          // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h>        // Dali::IntrusivePtr
#include <dali/public-api/common/vector-wrapper.h>       // std::vector<>
#include <dali/public-api/rendering/shader.h>            // Dali::Shader
#include <dali/public-api/rendering/uniform-block.h>     // Dali::UniformBlock

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Shader;
}
class Shader;
class UniformBlock;

using ShaderPtr = IntrusivePtr<Shader>;

/**
 * Shader is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class Shader : public Object, public Object::Observer
{
public:
  /**
   * @copydoc Dali::Shader::New()
   */
  static ShaderPtr New(std::string_view                vertexShader,
                       std::string_view                fragmentShader,
                       Dali::Shader::Hint::Value       hints,
                       std::string_view                shaderName,
                       std::vector<Dali::UniformBlock> uniformBlocks,
                       bool                            strongConnection);

  /**
   * @copydoc Dali::Shader::New()
   */
  static ShaderPtr New(Dali::Property::Value shaderMap, std::vector<Dali::UniformBlock> uniformBlocks, bool strongConnection);

  /**
   * Retrieve the scene-graph shader added by this object.
   * @return A pointer to the shader.
   */
  const SceneGraph::Shader& GetShaderSceneObject() const;

public: // Default property extensions from Object
  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue(Property::Index index) const override;

private: // implementation
  /**
   * Constructor
   *
   * @param[in] sceneObject the scene object
   */
  Shader(const SceneGraph::Shader* sceneObject);

  /**
   * @brief Update Shader Data
   * If a ShaderData of the same renderPassTag is already exist, it is replaced,
   * if not, new ShaderData is added.
   * @param[in] vertexShader Vertex shader code for the effect.
   * @param[in] fragmentShader Fragment Shader code for the effect.
   * @param[in] renderPassTag render pass tag of this shader data
   * @param[in] hints Hints to define the geometry of the rendered object
   * @param[in] name The name of shader data.
   */
  void UpdateShaderData(std::string_view vertexShader, std::string_view fragmentShader, uint32_t renderPassTag, Dali::Shader::Hint::Value hints, std::string_view name);

  /**
   * @brief Sets shader data from shaderMap.
   * The shaderMap should be Property::Map or Property::Array.
   * @param[in] shaderMap shader property map.
   */
  void SetShaderProperty(const Dali::Property::Value& shaderMap);

public:
  /**
   * @brief Connects to the uniform block.
   *
   * @param[in] uniformBlock The uniform block to connect.
   * @param[in] strongConnection If true, a strong connection is made to the uniform block. False as default, which means a weak connection.
   * @param[in] programCacheCleanRequired Whether program cache clean is required or not.
   * Could be false only if the shader never be rendered before. (e.g. shader constructor.)
   */
  void ConnectUniformBlock(UniformBlock& uniformBlock, bool strongConnection, bool programCacheCleanRequired = true);

  /**
   * @brief Disconnects to the uniform block.
   *
   * @param[in] uniformBlock The uniform block to connect.
   */
  void DisconnectUniformBlock(UniformBlock& uniformBlock);

protected: ///< From Dali::Internal::Object::Observer
  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectAdded()
   */
  void SceneObjectAdded(Object& object) override
  {
    // Do nothing
  }
  /**
   * @copydoc Dali::Internal::Object::Observer::SceneObjectRemoved()
   */
  void SceneObjectRemoved(Object& object) override
  {
    // Do nothing
  }
  /**
   * @copydoc Dali::Internal::Object::Observer::ObjectDestroyed()
   */
  void ObjectDestroyed(Object& object) override;

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~Shader() override;

private: // unimplemented methods
  Shader()                         = delete;
  Shader(const Shader&)            = delete;
  Shader& operator=(const Shader&) = delete;

private:
  std::vector<Internal::ShaderDataPtr> mShaderDataList;
  std::vector<Dali::UniformBlock>      mStrongConnectedUniformBlockList;

public:
  /**
   * @copydoc Dali::Shader::GetShaderLanguageVersion()
   */
  static uint32_t GetShaderLanguageVersion();

  /**
   * @copydoc Dali::Shader::GetShaderVersionPrefix()
   */
  static std::string GetShaderVersionPrefix();

  /**
   * @copydoc Dali::Shader::GetVertexShaderPrefix()
   */
  static std::string GetVertexShaderPrefix();

  /**
   * @copydoc Dali::Shader::GetFragmentShaderPrefix()
   */
  static std::string GetFragmentShaderPrefix();

public:
  /**
   * Generates tag 'legacy-prefix-end' with end position of
   * prefix text to make shader code parsing easier.
   * Function is public to be testable and integration api.
   */
  static std::string GenerateTaggedShaderPrefix(const std::string& shaderPrefix);
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Internal::Shader& GetImplementation(Dali::Shader& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Shader handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::Shader&>(object);
}

inline const Internal::Shader& GetImplementation(const Dali::Shader& handle)
{
  DALI_ASSERT_ALWAYS(handle && "Shader handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::Shader&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_SHADER_H
