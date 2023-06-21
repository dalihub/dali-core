#ifndef DALI_TEST_GRAPHICS_BUFFER_H
#define DALI_TEST_GRAPHICS_BUFFER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-buffer-create-info.h>
#include <dali/graphics-api/graphics-buffer.h>
#include <dali/graphics-api/graphics-types.h>

#include "test-gl-abstraction.h"
#include "test-trace-call-stack.h"

namespace Dali
{
class TestGraphicsProgram;
class TestGraphicsController;
class UniformBufferBindingDescriptor;

class TestGraphicsBuffer : public Graphics::Buffer
{
public:
  TestGraphicsBuffer(const Graphics::BufferCreateInfo& createInfo, TestGraphicsController& controller, TestGlAbstraction& glAbstraction, TraceCallStack& callStack);
  ~TestGraphicsBuffer();
  void DiscardResource();

  void   Bind();
  void   Unbind();
  void   Upload(uint32_t offset, uint32_t size);
  GLenum GetTarget();

  bool IsCPUAllocated() const
  {
    return mCpuOnly;
  }

  void BindAsUniformBuffer(const TestGraphicsProgram* program, const Dali::UniformBufferBindingDescriptor& uboBinding) const;

  TraceCallStack&         mCallStack;
  TestGraphicsController& mController;
  TestGlAbstraction&      mGl;
  std::vector<uint8_t>    memory;

  Graphics::BufferCreateInfo mCreateInfo;
  Graphics::BufferUsageFlags mUsage;
  GLuint                     mId{0};
  bool                       mCreated{false};
  bool                       mCpuOnly{false};
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_BUFFER_H
