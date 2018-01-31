#ifndef DALI_VULKAN_161117_VULKAN_PIPELINE_H
#define DALI_VULKAN_161117_VULKAN_PIPELINE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/graphics/vulkan/vulkan-shader.h>
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
class Pipeline : public VkManaged
{
public:

  /**
   * Creates immutable Pipeline object ( verified resources for tracking )
   * @param graphics
   * @param info
   * @return
   */
  static Handle<Pipeline> New( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info );

  /**
   * Destructor
   */
  ~Pipeline() override;

  /**
   * Sets the viewport
   * @param x
   * @param y
   * @param width
   * @param height
   */
  void SetViewport( float x, float y, float width, float height );

  /**
   * Attaches A shader module to the particular stage of pipeline
   * @param shader
   * @param stage
   * @return
   */
  bool SetShader( ShaderHandle shader, Shader::Type stage );

  /**
   *
   * @param attrDesc
   * @param bindingDesc
   */
  void SetVertexInputState(std::vector<vk::VertexInputAttributeDescription> attrDesc,
                           std::vector<vk::VertexInputBindingDescription> bindingDesc);

  /**
   *
   * @param topology
   * @param restartEnable
   */
  void SetInputAssemblyState( vk::PrimitiveTopology topology, bool restartEnable );
  /**
   *
   * @return
   */
  bool Compile();

private:

  Pipeline( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info );

public:

  /**
   * Returns Vulkan VkPipeline object associated with Pipeline
   * @return
   */
  vk::Pipeline GetVkPipeline() const;

  /**
   * Returns Vulkan VkPipelineLayout object associated with Pipeline
   * @return
   */
  vk::PipelineLayout GetVkPipelineLayout() const;

  bool OnDestroy() override;

private:
  class Impl;
  std::unique_ptr<Impl> mImpl;
};

using PipelineHandle = Handle<Pipeline>;

}
}
} // Namesp

#endif //DALI_VULKAN_161117_VULKAN_PIPELINE_H
