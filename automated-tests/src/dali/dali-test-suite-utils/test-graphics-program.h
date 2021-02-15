#ifndef DALI_TEST_GRAPHICS_PROGRAM_H
#define DALI_TEST_GRAPHICS_PROGRAM_H

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

#include <dali/graphics-api/graphics-program-create-info.h>
#include <dali/graphics-api/graphics-program.h>
#include "test-gl-abstraction.h"
#include "test-graphics-reflection.h"

namespace Dali
{
class TestGraphicsProgram : public Graphics::Program
{
public:
  TestGraphicsProgram(TestGlAbstraction& gl, const Graphics::ProgramCreateInfo& createInfo, Property::Array& vertexFormats);

  // For API
  const TestGraphicsReflection& GetReflection() const
  {
    // Build a reflection
    return mReflection;
  }

  // For tests
  TestGraphicsReflection& GetProgamReflection()
  {
    return mReflection;
  }

public:
  TestGlAbstraction&          mGl;
  Graphics::ProgramCreateInfo mCreateInfo;
  TestGraphicsReflection      mReflection;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_PROGRAM_H
