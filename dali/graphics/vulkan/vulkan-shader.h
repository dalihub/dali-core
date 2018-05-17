#ifndef DALI_GRAPHICS_VULKAN_SHADER
#define DALI_GRAPHICS_VULKAN_SHADER

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
namespace SpirV
{
class SPIRVShader;
}

class Graphics;
class Shader : public VkManaged // can be overriden as ShaderGLSL for example
{
public:

  enum class Type
  {
    VERTEX = static_cast<int>( vk::ShaderStageFlagBits::eVertex ),
    FRAGMENT = static_cast<int>( vk::ShaderStageFlagBits::eFragment ),
    COMPUTE = static_cast<int>( vk::ShaderStageFlagBits::eCompute ),
    GEOMETRY = static_cast<int>( vk::ShaderStageFlagBits::eGeometry ),
  };

  /**
   *
   * @param graphics
   * @param info
   * @return
   */
  static Handle<Shader> New( Graphics& graphics, const vk::ShaderModuleCreateInfo& info );

  /**
   * Creates new shader module from SPIR-V code
   * @param graphics
   * @param bytes
   * @param size
   * @return
   */
  static Handle<Shader> New( Graphics& graphics, const void* bytes, std::size_t size );

  /**
   *
   */
  ~Shader() override;

  vk::ShaderModule GetVkHandle() const;

  const SpirV::SPIRVShader& GetSPIRVReflection() const;

  bool OnDestroy() override;

  operator vk::ShaderModule() const
  {
    return GetVkHandle();
  }

  /**
   * Sets a unique pipeline stage to be used with the shader
   * @param shaderStage
   */
  void SetExplicitShaderStage( vk::ShaderStageFlagBits shaderStage );

  vk::ShaderStageFlagBits GetExplicitShaderStage() const;

private:

  Shader( Graphics& graphics, const vk::ShaderModuleCreateInfo& info );

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

using ShaderRef = Handle<Shader>;

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_SHADER
