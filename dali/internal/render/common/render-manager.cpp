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

// EXTERNAL INCLUDES
#include <memory.h>

// INTERNAL INCLUDES
#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/devel-api/threading/thread-pool.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/gl-context-helper-abstraction.h>
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

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_MANAGER" );
} // unnamed namespace
#endif

namespace
{
const float partialUpdateRatio = 0.8f; // If the partial update area exceeds 80%, change to full update.

/**
 * @brief Find the intersection of two AABB rectangles.
 * This is a logical AND operation. IE. The intersection is the area overlapped by both rectangles.
 * @param[in]     aabbA                  Rectangle A
 * @param[in]     aabbB                  Rectangle B
 * @return                               The intersection of rectangle A & B (result is a rectangle)
 */
inline ClippingBox IntersectAABB( const ClippingBox& aabbA, const ClippingBox& aabbB )
{
  ClippingBox intersectionBox;

  // First calculate the largest starting positions in X and Y.
  intersectionBox.x = std::max( aabbA.x, aabbB.x );
  intersectionBox.y = std::max( aabbA.y, aabbB.y );

  // Now calculate the smallest ending positions, and take the largest starting
  // positions from the result, to get the width and height respectively.
  // If the two boxes do not intersect at all, then we need a 0 width and height clipping area.
  // We use max here to clamp both width and height to >= 0 for this use-case.
  intersectionBox.width =  std::max( std::min( aabbA.x + aabbA.width,  aabbB.x + aabbB.width  ) - intersectionBox.x, 0 );
  intersectionBox.height = std::max( std::min( aabbA.y + aabbA.height, aabbB.y + aabbB.height ) - intersectionBox.y, 0 );

  return intersectionBox;
}

}

/**
 * Structure to contain internal data
 */
struct RenderManager::Impl
{
  Impl( Integration::GlAbstraction& glAbstraction,
        Integration::GlSyncAbstraction& glSyncAbstraction,
        Integration::GlContextHelperAbstraction& glContextHelperAbstraction,
        Integration::DepthBufferAvailable depthBufferAvailableParam,
        Integration::StencilBufferAvailable stencilBufferAvailableParam,
        Integration::PartialUpdateAvailable partialUpdateAvailableParam )
  : context( glAbstraction, &surfaceContextContainer ),
    currentContext( &context ),
    glAbstraction( glAbstraction ),
    glSyncAbstraction( glSyncAbstraction ),
    glContextHelperAbstraction( glContextHelperAbstraction ),
    renderQueue(),
    instructions(),
    renderAlgorithms(),
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
    stencilBufferAvailable( stencilBufferAvailableParam ),
    partialUpdateAvailable( partialUpdateAvailableParam ),
    defaultSurfaceOrientation( 0 )
  {
     // Create thread pool with just one thread ( there may be a need to create more threads in the future ).
    threadPool = std::unique_ptr<Dali::ThreadPool>( new Dali::ThreadPool() );
    threadPool->Initialize( 1u );
  }

  ~Impl()
  {
    threadPool.reset( nullptr ); // reset now to maintain correct destruction order
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
  Integration::GlContextHelperAbstraction&  glContextHelperAbstraction; ///< GL context helper abstraction
  RenderQueue                               renderQueue;             ///< A message queue for receiving messages from the update-thread.

  // Render instructions describe what should be rendered during RenderManager::Render()
  // Owned by RenderManager. Update manager updates instructions for the next frame while we render the current one
  RenderInstructionContainer                instructions;
  Render::RenderAlgorithms                  renderAlgorithms;        ///< The RenderAlgorithms object is used to action the renders required by a RenderInstruction

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
  Integration::PartialUpdateAvailable       partialUpdateAvailable;   ///< Whether the partial update is available

  std::unique_ptr<Dali::ThreadPool>         threadPool;               ///< The thread pool
  Vector<GLuint>                            boundTextures;            ///< The textures bound for rendering
  Vector<GLuint>                            textureDependencyList;    ///< The dependency list of binded textures
  int                                       defaultSurfaceOrientation; ///< defaultSurfaceOrientation for the default surface we are rendering to

};

RenderManager* RenderManager::New( Integration::GlAbstraction& glAbstraction,
                                   Integration::GlSyncAbstraction& glSyncAbstraction,
                                   Integration::GlContextHelperAbstraction& glContextHelperAbstraction,
                                   Integration::DepthBufferAvailable depthBufferAvailable,
                                   Integration::StencilBufferAvailable stencilBufferAvailable,
                                   Integration::PartialUpdateAvailable partialUpdateAvailable )
{
  RenderManager* manager = new RenderManager;
  manager->mImpl = new Impl( glAbstraction,
                             glSyncAbstraction,
                             glContextHelperAbstraction,
                             depthBufferAvailable,
                             stencilBufferAvailable,
                             partialUpdateAvailable );
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

void RenderManager::SetDefaultSurfaceRect(const Rect<int32_t>& rect)
{
  mImpl->defaultSurfaceRect = rect;
}

void RenderManager::SetDefaultSurfaceOrientation( int orientation )
{
  mImpl->defaultSurfaceOrientation = orientation;
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

void RenderManager::AddFrameBuffer( OwnerPointer< Render::FrameBuffer >& frameBuffer )
{
  Render::FrameBuffer* frameBufferPtr = frameBuffer.Release();
  mImpl->frameBufferContainer.PushBack( frameBufferPtr );
  if ( frameBufferPtr->IsSurfaceBacked() )
  {
    frameBufferPtr->Initialize( *mImpl->CreateSurfaceContext() );
  }
  else
  {
    frameBufferPtr->Initialize( mImpl->context );
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

void RenderManager::Render( Integration::RenderStatus& status, bool forceClear, bool uploadOnly )
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

  DALI_LOG_INFO( gLogFilter, Debug::General,
                 "Render: haveInstructions(%s) || mImpl->lastFrameWasRendered(%s) || forceClear(%s)\n",
                 haveInstructions ? "true" : "false",
                 mImpl->lastFrameWasRendered ? "true" : "false",
                 forceClear ? "true" : "false" );

  // Only render if we have instructions to render, or the last frame was rendered (and therefore a clear is required).
  if( haveInstructions || mImpl->lastFrameWasRendered || forceClear )
  {
    DALI_LOG_INFO( gLogFilter, Debug::General, "Render: Processing\n" );

    if ( !uploadOnly )
    {
      // Mark that we will require a post-render step to be performed (includes swap-buffers).
      status.SetNeedsPostRender( true );
    }

    // Switch to the shared context
    if ( mImpl->currentContext != &mImpl->context )
    {
      mImpl->currentContext = &mImpl->context;

      if ( mImpl->currentContext->IsSurfacelessContextSupported() )
      {
        mImpl->glContextHelperAbstraction.MakeSurfacelessContextCurrent();
      }

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

    if ( !uploadOnly )
    {
      for( uint32_t i = 0; i < count; ++i )
      {
        RenderInstruction& instruction = mImpl->instructions.At( mImpl->renderBufferIndex, i );

        DoRender( instruction );
      }
    }

    //Notify RenderGeometries that rendering has finished
    for ( auto&& iter : mImpl->geometryContainer )
    {
      iter->OnRenderFinished();
    }
  }
  else
  {
    DALI_LOG_RELEASE_INFO( "RenderManager::Render: Skip rendering [%d, %d, %d]\n", haveInstructions, mImpl->lastFrameWasRendered, forceClear );
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

bool GetDamagedRect( Rect<int32_t> &viewportRect, RenderInstruction& instruction, Rect<int32_t> &damagedRect )
{
  // merge bounding
  int dx1 = viewportRect.width, dx2 = 0, dy1 = viewportRect.height, dy2 = 0;
  int checkWidth = static_cast<int>( static_cast<float>( viewportRect.width ) * partialUpdateRatio );
  int checkHeight = static_cast<int>( static_cast<float>( viewportRect.height ) * partialUpdateRatio );
  Rect<int32_t> screenRect;

  bool isPartialUpdate = false;

  const RenderListContainer::SizeType renderListCount = instruction.RenderListCount();
  // Iterate through each render list.

  for( RenderListContainer::SizeType index = 0; index < renderListCount; ++index )
  {
    const RenderList* renderList = instruction.GetRenderList( index );

    if( renderList && !renderList->IsEmpty() && renderList->IsPartialUpdateEnabled() )
    {
      const std::size_t itemCount = renderList->Count();
      for( uint32_t itemIndex = 0u; itemIndex < itemCount; ++itemIndex )
      {
        const RenderItem& item = renderList->GetItem( itemIndex );

        if( item.mPartialUpdateEnabled )
        {
          isPartialUpdate = true;

          screenRect = item.CalculateViewportSpaceAABB( viewportRect.width, viewportRect.height, true );

          dx1 = std::min( screenRect.x, dx1 );
          dx2 = std::max( screenRect.x + screenRect.width, dx2);
          dy1 = std::min( screenRect.y, dy1 );
          dy2 = std::max( screenRect.y + screenRect.height, dy2 );

          if( ( dx2 - dx1 )  > checkWidth && ( dy2 - dy1 ) > checkHeight )
          {
            return false;
          }
        }
      }
    }
  }

  if( isPartialUpdate )
  {
    if( dx1 < 0.0f )
    {
      dx1 = 0.0f;
    }
    if( dy1 < 0.0f )
    {
      dy1 = 0.0f;
    }
    if( dx2 > viewportRect.width )
    {
      dx2 = viewportRect.width;
    }
    if( dy2 > viewportRect.height )
    {
      dy2 = viewportRect.height;
    }

    damagedRect.x = dx1;
    damagedRect.y = dy1;
    damagedRect.width = dx2 - dx1;
    damagedRect.height = dy2 - dy1;
  }

  return isPartialUpdate;
}

void RenderManager::DoRender( RenderInstruction& instruction )
{
  Rect<int32_t> viewportRect;
  Vector4   clearColor;
  bool isPartialUpdate = false;
  Dali::DamagedRect damagedRect;
  Dali::DamagedRect mergedRect;
  Dali::ClippingBox scissorBox;
  Dali::ClippingBox intersectRect;

  if ( instruction.mIsClearColorSet )
  {
    clearColor = instruction.mClearColor;
  }
  else
  {
    clearColor = Dali::RenderTask::DEFAULT_CLEAR_COLOR;
  }

  Rect<int32_t> surfaceRect = mImpl->defaultSurfaceRect;
  int surfaceOrientation = mImpl->defaultSurfaceOrientation;
  Integration::DepthBufferAvailable depthBufferAvailable = mImpl->depthBufferAvailable;
  Integration::StencilBufferAvailable stencilBufferAvailable = mImpl->stencilBufferAvailable;
  Integration::PartialUpdateAvailable partialUpdateAvailable = mImpl->partialUpdateAvailable;

  Render::SurfaceFrameBuffer* surfaceFrameBuffer = nullptr;
  if ( instruction.mFrameBuffer != 0 )
  {
    if ( instruction.mFrameBuffer->IsSurfaceBacked() )
    {
      surfaceFrameBuffer = static_cast<Render::SurfaceFrameBuffer*>( instruction.mFrameBuffer );

      if ( !surfaceFrameBuffer->IsSurfaceValid() )
      {
        // Skip rendering the frame buffer if the render surface becomes invalid
        return;
      }

      if ( mImpl->currentContext->IsSurfacelessContextSupported() )
      {
        Context* surfaceContext = surfaceFrameBuffer->GetContext();
        if ( mImpl->currentContext != surfaceContext )
        {
          // Switch the correct context if rendering to a surface
          mImpl->currentContext = surfaceContext;
          surfaceFrameBuffer->MakeContextCurrent();

          // Clear the current cached program when the context is switched
          mImpl->programController.ClearCurrentProgram();
        }
      }

      surfaceRect = Rect<int32_t>( 0, 0, static_cast<int32_t>( surfaceFrameBuffer->GetWidth() ), static_cast<int32_t>( surfaceFrameBuffer->GetHeight() ) );
    }
    else
    {
      // Switch to shared context for off-screen buffer
      mImpl->currentContext = &mImpl->context;

      if ( mImpl->currentContext->IsSurfacelessContextSupported() )
      {
        mImpl->glContextHelperAbstraction.MakeSurfacelessContextCurrent();
      }
    }
  }

  DALI_ASSERT_DEBUG( mImpl->currentContext->IsGlContextCreated() );

  // reset the program matrices for all programs once per frame
  // this ensures we will set view and projection matrix once per program per camera
  mImpl->programController.ResetProgramMatrices();

  if( instruction.mFrameBuffer )
  {
    instruction.mFrameBuffer->Bind( *mImpl->currentContext );

    if ( !instruction.mFrameBuffer->IsSurfaceBacked() )
    {
      // For each offscreen buffer, update the dependency list with the new texture id used by this frame buffer.
      Render::TextureFrameBuffer* textureFrameBuffer = static_cast<Render::TextureFrameBuffer*>( instruction.mFrameBuffer );
      mImpl->textureDependencyList.PushBack( textureFrameBuffer->GetTextureId() );
    }
  }
  else
  {
    mImpl->currentContext->BindFramebuffer( GL_FRAMEBUFFER, 0u );
  }


  if( surfaceFrameBuffer &&
      partialUpdateAvailable == Integration::PartialUpdateAvailable::TRUE )
  {
    // Iterate through each render list.
    if( surfaceFrameBuffer->IsPartialUpdateEnabled() )
    {
      isPartialUpdate = GetDamagedRect( surfaceRect, instruction, damagedRect ) ;
    }

    if( !isPartialUpdate )
    {
      damagedRect = surfaceRect;
    }

    surfaceFrameBuffer->SetDamagedRect( damagedRect, mergedRect );

    if( mergedRect.IsEmpty() )
    {
      isPartialUpdate = false;
    }
    else
    {
      scissorBox.x = mergedRect.x;
      scissorBox.y = mergedRect.y;
      scissorBox.width = mergedRect.width;
      scissorBox.height = mergedRect.height;
    }
  }

  if ( surfaceFrameBuffer )
  {
    mImpl->currentContext->Viewport( surfaceRect.x,
                              surfaceRect.y,
                              surfaceRect.width,
                              surfaceRect.height );
  }

  // Clear the entire color, depth and stencil buffers for the default framebuffer, if required.
  // It is important to clear all 3 buffers when they are being used, for performance on deferred renderers
  // e.g. previously when the depth & stencil buffers were NOT cleared, it caused the DDK to exceed a "vertex count limit",
  // and then stall. That problem is only noticeable when rendering a large number of vertices per frame.
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

  if( !instruction.mIgnoreRenderToFbo && ( instruction.mFrameBuffer != 0 ) )
  {
    if ( instruction.mFrameBuffer->IsSurfaceBacked() ) // Surface rendering
    {
      if ( instruction.mIsViewportSet )
      {
        // For glViewport the lower-left corner is (0,0)
        // For glViewport the lower-left corner is (0,0)
        const int32_t y = ( surfaceRect.height - instruction.mViewport.height ) - instruction.mViewport.y;
        viewportRect.Set( instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height );
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
      surfaceOrientation = 0;
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
        viewportRect.Set( instruction.mViewport.x, y, instruction.mViewport.width, instruction.mViewport.height );
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

  bool clearFullFrameRect = true;
  if( instruction.mFrameBuffer != 0 )
  {
    Viewport frameRect( 0, 0, instruction.mFrameBuffer->GetWidth(), instruction.mFrameBuffer->GetHeight() );
    clearFullFrameRect = ( frameRect == viewportRect );
  }
  else
  {
    clearFullFrameRect = ( surfaceRect == viewportRect );
  }

  if ( surfaceOrientation == 90 || surfaceOrientation == 270 )
  {
    int temp = viewportRect.width;
    viewportRect.width = viewportRect.height;
    viewportRect.height = temp;
  }

  mImpl->currentContext->Viewport(viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height);

  if( instruction.mIsClearColorSet )
  {
    mImpl->currentContext->ClearColor( clearColor.r,
                                       clearColor.g,
                                       clearColor.b,
                                       clearColor.a );

    if( !clearFullFrameRect )
    {
      mImpl->currentContext->SetScissorTest( true );
      if( isPartialUpdate )
      {
        intersectRect = IntersectAABB( scissorBox, viewportRect );
        mImpl->currentContext->Scissor( intersectRect.x, intersectRect.y, intersectRect.width, intersectRect.height );
      }
      else
      {
        mImpl->currentContext->Scissor( viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height );
      }
      mImpl->currentContext->Clear( clearMask, Context::FORCE_CLEAR );
      mImpl->currentContext->SetScissorTest( false );
    }
    else
    {
      mImpl->currentContext->SetScissorTest( false );
      mImpl->currentContext->Clear( clearMask, Context::FORCE_CLEAR );
    }
  }

  // Clear the list of bound textures
  mImpl->boundTextures.Clear();

  mImpl->renderAlgorithms.ProcessRenderInstruction(
      instruction,
      *mImpl->currentContext,
      mImpl->renderBufferIndex,
      depthBufferAvailable,
      stencilBufferAvailable,
      mImpl->boundTextures,
      surfaceOrientation,
      scissorBox );

  // Synchronise the FBO/Texture access when there are multiple contexts
  if ( mImpl->currentContext->IsSurfacelessContextSupported() )
  {
    // Check whether any binded texture is in the dependency list
    bool textureFound = false;

    if ( mImpl->boundTextures.Count() > 0u && mImpl->textureDependencyList.Count() > 0u )
    {
      for ( auto textureId : mImpl->textureDependencyList )
      {

        textureFound = std::find_if( mImpl->boundTextures.Begin(), mImpl->boundTextures.End(),
                                     [textureId]( GLuint id )
                                     {
                                       return textureId == id;
                                     } ) != mImpl->boundTextures.End();
      }
    }

    if ( textureFound )
    {

      if ( !instruction.mFrameBuffer || !instruction.mFrameBuffer->IsSurfaceBacked() )
      {
        // For off-screen buffer

        // Wait until all rendering calls for the currently context are executed
        mImpl->glContextHelperAbstraction.WaitClient();

        // Clear the dependency list
        mImpl->textureDependencyList.Clear();
      }
      else
      {
        // For surface-backed frame buffer

        // Worker thread lambda function
        auto& glContextHelperAbstraction = mImpl->glContextHelperAbstraction;
        auto workerFunction = [&glContextHelperAbstraction]( int workerThread )
        {
          // Switch to the shared context in the worker thread
          glContextHelperAbstraction.MakeSurfacelessContextCurrent();

          // Wait until all rendering calls for the shared context are executed
          glContextHelperAbstraction.WaitClient();

          // Must clear the context in the worker thread
          // Otherwise the shared context cannot be switched to from the render thread
          glContextHelperAbstraction.MakeContextNull();
        };

        auto future = mImpl->threadPool->SubmitTask( 0u, workerFunction );
        if ( future )
        {
          mImpl->threadPool->Wait();

          // Clear the dependency list
          mImpl->textureDependencyList.Clear();
        }
      }
    }
  }

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
  else
  {
    mImpl->currentContext->Flush();
  }

  if( instruction.mFrameBuffer && instruction.mFrameBuffer->IsSurfaceBacked() )
  {
    GLenum attachments[] = { GL_DEPTH, GL_STENCIL };
    mImpl->context.InvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
