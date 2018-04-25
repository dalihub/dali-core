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

#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

namespace
{
static const vk::ShaderStageFlags DEFAULT_SHADER_STAGES{ vk::ShaderStageFlagBits::eVertex|vk::ShaderStageFlagBits::eFragment };
}

/**
 * Class Pipeline::Impl
 * Internal implementation of the pipeline
 */
struct Pipeline::Impl
{


  Impl( Vulkan::Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info ) :
    mInfo( info ),
    mGraphics( graphics )
  {
  };

  Impl() = default;
  ~Impl()
  {
    if(mPipelineLayout)
    {
      mGraphics.GetDevice().destroyPipelineLayout( mPipelineLayout, mGraphics.GetAllocator() );
    }
    if(mPipeline)
    {
      mGraphics.GetDevice().destroyPipeline( mPipeline, mGraphics.GetAllocator() );
    }
  }

  /**
   *
   * @return
   */
  vk::Pipeline GetVkPipeline() const
  {
    return mPipeline;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  vk::Result Initialise()
  {
    if( !ValidateShaderModules() )
    {
      return vk::Result::eErrorInitializationFailed;
    }

    if(!mInfo.layout)
    {
      CreatePipelineLayout();
    }

    // use default render pass for default framebuffer
    // TODO: swapchain/surface should use vulkan-framebuffer object
    // in place of swapchain structures!
    if( !mInfo.renderPass )
    {
      SetRenderPass( mGraphics.GetSwapchainForFBID(0u)->
                                GetCurrentFramebuffer()->GetVkRenderPass());
    }

    if(!mInfo.pRasterizationState)
    {
      SetRasterizationState();
    }

    if(!mInfo.pDepthStencilState)
    {
      SetDepthStencilState();
    }

    if(!mInfo.pMultisampleState)
    {
      SetMultisampleState();
    }


    if(!mInfo.pColorBlendState)
    {
      SetColorBlendState();
    }

    mInfo.setFlags( vk::PipelineCreateFlagBits::eAllowDerivatives );

    // create pipeline
    mPipeline = VkAssert( mGraphics.GetDevice().createGraphicsPipeline( nullptr, mInfo, mGraphics.GetAllocator() ) );
    if(mPipeline)
    {
      return vk::Result::eSuccess;
    }
    return vk::Result::eErrorInitializationFailed;
  }

  void SetRenderPass( vk::RenderPass renderpass )
  {
    mInfo.setRenderPass( renderpass );
  }

  void SetDepthStencilState()
  {
    mDepthStencilState = vk::PipelineDepthStencilStateCreateInfo{};
    mDepthStencilState.setDepthBoundsTestEnable( false );
    mDepthStencilState.setStencilTestEnable( false );
    mInfo.setPDepthStencilState( & mDepthStencilState );
  }

  void SetMultisampleState()
  {
    mMultisampleState = vk::PipelineMultisampleStateCreateInfo{};
    mMultisampleState.setSampleShadingEnable( false );
    mMultisampleState.setRasterizationSamples( vk::SampleCountFlagBits::e1);
    mMultisampleState.setAlphaToCoverageEnable( false );
    mMultisampleState.setMinSampleShading( 1.0f );
    mMultisampleState.setPSampleMask( nullptr );
    mInfo.setPMultisampleState( &mMultisampleState );
  }

  void SetVertexInputState(
    std::vector<vk::VertexInputAttributeDescription> attrDesc,
    std::vector<vk::VertexInputBindingDescription> bindingDesc)
  {
    mBindingDesc.clear();
    mAttrDesc.clear();
    mVertexInputState = vk::PipelineVertexInputStateCreateInfo{};
    mVertexInputState.setPVertexAttributeDescriptions( (mAttrDesc = attrDesc).data() );
    mVertexInputState.setPVertexBindingDescriptions( (mBindingDesc = bindingDesc).data() );
    mVertexInputState.setVertexAttributeDescriptionCount( U32(mAttrDesc.size()) );
    mVertexInputState.setVertexBindingDescriptionCount( U32(mBindingDesc.size()) );
    mInfo.setPVertexInputState( &mVertexInputState );
  }

  /**
   * Sets the viewport on uncompiled pipeline
   * @return
   */
  void SetViewport( float x, float y, float width, float height )
  {
    assert( !mPipeline && "Pipeline cannot be changed anymore!");

    // AB: add scissor, read data from graphics for fullscreen viewport
    // simplified mode for the demo purposes
    mViewports.emplace_back( x, y, width, height );
    mViewports[0].setMinDepth( 0.0f );
    mViewports[0].setMaxDepth( 1.0f );
    mScissors = vk::Rect2D( { static_cast<int32_t>(x), static_cast<int32_t>(y) },
                            { U32(width), U32(height) });
    mViewportState = vk::PipelineViewportStateCreateInfo{}.
           setViewportCount( U32(mViewports.size()) ).
           setPViewports( mViewports.data() ).
           setPScissors( &mScissors ).
           setScissorCount( 1 );

    // replace viewport state
    mInfo.setPViewportState( &mViewportState );
  }

  /**
   *
   * @param topology
   * @param restartEnable
   */
  void SetInputAssemblyState( vk::PrimitiveTopology topology, bool restartEnable )
  {
    mInputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    mInputAssemblyState.setPrimitiveRestartEnable( restartEnable );
    mInputAssemblyState.setTopology( topology );
    mInfo.setPInputAssemblyState( &mInputAssemblyState );
  }

  void SetRasterizationState()
  {
    mRasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    mRasterizationState.setCullMode( vk::CullModeFlagBits::eNone );
    mRasterizationState.setDepthBiasClamp( 0.0f );
    mRasterizationState.setDepthBiasEnable( false );
    mRasterizationState.setDepthClampEnable( false );
    mRasterizationState.setFrontFace( vk::FrontFace::eCounterClockwise );
    mRasterizationState.setPolygonMode( vk::PolygonMode::eFill );
    mRasterizationState.setRasterizerDiscardEnable( false );
    mRasterizationState.setLineWidth( 1.0f );
    mInfo.setPRasterizationState( & mRasterizationState );
  }

  void SetColorBlendState()
  {
    mAttachementNoBlendState = vk::PipelineColorBlendAttachmentState{};
    mAttachementNoBlendState.setBlendEnable( true );
    mAttachementNoBlendState.setColorWriteMask( vk::ColorComponentFlagBits::eR |
                                                  vk::ColorComponentFlagBits::eG |
                                                  vk::ColorComponentFlagBits::eB |
                                                  vk::ColorComponentFlagBits::eA );
    mAttachementNoBlendState.setSrcColorBlendFactor( vk::BlendFactor::eSrcAlpha );
    mAttachementNoBlendState.setDstColorBlendFactor( vk::BlendFactor::eOneMinusSrc1Alpha );
    mAttachementNoBlendState.setSrcAlphaBlendFactor( vk::BlendFactor::eOne );
    mAttachementNoBlendState.setDstAlphaBlendFactor( vk::BlendFactor::eOneMinusSrc1Alpha );
    mAttachementNoBlendState.setColorBlendOp( vk::BlendOp::eAdd );
    mAttachementNoBlendState.setAlphaBlendOp( vk::BlendOp::eAdd );

    mColorBlendState.setBlendConstants( { 1.0f, 1.0f, 1.0f, 1.0f });
    mColorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    mColorBlendState.setAttachmentCount( 1 );
    mColorBlendState.setPAttachments( &mAttachementNoBlendState );
    mInfo.setPColorBlendState(&mColorBlendState);
  }
#pragma GCC diagnostic pop

  /**
   * Sets the shader. Must be set before compiling the pipeline, compiled pipeline
   * becomes immutable.
   * @param shader
   * @param stage
   * @return
   */
  bool SetShader( ShaderRef shader, Shader::Type stage )
  {
    assert( !mPipeline && "Pipeline cannot be changed anymore!");

    // check if shader isn't orphaned for some reason
    if( !mGraphics.FindShader( *shader ) )
    {
      return false;
    }

    auto info = vk::PipelineShaderStageCreateInfo{}.
                                                     setModule( *shader ).
                                                     setStage( static_cast<vk::ShaderStageFlagBits>( stage ) ).
                                                     setPName( "main" );
    mShaderStageCreateInfo.push_back( info );
    mShaderResources.push_back( shader );

    mInfo.setPStages( mShaderStageCreateInfo.data() );
    mInfo.setStageCount( static_cast<uint32_t>(mShaderStageCreateInfo.size()) );

    return false;
  }

  /**
   * Creates deferred pipeline layout. Since not all the shader modules
   * are supplied in one go the layout creation first must instantiate
   * correct descriptor set layouts.
   *
   * @todo: Store SPIRV data of shader modules in the cache rather than
   * parsing every time
   */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  void CreatePipelineLayout()
  {

    // pull desciptor set layouts from shaders
    auto layoutInfo = vk::PipelineLayoutCreateInfo{};

    using DSLayoutBindingArray = std::vector<vk::DescriptorSetLayoutBinding>;

    std::vector<DSLayoutBindingArray> allDescriptorSetLayouts;

    // concatenate all bindings
    // TODO: @todo validate if there are weird overlaps!
    for( auto&& shader : mShaderResources )
    {
      const auto& reflection = shader->GetSPIRVReflection();
      auto layouts = reflection.GenerateDescriptorSetLayoutCreateInfo();

      if(allDescriptorSetLayouts.size() < layouts.size())
      {
        allDescriptorSetLayouts.resize(layouts.size());
      }

      for( auto i = 0u; i < layouts.size(); ++i )
      {
        auto currIndex = allDescriptorSetLayouts[i].size();
        allDescriptorSetLayouts[i].insert( allDescriptorSetLayouts[i].end(),
        layouts[i].pBindings, layouts[i].pBindings + layouts[i].bindingCount );
        for( auto j = 0u; j < layouts[i].bindingCount; ++j )
        {
          allDescriptorSetLayouts[i][j+currIndex].setStageFlags( GetShaderStage( shader ) );
        }
      }
    }

    // create descriptor set layouts for the pipeline
    std::vector<vk::DescriptorSetLayout> dsLayouts{};
    dsLayouts.resize( allDescriptorSetLayouts.size() );
    mDSCreateInfoArray.clear();
    for( auto i = 0u; i < allDescriptorSetLayouts.size(); ++i )
    {
      auto info = vk::DescriptorSetLayoutCreateInfo{}.
                    setBindingCount( static_cast<uint32_t>(allDescriptorSetLayouts[i].size()) ).
                    setPBindings( allDescriptorSetLayouts[i].data() );

      mDSCreateInfoArray.push_back( info );
      dsLayouts[i] = VkAssert( mGraphics.GetDevice().createDescriptorSetLayout( info, mGraphics.GetAllocator() ) );
    }

    // create pipeline layout
    layoutInfo.setPSetLayouts( dsLayouts.data() );
    layoutInfo.setSetLayoutCount( static_cast<uint32_t>(dsLayouts.size()) );

    mPipelineLayout = VkAssert( mGraphics.GetDevice().createPipelineLayout( layoutInfo, mGraphics.GetAllocator() ) );

    mDSLayoutArray = dsLayouts;
    mInfo.setLayout( mPipelineLayout );
  }
#pragma GCC diagnostic pop

  vk::ShaderStageFlagBits GetShaderStage( ShaderRef shader )
  {
    for( auto&& stage : mShaderStageCreateInfo )
    {
      if( stage.module == *shader )
      {
        return stage.stage;
      }
    }
    return vk::ShaderStageFlagBits{};
  }

  bool Compile()
  {
    return Initialise() == vk::Result::eSuccess;
  }

  bool ValidateShaderModules()
  {
    for( auto i = 0u; i < mInfo.stageCount; ++i )
    {
      const auto& stage = mInfo.pStages[i];
      auto shaderHandle = mGraphics.FindShader( stage.module );
      if( shaderHandle )
      {
        bool tracked { false };
        for(auto&& sh : mShaderResources )
        {
          if( sh == shaderHandle )
          {
            tracked = true;
          }
        }
        if(!tracked)
        {
          mShaderResources.push_back(shaderHandle);
        }
      }
      else
      {
        return false; // invalid shader! Can't track it
      }
    }
    return true;
  }


  vk::GraphicsPipelineCreateInfo  mInfo           {    };
  vk::Pipeline                    mPipeline       { nullptr };
  uint32_t                        mModified       { 0u };
  Graphics&                       mGraphics;

  // resources
  std::vector<ShaderRef>     mShaderResources;

  vk::PipelineViewportStateCreateInfo mViewportState {};
  std::vector<vk::Viewport> mViewports {};
  vk::Rect2D mScissors {};

  std::vector<vk::PipelineShaderStageCreateInfo> mShaderStageCreateInfo;
  vk::PipelineLayout mPipelineLayout{};
  std::vector<vk::DescriptorSetLayoutCreateInfo>    mDSCreateInfoArray{};
  std::vector<vk::DescriptorSetLayout>              mDSLayoutArray{};

  // vertex input state
  vk::PipelineVertexInputStateCreateInfo            mVertexInputState {};
  std::vector<vk::VertexInputAttributeDescription>  mAttrDesc;
  std::vector<vk::VertexInputBindingDescription>    mBindingDesc;

  // vertex input assembly state
  vk::PipelineInputAssemblyStateCreateInfo          mInputAssemblyState {};

  // rasterization state
  vk::PipelineRasterizationStateCreateInfo          mRasterizationState {};

  // Dpeth/stencil state
  vk::PipelineDepthStencilStateCreateInfo           mDepthStencilState {};

  // Multisample state
  vk::PipelineMultisampleStateCreateInfo            mMultisampleState {};

  // Color blend
  vk::PipelineColorBlendStateCreateInfo             mColorBlendState {};
  vk::PipelineColorBlendAttachmentState             mAttachementNoBlendState {};


};

/*********************************************************************
 * Class Pipeline
 *
 */

PipelineRef Pipeline::New( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info )
{
  auto pipeline = Handle<Pipeline>( new Pipeline(graphics, info) );
  graphics.AddPipeline(pipeline);
  return pipeline;
}

Pipeline::~Pipeline() = default;

Pipeline::Pipeline( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info )
{
  mImpl = MakeUnique<Pipeline::Impl>( graphics, info );
}

vk::Pipeline Pipeline::GetVkPipeline() const
{
  return mImpl->GetVkPipeline();
}

bool Pipeline::OnDestroy()
{
  return false;
}

void Pipeline::SetViewport( float x, float y, float width, float height )
{
  mImpl->SetViewport( x, y, width, height );
}

bool Pipeline::SetShader( ShaderRef shader, Shader::Type stage )
{
  return mImpl->SetShader( shader, stage );
}

void Pipeline::SetVertexInputState(std::vector<vk::VertexInputAttributeDescription> attrDesc,
                         std::vector<vk::VertexInputBindingDescription> bindingDesc)
{
  mImpl->SetVertexInputState( attrDesc, bindingDesc );
}

void Pipeline::SetInputAssemblyState( vk::PrimitiveTopology topology, bool restartEnable )
{
  mImpl->SetInputAssemblyState( topology, restartEnable );
}

bool Pipeline::Compile()
{
  return mImpl->Compile();
}

vk::PipelineLayout Pipeline::GetVkPipelineLayout() const
{
  return mImpl->mPipelineLayout;
}

const std::vector<vk::DescriptorSetLayoutCreateInfo>& Pipeline::GetVkDescriptorSetLayoutCreateInfo() const
{
  return mImpl->mDSCreateInfoArray;
}

const std::vector<vk::DescriptorSetLayout>& Pipeline::GetVkDescriptorSetLayouts() const
{
  return mImpl->mDSLayoutArray;
}

const vk::PipelineInputAssemblyStateCreateInfo& Pipeline::GetInputAssemblyState() const
{
  return mImpl->mInputAssemblyState;
}

const vk::PipelineVertexInputStateCreateInfo& Pipeline::GetVertexInputState() const
{
  return mImpl->mVertexInputState;
}

const vk::PipelineViewportStateCreateInfo& Pipeline::GetViewportState() const
{
  return mImpl->mViewportState;
}

const vk::PipelineRasterizationStateCreateInfo& Pipeline::GetRasterizationState() const
{
  return mImpl->mRasterizationState;
}

const vk::PipelineMultisampleStateCreateInfo& Pipeline::GetMultisamplingState() const
{
  return mImpl->mMultisampleState;
}

const vk::PipelineDepthStencilStateCreateInfo& Pipeline::GetDepthStencilState() const
{
  return mImpl->mDepthStencilState;
}

const vk::PipelineColorBlendStateCreateInfo& Pipeline::GetColorBlendState() const
{
  return mImpl->mColorBlendState;
}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali