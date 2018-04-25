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
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f );

/**
 * Graphics pipeline state describes single batch-draw call state
 */
struct GraphicsPipelineState
{
  /**
   * Internal Data structure collects only POD state
   * details that are relevant to us
   */
  struct Info
  {
    Info()
      : blendEnabled( VK_FALSE ),
        srcColor( vk::BlendFactor::eOne ), srcAlpha( vk::BlendFactor::eOne ),
        dstColor( vk::BlendFactor::eOne ), dstAlpha( vk::BlendFactor::eOne ),
        vertexBufferAttributesBindings{},
        viewport{},
        topology{},
        depthTestEnabled{ VK_FALSE },
        stencilTestEnabled{ VK_FALSE },
        vertexShader{ nullptr },
        fragmentShader{ nullptr }
    {
    }

    // blending
    vk::Bool32      blendEnabled;
    vk::BlendFactor srcColor, srcAlpha;
    vk::BlendFactor dstColor, dstAlpha;

    // vertex input data
    std::vector<Dali::Graphics::API::RenderCommand::VertexAttributeBufferBinding> vertexBufferAttributesBindings;

    // viewport size
    vk::Rect2D viewport;

    // topology
    vk::PrimitiveTopology topology;

    // depth/stencil
    vk::Bool32 depthTestEnabled;
    vk::Bool32 stencilTestEnabled;

    // shaders
    ShaderRef vertexShader;
    ShaderRef fragmentShader;

    bool operator==( const Info& src ) const
    {
      return false; // TODO
    }
  };

  GraphicsPipelineState( const GraphicsPipelineState& src )
  : stack( 256 )
  {
    info = src.info;
    pipeline = src.pipeline;
  }

  explicit GraphicsPipelineState( GraphicsPipelineState::Info _info )
  : info( _info ), stack( 256 )
  {

  }

  bool operator==( const GraphicsPipelineState::Info& _info )
  {
    return (info == _info);
  }

  // Resets state
  void Reset()
  {
    pipeline.Reset();
  }

  bool Initialise( Graphics& graphics )
  {
    pipeline = Pipeline::New( graphics );
    pipeline->SetShader( info.vertexShader, Shader::Type::VERTEX );
    pipeline->SetShader( info.fragmentShader, Shader::Type::FRAGMENT );
    pipeline->SetViewport( float(info.viewport.offset.x), float(info.viewport.offset.y),
                           float(info.viewport.extent.width), float(info.viewport.extent.height) );
    pipeline->SetInputAssemblyState( info.topology, true );

    // TODO: support blending and other states
    return pipeline->Compile();
  }

  // Compiles state
  static std::unique_ptr<GraphicsPipelineState> Compile(const GraphicsPipelineState::Info& info)
  {
    return nullptr;
  }

  // state, created when compiled, immutable
  // renders particular set of objects
  PipelineRef pipeline;
  Info info;

  Utility::MemoryPool stack;
};

struct GraphicsDrawCommand
{
  GraphicsPipelineState& pipelineState;

  // command buffer/pool
  CommandPoolRef    commandPool;
  CommandBufferRef  commandBuffer;
  FenceRef fence;

  // resources
  std::vector<BufferRef>    buffers;
  std::vector<ImageRef>     images;
  std::vector<SamplerRef>   samplers;
  std::vector<ImageViewRef> imageViews;

  // descriptor sets and pool
  std::vector<DescriptorSetRef> descriptorSets;
  DescriptorPoolRef             descriptorPool;
};

struct Controller::Impl
{
  struct State
  {
    ShaderRef                     vertexShader;
    ShaderRef                     fragmentShader;
    DescriptorPoolRef             descriptorPool;
    PipelineRef                   pipeline;
    BufferRef                     vertexBuffer;
    BufferRef                     uniformBuffer0;
    BufferRef                     uniformBuffer1; // clip matrix
    std::vector<DescriptorSetRef> descriptorSets;
    CommandPoolRef                commandPool;
    CommandBufferRef              drawCommand;

    std::vector<CommandBufferRef> drawCommandPool; // max 1024 secondary buffers
    uint32_t                      drawPoolIndex{0u};
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
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wframe-larger-than="
  bool Initialise()
  {
    // Create factories
    mShaderFactory = std::make_unique<VulkanAPI::ShaderFactory>( mGraphics );
    mTextureFactory = std::make_unique<VulkanAPI::TextureFactory>( mGraphics );
    mBufferFactory = std::make_unique<VulkanAPI::BufferFactory>( mOwner );

    mUboManager = std::make_unique<VulkanAPI::UboManager>( mOwner );

    //mDebugPipelineState.vertexShader = Shader::New( mGraphics, VSH_CODE.data(), VSH_CODE.size() );

    //mDebugPipelineState.fragmentShader = Shader::New( mGraphics, FSH_CODE.data(), FSH_CODE.size() );

    mDebugPipelineState.descriptorPool = CreateDescriptorPool();

    const float halfWidth  = 0.5f;
    const float halfHeight = 0.5f;
    //#if 0
    const vec3 VERTICES[4] = {
      {halfWidth, halfHeight, 0.0f},
      {halfWidth, -halfHeight, 0.0f},
      {-halfWidth, halfHeight, 0.0f},
      {-halfWidth, -halfHeight, 0.0f},
    };

    mDebugPipelineState.vertexBuffer = Buffer::New( mGraphics, sizeof( VERTICES[0] ) * 4, Buffer::Type::VERTEX );
    auto& defaultAllocator           = mGraphics.GetDeviceMemoryManager().GetDefaultAllocator();
    mDebugPipelineState.vertexBuffer->BindMemory(
      defaultAllocator.Allocate( mDebugPipelineState.vertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible ) );

    auto ptr = mDebugPipelineState.vertexBuffer->GetMemoryHandle()->MapTyped<vec3>();
    std::copy( VERTICES, VERTICES + 4, ptr );
    mDebugPipelineState.vertexBuffer->GetMemoryHandle()->Unmap();

    // create command pool
    mDebugPipelineState.commandPool = CommandPool::New( mGraphics );

    CreatePipeline( mDebugPipelineState );

    // allocated descriptor pool ( 1024 sets, 1024 uniform buffers )
    auto size = vk::DescriptorPoolSize{}.setDescriptorCount( 1024 ).setType( vk::DescriptorType::eUniformBuffer );
    mDebugPipelineState.descriptorPool = DescriptorPool::New(
      mGraphics, vk::DescriptorPoolCreateInfo{}.setPoolSizeCount( 1 ).setPPoolSizes( &size ).setMaxSets( 1024 ) );

    mDebugPipelineState.uniformBuffer1 = Buffer::New( mGraphics, sizeof( CLIP_MATRIX ), Buffer::Type::UNIFORM );
    mDebugPipelineState.uniformBuffer1->BindMemory(
      mDefaultAllocator.Allocate( mDebugPipelineState.uniformBuffer1, vk::MemoryPropertyFlagBits::eHostVisible ) );

    auto clipPtr = mDebugPipelineState.uniformBuffer1->GetMemoryHandle()->MapTyped<mat4>();
    std::copy( &CLIP_MATRIX, &CLIP_MATRIX + 1, clipPtr );
    mDebugPipelineState.uniformBuffer1->GetMemoryHandle()->Unmap();

    return true;
  }

  PipelineRef CreatePipeline( State& state )
  {
    return PipelineRef();
    auto pipeline = Pipeline::New( mGraphics );

    pipeline->SetShader( state.vertexShader, Shader::Type::VERTEX );
    pipeline->SetShader( state.fragmentShader, Shader::Type::FRAGMENT );

    auto size = mGraphics.GetSurface( 0u )->GetSize();
    pipeline->SetViewport( 0, 0, static_cast<float>( size.width ), static_cast<float>( size.height ) );

    pipeline->SetVertexInputState(
      std::vector<vk::VertexInputAttributeDescription>{
        vk::VertexInputAttributeDescription{}.setBinding( 0 ).setOffset( 0 ).setLocation( 0 ).setFormat(
          vk::Format::eR32G32B32Sfloat )},
      std::vector<vk::VertexInputBindingDescription>{vk::VertexInputBindingDescription{}
                                                       .setBinding( 0 )
                                                       .setStride( sizeof( vec3 ) )
                                                       .setInputRate( vk::VertexInputRate::eVertex )} );
    pipeline->SetInputAssemblyState( vk::PrimitiveTopology::eTriangleStrip, false );



    if( !pipeline->Compile() )
    {
      pipeline.Reset();
    }

    state.pipeline = pipeline;
    return pipeline;
  }

  void SubmitCommand( Dali::Graphics::API::RenderCommand&& command )
  {

  }
#pragma GCC diagnostic pop
  void BeginFrame()
  {
    auto surface = mGraphics.GetSurface( 0u );

    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
    swapchain->AcquireNextFramebuffer();

    // rewind pools
    mDebugPipelineState.drawPoolIndex = 0u;
    mDebugPipelineState.descriptorPool->Reset();
    mDebugPipelineState.commandPool->Reset( true );
  }

  void EndFrame()
  {
    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
    swapchain->Present();
  }

  DescriptorPoolRef CreateDescriptorPool()
  {
    vk::DescriptorPoolSize size;
    size.setDescriptorCount( 1024 ).setType( vk::DescriptorType::eUniformBuffer );

    // TODO: how to organize this???
    auto pool = DescriptorPool::New(
      mGraphics, vk::DescriptorPoolCreateInfo{}.setMaxSets( 1024 ).setPoolSizeCount( 1 ).setPPoolSizes( &size ) );
    return pool;
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

    //mUboManager->MapAllBuffers();

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

  State mDebugPipelineState;

  std::unique_ptr<VulkanAPI::TextureFactory> mTextureFactory;
  std::unique_ptr<VulkanAPI::ShaderFactory> mShaderFactory;
  std::unique_ptr<VulkanAPI::BufferFactory> mBufferFactory;

  std::vector<GraphicsPipelineState> mStatePerBatchCache;
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
  mImpl->SubmitCommand( std::move( command ) );
  //SubmitCommandTest( std::move( command ) );
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
