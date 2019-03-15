/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/render/common/render-manager.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/core.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/common/render-algorithms.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/internal/render/renderers/render-texture-frame-buffer.h>
#include <dali/internal/render/renderers/render-surface-frame-buffer.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/shaders/program-controller.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Structure to contain internal data
 */
struct RenderManager::Impl
{
  Impl( Integration::GlAbstraction& glAbstraction,
        Integration::GlSyncAbstraction& glSyncAbstraction,
        Integration::DepthBufferAvailable depthBufferAvailableParam,
        Integration::StencilBufferAvailable stencilBufferAvailableParam )
  : context( glAbstraction ),
    currentContext( &context ),
    glAbstraction( glAbstraction ),
    glSyncAbstraction( glSyncAbstraction ),
    renderQueue(),
    instructions(),
    renderAlgorithms(),
    backgroundColor( Dali::Stage::DEFAULT_BACKGROUND_COLOR ),
    frameCount( 0u ),
    renderBufferIndex( SceneGraphBuffers::INITIAL_UPDATE_BUFFER_INDEX ),
    defaultSurfaceRect(),
    rendererContainer(),
    samplerContainer(),
    textureContainer(),
    frameBufferContainer(),
    lastFrameWasRendered( false ),
    programController( glAbstraction ),
    depthBufferAvailable( depthBufferAvailableParam ),
    stencilBufferAvailable( stencilBufferAvailableParam )
  {
  }

  ~Impl()
  {
  }

  void AddRenderTracker( Render::RenderTracker* renderTracker )
  {
    DALI_ASSERT_DEBUG( renderTracker != NULL );
    mRenderTrackers.PushBack( renderTracker );
  }

  void RemoveRenderTracker( Render::RenderTracker* renderTracker )
  {
    mRenderTrackers.EraseObject( renderTracker );
  }

  Context* CreateSurfaceContext()
  {
    surfaceContextContainer.PushBack( new Context( glAbstraction ) );
    return surfaceContextContainer[ surfaceContextContainer.Count() - 1 ];
  }

  void DestroySurfaceContext( Context* surfaceContext )
  {
    surfaceContextContainer.EraseObject( surfaceContext );
  }

  void UpdateTrackers()
  {
    for( auto&& iter : mRenderTrackers )
    {
      iter->PollSyncObject();
    }
  }

  // the order is important for destruction,
  // programs are owned by context at the moment.
  Context                                   context;                 ///< Holds the GL state of the share resource context
  Context*                                  currentContext;          ///< Holds the GL state of the current context for rendering
  OwnerContainer< Context* >                surfaceContextContainer; ///< List of owned contexts holding the GL state per surface
  Integration::GlAbstraction&               glAbstraction;           ///< GL abstraction
  Integration::GlSyncAbstraction&           glSyncAbstraction;       ///< GL sync abstraction
  RenderQueue                               renderQueue;             ///< A message queue for receiving messages from the update-thread.

  // Render instructions describe what should be rendered during RenderManager::Render()
  // Owned by RenderManager. Update manager updates instructions for the next frame while we render the current one
  RenderInstructionContainer                instructions;
  Render::RenderAlgorithms                  renderAlgorithms;        ///< The RenderAlgorithms object is used to action the renders required by a RenderInstruction

  Vector4                                   backgroundColor;         ///< The glClear color used at the beginning of each frame.

  uint32_t                                  frameCount;              ///< The current frame count
  BufferIndex                               renderBufferIndex;       ///< The index of the buffer to read from; this is opposite of the "update" buffer

  Rect<int32_t>                             defaultSurfaceRect;      ///< Rectangle for the default surface we are rendering to

  OwnerContainer< Render::Renderer* >       rendererContainer;       ///< List of owned renderers
  OwnerContainer< Render::Sampler* >        samplerContainer;        ///< List of owned samplers
  OwnerContainer< Render::Texture* >        textureContainer;        ///< List of owned textures
  OwnerContainer< Render::FrameBuffer* >    frameBufferContainer;    ///< List of owned framebuffers
  OwnerContainer< Render::PropertyBuffer* > propertyBufferContainer; ///< List of owned property buffers
  OwnerContainer< Render::Geometry* >       geometryContainer;       ///< List of owned Geometries

  bool                                      lastFrameWasRendered;    ///< Keeps track of the last frame being rendered due to having render instructions

  OwnerContainer< Render::RenderTracker* >  mRenderTrackers;         ///< List of render trackers

  ProgramController                         programController;        ///< Owner of the GL programs

  Integration::DepthBufferAvailable         depthBufferAvailable;     ///< Whether the depth buffer is available
  Integration::StencilBufferAvailable       stencilBufferAvailable;   ///< Whether the stencil buffer is available

};

RenderManager* RenderManager::New( Integration::GlAbstraction& glAbstraction,
                                   Integration::GlSyncAbstraction& glSyncAbstraction,
                                   Integration::DepthBufferAvailable depthBufferAvailable,
                                   Integration::StencilBufferAvailable stencilBufferAvailable )
{
  RenderManager* manager = new RenderManager;
  manager->mImpl = new Impl( glAbstraction,
                             glSyncAbstraction,
                             depthBufferAvailable,
                             stencilBufferAvailable );
  return manager;
}

RenderManager::RenderManager()
: mImpl(NULL)
{
}

RenderManager::~RenderManager()
{
  delete mImpl;
}

RenderQueue& RenderManager::GetRenderQueue()
{
  return mImpl->renderQueue;
}

void RenderManager::ContextCreated()
{
  mImpl->context.GlContextCreated();
  mImpl->programController.GlContextCreated();

  // renderers, textures and gpu buffers cannot reinitialize themselves
  // so they rely on someone reloading the data for them
}

void RenderManager::ContextDestroyed()
{
  mImpl->context.GlContextDestroyed();
  mImpl->programController.GlContextDestroyed();

  //Inform textures
  for( auto&& texture : mImpl->textureContainer )
  {
    texture->GlContextDestroyed();
  }

  //Inform framebuffers
  for( auto&& framebuffer : mImpl->frameBufferContainer )
  {
    framebuffer->GlContextDestroyed();
  }

  // inform renderers
  for( auto&& renderer : mImpl->rendererContainer )
  {
    renderer->GlContextDestroyed();
  }
}

void RenderManager::SetShaderSaver( ShaderSaver& upstream )
{
  mImpl->programController.SetShaderSaver( upstream );
}

RenderInstructionContainer& RenderManager::GetRenderInstructionContainer()
{
  return mImpl->instructions;
}

void RenderManager::SetBackgroundColor( const Vector4& color )
{
  mImpl->backgroundColor = color;
}

void RenderManager::SetDefaultSurfaceRect(const Rect<int32_t>& rect)
{
  mImpl->defaultSurfaceRect = rect;
}

void RenderManager::AddRenderer( OwnerPointer< Render::Renderer >& renderer )
{
  // Initialize the renderer as we are now in render thread
  renderer->Initialize( mImpl->context );

  mImpl->rendererContainer.PushBack( renderer.Release() );
}

void RenderManager::RemoveRenderer( Render::Renderer* renderer )
{
  mImpl->rendererContainer.EraseObject( renderer );
}

void RenderManager::AddSampler( OwnerPointer< Render::Sampler >& sampler )
{
  mImpl->samplerContainer.PushBack( sampler.Release() );
}

void RenderManager::RemoveSampler( Render::Sampler* sampler )
{
  mImpl->samplerContainer.EraseObject( sampler );
}

void RenderManager::AddTexture( OwnerPointer< Render::Texture >& texture )
{
  texture->Initialize( mImpl->context );
  mImpl->textureContainer.PushBack( texture.Release() );
}

void RenderManager::RemoveTexture( Render::Texture* texture )
{
  DALI_ASSERT_DEBUG( NULL != texture );

  // Find the texture, use reference to pointer so we can do the erase safely
  for ( auto&& iter : mImpl->textureContainer )
  {
    if ( iter == texture )
    {
      texture->Destroy( mImpl->context );
      mImpl->textureContainer.Erase( &iter ); // Texture found; now destroy it
      return;
    }
  }
}

void RenderManager::UploadTexture( Render::Texture* texture, PixelDataPtr pixelData, const Texture::UploadParams& params )
{
  texture->Upload( mImpl->context, pixelData, params );
}

void RenderManager::GenerateMipmaps( Render::Texture* texture )
{
  texture->GenerateMipmaps( mImpl->context );
}

void RenderManager::SetFilterMode( Render::Sampler* sampler, uint32_t minFilterMode, uint32_t magFilterMode )
{
  sampler->mMinificationFilter = static_cast<Dali::FilterMode::Type>(minFilterMode);
  sampler->mMagnificationFilter = static_cast<Dali::FilterMode::Type>(magFilterMode );
}

void RenderManager::SetWrapMode( Render::Sampler* sampler, uint32_t rWrapMode, uint32_t sWrapMode, uint32_t tWrapMode )
{
  sampler->mRWrapMode = static_cast<Dali::WrapMode::Type>(rWrapMode);
  sampler->mSWrapMode = static_cast<Dali::WrapMode::Type>(sWrapMode);
  sampler->mTWrapMode = static_cast<Dali::WrapMode::Type>(tWrapMode);
}

void RenderManager::AddFrameBuffer( Render::FrameBuffer* frameBuffer )
{
  mImpl->frameBufferContainer.PushBack( frameBuffer );
  if ( frameBuffer->IsSurfaceBacked() )
  {
    frameBuffer->Initialize( *mImpl->CreateSurfaceContext() );
  }
  else
  {
    frameBuffer->Initialize( mImpl->context );
  }
}

void RenderManager::RemoveFrameBuffer( Render::FrameBuffer* frameBuffer )
{
  DALI_ASSERT_DEBUG( NULL != frameBuffer );

  // Find the sampler, use reference so we can safely do the erase
  for ( auto&& iter : mImpl->frameBufferContainer )
  {
    if ( iter == frameBuffer )
    {
      frameBuffer->Destroy( mImpl->context );

      if ( frameBuffer->IsSurfaceBacked() )
      {
        auto surfaceFrameBuffer = static_cast<Render::SurfaceFrameBuffer*>( frameBuffer );
        mImpl->DestroySurfaceContext( surfaceFrameBuffer->GetContext() );
      }

      mImpl->frameBufferContainer.Erase( &iter ); // frameBuffer found; now destroy it

      break;
    }
  }
}

void RenderManager::AttachColorTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer )
{
  if ( !frameBuffer->IsSurfaceBacked() )
  {
    auto textureFrameBuffer = static_cast<Render::TextureFrameBuffer*>( frameBuffer );
    textureFrameBuffer->AttachColorTexture( mImpl->context, texture, mipmapLevel, layer );
  }
}

void RenderManager::AddPropertyBuffer( OwnerPointer< Render::PropertyBuffer >& propertyBuffer )
{
  mImpl->propertyBufferContainer.PushBack( propertyBuffer.Release() );
}

void RenderManager::RemovePropertyBuffer( Render::PropertyBuffer* propertyBuffer )
{
  mImpl->propertyBufferContainer.EraseObject( propertyBuffer );
}

void RenderManager::SetPropertyBufferFormat( Render::PropertyBuffer* propertyBuffer, OwnerPointer< Render::PropertyBuffer::Format>& format )
{
  propertyBuffer->SetFormat( format.Release() );
}

void RenderManager::SetPropertyBufferData( Render::PropertyBuffer* propertyBuffer, OwnerPointer< Vector<uint8_t> >& data, uint32_t size )
{
  propertyBuffer->SetData( data.Release(), size );
}

void RenderManager::SetIndexBuffer( Render::Geometry* geometry, Dali::Vector<uint16_t>& indices )
{
  geometry->SetIndexBuffer( indices );
}

void RenderManager::AddGeometry( OwnerPointer< Render::Geometry >& geometry )
{
  mImpl->geometryContainer.PushBack( geometry.Release() );
}

void RenderManager::RemoveGeometry( Render::Geometry* geometry )
{
  mImpl->geometryContainer.EraseObject( geometry );
}

void RenderManager::AttachVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer )
{
  DALI_ASSERT_DEBUG( NULL != geometry );

  // Find the geometry
  for ( auto&& iter : mImpl->geometryContainer )
  {
    if ( iter == geometry )
    {
      iter->AddPropertyBuffer( propertyBuffer );
      break;
    }
  }
}

void RenderManager::RemoveVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer )
{
  DALI_ASSERT_DEBUG( NULL != geometry );

  // Find the geometry
  for ( auto&& iter : mImpl->geometryContainer )
  {
    if ( iter == geometry )
    {
      iter->RemovePropertyBuffer( propertyBuffer );
      break;
    }
  }
}

void RenderManager::SetGeometryType( Render::Geometry* geometry, uint32_t geometryType )
{
  geometry->SetType( Render::Geometry::Type(geometryType) );
}

void RenderManager::AddRenderTracker( Render::RenderTracker* renderTracker )
{
  mImpl->AddRenderTracker(renderTracker);
}

void RenderManager::RemoveRenderTracker( Render::RenderTracker* renderTracker )
{
  mImpl->RemoveRenderTracker(renderTracker);
}

ProgramCache* RenderManager::GetProgramCache()
{
  return &(mImpl->programController);
}

void RenderManager::Render( Integration::RenderStatus& status, bool forceClear )
{
  DALI_PRINT_RENDER_START( mImpl->renderBufferIndex );

  // Core::Render documents that GL context must be current before calling Render
  DALI_ASSERT_DEBUG( mImpl->context.IsGlContextCreated() );

  // Increment the frame count at the beginning of each frame
  ++mImpl->frameCount;

  // Process messages queued during previous update
  mImpl->renderQueue.ProcessMessages( mImpl->renderBufferIndex );

  const uint32_t count = mImpl->instructions.Count( mImpl->renderBufferIndex );
  const bool haveInstructions = count > 0u;

  // Only render if we have instructions to render, or the last frame was rendered (and therefore a clear is required).
  if( haveInstructions || mImpl->lastFrameWasRendered || forceClear )
  {
    // Mark that we will require a post-render step to be performed (includes swap-buffers).
    status.SetNeedsPostRender( true );

    // Switch to the shared context
    if ( mImpl->currentContext != &mImpl->context )
    {
      mImpl->currentContext = &mImpl->context;
      // Clear the current cached program when the context is switched
      mImpl->programController.ClearCurrentProgram();
    }


    // Upload the geometries
    for( uint32_t i = 0; i < count; ++i )
    {
      RenderInstruction& instruction = mImpl->instructions.At( mImpl->renderBufferIndex, i );

      const Matrix* viewMatrix       = instruction.GetViewMatrix( mImpl->renderBufferIndex );
      const Matrix* projectionMatrix = instruction.GetProjectionMatrix( mImpl->renderBufferIndex );

      DALI_ASSERT_DEBUG( viewMatrix );
      DALI_ASSERT_DEBUG( projectionMatrix );

      if( viewMatrix && projectionMatrix )
      {
        const RenderListContainer::SizeType renderListCount = instruction.RenderListCount();

        // Iterate through each render list.
        for( RenderListContainer::SizeType index = 0; index < renderListCount; ++index )
        {
          const RenderList* renderList = instruction.GetRenderList( index );

          if( renderList && !renderList->IsEmpty() )
          {
            const std::size_t itemCount = renderList->Count();
            for( uint32_t itemIndex = 0u; itemIndex < itemCount; ++itemIndex )
            {
              const RenderItem& item = renderList->GetItem( itemIndex );
              if( DALI_LIKELY( item.mRenderer ) )
              {
                item.mRenderer->Upload( *mImpl->currentContext );
              }
            }
          }
        }
      }
    }

    for( uint32_t i = 0; i < count; ++i )
    {
      RenderInstruction& instruction = mImpl->instructions.At( mImpl->renderBufferIndex, i );

      DoRender( instruction );
    }

    GLenum attachments[] = { GL_DEPTH, GL_STENCIL };
    mImpl->context.InvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
    for ( auto&& context : mImpl->surfaceContextContainer )
    {
      context->InvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
    }

    //Notify RenderGeometries that rendering has finished
    for ( auto&& iter : mImpl->geometryContainer )
    {
      iter->OnRenderFinished();
    }
  }

  mImpl->UpdateTrackers();

  // If this frame was rendered due to instructions existing, we mark this so we know to clear the next frame.
  mImpl->lastFrameWasRendered = haveInstructions;

  /**
   * The rendering has finished; swap to the next buffer.
   * Ideally the update has just finished using this buffer; otherwise the render thread
   * should block until the update has finished.
   */
  mImpl->renderBufferIndex = (0 != mImpl->renderBufferIndex) ? 0 : 1;

  DALI_PRINT_RENDER_END();
}

void RenderManager::DoRender( RenderInstruction& instruction )
{
  Rect<int32_t> viewportRect;
  Vector4   clearColor;

  if ( instruction.mIsClearColorSet )
  {
    clearColor = instruction.mClearColor;
  }
  else
  {
    clearColor = Dali::RenderTask::DEFAULT_CLEAR_COLOR;
  }

  Rect<int32_t> surfaceRect = mImpl->defaultSurfaceRect;
  Vector4 backgroundColor = mImpl->backgroundColor;
  Integration::DepthBufferAvailable depthBufferAvailable = mImpl->depthBufferAvailable;
  Integration::StencilBufferAvailable stencilBufferAvailable = mImpl->stencilBufferAvailable;

  Render::SurfaceFrameBuffer* surfaceFrameBuffer = nullptr;
  if ( ( instruction.mFrameBuffer != 0 ) && instruction.mFrameBuffer->IsSurfaceBacked() )
  {
    surfaceFrameBuffer = static_cast<Render::SurfaceFrameBuffer*>( instruction.mFrameBuffer );

#if DALI_GLES_VERSION >= 30
    Context* surfaceContext = surfaceFrameBuffer->GetContext();
    if ( mImpl->currentContext != surfaceContext )
    {
      // Switch the correct context if rendering to a surface
      mImpl->currentContext = surfaceContext;
      // Clear the current cached program when the context is switched
      mImpl->programController.ClearCurrentProgram();
    }
#endif

    surfaceRect = Rect<int32_t>( 0, 0, static_cast<int32_t>( surfaceFrameBuffer->GetWidth() ), static_cast<int32_t>( surfaceFrameBuffer->GetHeight() ) );
    backgroundColor = surfaceFrameBuffer->GetBackgroundColor();
  }

  DALI_ASSERT_DEBUG( mImpl->currentContext->IsGlContextCreated() );

  // reset the program matrices for all programs once per frame
  // this ensures we will set view and projection matrix once per program per camera
  mImpl->programController.ResetProgramMatrices();

  if( instruction.mFrameBuffer )
  {
    instruction.mFrameBuffer->Bind( *mImpl->currentContext );
  }

  mImpl->currentContext->Viewport( surfaceRect.x,
                            surfaceRect.y,
                            surfaceRect.width,
                            surfaceRect.height );

  mImpl->currentContext->ClearColor( backgroundColor.r,
                              backgroundColor.g,
                              backgroundColor.b,
                              backgroundColor.a );

  // Clear the entire color, depth and stencil buffers for the default framebuffer, if required.
  // It is important to clear all 3 buffers when they are being used, for performance on deferred renderers
  // e.g. previously when the depth & stencil buffers were NOT cleared, it caused the DDK to exceed a "vertex count limit",
  // and then stall. That problem is only noticeable when rendering a large number of vertices per frame.

  mImpl->currentContext->SetScissorTest( false );

  GLbitfield clearMask = GL_COLOR_BUFFER_BIT;

  mImpl->currentContext->ColorMask( true );

  if( depthBufferAvailable == Integration::DepthBufferAvailable::TRUE )
  {
    mImpl->currentContext->DepthMask( true );
    clearMask |= GL_DEPTH_BUFFER_BIT;
  }

  if( stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE)
  {
    mImpl->currentContext->ClearStencil( 0 );
    mImpl->currentContext->StencilMask( 0xFF ); // 8 bit stencil mask, all 1's
    clearMask |= GL_STENCIL_BUFFER_BIT;
  }

  mImpl->currentContext->Clear( clearMask, Context::FORCE_CLEAR );

  if( !instruction.mIgnoreRenderToFbo && ( instruction.mFrameBuffer != 0 ) )
  {
    if ( instruction.mFrameBuffer->IsSurfaceBacked() ) // Surface rendering
    {
      if ( instruction.mIsViewportSet )
      {
        // For glViewport the lower-left corner is (0,0)
        // For glViewport the lower-left corner is (0,0)
        const int32_t y = ( surfaceRect.height - instruction.mViewport.height ) - instruction.mViewport.y;
        viewportRect.Set( instruction.mViewport.x,  y, instruction.mViewport.width, instruction.mViewport.height );
      }
      else
      {
        viewportRect = surfaceRect;
      }
    }
    else // Offscreen buffer rendering
    {
      if ( instruction.mIsViewportSet )
      {
        // For glViewport the lower-left corner is (0,0)
        const int32_t y = ( instruction.mFrameBuffer->GetHeight() - instruction.mViewport.height ) - instruction.mViewport.y;
        viewportRect.Set( instruction.mViewport.x,  y, instruction.mViewport.width, instruction.mViewport.height );
      }
      else
      {
        viewportRect.Set( 0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight() );
      }
    }
  }
  else // No Offscreen frame buffer rendering
  {
    // Check whether a viewport is specified, otherwise the full surface size is used
    if ( instruction.mFrameBuffer != 0 )
    {
      if ( instruction.mIsViewportSet )
      {
        // For glViewport the lower-left corner is (0,0)
        const int32_t y = ( instruction.mFrameBuffer->GetHeight() - instruction.mViewport.height ) - instruction.mViewport.y;
        viewportRect.Set( instruction.mViewport.x,  y, instruction.mViewport.width, instruction.mViewport.height );
      }
      else
      {
        viewportRect.Set( 0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight() );
      }
    }
    else
    {
      viewportRect = surfaceRect;
    }
  }

  mImpl->currentContext->Viewport(viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height);

  if ( instruction.mIsClearColorSet )
  {
    mImpl->currentContext->ClearColor( clearColor.r,
                                       clearColor.g,
                                       clearColor.b,
                                       clearColor.a );

    // Clear the viewport area only
    mImpl->currentContext->SetScissorTest( true );
    mImpl->currentContext->Scissor( viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height );
    mImpl->currentContext->ColorMask( true );
    mImpl->currentContext->Clear( GL_COLOR_BUFFER_BIT , Context::CHECK_CACHED_VALUES );
    mImpl->currentContext->SetScissorTest( false );
  }

  mImpl->renderAlgorithms.ProcessRenderInstruction(
      instruction,
      *mImpl->currentContext,
      mImpl->renderBufferIndex,
      depthBufferAvailable,
      stencilBufferAvailable );

  if( instruction.mRenderTracker && ( instruction.mFrameBuffer != 0 ) )
  {
    // This will create a sync object every frame this render tracker
    // is alive (though it should be now be created only for
    // render-once render tasks)
    instruction.mRenderTracker->CreateSyncObject( mImpl->glSyncAbstraction );
    instruction.mRenderTracker = NULL; // Only create once.
  }

  if ( surfaceFrameBuffer )
  {
    surfaceFrameBuffer->PostRender();
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
