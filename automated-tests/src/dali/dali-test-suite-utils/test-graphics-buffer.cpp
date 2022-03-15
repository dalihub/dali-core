/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "dali-test-suite-utils.h"
#include "test-graphics-program.h"
#include "test-graphics-reflection.h"

namespace Dali
{
TestGraphicsBuffer::TestGraphicsBuffer(TraceCallStack& callStack, TestGlAbstraction& glAbstraction, uint32_t size, Graphics::BufferUsageFlags usage)
: mCallStack(callStack),
  mGl(glAbstraction),
  mUsage(usage)
{
  memory.resize(size);
  mGl.GetBufferTrace().EnableLogging(false);
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
  namedParams["offset"] << offset;
  namedParams["size"] << size;
  mCallStack.PushCall("Buffer::Upload", o.str(), namedParams);
  if(size <= memory.size() && mCreated)
  {
    // Use subData to avoid re-allocation
    mGl.BufferSubData(GetTarget(), offset, size, &memory[offset]);
  }
  else
  {
    mGl.BufferData(GetTarget(), GLsizeiptr(size), &memory[0], GL_STATIC_DRAW); //@todo Query - do we need other usages?
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

void TestGraphicsBuffer::BindAsUniformBuffer(const TestGraphicsProgram* program, const Dali::UniformBufferBindingDescriptor& uboBinding) const
{
  auto*       reflection = static_cast<const TestGraphicsReflection*>(&program->GetReflection());
  const auto& uboInfo    = reflection->GetTestUniformBlock(0u);

  auto  offset = uboBinding.offset;
  auto* data   = memory.data() + offset;

  for(const auto& member : uboInfo.members)
  {
    uint32_t numElements = member.numElements > 0 ? member.numElements : 1;

    for(uint32_t i = 0; i < numElements; ++i)
    {
      switch(member.type)
      {
        case Property::VECTOR4:
        {
          auto value = *reinterpret_cast<const Dali::Vector4*>(data + member.offsets[i]);
          mGl.Uniform4f(member.locations[i], value.x, value.y, value.z, value.w);
          break;
        }
        case Property::VECTOR3:
        {
          auto value = *reinterpret_cast<const Dali::Vector3*>(data + member.offsets[i]);
          mGl.Uniform3f(member.locations[i], value.x, value.y, value.z);
          break;
        }
        case Property::VECTOR2:
        {
          auto value = *reinterpret_cast<const Dali::Vector2*>(data + member.offsets[i]);
          mGl.Uniform2f(member.locations[i], value.x, value.y);
          break;
        }
        case Property::FLOAT:
        {
          auto value = *reinterpret_cast<const float*>(data + member.offsets[i]);
          mGl.Uniform1f(member.locations[i], value);
          break;
        }
        case Property::INTEGER:
        {
          auto ptr   = reinterpret_cast<const GLint*>(data + member.offsets[i]);
          auto value = *ptr;
          mGl.Uniform1i(member.locations[i], value);
          break;
        }
        case Property::MATRIX:
        {
          auto value = reinterpret_cast<const float*>(data + member.offsets[i]);
          mGl.UniformMatrix4fv(member.locations[i], 1, GL_FALSE, value);
          break;
        }
        case Property::MATRIX3:
        {
          auto value = reinterpret_cast<const float*>(data + member.offsets[i]);
          mGl.UniformMatrix3fv(member.locations[i], 1, GL_FALSE, value);
          break;
        }
        default:
        {
          fprintf(stderr, "\n%s type not found\n", member.name.c_str());
        }
      }
    }
  }
}

} // namespace Dali
