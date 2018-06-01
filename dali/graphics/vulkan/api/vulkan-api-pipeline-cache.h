#ifndef DALI_GRAPHICS_VULKAN_API_PIPELINE_CACHE_H
#define DALI_GRAPHICS_VULKAN_API_PIPELINE_CACHE_H

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

#include <dali/graphics-api/graphics-api-pipeline.h>
#include <dali/graphics-api/graphics-api-pipeline-cache.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline-factory.h>

#include <memory>
#include <map>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
} // Vulkan

namespace VulkanAPI
{
namespace Internal
{
class Pipeline;
} // Internal

class Controller;
class PipelineFactory;

class PipelineCache : public API::PipelineCache
{
public:

  PipelineCache( Vulkan::Graphics& graphics, Controller& controller );

  ~PipelineCache() override;

  /**
   * Finds a pipeline based on the factory setup
   * @return pipeline implementation or nullptr if pipeline isn't part of cache
   */
  Internal::Pipeline* GetPipeline( const VulkanAPI::PipelineFactory& factory ) const;

  /**
   * Saves pipeline in the cache
   * @param pipeline
   * @return
   */
  bool SavePipeline( const VulkanAPI::PipelineFactory& factory, std::unique_ptr<Internal::Pipeline> pipeline );

  /**
   * Removes unused pipeline
   */
  bool RemovePipeline( Internal::Pipeline* pipeline );


private:

  struct CacheEntry
  {
    std::unique_ptr<Internal::Pipeline>     pipelineImpl;
    std::unique_ptr<PipelineFactory::Info>  info {}; // to compare if hash collision occurs
  };

  std::map<uint32_t, std::vector<CacheEntry>> mCacheMap;
};
} // VulkanAPI
} // Graphics
} // Dali

#endif // DALI_GRAPHICS_VULKAN_API_PIPELINE_CACHE_H
