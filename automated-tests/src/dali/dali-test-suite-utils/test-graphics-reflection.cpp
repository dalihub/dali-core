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

#include "test-graphics-reflection.h"
#include <dali/public-api/object/property-map.h>

namespace Dali
{
TestGraphicsReflection::TestGraphicsReflection(TestGlAbstraction& gl, Property::Array& vfs)
: mGl(gl)
{
  for(Property::Array::SizeType i = 0; i < vfs.Count(); ++i)
  {
    Property::Map* vertexFormat = vfs[i].GetMap();
    if(vertexFormat)
    {
      for(Property::Map::SizeType j = 0; j < vertexFormat->Count(); ++j)
      {
        auto key = vertexFormat->GetKeyAt(j);
        if(key.type == Property::Key::STRING)
        {
          mAttributes.push_back(key.stringKey);
        }
      }
    }
  }
}

uint32_t TestGraphicsReflection::GetVertexAttributeLocation(const std::string& name) const
{
  // Automatically assign locations to named attributes when requested
  auto iter = std::find(mAttributes.begin(), mAttributes.end(), name);
  if(iter != mAttributes.end())
  {
    return iter - mAttributes.begin();
  }
  else
  {
    uint32_t location = mAttributes.size();
    mAttributes.push_back(name);
    return location;
  }
  return 0u;
}

Dali::Graphics::VertexInputAttributeFormat TestGraphicsReflection::GetVertexAttributeFormat(uint32_t location) const
{
  return Dali::Graphics::VertexInputAttributeFormat{};
}

std::string TestGraphicsReflection::GetVertexAttributeName(uint32_t location) const
{
  return 0u;
}

std::vector<uint32_t> TestGraphicsReflection::GetVertexAttributeLocations() const
{
  std::vector<uint32_t> locs;
  for(uint32_t i = 0; i < mAttributes.size(); ++i)
  {
    locs.push_back(i);
  }
  return locs;
}

uint32_t TestGraphicsReflection::GetUniformBlockCount() const
{
  return 0u;
}

uint32_t TestGraphicsReflection::GetUniformBlockBinding(uint32_t index) const
{
  return 0u;
}

uint32_t TestGraphicsReflection::GetUniformBlockSize(uint32_t index) const
{
  return 0u;
}

bool TestGraphicsReflection::GetUniformBlock(uint32_t index, Dali::Graphics::UniformBlockInfo& out) const
{
  return true;
}

std::vector<uint32_t> TestGraphicsReflection::GetUniformBlockLocations() const
{
  return std::vector<uint32_t>{};
}

std::string TestGraphicsReflection::GetUniformBlockName(uint32_t blockIndex) const
{
  return std::string{};
}

uint32_t TestGraphicsReflection::GetUniformBlockMemberCount(uint32_t blockIndex) const
{
  return 0u;
}

std::string TestGraphicsReflection::GetUniformBlockMemberName(uint32_t blockIndex, uint32_t memberLocation) const
{
  return std::string{};
}

uint32_t TestGraphicsReflection::GetUniformBlockMemberOffset(uint32_t blockIndex, uint32_t memberLocation) const
{
  return 0u;
}

bool TestGraphicsReflection::GetNamedUniform(const std::string& name, Dali::Graphics::UniformInfo& out) const
{
  return true;
}

std::vector<Dali::Graphics::UniformInfo> TestGraphicsReflection::GetSamplers() const
{
  return std::vector<Dali::Graphics::UniformInfo>{};
}

Graphics::ShaderLanguage TestGraphicsReflection::GetLanguage() const
{
  return Graphics::ShaderLanguage::GLSL_3_1;
}

} // namespace Dali
