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

#include <dali/graphics/vulkan/api/vulkan-api-pipeline.h>
#include <dali/graphics/vulkan/api/internal/vulkan-api-pipeline-impl.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline-factory.h>
#include <dali/graphics/vulkan/api/internal/vulkan-pipeline-cache.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

Pipeline::Pipeline( Vulkan::Graphics& graphics, Controller& controller, const PipelineFactory* factory )
{
  mPipelineImpl = new Internal::Pipeline( graphics, controller, factory );

  // ownership on the implementation is given away to cache
  factory->mPipelineCache->SavePipeline( *factory, std::unique_ptr< Internal::Pipeline >( mPipelineImpl ) );

  mPipelineImpl->Reference();
}

Pipeline::~Pipeline()
{
  mPipelineImpl->Dereference();
}

Pipeline::Pipeline( Internal::Pipeline* impl ) :
        mPipelineImpl( impl )
{
  mPipelineImpl->Reference();
}

Vulkan::RefCountedPipeline Pipeline::GetVkPipeline() const
{
  return mPipelineImpl->GetVkPipeline();
}

const std::vector< vk::DescriptorSetLayout >& Pipeline::GetVkDescriptorSetLayouts() const
{
  return mPipelineImpl->GetVkDescriptorSetLayouts();
}

const std::vector< Vulkan::DescriptorSetLayoutSignature >& Pipeline::GetDescriptorSetLayoutSignatures() const
{
  return mPipelineImpl->GetDescriptorSetLayoutSignatures();
}

API::PipelineDynamicStateMask Pipeline::GetDynamicStateMask() const
{
  return mPipelineImpl->GetDynamicStateMask();
}


}
}
}
