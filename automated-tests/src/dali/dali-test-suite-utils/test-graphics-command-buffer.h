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
#include <dali/graphics-api/graphics-pipeline.h>
#include <dali/graphics-api/graphics-types.h>
#include <cstdint>
#include <vector>
#include "test-gl-abstraction.h"
#include "test-graphics-buffer.h"
#include "test-graphics-pipeline.h"
#include "test-trace-call-stack.h"

namespace Dali
{
class TestGraphicsTexture;
class TestGraphicsBuffer;
class TestGraphicsSampler;
class TestGraphicsPipeline;

enum class CommandType
{
  FLUSH                 = 1 << 0,
  BIND_TEXTURES         = 1 << 1,
  BIND_SAMPLERS         = 1 << 2,
  BIND_VERTEX_BUFFERS   = 1 << 3,
  BIND_INDEX_BUFFER     = 1 << 4,
  BIND_UNIFORM_BUFFER   = 1 << 5,
  BIND_PIPELINE         = 1 << 6,
  DRAW                  = 1 << 7,
  DRAW_INDEXED          = 1 << 8,
  DRAW_INDEXED_INDIRECT = 1 << 9,
  SET_SCISSOR           = 1 << 10,
  SET_SCISSOR_TEST      = 1 << 11,
  SET_VIEWPORT          = 1 << 12,
  SET_VIEWPORT_TEST     = 1 << 13
};

using CommandTypeMask = uint32_t;
template<typename T>
inline CommandTypeMask operator|(T flags, CommandType bit)
{
  return static_cast<CommandTypeMask>(flags) | static_cast<CommandTypeMask>(bit);
}

/**
 * @brief Descriptor of single buffer binding within
 * command buffer.
 */
struct VertexBufferBindingDescriptor
{
  const TestGraphicsBuffer* buffer{nullptr};
  uint32_t                  offset{0u};
};

/**
 * @brief Descriptor of ix buffer binding within
 * command buffer.
 */
struct IndexBufferBindingDescriptor
{
  const TestGraphicsBuffer* buffer{nullptr};
  uint32_t                  offset{};
  Graphics::Format          format{};
};

/**
 * @brief Descriptor of uniform buffer binding within
 * command buffer.
 */
struct UniformBufferBindingDescriptor
{
  const TestGraphicsBuffer* buffer{nullptr};
  uint32_t                  binding{0u};
  uint32_t                  offset{0u};
  bool                      emulated; ///<true if UBO is emulated for old gfx API
};

/**
 * @brief The descriptor of draw call
 */
struct DrawCallDescriptor
{
  /**
   * @brief Enum specifying type of the draw call
   */
  enum class Type
  {
    DRAW,
    DRAW_INDEXED,
    DRAW_INDEXED_INDIRECT
  };

  Type type{}; ///< Type of the draw call

  /**
   * Union contains data for all types of draw calls.
   */
  union
  {
    /**
     * @brief Vertex array draw
     */
    struct
    {
      uint32_t vertexCount;
      uint32_t instanceCount;
      uint32_t firstVertex;
      uint32_t firstInstance;
    } draw;

    /**
     * @brief Indexed draw
     */
    struct
    {
      uint32_t indexCount;
      uint32_t instanceCount;
      uint32_t firstIndex;
      int32_t  vertexOffset;
      uint32_t firstInstance;
    } drawIndexed;

    /**
     * @brief Indexed draw indirect
     */
    struct
    {
      const TestGraphicsBuffer* buffer;
      uint32_t                  offset;
      uint32_t                  drawCount;
      uint32_t                  stride;
    } drawIndexedIndirect;
  };
};

/**
 * Command structure allocates memory to store a single command
 */
struct Command
{
  Command()
  {
  }

  ~Command()
  {
  }

  /**
   * @brief Copy constructor
   * @param[in] rhs Command
   */
  Command(const Command& rhs)
  {
    switch(rhs.type)
    {
      case CommandType::BIND_VERTEX_BUFFERS:
      {
        data.bindVertexBuffers = rhs.data.bindVertexBuffers;
        break;
      }
      case CommandType::BIND_INDEX_BUFFER:
      {
        data.bindIndexBuffer = rhs.data.bindIndexBuffer;
        break;
      }
      case CommandType::BIND_SAMPLERS:
      {
        data.bindSamplers = rhs.data.bindSamplers;
        break;
      }
      case CommandType::BIND_TEXTURES:
      {
        data.bindTextures = rhs.data.bindTextures;
        break;
      }
      case CommandType::BIND_PIPELINE:
      {
        data.bindPipeline = rhs.data.bindPipeline;
        break;
      }
      case CommandType::BIND_UNIFORM_BUFFER:
      {
        data.bindUniformBuffers = rhs.data.bindUniformBuffers;
        break;
      }
      case CommandType::DRAW:
      {
        data.draw.type = rhs.data.draw.type;
        data.draw.draw = rhs.data.draw.draw;
        break;
      }
      case CommandType::DRAW_INDEXED:
      {
        data.draw.type        = rhs.data.draw.type;
        data.draw.drawIndexed = rhs.data.draw.drawIndexed;
        break;
      }
      case CommandType::DRAW_INDEXED_INDIRECT:
      {
        data.draw.type                = rhs.data.draw.type;
        data.draw.drawIndexedIndirect = rhs.data.draw.drawIndexedIndirect;
        break;
      }
      case CommandType::FLUSH:
      {
        // Nothing to do
        break;
      }
      case CommandType::SET_SCISSOR:
      {
        data.scissor.region = rhs.data.scissor.region;
        break;
      }
      case CommandType::SET_SCISSOR_TEST:
      {
        data.scissorTest.enable = rhs.data.scissorTest.enable;
        break;
      }
      case CommandType::SET_VIEWPORT:
      {
        data.viewport.region = rhs.data.viewport.region;
        break;
      }
      case CommandType::SET_VIEWPORT_TEST:
      {
        data.viewportTest.enable = rhs.data.viewportTest.enable;
        break;
      }
    }
    type = rhs.type;
  }

  /**
   * @brief move constructor
   * @param[in] rhs Command
   */
  Command(Command&& rhs) noexcept
  {
    switch(rhs.type)
    {
      case CommandType::BIND_VERTEX_BUFFERS:
      {
        data.bindVertexBuffers = std::move(rhs.data.bindVertexBuffers);
        break;
      }
      case CommandType::BIND_INDEX_BUFFER:
      {
        data.bindIndexBuffer = rhs.data.bindIndexBuffer;
        break;
      }
      case CommandType::BIND_UNIFORM_BUFFER:
      {
        data.bindUniformBuffers = std::move(rhs.data.bindUniformBuffers);
        break;
      }
      case CommandType::BIND_SAMPLERS:
      {
        data.bindSamplers = std::move(rhs.data.bindSamplers);
        break;
      }
      case CommandType::BIND_TEXTURES:
      {
        data.bindTextures = std::move(rhs.data.bindTextures);
        break;
      }
      case CommandType::BIND_PIPELINE:
      {
        data.bindPipeline = rhs.data.bindPipeline;
        break;
      }
      case CommandType::DRAW:
      {
        data.draw.type = rhs.data.draw.type;
        data.draw.draw = rhs.data.draw.draw;
        break;
      }
      case CommandType::DRAW_INDEXED:
      {
        data.draw.type        = rhs.data.draw.type;
        data.draw.drawIndexed = rhs.data.draw.drawIndexed;
        break;
      }
      case CommandType::DRAW_INDEXED_INDIRECT:
      {
        data.draw.type                = rhs.data.draw.type;
        data.draw.drawIndexedIndirect = rhs.data.draw.drawIndexedIndirect;
        break;
      }
      case CommandType::FLUSH:
      {
        // Nothing to do
        break;
      }
      case CommandType::SET_SCISSOR:
      {
        data.scissor.region = rhs.data.scissor.region;
        break;
      }
      case CommandType::SET_SCISSOR_TEST:
      {
        data.scissorTest.enable = rhs.data.scissorTest.enable;
        break;
      }
      case CommandType::SET_VIEWPORT:
      {
        data.viewport.region = rhs.data.viewport.region;
        break;
      }
      case CommandType::SET_VIEWPORT_TEST:
      {
        data.viewportTest.enable = rhs.data.viewportTest.enable;
        break;
      }
    }
    type = rhs.type;
  }

  CommandType type{CommandType::FLUSH}; ///< Type of command

  union CommandData
  {
    CommandData()
    {
    }

    ~CommandData()
    {
    } // do nothing

    struct
    {
      std::vector<Graphics::TextureBinding> textureBindings;
    } bindTextures{};

    // BindSampler command
    struct
    {
      std::vector<Graphics::SamplerBinding> samplerBindings;
    } bindSamplers;

    struct
    {
      using Binding = VertexBufferBindingDescriptor;
      std::vector<Binding> vertexBufferBindings;
    } bindVertexBuffers;

    struct : public IndexBufferBindingDescriptor
    {
    } bindIndexBuffer;

    struct
    {
      std::vector<UniformBufferBindingDescriptor> uniformBufferBindings{};
      UniformBufferBindingDescriptor              standaloneUniformsBufferBinding{};
    } bindUniformBuffers;

    struct
    {
      const TestGraphicsPipeline* pipeline{nullptr};
    } bindPipeline;

    struct : public DrawCallDescriptor
    {
    } draw;

    struct
    {
      Graphics::Rect2D region;
    } scissor;
    struct
    {
      bool enable;
    } scissorTest;
    struct
    {
      Graphics::Viewport region;
    } viewport;
    struct
    {
      bool enable;
    } viewportTest;
  } data;
};

class TestGraphicsCommandBuffer : public Graphics::CommandBuffer
{
public:
  TestGraphicsCommandBuffer(TraceCallStack& callstack, TestGlAbstraction& glAbstraction);
  ~TestGraphicsCommandBuffer()
  {
  }

  void BindVertexBuffers(uint32_t                             firstBinding,
                         std::vector<const Graphics::Buffer*> buffers,
                         std::vector<uint32_t>                offsets) override
  {
    mCommands.emplace_back();
    mCommands.back().type = CommandType::BIND_VERTEX_BUFFERS;
    auto& bindings        = mCommands.back().data.bindVertexBuffers.vertexBufferBindings;
    if(bindings.size() < firstBinding + buffers.size())
    {
      bindings.resize(firstBinding + buffers.size());
      auto index = firstBinding;
      for(auto& buf : buffers)
      {
        bindings[index].buffer = static_cast<const TestGraphicsBuffer*>(buf);
        bindings[index].offset = offsets[index - firstBinding];
        index++;
      }
    }
    mCallStack.PushCall("BindVertexBuffers", "");
  }

  void BindUniformBuffers(const std::vector<Graphics::UniformBufferBinding>& bindings) override
  {
    mCommands.emplace_back();
    auto& cmd     = mCommands.back();
    cmd.type      = CommandType::BIND_UNIFORM_BUFFER;
    auto& bindCmd = cmd.data.bindUniformBuffers;
    for(const auto& binding : bindings)
    {
      if(binding.buffer)
      {
        auto testBuffer = static_cast<const TestGraphicsBuffer*>(binding.buffer);
        if(testBuffer->IsCPUAllocated()) // standalone uniforms
        {
          bindCmd.standaloneUniformsBufferBinding.buffer   = testBuffer;
          bindCmd.standaloneUniformsBufferBinding.offset   = binding.offset;
          bindCmd.standaloneUniformsBufferBinding.binding  = binding.binding;
          bindCmd.standaloneUniformsBufferBinding.emulated = true;
        }
        else // Bind regular UBO
        {
          // resize binding slots
          if(binding.binding >= bindCmd.uniformBufferBindings.size())
          {
            bindCmd.uniformBufferBindings.resize(binding.binding + 1);
          }
          auto& slot    = bindCmd.uniformBufferBindings[binding.binding];
          slot.buffer   = testBuffer;
          slot.offset   = binding.offset;
          slot.binding  = binding.binding;
          slot.emulated = false;
        }
      }
    }
    mCallStack.PushCall("BindUniformBuffers", "");
  }

  void BindPipeline(const Graphics::Pipeline& pipeline) override
  {
    mCommands.emplace_back();
    mCommands.back().type                       = CommandType::BIND_PIPELINE;
    mCommands.back().data.bindPipeline.pipeline = static_cast<const TestGraphicsPipeline*>(&pipeline);
    mCallStack.PushCall("BindPipeline", "");
  }

  void BindTextures(std::vector<Graphics::TextureBinding>& textureBindings) override
  {
    mCommands.emplace_back();
    mCommands.back().type                              = CommandType::BIND_TEXTURES;
    mCommands.back().data.bindTextures.textureBindings = std::move(textureBindings);
    mCallStack.PushCall("BindTextures", "");
  }

  void BindSamplers(std::vector<Graphics::SamplerBinding>& samplerBindings) override
  {
    mCommands.emplace_back();
    mCommands.back().data.bindSamplers.samplerBindings = std::move(samplerBindings);
    mCallStack.PushCall("BindSamplers", "");
  }

  void BindPushConstants(void*    data,
                         uint32_t size,
                         uint32_t binding) override
  {
    mCallStack.PushCall("BindPushConstants", "");
  }

  void BindIndexBuffer(const Graphics::Buffer& buffer,
                       uint32_t                offset,
                       Graphics::Format        format) override
  {
    mCommands.emplace_back();
    mCommands.back().type                        = CommandType::BIND_INDEX_BUFFER;
    mCommands.back().data.bindIndexBuffer.buffer = static_cast<const TestGraphicsBuffer*>(&buffer);
    mCommands.back().data.bindIndexBuffer.offset = offset;
    mCommands.back().data.bindIndexBuffer.format = format;
    mCallStack.PushCall("BindIndexBuffer", "");
  }

  void BeginRenderPass(
    Graphics::RenderPass&             renderPass,
    Graphics::RenderTarget&           renderTarget,
    Graphics::Extent2D                renderArea,
    std::vector<Graphics::ClearValue> clearValues) override
  {
    mCallStack.PushCall("BeginRenderPass", "");
  }

  /**
   * @brief Ends current render pass
   *
   * This command must be issued in order to finalize the render pass.
   * It's up to the implementation whether anything has to be done but
   * the Controller may use end RP marker in order to resolve resource
   * dependencies (for example, to know when target texture is ready
   * before passing it to another render pass).
   */
  void EndRenderPass() override
  {
    mCallStack.PushCall("EndRenderPass", "");
  }

  void Draw(
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance) override
  {
    mCommands.emplace_back();
    mCommands.back().type  = CommandType::DRAW;
    auto& cmd              = mCommands.back().data.draw;
    cmd.type               = DrawCallDescriptor::Type::DRAW;
    cmd.draw.vertexCount   = vertexCount;
    cmd.draw.instanceCount = instanceCount;
    cmd.draw.firstInstance = firstInstance;
    cmd.draw.firstVertex   = firstVertex;
    mCallStack.PushCall("Draw", "");
  }

  void DrawIndexed(
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t  vertexOffset,
    uint32_t firstInstance) override
  {
    mCommands.emplace_back();
    mCommands.back().type         = CommandType::DRAW_INDEXED;
    auto& cmd                     = mCommands.back().data.draw;
    cmd.type                      = DrawCallDescriptor::Type::DRAW_INDEXED;
    cmd.drawIndexed.firstIndex    = firstIndex;
    cmd.drawIndexed.firstInstance = firstInstance;
    cmd.drawIndexed.indexCount    = indexCount;
    cmd.drawIndexed.vertexOffset  = vertexOffset;
    cmd.drawIndexed.instanceCount = instanceCount;
    mCallStack.PushCall("DrawIndexed", "");
  }

  void DrawIndexedIndirect(
    Graphics::Buffer& buffer,
    uint32_t          offset,
    uint32_t          drawCount,
    uint32_t          stride) override
  {
    mCommands.emplace_back();
    mCommands.back().type             = CommandType::DRAW_INDEXED_INDIRECT;
    auto& cmd                         = mCommands.back().data.draw;
    cmd.type                          = DrawCallDescriptor::Type::DRAW_INDEXED_INDIRECT;
    cmd.drawIndexedIndirect.buffer    = static_cast<const TestGraphicsBuffer*>(&buffer);
    cmd.drawIndexedIndirect.offset    = offset;
    cmd.drawIndexedIndirect.drawCount = drawCount;
    cmd.drawIndexedIndirect.stride    = stride;
    mCallStack.PushCall("DrawIndexedIndirect", "");
  }

  void Reset() override
  {
    mCommands.clear();
    mCallStack.PushCall("Reset", "");
  }

  void SetScissor(Graphics::Rect2D value) override
  {
    TraceCallStack::NamedParams params;
    params["x"] << value.x;
    params["y"] << value.y;
    params["width"] << value.width;
    params["height"] << value.height;
    mCallStack.PushCall("SetScissor", params.str(), params);

    mCommands.emplace_back();
    mCommands.back().type                = CommandType::SET_SCISSOR;
    mCommands.back().data.scissor.region = value;
  }

  void SetScissorTestEnable(bool value) override
  {
    TraceCallStack::NamedParams params;
    params["value"] << (value ? "T" : "F");
    mCallStack.PushCall("SetScissorTestEnable", params.str(), params);

    mCommands.emplace_back();
    mCommands.back().type                    = CommandType::SET_SCISSOR_TEST;
    mCommands.back().data.scissorTest.enable = value;
  }

  void SetViewport(Graphics::Viewport value) override
  {
    TraceCallStack::NamedParams params;
    params["x"] << value.x;
    params["y"] << value.y;
    params["width"] << value.width;
    params["height"] << value.height;
    params["minDepth"] << value.minDepth;
    params["maxDepth"] << value.maxDepth;
    mCallStack.PushCall("SetViewport", params.str(), params);

    mCommands.emplace_back();
    mCommands.back().type                 = CommandType::SET_VIEWPORT;
    mCommands.back().data.viewport.region = value;
  }

  void SetViewportEnable(bool value) override
  {
    TraceCallStack::NamedParams params;
    params["value"] << (value ? "T" : "F");
    mCallStack.PushCall("SetViewportEnable", params.str(), params);

    mCommands.emplace_back();
    mCommands.back().type                     = CommandType::SET_VIEWPORT_TEST;
    mCommands.back().data.viewportTest.enable = value;
  }

  [[nodiscard]] const std::vector<Command>& GetCommands() const
  {
    return mCommands;
  }

  /**
   * Returns number of draw commands
   * @return
   */
  int GetDrawCallsCount();

  /**
   * Retrieves state resolve for selected draw call
   * @param drawCommandIndex
   */
  void GetStateForDrawCall(int drawCallIndex);

  /**
   * Retrieves commands of specified type
   */
  std::vector<Command*> GetCommandsByType(CommandTypeMask mask);

private:
  TraceCallStack&    mCallStack;
  TestGlAbstraction& mGlAbstraction;

  std::vector<Command> mCommands;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_COMMAND_BUFFER_H
