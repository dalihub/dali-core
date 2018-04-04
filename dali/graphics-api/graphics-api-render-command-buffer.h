#ifndef DALI_GRAPHICS_API_RENDER_COMMAND_BUFFER_H
#define DALI_GRAPHICS_API_RENDER_COMMAND_BUFFER_H

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

namespace Dali
{
namespace Graphics
{
namespace API
{
class RenderCommandBuffer
{
  // Commands ( for now, simplified )
  virtual void BindVertexBuffer( uint32_t binding, Accessor<Buffer> vertexBuffer ) = 0;

  virtual void BindUniformBuffer( uint32_t binding, Accessor<Buffer> uniformBuffer ) = 0;

  virtual void BindIndexBuffer( Accessor<Buffer> indexBuffer ) = 0;

  virtual void BindShader( Accessor<Shader> shader ) = 0;

  virtual void BindTextures( uint32_t firstBinding, const TextureList& textures ) = 0;

  virtual void Draw( uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance ) = 0;

  virtual void DrawIndexed( uint32_t indexCount,
                            uint32_t instanceCount,
                            uint32_t firstIndex,
                            uint32_t firstInstance ) = 0;

  virtual void BindPipelineState() = 0;

  virtual void BindFramebuffer() = 0;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif //DALI_GRAPHICS_API_RENDER_COMMAND_BUFFER_H
