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

#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>
#include <iostream>
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/*
 * Class: Shader::Impl
 */
struct Shader::Impl
{
  Impl( Shader& owner, Graphics& graphics, const vk::ShaderModuleCreateInfo& info ) :
    mOwner( owner ),
    mGraphics( graphics ),
    mCreateInfo( info ),
    mPipelineShaderStage( vk::ShaderStageFlagBits::eAllGraphics )
  {
    mSPIRVShader = SpirV::SPIRVUtils::Parse( info.pCode, info.codeSize, vk::ShaderStageFlagBits::eVertex );
  }

  ~Impl()
  {
    if(mShaderModule)
    {
      mGraphics.GetDevice().destroyShaderModule( mShaderModule, mGraphics.GetAllocator() );
    }
  }

  vk::Result Initialise()
  {
    if( (mShaderModule = VkAssert( mGraphics.GetDevice().createShaderModule( mCreateInfo, mGraphics.GetAllocator() ) )))
    {
      return vk::Result::eSuccess;
    }
    return vk::Result::eErrorInitializationFailed;
  }

  vk::ShaderModule GetVkHandle() const
  {
    return mShaderModule;
  }

  Shader& mOwner;
  Graphics& mGraphics;
  vk::ShaderModuleCreateInfo mCreateInfo;
  vk::ShaderModule mShaderModule;
  std::unique_ptr<SpirV::SPIRVShader> mSPIRVShader;
  vk::ShaderStageFlagBits mPipelineShaderStage;
};

/*
 * Class: Shader
 */
Handle<Shader> Shader::New(Graphics &graphics, const vk::ShaderModuleCreateInfo &info)
{
  auto shader = Handle<Shader>(new Shader(graphics, info)); // can't use make unique because of permissions
  if(shader)
  {
    if( shader->mImpl->Initialise() == vk::Result::eSuccess )
    {
      graphics.AddShader( shader );
    }
  }
  return shader;
}

Handle<Shader> Shader::New( Graphics& graphics, const void* bytes, std::size_t size )
{
  return New( graphics, vk::ShaderModuleCreateInfo{}
                                .setCodeSize( size )
                                .setPCode(reinterpret_cast<const uint32_t*>(bytes)));
}

Shader::Shader( Graphics& graphics, const vk::ShaderModuleCreateInfo& info )
{
  mImpl = MakeUnique<Impl>( *this, graphics, info );
}

Shader::~Shader() = default;

vk::ShaderModule Shader::GetVkHandle() const
{
  return mImpl->GetVkHandle();
}

bool Shader::OnDestroy()
{
  if( !mImpl->mGraphics.IsShuttingDown() )
  {
    mImpl->mGraphics.RemoveShader( *this );
  }

  auto device = mImpl->mGraphics.GetDevice();
  auto shaderModule = mImpl->mShaderModule;
  auto allocator = &mImpl->mGraphics.GetAllocator();

  mImpl->mGraphics.DiscardResource( [device, shaderModule, allocator]() {
#ifndef NDEBUG
    printf("Invoking SHADER MODULE deleter function\n");
#endif
    device.destroyShaderModule( shaderModule, allocator );
  } );

  return true;
}

const SpirV::SPIRVShader& Shader::GetSPIRVReflection() const
{
  return *mImpl->mSPIRVShader;
}

void Shader::SetExplicitShaderStage( vk::ShaderStageFlagBits shaderStage )
{
  mImpl->mPipelineShaderStage = shaderStage;
}

vk::ShaderStageFlagBits Shader::GetExplicitShaderStage() const
{
  return mImpl->mPipelineShaderStage;
}


}
}
}