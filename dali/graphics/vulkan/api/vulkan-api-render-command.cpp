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
#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-pipeline-cache.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-sampler.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-swapchain.h>

#include <dali/graphics/vulkan/api/internal/vulkan-ubo-manager.h>
#include <dali/graphics/vulkan/api/internal/vulkan-ubo-pool.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>

#include <iostream>
#define debug( x ) std::cout << x << std::endl;

namespace Dali
{
namespace Graphics
{
using Vulkan::VkAssert;

namespace VulkanAPI
{

struct RenderCommand::VulkanPipelineState
{
  VulkanPipelineState() = default;
  ~VulkanPipelineState() = default;

  vk::PipelineColorBlendStateCreateInfo              colorBlend;
  std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentState;

  vk::PipelineDepthStencilStateCreateInfo   depthStencil;
  vk::PipelineInputAssemblyStateCreateInfo  inputAssembly;
  vk::PipelineMultisampleStateCreateInfo    multisample;
  vk::PipelineRasterizationStateCreateInfo  rasterization;
  vk::PipelineTessellationStateCreateInfo   tesselation;
  vk::PipelineVertexInputStateCreateInfo    vertexInput;

  struct ViewportState
  {
    vk::PipelineViewportStateCreateInfo createInfo;
    std::vector<vk::Viewport>           viewports;
    std::vector<vk::Rect2D>             scissors;
  } viewport;
};

RenderCommand::~RenderCommand() = default;

RenderCommand::RenderCommand( VulkanAPI::Controller& controller, Vulkan::Graphics& graphics, Vulkan::PipelineCache& pipelineCache )
: mController( controller ), mGraphics( graphics ), mPipelineCache( pipelineCache ), mCommandBuffer(), mPipeline(), mUpdateFlags( UPDATE_ALL )
{
}

///@todo: needs pipeline factory rather than pipeline creation in place!!!
bool RenderCommand::PreparePipeline()
{
  if( mUpdateFlags )
  {
    auto &pipelineCache = mGraphics.GetPipelineCache();

    const auto &renderState = GetRenderState();
    const auto &shader      = static_cast<const VulkanAPI::Shader &>( renderState.shader
                                                                                 .Get());

    auto vertexShader   = shader.GetShader(vk::ShaderStageFlagBits::eVertex);
    auto fragmentShader = shader.GetShader(vk::ShaderStageFlagBits::eFragment);

    auto pipelineRef = pipelineCache.GetPipeline({vertexShader, fragmentShader, renderState.blendState, {}});

    if (!pipelineRef)
    {
      // retrieve input attributes descriptions
      std::vector<Vulkan::SpirV::SPIRVVertexInputAttribute> attribs{};
      vertexShader->GetSPIRVReflection()
                  .GetVertexInputAttributes(attribs);
      std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};

      // prepare vertex buffer bindings
      auto                                           bindingIndex{0u};
      std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};

      uint32_t  currentBuffer{0xffffffff};
      if( !attribs.empty() && attribs.size() == GetVertexBufferBindings().size() )
      {
        for (auto &&vb : GetVertexBufferBindings())
        {
          if (currentBuffer != vb.buffer
                                 .GetHandle())
          {
            bindingDescriptions.emplace_back(vk::VertexInputBindingDescription{}
                                               .setBinding(bindingIndex)
                                               .setInputRate(
                                                 vb.rate == API::RenderCommand::InputAttributeRate::PER_VERTEX
                                                 ? vk::VertexInputRate::eVertex
                                                 : vk::VertexInputRate::eInstance)
                                               .setStride(vb.stride));
            bindingIndex++;
            currentBuffer = Vulkan::U32(vb.buffer
                                          .GetHandle());
          }


          attributeDescriptions.emplace_back(vk::VertexInputAttributeDescription{}
                                               .setBinding(bindingDescriptions.back()
                                                                              .binding)
                                               .setFormat(attribs[vb.location].format)
                                               .setOffset(vb.offset));
        }
      }
      else // incompatible pipeline
      {
        mUpdateFlags = 0;
        return false;
      }

      auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{}
        .setVertexBindingDescriptionCount(Vulkan::U32(bindingDescriptions.size()))
        .setPVertexBindingDescriptions(bindingDescriptions.data())
        .setVertexAttributeDescriptionCount(Vulkan::U32(attributeDescriptions.size()))
        .setPVertexAttributeDescriptions(attributeDescriptions.data());

      // descriptor set layout
      auto vshDsLayouts = vertexShader->GetSPIRVReflection()
                                      .GenerateDescriptorSetLayoutCreateInfo();
      auto fshDsLayouts = fragmentShader->GetSPIRVReflection()
                                        .GenerateDescriptorSetLayoutCreateInfo();

      decltype(vshDsLayouts) layouts;
      layouts.resize(std::max(vshDsLayouts.size(), fshDsLayouts.size()));

      std::vector<vk::DescriptorSetLayout> dsLayouts;

      for (auto i = 0u; i < layouts.size(); ++i)
      {
        std::vector<vk::DescriptorSetLayoutBinding> dsBindings;

        // concatenate bindings per set
        if (vshDsLayouts[i].bindingCount)
        {
          dsBindings.insert(
            dsBindings.end(), vshDsLayouts[i].pBindings, vshDsLayouts[i].pBindings + vshDsLayouts[i].bindingCount);
        }
        if (fshDsLayouts[i].bindingCount)
        {
          dsBindings.insert(
            dsBindings.end(), fshDsLayouts[i].pBindings, fshDsLayouts[i].pBindings + fshDsLayouts[i].bindingCount);
        }

        layouts[i].pBindings    = dsBindings.data();
        layouts[i].bindingCount = Vulkan::U32(dsBindings.size());


        dsLayouts.emplace_back(VkAssert(mGraphics.GetDevice()
                                                 .createDescriptorSetLayout(layouts[i], mGraphics.GetAllocator())));

      }

      // create layout
      auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo{}
        .setSetLayoutCount(Vulkan::U32(dsLayouts.size()))
        .setPSetLayouts(dsLayouts.data())
        .setPPushConstantRanges(nullptr)
        .setPushConstantRangeCount(0);

      auto pipelineLayout = VkAssert(mGraphics.GetDevice()
                                              .createPipelineLayout(pipelineLayoutCreateInfo,
                                                                    mGraphics.GetAllocator()));

      std::vector<vk::PipelineShaderStageCreateInfo> shaderStages =
                                                       {
                                                         vk::PipelineShaderStageCreateInfo{}
                                                           .setModule(vertexShader->GetVkHandle())
                                                           .setStage(vk::ShaderStageFlagBits::eVertex)
                                                           .setPName("main"),
                                                         vk::PipelineShaderStageCreateInfo{}
                                                           .setModule(fragmentShader->GetVkHandle())
                                                           .setStage(vk::ShaderStageFlagBits::eFragment)
                                                           .setPName("main")
                                                       };

      mVulkanPipelineState = std::make_unique<VulkanPipelineState>();

      // @todo use RenderTarget
      auto                           swapchain = mGraphics.GetSwapchainForFBID(0);
      auto                           fb        = swapchain->GetCurrentFramebuffer();
      vk::GraphicsPipelineCreateInfo pipelineInfo;
      pipelineInfo
        .setSubpass(0)
        .setRenderPass(fb->GetRenderPassVkHandle()) // based on render target
        .setBasePipelineHandle(nullptr)
        .setBasePipelineIndex(0)
        .setLayout(pipelineLayout)
        .setPColorBlendState(PrepareColorBlendStateCreateInfo())
        .setPDepthStencilState(PrepareDepthStencilStateCreateInfo())
        .setPDynamicState(nullptr)
        .setPInputAssemblyState(PrepareInputAssemblyStateCreateInfo())
        .setPMultisampleState(PrepareMultisampleStateCreateInfo())
        .setPRasterizationState(PrepareRasterizationStateCreateInfo())
        .setPTessellationState(PrepareTesselationStateCreateInfo())
          //.setPVertexInputState(PrepareVertexInputStateCreateInfo())
        .setPVertexInputState(&vertexInputState)
        .setPViewportState(PrepareViewportStateCreateInfo())
        .setPStages(shaderStages.data())
        .setStageCount(Vulkan::U32(shaderStages.size()));

      auto pipeline = Vulkan::Pipeline::New(mGraphics, pipelineInfo);

      pipeline->Compile();

      pipelineCache.AddPipeline(pipeline, Vulkan::PipelineDescription{vertexShader, fragmentShader, renderState.blendState, dsLayouts});

      pipelineRef = pipeline;

    }

    mPipeline = pipelineRef;

    mVkDescriptorSetLayouts.clear();

    mVkDescriptorSetLayouts = pipelineCache.GetDescriptorSetLayouts(mPipeline);

    // based on pipeline recreate descriptor pool
    auto poolSizes = std::vector<vk::DescriptorPoolSize>{
      vk::DescriptorPoolSize{}
        .setDescriptorCount(10)
        .setType(vk::DescriptorType::eUniformBuffer),
      vk::DescriptorPoolSize{}
        .setDescriptorCount(10)
        .setType(vk::DescriptorType::eCombinedImageSampler),
      vk::DescriptorPoolSize{}
        .setDescriptorCount(10)
        .setType(vk::DescriptorType::eSampledImage)
    };

    // create descriptor pool
    mDescriptorPool = Vulkan::DescriptorPool::New(mGraphics, vk::DescriptorPoolCreateInfo{}
      .setMaxSets(Vulkan::U32(mVkDescriptorSetLayouts.size() + 1))
      .setPPoolSizes(poolSizes.data())
      .setPoolSizeCount(Vulkan::U32(poolSizes.size())));

    // allocate descriptor sets. we need a descriptors for each descriptorset
    // in the shader
    // allocate descriptor sets for given pipeline layout
    mDescriptorSets = mDescriptorPool->AllocateDescriptorSets(
      vk::DescriptorSetAllocateInfo{}
        .setPSetLayouts(mVkDescriptorSetLayouts.data())
        .setDescriptorPool(nullptr)
        .setDescriptorSetCount(uint32_t(mVkDescriptorSetLayouts.size()))
    );

    AllocateUniformBufferMemory();

    BindUniformBuffers();

    BindTexturesAndSamplers();

    mUpdateFlags = 0u;
  }
  return true;
}

void RenderCommand::UpdateUniformBuffers()
{
  uint32_t uboIndex = 0u;
  if(!mUboBuffers.empty() && mUboBuffers.size() == mPushConstantsBindings.size())
  {
    for (auto &&pc : mPushConstantsBindings)
    {
      mUboBuffers[uboIndex++]->WriteKeepMapped(pc.data, 0, pc.size);
    }
  }
}

const vk::PipelineColorBlendStateCreateInfo* RenderCommand::PrepareColorBlendStateCreateInfo()
{
  //@todo support more attachments
  //@todo use data from render state
  auto blendAttachmentState = vk::PipelineColorBlendAttachmentState{}
    .setBlendEnable( vk::Bool32(mRenderState.blendState.blendingEnabled) )
    .setColorWriteMask( vk::ColorComponentFlagBits::eR |
                        vk::ColorComponentFlagBits::eG |
                        vk::ColorComponentFlagBits::eB |
                        vk::ColorComponentFlagBits::eA )
    .setSrcColorBlendFactor( vk::BlendFactor::eSrcAlpha )
    .setDstColorBlendFactor( vk::BlendFactor::eOneMinusSrcAlpha )
    .setSrcAlphaBlendFactor( vk::BlendFactor::eOne )
    .setDstAlphaBlendFactor( vk::BlendFactor::eOneMinusSrcAlpha )
    .setColorBlendOp( vk::BlendOp::eAdd )
    .setAlphaBlendOp( vk::BlendOp::eAdd );

  mVulkanPipelineState->colorBlendAttachmentState.emplace_back( blendAttachmentState );

  mVulkanPipelineState->colorBlend
                      .setBlendConstants( { 1.0f, 1.0f, 1.0f, 1.0f })
                      .setAttachmentCount( 1 )
                      .setPAttachments( mVulkanPipelineState->colorBlendAttachmentState.data() );

  return &mVulkanPipelineState->colorBlend;
}

const vk::PipelineDepthStencilStateCreateInfo* RenderCommand::PrepareDepthStencilStateCreateInfo()
{
  return &(mVulkanPipelineState->depthStencil
          .setDepthBoundsTestEnable( false )
          .setStencilTestEnable( false ));
}

const vk::PipelineInputAssemblyStateCreateInfo* RenderCommand::PrepareInputAssemblyStateCreateInfo()
{
  //@todo support topology and restart
  return &(mVulkanPipelineState->inputAssembly
  .setPrimitiveRestartEnable( true )
  .setTopology( vk::PrimitiveTopology::eTriangleStrip ));
}

const vk::PipelineMultisampleStateCreateInfo* RenderCommand::PrepareMultisampleStateCreateInfo()
{
  return &(mVulkanPipelineState->multisample
        .setSampleShadingEnable( false )
        .setRasterizationSamples( vk::SampleCountFlagBits::e1)
        .setAlphaToCoverageEnable( false )
        .setMinSampleShading( 1.0f )
        .setPSampleMask( nullptr ));
}

const vk::PipelineRasterizationStateCreateInfo* RenderCommand::PrepareRasterizationStateCreateInfo()
{
  return &(mVulkanPipelineState->rasterization
    .setCullMode( vk::CullModeFlagBits::eNone )
    .setDepthBiasClamp( 0.0f )
    .setDepthBiasEnable( false )
    .setDepthClampEnable( false )
    .setFrontFace( vk::FrontFace::eCounterClockwise )
    .setPolygonMode( vk::PolygonMode::eFill )
    .setRasterizerDiscardEnable( false )
    .setLineWidth( 1.0f ));
}

const vk::PipelineTessellationStateCreateInfo* RenderCommand::PrepareTesselationStateCreateInfo()
{
  return nullptr;
}

const vk::PipelineVertexInputStateCreateInfo* RenderCommand::PrepareVertexInputStateCreateInfo()
{
  /*
  mBindingDesc.clear();
  mAttrDesc.clear();
  mVertexInputState = vk::PipelineVertexInputStateCreateInfo{};
  mVertexInputState.setPVertexAttributeDescriptions( (mAttrDesc = attrDesc).data() );
  mVertexInputState.setPVertexBindingDescriptions( (mBindingDesc = bindingDesc).data() );
  mVertexInputState.setVertexAttributeDescriptionCount( U32(mAttrDesc.size()) );
  mVertexInputState.setVertexBindingDescriptionCount( U32(mBindingDesc.size()) );
  mInfo.setPVertexInputState( &mVertexInputState );
   */
  return nullptr;
}

const vk::PipelineViewportStateCreateInfo* RenderCommand::PrepareViewportStateCreateInfo()
{
  assert( !mPipeline && "Pipeline cannot be changed anymore!");

  auto width = mGraphics.GetSwapchainForFBID(0)->GetCurrentFramebuffer()->GetWidth();
  auto height = mGraphics.GetSwapchainForFBID(0)->GetCurrentFramebuffer()->GetHeight();

  // viewports
  mVulkanPipelineState->viewport.viewports.emplace_back( 0, 0, width, height );
  mVulkanPipelineState->viewport.viewports[0].setMinDepth( 0.0f );
  mVulkanPipelineState->viewport.viewports[0].setMaxDepth( 1.0f );

  // scissors
  mVulkanPipelineState->viewport.scissors.emplace_back( vk::Rect2D(
                        { static_cast<int32_t>(0), static_cast<int32_t>(0) },
                        { Vulkan::U32(width), Vulkan::U32(height) } )
                      );

  auto& viewState = mVulkanPipelineState->viewport;

  return &(mVulkanPipelineState->viewport.createInfo.
          setViewportCount( Vulkan::U32(viewState.viewports.size()) ).
          setPViewports( viewState.viewports.data() ).
          setPScissors( viewState.scissors.data() ).
          setScissorCount( Vulkan::U32(viewState.scissors.size())) );
}

const Vulkan::RefCountedCommandBuffer& RenderCommand::GetCommandBuffer() const
{
  return mCommandBuffer;
}

const Vulkan::PipelineCache& RenderCommand::GetPipelineCache() const
{
  return mPipelineCache;
}

const Vulkan::RefCountedPipeline& RenderCommand::GetPipeline() const
{
  return mPipeline;
}

void RenderCommand::AllocateUniformBufferMemory()
{
  // release any existing buffers
  mUboBuffers.clear();

  // based on pipeline allocate memory for each push constant and uniform buffer
  // using given UBOManager
  for( auto&& pc : mPushConstantsBindings )
  {
    mUboBuffers.emplace_back( mController.GetUboManager().Allocate( pc.size ) );
  }

}

void RenderCommand::BindUniformBuffers()
{
  // if uniform buffers for some reason changed, then rewrite
  // bind PCs
  for( uint32_t i = 0; i < mPushConstantsBindings.size(); ++i )
  {
    auto& ubo = mUboBuffers[i];
    auto& pc = mPushConstantsBindings[i];

    auto offset = ubo->GetBindingOffset();
    auto size = ubo->GetBindingSize();
    std::cout << "offset: " << offset << ", size: " << size << std::endl;
    debug("[RenderCommand] BindingUBO: binding = " << pc.binding);
    mDescriptorSets[0]->WriteUniformBuffer( pc.binding, ubo->GetBuffer(), ubo->GetBindingOffset(), ubo->GetBindingSize() );
  }
}

void RenderCommand::BindTexturesAndSamplers()
{
  // only if textures/samplers changed, rewrite
  for( auto&& texture : mTextureBindings )
  {
    auto& image = static_cast<VulkanAPI::Texture&>( texture.texture.Get() );
    debug("[RenderCommand] BindingTextureSampler: binding = " << texture.binding);
    mDescriptorSets[0]->WriteCombinedImageSampler( texture.binding, image.GetTextureRef()->GetSampler(), image.GetTextureRef()->GetImageView() );
  }
}

const std::vector<Vulkan::RefCountedDescriptorSet>& RenderCommand::GetDescriptorSets() const
{
  return mDescriptorSets;
}


} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali