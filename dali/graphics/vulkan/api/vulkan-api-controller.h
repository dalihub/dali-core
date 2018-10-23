#ifndef DALI_GRAPHICS_VULKAN_API_CONTROLLER_H
#define DALI_GRAPHICS_VULKAN_API_CONTROLLER_H

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
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-types.h>

namespace Dali
{
namespace Graphics
{

namespace Vulkan
{
class Graphics;
class Buffer;
class Image;
}

namespace VulkanAPI
{
class Controller;
class UboManager;
class DescriptorSetList;

/**
 * Structure describes deferred memory transfer
 * Source memory is owned by the buffer and will be discarded
 * when transfer is completed
 */
struct BufferMemoryTransfer
{
  BufferMemoryTransfer() = default;

  ~BufferMemoryTransfer()
  {
    delete [] srcPtr;
  }

  char* srcPtr{ nullptr };
  uint32_t srcSize{ 0u };

  Vulkan::RefCountedBuffer dstBuffer;
  uint32_t dstOffset{ 0u };
};

enum class TransferRequestType
{
  BUFFER_TO_IMAGE,
  IMAGE_TO_IMAGE,
  BUFFER_TO_BUFFER,
  IMAGE_TO_BUFFER,
  UNDEFINED
};

/**
 * Structure describing blitting request Buffer to Image
 */
struct ResourceTransferRequest
{
  ResourceTransferRequest( TransferRequestType type )
  : requestType( type )
  {}

  TransferRequestType requestType;

  struct
  {
    Vulkan::RefCountedBuffer                    srcBuffer   { nullptr };  /// Source buffer
    Vulkan::RefCountedImage                     dstImage    { nullptr };  /// Destination image
    vk::BufferImageCopy                         copyInfo    { };          /// Vulkan specific copy info
  } bufferToImageInfo;

  struct
  {
    Vulkan::RefCountedImage                    srcImage    { nullptr };  /// Source image
    Vulkan::RefCountedImage                    dstImage    { nullptr };  /// Destination image
    vk::ImageCopy                              copyInfo    { };          /// Vulkan specific copy info
  } imageToImageInfo;

  bool                                         deferredTransferMode{ true }; // Vulkan implementation prefers deferred mode

  // delete copy
  ResourceTransferRequest( const ResourceTransferRequest& ) = delete;
  ResourceTransferRequest& operator=( const ResourceTransferRequest& ) = delete;
  ResourceTransferRequest& operator=( ResourceTransferRequest&& obj ) = delete;

  ResourceTransferRequest( ResourceTransferRequest&& obj )
  {
    requestType = obj.requestType;
    deferredTransferMode = obj.deferredTransferMode;

    if( requestType == TransferRequestType::BUFFER_TO_IMAGE )
    {
      bufferToImageInfo.srcBuffer = std::move( obj.bufferToImageInfo.srcBuffer );
      bufferToImageInfo.dstImage = std::move( obj.bufferToImageInfo.dstImage );
      bufferToImageInfo.copyInfo = std::move( obj.bufferToImageInfo.copyInfo );
    }
    else if( requestType == TransferRequestType::IMAGE_TO_IMAGE )
    {
      imageToImageInfo.srcImage = std::move( obj.imageToImageInfo.srcImage );
      imageToImageInfo.dstImage = std::move( obj.imageToImageInfo.dstImage );
      imageToImageInfo.copyInfo = std::move( obj.imageToImageInfo.copyInfo );
    }
  }

};

/**
 * @brief Interface class for Manager types in the graphics API.
 */
class Controller : public Dali::Graphics::API::Controller
{
public:

  explicit Controller( Vulkan::Graphics& vulkanGraphics );

  void Initialise();

  /**
   * Must be created from VulkanGraphics
   * @param vulkanGraphics
   * @return
   */
  static std::unique_ptr< Controller > New( Vulkan::Graphics& vulkanGraphics );

  /**
   * @brief Create a new object
   */
  std::unique_ptr< API::Shader > CreateShader( const API::BaseFactory< API::Shader >& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr< API::Texture > CreateTexture( const API::BaseFactory< API::Texture >& factory ) override;

  /**
 * @brief Create a new object
 */
  std::unique_ptr< API::Buffer > CreateBuffer( const API::BaseFactory< API::Buffer >& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr< API::Sampler > CreateSampler( const API::BaseFactory< API::Sampler >& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr< API::Pipeline > CreatePipeline( const API::PipelineFactory& factory ) override;

  /**
   * @brief Create a new object
   */
  std::unique_ptr< API::Framebuffer > CreateFramebuffer( const API::BaseFactory< API::Framebuffer >& factory ) override;

  void SubmitCommands( std::vector< API::RenderCommand* > commands ) override;

  std::unique_ptr< API::RenderCommand > AllocateRenderCommand() override;

  virtual void BeginFrame() override;

  void EndFrame() override;

  void Pause() override;

  void Resume() override;

  bool EnableDepthStencilBuffer( bool enableDepth, bool enableStencil ) override;

  // VULKAN only

public:

  Vulkan::Graphics& GetGraphics() const;

  void ScheduleBufferMemoryTransfer( std::unique_ptr< VulkanAPI::BufferMemoryTransfer > transferRequest );

  void ScheduleResourceTransfer( VulkanAPI::ResourceTransferRequest&& transferRequest );

  void PushDescriptorWrite( const vk::WriteDescriptorSet& write );

  /**
   * Pushes descriptorsets to be freed by the allocator.
   * The descriptor sets must not be used any more by the renderer
   * @param descriptorSets
   */
  void FreeDescriptorSets( VulkanAPI::DescriptorSetList&& descriptorSetList );

  bool HasPendingResourceTransfers() const;

public:

  API::TextureFactory& GetTextureFactory() const override;

  API::ShaderFactory& GetShaderFactory() const override;

  API::BufferFactory& GetBufferFactory() const override;

  API::PipelineFactory& GetPipelineFactory() override;

  API::FramebufferFactory& GetFramebufferFactory() const override;

  API::SamplerFactory& GetSamplerFactory() override;

public:
  // not copyable
  Controller( const Controller& ) = delete;

  Controller& operator=( const Controller& ) = delete;

  ~Controller() override;

protected:
  // derived types should not be moved direcly to prevent slicing
  Controller( Controller&& ) noexcept = default;

  Controller& operator=( Controller&& ) noexcept;

  /**
   * Objects of this type should not directly.
   */
  Controller();

private:
  struct Impl;
  std::unique_ptr< Impl > mImpl;

public:
  struct Stats
  {
    uint32_t frame { 0u };
    uint32_t uniformBufferBindings { 0u };
    uint32_t samplerTextureBindings { 0u };
  };

  void PrintStats();

  Stats mStats;
};

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_API_CONTROLLER_H
