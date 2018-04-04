#ifndef DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_BUFFER_H
#define DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_BUFFER_H

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

#include <dali/graphics-api/graphics-api-render-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics-controller.h>
#include <cstdint>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

class RenderCommand;

class RenderCommandBuffer : public API::RenderCommandBuffer
{
public:

  void Reset();

  // Commands ( for now, simplified )
  void BindVertexBuffer( uint32_t binding, API::Accessor<API::Buffer> vertexBuffer ) override;

  void BindUniformBuffer( uint32_t binding, API::Accessor<API::Buffer> uniformBuffer ) override;

  void BindIndexBuffer( API::Accessor<API::Buffer> indexBuffer ) override;

  void BindShader( API::Accessor<API::Shader> shader ) override;

  void BindTextures( uint32_t firstBinding, const API::TextureList& textures ) override;

  void Draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance ) override;

  void DrawIndexed( uint32_t indexCount,
                            uint32_t instanceCount,
                            uint32_t firstIndex,
                            uint32_t firstInstance ) override;

  void BindPipelineState() override;

  void BindFramebuffer() override;

private:

  std::vector<RenderCommand> mCommands;
};

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali

#endif //DALI_GRAPHICS_VULKAN_API_RENDER_COMMAND_BUFFER_H