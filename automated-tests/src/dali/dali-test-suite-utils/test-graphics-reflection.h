#ifndef DALI_TEST_GRAPHICS_REFLECTION_H
#define DALI_TEST_GRAPHICS_REFLECTION_H

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

#include <dali/graphics-api/graphics-program-create-info.h>
#include <dali/graphics-api/graphics-reflection.h>
#include "test-gl-abstraction.h"

namespace Dali
{
class TestGraphicsReflection : public Graphics::Reflection
{
public:
  TestGraphicsReflection(TestGlAbstraction& gl, uint32_t program_id, Property::Array& vertexFormats, const Graphics::ProgramCreateInfo& createInfo, std::vector<UniformData>& customUniforms);

  uint32_t                                        GetVertexAttributeLocation(const std::string& name) const override;
  Dali::Graphics::VertexInputAttributeFormat      GetVertexAttributeFormat(uint32_t location) const override;
  std::string                                     GetVertexAttributeName(uint32_t location) const override;
  std::vector<uint32_t>                           GetVertexAttributeLocations() const override;
  uint32_t                                        GetUniformBlockCount() const override;
  uint32_t                                        GetUniformBlockBinding(uint32_t index) const override;
  uint32_t                                        GetUniformBlockSize(uint32_t index) const override;
  bool                                            GetUniformBlock(uint32_t index, Dali::Graphics::UniformBlockInfo& out) const override;
  std::vector<uint32_t>                           GetUniformBlockLocations() const override;
  std::string                                     GetUniformBlockName(uint32_t blockIndex) const override;
  uint32_t                                        GetUniformBlockMemberCount(uint32_t blockIndex) const override;
  std::string                                     GetUniformBlockMemberName(uint32_t blockIndex, uint32_t memberLocation) const override;
  uint32_t                                        GetUniformBlockMemberOffset(uint32_t blockIndex, uint32_t memberLocation) const override;
  bool                                            GetNamedUniform(const std::string& name, Dali::Graphics::UniformInfo& out) const override;
  const std::vector<Dali::Graphics::UniformInfo>& GetSamplers() const override;
  Graphics::ShaderLanguage                        GetLanguage() const override;

public: // Test methods
  struct TestUniformInfo
  {
    std::string            name{""}; // baseName in the case of arrays
    Graphics::UniformClass uniformClass{Graphics::UniformClass::UNDEFINED};
    uint32_t               binding{0u};
    uint32_t               bufferIndex{0u};
    std::vector<uint32_t>  offsets{};
    std::vector<uint32_t>  locations{};
    uint32_t               numElements{0u}; // 0 elements means this isn't an array; 1 element means this is an array of size 1
    Property::Type         type;
  };

  struct TestUniformBlockInfo
  {
    std::string                  name{""};
    uint32_t                     descriptorSet{0u};
    uint32_t                     binding{0u};
    uint32_t                     size{0u};
    std::vector<TestUniformInfo> members{};
  };

  void SetAttributes(std::vector<std::string> locations)
  {
    mAttributes.clear();
    mAttributes.resize(locations.size());
    for(auto& location : locations)
    {
      mAttributes.push_back(location);
    }
  }

  const TestUniformBlockInfo& GetTestUniformBlock(uint32_t index) const
  {
    return mUniformBlocks[index];
  }

  TestGlAbstraction&               mGl;
  mutable std::vector<std::string> mAttributes;
  std::vector<UniformData>         mCustomUniforms;

  TestUniformBlockInfo              mDefaultUniformBlock{}; ///< The emulated UBO containing all the standalone uniforms
  std::vector<TestUniformBlockInfo> mUniformBlocks{};       ///< List of uniform blocks
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_REFLECTION_H
