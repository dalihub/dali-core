/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-types.h>
#include "test-graphics-buffer.h"
#include "test-graphics-command-buffer.h"
#include "test-graphics-framebuffer.h"
#include "test-graphics-reflection.h"
#include "test-graphics-render-pass.h"
#include "test-graphics-render-target.h"
#include "test-graphics-sampler.h"
#include "test-graphics-shader.h"
#include "test-graphics-sync-object.h"
#include "test-graphics-texture.h"

#include <dali/integration-api/gl-defines.h>
#include <any>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sstream>

namespace Dali
{
namespace
{
template<class T>
struct TestGraphicsDeleter
{
  TestGraphicsDeleter() = default;
  void operator()(T* object)
  {
    // Discard resource
    object->DiscardResource();
  }
};

} // namespace

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
    << " nativeImagePtr:" << std::hex << createInfo.nativeImagePtr.Get();
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

std::ostream& operator<<(std::ostream& o, const Graphics::ColorAttachment& colorAttachment)
{
  o << "attachmentId:" << colorAttachment.attachmentId
    << " layerId:" << colorAttachment.layerId
    << " levelId:" << colorAttachment.levelId
    << " texture:" << colorAttachment.texture;
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::DepthStencilAttachment& depthStencilAttachment)
{
  o << "depthTexture:" << depthStencilAttachment.depthTexture
    << "depthLevel:" << depthStencilAttachment.depthLevel
    << "stencilTexture:" << depthStencilAttachment.stencilTexture
    << "stencilLevel:" << depthStencilAttachment.stencilLevel;
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::FramebufferCreateInfo& createInfo)
{
  o << "colorAttachments:";
  for(auto i = 0u; i < createInfo.colorAttachments.size(); ++i)
  {
    o << "[" << i << "]=" << createInfo.colorAttachments[i] << "  ";
  }
  o << "depthStencilAttachment:" << createInfo.depthStencilAttachment;
  o << "size: " << createInfo.size;
  return o;
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
    case Graphics::BlendOp::MULTIPLY:
    {
      op = GL_MULTIPLY;
      break;
    }
    case Graphics::BlendOp::SCREEN:
    {
      op = GL_SCREEN;
      break;
    }
    case Graphics::BlendOp::OVERLAY:
    {
      op = GL_OVERLAY;
      break;
    }
    case Graphics::BlendOp::DARKEN:
    {
      op = GL_DARKEN;
      break;
    }
    case Graphics::BlendOp::LIGHTEN:
    {
      op = GL_LIGHTEN;
      break;
    }
    case Graphics::BlendOp::COLOR_DODGE:
    {
      op = GL_COLORDODGE;
      break;
    }
    case Graphics::BlendOp::COLOR_BURN:
    {
      op = GL_COLORBURN;
      break;
    }
    case Graphics::BlendOp::HARD_LIGHT:
    {
      op = GL_HARDLIGHT;
      break;
    }
    case Graphics::BlendOp::SOFT_LIGHT:
    {
      op = GL_SOFTLIGHT;
      break;
    }
    case Graphics::BlendOp::DIFFERENCE:
    {
      op = GL_DIFFERENCE;
      break;
    }
    case Graphics::BlendOp::EXCLUSION:
    {
      op = GL_EXCLUSION;
      break;
    }
    case Graphics::BlendOp::HUE:
    {
      op = GL_HSL_HUE;
      break;
    }
    case Graphics::BlendOp::SATURATION:
    {
      op = GL_HSL_SATURATION;
      break;
    }
    case Graphics::BlendOp::COLOR:
    {
      op = GL_HSL_COLOR;
      break;
    }
    case Graphics::BlendOp::LUMINOSITY:
    {
      op = GL_HSL_LUMINOSITY;
      break;
    }
  }
  return op;
}

struct GLCompareOp
{
  constexpr explicit GLCompareOp(Graphics::CompareOp compareOp)
  {
    switch(compareOp)
    {
      case Graphics::CompareOp::NEVER:
        op = GL_NEVER;
        break;
      case Graphics::CompareOp::LESS:
        op = GL_LESS;
        break;
      case Graphics::CompareOp::EQUAL:
        op = GL_EQUAL;
        break;
      case Graphics::CompareOp::LESS_OR_EQUAL:
        op = GL_LEQUAL;
        break;
      case Graphics::CompareOp::GREATER:
        op = GL_GREATER;
        break;
      case Graphics::CompareOp::NOT_EQUAL:
        op = GL_NOTEQUAL;
        break;
      case Graphics::CompareOp::GREATER_OR_EQUAL:
        op = GL_GEQUAL;
        break;
      case Graphics::CompareOp::ALWAYS:
        op = GL_ALWAYS;
        break;
    }
  }
  GLenum op{GL_LESS};
};

struct GLStencilOp
{
  constexpr explicit GLStencilOp(Graphics::StencilOp stencilOp)
  {
    switch(stencilOp)
    {
      case Graphics::StencilOp::KEEP:
        op = GL_KEEP;
        break;
      case Graphics::StencilOp::ZERO:
        op = GL_ZERO;
        break;
      case Graphics::StencilOp::REPLACE:
        op = GL_REPLACE;
        break;
      case Graphics::StencilOp::INCREMENT_AND_CLAMP:
        op = GL_INCR;
        break;
      case Graphics::StencilOp::DECREMENT_AND_CLAMP:
        op = GL_DECR;
        break;
      case Graphics::StencilOp::INVERT:
        op = GL_INVERT;
        break;
      case Graphics::StencilOp::INCREMENT_AND_WRAP:
        op = GL_INCR_WRAP;
        break;
      case Graphics::StencilOp::DECREMENT_AND_WRAP:
        op = GL_DECR_WRAP;
        break;
    }
  }
  GLenum op{GL_KEEP};
};

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
: mCallStack(false, "TestGraphicsController."),
  mCommandBufferCallStack(false, "TestCommandBuffer."),
  mFrameBufferCallStack(false, "TestFrameBuffer.")
{
  mCallStack.Enable(true);
  mCommandBufferCallStack.Enable(true);
  auto& trace = mGl.GetTextureTrace();
  trace.Enable(true);
  trace.EnableLogging(false);
}

void TestGraphicsController::SetResourceBindingHints(const std::vector<Graphics::SceneResourceBinding>& resourceBindings)
{
  mCallStack.PushCall("SetResourceBindingHints", "");
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
    ProcessCommandBuffer(*commandBuffer);
  }
}

void TestGraphicsController::ProcessCommandBuffer(TestGraphicsCommandBuffer& commandBuffer)
{
  bool                     scissorEnabled = false;
  TestGraphicsFramebuffer* currentFramebuffer{nullptr};
  TestGraphicsPipeline*    currentPipeline{nullptr};
  bool                     recording = false;
  bool                     recorded  = false;

  for(auto& cmd : commandBuffer.GetCommands())
  {
    // process command
    switch(cmd.type)
    {
      case CommandType::BEGIN:
      {
        if(recording)
        {
          fprintf(stderr, "ERROR: Should only call Begin once per cmd buffer\n");
        }
        recording = true;
        break;
      }
      case CommandType::END:
      {
        if(!recording)
        {
          fprintf(stderr, "ERROR: Should only call End following a Begin");
        }
        recorded  = true;
        recording = false;
        break;
      }
      case CommandType::FLUSH:
      {
        if(recording)
        {
        }
        // Nothing to do here
        break;
      }
      case CommandType::BIND_TEXTURES:
      {
        for(auto& binding : cmd.data.bindTextures.textureBindings)
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
        break;
      }
      case CommandType::BIND_VERTEX_BUFFERS:
      {
        for(auto& binding : cmd.data.bindVertexBuffers.vertexBufferBindings)
        {
          auto graphicsBuffer = binding.buffer;
          auto vertexBuffer   = Uncast<TestGraphicsBuffer>(graphicsBuffer);
          vertexBuffer->Bind();
        }
        break;
      }
      case CommandType::BIND_INDEX_BUFFER:
      {
        auto& indexBufferBinding = cmd.data.bindIndexBuffer;
        if(indexBufferBinding.buffer)
        {
          auto buffer = Uncast<TestGraphicsBuffer>(indexBufferBinding.buffer);
          buffer->Bind();
        }
        break;
      }
      case CommandType::BIND_UNIFORM_BUFFER:
      {
        if(currentPipeline)
        {
          auto& bindings = cmd.data.bindUniformBuffers;

          auto buffer = bindings.standaloneUniformsBufferBinding;
          // based on reflection, issue gl calls
          buffer.buffer->BindAsUniformBuffer(static_cast<const TestGraphicsProgram*>(currentPipeline->programState.program), bindings.standaloneUniformsBufferBinding);

          // Store off uniform bindings for test retrieval:
          if(!bindings.uniformBufferBindings.empty())
          {
            mLastUniformBinding.buffer   = bindings.uniformBufferBindings.back().buffer;
            mLastUniformBinding.binding  = bindings.uniformBufferBindings.back().binding;
            mLastUniformBinding.offset   = bindings.uniformBufferBindings.back().offset;
            mLastUniformBinding.emulated = bindings.uniformBufferBindings.back().emulated;
          }
          else
          {
            mLastUniformBinding.buffer   = bindings.standaloneUniformsBufferBinding.buffer;
            mLastUniformBinding.binding  = bindings.standaloneUniformsBufferBinding.binding;
            mLastUniformBinding.offset   = bindings.standaloneUniformsBufferBinding.offset;
            mLastUniformBinding.emulated = true;
          }
        }
        break;
      }
      case CommandType::BIND_SAMPLERS:
      {
        break;
      }
      case CommandType::BIND_PIPELINE:
      {
        currentPipeline = Uncast<TestGraphicsPipeline>(cmd.data.bindPipeline.pipeline);
        BindPipeline(currentPipeline);
        break;
      }
      case CommandType::DRAW_NATIVE:
      {
        auto info = &cmd.data.draw.drawNative.drawNativeInfo;

        if(info->glesNativeInfo.eglSharedContextStoragePointer)
        {
          auto* anyContext = reinterpret_cast<std::any*>(info->glesNativeInfo.eglSharedContextStoragePointer);
          *anyContext      = reinterpret_cast<void*>(0x12345678u);
        }

        CallbackBase::ExecuteReturn<bool>(*info->callback, info->userData);
        break;
      }
      case CommandType::DRAW:
      {
        if(currentPipeline)
        {
          if(cmd.data.draw.draw.instanceCount == 0)
          {
            mGl.DrawArrays(GetTopology(currentPipeline->inputAssemblyState.topology),
                           cmd.data.draw.draw.firstVertex,
                           cmd.data.draw.draw.vertexCount);
          }
          else
          {
            mGl.DrawArraysInstanced(GetTopology(currentPipeline->inputAssemblyState.topology),
                                    cmd.data.draw.draw.firstVertex,
                                    cmd.data.draw.draw.vertexCount,
                                    cmd.data.draw.draw.instanceCount);
          }
        }
        break;
      }
      case CommandType::DRAW_INDEXED:
      {
        if(currentPipeline)
        {
          if(cmd.data.draw.draw.instanceCount == 0)
          {
            mGl.DrawElements(GetTopology(currentPipeline->inputAssemblyState.topology),
                             static_cast<GLsizei>(cmd.data.draw.drawIndexed.indexCount),
                             GL_UNSIGNED_SHORT,
                             reinterpret_cast<void*>(cmd.data.draw.drawIndexed.firstIndex));
          }
          else
          {
            mGl.DrawElementsInstanced(GetTopology(currentPipeline->inputAssemblyState.topology),
                                      static_cast<GLsizei>(cmd.data.draw.drawIndexed.indexCount),
                                      GL_UNSIGNED_SHORT,
                                      reinterpret_cast<void*>(cmd.data.draw.drawIndexed.firstIndex),
                                      cmd.data.draw.drawIndexed.instanceCount);
          }
        }
        break;
      }
      case CommandType::DRAW_INDEXED_INDIRECT:
      {
        if(currentPipeline)
        {
          mGl.DrawElements(GetTopology(currentPipeline->inputAssemblyState.topology),
                           static_cast<GLsizei>(cmd.data.draw.drawIndexed.indexCount),
                           GL_UNSIGNED_SHORT,
                           reinterpret_cast<void*>(cmd.data.draw.drawIndexed.firstIndex));
        }
        break;
      }
      case CommandType::SET_SCISSOR:
      {
        if(scissorEnabled)
        {
          auto& rect = cmd.data.scissor.region;
          mGl.Scissor(rect.x, rect.y, rect.width, rect.height);
        }
        break;
      }
      case CommandType::SET_SCISSOR_TEST:
      {
        if(cmd.data.scissorTest.enable)
        {
          mGl.Enable(GL_SCISSOR_TEST);
          scissorEnabled = true;
        }
        else
        {
          mGl.Disable(GL_SCISSOR_TEST);
          scissorEnabled = false;
        }
        break;
      }
      case CommandType::SET_VIEWPORT_TEST:
      {
        break;
      }
      case CommandType::SET_VIEWPORT: // @todo Consider correcting for orientation here?
      {
        auto& rect = cmd.data.viewport.region;
        mGl.Viewport(rect.x, rect.y, rect.width, rect.height);
        break;
      }

      case CommandType::SET_COLOR_MASK:
      {
        // Set all channels to the same mask
        const bool mask = cmd.data.colorMask.enabled;
        mGl.ColorMask(mask, mask, mask, mask);
        break;
      }
      case CommandType::CLEAR_STENCIL_BUFFER:
      {
        mGl.Clear(GL_STENCIL_BUFFER_BIT);
        break;
      }
      case CommandType::CLEAR_DEPTH_BUFFER:
      {
        mGl.Clear(GL_DEPTH_BUFFER_BIT);
        break;
      }

      case CommandType::SET_STENCIL_TEST_ENABLE:
      {
        if(cmd.data.stencilTest.enabled)
        {
          mGl.Enable(GL_STENCIL_TEST);
        }
        else
        {
          mGl.Disable(GL_STENCIL_TEST);
        }
        break;
      }

      case CommandType::SET_STENCIL_WRITE_MASK:
      {
        mGl.StencilMask(cmd.data.stencilWriteMask.mask);
        break;
      }
      case CommandType::SET_STENCIL_STATE:
      {
        mGl.StencilFunc(GLCompareOp(cmd.data.stencilState.compareOp).op,
                        cmd.data.stencilState.reference,
                        cmd.data.stencilState.compareMask);
        mGl.StencilOp(GLStencilOp(cmd.data.stencilState.failOp).op,
                      GLStencilOp(cmd.data.stencilState.depthFailOp).op,
                      GLStencilOp(cmd.data.stencilState.passOp).op);
        break;
      }

      case CommandType::SET_DEPTH_COMPARE_OP:
      {
        mGl.DepthFunc(GLCompareOp(cmd.data.depth.compareOp).op);
        break;
      }
      case CommandType::SET_DEPTH_TEST_ENABLE:
      {
        if(cmd.data.depth.testEnabled)
        {
          mGl.Enable(GL_DEPTH_TEST);
        }
        else
        {
          mGl.Disable(GL_DEPTH_TEST);
        }
        break;
      }
      case CommandType::SET_DEPTH_WRITE_ENABLE:
      {
        mGl.DepthMask(cmd.data.depth.writeEnabled);
        break;
      }

      case CommandType::EXECUTE_COMMAND_BUFFERS:
      {
        // Process secondary command buffers
        for(auto& buf : cmd.data.executeCommandBuffers.buffers)
        {
          ProcessCommandBuffer(*Uncast<TestGraphicsCommandBuffer>(buf));
        }
        break;
      }
      case CommandType::BEGIN_RENDER_PASS:
      {
        auto renderTarget = Uncast<TestGraphicsRenderTarget>(cmd.data.beginRenderPass.renderTarget);

        if(renderTarget)
        {
          auto fb = renderTarget->mCreateInfo.framebuffer;
          if(fb)
          {
            if(currentFramebuffer != fb)
            {
              currentFramebuffer = Uncast<TestGraphicsFramebuffer>(fb);
              currentFramebuffer->Bind();
            }
          }
          else
          {
            mGl.BindFramebuffer(GL_FRAMEBUFFER, 0);
          }
        }
        else
        {
          mGl.BindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        auto& clearValues = cmd.data.beginRenderPass.clearValues;
        if(clearValues.size() > 0)
        {
          const auto renderPass = static_cast<TestGraphicsRenderPass*>(cmd.data.beginRenderPass.renderPass);
          if(renderPass)
          {
            const auto& color0 = renderPass->attachments[0];
            GLuint      mask   = 0;
            if(color0.loadOp == Graphics::AttachmentLoadOp::CLEAR)
            {
              mask |= GL_COLOR_BUFFER_BIT;

              // Set clear color (todo: cache it!)
              // Something goes wrong here if Alpha mask is GL_TRUE
              mGl.ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
              mGl.ClearColor(clearValues[0].color.r,
                             clearValues[0].color.g,
                             clearValues[0].color.b,
                             clearValues[0].color.a);
            }

            // check for depth stencil
            if(renderPass->attachments.size() > 1)
            {
              const auto& depthStencil = renderPass->attachments.back();
              if(depthStencil.loadOp == Graphics::AttachmentLoadOp::CLEAR)
              {
                mGl.DepthMask(true);
                uint32_t depthClearColor = 0u;
                if(clearValues.size() == renderPass->attachments.size())
                {
                  depthClearColor = clearValues.back().depthStencil.depth;
                }
                mGl.ClearDepthf(depthClearColor);
                mask |= GL_DEPTH_BUFFER_BIT;
              }
              if(depthStencil.stencilLoadOp == Graphics::AttachmentLoadOp::CLEAR)
              {
                uint32_t stencilClearColor = 0u;
                if(clearValues.size() == renderPass->attachments.size())
                {
                  stencilClearColor = clearValues.back().depthStencil.stencil;
                }
                mGl.ClearStencil(stencilClearColor);
                mGl.StencilMask(0xFF); // Clear all the bitplanes (assume 8)
                mask |= GL_STENCIL_BUFFER_BIT;
              }
            }

            if(mask != 0)
            {
              // Test scissor area and RT size
              const auto& area = cmd.data.beginRenderPass.renderArea;
              if(area.x == 0 &&
                 area.y == 0 &&
                 renderTarget &&
                 area.width == renderTarget->mCreateInfo.extent.width &&
                 area.height == renderTarget->mCreateInfo.extent.height)
              {
                mGl.Disable(GL_SCISSOR_TEST);
                mGl.Clear(mask);
              }
              else
              {
                mGl.Enable(GL_SCISSOR_TEST);
                mGl.Scissor(cmd.data.beginRenderPass.renderArea.x, cmd.data.beginRenderPass.renderArea.y, cmd.data.beginRenderPass.renderArea.width, cmd.data.beginRenderPass.renderArea.height);
                mGl.Clear(mask);
                mGl.Disable(GL_SCISSOR_TEST);
              }
            }
          }
          else
          {
            DALI_ASSERT_DEBUG(0 && "BeginRenderPass has no render pass");
          }
        }
        break;
      }
      case CommandType::END_RENDER_PASS:
      {
        if(cmd.data.endRenderPass.syncObject != nullptr)
        {
          auto syncObject = Uncast<TestGraphicsSyncObject>(cmd.data.endRenderPass.syncObject);
          syncObject->InitializeResource(); // create the sync object.
        }
        break;
      }
    }
  }

  if(!recorded)
  {
    fprintf(stderr, "ERROR: No command buffer was recorded");
  }
}

void TestGraphicsController::BindPipeline(TestGraphicsPipeline* pipeline)
{
  auto& vi = pipeline->vertexInputState;
  for(auto& attribute : vi.attributes)
  {
    mGl.EnableVertexAttribArray(attribute.location);
    uint32_t attributeOffset = attribute.offset;
    GLsizei  stride          = vi.bufferBindings[attribute.binding].stride;

    auto rate = vi.bufferBindings[attribute.binding].inputRate;

    mGl.VertexAttribPointer(attribute.location,
                            GetNumComponents(attribute.format),
                            GetGlType(attribute.format),
                            GL_FALSE, // Not normalized
                            stride,
                            reinterpret_cast<void*>(attributeOffset));
    if(rate == Graphics::VertexInputRate::PER_VERTEX)
    {
      mGl.VertexAttribDivisor(attribute.location, 0);
    }
    else if(rate == Graphics::VertexInputRate::PER_INSTANCE)
    {
      mGl.VertexAttribDivisor(attribute.location, 1);
    }
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

  auto* program = static_cast<const TestGraphicsProgram*>(pipeline->programState.program);
  mGl.UseProgram(program->mImpl->mId);
}

/**
 * @brief Presents render target
 * @param renderTarget render target to present
 */
void TestGraphicsController::PresentRenderTarget(Graphics::RenderTarget* renderTarget)
{
  auto*                       rt = static_cast<const TestGraphicsRenderTarget*>(renderTarget);
  TraceCallStack::NamedParams namedParams;
  namedParams["renderTarget"] << std::hex << renderTarget;
  namedParams["surface"] << std::hex << rt->mCreateInfo.surface;
  mCallStack.PushCall("PresentRenderTarget", namedParams.str(), namedParams);
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

void TestGraphicsController::Shutdown()
{
  mCallStack.PushCall("Shutdown", "");
}

void TestGraphicsController::Destroy()
{
  mCallStack.PushCall("Destroy", "");
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

void TestGraphicsController::GenerateTextureMipmaps(const Graphics::Texture& texture)
{
  mCallStack.PushCall("GenerateTextureMipmaps", "");

  auto gfxTexture = Uncast<TestGraphicsTexture>(&texture);
  mGl.BindTexture(gfxTexture->GetTarget(), 0);
  mGl.GenerateMipmap(gfxTexture->GetTarget());
}

bool TestGraphicsController::EnableDepthStencilBuffer(const Graphics::RenderTarget& renderTarget,
                                                      bool                          enableDepth,
                                                      bool                          enableStencil)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["enableDepth"] << (enableDepth ? "T" : "F");
  namedParams["enableStencil"] << (enableStencil ? "T" : "F");
  mCallStack.PushCall("EnableDepthStencilBuffer", namedParams.str(), namedParams);
  return true;
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
  TraceCallStack::NamedParams namedParams;
  namedParams["usage"] << "0x" << std::hex << createInfo.usage;
  namedParams["propertiesFlags"] << createInfo.propertiesFlags;
  namedParams["size"] << createInfo.size;
  mCallStack.PushCall("CreateBuffer", namedParams.str(), namedParams);

  auto ptr = Graphics::MakeUnique<TestGraphicsBuffer, TestGraphicsDeleter<TestGraphicsBuffer>>(createInfo, *this, mGl, mCallStack);
  mAllocatedBuffers.push_back(ptr.get());
  return ptr;
}

void TestGraphicsController::DiscardBuffer(TestGraphicsBuffer* buffer)
{
  auto iter = std::find(mAllocatedBuffers.begin(), mAllocatedBuffers.end(), buffer);
  if(iter != mAllocatedBuffers.end())
  {
    mAllocatedBuffers.erase(iter);
  }
  delete buffer;
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
  return Graphics::MakeUnique<TestGraphicsRenderPass>(mGl, renderPassCreateInfo);
}

Graphics::UniquePtr<Graphics::Texture> TestGraphicsController::CreateTexture(const Graphics::TextureCreateInfo& textureCreateInfo, Graphics::UniquePtr<Graphics::Texture>&& oldTexture)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["textureCreateInfo"] << textureCreateInfo;
  mCallStack.PushCall("CreateTexture", namedParams.str(), namedParams);

  return Graphics::MakeUnique<TestGraphicsTexture>(mGl, textureCreateInfo);
}

Graphics::UniquePtr<Graphics::Framebuffer> TestGraphicsController::CreateFramebuffer(
  const Graphics::FramebufferCreateInfo&       createInfo,
  Graphics::UniquePtr<Graphics::Framebuffer>&& oldFramebuffer)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["framebufferCreateInfo"] << createInfo;
  mCallStack.PushCall("Controller::CreateFramebuffer", namedParams.str(), namedParams);

  return Graphics::MakeUnique<TestGraphicsFramebuffer>(mFrameBufferCallStack, mGl, createInfo);
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

      if(!std::equal(source.begin(), source.end(), cacheEntry.shaders[shader.pipelineStage].begin(), cacheEntry.shaders[shader.pipelineStage].end()))
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
  mProgramCache.back().programImpl = new TestGraphicsProgramImpl(*this, mGl, programCreateInfo, mVertexFormats, mCustomUniforms, mCustomUniformBlocks);

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
  TraceCallStack::NamedParams namedParams;
  namedParams["surface"] << std::hex << renderTargetCreateInfo.surface;
  mCallStack.PushCall("CreateRenderTarget", namedParams.str(), namedParams);

  return Graphics::MakeUnique<TestGraphicsRenderTarget>(mGl, renderTargetCreateInfo);
}

Graphics::UniquePtr<Graphics::SyncObject> TestGraphicsController::CreateSyncObject(
  const Graphics::SyncObjectCreateInfo&       syncObjectCreateInfo,
  Graphics::UniquePtr<Graphics::SyncObject>&& oldSyncObject)
{
  mCallStack.PushCall("CreateSyncObject", "");
  return Graphics::MakeUnique<TestGraphicsSyncObject>(mGraphicsSyncImpl, syncObjectCreateInfo);
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
  static GLint uniformAlign{0};

  Graphics::MemoryRequirements reqs{};
  mCallStack.PushCall("GetBufferMemoryRequirements", "");

  auto gfxBuffer = Uncast<TestGraphicsBuffer>(&buffer);
  if(gfxBuffer->mCreateInfo.usage & (0 | Graphics::BufferUsage::UNIFORM_BUFFER))
  {
    if(!uniformAlign)
    {
      // Throw off the shackles of constness
      auto& gl = *const_cast<TestGlAbstraction*>(&mGl);
      gl.GetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformAlign);
    }
    reqs.size      = gfxBuffer->mCreateInfo.size;
    reqs.alignment = uint32_t(uniformAlign);
  }
  return reqs;
}

Graphics::TextureProperties TestGraphicsController::GetTextureProperties(const Graphics::Texture& texture)
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

Graphics::Texture* TestGraphicsController::CreateTextureByResourceId(uint32_t resourceId, const Graphics::TextureCreateInfo& createInfo)
{
  Graphics::Texture*                     ret = nullptr;
  Graphics::UniquePtr<Graphics::Texture> texture;
  TraceCallStack::NamedParams            namedParams;
  namedParams["resourceId"] << resourceId;

  auto iter = mTextureUploadBindMapper.find(resourceId);
  DALI_ASSERT_ALWAYS(iter == mTextureUploadBindMapper.end());

  // Create new graphics texture.
  texture = CreateTexture(createInfo, std::move(texture));
  ret     = texture.get();

  mTextureUploadBindMapper.insert(std::make_pair(resourceId, std::move(texture)));

  mCallStack.PushCall("CreateTextureByResourceId", "", namedParams);
  return ret;
}

void TestGraphicsController::DiscardTextureFromResourceId(uint32_t resourceId)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["resourceId"] << resourceId;

  mTextureUploadBindMapper.erase(resourceId);

  mCallStack.PushCall("DiscardTextureFromResourceId", "", namedParams);
}

Graphics::Texture* TestGraphicsController::GetTextureFromResourceId(uint32_t resourceId)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["resourceId"] << resourceId;

  Graphics::Texture* ret = nullptr;

  auto iter = mTextureUploadBindMapper.find(resourceId);
  if(iter != mTextureUploadBindMapper.end())
  {
    ret = iter->second.get();
  }

  mCallStack.PushCall("GetTextureFromResourceId", "", namedParams);

  return ret;
}

Graphics::UniquePtr<Graphics::Texture> TestGraphicsController::ReleaseTextureFromResourceId(uint32_t resourceId)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["resourceId"] << resourceId;

  Graphics::UniquePtr<Graphics::Texture> texture;

  auto iter = mTextureUploadBindMapper.find(resourceId);
  if(iter != mTextureUploadBindMapper.end())
  {
    texture = std::move(iter->second);
    mTextureUploadBindMapper.erase(iter);
  }

  mCallStack.PushCall("ReleaseTextureFromResourceId", "", namedParams);

  return texture;
}

bool TestGraphicsController::HasClipMatrix() const
{
  return true;
}

const Matrix& TestGraphicsController::GetClipMatrix() const
{
  // This matrix transforms from GL -> Vulkan clip space
  constexpr float VULKAN_CLIP_MATRIX_DATA[] = {
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f};
  static const Matrix VULKAN_CLIP_MATRIX(VULKAN_CLIP_MATRIX_DATA);
  static const Matrix IDENTITY = Matrix::IDENTITY;

  // For now, return IDENTITY to stay in GL clip space.
  // @todo Add test toggle
  return IDENTITY;
}

uint32_t TestGraphicsController::GetDeviceLimitation(Graphics::DeviceCapability capability)
{
  if(capability == Graphics::DeviceCapability::MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT)
  {
    auto& gl           = *const_cast<TestGlAbstraction*>(&mGl);
    GLint uniformAlign = 0;
    gl.GetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformAlign);
    return uniformAlign; // Default for testing
  }
  return 0;
}

} // namespace Dali
