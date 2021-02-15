#ifndef DALI_TEST_GRAPHICS_REFLECTION_H
#define DALI_TEST_GRAPHICS_REFLECTION_H

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

#include <dali/graphics-api/graphics-reflection.h>
#include "test-gl-abstraction.h"

namespace Dali
{
class TestGraphicsReflection : public Graphics::Reflection
{
public:
  TestGraphicsReflection(TestGlAbstraction& gl, Property::Array& vertexFormats);

  uint32_t                                   GetVertexAttributeLocation(const std::string& name) const override;
  Dali::Graphics::VertexInputAttributeFormat GetVertexAttributeFormat(uint32_t location) const override;
  std::string                                GetVertexAttributeName(uint32_t location) const override;
  std::vector<uint32_t>                      GetVertexAttributeLocations() const override;
  uint32_t                                   GetUniformBlockCount() const override;
  uint32_t                                   GetUniformBlockBinding(uint32_t index) const override;
  uint32_t                                   GetUniformBlockSize(uint32_t index) const override;
  bool                                       GetUniformBlock(uint32_t index, Dali::Graphics::UniformBlockInfo& out) const override;
  std::vector<uint32_t>                      GetUniformBlockLocations() const override;
  std::string                                GetUniformBlockName(uint32_t blockIndex) const override;
  uint32_t                                   GetUniformBlockMemberCount(uint32_t blockIndex) const override;
  std::string                                GetUniformBlockMemberName(uint32_t blockIndex, uint32_t memberLocation) const override;
  uint32_t                                   GetUniformBlockMemberOffset(uint32_t blockIndex, uint32_t memberLocation) const override;
  bool                                       GetNamedUniform(const std::string& name, Dali::Graphics::UniformInfo& out) const override;
  std::vector<Dali::Graphics::UniformInfo>   GetSamplers() const override;
  Graphics::ShaderLanguage                   GetLanguage() const override;

public: // Test methods
  void SetAttributes(std::vector<std::string> locations)
  {
    mAttributes.clear();
    mAttributes.resize(locations.size());
    for(auto& location : locations)
    {
      mAttributes.push_back(location);
    }
  }

  TestGlAbstraction&               mGl;
  mutable std::vector<std::string> mAttributes;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_REFLECTION_H
