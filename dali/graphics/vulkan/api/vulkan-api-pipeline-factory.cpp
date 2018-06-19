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

#include <dali/graphics/vulkan/api/vulkan-api-pipeline-factory.h>
#include <dali/graphics/vulkan/api/internal/vulkan-pipeline-cache.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

namespace
{
/**
 * Hashing binary data, it may not be the best algorithm
 * @return
 */
uint32_t HashBinary( const void* dataObj, uint32_t size )
{
  const uint8_t* data = reinterpret_cast<const uint8_t*>(dataObj);
  uint32_t hash = 5381u;
  uint32_t c;
  for( uint32_t i = 0; i < size; ++i )
  {
    c = *data++;
    hash = ( ( hash << 5 ) + hash ) + c; /* hash * 33 + c */
  }
  return hash;
}

uint32_t HashPipeline( const VulkanAPI::PipelineFactory::Info& info )
{
  uint32_t dsHash = HashBinary( &info.depthStencilState, sizeof( info.depthStencilState ) );
  uint32_t cbHash = HashBinary( &info.colorBlendState, sizeof( info.colorBlendState ) );
  uint32_t shHash = HashBinary( &info.shaderState, sizeof( info.shaderState ) );
  uint32_t vpHash = HashBinary( &info.viewportState, sizeof( info.viewportState ) );
  uint32_t rsHash = HashBinary( &info.rasterizationState, sizeof( info.rasterizationState ) );
  uint32_t iaHash = HashBinary( &info.inputAssemblyState, sizeof( info.inputAssemblyState ) );

  // vertex input contains std containers so has to be hashed differently
  uint32_t viStateBindingsHash = HashBinary( info.vertexInputState.bufferBindings.data(), uint32_t(
          sizeof( API::VertexInputState::Binding ) * info.vertexInputState.bufferBindings.size() ) );
  uint32_t viStateAttributesHash = HashBinary( info.vertexInputState.attributes.data(), uint32_t(
          sizeof( API::VertexInputState::Attribute ) * info.vertexInputState.attributes.size() ) );

  // rehash all
  std::array< uint32_t, 8 > allHashes = {
          dsHash, cbHash, shHash, vpHash, rsHash, iaHash, viStateBindingsHash, viStateAttributesHash
  };

  return HashBinary( allHashes.data(), uint32_t( allHashes.size() * sizeof( uint32_t ) ) );
}

}

PipelineFactory::PipelineFactory( Controller& controller )
        : mController( controller ),
          mGraphics( controller.GetGraphics() ),
          mHashCode( 0u )
{
}

API::PipelineFactory& PipelineFactory::SetPipelineCache( VulkanAPI::PipelineCache& pipelineCache )
{
  mPipelineCache = dynamic_cast<VulkanAPI::PipelineCache*>(&pipelineCache);
  return *this;
}


API::PipelineFactory& PipelineFactory::SetColorBlendState( const API::ColorBlendState& state )
{
  mInfo.colorBlendState = state;
  mHashCode = 0u;
  return *this;
}


API::PipelineFactory& PipelineFactory::SetShaderState( const API::ShaderState& state )
{
  mInfo.shaderState = state;
  mHashCode = 0u;
  return *this;
}

API::PipelineFactory& PipelineFactory::SetViewportState( const API::ViewportState& state )
{
  mInfo.viewportState = state;
  mHashCode = 0u;
  return *this;
}

API::PipelineFactory& PipelineFactory::SetBasePipeline( API::Pipeline& pipeline )
{
  mBasePipeline = dynamic_cast<VulkanAPI::Pipeline*>(&pipeline);
  mHashCode = 0u;
  return *this;
}

API::PipelineFactory& PipelineFactory::SetDepthStencilState( API::DepthStencilState state )
{
  mInfo.depthStencilState = state;
  mHashCode = 0u;
  return *this;
}

API::PipelineFactory& PipelineFactory::SetRasterizationState( const API::RasterizationState& state )
{
  mInfo.rasterizationState = state;
  mHashCode = 0u;
  return *this;
}

API::PipelineFactory& PipelineFactory::SetVertexInputState( const API::VertexInputState& state )
{
  mInfo.vertexInputState = state;
  mHashCode = 0u;
  return *this;
}

API::PipelineFactory& PipelineFactory::SetInputAssemblyState( const API::InputAssemblyState& state )
{
  mInfo.inputAssemblyState = state;
  mHashCode = 0u;
  return *this;
}

std::unique_ptr< API::Pipeline > PipelineFactory::Create() const
{
  // todo: validate pipeline

  // check cache
  if( mPipelineCache )
  {
    auto ptr = mPipelineCache->GetPipeline( *this );
    // if pipeline is already in cache, attach implementation and return unique ptr
    if( ptr )
    {
      return std::unique_ptr< API::Pipeline >( new VulkanAPI::Pipeline( ptr ) );
    }
  }

  return std::unique_ptr< API::Pipeline >( new VulkanAPI::Pipeline( mGraphics, mController, this ) );
}

void PipelineFactory::Reset()
{
  mInfo.depthStencilState = {};
  mInfo.colorBlendState = {};
  mInfo.shaderState = {};
  mInfo.viewportState = {};
  mInfo.rasterizationState = {};
  mInfo.vertexInputState = {};
  mInfo.inputAssemblyState = {};
  mPipelineCache = nullptr;
  mBasePipeline = nullptr;
}

uint32_t PipelineFactory::GetHashCode() const
{
  if( !mHashCode )
  {
    mHashCode = HashPipeline( GetCreateInfo() );
  }
  return mHashCode;
}

}
}
}