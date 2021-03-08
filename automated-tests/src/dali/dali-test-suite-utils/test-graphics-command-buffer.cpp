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

int TestGraphicsCommandBuffer::GetDrawCallsCount()
{
  int count = 0;
  for( auto& cmd : mCommands )
  {
    if( cmd.type == CommandType::DRAW ||
      cmd.type == CommandType::DRAW_INDEXED ||
      cmd.type == CommandType::DRAW_INDEXED_INDIRECT )
    {
      ++count;
    }
  }
  return count;
}

void TestGraphicsCommandBuffer::GetStateForDrawCall( int drawCallIndex )
{
  int index = 0;
  std::vector<Command> mCommandStack{};
  for( auto& cmd : mCommands )
  {
    mCommandStack.push_back(cmd);
    if( cmd.type == CommandType::DRAW ||
        cmd.type == CommandType::DRAW_INDEXED ||
        cmd.type == CommandType::DRAW_INDEXED_INDIRECT )
    {
      if( index == drawCallIndex )
      {
        break;
      }
      mCommandStack.clear();
      ++index;
    }
  }
}

std::vector<Command*> TestGraphicsCommandBuffer::GetCommandsByType( CommandTypeMask mask )
{
  std::vector<Command*> mCommandStack{};
  for( auto& cmd : mCommands )
  {
    if(uint32_t(cmd.type) == (mask & uint32_t(cmd.type)) )
    {
      mCommandStack.emplace_back( &cmd );
    }
  }
  return mCommandStack;
}

} // namespace Dali
