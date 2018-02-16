/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

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

  vk::Result Initialise()
  {
    if( !ValidateShaderModules() )
    {
      return vk::Result::eErrorInitializationFailed;
    }

    CreatePipelineLayout();

    // use default render pass for default framebuffer
    // TODO: swapchain/surface should use vulkan-framebuffer object
    // in place of swapchain structures!
    if( !mInfo.renderPass )
    {
      SetRenderPass( mGraphics.GetSurface( 0 ).GetRenderPass() );
    }

    SetRasterizationState();

    SetDepthStencilState();

    SetMultisampleState();

    SetColorBlendState();

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
    //mAttachementNoBlendState.setBlendEnable( true );
    mAttachementNoBlendState.setColorWriteMask( vk::ColorComponentFlagBits::eR |
                                                  vk::ColorComponentFlagBits::eG |
                                                  vk::ColorComponentFlagBits::eB |
                                                  vk::ColorComponentFlagBits::eA );

    mColorBlendState.setBlendConstants( { 1.0f, 1.0f, 1.0f, 1.0f });
    mColorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    mColorBlendState.setAttachmentCount( 1 );
    mColorBlendState.setPAttachments( &mAttachementNoBlendState );
    mInfo.setPColorBlendState(&mColorBlendState);
  }

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
   *
   */
  void CreatePipelineLayout()
  {
    // pull desciptor set layouts from shaders
    auto layoutInfo = vk::PipelineLayoutCreateInfo{};

    //info.setPSetLayouts( vk::DescriptorSetLayout )

    std::vector<vk::DescriptorSetLayout> allLayouts{};
    for( auto&& shader : mShaderResources )
    {
      auto& layouts = shader->GetDescriptorSetLayouts();
      if( layouts.size() )
      {
        allLayouts.resize(layouts.size());
        for (auto i = 0u; i < layouts.size(); ++i)
        {
          if (layouts[i])
          {
            allLayouts[i] = layouts[i];
          }
        }
      }
    }

    layoutInfo.setPSetLayouts( allLayouts.data() );
    layoutInfo.setSetLayoutCount( static_cast<uint32_t>(allLayouts.size()) );

    mPipelineLayout = VkAssert( mGraphics.GetDevice().createPipelineLayout( layoutInfo, mGraphics.GetAllocator() ) );

    mInfo.setLayout( mPipelineLayout );
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
        mShaderResources.push_back( shaderHandle );
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
  std::vector<Handle<Shader>>     mShaderResources;

  vk::PipelineViewportStateCreateInfo mViewportState {};
  std::vector<vk::Viewport> mViewports {};
  vk::Rect2D mScissors {};

  std::vector<vk::PipelineShaderStageCreateInfo> mShaderStageCreateInfo;
  vk::PipelineLayout mPipelineLayout{};

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

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali