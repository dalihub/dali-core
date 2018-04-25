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
    PipelineRef                     pipeline;
    PipelineRef                     parentPipeline; // todo: implement pipeline inheritance
    vk::GraphicsPipelineCreateInfo  createInfo;
    vk::PipelineLayoutCreateInfo    layoutCreateInfo;
    vk::PipelineLayout              layout;
    uint32_t                        hashValue;
  };

  Impl( PipelineCache& pipelineCache, Graphics& graphics )
  : mPipelineCache( pipelineCache ),
    mGraphics( graphics )
  {

  }

  /**
   * Creates new pipeline or returns existing one
   * @param pipeline
   */
  PipelineRef CreatePipeline( const vk::PipelineLayoutCreateInfo& layoutCreateInfo, const vk::GraphicsPipelineCreateInfo& info )
  {
    PipelineRef retval;
    if(retval = GetPipeline(info))
    {
      return retval;
    }

    auto createInfo = vk::GraphicsPipelineCreateInfo{ info };

    auto device = mGraphics.GetDevice();
    auto allocator = mGraphics.GetAllocator();

    auto layout = VkAssert( device.createPipelineLayout( layoutCreateInfo, allocator ) );
    createInfo.layout = layout;

    auto pipeline = Pipeline::New( mGraphics, createInfo );
    auto cacheItem = PipelineCacheItem{};
    cacheItem.pipeline = pipeline;
    cacheItem.layout = layout;
    cacheItem.layoutCreateInfo = layoutCreateInfo;
    cacheItem.createInfo = createInfo;
    cacheItem.hashValue = 0;

    retval = pipeline;

    mPipelines.emplace_back( cacheItem );

    return retval;
  }

  /**
   * Finds suitable cached pipeline based on pipelineInfo
   * @param pipelineInfo
   * @return
   */
  PipelineRef GetPipeline( const vk::GraphicsPipelineCreateInfo& pipelineInfo )
  {
    return PipelineRef();
  }

  /**
   * Deletes cached pipeline( may not delete actual pipeline )
   * @param pipeline
   * @return
   */
  bool DiscardPipeline( const PipelineRef& pipeline )
  {
    return false;
  }

  /**
   * Checks whether pipeline has been previously registered with cache
   * @param pipeline
   * @return
   */
  bool PipelineExists( const PipelineRef& pipeline )
  {
    return false;
  }

  /**
   * Retrieves pipeline create info structure
   * @param pipeline
   * @param outInfo
   * @return
   */
  bool GetGraphicsPipelineCreateInfo( const PipelineRef& pipeline, vk::GraphicsPipelineCreateInfo& outInfo )
  {
    return false;
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

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

