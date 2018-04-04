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

#include <dali/graphics/vulkan/api/vulkan-api-render-command-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

enum class CommandType
{
  CMD_RESET,
};

template<class... Args>
struct RenderCommandCallback
{
  CommandType                                        commandType;
  std::function<void, RenderCommandBuffer&, Args...> delegate;
  std::tuple<Args...>                                args;
  RenderCommandBuffer& commandBuffer;
};

namespace
{

void CmdReset( RenderCommandBuffer& buffer )
{

}

void CmdBindVertexBuffer( uint32_t binding, API::Accessor<API::Buffer> vertexBuffer )
{

}

}


void RenderCommandBuffer::Reset()
{
  RenderCommandCallback<void> callback = {  CommandType::CMD_RESET, CmdReset, { void }  };
  mCommands.push_back(  )
}

// Commands ( for now, simplified )
void RenderCommandBuffer::BindVertexBuffer( uint32_t binding, API::Accessor<API::Buffer> vertexBuffer )
{

}

void RenderCommandBuffer::BindUniformBuffer( uint32_t binding, API::Accessor<API::Buffer> uniformBuffer )
{
}

void RenderCommandBuffer::BindIndexBuffer( API::Accessor<API::Buffer> indexBuffer )
{
}

void RenderCommandBuffer::BindShader( API::Accessor<API::Shader> shader )
{
}

void RenderCommandBuffer::BindTextures( uint32_t firstBinding, const API::TextureList& textures )
{
}

void RenderCommandBuffer::Draw( uint32_t vertexCount,
                                uint32_t instanceCount,
                                uint32_t firstVertex,
                                uint32_t firstInstance )
{
}

void RenderCommandBuffer::DrawIndexed( uint32_t indexCount,
                                       uint32_t instanceCount,
                                       uint32_t firstIndex,
                                       uint32_t firstInstance )
{
}

void RenderCommandBuffer::BindPipelineState()
{
}

void RenderCommandBuffer::BindFramebuffer()
{
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali

#endif //VULKAN_