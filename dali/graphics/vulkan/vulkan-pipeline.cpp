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
#include <dali/graphics/vulkan/vulkan-shader.h>

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

    // create pipeline
    mPipeline = VkAssert( mGraphics.GetDevice().createGraphicsPipeline( nullptr, mInfo, mGraphics.GetAllocator() ) );
    if(mPipeline)
    {
      return vk::Result::eSuccess;
    }
    return vk::Result::eErrorInitializationFailed;
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
    mViewportState = vk::PipelineViewportStateCreateInfo{}.
           setViewportCount( 1 ).
           setPViewports( &vk::Viewport{}.
                                          setWidth( width ).
                                          setHeight( height ).
                                          setY( y ).
                                          setX( x )).
           setPScissors( nullptr ).
           setScissorCount( 0 );

    // replace viewport state
    mInfo.setPViewportState( &mViewportState );
  }

  /**
   * Sets the shader. Must be set before compiling the pipeline, compiled pipeline
   * becomes immutable.
   * @param shader
   * @param stage
   * @return
   */
  bool SetShader( ShaderHandle shader, Shader::Type stage )
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
  std::vector<vk::PipelineShaderStageCreateInfo> mShaderStageCreateInfo;
  vk::PipelineLayout mPipelineLayout{};
};

/*********************************************************************
 * Class Pipeline
 *
 */

PipelineHandle Pipeline::New( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info )
{
  auto pipeline = std::unique_ptr<Pipeline>( new Pipeline(graphics, info) );
  PipelineHandle handle(pipeline.get());
  graphics.AddPipeline(std::move(pipeline));
  return handle;
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

bool Pipeline::SetShader( ShaderHandle shader, Shader::Type stage )
{
  return mImpl->SetShader( shader, stage );
}

bool Pipeline::Compile()
{
  return mImpl->Compile();
}


} // namespace Vulkan

} // namespace Graphics

} // namespace Dali