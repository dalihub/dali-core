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

#include <dali/graphics/vulkan/internal/vulkan-resource-register.h>

#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-pipeline.h>
#include <dali/graphics/vulkan/internal/vulkan-shader.h>
#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>

#include <algorithm>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

ResourceRegister& ResourceRegister::AddBuffer( Buffer& buffer )
{
  mBuffers.push_back( &buffer );
  return *this;
}

ResourceRegister& ResourceRegister::AddImage( Image& image )
{
  mImages.push_back( &image );
  return *this;
}

ResourceRegister& ResourceRegister::AddImageView( ImageView& imageView )
{
  mImageViews.push_back( &imageView );
  return *this;
}

ResourceRegister& ResourceRegister::AddShader( Shader& shader )
{
  mShaders.push_back( &shader );
  return *this;
}

ResourceRegister& ResourceRegister::AddDescriptorPool( DescriptorPool& pool )
{
  mDescriptorPools.push_back( &pool );
  return *this;
}

ResourceRegister& ResourceRegister::AddFramebuffer( Framebuffer& framebuffer )
{
  mFramebuffers.push_back( &framebuffer );
  return *this;
}

ResourceRegister& ResourceRegister::AddSampler( Sampler& sampler )
{
  mSamplers.push_back( &sampler );
  return *this;
}

RefCountedShader ResourceRegister::FindShader( vk::ShaderModule shaderModule )
{
  auto iterator = std::find_if( mShaders.begin(),
                                mShaders.end(),
                                [ & ]( const Shader* entry ) {
                                  return entry->GetVkHandle() == shaderModule;
                                } );

  return iterator == mShaders.end() ? RefCountedShader() : RefCountedShader( *iterator );
}

RefCountedDescriptorPool ResourceRegister::FindDescriptorPool( vk::DescriptorPool descriptorPool )
{
  auto iterator = std::find_if( mDescriptorPools.begin(),
                                mDescriptorPools.end(),
                                [ & ]( const DescriptorPool* entry ) {
                                  return entry->GetVkHandle() == descriptorPool;
                                } );

  return iterator == mDescriptorPools.end() ? RefCountedDescriptorPool() : RefCountedDescriptorPool( *iterator );
}

RefCountedFramebuffer ResourceRegister::FindFramebuffer( vk::Framebuffer framebuffer )
{
  auto iterator = std::find_if( mFramebuffers.begin(),
                                mFramebuffers.end(),
                                [ & ]( const Framebuffer* entry ) {
                                  return entry->GetVkHandle() == framebuffer;
                                } );

  return iterator == mFramebuffers.end() ? RefCountedFramebuffer() : RefCountedFramebuffer( *iterator );
}

RefCountedSampler ResourceRegister::FindSampler( vk::Sampler sampler )
{
  auto iterator = std::find_if( mSamplers.begin(),
                                mSamplers.end(),
                                [ & ]( const Sampler* entry ) { return entry->GetVkHandle() == sampler; } );

  return iterator == mSamplers.end() ? RefCountedSampler() : RefCountedSampler( &**iterator );
}

RefCountedBuffer ResourceRegister::FindBuffer( vk::Buffer buffer )
{
  auto iterator = std::find_if( mBuffers.begin(),
                                mBuffers.end(),
                                [ & ]( const Buffer* entry ) { return entry->GetVkHandle() == buffer; } );

  return iterator == mBuffers.end() ? RefCountedBuffer() : RefCountedBuffer( *iterator );
}

RefCountedImage ResourceRegister::FindImage( vk::Image image )
{
  auto iterator = std::find_if( mImages.begin(),
                                mImages.end(),
                                [ & ]( const Image* entry ) { return entry->GetVkHandle() == image; } );

  return iterator == mImages.end() ? RefCountedImage() : RefCountedImage( *iterator );
}

RefCountedImageView ResourceRegister::FindImageView( vk::ImageView imageView )
{
  auto iterator = std::find_if( mImageViews.begin(),
                                mImageViews.end(),
                                [ & ]( const ImageView* entry ) {
                                  return entry->GetVkHandle() == imageView;
                                } );

  return iterator == mImageViews.end() ? RefCountedImageView() : RefCountedImageView( *iterator );
}

ResourceRegister& ResourceRegister::RemoveBuffer( Buffer& buffer )
{
  if( !mBuffers.empty() )
  {
    auto found = std::find_if( mBuffers.begin(),
                               mBuffers.end(),
                               [ & ]( const Buffer* entry ) {
                                 return entry->GetVkHandle() == buffer.GetVkHandle();
                               } );

    std::iter_swap( found, std::prev( mBuffers.end() ) );
    mBuffers.pop_back();
  }
  return *this;
}

ResourceRegister& ResourceRegister::RemoveImage( Image& image )
{
  if( !mImages.empty() )
  {
    auto found = std::find_if( mImages.begin(),
                               mImages.end(),
                               [ & ]( const Image* entry ) {
                                 return entry->GetVkHandle() == image.GetVkHandle();
                               } );

    std::iter_swap( found, std::prev( mImages.end() ) );
    mImages.pop_back();
  }
  return *this;
}

ResourceRegister& ResourceRegister::RemoveImageView( ImageView& imageView )
{
  if( !mImageViews.empty() )
  {
    auto found = std::find_if( mImageViews.begin(),
                               mImageViews.end(),
                               [ & ]( const ImageView* entry ) {
                                 return entry->GetVkHandle() == imageView.GetVkHandle();
                               } );

    std::iter_swap( found, std::prev( mImageViews.end() ) );
    mImageViews.pop_back();
  }
  return *this;
}

ResourceRegister& ResourceRegister::RemoveShader( Shader& shader )
{
  if( !mShaders.empty() )
  {
    auto iterator = std::find_if( mShaders.begin(),
                                  mShaders.end(),
                                  [ & ]( const Shader* entry ) {
                                    return &*entry == &shader;
                                  } );

    std::iter_swap( iterator, std::prev( mShaders.end() ) );
    mShaders.pop_back();
  }
  return *this;
}

ResourceRegister& ResourceRegister::RemoveDescriptorPool( DescriptorPool& descriptorPool )
{
  if( !mDescriptorPools.empty() )
  {
    auto iterator = std::find_if( mDescriptorPools.begin(),
                                  mDescriptorPools.end(),
                                  [ & ]( const DescriptorPool* entry ) {
                                    return entry->GetVkHandle() == descriptorPool.GetVkHandle();
                                  } );

    std::iter_swap( iterator, std::prev( mDescriptorPools.end() ) );
    mDescriptorPools.pop_back();
  }
  return *this;
}

ResourceRegister& ResourceRegister::RemoveFramebuffer( Framebuffer& framebuffer )
{
  if( !mFramebuffers.empty() )
  {
    auto iterator = std::find_if( mFramebuffers.begin(),
                                  mFramebuffers.end(),
                                  [ & ]( const Framebuffer* entry ) {
                                    return entry->GetVkHandle() == framebuffer.GetVkHandle();
                                  } );

    std::iter_swap( iterator, std::prev( mFramebuffers.end() ) );
    mFramebuffers.pop_back();
  }
  return *this;
}

ResourceRegister& ResourceRegister::RemoveSampler( Sampler& sampler )
{
  if( !mSamplers.empty() )
  {
    auto iterator = std::find_if( mSamplers.begin(),
                                  mSamplers.end(),
                                  [ & ]( const Sampler* entry ) {
                                    return entry->GetVkHandle() == sampler.GetVkHandle();
                                  } );

    std::iter_swap( iterator, std::prev( mSamplers.end() ) );
    mSamplers.pop_back();
  }
  return *this;
}

// Called only by the Graphics class destructor
void ResourceRegister::Clear()
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
}

void ResourceRegister::PrintReferenceCountReport()
{
#if defined(DEBUG_ENABLED)
  auto totalObjectCount = mBuffers.size() +
                          mImages.size() +
                          mImageViews.size() +
                          mShaders.size() +
                          mDescriptorPools.size() +
                          mFramebuffers.size() +
                          mSamplers.size();

  uint32_t totalRefCount = 0;
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "TOTAL OBJECT COUNT: %ld\n", totalObjectCount )
  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "BUFFER REFERENCES:" )
  for( auto* buffer : mBuffers )
  {
    auto refCount = buffer->GetRefCount();
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tbuffer->%p : %d\n", static_cast< void* >(buffer->GetVkHandle()),
                   refCount )
    totalRefCount += refCount;
  }
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal reference count: %d\n", totalRefCount )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal object count: %ld\n\n", mBuffers.size() )
  totalRefCount = 0;

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "IMAGE REFERENCES:" )
  for( auto* image : mImages )
  {
    auto refCount = image->GetRefCount();
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "\timage->%p : %d\n", static_cast< void* >(image->GetVkHandle()),
                   refCount )
    totalRefCount += refCount;
  }
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal reference count: %d\n", totalRefCount )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal object count: %ld\n\n", mImages.size() )
  totalRefCount = 0;

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "IMAGE VIEW REFERENCES:" )
  for( auto& imageView : mImageViews )
  {
    auto refCount = imageView->GetRefCount();
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "\timage view->%p : %d\n",
                   static_cast< void* >(imageView->GetVkHandle()), refCount )
    totalRefCount += refCount;
  }
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal reference count: %d\n", totalRefCount )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal object count: %ld\n\n", mImageViews.size() )
  totalRefCount = 0;

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "SHADER MODULE REFERENCES:" )
  for( auto& shader : mShaders )
  {
    auto refCount = shader->GetRefCount();
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tshader module->%p : %d\n",
                   static_cast< void* >(shader->GetVkHandle()), refCount )
    totalRefCount += refCount;
  }
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal reference count: %d\n", totalRefCount )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal object count: %ld\n\n", mShaders.size() )
  totalRefCount = 0;

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "DESCRIPTOR POOL REFERENCES:" )
  for( auto& descPool : mDescriptorPools )
  {
    auto refCount = descPool->GetRefCount();
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tdescriptor pool->%p : %d\n",
                   static_cast< void* >(descPool->GetVkHandle()), refCount )
    totalRefCount += refCount;
  }
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal reference count: %d\n", totalRefCount )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal object count: %ld\n\n", mDescriptorPools.size() )
  totalRefCount = 0;

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "FRAMEBUFFER REFERENCES:" )
  for( auto& framebuffer : mFramebuffers )
  {
    auto refCount = framebuffer->GetRefCount();
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tframebuffer->%p : %d\n",
                   static_cast< void* >(framebuffer->GetVkHandle()), refCount )
    totalRefCount += refCount;
  }
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal reference count: %d\n", totalRefCount )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal object count: %ld\n\n", mFramebuffers.size() )
  totalRefCount = 0;

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "SAMPLER REFERENCES:" )
  for( auto& sampler : mSamplers )
  {
    auto refCount = sampler->GetRefCount();
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tsampler->%p : %d\n", static_cast< void* >(sampler->GetVkHandle()),
                   refCount )
    totalRefCount += refCount;
  }
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal reference count: %d\n", totalRefCount )
  DALI_LOG_INFO( gVulkanFilter, Debug::General, "\tTotal object count: %ld\n\n", mSamplers.size() )
#endif
}

} //namespace Vulkan
} //namespace Graphics
} //namespace Dali
