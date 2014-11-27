/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/core.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/render-algorithms.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-instruction-container.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>
#include <dali/internal/render/gl-resources/native-frame-buffer-texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/renderers/render-material.h>
#include <dali/internal/render/renderers/scene-graph-renderer.h>
#include <dali/internal/render/shaders/program-controller.h>

// Uncomment the next line to enable frame snapshot logging
//#define FRAME_SNAPSHOT_LOGGING

#ifdef FRAME_SNAPSHOT_LOGGING

using namespace Dali::Internal::Render;

namespace // unnamed namespace
{
unsigned int SNAPSHOT_FRAME_FREQUENCY = 1200; // dump every 20-30 seconds
} // unnamed namespace

#define SET_SNAPSHOT_FRAME_LOG_LEVEL \
  DALI_LOG_FILTER_SET_LEVEL(Context::gGlLogFilter, ((GetFrameCount() % SNAPSHOT_FRAME_FREQUENCY)==5 ? Debug::General : Debug::Concise));

#else // FRAME_SNAPSHOT_LOGGING

#define SET_SNAPSHOT_FRAME_LOG_LEVEL

#endif // FRAME_SNAPSHOT_LOGGING

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

typedef OwnerContainer< Renderer* >            RendererOwnerContainer;
typedef RendererOwnerContainer::Iterator       RendererOwnerIter;

typedef OwnerContainer< RenderMaterial* >      RenderMaterialContainer;
typedef RenderMaterialContainer::Iterator      RenderMaterialIter;
typedef RenderMaterialContainer::ConstIterator RenderMaterialConstIter;

typedef OwnerContainer< RenderTracker* >       RenderTrackerContainer;
typedef RenderTrackerContainer::Iterator       RenderTrackerIter;
typedef RenderTrackerContainer::ConstIterator  RenderTrackerConstIter;

/**
 * Structure to contain internal data
 */
struct RenderManager::Impl
{
  Impl( Dali::Integration::GlAbstraction& glAbstraction,
        ResourcePostProcessList& resourcePostProcessQ,
        PostProcessResourceDispatcher& postProcessDispatcher )
  : context( glAbstraction ),
    renderQueue(),
    textureCache( renderQueue, postProcessDispatcher, context ),
    resourcePostProcessQueue( resourcePostProcessQ ),
    instructions(),
    backgroundColor( Dali::Stage::DEFAULT_BACKGROUND_COLOR ),
    frameTime( 0.0f ),
    lastFrameTime( 0.0f ),
    frameCount( 0 ),
    renderBufferIndex( SceneGraphBuffers::INITIAL_UPDATE_BUFFER_INDEX ),
    defaultSurfaceRect(),
    rendererContainer(),
    materials(),
    renderersAdded( false ),
    firstRenderCompleted( false ),
    defaultShader( NULL ),
    programController( postProcessDispatcher, glAbstraction )
  {
  }

  ~Impl()
  {
  }

  void AddRenderTracker( RenderTracker* renderTracker )
  {
    DALI_ASSERT_DEBUG( renderTracker != NULL );
    mRenderTrackers.PushBack( renderTracker );
  }

  void RemoveRenderTracker( RenderTracker* renderTracker )
  {
    DALI_ASSERT_DEBUG( renderTracker != NULL );
    for(RenderTrackerIter iter = mRenderTrackers.Begin(), end = mRenderTrackers.End(); iter != end; ++iter)
    {
      if( *iter == renderTracker )
      {
        mRenderTrackers.Erase( iter );
        break;
      }
    }
  }

  void UpdateTrackers()
  {
    for(RenderTrackerIter iter = mRenderTrackers.Begin(), end = mRenderTrackers.End(); iter != end; ++iter)
    {
      (*iter)->PollSyncObject();
    }
  }

  // the order is important for destruction,
  // programs are owned by context at the moment.
  Context                             context;                  ///< holds the GL state
  RenderQueue                         renderQueue;              ///< A message queue for receiving messages from the update-thread.
  TextureCache                        textureCache;             ///< Cache for all GL textures
  ResourcePostProcessList&            resourcePostProcessQueue; ///< A queue for requesting resource post processing in update thread

  // Render instructions describe what should be rendered during RenderManager::Render()
  // Owned by RenderManager. Update manager updates instructions for the next frame while we render the current one
  RenderInstructionContainer          instructions;

  Vector4                             backgroundColor;      ///< The glClear color used at the beginning of each frame.

  float                               frameTime;            ///< The elapsed time since the previous frame
  float                               lastFrameTime;        ///< Last frame delta.

  unsigned int                        frameCount;           ///< The current frame count
  BufferIndex                         renderBufferIndex;    ///< The index of the buffer to read from; this is opposite of the "update" buffer

  Rect<int>                           defaultSurfaceRect;   ///< Rectangle for the default surface we are rendering to

  RendererOwnerContainer              rendererContainer;    ///< List of owned renderers
  RenderMaterialContainer             materials;            ///< List of owned render materials

  bool                                renderersAdded;

  RenderTrackerContainer              mRenderTrackers;      ///< List of render trackers

  bool                                firstRenderCompleted; ///< False until the first render is done
  Shader*                             defaultShader;        ///< Default shader to use
  ProgramController                   programController;    ///< Owner of the GL programs
};

RenderManager* RenderManager::New( Integration::GlAbstraction& glAbstraction, ResourcePostProcessList& resourcePostProcessQ )
{
  RenderManager* manager = new RenderManager;
  manager->mImpl = new Impl( glAbstraction, resourcePostProcessQ, *manager );
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

TextureCache& RenderManager::GetTextureCache()
{
  return mImpl->textureCache;
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
  // @todo Set an atomic value to prevent render manager rendering again until
  // ContextCreated has been called.

  mImpl->context.GlContextDestroyed();
  mImpl->programController.GlContextDestroyed();

  // inform texture cache
  mImpl->textureCache.GlContextDestroyed(); // Clears gl texture ids

  // inform renderers
  RendererOwnerContainer::Iterator end = mImpl->rendererContainer.End();
  RendererOwnerContainer::Iterator iter = mImpl->rendererContainer.Begin();
  for( ; iter != end; ++iter )
  {
    GlResourceOwner* renderer = *iter;
    renderer->GlContextDestroyed(); // Clear up vertex buffers
  }
}

void RenderManager::DispatchPostProcessRequest(ResourcePostProcessRequest& request)
{
  mImpl->resourcePostProcessQueue[ mImpl->renderBufferIndex ].push_back( request );
}

RenderInstructionContainer& RenderManager::GetRenderInstructionContainer()
{
  return mImpl->instructions;
}

void RenderManager::SetBackgroundColor( const Vector4& color )
{
  mImpl->backgroundColor = color;
}

void RenderManager::SetFrameDeltaTime( float deltaTime )
{
  mImpl->frameTime = deltaTime;
}

void RenderManager::SetDefaultSurfaceRect(const Rect<int>& rect)
{
  mImpl->defaultSurfaceRect = rect;
}

void RenderManager::AddRenderer( Renderer* renderer )
{
  // Initialize the renderer as we are now in render thread
  renderer->Initialize( mImpl->context, mImpl->textureCache );

  mImpl->rendererContainer.PushBack( renderer );

  if( !mImpl->renderersAdded )
  {
    mImpl->renderersAdded = true;
  }
}

void RenderManager::RemoveRenderer( Renderer* renderer )
{
  DALI_ASSERT_DEBUG( NULL != renderer );

  RendererOwnerContainer& renderers = mImpl->rendererContainer;

  // Find the renderer
  for ( RendererOwnerIter iter = renderers.Begin(); iter != renderers.End(); ++iter )
  {
    if ( *iter == renderer )
    {
      renderers.Erase( iter ); // Renderer found; now destroy it
      break;
    }
  }
}

void RenderManager::AddRenderMaterial( RenderMaterial* renderMaterial )
{
  DALI_ASSERT_DEBUG( NULL != renderMaterial );

  mImpl->materials.PushBack( renderMaterial );
  renderMaterial->Initialize( mImpl->textureCache );
}

void RenderManager::RemoveRenderMaterial( RenderMaterial* renderMaterial )
{
  DALI_ASSERT_DEBUG( NULL != renderMaterial );

  RenderMaterialContainer& materials = mImpl->materials;

  // Find the render material and destroy it
  for ( RenderMaterialIter iter = materials.Begin(); iter != materials.End(); ++iter )
  {
    RenderMaterial& current = **iter;
    if ( &current == renderMaterial )
    {
      materials.Erase( iter );
      break;
    }
  }
}

void RenderManager::AddRenderTracker( RenderTracker* renderTracker )
{
  mImpl->AddRenderTracker(renderTracker);
}

void RenderManager::RemoveRenderTracker( RenderTracker* renderTracker )
{
  mImpl->RemoveRenderTracker(renderTracker);
}

void RenderManager::SetDefaultShader( Shader* shader )
{
  mImpl->defaultShader = shader;
}

ProgramCache* RenderManager::GetProgramCache()
{
  return &(mImpl->programController);
}

bool RenderManager::Render( Integration::RenderStatus& status )
{
  DALI_PRINT_RENDER_START( mImpl->renderBufferIndex );

  // Core::Render documents that GL context must be current before calling Render
  DALI_ASSERT_DEBUG( mImpl->context.IsGlContextCreated() );

  status.SetHasRendered( false );

  // Increment the frame count at the beginning of each frame
  ++(mImpl->frameCount);
  mImpl->context.SetFrameCount(mImpl->frameCount);
  mImpl->context.ClearRendererCount();
  mImpl->context.ClearCulledCount();

  PERF_MONITOR_START(PerformanceMonitor::DRAW_NODES);

  SET_SNAPSHOT_FRAME_LOG_LEVEL;

  // Process messages queued during previous update
  mImpl->renderQueue.ProcessMessages( mImpl->renderBufferIndex );

  // No need to make any gl calls if we've done 1st glClear & don't have any renderers to render during startup.
  if( !mImpl->firstRenderCompleted || mImpl->renderersAdded )
  {
    // switch rendering to adaptor provided (default) buffer
    mImpl->context.BindFramebuffer( GL_FRAMEBUFFER, 0 );

    mImpl->context.Viewport( mImpl->defaultSurfaceRect.x,
                             mImpl->defaultSurfaceRect.y,
                             mImpl->defaultSurfaceRect.width,
                             mImpl->defaultSurfaceRect.height );

    mImpl->context.ClearColor( mImpl->backgroundColor.r,
                               mImpl->backgroundColor.g,
                               mImpl->backgroundColor.b,
                               mImpl->backgroundColor.a );

    mImpl->context.ClearStencil( 0 );

    // Clear the entire color, depth and stencil buffers for the default framebuffer.
    // It is important to clear all 3 buffers, for performance on deferred renderers like Mali
    // e.g. previously when the depth & stencil buffers were NOT cleared, it caused the DDK to exceed a "vertex count limit",
    // and then stall. That problem is only noticeable when rendering a large number of vertices per frame.
    mImpl->context.SetScissorTest( false );
    mImpl->context.ColorMask( true );
    mImpl->context.DepthMask( true );
    mImpl->context.StencilMask( 0xFF ); // 8 bit stencil mask, all 1's
    mImpl->context.Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    // reset the program matrices for all programs once per frame
    // this ensures we will set view and projection matrix once per program per camera
    // @todo move programs out of context onto a program controller and let that handle this
    mImpl->programController.ResetProgramMatrices();

    // if we don't have default shader, no point doing the render calls
    if( mImpl->defaultShader )
    {
      size_t count = mImpl->instructions.Count( mImpl->renderBufferIndex );
      for ( size_t i = 0; i < count; ++i )
      {
        RenderInstruction& instruction = mImpl->instructions.At( mImpl->renderBufferIndex, i );

        DoRender( instruction, *mImpl->defaultShader, mImpl->lastFrameTime );

        const RenderListContainer::SizeType countRenderList = instruction.RenderListCount();
        if ( countRenderList > 0 )
        {
          status.SetHasRendered( true );
        }
      }
      GLenum attachments[] = { GL_DEPTH, GL_STENCIL };
      mImpl->context.InvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);

      mImpl->UpdateTrackers();

      mImpl->firstRenderCompleted = true;
    }
  }

  PERF_MONITOR_END(PerformanceMonitor::DRAW_NODES);

  // Update the frame time
  mImpl->lastFrameTime = mImpl->frameTime;

  // check if anything has been posted to the update thread
  bool updateRequired = !mImpl->resourcePostProcessQueue[ mImpl->renderBufferIndex ].empty();

  /**
   * The rendering has finished; swap to the next buffer.
   * Ideally the update has just finished using this buffer; otherwise the render thread
   * should block until the update has finished.
   */
  mImpl->renderBufferIndex = (0 != mImpl->renderBufferIndex) ? 0 : 1;

  DALI_PRINT_RENDER_END();

  DALI_PRINT_RENDERER_COUNT(mImpl->frameCount, mImpl->context.GetRendererCount());
  DALI_PRINT_CULL_COUNT(mImpl->frameCount, mImpl->context.GetCulledCount());

  return updateRequired;
}

void RenderManager::DoRender( RenderInstruction& instruction, Shader& defaultShader, float elapsedTime )
{
  Rect<int> viewportRect;
  Vector4   clearColor;

  if ( instruction.mIsClearColorSet )
  {
    clearColor = instruction.mClearColor;
  }
  else
  {
    clearColor = Dali::RenderTask::DEFAULT_CLEAR_COLOR;
  }

  FrameBufferTexture* offscreen = NULL;

  if ( instruction.mOffscreenTextureId != 0 )
  {
    offscreen = mImpl->textureCache.GetFramebuffer( instruction.mOffscreenTextureId );
    DALI_ASSERT_DEBUG( NULL != offscreen );

    if( NULL != offscreen &&
        offscreen->Prepare() )
    {
      // Check whether a viewport is specified, otherwise the full surface size is used
      if ( instruction.mIsViewportSet )
      {
        // For glViewport the lower-left corner is (0,0)
        const int y = ( offscreen->GetHeight() - instruction.mViewport.height ) - instruction.mViewport.y;
        viewportRect.Set( instruction.mViewport.x,  y, instruction.mViewport.width, instruction.mViewport.height );
      }
      else
      {
        viewportRect.Set( 0, 0, offscreen->GetWidth(), offscreen->GetHeight() );
      }
    }
    else
    {
      // Offscreen is NULL or could not be prepared.
      return;
    }
  }
  else // !(instruction.mOffscreenTexture)
  {
    // switch rendering to adaptor provided (default) buffer
    mImpl->context.BindFramebuffer( GL_FRAMEBUFFER, 0 );

    // Check whether a viewport is specified, otherwise the full surface size is used
    if ( instruction.mIsViewportSet )
    {
      // For glViewport the lower-left corner is (0,0)
      const int y = ( mImpl->defaultSurfaceRect.height - instruction.mViewport.height ) - instruction.mViewport.y;
      viewportRect.Set( instruction.mViewport.x,  y, instruction.mViewport.width, instruction.mViewport.height );
    }
    else
    {
      viewportRect = mImpl->defaultSurfaceRect;
    }
  }

  mImpl->context.Viewport(viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height);

  if ( instruction.mIsClearColorSet )
  {
    mImpl->context.ClearColor( clearColor.r,
                               clearColor.g,
                               clearColor.b,
                               clearColor.a );

    // Clear the viewport area only
    mImpl->context.SetScissorTest( true );
    mImpl->context.Scissor( viewportRect.x, viewportRect.y, viewportRect.width, viewportRect.height );
    mImpl->context.ColorMask( true );
    mImpl->context.Clear( GL_COLOR_BUFFER_BIT );
    mImpl->context.SetScissorTest( false );
  }

  Render::ProcessRenderInstruction( instruction,
                                    mImpl->context,
                                    defaultShader,
                                    mImpl->renderBufferIndex,
                                    elapsedTime );

  if(instruction.mOffscreenTextureId != 0)
  {
    GLenum attachments[] = { GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
    mImpl->context.InvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
  }

  if( instruction.mRenderTracker && offscreen != NULL )
  {
    instruction.mRenderTracker->CreateSyncObject();
    instruction.mRenderTracker = NULL; // Only create once.
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
