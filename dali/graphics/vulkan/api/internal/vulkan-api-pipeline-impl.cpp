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

#include <dali/graphics/vulkan/api/vulkan-api-pipeline-cache.h>
#include <dali/graphics/vulkan/api/internal/vulkan-api-pipeline-impl.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>

#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>

#include <dali/graphics/vulkan/api/vulkan-api-shader.h>

#include <array>
namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
namespace Internal
{
using Vulkan::VkAssert;
namespace
{
constexpr vk::BlendOp ConvertBlendOp( Dali::Graphics::API::Pipeline::BlendOp blendOp )
{
  using Dali::Graphics::API::Pipeline;
  switch( blendOp )
  {
    case Pipeline::BlendOp::ADD:
      return vk::BlendOp::eAdd;
    case Pipeline::BlendOp::SUBTRACT:
      return vk::BlendOp::eSubtract;
    case Pipeline::BlendOp::REVERSE_SUBTRACT:
      return vk::BlendOp::eReverseSubtract;
    case Pipeline::BlendOp::MIN:
      return vk::BlendOp::eMin;
    case Pipeline::BlendOp::MAX:
      return vk::BlendOp::eMax;
  }
  return vk::BlendOp{};
}

constexpr vk::BlendFactor ConvertBlendFactor( Dali::Graphics::API::Pipeline::BlendFactor blendFactor )
{
  using Dali::Graphics::API::Pipeline;
  switch( blendFactor )
  {
    case Pipeline::BlendFactor::ZERO:
      return vk::BlendFactor::eZero;
    case Pipeline::BlendFactor::ONE:
      return vk::BlendFactor::eOne;
    case Pipeline::BlendFactor::SRC_COLOR:
      return vk::BlendFactor::eSrcColor;
    case Pipeline::BlendFactor::ONE_MINUS_SRC_COLOR:
      return vk::BlendFactor::eOneMinusSrcAlpha;
    case Pipeline::BlendFactor::DST_COLOR:
      return vk::BlendFactor::eDstColor;
    case Pipeline::BlendFactor::ONE_MINUS_DST_COLOR:
      return vk::BlendFactor::eOneMinusDstColor;
    case Pipeline::BlendFactor::SRC_ALPHA:
      return vk::BlendFactor::eSrcAlpha;
    case Pipeline::BlendFactor::ONE_MINUS_SRC_ALPHA:
      return vk::BlendFactor::eOneMinusSrcAlpha;
    case Pipeline::BlendFactor::DST_ALPHA:
      return vk::BlendFactor::eDstAlpha;
    case Pipeline::BlendFactor::ONE_MINUS_DST_ALPHA:
      return vk::BlendFactor::eOneMinusDstAlpha;
    case Pipeline::BlendFactor::CONSTANT_COLOR:
      return vk::BlendFactor::eConstantColor;
    case Pipeline::BlendFactor::ONE_MINUS_CONSTANT_COLOR:
      return vk::BlendFactor::eOneMinusConstantColor;
    case Pipeline::BlendFactor::CONSTANT_ALPHA:
      return vk::BlendFactor::eConstantAlpha;
    case Pipeline::BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
      return vk::BlendFactor::eOneMinusConstantAlpha;
    case Pipeline::BlendFactor::SRC_ALPHA_SATURATE:
      return vk::BlendFactor::eSrcAlphaSaturate;
    case Pipeline::BlendFactor::SRC1_COLOR:
      return vk::BlendFactor::eSrc1Color;
    case Pipeline::BlendFactor::ONE_MINUS_SRC1_COLOR:
      return vk::BlendFactor::eOneMinusSrc1Color;
    case Pipeline::BlendFactor::SRC1_ALPHA:
      return vk::BlendFactor::eSrc1Alpha;
    case Pipeline::BlendFactor::ONE_MINUS_SRC1_ALPHA:
      return vk::BlendFactor::eOneMinusSrc1Alpha;
  }
  return vk::BlendFactor{};
}

constexpr vk::CompareOp ConvertCompareOp( Dali::Graphics::API::Pipeline::CompareOp compareOp)
{
  using Dali::Graphics::API::Pipeline;
  switch (compareOp)
  {
    case Pipeline::CompareOp::NEVER:
      return vk::CompareOp::eNever;
    case Pipeline::CompareOp::LESS:
      return vk::CompareOp::eLess;
    case Pipeline::CompareOp::EQUAL:
      return vk::CompareOp::eEqual;
    case Pipeline::CompareOp::LESS_OR_EQUAL:
      return vk::CompareOp::eLessOrEqual;
    case Pipeline::CompareOp::GREATER:
      return vk::CompareOp::eGreater;
    case Pipeline::CompareOp::NOT_EQUAL:
      return vk::CompareOp::eNotEqual;
    case Pipeline::CompareOp::GREATER_OR_EQUAL:
      return vk::CompareOp::eGreaterOrEqual;
    case Pipeline::CompareOp::ALWAYS:
      return vk::CompareOp::eAlways;
  }
  return vk::CompareOp{};
}

constexpr vk::PrimitiveTopology ConvertPrimitiveTopology( Dali::Graphics::API::Pipeline::PrimitiveTopology topology)
{
  using Dali::Graphics::API::Pipeline;
  switch (topology)
  {
    case Pipeline::PrimitiveTopology::POINT_LIST:
      return vk::PrimitiveTopology::ePointList;
    case Pipeline::PrimitiveTopology::LINE_LIST:
      return vk::PrimitiveTopology::eLineList;
    case Pipeline::PrimitiveTopology::LINE_STRIP:
      return vk::PrimitiveTopology::eLineStrip;
    case Pipeline::PrimitiveTopology::TRIANGLE_LIST:
      return vk::PrimitiveTopology::eTriangleList;
    case Pipeline::PrimitiveTopology::TRIANGLE_STRIP:
      return vk::PrimitiveTopology::eTriangleStrip;
    case Pipeline::PrimitiveTopology::TRIANGLE_FAN:
      return vk::PrimitiveTopology::eTriangleFan;
  }
  return vk::PrimitiveTopology{};
}

constexpr vk::CullModeFlagBits ConvertCullMode( Dali::Graphics::API::Pipeline::CullMode cullMode)
{
  using Dali::Graphics::API::Pipeline;
  switch (cullMode)
  {
    case Pipeline::CullMode::NONE:
      return vk::CullModeFlagBits::eNone;
    case Pipeline::CullMode::FRONT:
      return vk::CullModeFlagBits::eFront;
    case Pipeline::CullMode::BACK:
      return vk::CullModeFlagBits::eBack;
    case Pipeline::CullMode::FRONT_AND_BACK:
      return vk::CullModeFlagBits::eFrontAndBack;
  }
  return vk::CullModeFlagBits{};
}

constexpr vk::PolygonMode ConvertPolygonMode( Dali::Graphics::API::Pipeline::PolygonMode polygonMode)
{
  using Dali::Graphics::API::Pipeline;
  switch (polygonMode)
  {
    case Pipeline::PolygonMode::FILL:
      return vk::PolygonMode::eFill;
    case Pipeline::PolygonMode::LINE:
      return vk::PolygonMode::eLine;
    case Pipeline::PolygonMode::POINT:
      return vk::PolygonMode::ePoint;
  }
  return vk::PolygonMode{};
}

constexpr vk::FrontFace ConvertFrontFace( Dali::Graphics::API::Pipeline::FrontFace frontFace)
{
  using Dali::Graphics::API::Pipeline;
  switch (frontFace)
  {
    case Pipeline::FrontFace::CLOCKWISE:
      return vk::FrontFace::eClockwise;
    case Pipeline::FrontFace::COUNTER_CLOCKWISE:
      return vk::FrontFace::eCounterClockwise;
  }
  return vk::FrontFace{};
}

}

struct Pipeline::PipelineCreateInfo
{
  PipelineFactory::Info info;
};

struct Pipeline::VulkanPipelineState
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

  Vulkan::RefCountedPipeline pipeline{};
};

Pipeline::Pipeline( Vulkan::Graphics& graphics, Controller& controller, const PipelineFactory* factory )
: mGraphics( controller.GetGraphics() ),
  mController( controller )
{
  mHashCode = factory->GetHashCode();

  // copy info
  mCreateInfo = std::unique_ptr<PipelineCreateInfo>(new PipelineCreateInfo({factory->GetCreateInfo()}));

  mPipelineCache = factory->mPipelineCache;

  Initialise();
}

Pipeline::~Pipeline()
{
  // deleting pipeline
}

uint32_t GetLocationIndex( const std::vector<Vulkan::SpirV::SPIRVVertexInputAttribute>& attribs, uint32_t location )
{
  auto retval = 0u;

  for( auto&& attr : attribs )
  {
    if( attr.location == location )
    {
      return retval;
    }
    retval++;
  }
  return -1u;
}

bool Pipeline::Initialise()
{
  mVulkanPipelineState = std::unique_ptr<VulkanPipelineState>( new VulkanPipelineState() );

  // get shaders
  const auto& shader = static_cast<const VulkanAPI::Shader*>(mCreateInfo->info.shaderState.shaderProgram);
  auto vertexShader   = shader->GetShader(vk::ShaderStageFlagBits::eVertex);
  auto fragmentShader = shader->GetShader(vk::ShaderStageFlagBits::eFragment);

  // retrieve input attributes descriptions
  std::vector<Vulkan::SpirV::SPIRVVertexInputAttribute> attribs{};
  vertexShader->GetSPIRVReflection()
              .GetVertexInputAttributes(attribs);

  std::vector<vk::VertexInputAttributeDescription>    attributeDescriptions{};
  std::vector<vk::VertexInputBindingDescription>      bindingDescriptions{};

  const auto& bufferBindings = mCreateInfo->info.vertexInputState.bufferBindings;
  const auto& attributes = mCreateInfo->info.vertexInputState.attributes;

  // vertex buffer bindings match order of buffers
  for( uint32_t bindingIndex = 0u; bindingIndex < bufferBindings.size(); ++ bindingIndex )
  {
    bindingDescriptions.emplace_back(vk::VertexInputBindingDescription{}
                                       .setBinding(bindingIndex)
                                       .setInputRate(
                                         bufferBindings[bindingIndex].inputRate == API::Pipeline::VertexInputRate::PER_VERTEX
                                         ? vk::VertexInputRate::eVertex
                                         : vk::VertexInputRate::eInstance)
                                       .setStride(bufferBindings[bindingIndex].stride));
  }

  // create attribute descriptions
  if( !attribs.empty() && attribs.size() == attribs.size() )
  {
    for (auto&& vb : attributes )
    {
      attributeDescriptions.emplace_back(vk::VertexInputAttributeDescription{}
                                           .setBinding(bindingDescriptions.back()
                                                                          .binding)
                                           .setFormat(attribs[GetLocationIndex( attribs, vb.location)].format)
                                           .setLocation(vb.location )
                                           .setOffset(vb.offset));
    }
  }
  else // incompatible pipeline
  {
    return false;
  }

  // prepare vertex input state
  auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{}
    .setVertexBindingDescriptionCount(Vulkan::U32(bindingDescriptions.size()))
    .setPVertexBindingDescriptions(bindingDescriptions.data())
    .setVertexAttributeDescriptionCount(Vulkan::U32(attributeDescriptions.size()))
    .setPVertexAttributeDescriptions(attributeDescriptions.data());

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

  // @todo use RenderTarget
  auto swapchain = mGraphics.GetSwapchainForFBID(0);
  auto fb = swapchain->GetCurrentFramebuffer();

  vk::GraphicsPipelineCreateInfo pipelineInfo;
  pipelineInfo
    .setSubpass(0)
    .setRenderPass(fb->GetRenderPassVkHandle()) // based on render target
    .setBasePipelineHandle(nullptr)
    .setBasePipelineIndex(0)
    .setLayout(PreparePipelineLayout())
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

  mVulkanPipelineState->pipeline = pipeline;
  return true;
}



const vk::PipelineColorBlendStateCreateInfo* Pipeline::PrepareColorBlendStateCreateInfo()
{
  const auto& factoryInfo = mCreateInfo->info;

  // blending enabled
  bool blendEnable = factoryInfo.colorBlendState.blendEnable;

  // color write mask
  vk::ColorComponentFlags colorWriteMask{};
  colorWriteMask |= ( factoryInfo.colorBlendState.colorComponentWriteBits & 1 ) ? vk::ColorComponentFlagBits::eR : vk::ColorComponentFlagBits{};
  colorWriteMask |= ( factoryInfo.colorBlendState.colorComponentWriteBits & 2 ) ? vk::ColorComponentFlagBits::eG : vk::ColorComponentFlagBits{};
  colorWriteMask |= ( factoryInfo.colorBlendState.colorComponentWriteBits & 4 ) ? vk::ColorComponentFlagBits::eB : vk::ColorComponentFlagBits{};
  colorWriteMask |= ( factoryInfo.colorBlendState.colorComponentWriteBits & 8 ) ? vk::ColorComponentFlagBits::eA : vk::ColorComponentFlagBits{};

  auto srcColorBlendFactor = ConvertBlendFactor( factoryInfo.colorBlendState.srcColorBlendFactor );
  auto dstColorBlendFactor = ConvertBlendFactor( factoryInfo.colorBlendState.dstColorBlendFactor );
  auto srcAlphaBlendFactor = ConvertBlendFactor( factoryInfo.colorBlendState.srcAlphaBlendFactor );
  auto dstAlphaBlendFactor = ConvertBlendFactor( factoryInfo.colorBlendState.dstAlphaBlendFactor );
  auto colorBlendOp = ConvertBlendOp( factoryInfo.colorBlendState.colorBlendOp );
  auto alphaBlendOp = ConvertBlendOp( factoryInfo.colorBlendState.alphaBlendOp );

  auto blendAttachmentState = vk::PipelineColorBlendAttachmentState{}
    .setBlendEnable( vk::Bool32( blendEnable ) )
    .setColorWriteMask( colorWriteMask )
    .setSrcColorBlendFactor( srcColorBlendFactor )
    .setDstColorBlendFactor( dstColorBlendFactor )
    .setSrcAlphaBlendFactor( srcAlphaBlendFactor )
    .setDstAlphaBlendFactor( dstAlphaBlendFactor )
    .setColorBlendOp( colorBlendOp )
    .setAlphaBlendOp( alphaBlendOp );

  mVulkanPipelineState->colorBlendAttachmentState.emplace_back( blendAttachmentState );

  mVulkanPipelineState->colorBlend
                      .setBlendConstants( { factoryInfo.colorBlendState.blendConstants[0],
                                            factoryInfo.colorBlendState.blendConstants[1],
                                            factoryInfo.colorBlendState.blendConstants[2],
                                            factoryInfo.colorBlendState.blendConstants[3] } )
                      .setAttachmentCount( 1 )
                      .setPAttachments( mVulkanPipelineState->colorBlendAttachmentState.data() );

  return &mVulkanPipelineState->colorBlend;
}

const vk::PipelineDepthStencilStateCreateInfo* Pipeline::PrepareDepthStencilStateCreateInfo()
{
  const auto& dsInfo = mCreateInfo->info.depthStencilState;
  return &(mVulkanPipelineState->depthStencil
                               .setDepthTestEnable( vk::Bool32(dsInfo.depthTestEnable) )
                               .setDepthWriteEnable( vk::Bool32(dsInfo.depthWriteEnable) )
                               .setDepthCompareOp( ConvertCompareOp( dsInfo.depthCompareOp ))
                               .setDepthBoundsTestEnable( false )
                               .setStencilTestEnable( false )); //@ todo stencil test
}

const vk::PipelineInputAssemblyStateCreateInfo* Pipeline::PrepareInputAssemblyStateCreateInfo()
{
  const auto& iaInfo = mCreateInfo->info.inputAssemblyState;
  //@todo support topology and restart
  return &(mVulkanPipelineState->inputAssembly
                               .setPrimitiveRestartEnable( vk::Bool32( iaInfo.primitiveRestartEnable ) )
                               .setTopology( ConvertPrimitiveTopology( iaInfo.topology ) ));
}

const vk::PipelineMultisampleStateCreateInfo* Pipeline::PrepareMultisampleStateCreateInfo()
{
  return &(mVulkanPipelineState->multisample
                               .setSampleShadingEnable( vk::Bool32(false) )
                               .setRasterizationSamples( vk::SampleCountFlagBits::e1 )
                               .setAlphaToCoverageEnable( vk::Bool32(false) )
                               .setMinSampleShading( 1.0f )
                               .setPSampleMask( nullptr ));
}

const vk::PipelineRasterizationStateCreateInfo* Pipeline::PrepareRasterizationStateCreateInfo()
{
  const auto& rsInfo = mCreateInfo->info.rasterizationState;
  return &(mVulkanPipelineState->rasterization
                               .setCullMode( ConvertCullMode( rsInfo.cullMode ) )
                               .setDepthBiasClamp( 0.0f )
                               .setDepthBiasEnable( vk::Bool32(false) )
                               .setDepthClampEnable( vk::Bool32(false) )
                               .setFrontFace( ConvertFrontFace( rsInfo.frontFace ) )
                               .setPolygonMode( ConvertPolygonMode( rsInfo.polygonMode ) )
                               .setRasterizerDiscardEnable( vk::Bool32(false) )
                               .setLineWidth( 1.0f ));
}

const vk::PipelineViewportStateCreateInfo* Pipeline::PrepareViewportStateCreateInfo()
{
  const auto& vpInfo = mCreateInfo->info.viewportState;

  auto width = vpInfo.viewport.width;
  auto height = vpInfo.viewport.height;

  if( !uint32_t(width) )
  {
    width = float(mGraphics.GetSwapchainForFBID(0)->GetCurrentFramebuffer()->GetWidth());
    height = float(mGraphics.GetSwapchainForFBID(0)->GetCurrentFramebuffer()->GetHeight());
  }

  // viewports
  mVulkanPipelineState->viewport.viewports.emplace_back( vpInfo.viewport.x, vpInfo.viewport.y, width, height );
  mVulkanPipelineState->viewport.viewports[0].setMinDepth( vpInfo.viewport.minDepth );
  mVulkanPipelineState->viewport.viewports[0].setMaxDepth( vpInfo.viewport.maxDepth );

  // scissors
  // todo: add scissor support
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

const vk::PipelineTessellationStateCreateInfo* Pipeline::PrepareTesselationStateCreateInfo()
{
  return nullptr;
}

const vk::PipelineLayout Pipeline::PreparePipelineLayout()
{
  // descriptor set layout
  const auto& shader = static_cast<const VulkanAPI::Shader*>(mCreateInfo->info.shaderState.shaderProgram);
  auto vertexShader   = shader->GetShader(vk::ShaderStageFlagBits::eVertex);
  auto fragmentShader = shader->GetShader(vk::ShaderStageFlagBits::eFragment);

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

  mVkDescriptorSetLayouts = dsLayouts;

  // create layout
  auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo{}
    .setSetLayoutCount(Vulkan::U32(dsLayouts.size()))
    .setPSetLayouts(dsLayouts.data())
    .setPPushConstantRanges(nullptr)
    .setPushConstantRangeCount(0);

  return VkAssert(mGraphics.GetDevice().createPipelineLayout(pipelineLayoutCreateInfo, mGraphics.GetAllocator()));
}

void Pipeline::Reference()
{
  ++mRefCounter;
}

void Pipeline::Dereference()
{
  if(mRefCounter)
  {
    if (--mRefCounter == 0)
    {
      Destroy();
    }
  }
}

void Pipeline::Destroy()
{
  if(mPipelineCache)
  {
    mPipelineCache->RemovePipeline( this );
  }
}

uint32_t Pipeline::GetHashCode() const
{
  return mHashCode;
}

Vulkan::RefCountedPipeline Pipeline::GetVkPipeline() const
{
  return mVulkanPipelineState->pipeline;
}

const std::vector<vk::DescriptorSetLayout>& Pipeline::GetVkDescriptorSetLayouts() const
{
  return mVkDescriptorSetLayouts;
}

} // Internal
} // VulkanAPI
} // Graphics
} // Dal

