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
#include <dali/graphics/vulkan/api/internal/vulkan-api-descriptor-set-allocator.h>

#define logstream() bb.log() << "F:" << mController.GetFrameCount() << ": "

namespace Dali
{
namespace Graphics
{
using Vulkan::VkAssert;

namespace VulkanAPI
{

struct RenderCommand::DescriptorSetData
{
  std::vector< Vulkan::DescriptorSetLayoutSignature > descriptorSetLayoutSignatures;
  std::vector< vk::DescriptorSetLayout >              descriptorSetLayouts;
  std::vector< DescriptorSetRequirements >            descriptorSetRequirements;
  DescriptorSetList                                   vkDescriptorSets;
};

RenderCommand::~RenderCommand()
{
}

void RenderCommand::DiscardDescriptorSets()
{
  if( !mData->vkDescriptorSets.descriptorSets.empty() )
  {
    mController.FreeDescriptorSets( std::move( mData->vkDescriptorSets ) );
  }
}

RenderCommand::RenderCommand( VulkanAPI::Controller& controller, Vulkan::Graphics& graphics )
: mController( controller ),
  mGraphics( graphics ),
  mData( new DescriptorSetData() )
{
  logstream() << "RenderCommand::Ctor" << bb.end();
}

void RenderCommand::PrepareResources()
{
  logstream() << "RenderCommand::PrepareResources()" << bb.end();
  if( mUpdateFlags )
  {
    if( mUpdateFlags & (API::RENDER_COMMAND_UPDATE_UNIFORM_BUFFER_BIT ))
    {
      BindUniformBuffers();
    }

    if( mUpdateFlags & ( API::RENDER_COMMAND_UPDATE_TEXTURE_BIT|API::RENDER_COMMAND_UPDATE_SAMPLER_BIT) )
    {
      BindTexturesAndSamplers();
    }

    mUpdateFlags = 0u;
  }
}

void RenderCommand::AllocateDescriptorSets( VulkanAPI::Internal::DescriptorSetAllocator& dsAllocator )
{
  // allocate descriptor
  bb.push();
  if( dsAllocator.AllocateDescriptorSets( mData->descriptorSetLayoutSignatures, mData->descriptorSetLayouts, mData->vkDescriptorSets ) )
  {
    logstream() << "New descriptors allocated, expecting to rebind" << bb.end();

    mUpdateFlags |= API::RENDER_COMMAND_UPDATE_UNIFORM_BUFFER_BIT;
    mUpdateFlags |= API::RENDER_COMMAND_UPDATE_TEXTURE_BIT;
  }
  else
  {
    logstream() << "No need to allocate descriptor sets" << bb.end();
  }
  bb.pop();
}

void RenderCommand::UpdateDescriptorSetAllocationRequirements( std::vector<DescriptorSetRequirements>& requirements )
{
  //@ todo: only when pipeline has changed!
  BuildDescriptorSetRequirements();
  if( mData->descriptorSetRequirements.empty() ) // shouldn't be empty
  {
    logstream() << "No descriptor sets found!" << bb.end();
    return;
  }
  logstream() << "Updating requirements" << bb.end();

  // If pipeline changed we need to free descriptor sets and allocate new ones.
  // Destroying old descriptors must happen before allocating new ones
  if( mUpdateFlags & API::RENDER_COMMAND_UPDATE_PIPELINE_BIT )
  {
    logstream() << "Updating requirements: pipeline has changed" << bb.end();
    auto pipeline = static_cast<const VulkanAPI::Pipeline *>( mPipeline );
    if( !pipeline )
    {
      return;
    }
    mVulkanPipeline = pipeline->GetVkPipeline();
    mData->descriptorSetLayoutSignatures = pipeline->GetDescriptorSetLayoutSignatures();
    mData->descriptorSetLayouts = pipeline->GetVkDescriptorSetLayouts();

    // free descriptor sets for that pool
    if( !mData->vkDescriptorSets.descriptorSets.empty() )
    {
      logstream() << "Updating requirements: erasing descriptor sets" << bb.end();
      mController.FreeDescriptorSets( std::move( mData->vkDescriptorSets ) );
      mData->vkDescriptorSets = {};
    }
  }

  for( auto& requirement : mData->descriptorSetRequirements )
  {
    auto it = std::find_if( requirements.begin(),
                  requirements.end(),
                  [&]( DescriptorSetRequirements& item )->bool {
                    if( item.layoutSignature == requirement.layoutSignature )
                    {
                      for( auto i = 0u; i < uint32_t(Vulkan::DescriptorType::DESCRIPTOR_TYPE_COUNT); ++i )
                      {
                        item.requirements[i] += requirement.requirements[i];
                      }
                      item.maxSets += requirement.maxSets;
                      return true;
                    }
                    return false;
                  } );
    if( it == requirements.end() )
    {
      requirements.emplace_back( requirement );
      it = requirements.end()-1;
    }

    if( mData->vkDescriptorSets.descriptorSets.empty() )
    {
      (*it).unallocatedSets += uint32_t( mData->descriptorSetLayouts.size() );
    }
  }

}

void RenderCommand::BuildDescriptorSetRequirements()
{
  if( !(mUpdateFlags & API::RENDER_COMMAND_UPDATE_PIPELINE_BIT) )
  {
    logstream() << "Pipeline hasn't changed" << bb.end();
  }
  //@ todo: only when pipeline has changed!
  // For new pipeline build descriptor set requirements array
  auto pipeline = static_cast<const VulkanAPI::Pipeline *>( mPipeline );
  if( !pipeline )
  {
    return;
  }

  mData->descriptorSetLayoutSignatures = pipeline->GetDescriptorSetLayoutSignatures();
  mData->descriptorSetLayouts = pipeline->GetVkDescriptorSetLayouts();
  mData->descriptorSetRequirements = {};

  for( auto& signature : mData->descriptorSetLayoutSignatures )
  {
    // See whether we have this signature in
    auto it = std::find_if( mData->descriptorSetRequirements.begin(),
                            mData->descriptorSetRequirements.end(),
                            [&]( auto& item )->bool { return item.layoutSignature == signature; } );

    if( it == mData->descriptorSetRequirements.end() )
    {
      mData->descriptorSetRequirements.emplace_back();
      it = mData->descriptorSetRequirements.end()-1;
    }

    it->layoutSignature = signature;
    auto decoded = signature.Decode();
    for( auto& descriptorSet : decoded )
    {
      auto requirementIndex = uint32_t(std::get<0>(descriptorSet));
      auto descriptorCount = uint32_t(std::get<1>(descriptorSet));
      it->requirements[ requirementIndex ] += descriptorCount;
    }

    // increments maximum number of sets
    it->maxSets++;
  }

}

void RenderCommand::UpdateDescriptorSets( bool force )
{
}

void RenderCommand::BindUniformBuffers()
{
  if( !mUniformBufferBindings )
  {
    return;
  }

  logstream() << "Binding uniform buffers" << bb.end();
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
                              .setDstSet( mData->vkDescriptorSets.descriptorSets[0] )
                              .setDstBinding( binding.binding )
                              .setDstArrayElement( 0 ) );
  }
}


void RenderCommand::BindPipeline( Vulkan::RefCountedCommandBuffer& commandBuffer )
{
  if( mPipeline )
  {
    logstream() << "Binding pipeline"  << bb.end();
    auto pipeline = static_cast<const VulkanAPI::Pipeline *>( mPipeline );
    pipeline->Bind( commandBuffer );
  }
}

void RenderCommand::BindTexturesAndSamplers()
{
  // only if textures/samplers changed, rewrite
  if( !mTextureBindings )
  {
    return;
  }
  logstream() << "Binding textures" << bb.end();
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
    DALI_LOG_STREAM( gVulkanFilter, Debug::General,
                     "[RenderCommand] BindingTextureSampler: binding = " << texture.binding );


    auto imageViewInfo = vk::DescriptorImageInfo{}
      .setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
      .setImageView( image->GetImageViewRef()->GetVkHandle() )
      .setSampler(!texture.sampler ?
                  image->GetSamplerRef()->GetVkHandle() :
                  static_cast<const VulkanAPI::Sampler*>( texture.sampler )->GetSampler()->GetVkHandle() );

    mController.PushDescriptorWrite(
      vk::WriteDescriptorSet{}.setPImageInfo( &imageViewInfo )
                              .setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
                              .setDescriptorCount( 1 )
                              .setDstSet( mData->vkDescriptorSets.descriptorSets[0] )
                              .setDstBinding( texture.binding )
                              .setDstArrayElement( 0 ) );
  }
}

/*
const std::vector< Vulkan::RefCountedDescriptorSet >& RenderCommand::GetDescriptorSets() const
{
  return mDescriptorSets;
}
*/
const std::vector< vk::DescriptorSet >& RenderCommand::GetDescriptorSets()
{
  logstream() << "Testing validty" << bb.end();
  std::vector<bool> results;
  if( !mController.TestDescriptorSetsValid( mData->vkDescriptorSets, results ) )
  {
    logstream() << "Invalid descriptor set = " << uintptr_t(static_cast<VkDescriptorSet>(mData->vkDescriptorSets.descriptorSets[0])) << bb.end();
    logstream() << "Damn" << bb.end();
    puts("What would Adeel say? Oh shit!");
  }
  return mData->vkDescriptorSets.descriptorSets;
}


const vk::Pipeline& RenderCommand::GetVulkanPipeline() const
{
  return mVulkanPipeline;
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
