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

#include "test-graphics-command-buffer.h"

namespace Dali
{
TestGraphicsCommandBuffer::TestGraphicsCommandBuffer(TraceCallStack& callstack, TestGlAbstraction& glAbstraction)
: mCallStack(callstack),
  mGlAbstraction(glAbstraction)
{
}

void TestGraphicsCommandBuffer::BindVertexBuffers(uint32_t                             firstBinding,
                                                  std::vector<const Graphics::Buffer*> buffers,
                                                  std::vector<uint32_t>                offsets)
{
  mCallStack.PushCall("BindVertexBuffers", "");
}

void TestGraphicsCommandBuffer::BindUniformBuffers(const std::vector<Graphics::UniformBufferBinding>& bindings)
{
  mCallStack.PushCall("BindUniformBuffers", "");
}

void TestGraphicsCommandBuffer::BindPipeline(const Graphics::Pipeline& pipeline)
{
  mCallStack.PushCall("BindPipeline", "");
}

void TestGraphicsCommandBuffer::BindTextures(std::vector<Graphics::TextureBinding>& textureBindings)
{
  mCallStack.PushCall("BindTextures", "");
  for(auto& binding : textureBindings)
  {
    mTextureBindings.push_back(binding);
  }
}

void TestGraphicsCommandBuffer::BindSamplers(std::vector<Graphics::SamplerBinding>& samplerBindings)
{
  mCallStack.PushCall("BindSamplers", "");
}

void TestGraphicsCommandBuffer::BindPushConstants(void*    data,
                                                  uint32_t size,
                                                  uint32_t binding)
{
  mCallStack.PushCall("BindPushConstants", "");
}

void TestGraphicsCommandBuffer::BindIndexBuffer(const Graphics::Buffer& buffer,
                                                uint32_t                offset,
                                                Graphics::Format        format)
{
  mCallStack.PushCall("BindIndexBuffer", "");
}

void TestGraphicsCommandBuffer::BeginRenderPass(
  Graphics::RenderPass&             renderPass,
  Graphics::RenderTarget&           renderTarget,
  Graphics::Extent2D                renderArea,
  std::vector<Graphics::ClearValue> clearValues)
{
  mCallStack.PushCall("BeginRenderPass", "");
}

void TestGraphicsCommandBuffer::EndRenderPass()
{
  mCallStack.PushCall("EndRenderPass", "");
}

void TestGraphicsCommandBuffer::Draw(
  uint32_t vertexCount,
  uint32_t instanceCount,
  uint32_t firstVertex,
  uint32_t firstInstance)
{
  mCallStack.PushCall("Draw", "");
}

void TestGraphicsCommandBuffer::DrawIndexed(
  uint32_t indexCount,
  uint32_t instanceCount,
  uint32_t firstIndex,
  int32_t  vertexOffset,
  uint32_t firstInstance)
{
  mCallStack.PushCall("DrawIndexed", "");
}

void TestGraphicsCommandBuffer::DrawIndexedIndirect(
  Graphics::Buffer& buffer,
  uint32_t          offset,
  uint32_t          drawCount,
  uint32_t          stride)
{
  mCallStack.PushCall("DrawIndexedIndirect", "");
}

void TestGraphicsCommandBuffer::Reset(Graphics::CommandBuffer& commandBuffer)
{
  mCallStack.PushCall("Reset", "");
}

void TestGraphicsCommandBuffer::SetScissor(Graphics::Extent2D value)
{
  mCallStack.PushCall("SetScissor", "");
}

void TestGraphicsCommandBuffer::SetScissorTestEnable(bool value)
{
  mCallStack.PushCall("SetScissorTestEnable", "");
}

void TestGraphicsCommandBuffer::SetViewport(Graphics::Viewport value)
{
  mCallStack.PushCall("SetViewport", "");
}

void TestGraphicsCommandBuffer::SetViewportEnable(bool value)
{
  mCallStack.PushCall("SetViewportEnable", "");
}

} // namespace Dali
