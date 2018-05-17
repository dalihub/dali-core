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

#include <dali/graphics/vulkan/vulkan-resource-cache.h>

#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-sampler.h>

#include <algorithm>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

ResourceCache& ResourceCache::AddBuffer( BufferRef buffer )
{
  mBuffers.push_back( buffer );
  return *this;
}

ResourceCache& ResourceCache::AddImage( ImageRef image )
{
  mImages.push_back( image );
  return *this;
}

ResourceCache& ResourceCache::AddPipeline( PipelineRef pipeline )
{
  mPipelines.push_back( pipeline );
  return *this;
}

ResourceCache& ResourceCache::AddShader( ShaderRef shader )
{
  mShaders.push_back( shader );
  return *this;
}

ResourceCache& ResourceCache::AddCommandPool( CommandPoolRef pool )
{
  mCommandPools.push_back( pool );
  return *this;
}

ResourceCache& ResourceCache::AddDescriptorPool( DescriptorPoolRef pool )
{
  mDescriptorPools.push_back( pool );
  return *this;
}

ResourceCache& ResourceCache::AddFramebuffer( FramebufferRef framebuffer )
{
  mFramebuffers.push_back( framebuffer );
  return *this;
}

ResourceCache& ResourceCache::AddSampler( SamplerRef sampler )
{
  mSamplers.push_back(sampler);
  return *this;
}

PipelineRef ResourceCache::FindPipeline( vk::Pipeline pipeline )
{
  auto iterator = std::find_if(mPipelines.begin(),
                               mPipelines.end(),
                               [&](const PipelineRef entry) { return entry->GetVkHandle() == pipeline; });

  return iterator == mPipelines.end() ? PipelineRef() : PipelineRef(&**iterator);
}

ShaderRef ResourceCache::FindShader( vk::ShaderModule shaderModule )
{
  auto iterator = std::find_if(mShaders.begin(),
                               mShaders.end(),
                               [&](const ShaderRef entry) { return entry->GetVkHandle() == shaderModule; });

  return iterator == mShaders.end() ? ShaderRef() : ShaderRef(&**iterator);
}

CommandPoolRef ResourceCache::FindCommandPool( vk::CommandPool commandPool )
{
  auto iterator = std::find_if(mCommandPools.begin(),
                               mCommandPools.end(),
                               [&](const CommandPoolRef entry) { return entry->GetVkHandle() == commandPool; });

  return iterator == mCommandPools.end() ? CommandPoolRef() : CommandPoolRef(&**iterator);
}

DescriptorPoolRef ResourceCache::FindDescriptorPool( vk::DescriptorPool descriptorPool )
{
  auto iterator = std::find_if(mDescriptorPools.begin(),
                               mDescriptorPools.end(),
                               [&](const DescriptorPoolRef entry) { return entry->GetVkHandle() == descriptorPool; });

  return iterator == mDescriptorPools.end() ? DescriptorPoolRef() : DescriptorPoolRef(&**iterator);
}

FramebufferRef ResourceCache::FindFramebuffer( vk::Framebuffer framebuffer )
{
  auto iterator = std::find_if(mFramebuffers.begin(),
                               mFramebuffers.end(),
                               [&](const FramebufferRef entry) { return entry->GetVkHandle() == framebuffer; });

  return iterator == mFramebuffers.end() ? FramebufferRef() : FramebufferRef(&**iterator);
}

SamplerRef ResourceCache::FindSampler( vk::Sampler sampler )
{
  auto iterator = std::find_if(mSamplers.begin(),
                               mSamplers.end(),
                               [&](const SamplerRef entry) { return entry->GetVkHandle() == sampler; });

  return iterator == mSamplers.end() ? SamplerRef() : SamplerRef(&**iterator);
}

BufferRef ResourceCache::FindBuffer( vk::Buffer buffer )
{
  auto iterator = std::find_if(mBuffers.begin(),
                               mBuffers.end(),
                               [&](const BufferRef entry) { return entry->GetVkHandle() == buffer; });

  return iterator == mBuffers.end() ? BufferRef() : BufferRef(&**iterator);
}

ImageRef ResourceCache::FindImage( vk::Image image )
{
  auto iterator = std::find_if(mImages.begin(),
                               mImages.end(),
                               [&](const ImageRef entry) { return entry->GetVkHandle() == image; });

  return iterator == mImages.end() ? ImageRef() : ImageRef(&**iterator);
}

ResourceCache& ResourceCache::RemoveBuffer( Buffer& buffer )
{
  if( !mBuffers.empty() )
  {
    auto found = std::find_if(mBuffers.begin(),
                              mBuffers.end(),
                              [&](const BufferRef entry) { return &(*entry) == &buffer; });

    std::iter_swap(found, std::prev(mBuffers.end()));
    mBuffers.back().Reset();
    mBuffers.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveImage( Image& image )
{
  if( !mImages.empty() )
  {
    auto found = std::find_if(mImages.begin(),
                              mImages.end(),
                              [&](const ImageRef entry) { return &(*entry) == &image; });

    std::iter_swap(found, std::prev(mImages.end()));
    mImages.back().Reset();
    mImages.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemovePipeline( Pipeline &pipeline )
{
  if( !mPipelines.empty() )
  {
    auto found = std::find_if(mPipelines.begin(),
                              mPipelines.end(),
                              [&](const PipelineRef entry) { return &(*entry) == &pipeline; });

    std::iter_swap(found, std::prev(mPipelines.end()));
    mPipelines.back().Reset();
    mPipelines.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveShader( Shader& shader )
{
  if( !mShaders.empty() )
  {
    auto iterator = std::find_if(mShaders.begin(),
                                 mShaders.end(),
                                 [&](const ShaderRef entry) { return &*entry == &shader; });

    std::iter_swap(iterator, std::prev(mShaders.end()));
    mShaders.back().Reset();
    mShaders.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveCommandPool( CommandPool& commandPool )
{
  if( !mCommandPools.empty() )
  {
    auto iterator = std::find_if(mCommandPools.begin(),
                                 mCommandPools.end(),
                                 [&](const CommandPoolRef entry) { return &*entry == &commandPool; });

    std::iter_swap(iterator, std::prev(mCommandPools.end()));
    mCommandPools.back().Reset();
    mCommandPools.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveDescriptorPool( DescriptorPool& descriptorPool )
{
  if( !mDescriptorPools.empty() )
  {
    auto iterator = std::find_if(mDescriptorPools.begin(),
                                 mDescriptorPools.end(),
                                 [&](const DescriptorPoolRef entry) { return &*entry == &descriptorPool; });

    std::iter_swap(iterator, std::prev(mDescriptorPools.end()));
    mDescriptorPools.back().Reset();
    mDescriptorPools.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveFramebuffer( Framebuffer &framebuffer )
{
  if( !mFramebuffers.empty() )
  {
    auto iterator = std::find_if(mFramebuffers.begin(),
                                 mFramebuffers.end(),
                                 [&](const FramebufferRef entry) { return &*entry == &framebuffer; });

    std::iter_swap(iterator, std::prev(mFramebuffers.end()));
    mFramebuffers.back().Reset();
    mFramebuffers.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveSampler( Sampler &sampler )
{
  if( !mSamplers.empty() )
  {
    auto iterator = std::find_if(mSamplers.begin(),
                                 mSamplers.end(),
                                 [&](const SamplerRef entry) { return &*entry == &sampler; });

    std::iter_swap(iterator, std::prev(mSamplers.end()));
    mSamplers.back().Reset();
    mSamplers.pop_back();
  }
  return *this;
}

} //namespace Vulkan
} //namespace Graphics
} //namespace Dali
