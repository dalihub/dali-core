/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 *
 */

#include <dali/graphics/vulkan/api/vulkan-api-shader.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
using namespace Dali::Graphics::API;
using namespace Dali::Graphics::Vulkan;

using ShaderLanguage = Dali::Graphics::API::ShaderDetails::Language;
using ShaderPipelineStage = Dali::Graphics::API::ShaderDetails::PipelineStage;
using ShaderSource = Dali::Graphics::API::ShaderDetails::ShaderSource;

Shader::Shader( Vulkan::Graphics& graphics ) :
mGraphics( graphics )
{

}

ShaderRef Shader::GetShader( vk::ShaderStageFlagBits shaderStage ) const
{
  switch( shaderStage )
  {
    case vk::ShaderStageFlagBits::eVertex:
    {
      return mVertexShader;
    }
    case vk::ShaderStageFlagBits::eFragment:
    {
      return mFragmentShader;
    }
    case vk::ShaderStageFlagBits::eTessellationControl:
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
    case vk::ShaderStageFlagBits::eGeometry:
    case vk::ShaderStageFlagBits::eCompute:
    case vk::ShaderStageFlagBits::eAllGraphics:
    case vk::ShaderStageFlagBits::eAll:
    {
    }
  }
  return ShaderRef();
}

Shader& Shader::DownCast( Dali::Graphics::API::Shader& shader )
{
  return *( dynamic_cast<Shader*>( &shader ) );
}

Vulkan::ShaderRef Shader::GetShaderRef( vk::ShaderStageFlagBits shaderStage ) const
{
  if(shaderStage == vk::ShaderStageFlagBits::eVertex)
  {
    return mVertexShader;
  }
  else if(shaderStage == vk::ShaderStageFlagBits::eFragment)
  {
    return mFragmentShader;
  }
  else
  {
    return Vulkan::ShaderRef( );
  }
}

bool Shader::AddShaderModule( Dali::Graphics::API::ShaderDetails::PipelineStage pipelineStage,
                              Dali::Graphics::API::ShaderDetails::Language language,
                              Dali::Graphics::API::ShaderDetails::ShaderSource shaderSource )

{
  // TODO: AB: only supported language is SPIRV for now
  if( language != ShaderLanguage::SPIRV_1_0 && language != ShaderLanguage::SPIRV_1_1 )
  {
    // unsupported language
    return false;
  }

  // TODO: AB: only binary shader supported for now
  if( shaderSource.type != ShaderDetails::ShaderSourceType::BINARY )
  {
    return false;
  }

  auto shaderRef = Vulkan::Shader::New( mGraphics, &shaderSource.code[0], shaderSource.code.size() );

  if(!shaderRef)
  {
    return false;
  }

  if(pipelineStage == ShaderPipelineStage::VERTEX )
  {
    mVertexShader = shaderRef;
    mVertexShader->SetExplicitShaderStage( vk::ShaderStageFlagBits::eVertex );
    // update input attributes
    mVertexShader->GetSPIRVReflection().GetVertexInputAttributes( mVertexInputAttributes );
  }
  else if(pipelineStage == ShaderPipelineStage::FRAGMENT )
  {
    mFragmentShader = shaderRef;
    mVertexShader->SetExplicitShaderStage( vk::ShaderStageFlagBits::eFragment );
  }

  mUniformBlocks.clear();
  mUniformOpaques.clear();

  // recreate reflection
  if( mVertexShader )
  {
    auto& ubo = mVertexShader->GetSPIRVReflection().GetUniformBlocks();
    mUniformBlocks.insert(mUniformBlocks.end(), ubo.begin(), ubo.end() );

    auto& uniforms = mVertexShader->GetSPIRVReflection().GetOpaqueUniforms();
    mUniformOpaques.insert(mUniformOpaques.end(), uniforms.begin(), uniforms.end() );
  }
  if( mFragmentShader )
  {
    auto& ubo = mFragmentShader->GetSPIRVReflection().GetUniformBlocks();
    mUniformBlocks.insert(mUniformBlocks.end(), ubo.begin(), ubo.end() );

    auto& uniforms = mFragmentShader->GetSPIRVReflection().GetOpaqueUniforms();
    mUniformOpaques.insert(mUniformOpaques.end(), uniforms.begin(), uniforms.end() );
  }

  // verify bindings and descriptor sets ( must be no overlaps, or if there are, only one binding will be used )
  return true;
}

bool Shader::IsReflectionSupported() const
{
  return true;
}

uint32_t Shader::GetVertexAttributeLocation(const std::string& name) const
{
  if( !mVertexShader )
  {
    return API::ShaderDetails::ERROR_VERTEX_INPUT_ATTRIBUTE_NOT_FOUND;
  }

  for( auto&& attr : mVertexInputAttributes )
  {
    if( attr.name == name )
    {
      return attr.location;
    }
  }
  return API::ShaderDetails::ERROR_VERTEX_INPUT_ATTRIBUTE_NOT_FOUND;
}

API::ShaderDetails::VertexInputAttributeFormat Shader::GetVertexAttributeFormat( uint32_t location ) const
{
  if( !mVertexShader || mVertexInputAttributes.size() <= location )
  {
    return API::ShaderDetails::VertexInputAttributeFormat::UNDEFINED;
  }

  const auto& attr = mVertexInputAttributes[location];

  if( attr.format == vk::Format::eR32Sfloat )
  {
    return API::ShaderDetails::VertexInputAttributeFormat::FLOAT;
  }
  if( attr.format == vk::Format::eR32Sint )
  {
    return API::ShaderDetails::VertexInputAttributeFormat::INTEGER;
  }
  if( attr.format == vk::Format::eR32G32Sfloat )
  {
    return API::ShaderDetails::VertexInputAttributeFormat::VEC2;
  }
  if( attr.format == vk::Format::eR32G32B32Sfloat )
  {
    return API::ShaderDetails::VertexInputAttributeFormat::VEC3;
  }
  if( attr.format == vk::Format::eR32G32B32A32Sfloat )
  {
    return API::ShaderDetails::VertexInputAttributeFormat::VEC4;
  }
  return API::ShaderDetails::VertexInputAttributeFormat::UNDEFINED;
}

std::string Shader::GetVertexAttributeName(uint32_t location) const
{
  if( !mVertexShader || mVertexInputAttributes.size() <= location )
  {
    return "";
  }

  return mVertexInputAttributes[location].name;
}

std::vector<uint32_t> Shader::GetVertexAttributeLocations() const
{
  std::vector<uint32_t> locations;
  for( auto&& attr : mVertexInputAttributes )
  {
    if( attr.format != vk::Format::eUndefined )
    {
      locations.push_back( attr.location );
    }
  }

  return locations;
}

std::vector<uint32_t> Shader::GetUniformBlockLocations() const
{
  std::vector<uint32_t> retval{};
  for( auto&& ubo : mUniformBlocks )
  {
    retval.emplace_back( ubo.binding );
  }
  return retval;
}

std::string Shader::GetUniformBlockName( uint32_t blockIndex ) const
{
  return mUniformBlocks[blockIndex].name;
}

uint32_t Shader::GetUniformBlockMemberCount( uint32_t blockIndex ) const
{
  return U32(mUniformBlocks[blockIndex].members.size());
}

std::string Shader::GetUniformBlockMemberName( uint32_t blockIndex, uint32_t memberLocation ) const
{
  return mUniformBlocks[blockIndex].members[memberLocation].name;
}

uint32_t Shader::GetUniformBlockMemberOffset( uint32_t blockIndex, uint32_t memberLocation ) const
{
  return mUniformBlocks[blockIndex].members[memberLocation].offset;
}

bool Shader::GetNamedUniform( const std::string& name, API::ShaderDetails::UniformInfo& out ) const
{
  // check uniform blocks first
  auto index = 0u;
  for( auto&& ubo : mUniformBlocks )
  {
    for( auto&& member : ubo.members )
    {
      if( name == member.name || name == (ubo.name + "." + member.name) )
      {
        out.name = name;
        out.location = member.location;
        out.binding = ubo.binding;
        out.bufferIndex = index;
        out.offset = member.offset;
        out.uniformClass = ShaderDetails::UniformClass::UNIFORM_BUFFER;
        return true;
      }
    }
    index++;
  }

  // check samplers
  for( auto&& uniform : mUniformOpaques )
  {
    if( uniform.name == name )
    {
      out.uniformClass = ShaderDetails::UniformClass::COMBINED_IMAGE_SAMPLER;
      out.binding = uniform.binding;
      out.name = name;
      out.offset = 0;
      out.location = 0;
      return true;
    }
  }
  return false;
}

std::vector<API::ShaderDetails::UniformInfo> Shader::GetSamplers() const
{
  std::vector<API::ShaderDetails::UniformInfo> retval;
  for( auto&& uniform : mUniformOpaques )
  {
    if(uniform.type == vk::DescriptorType::eCombinedImageSampler )
    {
      API::ShaderDetails::UniformInfo info;
      info.location = 0u;
      info.binding = uniform.binding;
      info.offset = 0;
      info.name = uniform.name;
      info.uniformClass = API::ShaderDetails::UniformClass::COMBINED_IMAGE_SAMPLER;
      retval.emplace_back( info );
    }
  }
  return retval;
}

uint32_t Shader::GetUniformBlockCount() const
{
  return U32(mUniformBlocks.size());
}

bool Shader::GetUniformBlock( uint32_t index, API::ShaderDetails::UniformBlockInfo& out ) const
{
  if( index >= mUniformBlocks.size() )
  {
    return false;
  }

  const auto& block = mUniformBlocks[index];

  out.name = block.name;
  out.binding = block.binding;
  out.descriptorSet = block.descriptorSet;
  auto membersSize = block.members.size();
  out.members.resize( membersSize );
  out.size = block.size;
  for( auto i = 0u; i < out.members.size(); ++i )
  {
    const auto& memberUniform = block.members[i];
    out.members[i].name = memberUniform.name;
    out.members[i].binding = block.binding;
    out.members[i].uniformClass = API::ShaderDetails::UniformClass::UNIFORM_BUFFER;
    out.members[i].offset = memberUniform.offset;
    out.members[i].location = memberUniform.location;
  }

  return true;
}


} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali