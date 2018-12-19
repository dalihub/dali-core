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

#include <dali/graphics/vulkan/api/vulkan-api-controller.h>

#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics/vulkan/internal/vulkan-surface.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-swapchain.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>

// API
#include <dali/graphics/vulkan/api/vulkan-api-shader.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-framebuffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-shader-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline.h>
#include <dali/graphics/vulkan/api/vulkan-api-pipeline-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-framebuffer-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-sampler-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-render-command.h>
#include <dali/graphics/vulkan/api/internal/vulkan-pipeline-cache.h>
#include <dali/graphics/vulkan/api/internal/vulkan-api-descriptor-set-allocator.h>

#include <dali/graphics/thread-pool.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
namespace DepthStencilFlagBits
{
static constexpr uint32_t DEPTH_BUFFER_BIT   = 1; // depth buffer enabled
static constexpr uint32_t STENCIL_BUFFER_BIT = 2; // stencil buffer enabled
}

// State of the depth-stencil buffer
using DepthStencilFlags = uint32_t;

struct Controller::Impl
{
  struct RenderPassData
  {
    // only move semantics
    RenderPassData( vk::RenderPassBeginInfo _beginInfo,
                    std::vector< vk::ClearValue >&& _clearColorValues,
                    Vulkan::RefCountedFramebuffer _framebuffer )
      : beginInfo( _beginInfo ),
        colorValues( std::move( _clearColorValues ) ),
        framebuffer( std::move( _framebuffer ) ),
        renderCommands()
    {
    }

    // no default constructor!
    RenderPassData() = delete;

    vk::RenderPassBeginInfo beginInfo{};
    std::vector< vk::ClearValue > colorValues{};
    Vulkan::RefCountedFramebuffer framebuffer{};
    std::vector< Dali::Graphics::API::RenderCommand* > renderCommands;
  };

  Impl( Controller& owner, Dali::Graphics::Vulkan::Graphics& graphics )
  : mGraphics( graphics ),
    mOwner( owner ),
    mDepthStencilBufferCurrentState( 0u ),
    mDepthStencilBufferRequestedState( 0u )
  {
  }

  ~Impl() = default;

  // TODO: @todo this function initialises basic buffers, shaders and pipeline
  // for the prototype ONLY
  bool Initialise()
  {
    // Create factories
    mShaderFactory = MakeUnique< VulkanAPI::ShaderFactory >( mGraphics );
    mTextureFactory = MakeUnique< VulkanAPI::TextureFactory >( mOwner );
    mBufferFactory = MakeUnique< VulkanAPI::BufferFactory >( mOwner );
    mFramebufferFactory = MakeUnique< VulkanAPI::FramebufferFactory >( mOwner );
    mPipelineFactory = MakeUnique< VulkanAPI::PipelineFactory >( mOwner );
    mSamplerFactory = MakeUnique< VulkanAPI::SamplerFactory >( mOwner );

    mDefaultPipelineCache = MakeUnique< VulkanAPI::PipelineCache >();
    mDescriptorSetAllocator = MakeUnique< VulkanAPI::Internal::DescriptorSetAllocator>( mOwner );

    return mThreadPool.Initialize();
  }

  void UpdateDepthStencilBuffer()
  {
    // if we enable depth/stencil dynamically we need to block and invalidate pipeline cache
    // enable depth-stencil
    if( mDepthStencilBufferCurrentState != mDepthStencilBufferRequestedState )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "UpdateDepthStencilBuffer(): New state: DEPTH: %d, STENCIL: %d\n", int(mDepthStencilBufferRequestedState&1), int((mDepthStencilBufferRequestedState>>1)&1) );

      // Formats
      const std::array<vk::Format, 4> DEPTH_STENCIL_FORMATS = {
        vk::Format::eUndefined,       // no depth nor stencil needed
        vk::Format::eD16Unorm,        // only depth buffer
        vk::Format::eS8Uint,          // only stencil buffer
        vk::Format::eD24UnormS8Uint   // depth and stencil buffers
      };

      mGraphics.DeviceWaitIdle();

      mGraphics.GetSwapchainForFBID(0u)->SetDepthStencil( DEPTH_STENCIL_FORMATS[mDepthStencilBufferRequestedState] );

      // make sure GPU finished any pending work
      mGraphics.DeviceWaitIdle();

      mDepthStencilBufferCurrentState = mDepthStencilBufferRequestedState;
    }
  }

  void AcquireNextFramebuffer()
  {
    // for all swapchains acquire new framebuffer
    auto surface = mGraphics.GetSurface( 0u );

    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );

    if ( mGraphics.IsSurfaceResized() )
    {
      swapchain->Invalidate();
    }

    // We won't run garbage collection in case there are pending resource transfers.
    swapchain->AcquireNextFramebuffer( !mOwner.HasPendingResourceTransfers() );

    if( !swapchain->IsValid() )
    {
      // make sure device doesn't do any work before replacing swapchain
      mGraphics.DeviceWaitIdle();

      // replace swapchain
      swapchain = mGraphics.ReplaceSwapchainForSurface( surface, std::move(swapchain) );

      // get new valid framebuffer
      swapchain->AcquireNextFramebuffer( !mOwner.HasPendingResourceTransfers() );
    }
  }

  void CompilePipelines()
  {
    // could be using another thread...could be...
    mDefaultPipelineCache->Compile();
  }

  void BeginFrame()
  {
    // Acquire next framebuffer image
    AcquireNextFramebuffer();

    // Compile all pipelines that haven't been initialised yet
    CompilePipelines();

    mRenderPasses.clear();
    mCurrentFramebuffer.Reset();
  }

  void EndFrame()
  {
    // Update descriptor sets if there are any updates
    // swap all swapchains
    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );

    if( !mRenderPasses.empty() )
    {
      // Ensure there are enough command buffers for each render pass,
      swapchain->AllocateCommandBuffers( mRenderPasses.size() );
      std::vector<Vulkan::RefCountedCommandBuffer>& renderPassBuffers = swapchain->GetCommandBuffers();
      uint32_t index = 0;
      for( auto& renderPassData : mRenderPasses )
      {
        ProcessRenderPassData( renderPassBuffers[index], renderPassData );
        ++index;
      }
    }
    else
    {
      auto primaryCommandBuffer = swapchain->GetLastCommandBuffer();
      primaryCommandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr );
      primaryCommandBuffer->BeginRenderPass( vk::RenderPassBeginInfo{}
        .setFramebuffer( swapchain->GetCurrentFramebuffer()->GetVkHandle() )
        .setRenderPass(swapchain->GetCurrentFramebuffer()->GetRenderPass() )
        .setRenderArea( { {0, 0}, { swapchain->GetCurrentFramebuffer()->GetWidth(), swapchain->GetCurrentFramebuffer()->GetHeight() } } )
        .setPClearValues( swapchain->GetCurrentFramebuffer()->GetClearValues().data() )
        .setClearValueCount( uint32_t(swapchain->GetCurrentFramebuffer()->GetClearValues().size()) ), vk::SubpassContents::eInline );
      primaryCommandBuffer->EndRenderPass();
      primaryCommandBuffer->End();
    }

    for( auto& future : mMemoryTransferFutures )
    {
      future->Wait();
      future.reset();
    }

    mMemoryTransferFutures.clear();

    swapchain->Present();
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

  API::FramebufferFactory& GetFramebufferFactory() const
  {
    mFramebufferFactory->Reset();
    return *( mFramebufferFactory.get() );
  }


  std::unique_ptr< API::RenderCommand > AllocateRenderCommand()
  {
    return std::make_unique< VulkanAPI::RenderCommand >( mOwner, mGraphics );
  }

  void UpdateRenderPass( const std::vector< Dali::Graphics::API::RenderCommand*  >& commands, uint32_t startIndex, uint32_t endIndex )
  {
    auto firstCommand = static_cast<VulkanAPI::RenderCommand*>(commands[startIndex]);
    auto renderTargetBinding = firstCommand->GetRenderTargetBinding();

    Vulkan::RefCountedFramebuffer framebuffer{ nullptr };
    if( renderTargetBinding.framebuffer )
    {
      framebuffer = static_cast<const VulkanAPI::Framebuffer&>(*renderTargetBinding.framebuffer).GetFramebufferRef();
    }
    else
    {
      // use first surface/swapchain as render target
      auto surface = mGraphics.GetSurface( 0u );
      auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
      framebuffer = swapchain->GetCurrentFramebuffer();
    }

    if( framebuffer != mCurrentFramebuffer )
    {
      mCurrentFramebuffer = framebuffer;

      // @todo Only do if there is a color attachment and color clear is enabled and there is a clear color
      auto newColors = mCurrentFramebuffer->GetClearValues();
      newColors[0].color.setFloat32( { renderTargetBinding.clearColors[0].r,
                                       renderTargetBinding.clearColors[0].g,
                                       renderTargetBinding.clearColors[0].b,
                                       renderTargetBinding.clearColors[0].a
                                     } );

      mRenderPasses.emplace_back(
              vk::RenderPassBeginInfo{}
                      .setRenderPass( mCurrentFramebuffer->GetRenderPass() )
                      .setFramebuffer( mCurrentFramebuffer->GetVkHandle() )
                      .setRenderArea( vk::Rect2D( { 0, 0 }, { mCurrentFramebuffer->GetWidth(),
                                                              mCurrentFramebuffer->GetHeight() } ) )
                      .setClearValueCount( uint32_t( newColors.size() ) )
                      .setPClearValues( newColors.data() ),
              std::move( newColors ),
              framebuffer );

    }

    auto& vector = mRenderPasses.back().renderCommands;
    vector.insert( vector.end(), commands.begin()+int(startIndex), commands.begin()+int(endIndex) );
  }

  /**
   * Submits number of commands in one go ( simiar to vkCmdExecuteCommands )
   * @param commands
   */
  void SubmitCommands( std::vector< Dali::Graphics::API::RenderCommand* > commands )
  {
    /*
     * analyze descriptorset needs pers signature
     */
    std::vector<DescriptorSetRequirements> dsRequirements {};
    for( auto& command : commands )
    {
      auto vulkanCommand = static_cast<VulkanAPI::RenderCommand*>( command );
      vulkanCommand->UpdateDescriptorSetAllocationRequirements( dsRequirements, *mDescriptorSetAllocator.get() );
    }

    if( !mDescriptorSetsFreeList.empty() )
    {
      mDescriptorSetAllocator->FreeDescriptorSets( std::move( mDescriptorSetsFreeList ) );
      mDescriptorSetsFreeList.clear();
    }

    /*
     * Update descriptor pools based on the requirements
     */
    if( dsRequirements.size() )
    {
      mDescriptorSetAllocator->UpdateWithRequirements( dsRequirements, 0u );
    }

    /**
     * Allocate descriptor sets for all signatures that requirements forced
     * recreating pools
     */
    for( auto& command : commands )
    {
      auto vulkanCommand = static_cast<VulkanAPI::RenderCommand*>( command );
      vulkanCommand->AllocateDescriptorSets( *mDescriptorSetAllocator );
    }

    mMemoryTransferFutures.emplace_back( mThreadPool.SubmitTask(0, Task([this](uint32_t workerIndex){
      // if there are any scheduled writes
      if( !mBufferTransferRequests.empty() )
      {
        for( auto&& req : mBufferTransferRequests )
        {
          void* dst = req->dstBuffer->GetMemory()->Map();
          memcpy( dst, &*req->srcPtr, req->srcSize );
          req->dstBuffer->GetMemory()->Flush();
          req->dstBuffer->GetMemory()->Unmap();
        }
        mBufferTransferRequests.clear();
      }

      // execute all scheduled resource transfers
      ProcessResourceTransferRequests();
    })) );

    // the list of commands may be empty, but still we may have scheduled memory
    // transfers
    if( commands.empty() )
    {
      return;
    }

    // Update render pass data per framebuffer
    const Dali::Graphics::API::Framebuffer* currFramebuffer = nullptr;
    uint32_t previousPassBeginIndex = 0u;
    uint32_t index = 0u;
    for( auto& command : commands )
    {
      if( command->mRenderTargetBinding.framebuffer != currFramebuffer )
      {
        if( index )
        {
          UpdateRenderPass( commands, previousPassBeginIndex, index );
        }
        previousPassBeginIndex = index;
        currFramebuffer = command->mRenderTargetBinding.framebuffer;
      }
      ++index;
    }
    UpdateRenderPass( commands, previousPassBeginIndex, index );
  }

  // @todo: possibly optimise
  bool TestCopyRectIntersection( const ResourceTransferRequest* srcRequest, const ResourceTransferRequest* currentRequest )
  {
    auto srcOffset = srcRequest->bufferToImageInfo.copyInfo.imageOffset;
    auto srcExtent = srcRequest->bufferToImageInfo.copyInfo.imageExtent;

    auto curOffset = currentRequest->bufferToImageInfo.copyInfo.imageOffset;
    auto curExtent = currentRequest->bufferToImageInfo.copyInfo.imageExtent;

    auto offsetX0 = std::min( srcOffset.x, curOffset.x );
    auto offsetY0 = std::min( srcOffset.y, curOffset.y );
    auto offsetX1 = std::max( srcOffset.x+int32_t(srcExtent.width), curOffset.x+int32_t(curExtent.width) );
    auto offsetY1 = std::max( srcOffset.y+int32_t(srcExtent.height), curOffset.y+int32_t(curExtent.height) );

    return ( (offsetX1 - offsetX0) < (int32_t(srcExtent.width) + int32_t(curExtent.width)) &&
             (offsetY1 - offsetY0) < (int32_t(srcExtent.height) + int32_t(curExtent.height)) );

  }


  void ProcessResourceTransferRequests( bool immediateOnly = false )
  {
    std::lock_guard<std::recursive_mutex> lock(mResourceTransferMutex);
    if(!mResourceTransferRequests.empty())
    {
      using ResourceTransferRequestList = std::vector<const ResourceTransferRequest*>;

      /**
       * Structure associating unique images and lists of transfer requests for which
       * the key image is a destination. It contains separate lists of requests per image.
       * Each list of requests groups non-intersecting copy operations into smaller batches.
       */
      struct ResourceTransferRequestPair
      {
        ResourceTransferRequestPair( const Vulkan::RefCountedImage& key )
        : image( key ), requestList{{}}
        {
        }

        Vulkan::RefCountedImage                  image;
        std::vector<ResourceTransferRequestList> requestList;
      };

      // Map of all the requests where 'image' is a key.
      std::vector<ResourceTransferRequestPair> requestMap;

      auto highestBatchIndex = 1u;

      // Collect all unique destination images and all transfer requests associated with them
      for( const auto& req : mResourceTransferRequests )
      {
        Vulkan::RefCountedImage image { nullptr };
        if( req.requestType == TransferRequestType::BUFFER_TO_IMAGE )
        {
          image = req.bufferToImageInfo.dstImage;
        }
        else if ( req.requestType == TransferRequestType::IMAGE_TO_IMAGE )
        {
          image = req.imageToImageInfo.dstImage;
        }

        assert( image );

        auto predicate = [&]( auto& item )->bool {
          return image->GetVkHandle() == item.image->GetVkHandle();
        };
        auto it = std::find_if( requestMap.begin(), requestMap.end(), predicate );

        if( it == requestMap.end() )
        {
          // initialise new array
          requestMap.emplace_back( image );
          it = requestMap.end()-1;
        }

        auto& transfers = it->requestList;

        // Compare with current transfer list whether there are any intersections
        // with current image copy area. If intersection occurs, start new list
        auto& currentList = transfers.back();

        bool intersects( false );
        for( auto& item : currentList )
        {
          // if area intersects create new list
          if( (intersects = TestCopyRectIntersection( item, &req )) )
          {
            transfers.push_back({});
            highestBatchIndex = std::max( highestBatchIndex, uint32_t(transfers.size()) );
            break;
          }
        }

        // push request to the most recently created list
        transfers.back().push_back( &req );
      }

      // For all unique images prepare layout transition barriers as all of them must be
      // in eTransferDstOptimal layout
      std::vector<vk::ImageMemoryBarrier> preLayoutBarriers;
      std::vector<vk::ImageMemoryBarrier> postLayoutBarriers;
      for( auto& item : requestMap )
      {
        auto image = item.image;
        // add barrier
        preLayoutBarriers.push_back( mGraphics.CreateImageMemoryBarrier( image, image->GetImageLayout(), vk::ImageLayout::eTransferDstOptimal ) );
        postLayoutBarriers.push_back( mGraphics.CreateImageMemoryBarrier( image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal ) );
        image->SetImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal );
      }

      // Build command buffer for each image until reaching next sync point
      auto commandBuffer = mGraphics.CreateCommandBuffer( true );

      // Fence between submissions
      auto fence = mGraphics.CreateFence( {} );

      /**
       * The loop iterates through requests for each unique image. It parallelizes
       * transfers to images until end of data in the batch.
       * After submitting copy commands the loop waits for the fence to be signalled
       * and repeats recording for the next batch of transfer requests.
       */
      for( auto i = 0u; i < highestBatchIndex; ++i )
      {
        commandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr );

        // change image layouts only once
        if( i == 0 )
        {
          commandBuffer->PipelineBarrier( vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, preLayoutBarriers );
        }

        for( auto& item : requestMap )
        {
          auto& batchItem = item.requestList;
          if( batchItem.size() <= i )
          {
            continue;
          }

          auto& requestList = batchItem[i];

          // record all copy commands for this batch
          for( auto& req : requestList )
          {
            if( req->requestType == TransferRequestType::BUFFER_TO_IMAGE )
            {
              commandBuffer->CopyBufferToImage( req->bufferToImageInfo.srcBuffer,
                                                req->bufferToImageInfo.dstImage, vk::ImageLayout::eTransferDstOptimal,
                                                { req->bufferToImageInfo.copyInfo } );

            }
            else if( req->requestType == TransferRequestType::IMAGE_TO_IMAGE )
            {
              commandBuffer->CopyImage( req->imageToImageInfo.srcImage, vk::ImageLayout::eTransferSrcOptimal,
                                        req->imageToImageInfo.dstImage, vk::ImageLayout::eTransferDstOptimal,
                                        { req->imageToImageInfo.copyInfo });
            }
          }
        }

        // if this is the last batch restore original layouts
        if( i == highestBatchIndex - 1 )
        {
          commandBuffer->PipelineBarrier( vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, postLayoutBarriers );
        }
        commandBuffer->End();

        // submit to the queue
        mGraphics.Submit( mGraphics.GetTransferQueue(0u), { Vulkan::SubmissionData{ {}, {}, { commandBuffer }, {} } }, fence );
        mGraphics.WaitForFence( fence );
        mGraphics.ResetFence( fence );
      }

      // Destroy staging resources immediately
      for( auto& request : mResourceTransferRequests )
      {
        if( request.requestType == TransferRequestType::BUFFER_TO_IMAGE )
        {
          auto& buffer = request.bufferToImageInfo.srcBuffer;
          assert( buffer.GetRefCount() == 1 );
          buffer->DestroyNow();
        }
        else if( request.requestType == TransferRequestType::IMAGE_TO_IMAGE )
        {
          auto& image = request.imageToImageInfo.srcImage;
          assert( image.GetRefCount() == 1 );
          image->DestroyNow();
        }
      }

      // Clear transfer queue
      mResourceTransferRequests.clear();
    }
  }

  void InvalidateSwapchain()
  {
    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
    swapchain->Invalidate();
  }

  void ProcessRenderPassData( Vulkan::RefCountedCommandBuffer commandBuffer, const RenderPassData& renderPassData )
  {
    commandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr );
    commandBuffer->BeginRenderPass( renderPassData.beginInfo, vk::SubpassContents::eInline );

    // update descriptor sets
    for( auto&& command : renderPassData.renderCommands )
    {
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);
      apiCommand->PrepareResources();
    }

    if( mDescriptorWrites.size() )
    {
      mGraphics.GetDevice().updateDescriptorSets( uint32_t( mDescriptorWrites.size() ), mDescriptorWrites.data(), 0, nullptr );
      mDescriptorWrites.clear();
      mDescriptorInfoStack.clear();
    }

    for( auto&& command : renderPassData.renderCommands )
    {
#if defined(DEBUG_ENABLED)
      if( getenv( "LOG_VULKAN_API" ) )
      {
        DALI_LOG_STREAM( gVulkanFilter, Debug::General, *command );
      }
#endif

      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);

      // skip if there's no valid pipeline
      if( !apiCommand->GetVulkanPipeline() )
      {
        continue;
      }

      apiCommand->BindPipeline( commandBuffer );

      //@todo add assert to check the pipeline render pass nad the inherited render pass are the same

      // set dynamic state
      if( apiCommand->mDrawCommand.scissorTestEnable )
      {
        vk::Rect2D scissorRect( { apiCommand->mDrawCommand.scissor.x,
                                  apiCommand->mDrawCommand.scissor.y },
                                { apiCommand->mDrawCommand.scissor.width,
                                  apiCommand->mDrawCommand.scissor.height } );

        commandBuffer->SetScissor( 0, 1, &scissorRect );
      }

      // dynamic state: viewport
      auto vulkanApiPipeline = static_cast<const VulkanAPI::Pipeline*>(apiCommand->GetPipeline());

      auto dynamicStateMask = vulkanApiPipeline->GetDynamicStateMask();
      if( (dynamicStateMask & API::PipelineDynamicStateBits::VIEWPORT_BIT) && apiCommand->mDrawCommand.viewportEnable )
      {
        auto viewportRect = apiCommand->mDrawCommand.viewport;

        vk::Viewport viewport( viewportRect.x,
                               viewportRect.y,
                               viewportRect.width,
                               viewportRect.height,
                               viewportRect.minDepth,
                               viewportRect.maxDepth );

        commandBuffer->SetViewport( 0, 1, &viewport );
      }

      // bind vertex buffers
      auto binding = 0u;
      for( auto&& vb : apiCommand->GetVertexBufferBindings() )
      {
        commandBuffer->BindVertexBuffer( binding++, static_cast<const VulkanAPI::Buffer&>( *vb ).GetBufferRef(), 0 );
      }

      // note: starting set = 0
      const auto& descriptorSets = apiCommand->GetDescriptorSets();
      commandBuffer->BindDescriptorSets( descriptorSets, vulkanApiPipeline->GetVkPipelineLayout(), 0, uint32_t( descriptorSets.size() ) );

      // draw
      const auto& drawCommand = apiCommand->GetDrawCommand();

      const auto& indexBinding = apiCommand->GetIndexBufferBinding();
      if( indexBinding.buffer )
      {
        commandBuffer->BindIndexBuffer( static_cast<const VulkanAPI::Buffer&>(*indexBinding.buffer).GetBufferRef(),
                                 0, vk::IndexType::eUint16 );
        commandBuffer->DrawIndexed( drawCommand.indicesCount,
                             drawCommand.instanceCount,
                             drawCommand.firstIndex,
                             0,
                             drawCommand.firstInstance );
      }
      else
      {
        commandBuffer->Draw( drawCommand.vertexCount,
                      drawCommand.instanceCount,
                      drawCommand.firstVertex,
                      drawCommand.firstInstance );
      }
    }

    commandBuffer->EndRenderPass();
    commandBuffer->End();
  }

  bool EnableDepthStencilBuffer( bool enableDepth, bool enableStencil )
  {
    mDepthStencilBufferRequestedState = ( enableDepth ? DepthStencilFlagBits::DEPTH_BUFFER_BIT : 0u ) |
                                        ( enableStencil ? DepthStencilFlagBits::STENCIL_BUFFER_BIT : 0u );

    auto retval = mDepthStencilBufferRequestedState != mDepthStencilBufferCurrentState;

    UpdateDepthStencilBuffer();

    return retval;
  }

  void RunGarbageCollector( size_t numberOfDiscardedRenderers )
  {
    // @todo Decide what GC's to run.
  }

  void DiscardUnusedResources()
  {
    mGraphics.GetGraphicsQueue(0).GetVkHandle().waitIdle();

    mGraphics.GetSwapchainForFBID(0)->ResetAllCommandBuffers();

    mGraphics.CollectGarbage();

    mDescriptorSetAllocator->InvalidateAllDescriptorSets();
  }

  bool IsDiscardQueueEmpty()
  {
    return mGraphics.GetDiscardQueue(0u).empty() && mGraphics.GetDiscardQueue(1u).empty();
  }

  void SwapBuffers()
  {
    mGraphics.SwapBuffers();
  }

  uint32_t GetSwapchainBufferCount()
  {
    return mGraphics.GetSwapchainForFBID(0u)->GetImageCount();
  }

  std::unique_ptr< PipelineCache > mDefaultPipelineCache;
  std::unique_ptr< VulkanAPI::Internal::DescriptorSetAllocator > mDescriptorSetAllocator;

  Vulkan::Graphics& mGraphics;
  Controller& mOwner;

  std::unique_ptr< VulkanAPI::TextureFactory > mTextureFactory;
  std::unique_ptr< VulkanAPI::ShaderFactory > mShaderFactory;
  std::unique_ptr< VulkanAPI::BufferFactory > mBufferFactory;
  std::unique_ptr< VulkanAPI::PipelineFactory > mPipelineFactory;
  std::unique_ptr< VulkanAPI::FramebufferFactory > mFramebufferFactory;
  std::unique_ptr< VulkanAPI::SamplerFactory > mSamplerFactory;

  // used for UBOs
  std::vector< std::unique_ptr< VulkanAPI::BufferMemoryTransfer > > mBufferTransferRequests;

  // used for texture<->buffer<->memory transfers
  std::vector< ResourceTransferRequest > mResourceTransferRequests;

  Vulkan::RefCountedFramebuffer mCurrentFramebuffer;
  std::vector< RenderPassData > mRenderPasses;

  ThreadPool mThreadPool;

  std::vector< std::shared_ptr< Future<void> > > mMemoryTransferFutures;

  std::vector<vk::WriteDescriptorSet> mDescriptorWrites;

  struct DescriptorInfo
  {
    vk::DescriptorImageInfo imageInfo;
    vk::DescriptorBufferInfo bufferInfo;
  };

  std::deque<DescriptorInfo> mDescriptorInfoStack;

  std::mutex mDescriptorWriteMutex{};
  std::recursive_mutex mResourceTransferMutex{};

  DepthStencilFlags mDepthStencilBufferCurrentState { 0u };
  DepthStencilFlags mDepthStencilBufferRequestedState { 0u };

  std::vector<DescriptorSetList> mDescriptorSetsFreeList;

  uint32_t mBufferIndex{0u};
  bool mDrawOnResume{ false };
};

// TODO: @todo temporarily ignore missing return type, will be fixed later

std::unique_ptr< API::Shader > Controller::CreateShader( const API::BaseFactory< API::Shader >& factory )
{
  return factory.Create();
}

std::unique_ptr< API::Texture > Controller::CreateTexture( const API::BaseFactory< API::Texture >& factory )
{
  return factory.Create();
}

std::unique_ptr< API::Buffer > Controller::CreateBuffer( const API::BaseFactory< API::Buffer >& factory )
{
  return factory.Create();
}

std::unique_ptr< API::Sampler > Controller::CreateSampler( const API::BaseFactory< API::Sampler >& factory )
{
  return factory.Create();
}

std::unique_ptr< API::Pipeline > Controller::CreatePipeline( const API::PipelineFactory& factory )
{
  auto& pipelineFactory = const_cast<PipelineFactory&>(dynamic_cast<const PipelineFactory&>( factory ));

  // if no custom cache, use default one
  if( !pipelineFactory.mPipelineCache )
  {
    pipelineFactory.mPipelineCache = mImpl->mDefaultPipelineCache.get();
  }

  return mImpl->mPipelineFactory->Create();
}

std::unique_ptr< API::Framebuffer > Controller::CreateFramebuffer( const API::BaseFactory< API::Framebuffer >& factory )
{
  return factory.Create();
}

std::unique_ptr< Controller > Controller::New( Vulkan::Graphics& vulkanGraphics )
{
  return std::make_unique< Controller >( vulkanGraphics );
}

Controller::Controller( Vulkan::Graphics& vulkanGraphics ) : mImpl( std::make_unique< Impl >( *this, vulkanGraphics ) )
{
}

void Controller::Initialise()
{
  mImpl->Initialise();
}

Controller::~Controller() = default;

Controller::Controller() = default;

Controller& Controller::operator=( Controller&& ) noexcept = default;

void Controller::BeginFrame()
{
  mImpl->mDrawOnResume = false;
  mStats.samplerTextureBindings = 0;
  mStats.uniformBufferBindings = 0;

  //@ todo, not multithreaded yet
  mImpl->mDescriptorWrites.clear();

  mStats.frame++;
  mImpl->BeginFrame();
}

void Controller::PushDescriptorWrite( const vk::WriteDescriptorSet& write )
{
  vk::DescriptorImageInfo* pImageInfo { nullptr };
  vk::DescriptorBufferInfo* pBufferInfo { nullptr };
  std::lock_guard<std::mutex> lock( mImpl->mDescriptorWriteMutex );
  if( write.pImageInfo )
  {
    mImpl->mDescriptorInfoStack.emplace_back();
    mImpl->mDescriptorInfoStack.back().imageInfo = *write.pImageInfo;
    pImageInfo = &mImpl->mDescriptorInfoStack.back().imageInfo;
  }
  else if( write.pBufferInfo )
  {
    mImpl->mDescriptorInfoStack.emplace_back();
    mImpl->mDescriptorInfoStack.back().bufferInfo = *write.pBufferInfo;
    pBufferInfo = &mImpl->mDescriptorInfoStack.back().bufferInfo;
  }
  mImpl->mDescriptorWrites.emplace_back( write );
  mImpl->mDescriptorWrites.back().pBufferInfo = pBufferInfo;
  mImpl->mDescriptorWrites.back().pImageInfo = pImageInfo;
}

void Controller::FreeDescriptorSets( DescriptorSetList&& descriptorSetList )
{
  if( descriptorSetList.descriptorSets.empty() )
  {
    return;
  }
  mImpl->mDescriptorSetsFreeList.emplace_back( std::move( descriptorSetList ) );
}

bool Controller::TestDescriptorSetsValid( VulkanAPI::DescriptorSetList& descriptorSetList, std::vector<bool>& results ) const
{
  return mImpl->mDescriptorSetAllocator->TestIfValid( descriptorSetList, results );
}

void Controller::EndFrame()
{
  mImpl->EndFrame();

#if(DEBUG_ENABLED)
  // print stats
  PrintStats();
#endif
}

void Controller::PrintStats()
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Frame: %d\n", int(mStats.frame));
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  UBO bindings: %d\n", int(mStats.uniformBufferBindings));
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "  Tex bindings: %d\n", int(mStats.samplerTextureBindings));
}

void Controller::Pause()
{
}

void Controller::Resume()
{
  // Ensure we re-draw at least once:
  mImpl->mDrawOnResume = true;
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

API::FramebufferFactory& Controller::GetFramebufferFactory() const
{
  return mImpl->GetFramebufferFactory();
}

API::PipelineFactory& Controller::GetPipelineFactory()
{
  mImpl->mPipelineFactory->Reset();
  return *mImpl->mPipelineFactory;
}

API::SamplerFactory& Controller::GetSamplerFactory()
{
  return mImpl->mSamplerFactory->Reset();
}

Vulkan::Graphics& Controller::GetGraphics() const
{
  return mImpl->mGraphics;
}

void Controller::ScheduleBufferMemoryTransfer( std::unique_ptr< VulkanAPI::BufferMemoryTransfer > transferRequest )
{
  mImpl->mBufferTransferRequests.emplace_back( std::move( transferRequest ) );
}

void Controller::ScheduleResourceTransfer( VulkanAPI::ResourceTransferRequest&& transferRequest )
{
  std::lock_guard<std::recursive_mutex> lock(mImpl->mResourceTransferMutex);
  mImpl->mResourceTransferRequests.emplace_back( std::move(transferRequest) );

  // if we requested immediate upload then request will be processed instantly with skipping
  // all the deferred update requests
  if( !mImpl->mResourceTransferRequests.back().deferredTransferMode )
  {
    mImpl->ProcessResourceTransferRequests( true );
  }
}

bool Controller::HasPendingResourceTransfers() const
{
  return !mImpl->mResourceTransferRequests.empty();
}

void Controller::SubmitCommands( std::vector< API::RenderCommand* > commands )
{
  mImpl->SubmitCommands( std::move( commands ) );
}

std::unique_ptr< API::RenderCommand > Controller::AllocateRenderCommand()
{
  return mImpl->AllocateRenderCommand();
}

bool Controller::EnableDepthStencilBuffer( bool enableDepth, bool enableStencil )
{
  return mImpl->EnableDepthStencilBuffer( enableDepth, enableStencil );
}

void Controller::RunGarbageCollector( size_t numberOfDiscardedRenderers )
{
  mImpl->RunGarbageCollector( numberOfDiscardedRenderers );
}

void Controller::DiscardUnusedResources()
{
  mImpl->DiscardUnusedResources();
}

bool Controller::IsDiscardQueueEmpty()
{
  return mImpl->IsDiscardQueueEmpty();
}

bool Controller::IsDrawOnResumeRequired()
{
  return mImpl->mDrawOnResume;
}

void Controller::WaitIdle()
{
  mImpl->mGraphics.GetGraphicsQueue(0u).GetVkHandle().waitIdle();
}

void Controller::SwapBuffers()
{
  mImpl->SwapBuffers();
}

uint32_t Controller::GetSwapchainBufferCount()
{
  return mImpl->GetSwapchainBufferCount();
}

uint32_t Controller::GetCurrentBufferIndex()
{
  return mImpl->mGraphics.GetCurrentBufferIndex();
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
