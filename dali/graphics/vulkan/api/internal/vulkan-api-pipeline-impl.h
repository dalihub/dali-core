#ifndef DALI_GRAPHICS_VULKAN_API_PIPELINE_IMPL_H
#define DALI_GRAPHICS_VULKAN_API_PIPELINE_IMPL_H

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

#include <dali/graphics/vulkan/internal/vulkan-types.h>
#include <atomic>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
}

namespace VulkanAPI
{
class Pipeline;

class Controller;

class PipelineFactory;

class PipelineCache;
namespace Internal
{
class Pipeline
{
public:

  Pipeline( Vulkan::Graphics& graphics, Controller& controller, const PipelineFactory* factory );

  ~Pipeline();

  uint32_t GetHashCode() const;

  bool Initialise();

  void Reference();

  void Dereference();

  void Destroy();

  Vulkan::RefCountedPipeline GetVkPipeline() const;

  const std::vector< vk::DescriptorSetLayout >& GetVkDescriptorSetLayouts() const;

private:

  const vk::PipelineColorBlendStateCreateInfo* PrepareColorBlendStateCreateInfo();

  const vk::PipelineDepthStencilStateCreateInfo* PrepareDepthStencilStateCreateInfo();

  const vk::PipelineInputAssemblyStateCreateInfo* PrepareInputAssemblyStateCreateInfo();

  const vk::PipelineMultisampleStateCreateInfo* PrepareMultisampleStateCreateInfo();

  const vk::PipelineRasterizationStateCreateInfo* PrepareRasterizationStateCreateInfo();

  const vk::PipelineTessellationStateCreateInfo* PrepareTesselationStateCreateInfo();

  const vk::PipelineViewportStateCreateInfo* PrepareViewportStateCreateInfo();

  const vk::PipelineDynamicStateCreateInfo* PrepareDynamicStateCreatInfo();

  const vk::PipelineLayout PreparePipelineLayout();


  Vulkan::Graphics& mGraphics;
  Controller& mController;
  uint32_t mHashCode;

  std::atomic_int mRefCounter{ 0u };

  // wrapper for copy of cSreate data
  struct PipelineCreateInfo;
  std::unique_ptr< PipelineCreateInfo > mCreateInfo;

  struct VulkanPipelineState;
  std::unique_ptr< VulkanPipelineState > mVulkanPipelineState;

  std::vector< vk::DescriptorSetLayout > mVkDescriptorSetLayouts;

  PipelineCache* mPipelineCache;
};


}

}
}
}

#endif //DALI_GRAPHICS_VULKAN_API_PIPELINE_IMPL_H
