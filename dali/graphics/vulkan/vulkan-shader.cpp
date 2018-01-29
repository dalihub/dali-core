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
  Impl( Graphics& graphics, const vk::ShaderModuleCreateInfo& info ) :
    mGraphics( graphics ),
    mCreateInfo( info )
  {
  }

  ~Impl()
  {
    if(mDSLayouts.size())
    {
      for(auto&& ds : mDSLayouts )
      {
        mGraphics.GetDevice().destroyDescriptorSetLayout( ds, mGraphics.GetAllocator() );
      }
    }
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

  Graphics& mGraphics;
  vk::ShaderModuleCreateInfo mCreateInfo;
  vk::ShaderModule mShaderModule;

  // creates new descriptor set layout
  // TODO: should be read from the shader and any manual setting should not
  // take place after we have proper reflection!
  void SetDescriptorSetLayout( uint32_t set, vk::DescriptorSetLayoutCreateInfo info )
  {
    auto layout = VkAssert( mGraphics.GetDevice().createDescriptorSetLayout( info, mGraphics.GetAllocator() ) );
    if( mDSLayouts.size() >= set )
    {
      mDSLayouts.resize( set+1 );
    }
    else if ( mDSLayouts[set] ) // already existing set ( this is error but for now handle it )
    {
      mGraphics.GetDevice().destroyDescriptorSetLayout( mDSLayouts[set], mGraphics.GetAllocator() );
    }
    mDSLayouts[set] = layout;
  }

  const std::vector<vk::DescriptorSetLayout>& GetDescriptorSetLayouts() const
  {
    return mDSLayouts;
  }

  std::vector<vk::DescriptorSetLayout> mDSLayouts; // descriptorset layouts

};

/*
 * Class: Shader
 */

Handle<Shader> Shader::New(Graphics &graphics, const vk::ShaderModuleCreateInfo &info)
{
  auto shader = std::unique_ptr<Shader>(new Shader(graphics, info)); // can't use make unique because of permissions
  if(shader)
  {
    if( shader->mImpl->Initialise() == vk::Result::eSuccess )
    {
      auto retval = Handle<Shader>( shader.get() );
      graphics.AddShader( std::move(shader) );
      return retval;
    }
  }
  return Handle<Shader>();
}

Handle<Shader> Shader::New( Graphics& graphics, const void* bytes, std::size_t size )
{
  return New( graphics, vk::ShaderModuleCreateInfo{}
                                .setCodeSize( size )
                                .setPCode(reinterpret_cast<const uint32_t*>(bytes)));
}

Shader::Shader( Graphics& graphics, const vk::ShaderModuleCreateInfo& info )
{
  mImpl = MakeUnique<Impl>( graphics, info );
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

void Shader::SetDescriptorSetLayout( uint32_t set, vk::DescriptorSetLayoutCreateInfo info )
{
  mImpl->SetDescriptorSetLayout( set, info );
}

const std::vector<vk::DescriptorSetLayout>& Shader::GetDescriptorSetLayouts() const
{
  return mImpl->GetDescriptorSetLayouts();
}
}
}
}