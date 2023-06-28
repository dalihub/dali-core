#ifndef DALI_TEST_GRAPHICS_PROGRAM_H
#define DALI_TEST_GRAPHICS_PROGRAM_H

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

#include <dali/graphics-api/graphics-program-create-info.h>
#include <dali/graphics-api/graphics-program.h>
#include "test-gl-abstraction.h"
#include "test-graphics-reflection.h"

namespace Dali
{
class TestGraphicsProgramImpl
{
public:
  TestGraphicsProgramImpl(TestGlAbstraction& gl, const Graphics::ProgramCreateInfo& createInfo, Property::Array& vertexFormats, std::vector<UniformData>& customUniforms, std::vector<TestGraphicsReflection::TestUniformBlockInfo>& customUniformBlocks);

  // For API
  const TestGraphicsReflection& GetReflection() const
  {
    // Build a reflection
    return mReflection;
  }

  // For tests
  TestGraphicsReflection& GetProgramReflection()
  {
    return mReflection;
  }

  bool GetParameter(uint32_t parameterId, void* outData);

public:
  TestGlAbstraction&          mGl;
  uint32_t                    mId;
  Graphics::ProgramCreateInfo mCreateInfo;
  TestGraphicsReflection      mReflection;
};

class TestGraphicsProgram : public Graphics::Program
{
public:
  TestGraphicsProgram(TestGraphicsProgramImpl* impl);

  const TestGraphicsReflection& GetReflection() const
  {
    return mImpl->GetReflection();
  }
  bool GetParameter(uint32_t parameterId, void* outData)
  {
    return mImpl->GetParameter(parameterId, outData);
  }

  TestGraphicsReflection& GetProgamReflection()
  {
    return mImpl->GetProgramReflection();
  }

public:
  TestGraphicsProgramImpl* mImpl;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_PROGRAM_H
