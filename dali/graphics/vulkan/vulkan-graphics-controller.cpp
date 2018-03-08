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
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-surface.h>

using namespace glm;

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
static const mat4 CLIP_MATRIX(
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f );

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
    mDebugPipelineState.vertexShader = Shader::New( mGraphics, VSH_CODE.data(), VSH_CODE.size() );

    mDebugPipelineState.fragmentShader = Shader::New( mGraphics, FSH_CODE.data(), FSH_CODE.size() );

    mDebugPipelineState.descriptorPool = CreateDescriptorPool();

    const float halfWidth = 0.5f;
    const float halfHeight = 0.5f;
//#if 0
    const vec3 VERTICES[4] =
                  {
                    { halfWidth,  halfHeight, 0.0f },
                    { halfWidth, -halfHeight, 0.0f },
                    { -halfWidth,  halfHeight, 0.0f },
                    { -halfWidth, -halfHeight, 0.0f },
                  };
//#endif
#if 0
    const vec3 VERTICES[4] = {
      {-halfWidth, -halfHeight, 0.0f},
      {halfWidth,  -halfHeight, 0.0f},
      {halfWidth,  halfHeight, 0.0f},
      {-halfWidth, halfHeight, 0.0f}
    };
#endif
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
    auto pipeline = Pipeline::New( mGraphics );

    pipeline->SetShader( state.vertexShader, Shader::Type::VERTEX );
    pipeline->SetShader( state.fragmentShader, Shader::Type::FRAGMENT );

    auto size = mGraphics.GetSurface( 0u ).GetSize();
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

  void SubmitCommand( API::RenderCommand&& command )
  {
    auto& state = mDebugPipelineState;

    const auto& bufferList = command.GetBufferList();
    auto        drawcalls  = command.GetPrimitiveCount();

    // create pool of commands to be re-recorded
    if( state.drawCommandPool.empty() )
    {
      for( auto i = 0u; i < 1024; ++i )
      {
        state.drawCommandPool.push_back( state.commandPool->NewCommandBuffer( false ) );
      }
    }

    uint32_t                      stride = sizeof( mat4 ) + sizeof( vec4 ) + sizeof( vec3 );
    std::vector<CommandBufferRef> executeCommands;
    for( auto&& buf : bufferList.Get() )
    {
      // TODO: @todo implement minimum offset!
      const uint32_t sizeOfUniformBuffer = U32((buf->GetSize() / drawcalls.Get()));
      const uint32_t uniformBlockOffsetStride = ((sizeOfUniformBuffer / 256)+1)*256;
      const uint32_t uniformBlockMemoryNeeded = U32(uniformBlockOffsetStride*drawcalls.Get());

      // create buffer if doesn't exist
      if( !state.uniformBuffer0 )
      {
        state.uniformBuffer0 = Buffer::New( mGraphics, uniformBlockMemoryNeeded, Buffer::Type::UNIFORM );
      }
      if( state.uniformBuffer0->GetSize() < uniformBlockMemoryNeeded || !state.uniformBuffer0->GetMemoryHandle() )
      {
        // allocate and bind memory if needed ( buffer increased size or buffer hasn't been bound yet )
        state.uniformBuffer0->BindMemory(
          mDefaultAllocator.Allocate( state.uniformBuffer0, vk::MemoryPropertyFlagBits::eHostVisible ) );
      }

      // fill memory

      struct UB
      {
        mat4 mvp;
        vec4 color;
        vec3 size;
      } __attribute__((aligned(16)));

      auto memory = state.uniformBuffer0->GetMemoryHandle();
      auto outPtr = memory->MapTyped<char>();
      for( auto i = 0u; i < drawcalls.Get(); ++i )
      {
        // copy chunk of data
        UB* inputData = (reinterpret_cast<UB*>(buf->GetDataBase())) + i;
        UB* outputData = (reinterpret_cast<UB*>(outPtr + (i*uniformBlockOffsetStride)));
        *outputData = *inputData;

        auto descriptorSets = state.descriptorPool->AllocateDescriptorSets(
          vk::DescriptorSetAllocateInfo{}.setDescriptorSetCount( 1 ).setPSetLayouts(
            state.pipeline->GetVkDescriptorSetLayouts().data() ) );

        descriptorSets[0]->WriteUniformBuffer( 0, state.uniformBuffer0, i * uniformBlockOffsetStride, stride );
        descriptorSets[0]->WriteUniformBuffer( 1, state.uniformBuffer1, 0, state.uniformBuffer1->GetSize() );

        // record draw call
        auto cmdbuf = state.commandPool->NewCommandBuffer( false );
        cmdbuf->Begin( vk::CommandBufferUsageFlagBits::eRenderPassContinue );
        cmdbuf->BindVertexBuffer( 0, state.vertexBuffer, 0 );
        cmdbuf->BindGraphicsPipeline( state.pipeline );
        cmdbuf->BindDescriptorSets( descriptorSets, 0 );
        cmdbuf->Draw( 4, 1, 0, 0 );
        cmdbuf->End();

        executeCommands.push_back( cmdbuf );
      }

      memory->Unmap();

      // execute buffer
      mGraphics.GetSurface( 0u ).GetCurrentCommandBuffer()->ExecuteCommands( executeCommands );

      // break, one pass only
      break;
    }
  }
#pragma GCC diagnostic pop
  void BeginFrame()
  {
    auto& surface = mGraphics.GetSurface( 0u );
    surface.AcquireNextImage();

    // rewind pools
    mDebugPipelineState.drawPoolIndex = 0u;
    mDebugPipelineState.descriptorPool->Reset();
    mDebugPipelineState.commandPool->Reset( true );
  }

  void EndFrame()
  {
    auto& surface = mGraphics.GetSurface( 0u );
    surface.Present();
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

  Graphics&           mGraphics;
  Controller&         mOwner;
  GpuMemoryAllocator& mDefaultAllocator;

  State mDebugPipelineState;
};

// TODO: @todo temporarily ignore missing return type, will be fixed later
#pragma GCC diagnostic push
#pragma GCC diagnostic     ignored "-Wreturn-type"
API::Accessor<API::Shader> Controller::CreateShader( const API::BaseFactory<API::Shader>& factory )
{
}

API::Accessor<API::Texture> Controller::CreateTexture( const API::BaseFactory<API::Texture>& factory )
{
}

API::Accessor<API::TextureSet> Controller::CreateTextureSet( const API::BaseFactory<API::TextureSet>& factory )
{
}

API::Accessor<API::DynamicBuffer> Controller::CreateDynamicBuffer( const API::BaseFactory<API::DynamicBuffer>& factory )
{
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

void Controller::SubmitCommand( API::RenderCommand&& command )
{
  mImpl->SubmitCommand( std::move( command ) );
}

void Controller::BeginFrame()
{
  mImpl->BeginFrame();
}

void Controller::EndFrame()
{
  mImpl->EndFrame();
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
