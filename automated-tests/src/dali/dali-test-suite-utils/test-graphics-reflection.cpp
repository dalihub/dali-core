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
#include "test-graphics-shader.h"

#include <dali/public-api/object/property-map.h>
#include <vector>
#include <string>
namespace Dali
{
namespace
{
static const std::vector<UniformData> UNIFORMS =
{
  UniformData("uRendererColor",Property::Type::FLOAT),
  UniformData("uCustom", Property::Type::INTEGER),
  UniformData("uCustom3", Property::Type::VECTOR3),
  UniformData("uFadeColor", Property::Type::VECTOR4),
  UniformData("uUniform1", Property::Type::VECTOR4),
  UniformData("uUniform2", Property::Type::VECTOR4),
  UniformData("uUniform3", Property::Type::VECTOR4),
  UniformData("uFadeProgress", Property::Type::FLOAT),
  UniformData("uANormalMatrix", Property::Type::MATRIX3),
  UniformData("sEffect", Property::Type::FLOAT),
  UniformData("sTexture", Property::Type::FLOAT),
  UniformData("sTextureRect", Property::Type::FLOAT),
  UniformData("sGloss", Property::Type::FLOAT),
  UniformData("uColor", Property::Type::VECTOR4),
  UniformData("uModelMatrix", Property::Type::MATRIX),
  UniformData("uModelView", Property::Type::MATRIX),
  UniformData("uMvpMatrix", Property::Type::MATRIX),
  UniformData("uNormalMatrix", Property::Type::MATRIX3),
  UniformData("uProjection", Property::Type::MATRIX),
  UniformData("uSize", Property::Type::VECTOR3),
  UniformData("uViewMatrix", Property::Type::MATRIX),
  UniformData("uLightCameraProjectionMatrix", Property::Type::MATRIX),
  UniformData("uLightCameraViewMatrix", Property::Type::MATRIX),
};
}

TestGraphicsReflection::TestGraphicsReflection(TestGlAbstraction& gl, Property::Array& vfs, const Graphics::ProgramCreateInfo& createInfo, std::vector<UniformData>& customUniforms)
  : mGl(gl),
    mCustomUniforms(customUniforms)
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

  mDefaultUniformBlock.name = "";
  mDefaultUniformBlock.members = {};
  mDefaultUniformBlock.binding = 0;
  mDefaultUniformBlock.size = 64 * (UNIFORMS.size() + mCustomUniforms.size());
  mDefaultUniformBlock.descriptorSet = 0;
  mDefaultUniformBlock.members.clear();
  int loc = 0;
  for( const auto& data : UNIFORMS )
  {
    mDefaultUniformBlock.members.emplace_back();
    auto& item = mDefaultUniformBlock.members.back();
    item.name = data.name;
    item.binding = 0;
    item.offset = loc*64;
    item.location = loc++;
    item.bufferIndex = 0;
    item.uniformClass = Graphics::UniformClass::UNIFORM;
  }

  for( const auto& data : mCustomUniforms )
  {
    fprintf(stderr, "\ncustom uniforms: %s\n", data.name.c_str());
    mDefaultUniformBlock.members.emplace_back();
    auto& item = mDefaultUniformBlock.members.back();
    item.name = data.name;
    item.binding = 0;
    item.offset = loc*64;
    item.location = loc++;
    item.bufferIndex = 0;
    item.uniformClass = Graphics::UniformClass::UNIFORM;
  }

  mUniformBlocks.push_back(mDefaultUniformBlock);
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
  return mUniformBlocks.size();
}

uint32_t TestGraphicsReflection::GetUniformBlockBinding(uint32_t index) const
{
  return 0u;
}

uint32_t TestGraphicsReflection::GetUniformBlockSize(uint32_t index) const
{
  // 64 bytes per uniform (64 = 4x4 matrix)
  // TODO: fix if array will be used
  return 64 * (UNIFORMS.size() + mCustomUniforms.size());
}

bool TestGraphicsReflection::GetUniformBlock(uint32_t index, Dali::Graphics::UniformBlockInfo& out) const
{
  if(index >= mUniformBlocks.size())
  {
    return false;
  }

  const auto& block = mUniformBlocks[index];

  out.name          = block.name;
  out.binding       = block.binding;
  out.descriptorSet = block.descriptorSet;
  auto membersSize  = block.members.size();
  out.members.resize(membersSize);
  out.size = block.size;
  for(auto i = 0u; i < out.members.size(); ++i)
  {
    const auto& memberUniform   = block.members[i];
    out.members[i].name         = memberUniform.name;
    out.members[i].binding      = block.binding;
    out.members[i].uniformClass = Graphics::UniformClass::UNIFORM;
    out.members[i].offset       = memberUniform.offset;
    out.members[i].location     = memberUniform.location;
  }

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
  if(blockIndex < mUniformBlocks.size())
  {
    return static_cast<uint32_t>(mUniformBlocks[blockIndex].members.size());
  }
  else
  {
    return 0u;
  }
}

std::string TestGraphicsReflection::GetUniformBlockMemberName(uint32_t blockIndex, uint32_t memberLocation) const
{
  if(blockIndex < mUniformBlocks.size() && memberLocation < mUniformBlocks[blockIndex].members.size())
  {
    return mUniformBlocks[blockIndex].members[memberLocation].name;
  }
  else
  {
    return std::string();
  }
}

uint32_t TestGraphicsReflection::GetUniformBlockMemberOffset(uint32_t blockIndex, uint32_t memberLocation) const
{
  if(blockIndex < mUniformBlocks.size() && memberLocation < mUniformBlocks[blockIndex].members.size())
  {
    return mUniformBlocks[blockIndex].members[memberLocation].offset;
  }
  else
  {
    return 0u;
  }
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

Dali::Property::Type TestGraphicsReflection::GetMemberType( int blockIndex, int location) const
{
  return location < static_cast<int>(UNIFORMS.size()) ? UNIFORMS[location].type : mCustomUniforms[location - UNIFORMS.size()].type;
}

} // namespace Dali
