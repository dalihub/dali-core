#include "generated/spv-shaders-gen.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <glm/vector_relational.hpp>

#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-graphics-controller.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-pipeline.h>
#include <dali/graphics/vulkan/vulkan-pipeline-cache.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-sampler.h>
#include <dali/graphics/vulkan/vulkan-image.h>
#include <dali/graphics/vulkan/vulkan-graphics-texture.h>
#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics/graphics-object-owner.h>


// API
#include <dali/graphics/vulkan/api/vulkan-api-shader.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-shader-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-render-command.h>

#include <dali/graphics/vulkan/api/internal/vulkan-ubo-manager.h>

#include <iostream>
#include <dali/graphics-api/utility/utility-memory-pool.h>

using namespace glm;

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
static const mat4 CLIP_MATRIX(
  1.0f,  0.0f, 0.0f, 0.0f,
  0.0f, -1.0f, 0.0f, 0.0f,
  0.0f,  0.0f, 0.5f, 0.0f,
  0.0f,  0.0f, 0.5f, 1.0f );

struct Controller::Impl
{
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
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wframe-larger-than="
  bool Initialise()
  {
    // Create factories
    mShaderFactory = std::make_unique<VulkanAPI::ShaderFactory>( mGraphics );
    mTextureFactory = std::make_unique<VulkanAPI::TextureFactory>( mGraphics );
    mBufferFactory = std::make_unique<VulkanAPI::BufferFactory>( mOwner );

    mUboManager = std::make_unique<VulkanAPI::UboManager>( mOwner );

    return true;
  }

#pragma GCC diagnostic pop
  void BeginFrame()
  {
    auto surface = mGraphics.GetSurface( 0u );

    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
    swapchain->AcquireNextFramebuffer();
  }

  void EndFrame()
  {
    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
    swapchain->Present();
  }

  API::TextureFactory& GetTextureFactory() const
  {
    return *(mTextureFactory.get());
  }

  API::ShaderFactory& GetShaderFactory() const
  {
    return *(mShaderFactory.get());
  }

  API::BufferFactory& GetBufferFactory() const
  {
    return *(mBufferFactory.get());
  }

  /**
   * NEW IMPLEMENTACIONE
   */

  std::unique_ptr<API::RenderCommand> AllocateRenderCommand()
  {
    return std::make_unique<VulkanAPI::RenderCommand>( mOwner, mGraphics, *(mPipelineCache.get()) );
  }

  /**
   * Submits number of commands in one go ( simiar to vkCmdExecuteCommands )
   * @param commands
   */
  void SubmitCommands( std::vector<Dali::Graphics::API::RenderCommand*> commands )
  {
    // if there are any scheduled writes
    if(!mBufferTransferRequests.empty())
    {
      for(auto&& req : mBufferTransferRequests )
      {
        void* dst = req->dstBuffer->GetMemoryHandle()->Map();
        memcpy( dst, &*req->srcPtr, req->srcSize );
        req->dstBuffer->GetMemoryHandle()->Unmap();
      }
      mBufferTransferRequests.clear();
    }

    std::vector<CommandBufferRef> cmdBufRefs{};

    // Prepare pipelines
    for( auto&& command : commands )
    {
      // prepare pipelines
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);
      apiCommand->PreparePipeline();
    }

    // Update uniform buffers
    for( auto&& command : commands )
    {
      // prepare pipelines
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);
      apiCommand->UpdateUniformBuffers();
    }

    mUboManager->UnmapAllBuffers();

    // set up writes
    for( auto&& command : commands )
    {
      //const auto& vertexBufferBindings = command->GetVertexBufferBindings();
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);

      //apiCommand->PreparePipeline();

      if( !mCommandPool )
      {
        mCommandPool = Vulkan::CommandPool::New( mGraphics );
      }
      // start new command buffer
      auto cmdbuf = mCommandPool->NewCommandBuffer( false );
      cmdbuf->Reset();
      cmdbuf->Begin( vk::CommandBufferUsageFlagBits::eRenderPassContinue );
      cmdbuf->BindGraphicsPipeline( apiCommand->GetPipeline() );

      // bind vertex buffers
      auto binding = 0u;
      for( auto&& vb : apiCommand->GetVertexBufferBindings() )
      {
        cmdbuf->BindVertexBuffer( binding++, static_cast<const VulkanAPI::Buffer&>(vb.buffer.Get()).GetBufferRef(), vb.offset );
      }

      // note: starting set = 0
      cmdbuf->BindDescriptorSets( apiCommand->GetDescriptorSets(), 0 );

      // draw
      const auto& drawCommand = apiCommand->GetDrawCommand();
      cmdbuf->Draw( drawCommand.vertexCount, drawCommand.instanceCount, drawCommand.firstVertex, drawCommand.firstInstance );
      cmdbuf->End();
      cmdBufRefs.emplace_back( cmdbuf );
    }

    // execute as secondary buffers
    mGraphics.GetSwapchainForFBID(0)->GetPrimaryCommandBuffer()
      ->ExecuteCommands( cmdBufRefs );

  }

  // resources
  std::vector<TextureRef> mTextures;
  std::vector<ShaderRef>  mShaders;
  std::vector<BufferRef>  mBuffers;

  // owner objects
  ObjectOwner<API::Texture>   mTexturesOwner;
  ObjectOwner<API::Shader>    mShadersOwner;
  ObjectOwner<API::Buffer>    mBuffersOwner;

  Graphics&           mGraphics;
  Controller&         mOwner;
  GpuMemoryAllocator& mDefaultAllocator;

  std::unique_ptr<VulkanAPI::TextureFactory> mTextureFactory;
  std::unique_ptr<VulkanAPI::ShaderFactory> mShaderFactory;
  std::unique_ptr<VulkanAPI::BufferFactory> mBufferFactory;

  // todo: should be per thread
  CommandPoolRef mCommandPool;

  std::vector<std::unique_ptr<VulkanAPI::BufferMemoryTransfer>> mBufferTransferRequests;

  std::unique_ptr<PipelineCache> mPipelineCache;

  std::unique_ptr<VulkanAPI::UboManager> mUboManager;

};

// TODO: @todo temporarily ignore missing return type, will be fixed later
#pragma GCC diagnostic push
#pragma GCC diagnostic     ignored "-Wreturn-type"
API::Accessor<API::Shader> Controller::CreateShader( const API::BaseFactory<API::Shader>& factory )
{
  auto handle = mImpl->mShadersOwner.CreateObject( factory );
  auto& apiShader = static_cast<VulkanAPI::Shader&>(mImpl->mShadersOwner[handle]);
  auto vertexShaderRef = apiShader.GetShaderRef( vk::ShaderStageFlagBits::eVertex );
  auto fragmentShaderRef = apiShader.GetShaderRef( vk::ShaderStageFlagBits::eFragment );
  mImpl->mShaders.push_back( vertexShaderRef );
  mImpl->mShaders.push_back( fragmentShaderRef );
  return API::Accessor<API::Shader>( mImpl->mShadersOwner, handle);
}

API::Accessor<API::Texture> Controller::CreateTexture( const API::BaseFactory<API::Texture>& factory )
{
  auto handle = mImpl->mTexturesOwner.CreateObject( factory );
  auto textureRef = static_cast<VulkanAPI::Texture&>(mImpl->mTexturesOwner[handle]).GetTextureRef();
  mImpl->mTextures.push_back( textureRef );
  return API::Accessor<API::Texture>( mImpl->mTexturesOwner, handle);
}

API::Accessor<API::TextureSet> Controller::CreateTextureSet( const API::BaseFactory<API::TextureSet>& factory )
{

}

API::Accessor<API::DynamicBuffer> Controller::CreateDynamicBuffer( const API::BaseFactory<API::DynamicBuffer>& factory )
{
}

API::Accessor<API::Buffer> Controller::CreateBuffer( const API::BaseFactory<API::Buffer>& factory )
{
  auto handle = mImpl->mBuffersOwner.CreateObject( factory );
  auto bufferRef = static_cast<VulkanAPI::Buffer&>(mImpl->mBuffersOwner[handle]).GetBufferRef();
  mImpl->mBuffers.push_back( bufferRef );
  return API::Accessor<API::Buffer>( mImpl->mBuffersOwner, handle);
}

API::Accessor<API::StaticBuffer> Controller::CreateStaticBuffer( const API::BaseFactory<API::StaticBuffer>& factory )
{
}

API::Accessor<API::Sampler> Controller::CreateSampler( const API::BaseFactory<API::Sampler>& factory )
{
}

API::Accessor<API::Framebuffer> Controller::CreateFramebuffer( const API::BaseFactory<API::Framebuffer>& factory )
{
}

std::unique_ptr<char> Controller::CreateBuffer( size_t numberOfElements, size_t elementSize )
{
  return std::unique_ptr<char>( new char[numberOfElements * elementSize] );
}

#pragma GCC diagnostic pop

std::unique_ptr<Controller> Controller::New( Graphics& vulkanGraphics )
{
  return std::make_unique<Controller>( vulkanGraphics );
}

Controller::Controller( Graphics& vulkanGraphics ) : mImpl( std::make_unique<Impl>( *this, vulkanGraphics ) )
{
  mImpl->Initialise();
}

Controller::~Controller()       = default;
Controller::Controller()        = default;
Controller& Controller::operator=( Controller&& ) = default;

void Controller::GetRenderItemList()
{
}

void Controller::SubmitCommand( Dali::Graphics::API::RenderCommand&& command )
{
  // not in use
}

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

Vulkan::Graphics& Controller::GetGraphics() const
{
  return mImpl->mGraphics;
}

void Controller::ScheduleBufferMemoryTransfer( std::unique_ptr<VulkanAPI::BufferMemoryTransfer> transferRequest )
{
  mImpl->mBufferTransferRequests.emplace_back( std::move(transferRequest) );
}

VulkanAPI::UboManager& Controller::GetUboManager()
{
  return *mImpl->mUboManager;
}

void Controller::SubmitCommands( std::vector<API::RenderCommand*> commands )
{
  mImpl->SubmitCommands( std::move(commands) );
}

std::unique_ptr<API::RenderCommand> Controller::AllocateRenderCommand()
{
  return mImpl->AllocateRenderCommand();
}


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
