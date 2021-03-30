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

#include "test-graphics-controller.h"

#include "test-graphics-buffer.h"
#include "test-graphics-command-buffer.h"
#include "test-graphics-reflection.h"
#include "test-graphics-sampler.h"
#include "test-graphics-shader.h"
#include "test-graphics-texture.h"

#include <dali/integration-api/gl-defines.h>
#include <cstdio>
#include <iostream>
#include <sstream>

namespace Dali
{
template<typename T>
T* Uncast(const Graphics::CommandBuffer* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Texture* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Sampler* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Buffer* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

template<typename T>
T* Uncast(const Graphics::Shader* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

std::ostream& operator<<(std::ostream& o, const Graphics::BufferCreateInfo& bufferCreateInfo)
{
  return o << "usage:" << std::hex << bufferCreateInfo.usage << ", size:" << std::dec << bufferCreateInfo.size;
}

std::ostream& operator<<(std::ostream& o, const Graphics::CommandBufferCreateInfo& commandBufferCreateInfo)
{
  return o << "level:" << (commandBufferCreateInfo.level == Graphics::CommandBufferLevel::PRIMARY ? "PRIMARY" : "SECONDARY")
           << ", fixedCapacity:" << std::dec << commandBufferCreateInfo.fixedCapacity;
}

std::ostream& operator<<(std::ostream& o, const Graphics::TextureType& textureType)
{
  switch(textureType)
  {
    case Graphics::TextureType::TEXTURE_2D:
      o << "TEXTURE_2D";
      break;
    case Graphics::TextureType::TEXTURE_3D:
      o << "TEXTURE_3D";
      break;
    case Graphics::TextureType::TEXTURE_CUBEMAP:
      o << "TEXTURE_CUBEMAP";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::Extent2D extent)
{
  o << "width:" << extent.width << ", height:" << extent.height;
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::TextureCreateInfo& createInfo)
{
  o << "textureType:" << createInfo.textureType
    << " size:" << createInfo.size
    << " format:" << static_cast<uint32_t>(createInfo.format)
    << " mipMapFlag:" << createInfo.mipMapFlag
    << " layout:" << (createInfo.layout == Graphics::TextureLayout::LINEAR ? "LINEAR" : "OPTIMAL")
    << " usageFlags:" << std::hex << createInfo.usageFlags
    << " data:" << std::hex << createInfo.data
    << " dataSize:" << std::dec << createInfo.dataSize
    << " nativeImagePtr:" << std::hex << createInfo.nativeImagePtr;
  return o;
}

std::ostream& operator<<(std::ostream& o, Graphics::SamplerAddressMode addressMode)
{
  switch(addressMode)
  {
    case Graphics::SamplerAddressMode::REPEAT:
      o << "REPEAT";
      break;
    case Graphics::SamplerAddressMode::MIRRORED_REPEAT:
      o << "MIRRORED_REPEAT";
      break;
    case Graphics::SamplerAddressMode::CLAMP_TO_EDGE:
      o << "CLAMP_TO_EDGE";
      break;
    case Graphics::SamplerAddressMode::CLAMP_TO_BORDER:
      o << "CLAMP_TO_BORDER";
      break;
    case Graphics::SamplerAddressMode::MIRROR_CLAMP_TO_EDGE:
      o << "MIRROR_CLAMP_TO_EDGE";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, Graphics::SamplerFilter filterMode)
{
  switch(filterMode)
  {
    case Graphics::SamplerFilter::LINEAR:
      o << "LINEAR";
      break;
    case Graphics::SamplerFilter::NEAREST:
      o << "NEAREST";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, Graphics::SamplerMipmapMode mipmapMode)
{
  switch(mipmapMode)
  {
    case Graphics::SamplerMipmapMode::NONE:
      o << "NONE";
      break;
    case Graphics::SamplerMipmapMode::LINEAR:
      o << "LINEAR";
      break;
    case Graphics::SamplerMipmapMode::NEAREST:
      o << "NEAREST";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::SamplerCreateInfo& createInfo)
{
  o << "minFilter:" << createInfo.minFilter
    << " magFilter:" << createInfo.magFilter
    << " wrapModeU:" << createInfo.addressModeU
    << " wrapModeV:" << createInfo.addressModeV
    << " wrapModeW:" << createInfo.addressModeW
    << " mipMapMode:" << createInfo.mipMapMode;
  return o;
}

class TestGraphicsMemory : public Graphics::Memory
{
public:
  TestGraphicsMemory(TraceCallStack& callStack, TestGraphicsBuffer& buffer, uint32_t mappedOffset, uint32_t mappedSize)
  : mCallStack(callStack),
    mBuffer(buffer),
    mMappedOffset(mappedOffset),
    mMappedSize(mappedSize),
    mLockedOffset(0u),
    mLockedSize(0u)
  {
  }

  void* LockRegion(uint32_t offset, uint32_t size) override
  {
    std::ostringstream o;
    o << offset << ", " << size;
    mCallStack.PushCall("Memory::LockRegion", o.str());

    if(offset > mMappedOffset + mMappedSize ||
       size + offset > mMappedOffset + mMappedSize)
    {
      fprintf(stderr, "TestGraphics.Memory::LockRegion() Out of bounds");
      mBuffer.memory.resize(mMappedOffset + offset + size); // Grow to prevent memcpy from crashing
    }
    mLockedOffset = offset;
    mLockedSize   = size;
    return &mBuffer.memory[mMappedOffset + offset];
  }

  void Unlock(bool flush) override
  {
    mCallStack.PushCall("Memory::Unlock", (flush ? "Flush" : "NoFlush"));
    if(flush)
    {
      Flush();
    }
  }

  void Flush() override
  {
    mCallStack.PushCall("Memory::Flush", "");
    mBuffer.Bind();
    mBuffer.Upload(mMappedOffset + mLockedOffset, mLockedSize);
    mBuffer.Unbind();
  }

  TraceCallStack&     mCallStack;
  TestGraphicsBuffer& mBuffer;
  uint32_t            mMappedOffset;
  uint32_t            mMappedSize;
  uint32_t            mLockedOffset;
  uint32_t            mLockedSize;
};

TestGraphicsController::TestGraphicsController()
: mCallStack(true, "TestGraphicsController."),
  mCommandBufferCallStack(true, "TestCommandBuffer.")
{
  mCallStack.Enable(true);
  mCommandBufferCallStack.Enable(true);
  auto& trace = mGl.GetTextureTrace();
  trace.Enable(true);
  trace.EnableLogging(true);
}

int GetNumComponents(Graphics::VertexInputFormat vertexFormat)
{
  switch(vertexFormat)
  {
    case Graphics::VertexInputFormat::UNDEFINED:
    case Graphics::VertexInputFormat::FLOAT:
    case Graphics::VertexInputFormat::INTEGER:
      return 1;
    case Graphics::VertexInputFormat::IVECTOR2:
    case Graphics::VertexInputFormat::FVECTOR2:
      return 2;
    case Graphics::VertexInputFormat::IVECTOR3:
    case Graphics::VertexInputFormat::FVECTOR3:
      return 3;
    case Graphics::VertexInputFormat::FVECTOR4:
    case Graphics::VertexInputFormat::IVECTOR4:
      return 4;
  }
  return 1;
}

GLint GetSize(Graphics::VertexInputFormat vertexFormat)
{
  switch(vertexFormat)
  {
    case Graphics::VertexInputFormat::UNDEFINED:
      return 1u;
    case Graphics::VertexInputFormat::INTEGER:
    case Graphics::VertexInputFormat::IVECTOR2:
    case Graphics::VertexInputFormat::IVECTOR3:
    case Graphics::VertexInputFormat::IVECTOR4:
      return 2u;
    case Graphics::VertexInputFormat::FLOAT:
    case Graphics::VertexInputFormat::FVECTOR2:
    case Graphics::VertexInputFormat::FVECTOR3:
    case Graphics::VertexInputFormat::FVECTOR4:
      return 4u;
  }
  return 1u;
}

GLint GetGlType(Graphics::VertexInputFormat vertexFormat)
{
  switch(vertexFormat)
  {
    case Graphics::VertexInputFormat::UNDEFINED:
      return GL_BYTE;
    case Graphics::VertexInputFormat::INTEGER:
    case Graphics::VertexInputFormat::IVECTOR2:
    case Graphics::VertexInputFormat::IVECTOR3:
    case Graphics::VertexInputFormat::IVECTOR4:
      return GL_SHORT;
    case Graphics::VertexInputFormat::FLOAT:
    case Graphics::VertexInputFormat::FVECTOR2:
    case Graphics::VertexInputFormat::FVECTOR3:
    case Graphics::VertexInputFormat::FVECTOR4:
      return GL_FLOAT;
  }
  return GL_BYTE;
}

GLenum GetTopology(Graphics::PrimitiveTopology topology)
{
  switch(topology)
  {
    case Graphics::PrimitiveTopology::POINT_LIST:
      return GL_POINTS;

    case Graphics::PrimitiveTopology::LINE_LIST:
      return GL_LINES;

    case Graphics::PrimitiveTopology::LINE_LOOP:
      return GL_LINE_LOOP;

    case Graphics::PrimitiveTopology::LINE_STRIP:
      return GL_LINE_STRIP;

    case Graphics::PrimitiveTopology::TRIANGLE_LIST:
      return GL_TRIANGLES;

    case Graphics::PrimitiveTopology::TRIANGLE_STRIP:
      return GL_TRIANGLE_STRIP;

    case Graphics::PrimitiveTopology::TRIANGLE_FAN:
      return GL_TRIANGLE_FAN;
  }
  return GL_TRIANGLES;
}

GLenum GetCullFace(Graphics::CullMode cullMode)
{
  switch(cullMode)
  {
    case Graphics::CullMode::NONE:
      return GL_NONE;
    case Graphics::CullMode::FRONT:
      return GL_FRONT;
    case Graphics::CullMode::BACK:
      return GL_BACK;
    case Graphics::CullMode::FRONT_AND_BACK:
      return GL_FRONT_AND_BACK;
  }
  return GL_NONE;
}

GLenum GetFrontFace(Graphics::FrontFace frontFace)
{
  if(frontFace == Graphics::FrontFace::CLOCKWISE)
  {
    return GL_CW;
  }
  return GL_CCW;
}

GLenum GetBlendFactor(Graphics::BlendFactor blendFactor)
{
  GLenum glFactor = GL_ZERO;

  switch(blendFactor)
  {
    case Graphics::BlendFactor::ZERO:
      glFactor = GL_ZERO;
      break;
    case Graphics::BlendFactor::ONE:
      glFactor = GL_ONE;
      break;
    case Graphics::BlendFactor::SRC_COLOR:
      glFactor = GL_SRC_COLOR;
      break;
    case Graphics::BlendFactor::ONE_MINUS_SRC_COLOR:
      glFactor = GL_ONE_MINUS_SRC_COLOR;
      break;
    case Graphics::BlendFactor::DST_COLOR:
      glFactor = GL_DST_COLOR;
      break;
    case Graphics::BlendFactor::ONE_MINUS_DST_COLOR:
      glFactor = GL_ONE_MINUS_DST_COLOR;
      break;
    case Graphics::BlendFactor::SRC_ALPHA:
      glFactor = GL_SRC_ALPHA;
      break;
    case Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA:
      glFactor = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case Graphics::BlendFactor::DST_ALPHA:
      glFactor = GL_DST_ALPHA;
      break;
    case Graphics::BlendFactor::ONE_MINUS_DST_ALPHA:
      glFactor = GL_ONE_MINUS_DST_ALPHA;
      break;
    case Graphics::BlendFactor::CONSTANT_COLOR:
      glFactor = GL_CONSTANT_COLOR;
      break;
    case Graphics::BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      glFactor = GL_ONE_MINUS_CONSTANT_COLOR;
      break;
    case Graphics::BlendFactor::CONSTANT_ALPHA:
      glFactor = GL_CONSTANT_ALPHA;
      break;
    case Graphics::BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      glFactor = GL_ONE_MINUS_CONSTANT_ALPHA;
      break;
    case Graphics::BlendFactor::SRC_ALPHA_SATURATE:
      glFactor = GL_SRC_ALPHA_SATURATE;
      break;
      // GLES doesn't appear to have dual source blending.
    case Graphics::BlendFactor::SRC1_COLOR:
      glFactor = GL_SRC_COLOR;
      break;
    case Graphics::BlendFactor::ONE_MINUS_SRC1_COLOR:
      glFactor = GL_ONE_MINUS_SRC_COLOR;
      break;
    case Graphics::BlendFactor::SRC1_ALPHA:
      glFactor = GL_SRC_ALPHA;
      break;
    case Graphics::BlendFactor::ONE_MINUS_SRC1_ALPHA:
      glFactor = GL_ONE_MINUS_SRC_ALPHA;
      break;
  }
  return glFactor;
}

GLenum GetBlendOp(Graphics::BlendOp blendOp)
{
  GLenum op = GL_FUNC_ADD;
  switch(blendOp)
  {
    case Graphics::BlendOp::ADD:
      op = GL_FUNC_ADD;
      break;
    case Graphics::BlendOp::SUBTRACT:
      op = GL_FUNC_SUBTRACT;
      break;
    case Graphics::BlendOp::REVERSE_SUBTRACT:
      op = GL_FUNC_REVERSE_SUBTRACT;
      break;
    case Graphics::BlendOp::MIN:
      op = GL_MIN;
      break;
    case Graphics::BlendOp::MAX:
      op = GL_MAX;
      break;

      // @todo Add advanced blend equations
  }
  return op;
}

void TestGraphicsController::SubmitCommandBuffers(const Graphics::SubmitInfo& submitInfo)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["submitInfo"] << "cmdBuffer[" << submitInfo.cmdBuffer.size()
                            << "], flags:" << std::hex << submitInfo.flags;

  mCallStack.PushCall("SubmitCommandBuffers", "", namedParams);

  mSubmitStack.emplace_back(submitInfo);

  for(auto& graphicsCommandBuffer : submitInfo.cmdBuffer)
  {
    auto commandBuffer = Uncast<TestGraphicsCommandBuffer>(graphicsCommandBuffer);

    auto value = commandBuffer->GetCommandsByType(0 | CommandType::BIND_TEXTURES);
    if(!value.empty())
    {
      // must be fixed
      for(auto& binding : value[0]->data.bindTextures.textureBindings)
      {
        if(binding.texture)
        {
          auto texture = Uncast<TestGraphicsTexture>(binding.texture);

          texture->Bind(binding.binding);

          if(binding.sampler)
          {
            auto sampler = Uncast<TestGraphicsSampler>(binding.sampler);
            if(sampler)
            {
              sampler->Apply(texture->GetTarget());
            }
          }

          texture->Prepare(); // Ensure native texture is ready
        }
      }
    }

    // IndexBuffer binding,
    auto bindIndexBufferCmds = commandBuffer->GetCommandsByType(0 | CommandType::BIND_INDEX_BUFFER);
    if(!bindIndexBufferCmds.empty())
    {
      auto& indexBufferBinding = bindIndexBufferCmds[0]->data.bindIndexBuffer;
      if(indexBufferBinding.buffer)
      {
        auto buffer = Uncast<TestGraphicsBuffer>(indexBufferBinding.buffer);
        buffer->Bind();
      }
    }

    // VertexBuffer binding,
    auto bindVertexBufferCmds = commandBuffer->GetCommandsByType(0 | CommandType::BIND_VERTEX_BUFFERS);
    if(!bindVertexBufferCmds.empty())
    {
      for(auto& binding : bindVertexBufferCmds[0]->data.bindVertexBuffers.vertexBufferBindings)
      {
        auto graphicsBuffer = binding.buffer;
        auto vertexBuffer   = Uncast<TestGraphicsBuffer>(graphicsBuffer);
        vertexBuffer->Bind();
      }
    }

    bool scissorEnabled = false;

    auto scissorTestList = commandBuffer->GetCommandsByType(0 | CommandType::SET_SCISSOR_TEST);
    if(!scissorTestList.empty())
    {
      if(scissorTestList[0]->data.scissorTest.enable)
      {
        mGl.Enable(GL_SCISSOR_TEST);
        scissorEnabled = true;
      }
      else
      {
        mGl.Disable(GL_SCISSOR_TEST);
      }
    }

    auto scissorList = commandBuffer->GetCommandsByType(0 | CommandType::SET_SCISSOR);
    if(!scissorList.empty() && scissorEnabled)
    {
      auto& rect = scissorList[0]->data.scissor.region;
      mGl.Scissor(rect.x, rect.y, rect.width, rect.height);
    }

    auto viewportList = commandBuffer->GetCommandsByType(0 | CommandType::SET_VIEWPORT);
    if(!viewportList.empty())
    {
      mGl.Viewport(viewportList[0]->data.viewport.region.x, viewportList[0]->data.viewport.region.y, viewportList[0]->data.viewport.region.width, viewportList[0]->data.viewport.region.height);
    }

    // ignore viewport enable

    // Pipeline attribute setup
    auto bindPipelineCmds = commandBuffer->GetCommandsByType(0 | CommandType::BIND_PIPELINE);
    if(!bindPipelineCmds.empty())
    {
      auto  pipeline = bindPipelineCmds[0]->data.bindPipeline.pipeline;
      auto& vi       = pipeline->vertexInputState;
      for(auto& attribute : vi.attributes)
      {
        mGl.EnableVertexAttribArray(attribute.location);
        uint32_t attributeOffset = attribute.offset;
        GLsizei  stride          = vi.bufferBindings[attribute.binding].stride;

        mGl.VertexAttribPointer(attribute.location,
                                GetNumComponents(attribute.format),
                                GetGlType(attribute.format),
                                GL_FALSE, // Not normalized
                                stride,
                                reinterpret_cast<void*>(attributeOffset));
      }

      // Cull face setup
      auto& rasterizationState = pipeline->rasterizationState;
      if(rasterizationState.cullMode == Graphics::CullMode::NONE)
      {
        mGl.Disable(GL_CULL_FACE);
      }
      else
      {
        mGl.Enable(GL_CULL_FACE);
        mGl.CullFace(GetCullFace(rasterizationState.cullMode));
      }

      mGl.FrontFace(GetFrontFace(rasterizationState.frontFace));
      // We don't modify glPolygonMode in our context/abstraction from GL_FILL (the GL default),
      // so it isn't present in the API (and won't have any tests!)

      // Blending setup
      auto& colorBlendState = pipeline->colorBlendState;
      if(colorBlendState.blendEnable)
      {
        mGl.Enable(GL_BLEND);

        mGl.BlendFuncSeparate(GetBlendFactor(colorBlendState.srcColorBlendFactor),
                              GetBlendFactor(colorBlendState.dstColorBlendFactor),
                              GetBlendFactor(colorBlendState.srcAlphaBlendFactor),
                              GetBlendFactor(colorBlendState.dstAlphaBlendFactor));
        if(colorBlendState.colorBlendOp != colorBlendState.alphaBlendOp)
        {
          mGl.BlendEquationSeparate(GetBlendOp(colorBlendState.colorBlendOp), GetBlendOp(colorBlendState.alphaBlendOp));
        }
        else
        {
          mGl.BlendEquation(GetBlendOp(colorBlendState.colorBlendOp));
        }
        mGl.BlendColor(colorBlendState.blendConstants[0],
                       colorBlendState.blendConstants[1],
                       colorBlendState.blendConstants[2],
                       colorBlendState.blendConstants[3]);
      }
      else
      {
        mGl.Disable(GL_BLEND);
      }

      // draw call
      auto topology = pipeline->inputAssemblyState.topology;

      // UniformBuffer binding (once we know pipeline)
      auto bindUniformBuffersCmds = commandBuffer->GetCommandsByType(0 | CommandType::BIND_UNIFORM_BUFFER);
      if(!bindUniformBuffersCmds.empty())
      {
        auto buffer = bindUniformBuffersCmds[0]->data.bindUniformBuffers.standaloneUniformsBufferBinding;

        // based on reflection, issue gl calls
        buffer.buffer->BindAsUniformBuffer(static_cast<const TestGraphicsProgram*>(pipeline->programState.program));
      }

      auto drawCmds = commandBuffer->GetCommandsByType(0 |
                                                       CommandType::DRAW |
                                                       CommandType::DRAW_INDEXED_INDIRECT |
                                                       CommandType::DRAW_INDEXED);

      if(!drawCmds.empty())
      {
        if(drawCmds[0]->data.draw.type == DrawCallDescriptor::Type::DRAW_INDEXED)
        {
          mGl.DrawElements(GetTopology(topology),
                           static_cast<GLsizei>(drawCmds[0]->data.draw.drawIndexed.indexCount),
                           GL_UNSIGNED_SHORT,
                           reinterpret_cast<void*>(drawCmds[0]->data.draw.drawIndexed.firstIndex));
        }
        else
        {
          mGl.DrawArrays(GetTopology(topology), 0, drawCmds[0]->data.draw.draw.vertexCount);
        }
      }
      // attribute clear
      for(auto& attribute : vi.attributes)
      {
        mGl.DisableVertexAttribArray(attribute.location);
      }
    }
  }
}

/**
 * @brief Presents render target
 * @param renderTarget render target to present
 */
void TestGraphicsController::PresentRenderTarget(Graphics::RenderTarget* renderTarget)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["renderTarget"] << std::hex << renderTarget;
  mCallStack.PushCall("PresentRenderTarget", "", namedParams);
}

/**
 * @brief Waits until the GPU is idle
 */
void TestGraphicsController::WaitIdle()
{
  mCallStack.PushCall("WaitIdle", "");
}

/**
 * @brief Lifecycle pause event
 */
void TestGraphicsController::Pause()
{
  mCallStack.PushCall("Pause", "");
}

/**
 * @brief Lifecycle resume event
 */
void TestGraphicsController::Resume()
{
  mCallStack.PushCall("Resume", "");
}

void TestGraphicsController::UpdateTextures(const std::vector<Graphics::TextureUpdateInfo>&       updateInfoList,
                                            const std::vector<Graphics::TextureUpdateSourceInfo>& sourceList)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["updateInfoList"] << "[" << updateInfoList.size() << "]:";
  namedParams["sourceList"] << "[" << sourceList.size() << "]:";

  mCallStack.PushCall("UpdateTextures", "", namedParams);

  // Call either TexImage2D or TexSubImage2D
  for(unsigned int i = 0; i < updateInfoList.size(); ++i)
  {
    auto& updateInfo = updateInfoList[i];
    auto& source     = sourceList[i];

    auto texture = static_cast<TestGraphicsTexture*>(updateInfo.dstTexture);
    texture->Bind(0); // Use first texture unit during resource update
    texture->Update(updateInfo, source);
  }
}

bool TestGraphicsController::EnableDepthStencilBuffer(bool enableDepth, bool enableStencil)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["enableDepth"] << (enableDepth ? "T" : "F");
  namedParams["enableStencil"] << (enableStencil ? "T" : "F");
  mCallStack.PushCall("EnableDepthStencilBuffer", "", namedParams);
  return false;
}

void TestGraphicsController::RunGarbageCollector(size_t numberOfDiscardedRenderers)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["numberOfDiscardedRenderers"] << numberOfDiscardedRenderers;
  mCallStack.PushCall("RunGarbageCollector", "", namedParams);
}

void TestGraphicsController::DiscardUnusedResources()
{
  mCallStack.PushCall("DiscardUnusedResources", "");
}

bool TestGraphicsController::IsDiscardQueueEmpty()
{
  mCallStack.PushCall("IsDiscardQueueEmpty", "");
  return isDiscardQueueEmptyResult;
}

/**
 * @brief Test if the graphics subsystem has resumed & should force a draw
 *
 * @return true if the graphics subsystem requires a re-draw
 */
bool TestGraphicsController::IsDrawOnResumeRequired()
{
  mCallStack.PushCall("IsDrawOnResumeRequired", "");
  return isDrawOnResumeRequiredResult;
}

Graphics::UniquePtr<Graphics::Buffer> TestGraphicsController::CreateBuffer(const Graphics::BufferCreateInfo& createInfo, Graphics::UniquePtr<Graphics::Buffer>&& oldBuffer)
{
  std::ostringstream oss;
  oss << "bufferCreateInfo:" << createInfo;
  mCallStack.PushCall("CreateBuffer", oss.str());
  return Graphics::MakeUnique<TestGraphicsBuffer>(mCallStack, mGl, createInfo.size, createInfo.usage);
}

Graphics::UniquePtr<Graphics::CommandBuffer> TestGraphicsController::CreateCommandBuffer(const Graphics::CommandBufferCreateInfo& commandBufferCreateInfo, Graphics::UniquePtr<Graphics::CommandBuffer>&& oldCommandBuffer)
{
  std::ostringstream oss;
  oss << "commandBufferCreateInfo:" << commandBufferCreateInfo;
  mCallStack.PushCall("CreateCommandBuffer", oss.str());
  return Graphics::MakeUnique<TestGraphicsCommandBuffer>(mCommandBufferCallStack, mGl);
}

Graphics::UniquePtr<Graphics::RenderPass> TestGraphicsController::CreateRenderPass(const Graphics::RenderPassCreateInfo& renderPassCreateInfo, Graphics::UniquePtr<Graphics::RenderPass>&& oldRenderPass)
{
  mCallStack.PushCall("CreateRenderPass", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Texture> TestGraphicsController::CreateTexture(const Graphics::TextureCreateInfo& textureCreateInfo, Graphics::UniquePtr<Graphics::Texture>&& oldTexture)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["textureCreateInfo"] << textureCreateInfo;
  mCallStack.PushCall("CreateTexture", namedParams.str(), namedParams);

  return Graphics::MakeUnique<TestGraphicsTexture>(mGl, textureCreateInfo);
}

Graphics::UniquePtr<Graphics::Framebuffer> TestGraphicsController::CreateFramebuffer(const Graphics::FramebufferCreateInfo& framebufferCreateInfo, Graphics::UniquePtr<Graphics::Framebuffer>&& oldFramebuffer)
{
  mCallStack.PushCall("CreateFramebuffer", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Pipeline> TestGraphicsController::CreatePipeline(const Graphics::PipelineCreateInfo& pipelineCreateInfo, Graphics::UniquePtr<Graphics::Pipeline>&& oldPipeline)
{
  mCallStack.PushCall("CreatePipeline", "");
  return std::make_unique<TestGraphicsPipeline>(mGl, pipelineCreateInfo);
}

Graphics::UniquePtr<Graphics::Program> TestGraphicsController::CreateProgram(const Graphics::ProgramCreateInfo& programCreateInfo, Graphics::UniquePtr<Graphics::Program>&& oldProgram)
{
  mCallStack.PushCall("CreateProgram", "");

  for(auto cacheEntry : mProgramCache)
  {
    bool found = true;
    for(auto& shader : *(programCreateInfo.shaderState))
    {
      auto                 graphicsShader = Uncast<TestGraphicsShader>(shader.shader);
      std::vector<uint8_t> source;
      source.resize(graphicsShader->mCreateInfo.sourceSize);
      memcpy(&source[0], graphicsShader->mCreateInfo.sourceData, graphicsShader->mCreateInfo.sourceSize);

      if(!std::equal(source.begin(), source.end(), cacheEntry.shaders[shader.pipelineStage].begin()))
      {
        found = false;
        break;
      }
    }
    if(found)
    {
      return Graphics::MakeUnique<TestGraphicsProgram>(cacheEntry.programImpl);
    }
  }

  mProgramCache.emplace_back();
  mProgramCache.back().programImpl = new TestGraphicsProgramImpl(mGl, programCreateInfo, mVertexFormats, mCustomUniforms);
  for(auto& shader : *(programCreateInfo.shaderState))
  {
    auto graphicsShader = Uncast<TestGraphicsShader>(shader.shader);
    mProgramCache.back().shaders[shader.pipelineStage].resize(graphicsShader->mCreateInfo.sourceSize);
    memcpy(&mProgramCache.back().shaders[shader.pipelineStage][0], graphicsShader->mCreateInfo.sourceData, graphicsShader->mCreateInfo.sourceSize);
  }
  return Graphics::MakeUnique<TestGraphicsProgram>(mProgramCache.back().programImpl);
}

Graphics::UniquePtr<Graphics::Shader> TestGraphicsController::CreateShader(const Graphics::ShaderCreateInfo& shaderCreateInfo, Graphics::UniquePtr<Graphics::Shader>&& oldShader)
{
  mCallStack.PushCall("CreateShader", "");
  return Graphics::MakeUnique<TestGraphicsShader>(mGl, shaderCreateInfo);
}

Graphics::UniquePtr<Graphics::Sampler> TestGraphicsController::CreateSampler(const Graphics::SamplerCreateInfo& samplerCreateInfo, Graphics::UniquePtr<Graphics::Sampler>&& oldSampler)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["samplerCreateInfo"] << samplerCreateInfo;
  mCallStack.PushCall("CreateSampler", namedParams.str(), namedParams);

  return Graphics::MakeUnique<TestGraphicsSampler>(mGl, samplerCreateInfo);
}

Graphics::UniquePtr<Graphics::RenderTarget> TestGraphicsController::CreateRenderTarget(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo, Graphics::UniquePtr<Graphics::RenderTarget>&& oldRenderTarget)
{
  mCallStack.PushCall("CreateRenderTarget", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Memory> TestGraphicsController::MapBufferRange(const Graphics::MapBufferInfo& mapInfo)
{
  mCallStack.PushCall("MapBufferRange", "");

  auto buffer = static_cast<TestGraphicsBuffer*>(mapInfo.buffer);
  buffer->memory.resize(mapInfo.offset + mapInfo.size); // For initial testing, allow writes past capacity

  return std::make_unique<TestGraphicsMemory>(mCallStack, *buffer, mapInfo.offset, mapInfo.size);
}

Graphics::UniquePtr<Graphics::Memory> TestGraphicsController::MapTextureRange(const Graphics::MapTextureInfo& mapInfo)
{
  mCallStack.PushCall("MapTextureRange", "");
  return nullptr;
}

void TestGraphicsController::UnmapMemory(Graphics::UniquePtr<Graphics::Memory> memory)
{
  mCallStack.PushCall("UnmapMemory", "");
}

Graphics::MemoryRequirements TestGraphicsController::GetTextureMemoryRequirements(Graphics::Texture& texture) const
{
  mCallStack.PushCall("GetTextureMemoryRequirements", "");
  return Graphics::MemoryRequirements{};
}

Graphics::MemoryRequirements TestGraphicsController::GetBufferMemoryRequirements(Graphics::Buffer& buffer) const
{
  mCallStack.PushCall("GetBufferMemoryRequirements", "");
  return Graphics::MemoryRequirements{};
}

const Graphics::TextureProperties& TestGraphicsController::GetTextureProperties(const Graphics::Texture& texture)
{
  static Graphics::TextureProperties textureProperties{};
  mCallStack.PushCall("GetTextureProperties", "");

  return textureProperties;
}

const Graphics::Reflection& TestGraphicsController::GetProgramReflection(const Graphics::Program& program)
{
  mCallStack.PushCall("GetProgramReflection", "");

  return static_cast<const TestGraphicsProgram*>(&program)->GetReflection();
}

bool TestGraphicsController::PipelineEquals(const Graphics::Pipeline& pipeline0, const Graphics::Pipeline& pipeline1) const
{
  mCallStack.PushCall("PipelineEquals", "");
  return false;
}

bool TestGraphicsController::GetProgramParameter(Graphics::Program& program, uint32_t parameterId, void* outData)
{
  mCallStack.PushCall("GetProgramParameter", "");
  auto graphicsProgram = Uncast<TestGraphicsProgram>(&program);
  return graphicsProgram->GetParameter(parameterId, outData);
}

} // namespace Dali
