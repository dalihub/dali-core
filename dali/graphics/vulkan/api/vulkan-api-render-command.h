#ifndef DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_H
#define DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_H

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

#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class PipelineCache;
}

namespace VulkanAPI
{
class Controller;

class Ubo;

class Pipeline;

/**
 * Render command stores internal command buffer per draw call
 *
 * For Vulkan:
 *
 * - RenderCommand stores all the relevant data
 * - Changing pipeline determines recreating certain resources
 * - Currently each RC has own DescriptorSetPool
 * - Currently each RC has a handle to UBOs
 * - UBOs are assigned upon pipeline creation/assignment
 * - Descriptor sets are allocated upon pipeline creation
 * - Note: need resource "versioning"
 */
class RenderCommand : public Graphics::API::RenderCommand
{
public:

  constexpr static uint32_t UPDATE_ALL = 0xffffffff;

  RenderCommand( VulkanAPI::Controller& controller,
                 Vulkan::Graphics& graphics );

  ~RenderCommand() override;

  /**
   * Forces pipeline compilation whenever something changed and
   * updates cache
   * @return
   */
  void PrepareResources();

  /**
   * Updates uniform buffers
   */
  void UpdateUniformBuffers();

  /**
   * Writes uniform buffers into descriptor set
   */
  void BindUniformBuffers();

  /**
   * Writes texture/sample combo into descriptor set
   */
  void BindTexturesAndSamplers();

  /**
   * Binds vertex buffers
   */
  void BindVertexBuffers();

  /**
   * Returns an array of updated descriptor sets
   * @return
   */
  const std::vector< Vulkan::RefCountedDescriptorSet >& GetDescriptorSets() const;

  /**
   * Returns associated Vulkan command buffer
   * @return
   */
  const Vulkan::RefCountedCommandBuffer& GetCommandBuffer() const;

  /**
   * Returns Vulkan backed pipeline
   * @return
   */
  Vulkan::RefCountedPipeline GetVulkanPipeline() const;

private:

  /**
   * Allocates UBO memory based on the pipeline. Executed only
   * once per pipeline
   */
  void AllocateUniformBufferMemory();

private:

  VulkanAPI::Controller& mController;
  Vulkan::Graphics& mGraphics;
  Vulkan::RefCountedCommandBuffer mCommandBuffer;
  Vulkan::RefCountedPipeline mVulkanPipeline;
  Vulkan::RefCountedDescriptorPool mDescriptorPool;

  std::vector< vk::DescriptorSetLayout > mVkDescriptorSetLayouts;

  std::vector< Vulkan::RefCountedDescriptorSet > mDescriptorSets;

  std::vector< std::unique_ptr< Ubo>> mUboBuffers;

  uint32_t mUpdateFlags;
};

}
}
}
#endif //DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_H
