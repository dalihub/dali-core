#ifndef DALI_GRAPHICS_VULKAN_RESOURCE_CACHE
#define DALI_GRAPHICS_VULKAN_RESOURCE_CACHE

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#ifndef VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_EXCEPTIONS
#endif

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 * Stores and manages Vulkan resources
 */
class ResourceCache final
{
public:
  /**
   * Adds the provided buffer object to the buffer cache
   * @param buffer The buffer object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddBuffer( BufferRef buffer );

  /**
   * Adds the provided image object to the image cache
   * @param image The image object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddImage( ImageRef image );

  /**
   * Adds the provided pipeline object to the pipeline cache
   * @param pipeline The pipeline object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddPipeline( PipelineRef pipeline );

  /**
   * Adds the provided shader object to the pipeline cache
   * @param shader The shader object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddShader( ShaderRef shader );

  /**
   * Adds the provided command pool object to the command pool cache
   * @param pool The command pool object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddCommandPool( CommandPoolRef pool );

  /**
   * Adds the provided descriptor pool object to the descriptor pool cache
   * @param pool The descriptor pool object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddDescriptorPool( DescriptorPoolRef pool );

  /**
   * Adds the provided framebuffer object to the framebuffer cache
   * @param framebuffer The framebuffer object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddFramebuffer( FramebufferRef framebuffer );

  /**
   * Adds the provided sampler object to the sampler cache
   * @param sampler The sampler object to be added to the cache
   * @return A reference to the ResourceCache
   */
  ResourceCache& AddSampler( SamplerRef sampler );


  /**
   * Finds the buffer object using the specified Vulkan handle in the cache
   * @param buffer The Vulkan handle of the buffer object to be found
   * @return A Handle to the buffer object if found. An empty Handle otherwise
   */
  BufferRef FindBuffer( vk::Buffer buffer );

  /**
   * Finds the image object using the specified Vulkan handle in the cache
   * @param image The Vulkan handle of the image object to be found
   * @return A handle to the Image object if found. An empty Handle otherwise
   */
  ImageRef FindImage( vk::Image image);

  /**
   * Finds the Pipeline object using the specified Vulkan handle in the cache
   * @param pipeline The Vulkan handle of the Pipeline object to be found
   * @return A Handle to the Pipeline object if found. An empty Handle otherwise
   */
  PipelineRef FindPipeline( vk::Pipeline pipeline );

  /**
   * Finds the shader module using the specified Vulkan handle in the cache
   * @param shaderModule The Vulkan handle of the shader module to be found
   * @return A Handle to the Shader module if found. An empty Handle otherwise
   */
  ShaderRef FindShader( vk::ShaderModule shaderModule );

  /**
   * Finds the CommandPool object using the specified Vulkan handle in the cache
   * @param commandPool The Vulkan handle of the CommandPool object to be found
   * @return A Handle to the CommandPool object if found. An empty Handle otherwise
   */
  CommandPoolRef FindCommandPool( vk::CommandPool commandPool );

  /**
   * Finds the DescriptorPool object using the specified Vulkan handle in the cache
   * @param descriptorPool The Vulkan handle of the DescriptorPool object to be found
   * @return A Handle to the DescriptorPool object if found. An empty Handle otherwise
   */
  DescriptorPoolRef FindDescriptorPool( vk::DescriptorPool descriptorPool );

  /**
   * Finds the Framebuffer object using the specified Vulkan handle in the cache
   * @param framebuffer The Vulkan handle of the Framebuffer object to be found
   * @return A Handle to the Framebuffer object if found. An empty Handle otherwise
   */
  FramebufferRef FindFramebuffer( vk::Framebuffer framebuffer );

  /**
   * Finds the Sampler object using the specified Vulkan handle in the cache.
   * @param sampler The Vulkan handle of the Sampler object to be found
   * @return A Handle to the Sampler object if found. An empty Handle otherwise
   */
  SamplerRef FindSampler( vk::Sampler sampler );

  /**
   * Removes the specified Buffer from the cache
   * @param buffer The Buffer to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemoveBuffer( Buffer& buffer );

  /**
   * Removes the specified Image from the cache
   * @param image The Image to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemoveImage( Image& image );

  /**
   * Removes the specified Pipeline from the cache
   * @param pipeline The Pipeline to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemovePipeline( Pipeline& pipeline );

  /**
   * Removes the specified Shader from the cache
   * @param shader The Shader to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemoveShader( Shader& shader );

  /**
   * Removes the specified CommandPool from the cache
   * @param commandPool The CommandPool to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemoveCommandPool( CommandPool& commandPool );

  /**
   * Removes the specified DescriptorPool from the cache
   * @param descriptorPool The DescriptorPool to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemoveDescriptorPool( DescriptorPool& descriptorPool );

  /**
   * Removes the specified Framebuffer from the cache
   * @param framebuffer The DescriptorPool to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemoveFramebuffer( Framebuffer& framebuffer );

  /**
   * Removes the specified Sampler from the cache
   * @param sampler The Sampler to be removed
   * @return A reference to the ResourceCache
   */
  ResourceCache& RemoveSampler( Sampler& sampler );

  ResourceCache() = default;

  // The cache should not be copyable
  ResourceCache( const ResourceCache& other ) = delete;

  ResourceCache& operator=( const ResourceCache& other ) = delete;

  // The cache should not be movable
  ResourceCache( ResourceCache&& other ) = delete;

  ResourceCache&& operator=( ResourceCache&& other ) = delete;

private:
  std::vector<BufferRef> mBuffers;
  std::vector<ImageRef> mImages;
  std::vector<PipelineRef> mPipelines;
  std::vector<ShaderRef> mShaders;
  std::vector<CommandPoolRef> mCommandPools;
  std::vector<DescriptorPoolRef> mDescriptorPools;
  std::vector<FramebufferRef> mFramebuffers;
  std::vector<SamplerRef> mSamplers;
};

} //namespace Vulkan
} //namespace Graphics
} //namespace Dali

#endif //DALI_GRAPHICS_VULKAN_RESOURCE_CACHE
