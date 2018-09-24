#ifndef DALI_GRAPHICS_VULKAN_API_PIPELINE_H
#define DALI_GRAPHICS_VULKAN_API_PIPELINE_H

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

#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>
#include <dali/graphics-api/graphics-api-pipeline.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;

class PipelineCache;
}

namespace VulkanAPI
{
namespace Internal
{
class Pipeline;
}

class Controller;

class PipelineFactory;

/**
 * Pipeline due to its nature ( it's cached, single ownership ) is only an interface
 */
class Pipeline : public API::Pipeline
{
public:

  Pipeline( Vulkan::Graphics& graphics, Controller& controller, const PipelineFactory* factory );

  ~Pipeline() override;

  // non-copyable
  Pipeline( const Pipeline& ) = delete;

  Pipeline& operator=( const Pipeline& ) = delete;

  // movable
  Pipeline( Pipeline&& ) = default;

  Pipeline& operator=( Pipeline&& ) = default;

  Pipeline( Internal::Pipeline* impl );

  const vk::Pipeline& GetVkPipeline() const;

  const vk::PipelineLayout& GetVkPipelineLayout() const;

  const std::vector< vk::DescriptorSetLayout >& GetVkDescriptorSetLayouts() const;

  const std::vector< Vulkan::DescriptorSetLayoutSignature>& GetDescriptorSetLayoutSignatures() const;

  API::PipelineDynamicStateMask GetDynamicStateMask() const;

  void Bind( Vulkan::RefCountedCommandBuffer& commandBuffer ) const;

  bool HasDepthEnabled() const;

  bool HasStencilEnabled() const;

public:

  Internal::Pipeline* GetImplementation() const
  {
    return mPipelineImpl;
  }

private:
  Internal::Pipeline* mPipelineImpl;
};
}
}
}

#endif // DALI_GRAPHICS_VULKAN_API_PIPELINE_H
