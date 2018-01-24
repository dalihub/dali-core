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
    mGraphics( graphics ),
    mInfo( info )
  {
  };

  Impl() = default;
  ~Impl() = default;

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
    mPipeline = VkAssert( mGraphics.GetDevice().createGraphicsPipeline( nullptr, mInfo, mGraphics.GetAllocator() ) );
    if(mPipeline)
    {
      return vk::Result::eSuccess;
    }
    return vk::Result::eErrorInitializationFailed;
  }

  vk::GraphicsPipelineCreateInfo  mInfo           {    };
  vk::Pipeline                    mPipeline       { nullptr };
  uint32_t                        mModified       { 0u };
  Graphics&                       mGraphics;
};

/*********************************************************************
 * Class Pipeline
 *
 */

std::unique_ptr<Pipeline> Pipeline::New( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info )
{
  auto retval = std::unique_ptr<Pipeline>( new Pipeline(graphics, info) );
  if( vk::Result::eSuccess == retval->mImpl->Initialise() )
  {
    return retval;
  }
  return nullptr;
}

Pipeline::Pipeline( Graphics& graphics, const vk::GraphicsPipelineCreateInfo& info )
{
  mImpl = MakeUnique<Pipeline::Impl>( graphics, info );
}

vk::Pipeline Pipeline::GetVkPipeline() const
{
  return mImpl->GetVkPipeline();
}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali