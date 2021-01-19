#ifndef DALI_TEST_GRAPHICS_COMMAND_BUFFER_H
#define DALI_TEST_GRAPHICS_COMMAND_BUFFER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

#include <dali/graphics-api/graphics-command-buffer-create-info.h>
#include <dali/graphics-api/graphics-command-buffer.h>
#include <dali/graphics-api/graphics-types.h>
#include <cstdint>
#include <vector>
#include "test-gl-abstraction.h"
#include "test-trace-call-stack.h"

namespace Dali
{
class TestGraphicsCommandBuffer : public Graphics::CommandBuffer
{
public:
  TestGraphicsCommandBuffer(TraceCallStack& callstack, TestGlAbstraction& glAbstraction);

  void BindVertexBuffers(uint32_t                             firstBinding,
                         std::vector<const Graphics::Buffer*> buffers,
                         std::vector<uint32_t>                offsets);

  void BindUniformBuffers(const std::vector<Graphics::UniformBufferBinding>& bindings);

  void BindPipeline(const Graphics::Pipeline& pipeline);

  void BindTextures(std::vector<Graphics::TextureBinding>& textureBindings);

  void BindSamplers(std::vector<Graphics::SamplerBinding>& samplerBindings);

  void BindPushConstants(void*    data,
                         uint32_t size,
                         uint32_t binding);

  void BindIndexBuffer(const Graphics::Buffer& buffer,
                       uint32_t                offset,
                       Graphics::Format        format);

  void BeginRenderPass(Graphics::RenderPass&             renderPass,
                       Graphics::RenderTarget&           renderTarget,
                       Graphics::Extent2D                renderArea,
                       std::vector<Graphics::ClearValue> clearValues);

  void EndRenderPass();

  void Draw(
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance);

  void DrawIndexed(
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t  vertexOffset,
    uint32_t firstInstance);

  void DrawIndexedIndirect(
    Graphics::Buffer& buffer,
    uint32_t          offset,
    uint32_t          drawCount,
    uint32_t          stride);

  void Reset(Graphics::CommandBuffer& commandBuffer);

  void SetScissor(Graphics::Extent2D value);

  void SetScissorTestEnable(bool value);

  void SetViewport(Graphics::Viewport value);

  void SetViewportEnable(bool value);

public:
  TraceCallStack     mCallStack;
  TestGlAbstraction& mGlAbstraction;

  std::vector<Graphics::TextureBinding> mTextureBindings;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_COMMAND_BUFFER_H
