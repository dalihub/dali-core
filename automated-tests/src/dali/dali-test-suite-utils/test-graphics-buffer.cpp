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

#include "test-graphics-buffer.h"

#include <sstream>

namespace Dali
{
TestGraphicsBuffer::TestGraphicsBuffer(TraceCallStack& callStack, TestGlAbstraction& glAbstraction, uint32_t size, Graphics::BufferUsageFlags usage)
: mCallStack(callStack),
  mGl(glAbstraction),
  mUsage(usage)
{
  memory.reserve(size);
  mGl.GetBufferTrace().EnableLogging(true);
}

void TestGraphicsBuffer::Bind()
{
  mCallStack.PushCall("Buffer::Bind", "");
  if(!mId)
  {
    mGl.GenBuffers(1, &mId);
  }
  mGl.BindBuffer(GetTarget(), mId);
}

void TestGraphicsBuffer::Unbind()
{
  mCallStack.PushCall("Buffer::Unbind", "");
  if(mId)
  {
    mGl.BindBuffer(GetTarget(), 0);
  }
}

void TestGraphicsBuffer::Upload(uint32_t offset, uint32_t size)
{
  std::ostringstream o;
  o << offset << ", " << size;
  TraceCallStack::NamedParams namedParams;
  namedParams["offset"] = offset;
  namedParams["size"]   = size;
  mCallStack.PushCall("Buffer::Upload", o.str(), namedParams);
  if(size <= memory.size() && mCreated)
  {
    // Use subData to avoid re-allocation
    mGl.BufferSubData(GetTarget(), offset, size, &memory[offset]);
  }
  else
  {
    mGl.BufferData(GetTarget(), size, &memory[0], GL_STATIC_DRAW); //@todo Query - do we need other usages?
    mCreated = true;
  }
}

GLenum TestGraphicsBuffer::GetTarget()
{
  GLenum target = GL_ARRAY_BUFFER;
  if((mUsage & static_cast<Graphics::BufferUsageFlags>(Graphics::BufferUsage::INDEX_BUFFER)) != 0)
  {
    target = GL_ELEMENT_ARRAY_BUFFER;
  }
  return target;
}

} // namespace Dali
