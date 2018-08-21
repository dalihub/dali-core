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
#include <dali/graphics/vulkan/internal/spirv/vulkan-spirv.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-pipeline.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>

#include <dali/graphics/vulkan/internal/vulkan-swapchain.h>

#include <dali/graphics/vulkan/api/internal/vulkan-ubo-manager.h>
#include <dali/graphics/vulkan/api/internal/vulkan-ubo-pool.h>
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
        : mController( controller ), mGraphics( graphics ), mCommandBuffer()
{
}

std::mutex mutex;

///@todo: needs pipeline factory rather than pipeline creation in place!!!
void RenderCommand::PrepareResources( std::vector< vk::WriteDescriptorSet >& descriptorWrites )
{
  if( mUpdateFlags )
  {
    if( mUpdateFlags & API::RENDER_COMMAND_UPDATE_PIPELINE_BIT )
    {
      auto pipeline = dynamic_cast<const VulkanAPI::Pipeline *>( mPipeline );
      if( !pipeline )
      {
        return;
      }
      mVkDescriptorSetLayouts.clear();
      mVkDescriptorSetLayouts = pipeline->GetVkDescriptorSetLayouts();
      auto dsLayoutSignatures = pipeline->GetDescriptorSetLayoutSignatures();
      mVulkanPipeline         = pipeline->GetVkPipeline();

      if( mDescriptorSets.empty() )
      {
        mDescriptorSets = mGraphics.AllocateDescriptorSets( dsLayoutSignatures, mVkDescriptorSetLayouts );
      }

      {
        std::lock_guard< std::mutex > lock( mutex );
        AllocateUniformBufferMemory();
      }
    }

    if( mUBONeedsBinding )
    {
      mBufferInfos.clear();
      mBufferInfos.resize( mPushConstantsBindings.size() );

      // if uniform buffers for some reason changed, then rewrite
      // bind PCs
      for( uint32_t i = 0; i < mPushConstantsBindings.size(); ++i )
      {
        auto& ubo = mUboBuffers[i];
        auto& pc = mPushConstantsBindings[i];

  #ifdef DEBUG_ENABLED
        auto offset = ubo->GetBindingOffset();
      auto size = ubo->GetBindingSize();

      DALI_LOG_STREAM( gVulkanFilter, Debug::General, "offset: " << offset << ", size: " << size );
      DALI_LOG_STREAM( gVulkanFilter, Debug::General, "[RenderCommand] BindingUBO: binding = " << pc.binding );
  #endif

        auto bufferInfo = vk::DescriptorBufferInfo{}
        .setOffset( static_cast<uint32_t>( ubo->GetBindingOffset() ) )
        .setRange( static_cast<uint32_t>( ubo->GetBindingSize() ) )
        .setBuffer( ubo->GetBuffer()->GetVkHandle() );

        assert( (ubo->GetBindingOffset() % 256 ) == 0 );

        mBufferInfos[i] = bufferInfo;

        auto write = vk::WriteDescriptorSet{}.setPBufferInfo( &mBufferInfos[i] )
                                             .setDescriptorType( vk::DescriptorType::eUniformBuffer )
                                             .setDescriptorCount( 1 )
                                             .setDstSet( mDescriptorSets[0]->GetVkDescriptorSet() )
                                             .setDstBinding( pc.binding )
                                             .setDstArrayElement( 0 );

        descriptorWrites.emplace_back( write );
      }
      mUBONeedsBinding = false;
    }

    if( mUpdateFlags & ( API::RENDER_COMMAND_UPDATE_TEXTURE_BIT|API::RENDER_COMMAND_UPDATE_SAMPLER_BIT) )
    {

      mImageInfos.clear();
      mImageInfos.resize( mTextureBindings.size() );

      // only if textures/samplers changed, rewrite
      for( auto i = 0u; i < mTextureBindings.size(); ++i)
      {
        auto& texture = mTextureBindings[i];
        auto image = static_cast<const VulkanAPI::Texture*>(texture.texture);

        // test if image is valid, skip invalid image
        // @todo: possibly use builtin 'broken' image
        if( !image || !image->GetImageRef() )
        {
          continue;
        }

        DALI_LOG_STREAM( gVulkanFilter, Debug::General,
                         "[RenderCommand] BindingTextureSampler: binding = " << texture.binding );

        auto imageViewInfo = vk::DescriptorImageInfo{}
        .setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
        .setImageView( image->GetImageViewRef()->GetVkHandle() )
        .setSampler( !texture.sampler ?
                     image->GetSamplerRef()->GetVkHandle() :
                     static_cast<const VulkanAPI::Sampler*>( texture.sampler )->GetSampler()->GetVkHandle() );

        mImageInfos[i] = imageViewInfo;

        auto write = vk::WriteDescriptorSet{}.setPImageInfo( &mImageInfos[i] )
                                             .setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
                                             .setDescriptorCount( 1 )
                                             .setDstSet( mDescriptorSets[0]->GetVkDescriptorSet() )
                                             .setDstBinding( texture.binding )
                                             .setDstArrayElement( 0 );

        descriptorWrites.emplace_back( write );
      }
    }
    mUpdateFlags = 0u;
  }
}

void RenderCommand::UpdateUniformBuffers()
{
  uint32_t uboIndex = 0u;
  if( !mUboBuffers.empty() && mUboBuffers.size() == mPushConstantsBindings.size() )
  {
    for( auto&& pc : mPushConstantsBindings )
    {
      mUboBuffers[uboIndex++]->WriteKeepMapped( pc.data, 0, pc.size );
    }
  }
}

const Vulkan::RefCountedCommandBuffer& RenderCommand::GetCommandBuffer() const
{
  return mCommandBuffer;
}

void RenderCommand::AllocateUniformBufferMemory()
{
  // release any existing buffers
  mUboBuffers.clear();
  mUBONeedsBinding = true;
  // based on pipeline allocate memory for each push constant and uniform buffer
  // using given UBOManager
  for( auto&& pc : mPushConstantsBindings )
  {
    mUboBuffers.emplace_back( mController.GetUboManager().Allocate( pc.size ) );
    mUboBuffers.back()->Map(); //TODO: This is not the place to map the buffer
  }
}

const std::vector< Vulkan::RefCountedDescriptorSet >& RenderCommand::GetDescriptorSets() const
{
  return mDescriptorSets;
}

Vulkan::RefCountedPipeline RenderCommand::GetVulkanPipeline() const
{
  return mVulkanPipeline;
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
