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
}
namespace VulkanAPI
{

/**
 * Render command stores internal command buffer per draw call
 */
class RenderCommand : public Graphics::API::RenderCommand
{
public:


  RenderCommand( Vulkan::Graphics& graphics,
                 Vulkan::PipelineCache& pipelineCache );

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

  Vulkan::Graphics&         mGraphics;
  Vulkan::PipelineCache&    mPipelineCache;
  Vulkan::CommandBufferRef  mCommandBuffer;
  Vulkan::PipelineRef       mPipeline;

  uint32_t mUpdateFlags;
};

}
}
}
#endif //DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_H
