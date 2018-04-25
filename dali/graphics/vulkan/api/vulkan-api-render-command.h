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

//#include <dali/graphics/vulkan/vulkan-command-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class PipelineCache;
class Controller;
}
namespace VulkanAPI
{

using Controller = Vulkan::Controller;

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


  RenderCommand( VulkanAPI::Controller& controller,
                 Vulkan::Graphics& graphics,
                 Vulkan::PipelineCache& pipelineCache );

  ~RenderCommand() override;

  /**
   * Forces pipeline compilation whenever something changed and
   * updates cache
   * @return
   */
  bool PreparePipeline();

  /**
   * Returns associated Vulkan command buffer
   * @return
   */
  const Vulkan::CommandBufferRef& GetCommandBuffer() const;

  /**
   * Returns pipeline cache
   * @return
   */
  const Vulkan::PipelineCache& GetPipelineCache() const;

  /**
   * Returns pipeline
   * @return
   */
  const Vulkan::PipelineRef& GetPipeline() const;

private:

  const vk::PipelineColorBlendStateCreateInfo*      PrepareColorBlendStateCreateInfo();

  const vk::PipelineDepthStencilStateCreateInfo*    PrepareDepthStencilStateCreateInfo();

  const vk::PipelineInputAssemblyStateCreateInfo*   PrepareInputAssemblyStateCreateInfo();

  const vk::PipelineMultisampleStateCreateInfo*     PrepareMultisampleStateCreateInfo();

  const vk::PipelineRasterizationStateCreateInfo*   PrepareRasterizationStateCreateInfo();

  const vk::PipelineTessellationStateCreateInfo*    PrepareTesselationStateCreateInfo();

  const vk::PipelineVertexInputStateCreateInfo*     PrepareVertexInputStateCreateInfo();

  const vk::PipelineViewportStateCreateInfo*        PrepareViewportStateCreateInfo();

  /**
   * Allocates UBO memory based on the pipeline. Executed only
   * once per pipeline
   */
  void AllocateUniformBufferMemory();

private:

  /**
   * Describes assigned UBO buffers
   */
/*
  struct UboBuffer
  {
    Vulkan::BufferRef buffer;
    uint32_t offset;
    uint32_t size;
  };
  */
  struct VulkanPipelineState;
  std::unique_ptr<VulkanPipelineState>  mVulkanPipelineState;

  VulkanAPI::Controller&                mController;
  Vulkan::Graphics&                     mGraphics;
  Vulkan::PipelineCache&                mPipelineCache;
  Vulkan::CommandBufferRef              mCommandBuffer;
  Vulkan::PipelineRef                   mPipeline;
  Vulkan::DescriptorPoolRef             mDescriptorPool;
  std::vector<Vulkan::DescriptorSetRef> mDescriptorSets;

  // all ubos assigned
  //std::vector<UboBuffer>                mUboBuffers;

  uint32_t mUpdateFlags;
};

}
}
}
#endif //DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_H
