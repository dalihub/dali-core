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
                              [&](const RefCountedBuffer& entry)
                              {
                                return entry->GetVkHandle() == buffer.GetVkHandle();
                              });

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
                              [&](const RefCountedImage& entry)
                              {
                                return entry->GetVkHandle() == image.GetVkHandle();
                              });

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
                              [&](const RefCountedImageView& entry)
                              {
                                return entry->GetVkHandle() == imageView.GetVkHandle();
                              });

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
                                 [&](const RefCountedShader& entry)
                                 {
                                   return &*entry == &shader;
                                 });

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

    auto found = mCommandPools.end();

    auto it = mCommandPools.begin();
    while ( it != mCommandPools.end() )
    {
      auto& refcounted = (*it).second;
      if ( refcounted->GetVkHandle() == commandPool.GetVkHandle() )
      {
        found = it;
        break;
      }

      ++it;
    }

    mCommandPools.erase( found );
  }
  return *this;
}

ResourceCache& ResourceCache::RemoveDescriptorPool( DescriptorPool& descriptorPool )
{
  if( !mDescriptorPools.empty() )
  {
    auto iterator = std::find_if(mDescriptorPools.begin(),
                                 mDescriptorPools.end(),
                                 [&](const RefCountedDescriptorPool& entry)
                                 {
                                   return entry->GetVkHandle() == descriptorPool.GetVkHandle();
                                 });

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
                                 [&](const RefCountedFramebuffer& entry)
                                 {
                                   return entry->GetVkHandle() == framebuffer.GetVkHandle();
                                 });

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
                                 [&](const RefCountedSampler& entry)
                                 {
                                   return entry->GetVkHandle() == sampler.GetVkHandle();
                                 });

    std::iter_swap(iterator, std::prev(mSamplers.end()));
    mSamplers.back().Reset();
    mSamplers.pop_back();
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
#ifndef NDEBUG
  printf("DISCARD QUEUE SIZE: %ld\n", mDiscardQueue.size());
#endif
}

// Called only by the Graphics class destructor
void ResourceCache::Clear()
{
  //This call assumes that all possible render threads have been joined by this point.
  //This function is called by the Graphics class destructor. At this point the caches
  //should contain the last reference of all created objects.
  //Clearing the cache here will enqueue all resources to the discard queue ready to be garbage collected.
  mBuffers.clear();
  mImages.clear();
  mImageViews.clear();
  mDescriptorPools.clear();
  mShaders.clear();
  mSamplers.clear();
  mFramebuffers.clear();
  mCommandPools.clear();
}

void ResourceCache::PrintReferenceCountReport( size_t* outObjectCount )
{
  auto totalObjectCount = mBuffers.size() +
                          mImages.size() +
                          mImageViews.size() +
                          mShaders.size() +
                          mDescriptorPools.size() +
                          mFramebuffers.size() +
                          mSamplers.size();

  if( outObjectCount )
  {
    *outObjectCount = totalObjectCount;
  }

  uint32_t totalRefCount = 0;
  printf("TOTAL OBJECT COUNT: %ld\n", totalObjectCount);
  printf("BUFFER REFERENCES:\n");
  for (auto& buffer : mBuffers)
  {
    auto refCount = buffer->GetRefCount();
    printf("\tbuffer->%p : %d\n", static_cast< void* >(buffer->GetVkHandle()), refCount);
    totalRefCount += refCount;
  }
  printf("\tTotal reference count: %d\n", totalRefCount);
  printf("\tTotal object count: %ld\n\n", mBuffers.size());
  totalRefCount = 0;

  printf("IMAGE REFERENCES:\n");
  for( auto& image : mImages )
  {
    auto refCount = image->GetRefCount();
    printf("\timage->%p : %d\n", static_cast< void* >(image->GetVkHandle()), refCount);
    totalRefCount += refCount;
  }
  printf("\tTotal reference count: %d\n", totalRefCount);
  printf("\tTotal object count: %ld\n\n", mImages.size());
  totalRefCount = 0;

  printf("IMAGE VIEW REFERENCES:\n");
  for( auto& imageView : mImageViews )
  {
    auto refCount = imageView->GetRefCount();
    printf("\timage view->%p : %d\n", static_cast< void* >(imageView->GetVkHandle()), refCount);
    totalRefCount += refCount;
  }
  printf("\tTotal reference count: %d\n", totalRefCount);
  printf("\tTotal object count: %ld\n\n", mImageViews.size());
  totalRefCount = 0;

  printf("SHADER MODULE REFERENCES:\n");
  for( auto& shader : mShaders )
  {
    auto refCount = shader->GetRefCount();
    printf("\tshader module->%p : %d\n", static_cast< void* >(shader->GetVkHandle()), refCount);
    totalRefCount += refCount;
  }
  printf("\tTotal reference count: %d\n", totalRefCount);
  printf("\tTotal object count: %ld\n\n", mShaders.size());
  totalRefCount = 0;

  printf("DESCRIPTOR POOL REFERENCES:\n");
  for( auto& descPool : mDescriptorPools )
  {
    auto refCount = descPool->GetRefCount();
    printf("\tdescriptor pool->%p : %d\n", static_cast< void* >(descPool->GetVkHandle()), refCount);
    totalRefCount += refCount;
  }
  printf("\tTotal reference count: %d\n", totalRefCount);
  printf("\tTotal object count: %ld\n\n", mDescriptorPools.size());
  totalRefCount = 0;

  printf("FRAMEBUFFER REFERENCES:\n");
  for( auto& framebuffer : mFramebuffers )
  {
    auto refCount = framebuffer->GetRefCount();
    printf("\tframebuffer->%p : %d\n", static_cast< void* >(framebuffer->GetVkHandle()), refCount);
    totalRefCount += refCount;
  }
  printf("\tTotal reference count: %d\n", totalRefCount);
  printf("\tTotal object count: %ld\n\n", mFramebuffers.size());
  totalRefCount = 0;

  printf("SAMPLER REFERENCES:\n");
  for( auto& sampler : mSamplers )
  {
    auto refCount = sampler->GetRefCount();
    printf("\tsampler->%p : %d\n", static_cast< void* >(sampler->GetVkHandle()), refCount);
    totalRefCount += refCount;
  }
  printf("\tTotal reference count: %d\n", totalRefCount);
  printf("\tTotal object count: %ld\n\n", mSamplers.size());
}

} //namespace Vulkan
} //namespace Graphics
} //namespace Dali
