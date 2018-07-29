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
#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>
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
#include <dali/graphics/thread-pool.h>

#include <dali/integration-api/debug.h>

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
                      Vulkan::RefCountedFramebuffer _framebuffer,
                      std::vector< Dali::Graphics::API::RenderCommand* > _renderCommands )
            : beginInfo( _beginInfo ),
              colorValues( std::move( _clearColorValues ) ),
              framebuffer( std::move( _framebuffer ) ),
              renderCommands( std::move( _renderCommands ) )
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
    //fprintf(stderr,"%s:<<--\n",__FUNCTION__);

    auto surface = mGraphics.GetSurface( 0u );

    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );

    if ( mGraphics.IsSurfaceResized() )
    {
      swapchain->Invalidate();
    }

    // We won't run garbage collection in case there are pending resource transfers.
    //fprintf(stderr,"%s:call AcquireNextFramebuffer---\n",__FUNCTION__);
    swapchain->AcquireNextFramebuffer( !mOwner.HasPendingResourceTransfers() );

    if( !swapchain->IsValid() )
    {
      // make sure device doesn't do any work before replacing swapchain
      mGraphics.DeviceWaitIdle();

      // replace swapchain
     //fprintf(stderr,"%s:call ReplaceSwapchainForSurface---\n",__FUNCTION__);
      swapchain = mGraphics.ReplaceSwapchainForSurface( surface, std::move(swapchain) );

      // get new valid framebuffer
     //fprintf(stderr,"%s:call AcquireNextFramebuffer---\n",__FUNCTION__);
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
    //fprintf(stderr,"%s:<<--\n",__FUNCTION__);

    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );

    if( !mRenderPasses.empty() )
    {
      // Ensure there are enough command buffers for each render pass,
      swapchain->AllocateCommandBuffers( mRenderPasses.size() );
      std::vector<Vulkan::RefCountedCommandBuffer>& renderPassBuffers = swapchain->GetCommandBuffers();

      uint32_t index = 0;
      for( auto& renderPassData : mRenderPasses )
      {
        //fprintf(stderr,"%s:call ProcessRenderPassData---\n",__FUNCTION__);
        ProcessRenderPassData( renderPassBuffers[index], renderPassData );
        ++index;
      }
    }
    else
    {
      //fprintf(stderr,"%s:BeginRenderPass for empty Renderpass---\n",__FUNCTION__);
      auto primaryCommandBuffer = swapchain->GetLastCommandBuffer();
      primaryCommandBuffer->BeginRenderPass( vk::RenderPassBeginInfo{}
        .setFramebuffer( swapchain->GetCurrentFramebuffer()->GetVkHandle() )
        .setRenderPass(swapchain->GetCurrentFramebuffer()->GetRenderPass() )
        .setRenderArea( { {0, 0}, { swapchain->GetCurrentFramebuffer()->GetWidth(), swapchain->GetCurrentFramebuffer()->GetHeight() } } )
        .setPClearValues( swapchain->GetCurrentFramebuffer()->GetClearValues().data() )
        .setClearValueCount( uint32_t(swapchain->GetCurrentFramebuffer()->GetClearValues().size()) ), vk::SubpassContents::eSecondaryCommandBuffers );
      //fprintf(stderr,"%s:EndRenderPass for empty Renderpass---\n",__FUNCTION__);
      primaryCommandBuffer->EndRenderPass();
    }

    for( auto& future : mMemoryTransferFutures )
    {
      future->Wait();
      future.reset();
    }

    mMemoryTransferFutures.clear();

     //fprintf(stderr,"%s:call Present---\n",__FUNCTION__);
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

  void UpdateRenderPass( std::vector< Dali::Graphics::API::RenderCommand*  >&& commands )
  {
        //fprintf(stderr,"%s:<<--\n",__FUNCTION__);
    auto firstCommand = static_cast<VulkanAPI::RenderCommand*>(commands[0]);
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
              framebuffer,
              commands );
    }
    else
    {
      auto& vector = mRenderPasses.back().renderCommands;
      vector.insert( vector.end(), commands.begin(), commands.end() );
    }
  }

  /**
   * Submits number of commands in one go ( simiar to vkCmdExecuteCommands )
   * @param commands
   */
  void SubmitCommands( std::vector< Dali::Graphics::API::RenderCommand* > commands )
  {
       //fprintf(stderr,"%s:<<--\n",__FUNCTION__);
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

    // Begin render pass for render target
    // clear color obtained from very first command in the batch
    UpdateRenderPass( std::move( commands ) );
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
    //fprintf(stderr,"%s:ProcessResourceTransferRequests()~~~~~~\n",__FUNCTION__);
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
        else if ( req.requestType == TransferRequestType::USE_TBM )
        {
          //fprintf(stderr,"%s:update by vkImage of TBM!!!!\n",__FUNCTION__);
          image = req.useTBMInfo.srcImage;
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

        if ( image->IsExternal() )
        {
          //fprintf(stderr,"%s:call CreateImageMemoryBarrier  preLayout for TBM!!!!\n",__FUNCTION__);
          preLayoutBarriers.push_back( mGraphics.CreateImageMemoryBarrier( image, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral ) );
          //postLayoutBarriers.push_back( mGraphics.CreateImageMemoryBarrier( image, vk::ImageLayout::eUndefined, image->GetImageLayout() ) );
          image->SetImageLayout( vk::ImageLayout::eUndefined );
        }
        else
        {
           //fprintf(stderr,"%s:call CreateImageMemoryBarrier  preLayout\n",__FUNCTION__);
          preLayoutBarriers.push_back( mGraphics.CreateImageMemoryBarrier( image, image->GetImageLayout(), vk::ImageLayout::eTransferDstOptimal ) );
          //fprintf(stderr,"%s:call CreateImageMemoryBarrier  postLayout\n",__FUNCTION__);
          postLayoutBarriers.push_back( mGraphics.CreateImageMemoryBarrier( image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal ) );
          image->SetImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal );
        }
      }

      // Build command buffer for each image until reaching next sync point
      //fprintf(stderr,"%s:CreateCommandBuffer---\n",__FUNCTION__);
      auto commandBuffer = mGraphics.CreateCommandBuffer( true );

      // Fence between submissions
       //fprintf(stderr,"%s:CreateFence---\n",__FUNCTION__);
      auto fence = mGraphics.CreateFence( {} );

      /**
       * The loop iterates through requests for each unique image. It parallelizes
       * transfers to images until end of data in the batch.
       * After submitting copy commands the loop waits for the fence to be signalled
       * and repeats recording for the next batch of transfer requests.
       */
      for( auto i = 0u; i < highestBatchIndex; ++i )
      {
        //fprintf(stderr,"%s:Begin CommandBuffer---\n",__FUNCTION__);
        commandBuffer->Begin( vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr );

        // change image layouts only once
        if( i == 0 )
        {
          //fprintf(stderr,"%s:PipelineBarrier for preLayoutBarriers---\n",__FUNCTION__);
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
          //fprintf(stderr,"%s:PipelineBarrier for postLayoutBarriers---\n",__FUNCTION__);
          commandBuffer->PipelineBarrier( vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, postLayoutBarriers );
        }
        //fprintf(stderr,"%s:End CommandBuffer---\n",__FUNCTION__);
        commandBuffer->End();

        // submit to the queue
        auto transferQueue = mGraphics.GetTransferQueue( 0u );
        //fprintf(stderr,"%s:Submit---\n",__FUNCTION__);
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
     //fprintf(stderr,"%s:<<--\n",__FUNCTION__);
    auto swapchain = mGraphics.GetSwapchainForFBID( 0u );
    swapchain->Invalidate();
  }

  void ProcessRenderPassData( Vulkan::RefCountedCommandBuffer commandBuffer, const RenderPassData& renderPassData )
  {
    //fprintf(stderr,"%s:<<--\n",__FUNCTION__);
    //fprintf(stderr,"%s:BegineRenderPass---\n",__FUNCTION__);
    commandBuffer->BeginRenderPass( renderPassData.beginInfo, vk::SubpassContents::eInline );

    // update descriptor sets
    for( auto&& command : renderPassData.renderCommands )
    {
      auto apiCommand = static_cast<VulkanAPI::RenderCommand*>(command);
      apiCommand->PrepareResources();
    }

    if( mDescriptorWrites.size() )
    {
      //fprintf(stderr,"%s: call updateDescriptorSets\n",__FUNCTION__);
      for(uint32_t i=0; i<uint32_t( mDescriptorWrites.size() ); i++)
      {
        vk::WriteDescriptorSet descriptorWrite = mDescriptorWrites[i];
        //fprintf(stderr,"descriptorWrite.dstBinding :%d\n",descriptorWrite.dstBinding);
        ////fprintf(stderr,"descriptorWrite.pImageInfo :%p\n",descriptorWrite.pImageInfo);
        ////fprintf(stderr,"descriptorWrite.pBufferInfo :%p\n",descriptorWrite.pBufferInfo);

        if (descriptorWrite.pImageInfo)
        {
            //fprintf(stderr,"pImageInfo.sampler :%llud\n",static_cast<VkSampler>(descriptorWrite.pImageInfo->sampler));
            //fprintf(stderr,"pImageInfo.imageView :%llud\n",static_cast<VkImage>(descriptorWrite.pImageInfo->imageView));
            //fprintf(stderr,"pImageInfo.imageLayout :%d\n",static_cast<VkImageLayout>(descriptorWrite.pImageInfo->imageLayout));
        }
        else
        {
            //fprintf(stderr,"descriptorWrite.pImageInfo is NULL\n");
        }

      //   descriptorWrite.dstBinding
      //   descriptorWrite.pImageInfo
      //   descriptorWrite.pBufferInfo


      //   descriptorWrite.pImageInfo.sampler
      // descriptorWrite.pImageInfo.imageView
      // descriptorWrite.pImageInfo.imageLayout

      }
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
      //fprintf(stderr,"%s:GetVulkanPipeline---\n",__FUNCTION__);
      if( !apiCommand->GetVulkanPipeline() )
      {
        continue;
      }

      //fprintf(stderr,"%s:BindPipeline---\n",__FUNCTION__);
      apiCommand->BindPipeline( commandBuffer );

      //@todo add assert to check the pipeline render pass nad the inherited render pass are the same

      // set dynamic state
      if( apiCommand->mDrawCommand.scissorTestEnable )
      {
        vk::Rect2D scissorRect( { apiCommand->mDrawCommand.scissor.x,
                                  apiCommand->mDrawCommand.scissor.y },
                                { apiCommand->mDrawCommand.scissor.width,
                                  apiCommand->mDrawCommand.scissor.height } );

        //fprintf(stderr,"%s:SetScissor---\n",__FUNCTION__);
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

         //fprintf(stderr,"%s:SetViewport---\n",__FUNCTION__);
        commandBuffer->SetViewport( 0, 1, &viewport );
      }

      // bind vertex buffers
      auto binding = 0u;
      //fprintf(stderr,"%s:GetVertexBufferBindings---\n",__FUNCTION__);
      for( auto&& vb : apiCommand->GetVertexBufferBindings() )
      {
        //fprintf(stderr,"%s:BindVertexBuffer---\n",__FUNCTION__);
        commandBuffer->BindVertexBuffer( binding++, static_cast<const VulkanAPI::Buffer&>( *vb ).GetBufferRef(), 0 );
      }

      // note: starting set = 0
       //fprintf(stderr,"%s:GetDescriptorSets---\n",__FUNCTION__);
      const auto& descriptorSets = apiCommand->GetDescriptorSets();
      //fprintf(stderr,"%s:BindDescriptorSets---\n",__FUNCTION__);
      commandBuffer->BindDescriptorSets( descriptorSets, vulkanApiPipeline->GetVkPipelineLayout(), 0, uint32_t( descriptorSets.size() ) );

      // draw
      //fprintf(stderr,"%s:GetDrawCommand---\n",__FUNCTION__);
      const auto& drawCommand = apiCommand->GetDrawCommand();

      const auto& indexBinding = apiCommand->GetIndexBufferBinding();
      if( indexBinding.buffer )
      {
        //fprintf(stderr,"%s:BindIndexBuffer---\n",__FUNCTION__);
        commandBuffer->BindIndexBuffer( static_cast<const VulkanAPI::Buffer&>(*indexBinding.buffer).GetBufferRef(),
                                 0, vk::IndexType::eUint16 );
       //fprintf(stderr,"%s:DrawIndexed---\n",__FUNCTION__);
        commandBuffer->DrawIndexed( drawCommand.indicesCount,
                             drawCommand.instanceCount,
                             drawCommand.firstIndex,
                             0,
                             drawCommand.firstInstance );
      }
      else
      {
       //fprintf(stderr,"%s:Draw---\n",__FUNCTION__);
        commandBuffer->Draw( drawCommand.vertexCount,
                      drawCommand.instanceCount,
                      drawCommand.firstVertex,
                      drawCommand.firstInstance );
      }
    }

   //fprintf(stderr,"%s:EndRenderPass---\n",__FUNCTION__);
    commandBuffer->EndRenderPass();
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

  std::unique_ptr< PipelineCache > mDefaultPipelineCache;

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
  //fprintf(stderr,"%s:[[------- Controller::BeginFrame(), thread %lud\n",__FUNCTION__ ,pthread_self());
  mStats.samplerTextureBindings = 0;
  mStats.uniformBufferBindings = 0;

  //@ todo, not multithreaded yet
  mImpl->mDescriptorWrites.clear();

  mImpl->BeginFrame();
}

void Controller::PushDescriptorWrite( const vk::WriteDescriptorSet& write )
{
    //fprintf(stderr,"%s:<<--\n",__FUNCTION__);
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

void Controller::EndFrame()
{

  mImpl->EndFrame();

#if(DEBUG_ENABLED)
  // print stats
  PrintStats();
#endif

     //fprintf(stderr,"%s:<<--\n",__FUNCTION__);
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

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
