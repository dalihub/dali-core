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

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-pipeline-cache.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-shader.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

struct PipelineCache::Impl
{
  struct PipelineCacheItem
  {

    PipelineCacheItem( PipelineRef _pipeline, const PipelineDescription& _description )
      : pipeline(_pipeline), description(_description)
    {

    }
    PipelineRef                           pipeline;
    PipelineDescription                   description;
  };

  Impl( PipelineCache& pipelineCache, Graphics& graphics )
  : mPipelineCache( pipelineCache ),
    mGraphics( graphics )
  {

  }


  /**
   * Finds suitable cached pipeline based on pipelineInfo
   * @param pipelineInfo
   * @return
   */
  PipelineRef GetPipeline( const PipelineDescription& description )
  {
    for( auto&& item : mPipelines )
    {
      if( item.description == description )
      {
        return item.pipeline;
      }
    }
    return PipelineRef();
  }


  bool AddPipeline( PipelineRef pipeline, const PipelineDescription& description )
  {
    mPipelines.emplace_back( pipeline, description );
    return true;
  }

  std::vector<vk::DescriptorSetLayout> GetDescriptorSetLayouts( const PipelineRef& pipeline ) const
  {
    auto retval = std::vector<vk::DescriptorSetLayout>{};
    for( auto&& item : mPipelines )
    {
      if( item.pipeline == pipeline )
      {
        return item.description.descriptorSetLayouts;
      }
    }
    return retval;
  }

  ~Impl() = default;

  PipelineCache&  mPipelineCache;
  Graphics&       mGraphics;

  std::vector<PipelineCacheItem> mPipelines;
};

PipelineCache::PipelineCache( Graphics& graphics )
{
  mImpl = std::make_unique<Impl>( *this, graphics );
}

PipelineCache::~PipelineCache() = default;

PipelineRef PipelineCache::GetPipeline( const PipelineDescription& desc ) const
{
  return mImpl->GetPipeline( desc );
}

bool PipelineCache::AddPipeline( PipelineRef pipeline, const PipelineDescription& desc )
{
  return mImpl->AddPipeline( pipeline,desc );
}

std::vector<vk::DescriptorSetLayout> PipelineCache::GetDescriptorSetLayouts( const PipelineRef& pipeline ) const
{
  return mImpl->GetDescriptorSetLayouts( pipeline );
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

