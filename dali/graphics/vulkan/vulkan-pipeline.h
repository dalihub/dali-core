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

  ~Pipeline() override;

  /**
   *
   * @param x
   * @param y
   * @param width
   * @param height
   */
  void SetViewport( float x, float y, float width, float height );

  /**
   *
   * @param shader
   * @param stage
   * @return
   */
  bool SetShader( ShaderHandle shader, Shader::Type stage );

  /**
   *
   * @return
   */
  bool Compile();

private:

  Pipeline( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info );

public:

  vk::Pipeline GetVkPipeline() const;

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
