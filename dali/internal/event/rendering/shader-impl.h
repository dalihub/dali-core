#ifndef DALI_INTERNAL_SHADER_H
#define DALI_INTERNAL_SHADER_H

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
#include <dali/internal/common/shader-data.h>            // ShaderPtr
#include <dali/internal/event/common/object-connector.h> // Dali::Internal::ObjectConnector
#include <dali/internal/event/common/object-impl.h>      // Dali::Internal::Object
#include <dali/public-api/common/dali-common.h>          // DALI_ASSERT_ALWAYS
#include <dali/public-api/common/intrusive-ptr.h>        // Dali::IntrusivePtr
#include <dali/public-api/rendering/shader.h>            // Dali::Shader

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Shader;
}

class Shader;
using ShaderPtr = IntrusivePtr<Shader>;

/**
 * Shader is an object that contains an array of structures of values that
 * can be accessed as properties.
 */
class Shader : public Object
{
public:
  /**
   * @copydoc Dali::Shader::New()
   */
  static ShaderPtr New(std::string_view          vertexShader,
                       std::string_view          fragmentShader,
                       Dali::Shader::Hint::Value hints);

  /**
   * @copydoc Dali::Shader::New()
   */
  static ShaderPtr New(Dali::Property::Value shaderMap);

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
   * @param sceneObject the scene object
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
   */
  void UpdateShaderData(std::string_view vertexShader, std::string_view fragmentShader, uint32_t renderPassTag, Dali::Shader::Hint::Value hints);

  /**
   * @brief Sets shader data from shaderMap.
   * The shaderMap should be Property::Map or Property::Array.
   * @param[in] shaderMap shader property map.
   */
  void SetShaderProperty(const Dali::Property::Value& shaderMap);

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

public:
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
