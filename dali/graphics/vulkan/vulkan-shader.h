#ifndef DALI_VULKAN_161117_VULKAN_SHADER_H
#define DALI_VULKAN_161117_VULKAN_SHADER_H

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

class Graphics;
class Shader // can be overriden as ShaderGLSL for example
{
public:

  enum Type
  {
    VERTEX,
    FRAGMENT,
    COMPUTE,
    GEOMETRY
  };

  /**
   *
   * @param graphics
   * @param info
   * @return
   */
  static std::unique_ptr<Shader> New( Graphics& graphics, const vk::ShaderModuleCreateInfo& info );

  /**
   * Creates new shader module from SPIRV code
   * @param graphics
   * @param bytes
   * @param size
   * @return
   */
  static std::unique_ptr<Shader> New( Graphics& graphics, const void* bytes, std::size_t size );

  /**
   *
   */
  ~Shader();

  vk::ShaderModule GetVkShaderModule() const;

private:

  Shader( Graphics& graphics, const vk::ShaderModuleCreateInfo& info );


private:

  class Impl;
  std::unique_ptr<Impl> mImpl;
};

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif //DALI_VULKAN_161117_VULKAN_SHADER_H
