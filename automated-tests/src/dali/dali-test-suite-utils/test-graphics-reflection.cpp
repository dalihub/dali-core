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

#include "test-graphics-reflection.h"
#include "test-graphics-controller.h"
#include "test-graphics-shader.h"

#include <dali/public-api/object/property-map.h>
#include <string>
#include <vector>

extern "C"
{
  void tet_infoline(const char* str);
  void tet_printf(const char* format, ...);
}

namespace Dali
{
namespace
{
static const std::vector<UniformData> UNIFORMS =
  {
    UniformData("uRendererColor", Property::Type::FLOAT),
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
    UniformData("uActorColor", Property::Type::VECTOR4),
    UniformData("uModelMatrix", Property::Type::MATRIX),
    UniformData("uModelView", Property::Type::MATRIX),
    UniformData("uMvpMatrix", Property::Type::MATRIX),
    UniformData("uNormalMatrix", Property::Type::MATRIX3),
    UniformData("uProjection", Property::Type::MATRIX),
    UniformData("uScale", Property::Type::VECTOR3),
    UniformData("uSize", Property::Type::VECTOR3),
    UniformData("uViewMatrix", Property::Type::MATRIX),
    UniformData("uLightCameraProjectionMatrix", Property::Type::MATRIX),
    UniformData("uLightCameraViewMatrix", Property::Type::MATRIX),

    // WARNING: IF YOU CHANGE THIS LIST, ALSO CHANGE mActiveUniforms IN test-gl-abstraction, Initialize
};

/**
 * Helper function that returns size of uniform datatypes based
 * on property type.
 */
constexpr int GetSizeForType(Property::Type type)
{
  switch(type)
  {
    case Property::Type::BOOLEAN:
    {
      return sizeof(bool);
    }
    case Property::Type::FLOAT:
    {
      return sizeof(float);
    }
    case Property::Type::INTEGER:
    {
      return sizeof(int);
    }
    case Property::Type::VECTOR2:
    {
      return sizeof(Vector2);
    }
    case Property::Type::VECTOR3:
    {
      return sizeof(Vector3);
    }
    case Property::Type::VECTOR4:
    {
      return sizeof(Vector4);
    }
    case Property::Type::MATRIX3:
    {
      return sizeof(Matrix3);
    }
    case Property::Type::MATRIX:
    {
      return sizeof(Matrix);
    }
    default:
    {
      return 0;
    }
  };
}

} // namespace

TestGraphicsReflection::TestGraphicsReflection(TestGraphicsController& controller, TestGlAbstraction& gl, uint32_t programId, Property::Array& vfs, const Graphics::ProgramCreateInfo& createInfo, std::vector<UniformData>& customUniforms, std::vector<TestGraphicsReflection::TestUniformBlockInfo>& customUniformBlocks)
: mController(controller),
  mGl(gl),
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

  mDefaultUniformBlock.name          = "";
  mDefaultUniformBlock.members       = {};
  mDefaultUniformBlock.binding       = 0;
  mDefaultUniformBlock.descriptorSet = 0;
  mDefaultUniformBlock.members.clear();

  int offset = 0;
  for(const auto& data : UNIFORMS)
  {
    mDefaultUniformBlock.members.emplace_back();
    auto& item   = mDefaultUniformBlock.members.back();
    item.name    = data.name;
    item.binding = 0;
    item.offsets.push_back(offset);
    item.locations.push_back(gl.GetUniformLocation(programId, data.name.c_str()));
    item.bufferIndex  = 0;
    item.uniformClass = Graphics::UniformClass::UNIFORM;
    item.type         = data.type;
    offset += GetSizeForType(data.type);
  }

  for(const auto& data : mCustomUniforms)
  {
    fprintf(stderr, "\ncustom uniforms: %s\n", data.name.c_str());

    auto iter        = data.name.find("[", 0);
    int  numElements = 1;
    if(iter != std::string::npos)
    {
      auto baseName = data.name.substr(0, iter);
      iter++;
      numElements = std::stoi(data.name.substr(iter));
      if(numElements == 0)
      {
        numElements = 1;
      }
      iter = data.name.find("]");
      std::string suffix;
      if(iter != std::string::npos && iter + 1 != data.name.length())
      {
        suffix = data.name.substr(iter + 1); // If there is a suffix, it means it is an element of an array of struct
      }

      if(!suffix.empty())
      {
        // Write multiple items
        for(int i = 0; i < numElements; ++i)
        {
          std::stringstream elementNameStream;
          elementNameStream << baseName << "[" << i << "]" << suffix;
          mDefaultUniformBlock.members.emplace_back();
          auto& item   = mDefaultUniformBlock.members.back();
          item.name    = elementNameStream.str();
          item.binding = 0;
          item.offsets.push_back(offset);
          item.locations.push_back(gl.GetUniformLocation(programId, elementNameStream.str().c_str()));
          item.bufferIndex  = 0;
          item.uniformClass = Graphics::UniformClass::UNIFORM;
          item.type         = data.type;
          offset += GetSizeForType(data.type);
        }
      }
      else
      {
        // Write 1 item with multiple elements
        mDefaultUniformBlock.members.emplace_back();
        auto& item = mDefaultUniformBlock.members.back();

        item.name         = baseName;
        item.binding      = 0;
        item.bufferIndex  = 0;
        item.uniformClass = Graphics::UniformClass::UNIFORM;
        item.type         = data.type;
        item.numElements  = numElements;

        for(int i = 0; i < numElements; ++i)
        {
          std::stringstream elementNameStream;
          elementNameStream << baseName << "[" << i << "]";
          item.locations.push_back(gl.GetUniformLocation(programId, elementNameStream.str().c_str()));
          item.offsets.push_back(offset);
          offset += GetSizeForType(data.type);
        }
      }
    }
    else
    {
      // Write 1 item with 1 element
      mDefaultUniformBlock.members.emplace_back();
      auto& item   = mDefaultUniformBlock.members.back();
      item.name    = data.name;
      item.binding = 0;
      item.offsets.push_back(offset);
      item.locations.push_back(gl.GetUniformLocation(programId, item.name.c_str()));
      item.bufferIndex  = 0;
      item.uniformClass = Graphics::UniformClass::UNIFORM;
      item.type         = data.type;
      offset += GetSizeForType(data.type);
    }
  }
  mDefaultUniformBlock.size = offset;

  mUniformBlocks.push_back(mDefaultUniformBlock);
  for(auto& element : customUniformBlocks)
  {
    mUniformBlocks.push_back(element);
  }
}

uint32_t TestGraphicsReflection::GetVertexAttributeLocation(const std::string& name) const
{
  auto iter = std::find(mAttributes.begin(), mAttributes.end(), name);
  if(iter != mAttributes.end())
  {
    return iter - mAttributes.begin();
  }
  else if(mController.AutoAttrCreation())
  {
    uint32_t location = mAttributes.size();
    mAttributes.push_back(name);
    return location;
  }

  return -1;
}

Dali::Graphics::VertexInputAttributeFormat TestGraphicsReflection::GetVertexAttributeFormat(uint32_t location) const
{
  tet_infoline("Warning, TestGraphicsReflection::GetVertexAttributeFormat is unimplemented\n");
  return Dali::Graphics::VertexInputAttributeFormat{};
}

std::string TestGraphicsReflection::GetVertexAttributeName(uint32_t location) const
{
  tet_infoline("Warning, TestGraphicsReflection::GetVertexAttributeName is unimplemented\n");
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
  if(index >= mUniformBlocks.size())
  {
    return 0;
  }
  return mUniformBlocks[index].binding;
}

uint32_t TestGraphicsReflection::GetUniformBlockSize(uint32_t index) const
{
  if(index >= mUniformBlocks.size())
  {
    return 0;
  }

  const auto& block = mUniformBlocks[index];
  return block.size;
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
    out.members[i].offset       = memberUniform.offsets[0];
    out.members[i].location     = memberUniform.locations[0];
  }

  return true;
}

std::vector<uint32_t> TestGraphicsReflection::GetUniformBlockLocations() const
{
  tet_infoline("Warning, TestGraphicsReflection::GetUniformBlockLocations is unimplemented\n");
  return std::vector<uint32_t>{};
}

std::string TestGraphicsReflection::GetUniformBlockName(uint32_t blockIndex) const
{
  tet_infoline("Warning, TestGraphicsReflection::GetUniformBlockName is unimplemented\n");
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
    return mUniformBlocks[blockIndex].members[memberLocation].offsets[0];
  }
  else
  {
    return 0u;
  }
}

bool TestGraphicsReflection::GetNamedUniform(const std::string& name, Dali::Graphics::UniformInfo& out) const
{
  tet_infoline("Warning, TestGraphicsReflection::GetNamedUniform is unimplemented\n");
  return true;
}

const std::vector<Dali::Graphics::UniformInfo>& TestGraphicsReflection::GetSamplers() const
{
  tet_infoline("Warning, TestGraphicsReflection::GetSamplers is unimplemented\n");
  static std::vector<Dali::Graphics::UniformInfo> samplers{};
  return samplers;
}

Graphics::ShaderLanguage TestGraphicsReflection::GetLanguage() const
{
  return Graphics::ShaderLanguage::GLSL_3_1;
}

} // namespace Dali
