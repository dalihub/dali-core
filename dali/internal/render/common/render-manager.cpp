/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/render/common/render-algorithms.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/shaders/program-controller.h>
#include <dali/internal/update/common/scene-graph-scene.h>

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

/**
 * Structure to contain internal data
 */
struct RenderManager::Impl
{
  Impl( Integration::GlAbstraction& glAbstraction,
        Integration::GlSyncAbstraction& glSyncAbstraction,
        Integration::GlContextHelperAbstraction& glContextHelperAbstraction,
        Integration::DepthBufferAvailable depthBufferAvailableParam,
        Integration::StencilBufferAvailable stencilBufferAvailableParam )
  : context( glAbstraction, &sceneContextContainer ),
    currentContext( &context ),
    glAbstraction( glAbstraction ),
    glSyncAbstraction( glSyncAbstraction ),
    glContextHelperAbstraction( glContextHelperAbstraction ),
    renderQueue(),
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
    stencilBufferAvailable( stencilBufferAvailableParam )
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

  Context* CreateSceneContext()
  {
    sceneContextContainer.push_back( new Context( glAbstraction ) );
    return sceneContextContainer[ sceneContextContainer.size() - 1 ];
  }

  void DestroySceneContext( Context* sceneContext )
  {
    auto iter = std::find( sceneContextContainer.begin(), sceneContextContainer.end(), sceneContext );
    if( iter != sceneContextContainer.end() )
    {
      sceneContextContainer.erase( iter );
    }
  }

  Context* ReplaceSceneContext( Context* oldSceneContext )
  {
    Context* newContext = new Context( glAbstraction );
    std::replace( sceneContextContainer.begin(), sceneContextContainer.end(), oldSceneContext, newContext );
    return newContext;
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
  std::vector< Context* >                   sceneContextContainer;   ///< List of owned contexts holding the GL state per scene
  Integration::GlAbstraction&               glAbstraction;           ///< GL abstraction
  Integration::GlSyncAbstraction&           glSyncAbstraction;       ///< GL sync abstraction
  Integration::GlContextHelperAbstraction&  glContextHelperAbstraction; ///< GL context helper abstraction
  RenderQueue                               renderQueue;             ///< A message queue for receiving messages from the update-thread.

  std::vector< SceneGraph::Scene* >         sceneContainer;          ///< List of pointers to the scene graph objects of the scenes

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

  std::unique_ptr<Dali::ThreadPool>         threadPool;               ///< The thread pool
  Vector<GLuint>                            boundTextures;            ///< The textures bound for rendering
  Vector<GLuint>                            textureDependencyList;    ///< The dependency list of binded textures
};

RenderManager* RenderManager::New( Integration::GlAbstraction& glAbstraction,
                                   Integration::GlSyncAbstraction& glSyncAbstraction,
                                   Integration::GlContextHelperAbstraction& glContextHelperAbstraction,
                                   Integration::DepthBufferAvailable depthBufferAvailable,
                                   Integration::StencilBufferAvailable stencilBufferAvailable )
{
  RenderManager* manager = new RenderManager;
  manager->mImpl = new Impl( glAbstraction,
                             glSyncAbstraction,
                             glContextHelperAbstraction,
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

  // inform scenes
  for( auto&& scene : mImpl->sceneContainer )
  {
    scene->GlContextDestroyed();
  }
}

void RenderManager::SetShaderSaver( ShaderSaver& upstream )
{
  mImpl->programController.SetShaderSaver( upstream );
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

void RenderManager::AddFrameBuffer( OwnerPointer< Render::FrameBuffer >& frameBuffer )
{
  Render::FrameBuffer* frameBufferPtr = frameBuffer.Release();
  mImpl->frameBufferContainer.PushBack( frameBufferPtr );
  frameBufferPtr->Initialize( mImpl->context );
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
      mImpl->frameBufferContainer.Erase( &iter ); // frameBuffer found; now destroy it

      break;
    }
  }
}
void RenderManager::InitializeScene( SceneGraph::Scene* scene )
{
  scene->Initialize( *mImpl->CreateSceneContext() );
  mImpl->sceneContainer.push_back( scene );
}

void RenderManager::UninitializeScene( SceneGraph::Scene* scene )
{
  mImpl->DestroySceneContext( scene->GetContext() );

  auto iter = std::find( mImpl->sceneContainer.begin(), mImpl->sceneContainer.end(), scene );
  if( iter != mImpl->sceneContainer.end() )
  {
    mImpl->sceneContainer.erase( iter );
  }
}

void RenderManager::SurfaceReplaced( SceneGraph::Scene* scene )
{
  Context* newContext = mImpl->ReplaceSceneContext( scene->GetContext() );
  scene->Initialize( *newContext );
}

void RenderManager::AttachColorTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer )
{
  frameBuffer->AttachColorTexture( mImpl->context, texture, mipmapLevel, layer );
}

void RenderManager::AttachDepthTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel )
{
  frameBuffer->AttachDepthTexture( mImpl->context, texture, mipmapLevel );
}

void RenderManager::AttachDepthStencilTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel )
{
  frameBuffer->AttachDepthStencilTexture( mImpl->context, texture, mipmapLevel );
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

void RenderManager::PreRender( Integration::RenderStatus& status, bool forceClear, bool uploadOnly )
{
  DALI_PRINT_RENDER_START( mImpl->renderBufferIndex );

  // Core::Render documents that GL context must be current before calling Render
  DALI_ASSERT_DEBUG( mImpl->context.IsGlContextCreated() );

  // Increment the frame count at the beginning of each frame
  ++mImpl->frameCount;

  // Process messages queued during previous update
  mImpl->renderQueue.ProcessMessages( mImpl->renderBufferIndex );

  uint32_t count = 0u;
  for( uint32_t i = 0; i < mImpl->sceneContainer.size(); ++i )
  {
    count += mImpl->sceneContainer[i]->GetRenderInstructions().Count( mImpl->renderBufferIndex );
  }

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
    for( uint32_t i = 0; i < mImpl->sceneContainer.size(); ++i )
    {
      RenderInstructionContainer& instructions = mImpl->sceneContainer[i]->GetRenderInstructions();
      for ( uint32_t j = 0; j < instructions.Count( mImpl->renderBufferIndex ); ++j )
      {
        RenderInstruction& instruction = instructions.At( mImpl->renderBufferIndex, j );

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
    }
  }
}


void RenderManager::RenderScene( Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo )
{
  Internal::Scene& sceneInternal = GetImplementation( scene );
  SceneGraph::Scene* sceneObject = sceneInternal.GetSceneObject();

  uint32_t count = sceneObject->GetRenderInstructions().Count( mImpl->renderBufferIndex );

  for( uint32_t i = 0; i < count; ++i )
  {
    RenderInstruction& instruction = sceneObject->GetRenderInstructions().At( mImpl->renderBufferIndex, i );

    if ( ( renderToFbo && !instruction.mFrameBuffer ) || ( !renderToFbo && instruction.mFrameBuffer ) )
    {
      continue; // skip
    }

    // Mark that we will require a post-render step to be performed (includes swap-buffers).
    status.SetNeedsPostRender( true );

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
    Integration::DepthBufferAvailable depthBufferAvailable = mImpl->depthBufferAvailable;
    Integration::StencilBufferAvailable stencilBufferAvailable = mImpl->stencilBufferAvailable;

    if ( instruction.mFrameBuffer )
    {
      // offscreen buffer
      if ( mImpl->currentContext != &mImpl->context )
      {
        // Switch to shared context for off-screen buffer
        mImpl->currentContext = &mImpl->context;

        if ( mImpl->currentContext->IsSurfacelessContextSupported() )
        {
          mImpl->glContextHelperAbstraction.MakeSurfacelessContextCurrent();
        }

        // Clear the current cached program when the context is switched
        mImpl->programController.ClearCurrentProgram();
      }
    }
    else
    {
      if ( mImpl->currentContext->IsSurfacelessContextSupported() )
      {
        if ( mImpl->currentContext != sceneObject->GetContext() )
        {
          // Switch the correct context if rendering to a surface
          mImpl->currentContext = sceneObject->GetContext();

          // Clear the current cached program when the context is switched
          mImpl->programController.ClearCurrentProgram();
        }
      }

      surfaceRect = Rect<int32_t>( 0, 0, static_cast<int32_t>( scene.GetSize().width ), static_cast<int32_t>( scene.GetSize().height ) );
    }

    DALI_ASSERT_DEBUG( mImpl->currentContext->IsGlContextCreated() );

    // reset the program matrices for all programs once per frame
    // this ensures we will set view and projection matrix once per program per camera
    mImpl->programController.ResetProgramMatrices();

    if( instruction.mFrameBuffer )
    {
      instruction.mFrameBuffer->Bind( *mImpl->currentContext );

      // For each offscreen buffer, update the dependency list with the new texture id used by this frame buffer.
      for (unsigned int i0 = 0, i1 = instruction.mFrameBuffer->GetColorAttachmentCount(); i0 < i1; ++i0)
      {
        mImpl->textureDependencyList.PushBack( instruction.mFrameBuffer->GetTextureId(i0) );
      }
    }
    else
    {
      mImpl->currentContext->BindFramebuffer( GL_FRAMEBUFFER, 0u );
    }

    if ( !instruction.mFrameBuffer )
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
      // Offscreen buffer rendering
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
    else // No Offscreen frame buffer rendering
    {
      // Check whether a viewport is specified, otherwise the full surface size is used
      if ( instruction.mIsViewportSet )
      {
        // For glViewport the lower-left corner is (0,0)
        const int32_t y = ( surfaceRect.height - instruction.mViewport.height ) - instruction.mViewport.y;
        viewportRect.Set( instruction.mViewport.x,  y, instruction.mViewport.width, instruction.mViewport.height );
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
        mImpl->currentContext->Scissor( viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height );
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
        mImpl->boundTextures );

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
        if ( instruction.mFrameBuffer )
        {
          // For off-screen buffer

          // Wait until all rendering calls for the currently context are executed
          mImpl->glContextHelperAbstraction.WaitClient();

          // Clear the dependency list
          mImpl->textureDependencyList.Clear();
        }
        else
        {
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

    if( instruction.mRenderTracker && instruction.mFrameBuffer )
    {
      // This will create a sync object every frame this render tracker
      // is alive (though it should be now be created only for
      // render-once render tasks)
      instruction.mRenderTracker->CreateSyncObject( mImpl->glSyncAbstraction );
      instruction.mRenderTracker = nullptr; // Only create once.
    }

    if ( renderToFbo )
    {
      mImpl->currentContext->Flush();
    }
  }

  GLenum attachments[] = { GL_DEPTH, GL_STENCIL };
  mImpl->currentContext->InvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
}

void RenderManager::PostRender( bool uploadOnly )
{
  if ( !uploadOnly )
  {
    if ( mImpl->currentContext->IsSurfacelessContextSupported() )
    {
      mImpl->glContextHelperAbstraction.MakeSurfacelessContextCurrent();
    }

    GLenum attachments[] = { GL_DEPTH, GL_STENCIL };
    mImpl->context.InvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
  }

  //Notify RenderGeometries that rendering has finished
  for ( auto&& iter : mImpl->geometryContainer )
  {
    iter->OnRenderFinished();
  }

  mImpl->UpdateTrackers();


  uint32_t count = 0u;
  for( uint32_t i = 0; i < mImpl->sceneContainer.size(); ++i )
  {
    count += mImpl->sceneContainer[i]->GetRenderInstructions().Count( mImpl->renderBufferIndex );
  }

  const bool haveInstructions = count > 0u;

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

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
