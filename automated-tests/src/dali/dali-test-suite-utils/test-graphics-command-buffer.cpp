/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

std::ostream& operator<<(std::ostream& os, Graphics::BlendFactor op)
{
  switch(op)
  {
    case Graphics::BlendFactor::ZERO:
    {
      os << "ZERO";
      break;
    }
    case Graphics::BlendFactor::ONE:
    {
      os << "ONE";
      break;
    }
    case Graphics::BlendFactor::SRC_COLOR:
    {
      os << "SRC_COLOR";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_SRC_COLOR:
    {
      os << "ONE_MINUS_SRC_COLOR";
      break;
    }
    case Graphics::BlendFactor::DST_COLOR:
    {
      os << "DST_COLOR";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_DST_COLOR:
    {
      os << "ONE_MINUS_DST_COLOR";
      break;
    }
    case Graphics::BlendFactor::SRC_ALPHA:
    {
      os << "SRC_ALPHA";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_SRC_ALPHA:
    {
      os << "ONE_MINUS_SRC_ALPHA";
      break;
    }
    case Graphics::BlendFactor::DST_ALPHA:
    {
      os << "DST_ALPHA";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_DST_ALPHA:
    {
      os << "ONE_MINUS_DST_ALPHA";
      break;
    }
    case Graphics::BlendFactor::CONSTANT_COLOR:
    {
      os << "CONSTANT_COLOR";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_CONSTANT_COLOR:
    {
      os << "ONE_MINUS_CONSTANT_COLOR";
      break;
    }
    case Graphics::BlendFactor::CONSTANT_ALPHA:
    {
      os << "CONSTANT_ALPHA";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
    {
      os << "ONE_MINUS_CONSTANT_ALPHA";
      break;
    }
    case Graphics::BlendFactor::SRC_ALPHA_SATURATE:
    {
      os << "SRC_ALPHA_SATURATE";
      break;
    }
    case Graphics::BlendFactor::SRC1_COLOR:
    {
      os << "SRC1_COLOR";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_SRC1_COLOR:
    {
      os << "ONE_MINUS_SRC1_COLOR";
      break;
    }
    case Graphics::BlendFactor::SRC1_ALPHA:
    {
      os << "SRC1_ALPHA";
      break;
    }
    case Graphics::BlendFactor::ONE_MINUS_SRC1_ALPHA:
    {
      os << "ONE_MINUS_SRC1_ALPHA";
      break;
    }
  }
  return os;
};

std::ostream& operator<<(std::ostream& os, Graphics::BlendOp op)
{
  switch(op)
  {
    case Graphics::BlendOp::ADD:
    {
      os << "ADD";
      break;
    }
    case Graphics::BlendOp::SUBTRACT:
    {
      os << "SUBTRACT";
      break;
    }
    case Graphics::BlendOp::REVERSE_SUBTRACT:
    {
      os << "REVERSE_SUBTRACT";
      break;
    }
    case Graphics::BlendOp::MIN:
    {
      os << "MIN";
      break;
    }
    case Graphics::BlendOp::MAX:
    {
      os << "MAX";
      break;
    }
    case Graphics::BlendOp::MULTIPLY:
    {
      os << "MULTIPLY";
      break;
    }
    case Graphics::BlendOp::SCREEN:
    {
      os << "SCREEN";
      break;
    }
    case Graphics::BlendOp::OVERLAY:
    {
      os << "OVERLAY";
      break;
    }
    case Graphics::BlendOp::DARKEN:
    {
      os << "DARKEN";
      break;
    }
    case Graphics::BlendOp::LIGHTEN:
    {
      os << "LIGHTEN";
      break;
    }
    case Graphics::BlendOp::COLOR_DODGE:
    {
      os << "COLOR_DODGE";
      break;
    }
    case Graphics::BlendOp::COLOR_BURN:
    {
      os << "COLOR_BURN";
      break;
    }
    case Graphics::BlendOp::HARD_LIGHT:
    {
      os << "HARD_LIGHT";
      break;
    }
    case Graphics::BlendOp::SOFT_LIGHT:
    {
      os << "SOFT_LIGHT";
      break;
    }
    case Graphics::BlendOp::DIFFERENCE:
    {
      os << "DIFFERENCE";
      break;
    }
    case Graphics::BlendOp::EXCLUSION:
    {
      os << "EXCLUSION";
      break;
    }
    case Graphics::BlendOp::HUE:
    {
      os << "HUE";
      break;
    }
    case Graphics::BlendOp::SATURATION:
    {
      os << "SATURATION";
      break;
    }
    case Graphics::BlendOp::COLOR:
    {
      os << "COLOR";
      break;
    }
    case Graphics::BlendOp::LUMINOSITY:
    {
      os << "LUMINOSITY";
      break;
    }
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, Graphics::StencilOp op)
{
  switch(op)
  {
    case Graphics::StencilOp::KEEP:
      os << "KEEP";
      return os;
    case Graphics::StencilOp::ZERO:
      os << "ZERO";
      return os;
    case Graphics::StencilOp::REPLACE:
      os << "REPLACE";
      return os;
    case Graphics::StencilOp::INCREMENT_AND_CLAMP:
      os << "INCREMENT_AND_CLAMP";
      return os;
    case Graphics::StencilOp::DECREMENT_AND_CLAMP:
      os << "DECREMENT_AND_CLAMP";
      return os;
    case Graphics::StencilOp::INVERT:
      os << "INVERT";
      return os;
    case Graphics::StencilOp::INCREMENT_AND_WRAP:
      os << "INCREMENT_AND_WRAP";
      return os;
    case Graphics::StencilOp::DECREMENT_AND_WRAP:
      os << "DECREMENT_AND_WRAP";
      return os;
  }
  return os;
};

std::ostream& operator<<(std::ostream& os, Graphics::CompareOp op)
{
  switch(op)
  {
    case Graphics::CompareOp::NEVER:
      os << "NEVER";
      return os;
    case Graphics::CompareOp::LESS:
      os << "LESS";
      return os;
    case Graphics::CompareOp::EQUAL:
      os << "EQUAL";
      return os;
    case Graphics::CompareOp::LESS_OR_EQUAL:
      os << "LESS_OR_EQUAL";
      return os;
    case Graphics::CompareOp::GREATER:
      os << "GREATER";
      return os;
    case Graphics::CompareOp::NOT_EQUAL:
      os << "NOT_EQUAL";
      return os;
    case Graphics::CompareOp::GREATER_OR_EQUAL:
      os << "GREATER_OR_EQUAL";
      return os;
    case Graphics::CompareOp::ALWAYS:
      os << "ALWAYS";
      return os;
  }
  return os;
};

TestGraphicsCommandBuffer::TestGraphicsCommandBuffer(TraceCallStack& callstack)
: mCallStack(callstack)
{
}

int TestGraphicsCommandBuffer::GetDrawCallsCount()
{
  int count = 0;
  for(auto& cmd : mCommands)
  {
    if(cmd.type == CommandType::DRAW ||
       cmd.type == CommandType::DRAW_INDEXED ||
       cmd.type == CommandType::DRAW_INDEXED_INDIRECT)
    {
      ++count;
    }
  }
  return count;
}

void TestGraphicsCommandBuffer::GetStateForDrawCall(int drawCallIndex)
{
  int                  index = 0;
  std::vector<Command> mCommandStack{};
  for(auto& cmd : mCommands)
  {
    mCommandStack.push_back(cmd);
    if(cmd.type == CommandType::DRAW ||
       cmd.type == CommandType::DRAW_INDEXED ||
       cmd.type == CommandType::DRAW_INDEXED_INDIRECT)
    {
      if(index == drawCallIndex)
      {
        break;
      }
      mCommandStack.clear();
      ++index;
    }
  }
}

std::vector<const Command*> TestGraphicsCommandBuffer::GetCommandsByType(CommandTypeMask mask) const
{
  std::vector<const Command*> mCommandStack{};
  for(auto& cmd : mCommands)
  {
    if(uint32_t(cmd.type) == (mask & uint32_t(cmd.type)))
    {
      mCommandStack.emplace_back(&cmd);
    }
  }
  return mCommandStack;
}

std::vector<const Command*> TestGraphicsCommandBuffer::GetChildCommandsByType(CommandTypeMask mask) const
{
  std::vector<const Command*> mCommandStack{};
  for(auto& cmd : mCommands)
  {
    if(uint32_t(cmd.type) == (mask & uint32_t(cmd.type)))
    {
      mCommandStack.emplace_back(&cmd);
    }
    if(cmd.type == CommandType::EXECUTE_COMMAND_BUFFERS)
    {
      for(auto secondaryCB : cmd.data.executeCommandBuffers.buffers)
      {
        for(auto command : secondaryCB->GetChildCommandsByType(mask))
        {
          mCommandStack.push_back(command);
        }
      }
    }
  }
  return mCommandStack;
}

} // namespace Dali
