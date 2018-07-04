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
#include <dali/graphics/vulkan/api/vulkan-api-pipeline.h>

#if DEBUG_ENABLED
#include <sstream>
#endif

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

bool PipelineCache::SavePipeline( const VulkanAPI::PipelineFactory& factory, std::unique_ptr< Internal::Pipeline > pipeline )
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

#if DEBUG_ENABLED
std::string PipelineCache::DebugPrintPipelineInfo( const PipelineFactory::Info& info ) const
{
  std::stringstream ss;
  // shader
  ss << "  Shader: " << info.shaderState.shaderProgram << std::endl;

  // vertex input state
  ss << "  VI    : " << std::endl;
  {
    // attributes
    int k = 0;
    for (auto &&i : info.vertexInputState.attributes)
    {
      ss << "    attribute[" << k++ << "]" << std::endl;
      ss << "       binding  = " << i.binding << std::endl;
      ss << "       offset   = " << i.offset << std::endl;
      ss << "       location = " << i.location << std::endl;
    }
    // bindings
    k = 0;
    for (auto &&i : info.vertexInputState.bufferBindings)
    {
      ss << "    binding[" << k++ << "]" << std::endl;
      ss << "       inputRate = " << API::Debug::str(i.inputRate) << std::endl;
      ss << "       stride    = " << i.stride << std::endl;
    }
  }

  // vertex input state
  ss << "  IA    : " << std::endl;
  ss << "    primitiveRestartEnable: " << info.inputAssemblyState.primitiveRestartEnable << std::endl;
  ss << "    topology              : " << API::Debug::str(info.inputAssemblyState.topology) << std::endl;

  // depth/stencil
  const auto& ds = info.depthStencilState;
  ss << "  DS    : " << std::endl;
  ss << "    depthTestEnable: " << ds.depthTestEnable << std::endl;
  ss << "    depthWriteEnable: " << ds.depthWriteEnable << std::endl;
  ss << "    depthCompareOp: " << API::Debug::str(ds.depthCompareOp) << std::endl;
  ss << "    stencilTestEnable: " << ds.stencilTestEnable << std::endl;

  // rasterization state
  const auto& rs = info.rasterizationState;
  ss << "  RS    : " << std::endl;
  ss << "    polygonMode : " << API::Debug::str(rs.polygonMode) << std::endl;
  ss << "    frontFace   : " << API::Debug::str(rs.frontFace) << std::endl;
  ss << "    cullMode    : " << API::Debug::str(rs.cullMode) << std::endl;

  const auto& cb = info.colorBlendState;
  ss << "  CB    : " << std::endl;
  ss << "    colorBlendOp        : " << API::Debug::str(cb.colorBlendOp) << std::endl;
  ss << "    alphaBlendOp        : " << API::Debug::str(cb.alphaBlendOp) << std::endl;
  ss << "    blendConstants      : " << cb.blendConstants[0] << ", " << cb.blendConstants[1] << ", " << cb.blendConstants[2] << ", " << cb.blendConstants[3] << std::endl;
  ss << "    srcColorBlendFactor : " << API::Debug::str(cb.srcColorBlendFactor) << std::endl;
  ss << "    srcAlphaBlendFactor : " << API::Debug::str(cb.srcAlphaBlendFactor) << std::endl;
  ss << "    dstColorBlendFactor : " << API::Debug::str(cb.dstColorBlendFactor) << std::endl;
  ss << "    dstAlphaBlendFactor : " << API::Debug::str(cb.dstAlphaBlendFactor) << std::endl;
  ss << "    blendEnable         : " << cb.blendEnable << std::endl;
  ss << "    logicOpEnable       : " << cb.logicOpEnable << std::endl;
  ss << "    logicOp             : " << "UNDEFINED" << std::endl;
  ss << "    colorComponentWriteBits : " << cb.colorComponentWriteBits << std::endl;

  const auto& vp = info.viewportState;
  ss << "  VP    : " << std::endl;
  ss << "    viewport        : { " << vp.viewport.x << ", "
     << vp.viewport.y << ", "
     << vp.viewport.width << ", "
     << vp.viewport.height << " } " << std::endl;
  return ss.str();
}

std::string PipelineCache::DebugPrintPipeline( const VulkanAPI::Pipeline& pipeline ) const
{
  std::stringstream ss;
  auto impl = pipeline.GetImplementation();

  // find pipeline in cache
  const PipelineFactory::Info* info { nullptr };
  for( auto&& i : mCacheMap )
  {
    for (auto&& j : i.second)
    {
      if( j.pipelineImpl.get() == impl )
      {
        info = j.info.get();
        break;
      }
    }
    if( info )
    {
      break;
    }
  }

  if(!info)
  {
    return "Invalid pipeline!";
  }
  ss << "Pipeline id = " << &pipeline << ":" << std::endl;
  ss << DebugPrintPipelineInfo(*info);
  return ss.str();
}

std::string PipelineCache::DebugPrint() const
{
  std::stringstream ss;
  for( auto&& cacheEntry : mCacheMap )
  {
    for( auto&& info: cacheEntry.second )
    {
      ss << "Pipeline id = " << info.pipelineImpl.get() << ":" << std::endl;
      ss << DebugPrintPipelineInfo( *info.info.get() );
    }
  }
  return ss.str();
}

#endif
} // VulkanAPI
} // Graphics
} // Dali
