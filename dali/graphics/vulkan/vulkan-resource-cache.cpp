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
#include <dali/graphics/vulkan/vulkan-image-view.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-sampler.h>
#include <dali/graphics/vulkan/vulkan-fence.h>

#include <algorithm>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

ResourceCache& ResourceCache::AddBuffer( RefCountedBuffer buffer )
{
  mBuffers.push_back( buffer );
  return *this;
}

ResourceCache& ResourceCache::AddImage( RefCountedImage image )
{
  mImages.push_back( image );
  return *this;
}

ResourceCache& ResourceCache::AddImageView( RefCountedImageView imageView )
{
  mImageViews.push_back( imageView );
  return *this;
}

ResourceCache& ResourceCache::AddShader( RefCountedShader shader )
{
  mShaders.push_back( shader );
  return *this;
}

ResourceCache& ResourceCache::AddCommandPool( std::thread::id currentThreadId, RefCountedCommandPool pool )
{
  mCommandPools[currentThreadId] = pool;
  return *this;
}

ResourceCache& ResourceCache::AddDescriptorPool( RefCountedDescriptorPool pool )
{
  mDescriptorPools.push_back( pool );
  return *this;
}

ResourceCache& ResourceCache::AddFramebuffer( RefCountedFramebuffer framebuffer )
{
  mFramebuffers.push_back( framebuffer );
  return *this;
}

ResourceCache& ResourceCache::AddSampler( RefCountedSampler sampler )
{
  mSamplers.push_back( sampler );
  return *this;
}

ResourceCache& ResourceCache::AddFence( RefCountedFence fence )
{
  mFences.push_back( fence );
  return *this;
}

RefCountedShader ResourceCache::FindShader( vk::ShaderModule shaderModule )
{
  auto iterator = std::find_if(mShaders.begin(),
                               mShaders.end(),
                               [&](const RefCountedShader entry) { return entry->GetVkHandle() == shaderModule; });

  return iterator == mShaders.end() ? RefCountedShader() : RefCountedShader(&**iterator);
}

RefCountedCommandPool ResourceCache::FindCommandPool( std::thread::id currentThreadId )
{
  return mCommandPools.find( currentThreadId ) == mCommandPools.end() ? RefCountedCommandPool() : mCommandPools[ currentThreadId ];
}

RefCountedDescriptorPool ResourceCache::FindDescriptorPool( vk::DescriptorPool descriptorPool )
{
  auto iterator = std::find_if(mDescriptorPools.begin(),
                               mDescriptorPools.end(),
                               [&](const RefCountedDescriptorPool entry) { return entry->GetVkHandle() == descriptorPool; });

  return iterator == mDescriptorPools.end() ? RefCountedDescriptorPool() : RefCountedDescriptorPool(&**iterator);
}

RefCountedFramebuffer ResourceCache::FindFramebuffer( vk::Framebuffer framebuffer )
{
  auto iterator = std::find_if(mFramebuffers.begin(),
                               mFramebuffers.end(),
                               [&](const RefCountedFramebuffer entry) { return entry->GetVkHandle() == framebuffer; });

  return iterator == mFramebuffers.end() ? RefCountedFramebuffer() : RefCountedFramebuffer(&**iterator);
}

RefCountedSampler ResourceCache::FindSampler( vk::Sampler sampler )
{
  auto iterator = std::find_if(mSamplers.begin(),
                               mSamplers.end(),
                               [&](const RefCountedSampler entry) { return entry->GetVkHandle() == sampler; });

  return iterator == mSamplers.end() ? RefCountedSampler() : RefCountedSampler(&**iterator);
}

RefCountedFence ResourceCache::FindFence( vk::Fence fence )
{
  auto iterator = std::find_if(mFences.begin(),
                               mFences.end(),
                               [&](const RefCountedFence entry) { return entry->GetVkHandle() == fence; });

  return iterator == mFences.end() ? RefCountedFence() : RefCountedFence(&**iterator);
}

RefCountedBuffer ResourceCache::FindBuffer( vk::Buffer buffer )
{
  auto iterator = std::find_if(mBuffers.begin(),
                               mBuffers.end(),
                               [&](const RefCountedBuffer entry) { return entry->GetVkHandle() == buffer; });

  return iterator == mBuffers.end() ? RefCountedBuffer() : RefCountedBuffer(&**iterator);
}

RefCountedImage ResourceCache::FindImage( vk::Image image )
{
  auto iterator = std::find_if(mImages.begin(),
                               mImages.end(),
                               [&](const RefCountedImage entry) { return entry->GetVkHandle() == image; });

  return iterator == mImages.end() ? RefCountedImage() : RefCountedImage(&**iterator);
}

RefCountedImageView ResourceCache::FindImageView( vk::ImageView imageView )
{
  auto iterator = std::find_if(mImageViews.begin(),
                               mImageViews.end(),
                               [&](const RefCountedImageView entry) { return entry->GetVkHandle() == imageView; });

  return iterator == mImageViews.end() ? RefCountedImageView() : RefCountedImageView(&**iterator);
}

ResourceCache& ResourceCache::RemoveBuffer( Buffer& buffer )
{
  if( !mBuffers.empty() )
  {
    auto found = std::find_if(mBuffers.begin(),
                              mBuffers.end(),
                              [&](const RefCountedBuffer entry) { return &(*entry) == &buffer; });

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
                              [&](const RefCountedImage entry) { return &(*entry) == &image; });

    std::iter_swap(found, std::prev(mImages.end()));
    mImages.back().Reset();
    mImages.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveImageView( ImageView& imageView )
{
  if( !mImageViews.empty() )
  {
    auto found = std::find_if(mImageViews.begin(),
                              mImageViews.end(),
                              [&](const RefCountedImageView entry) { return &(*entry) == &imageView; });

    std::iter_swap(found, std::prev(mImageViews.end()));
    mImageViews.back().Reset();
    mImageViews.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveShader( Shader& shader )
{
  if( !mShaders.empty() )
  {
    auto iterator = std::find_if(mShaders.begin(),
                                 mShaders.end(),
                                 [&](const RefCountedShader entry) { return &*entry == &shader; });

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
    using EntryPair = std::pair< std::thread::id, RefCountedCommandPool >;
    auto iterator = std::find_if(mCommandPools.begin(),
                                 mCommandPools.end(),
                                 [&](const EntryPair& entry) { return &*(entry.second) == &commandPool; });

    mCommandPools.erase( iterator );
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveDescriptorPool( DescriptorPool& descriptorPool )
{
  if( !mDescriptorPools.empty() )
  {
    auto iterator = std::find_if(mDescriptorPools.begin(),
                                 mDescriptorPools.end(),
                                 [&](const RefCountedDescriptorPool entry) { return &*entry == &descriptorPool; });

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
                                 [&](const RefCountedFramebuffer entry) { return &*entry == &framebuffer; });

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
                                 [&](const RefCountedSampler entry) { return &*entry == &sampler; });

    std::iter_swap(iterator, std::prev(mSamplers.end()));
    mSamplers.back().Reset();
    mSamplers.pop_back();
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveFence( Fence& fence )
{
  if( !mFences.empty() )
  {
    auto iterator = std::find_if(mFences.begin(),
                                 mFences.end(),
                                 [&](const RefCountedFence entry) { return &*entry == &fence; });

    std::iter_swap(iterator, std::prev(mFences.end()));
    mFences.back().Reset();
    mFences.pop_back();
  }
  return *this;
}

void ResourceCache::CollectGarbage()
{
  for( const auto& deleter : mDiscardQueue )
  {
    deleter();
  }

  mDiscardQueue.clear();
}

void ResourceCache::EnqueueDiscardOperation( std::function<void()> deleter )
{
  mDiscardQueue.push_back(std::move(deleter));
}

} //namespace Vulkan
} //namespace Graphics
} //namespace Dali
