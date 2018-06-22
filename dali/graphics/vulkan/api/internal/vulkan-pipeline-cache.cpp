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

#include <dali/graphics/vulkan/internal/vulkan-pipeline.h>
#include <dali/graphics/vulkan/api/internal/vulkan-pipeline-cache.h>
#include <dali/graphics/vulkan/api/internal/vulkan-api-pipeline-impl.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

PipelineCache::PipelineCache( Vulkan::Graphics& graphics, Controller& controller )
{

}

PipelineCache::~PipelineCache() = default;

Internal::Pipeline* PipelineCache::GetPipeline( const VulkanAPI::PipelineFactory& factory ) const
{
  auto hashCode = factory.GetHashCode();
  auto iter = mCacheMap.find( hashCode );
  if( iter == mCacheMap.end() )
  {
    return nullptr;
  }

  auto& entries = iter->second;

  // return quickly if there's only one entry
  if( entries.size() == 1 )
  {
    return entries[0].pipelineImpl.get();
  }

  for( auto&& entry : entries )
  {
    if( !std::memcmp( &entry.info, &factory.GetCreateInfo(), sizeof( entry.info ) ) )
    {
      return entry.pipelineImpl.get();
    }
  }

  return nullptr;
}

bool
PipelineCache::SavePipeline( const VulkanAPI::PipelineFactory& factory, std::unique_ptr< Internal::Pipeline > pipeline )
{
  // hash factory
  auto hashCode = factory.GetHashCode();
  auto iter = mCacheMap.find( hashCode );
  std::vector< CacheEntry >* cacheEntries{ nullptr };

  if( iter == mCacheMap.end() )
  {
    mCacheMap[hashCode] = std::vector< CacheEntry >{};
    cacheEntries = &mCacheMap[hashCode];
  }
  else
  {
    cacheEntries = &iter->second;
  }

  // pass the ownership to the cache
  CacheEntry entry;
  entry.pipelineImpl = std::move( pipeline );
  entry.info = std::unique_ptr< PipelineFactory::Info >( new PipelineFactory::Info( factory.GetCreateInfo() ) );
  cacheEntries->push_back( std::move( entry ) );
  return true;
}

bool PipelineCache::RemovePipeline( Internal::Pipeline* pipeline )
{
  auto hashCode = pipeline->GetHashCode();
  auto iter = mCacheMap.find( hashCode );
  if( iter == mCacheMap.end() )
  {
    return false;
  }

  // delete pipeline
  // todo: for now use brute force, later - safe delete
  if( iter->second.size() == 1 )
  {
    iter->second.clear();
  }
  else
  {
    int32_t i = 0;
    for( auto&& entry : iter->second )
    {
      if( entry.pipelineImpl.get() == pipeline )
      {
        iter->second.erase( iter->second.begin() + i );
        break;
      }
      ++i;
    }
  }

  return true;
}

} // VulkanAPI
} // Graphics
} // Dali
