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
#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-pipeline-cache.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-swapchain.h>

namespace Dali
{
namespace Graphics
{
using Vulkan::VkAssert;

namespace VulkanAPI
{
RenderCommand::RenderCommand( Vulkan::Graphics& graphics, Vulkan::PipelineCache& pipelineCache )
: mGraphics( graphics ), mPipelineCache( pipelineCache ), mCommandBuffer(), mPipeline(), mUpdateFlags( 0u )
{
}

bool RenderCommand::PreparePipeline()
{
  // if (nothing changed)
  const auto& renderState = GetRenderState();
  const auto& shader      = static_cast<const VulkanAPI::Shader&>( renderState.shader.Get() );

  auto vertexShader   = shader.GetShader( vk::ShaderStageFlagBits::eVertex );
  auto fragmentShader = shader.GetShader( vk::ShaderStageFlagBits::eFragment );

  // retrieve input attributes descriptions
  std::vector<Vulkan::SpirV::SPIRVVertexInputAttribute> attribs{};
  vertexShader->GetSPIRVReflection().GetVertexInputAttributes( attribs );
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};

  // prepare vertex buffer bindings
  auto                                           bindingIndex{0u};
  std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};

  uint32_t currentBuffer{0xffffffff};
  for( auto&& vb : GetVertexBufferBindings() )
  {
    if( currentBuffer != vb.buffer.GetHandle() )
    {
      bindingDescriptions.emplace_back( vk::VertexInputBindingDescription{}
                                          .setBinding( bindingIndex )
                                          .setInputRate( vb.rate == API::RenderCommand::InputAttributeRate::PER_VERTEX
                                                           ? vk::VertexInputRate::eVertex
                                                           : vk::VertexInputRate::eInstance )
                                          .setStride( vb.stride ) );
      bindingIndex++;
      currentBuffer = Vulkan::U32( vb.buffer.GetHandle() );
    }

    attributeDescriptions.emplace_back( vk::VertexInputAttributeDescription{}
                                          .setBinding( bindingDescriptions.back().binding )
                                          .setFormat( attribs[vb.location].format )
                                          .setOffset( vb.offset ) );
  }

  // descriptor set layout
  auto                     vshDsLayouts = vertexShader->GetSPIRVReflection().GenerateDescriptorSetLayoutCreateInfo();
  auto                     fshDsLayouts = fragmentShader->GetSPIRVReflection().GenerateDescriptorSetLayoutCreateInfo();
  decltype( vshDsLayouts ) layouts;
  layouts.resize( std::max( vshDsLayouts.size(), fshDsLayouts.size() ) );

  std::vector<vk::DescriptorSetLayout> dsLayouts;

  for( auto i = 0u; i < layouts.size(); ++i )
  {
    std::vector<vk::DescriptorSetLayoutBinding> dsBindings;

    // concatenate bindings per set
    if( vshDsLayouts[i].bindingCount )
    {
      dsBindings.insert(
        dsBindings.end(), vshDsLayouts[i].pBindings, vshDsLayouts[i].pBindings + vshDsLayouts[i].bindingCount );
    }
    if( fshDsLayouts[i].bindingCount )
    {
      dsBindings.insert(
        dsBindings.end(), fshDsLayouts[i].pBindings, fshDsLayouts[i].pBindings + fshDsLayouts[i].bindingCount );
    }

    layouts[i].pBindings    = dsBindings.data();
    layouts[i].bindingCount = Vulkan::U32( dsBindings.size() );

    dsLayouts.emplace_back( VkAssert( mGraphics.GetDevice().createDescriptorSetLayout( layouts[i], mGraphics.GetAllocator() ) ) );

  }

  // create layout
  auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo{}
       .setSetLayoutCount( Vulkan::U32(dsLayouts.size()) )
       .setPSetLayouts( dsLayouts.data() )
       .setPPushConstantRanges( nullptr )
       .setPushConstantRangeCount( 0 );

  auto pipelineLayout = VkAssert( mGraphics.GetDevice().createPipelineLayout( pipelineLayoutCreateInfo, mGraphics.GetAllocator() ) );

  // @todo use RenderTarget
  auto swapchain = mGraphics.GetSwapchainForFBID(0);
  auto fb = swapchain->GetCurrentFramebuffer();
  vk::GraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo
    .setSubpass( 0 )
    .setRenderPass( fb->GetVkRenderPass() ) // based on render target
    .setBasePipelineHandle( nullptr )
    .setBasePipelineIndex( 0 )
    .setLayout( pipelineLayout )
    .setPColorBlendState( nullptr ) // blend state
    .setPDepthStencilState( nullptr )
    .setPDynamicState( nullptr )
    .setPInputAssemblyState( nullptr )
    .setPMultisampleState( nullptr )
    .setPRasterizationState( nullptr )
    .setPTessellationState( nullptr )
    .setPVertexInputState( nullptr )
    .setPViewportState( nullptr )
    .setPStages( nullptr )
    .setStageCount( 0 );

  auto pipeline = Vulkan::Pipeline::New( mGraphics, pipelineInfo );

  // cache it
  // create a comparison key


  return false;
}

const Vulkan::CommandBufferRef& RenderCommand::GetCommandBuffer() const
{
  return mCommandBuffer;
}

const Vulkan::PipelineCache& RenderCommand::GetPipelineCache() const
{
  return mPipelineCache;
}

const Vulkan::PipelineRef& RenderCommand::GetPipeline() const
{
  return mPipeline;
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali