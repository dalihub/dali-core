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

#include <dali/graphics-api/utility/utility-memory-pool.h>

#include <dali/graphics/vulkan/api/vulkan-api-render-command.h>

#include <dali/graphics/vulkan/api/vulkan-api-shader.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline.h>
#include <dali/graphics/vulkan/api/vulkan-api-sampler.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/internal/spirv/vulkan-spirv.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>

#include <dali/graphics/vulkan/internal/vulkan-swapchain.h>

#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>


namespace Dali
{
namespace Graphics
{
using Vulkan::VkAssert;

namespace VulkanAPI
{

RenderCommand::~RenderCommand() = default;

RenderCommand::RenderCommand( VulkanAPI::Controller& controller, Vulkan::Graphics& graphics )
: mController( controller ),
  mGraphics( graphics )
{
}


///@todo: needs pipeline factory rather than pipeline creation in place!!!
void RenderCommand::PrepareResources()
{
  if( mUpdateFlags )
  {
    if( mUpdateFlags & API::RENDER_COMMAND_UPDATE_PIPELINE_BIT )
    {
      auto pipeline = static_cast<const VulkanAPI::Pipeline *>( mPipeline );
      if( !pipeline )
      {
        return;
      }
      mVulkanPipeline = pipeline->GetVkPipeline();
      mDescriptorSets.clear();
      mDescriptorSets = mGraphics.AllocateDescriptorSets( pipeline->GetDescriptorSetLayoutSignatures(), pipeline->GetVkDescriptorSetLayouts() );

      // rebind data in case descriptor sets changed
      mUpdateFlags |= API::RENDER_COMMAND_UPDATE_UNIFORM_BUFFER_BIT;
      mUpdateFlags |= API::RENDER_COMMAND_UPDATE_TEXTURE_BIT;
    }

    if( mUpdateFlags & (API::RENDER_COMMAND_UPDATE_UNIFORM_BUFFER_BIT ))
    {
      mUBONeedsBinding = true;
    }

    BindUniformBuffers();

    if( mUpdateFlags & ( API::RENDER_COMMAND_UPDATE_TEXTURE_BIT|API::RENDER_COMMAND_UPDATE_SAMPLER_BIT) )
    {
      BindTexturesAndSamplers();
    }

    mUpdateFlags = 0u;
  }
}

void RenderCommand::BindUniformBuffers()
{
  if( !mUniformBufferBindings || !mUBONeedsBinding )
  {
    return;
  }

  for( auto i = 0u; i < mUniformBufferBindings->size(); ++i )
  {
    mController.mStats.uniformBufferBindings++;
    const auto& binding = (*mUniformBufferBindings)[i];

    auto bufferInfo = vk::DescriptorBufferInfo{}
      .setOffset( uint32_t( binding.offset ) )
      .setRange( uint32_t( binding.dataSize ) )
      .setBuffer( static_cast<const VulkanAPI::Buffer*>(binding.buffer)->GetBufferRef()->GetVkHandle() );

    mController.PushDescriptorWrite(
      vk::WriteDescriptorSet{}.setPBufferInfo( &bufferInfo )
                              .setDescriptorType( vk::DescriptorType::eUniformBuffer )
                              .setDescriptorCount( 1 )
                              .setDstSet( mDescriptorSets[0]->GetVkDescriptorSet() )
                              .setDstBinding( binding.binding )
                              .setDstArrayElement( 0 ) );
  }

  mUBONeedsBinding = false;
}


void RenderCommand::BindPipeline( Vulkan::RefCountedCommandBuffer& commandBuffer ) const
{
  if( mPipeline )
  {
    auto pipeline = static_cast<const VulkanAPI::Pipeline *>( mPipeline );
    pipeline->Bind( commandBuffer );
  }
}

void RenderCommand::BindTexturesAndSamplers()
{
  // only if textures/samplers changed, rewrite
  if( !mTextureBindings )
  {
    std::cout << "[RenderCommand] BindingTextureSampler: No textures" << std::endl;
    return;
  }
  for( const auto& texture : *mTextureBindings )
  {
    auto image = static_cast<const VulkanAPI::Texture*>(texture.texture);

    // test if image is valid, skip invalid image
    // @todo: possibly use builtin 'broken' image
    if( !image || !image->GetImageRef() )
    {
      continue;
    }
    mController.mStats.samplerTextureBindings++;
    //DALI_LOG_STREAM( gVulkanFilter, Debug::General,
    std::cout << "[RenderCommand] BindingTextureSampler: binding = " << texture.binding << std::endl;


    auto imageViewInfo = vk::DescriptorImageInfo{}
      .setImageLayout( vk::ImageLayout::eGeneral )
      .setImageView( image->GetImageViewRef()->GetVkHandle() )
      .setSampler(!texture.sampler ?
                  image->GetSamplerRef()->GetVkHandle() :
                  static_cast<const VulkanAPI::Sampler*>( texture.sampler )->GetSampler()->GetVkHandle() );

    mController.PushDescriptorWrite(
      vk::WriteDescriptorSet{}.setPImageInfo( &imageViewInfo )
                              .setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
                              .setDescriptorCount( 1 )
                              .setDstSet( mDescriptorSets[0]->GetVkDescriptorSet() )
                              .setDstBinding( texture.binding )
                              .setDstArrayElement( 0 ) );
  }
}

const std::vector< Vulkan::RefCountedDescriptorSet >& RenderCommand::GetDescriptorSets() const
{
  return mDescriptorSets;
}

const vk::Pipeline& RenderCommand::GetVulkanPipeline() const
{
  return mVulkanPipeline;
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
