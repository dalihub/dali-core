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
    mCreateInfo( info )
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

  vk::ShaderModule GetVkShaderModule() const
  {
    return mShaderModule;
  }

  Shader& mOwner;
  Graphics& mGraphics;
  vk::ShaderModuleCreateInfo mCreateInfo;
  vk::ShaderModule mShaderModule;
  std::unique_ptr<SpirV::SPIRVShader> mSPIRVShader;
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

vk::ShaderModule Shader::GetVkShaderModule() const
{
  return mImpl->GetVkShaderModule();
}

bool Shader::OnDestroy()
{
  mImpl->mGraphics.RemoveShader( *this );
  return true;
}

const SpirV::SPIRVShader& Shader::GetSPIRVReflection() const
{
  return *mImpl->mSPIRVShader;
}

}
}
}