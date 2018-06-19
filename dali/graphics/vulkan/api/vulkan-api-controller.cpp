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

#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-sampler.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-graphics-texture.h>

// API
#include <dali/graphics/vulkan/api/vulkan-api-shader.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-shader-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-render-command.h>
#include <dali/graphics/vulkan/api/internal/vulkan-pipeline-cache.h>

#include <dali/graphics/vulkan/api/internal/vulkan-ubo-manager.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

struct Controller::Impl
{
  struct RenderPassChange
  {
    // only move semantics
    RenderPassChange( vk::RenderPassBeginInfo _beginInfo,
                      std::vector< vk::ClearValue >&& _clearColorValues,
                      const Vulkan::RefCountedFramebuffer& _framebuffer,
                      uint32_t _offset )
            : beginInfo( std::move( _beginInfo ) ),
              colorValues( std::move( _clearColorValues ) ),
              framebuffer( _framebuffer ),
              offset( _offset )
    {
    }

    // no default constructor!
    RenderPassChange() = delete;

    vk::RenderPassBeginInfo beginInfo{};
    std::vector< vk::ClearValue > colorValues{};
    Vulkan::RefCountedFramebuffer framebuffer{};
    uint32_t offset{ 0 };
  };

  Impl( Controller& owner, Dali::Graphics::Vulkan::Graphics& graphics )
          : mGraphics( graphics ),
            mOwner( owner ),
            mDefaultAllocator( mGraphics.GetDeviceMemoryManager().GetDefaultAllocator() )
  {
  }

  ~Impl()
  {
  }

  // TODO: @todo this function initialises basic buffers, shaders and pipeline
  // for the prototype ONLY
  bool Initialise()
  {
    // Create factories
    mShaderFactory = std::make_unique< VulkanAPI::ShaderFactory >( mGraphics );
    mTextureFactory = std::make_unique< VulkanAPI::TextureFactory >( mGraphics );
    mBufferFactory = std::make_unique< VulkanAPI::BufferFactory >( mOwner );
    mPipelineFactory = std::make_unique< VulkanAPI::PipelineFactory >( mOwner );

    mUboManager = std::make_unique< VulkanAPI::UboManager >( mOwner );

    mDefaultPipelineCache = std::make_unique< VulkanAPI::PipelineCache >( mGraphics, mOwner );

    return true;
  }


  void BeginFrame()
  {
    // for all swapchains acquire new framebuffer
    auto surface = mGraphics.GetSurface( 0u );

    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );

    swapchain->AcquireNextFramebuffer();

    mCurrentFramebuffer.Reset();
  }

  void EndFrame()
  {
    // swap all swapchains
    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );

    auto primaryCommandBuffer = swapchain->GetCurrentCommandBuffer();

    for( auto i = 0u; i < mRenderPasses.size(); ++i )
    {
      if( i != 0u )
      {
        primaryCommandBuffer->EndRenderPass();
      }

      const auto& rp = mRenderPasses[i];
      uint32_t offset = rp.offset;
      auto count = uint32_t(
              ( i == mRenderPasses.size() - 1 ) ?
              mSecondaryCommandBufferRefs.size() - rp.offset :
              mRenderPasses[i + 1].offset - rp.offset );

      primaryCommandBuffer->BeginRenderPass( rp.beginInfo, vk::SubpassContents::eSecondaryCommandBuffers );
      primaryCommandBuffer->ExecuteCommands( mSecondaryCommandBufferRefs, offset, count );
    }

    primaryCommandBuffer->EndRenderPass();

    swapchain->Present();
    mSecondaryCommandBufferRefs.clear();
    mRenderPasses.clear();
  }

  API::TextureFactory& GetTextureFactory() const
  {
    return *( mTextureFactory.get() );
  }

  API::ShaderFactory& GetShaderFactory() const
  {
    return *( mShaderFactory.get() );
  }

  API::BufferFactory& GetBufferFactory() const
  {
    return *( mBufferFactory.get() );
  }

  /**
   * NEW IMPLEMENTATION
   */

  std::unique_ptr< API::RenderCommand > AllocateRenderCommand()
  {
    return std::make_unique< VulkanAPI::RenderCommand >( mOwner, mGraphics );
  }

  bool UpdateRenderPass( const API::RenderCommand::RenderTargetBinding& renderTargetBinding )
  {
    Vulkan::RefCountedFramebuffer framebuffer{ nullptr };
    if( renderTargetBinding.framebuffer.Exists() )
    {
      // @todo use VulkanAPI::Framebuffer when available
      //framebuffer = static_cast<VulkanAPI::Framebuffer&>(renderTargetBinding.framebuffer.Get()).GetVkHandle();
    }
    else
    {
      // use first surface/swapchain as render target
      auto surface = mGraphics.GetSurface( 0u );
      auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
      framebuffer = swapchain->GetCurrentFramebuffer();
    }

    // If there is no framebuffer the it means DALi tries to use
    // default framebuffer. For now just substitute with surface/swapchain
    if( framebuffer != mCurrentFramebuffer )
    {
      auto primaryCommandBuffer = mGraphics.GetSwapchainForFBID( 0 )->GetCurrentCommandBuffer();

      mCurrentFramebuffer = framebuffer;
      mCurrentFramebuffer->GetRenderPassVkHandle();
      auto newColors = mCurrentFramebuffer->GetDefaultClearValues();
      newColors[0].color.setFloat32( { renderTargetBinding.clearColors[0].r,
                                       renderTargetBinding.clearColors[0].g,
                                       renderTargetBinding.clearColors[0].b,
                                       renderTargetBinding.clearColors[0].a
                                     } );
      mRenderPasses.emplace_back(
              // render pass
              vk::RenderPassBeginInfo{}
                      .setRenderPass( mCurrentFramebuffer->GetRenderPassVkHandle() )
                      .setFramebuffer( mCurrentFramebuffer->GetVkHandle() )
                      .setRenderArea( vk::Rect2D( { 0, 0 }, { mCurrentFramebuffer->GetWidth(),
                                                              mCurrentFramebuffer->GetHeight() } ) )
                      .setClearValueCount( uint32_t( mCurrentFramebuffer->GetDefaultClearValues().size() ) )
                      .setPClearValues( newColors.data() ),

              // colors
              std::move( newColors ),

              // framebuffer
              framebuffer,

              // offset when to begin new render pass
              0 );
      return true;
    }

    // same render pass
    return false;
  }

  /**
   * Submits number of commands in one go ( simiar to vkCmdExecuteCommands )
   * @param commands
   */
  void SubmitCommands( std::vector< Dali::Graphics::API::RenderCommand* > commands )
  {
    // if there are any scheduled writes
    if( !mBufferTransferRequests.empty() )
    {
      for( auto&& req : mBufferTransferRequests )
      {
        void* dst = req->dstBuffer->GetMemoryHandle()->Map();
        memcpy( dst, &*req->srcPtr, req->srcSize );
        req->dstBuffer->GetMemoryHandle()->Unmap();
      }
      mBufferTransferRequests.clear();
    }

    // the list of commands may be empty, but still we may have scheduled memory
    // transfers
    if( commands.empty() )
    {
      return;
    }

    std::vector< Vulkan::RefCountedCommandBuffer > cmdBufRefs{};

    // Update uniform buffers
    for( auto&& command : commands )
    {
      // prepare pipelines
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);
      apiCommand->UpdateUniformBuffers();
    }

    mUboManager->UnmapAllBuffers();

    // bind resources
    for( auto&& command : commands )
    {
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);
      apiCommand->PrepareResources();
    }

    // Begin render pass for render target
    // clear color obtained from very first command in the batch
    auto firstCommand = static_cast<VulkanAPI::RenderCommand*>(commands[0]);
    UpdateRenderPass( firstCommand->GetRenderTargetBinding() );

    // set up writes
    for( auto&& command : commands )
    {
      //const auto& vertexBufferBindings = command->GetVertexBufferBindings();
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);

      // skip if there's no valid pipeline
      if( !apiCommand->GetVulkanPipeline() )
      {
        continue;
      }

      // start new command buffer
      auto cmdbuf = mGraphics.CreateCommandBuffer( false );//mCommandPool->NewCommandBuffer( false );
      cmdbuf->Reset();
      cmdbuf->Begin( vk::CommandBufferUsageFlagBits::eRenderPassContinue );
      cmdbuf->BindGraphicsPipeline( apiCommand->GetVulkanPipeline() );

      // bind vertex buffers
      auto binding = 0u;
      for( auto&& vb : apiCommand->GetVertexBufferBindings() )
      {
        cmdbuf->BindVertexBuffer( binding++, static_cast<const VulkanAPI::Buffer&>( vb.Get()).GetBufferRef(), 0 );
      }

      // note: starting set = 0
      cmdbuf->BindDescriptorSets( apiCommand->GetDescriptorSets(), 0 );

      // draw
      const auto& drawCommand = apiCommand->GetDrawCommand();

      const auto& indexBinding = apiCommand->GetIndexBufferBinding();
      if( indexBinding.buffer.Exists() )
      {
        cmdbuf->BindIndexBuffer( static_cast<const VulkanAPI::Buffer&>(indexBinding.buffer.Get()).GetBufferRef(),
                                 0, vk::IndexType::eUint16 );
        cmdbuf->DrawIndexed( drawCommand.indicesCount,
                             drawCommand.instanceCount,
                             drawCommand.firstIndex,
                             0,
                             drawCommand.firstInstance );
      }
      else
      {
        cmdbuf->Draw( drawCommand.vertexCount,
                      drawCommand.instanceCount,
                      drawCommand.firstVertex,
                      drawCommand.firstInstance );
      }
      cmdbuf->End();
      mSecondaryCommandBufferRefs.emplace_back( cmdbuf );
    }

  }

  // resources
  std::vector< Vulkan::RefCountedTexture > mTextures;
  std::vector< Vulkan::RefCountedShader > mShaders;
  std::vector< Vulkan::RefCountedBuffer > mBuffers;

  // owner objects
  ObjectOwner< API::Texture > mTexturesOwner;
  ObjectOwner< API::Shader > mShadersOwner;
  ObjectOwner< API::Buffer > mBuffersOwner;

  std::unique_ptr< PipelineCache > mDefaultPipelineCache;

  Vulkan::Graphics& mGraphics;
  Controller& mOwner;
  Vulkan::GpuMemoryAllocator& mDefaultAllocator;

  std::unique_ptr< VulkanAPI::TextureFactory > mTextureFactory;
  std::unique_ptr< VulkanAPI::ShaderFactory > mShaderFactory;
  std::unique_ptr< VulkanAPI::BufferFactory > mBufferFactory;
  std::unique_ptr< VulkanAPI::PipelineFactory > mPipelineFactory;

  std::vector< std::unique_ptr< VulkanAPI::BufferMemoryTransfer>> mBufferTransferRequests;

  std::unique_ptr< VulkanAPI::UboManager > mUboManager;

  // Accumulate all the secondary command buffers of the frame here to avoid them being overwritten
  // This accumulator vector gets cleared at the end of the frame. The command buffers are returned to the pool
  // and ready to be used for the next frame.
  std::vector< Vulkan::RefCountedCommandBuffer > mSecondaryCommandBufferRefs;

  Vulkan::RefCountedFramebuffer mCurrentFramebuffer;
  std::vector< RenderPassChange > mRenderPasses;

};

// TODO: @todo temporarily ignore missing return type, will be fixed later

API::Accessor< API::Shader > Controller::CreateShader( const API::BaseFactory< API::Shader >& factory )
{
  auto handle = mImpl->mShadersOwner.CreateObject( factory );
  auto& apiShader = static_cast<VulkanAPI::Shader&>(mImpl->mShadersOwner[handle]);
  auto vertexShaderRef = apiShader.GetShaderRef( vk::ShaderStageFlagBits::eVertex );
  auto fragmentShaderRef = apiShader.GetShaderRef( vk::ShaderStageFlagBits::eFragment );
  mImpl->mShaders.push_back( vertexShaderRef );
  mImpl->mShaders.push_back( fragmentShaderRef );
  return API::Accessor< API::Shader >( mImpl->mShadersOwner, handle );
}

API::Accessor< API::Texture > Controller::CreateTexture( const API::BaseFactory< API::Texture >& factory )
{
  auto handle = mImpl->mTexturesOwner.CreateObject( factory );
  auto textureRef = static_cast<VulkanAPI::Texture&>(mImpl->mTexturesOwner[handle]).GetTextureRef();
  mImpl->mTextures.push_back( textureRef );
  return API::Accessor< API::Texture >( mImpl->mTexturesOwner, handle );
}

API::Accessor< API::Buffer > Controller::CreateBuffer( const API::BaseFactory< API::Buffer >& factory )
{
  auto handle = mImpl->mBuffersOwner.CreateObject( factory );
  auto bufferRef = static_cast<VulkanAPI::Buffer&>(mImpl->mBuffersOwner[handle]).GetBufferRef();
  mImpl->mBuffers.push_back( bufferRef );
  return API::Accessor< API::Buffer >( mImpl->mBuffersOwner, handle );
}

API::Accessor< API::Sampler > Controller::CreateSampler( const API::BaseFactory< API::Sampler >& factory )
{
  return { nullptr };
}

std::unique_ptr< API::Pipeline > Controller::CreatePipeline( const API::BaseFactory< API::Pipeline >& factory )
{
  auto& pipelineFactory = const_cast<PipelineFactory&>(dynamic_cast<const PipelineFactory&>( factory ));

  // if no custom cache, use default one
  if( !pipelineFactory.mPipelineCache )
  {
    pipelineFactory.mPipelineCache = mImpl->mDefaultPipelineCache.get();
  }

  return mImpl->mPipelineFactory->Create();
}

API::Accessor< API::Framebuffer > Controller::CreateFramebuffer( const API::BaseFactory< API::Framebuffer >& factory )
{
  return { nullptr };
}

std::unique_ptr< char > Controller::CreateBuffer( size_t numberOfElements, size_t elementSize )
{
  return std::unique_ptr< char >( new char[numberOfElements * elementSize] );
}

std::unique_ptr< Controller > Controller::New( Vulkan::Graphics& vulkanGraphics )
{
  return std::make_unique< Controller >( vulkanGraphics );
}

Controller::Controller( Vulkan::Graphics& vulkanGraphics ) : mImpl( std::make_unique< Impl >( *this, vulkanGraphics ) )
{
  mImpl->Initialise();
}

Controller::~Controller() = default;

Controller::Controller() = default;

Controller& Controller::operator=( Controller&& ) noexcept = default;

void Controller::BeginFrame()
{
  mImpl->BeginFrame();
}

void Controller::EndFrame()
{
  mImpl->EndFrame();
}

API::TextureFactory& Controller::GetTextureFactory() const
{
  return mImpl->GetTextureFactory();
}

API::ShaderFactory& Controller::GetShaderFactory() const
{
  return mImpl->GetShaderFactory();
}

API::BufferFactory& Controller::GetBufferFactory() const
{
  return mImpl->GetBufferFactory();
}

API::PipelineFactory& Controller::GetPipelineFactory()
{
  mImpl->mPipelineFactory->Reset();
  return *mImpl->mPipelineFactory;
}

Vulkan::Graphics& Controller::GetGraphics() const
{
  return mImpl->mGraphics;
}

void Controller::ScheduleBufferMemoryTransfer( std::unique_ptr< VulkanAPI::BufferMemoryTransfer > transferRequest )
{
  mImpl->mBufferTransferRequests.emplace_back( std::move( transferRequest ) );
}

VulkanAPI::UboManager& Controller::GetUboManager()
{
  return *mImpl->mUboManager;
}

void Controller::SubmitCommands( std::vector< API::RenderCommand* > commands )
{
  mImpl->SubmitCommands( std::move( commands ) );
}

std::unique_ptr< API::RenderCommand > Controller::AllocateRenderCommand()
{
  return mImpl->AllocateRenderCommand();
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
