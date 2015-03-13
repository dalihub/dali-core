/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

#include "scene-graph-renderer-attachment.h"
#include <dali/internal/update/effects/scene-graph-material.h>
#include <dali/internal/update/effects/scene-graph-sampler.h>
#include <dali/internal/update/geometry/scene-graph-geometry.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-renderer.h>

#if defined(DEBUG_ENABLED)
Debug::Filter* gImageAttachmentLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SCENE_GRAPH_IMAGE_ATTACHMENT");

#define ATTACHMENT_LOG(level)                                                 \
  DALI_LOG_INFO(gImageAttachmentLogFilter, level, "SceneGraph::ImageAttachment::%s: this:%p\n", __FUNCTION__, this)
#define ATTACHMENT_LOG_FMT(level, format, args...) \
  DALI_LOG_INFO(gImageAttachmentLogFilter, level, "SceneGraph::ImageAttachment::%s: this:%p " format, __FUNCTION__, this, ## args)

#else

#define ATTACHMENT_LOG(level)
#define ATTACHMENT_LOG_FMT(level, format, args...)

#endif


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RendererAttachment* RendererAttachment::New()
{
  return new RendererAttachment();
}


RendererAttachment::RendererAttachment()
: RenderableAttachment( false ),
  mRenderer(NULL),
  mMaterial(NULL),
  mGeometry(NULL),
  mDepthIndex(0)
{
}

RendererAttachment::~RendererAttachment()
{
  mMaterial=NULL;
  mGeometry=NULL;
}

void RendererAttachment::SetMaterial( BufferIndex updateBufferIndex, const Material* material)
{
  mMaterial = material;

  // Tell renderer about a new provider
  if( mRenderer )
  {
    typedef MessageValue1< NewRenderer, const MaterialDataProvider*> DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &NewRenderer::SetMaterialDataProvider, material );
  }
}

const Material& RendererAttachment::GetMaterial() const
{
  return *mMaterial;
}

void RendererAttachment::SetGeometry( BufferIndex updateBufferIndex, const Geometry* geometry)
{
  mGeometry = geometry;

  // Tell renderer about a new provider
  if( mRenderer )
  {
    typedef MessageValue1< NewRenderer, const GeometryDataProvider*> DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &NewRenderer::SetGeometryDataProvider, geometry );
  }
}

const Geometry& RendererAttachment::GetGeometry() const
{
  return *mGeometry;
}

void RendererAttachment::SetDepthIndex( int index )
{
  // @todo MESH_REWORK
  mDepthIndex = index;
}

int RendererAttachment::GetDepthIndex() const
{
  return mDepthIndex;
}


Renderer& RendererAttachment::GetRenderer()
{
  return *mRenderer;
}

const Renderer& RendererAttachment::GetRenderer() const
{
  return *mRenderer;
}

void RendererAttachment::DoPrepareRender( BufferIndex updateBufferIndex )
{
  // Do nothing
}

bool RendererAttachment::IsFullyOpaque( BufferIndex updateBufferIndex )
{
  bool opaque = false;

  if( mParent )
  {
    opaque = mParent->GetWorldColor( updateBufferIndex ).a >= FULLY_OPAQUE;
  }

  if( mMaterial != NULL )
  {
    // Require that all affecting samplers are opaque
    unsigned int opaqueCount=0;
    unsigned int affectingCount=0;

    const Material::Samplers& samplers = mMaterial->GetSamplers();
    for( Material::Samplers::ConstIterator iter = samplers.Begin();
         iter != samplers.End(); ++iter )
    {
      const Sampler* sampler = static_cast<const Sampler*>(*iter);
      if( sampler != NULL )
      {
        if( sampler->AffectsTransparency( updateBufferIndex ) )
        {
          affectingCount++;
          if( sampler->IsFullyOpaque( updateBufferIndex ) )
          {
            opaqueCount++;
          }
        }
      }
    }
    opaque = (opaqueCount == affectingCount);
  }

  return opaque;
}

void RendererAttachment::SizeChanged( BufferIndex updateBufferIndex )
{
  // Do nothing.
}

void RendererAttachment::AttachToSceneGraph( SceneController& sceneController, BufferIndex updateBufferIndex )
{
  mSceneController = &sceneController;
}

void RendererAttachment::ConnectToSceneGraph2( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( mSceneController );
  mRenderer = NewRenderer::New( *mParent, mGeometry, mMaterial );
  mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
}

void RendererAttachment::OnDestroy2()
{
  DALI_ASSERT_DEBUG( mSceneController );
  mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
  mRenderer = NULL;
}

bool RendererAttachment::DoPrepareResources(
  BufferIndex updateBufferIndex,
  ResourceManager& resourceManager )
{
  DALI_ASSERT_DEBUG( mSceneController );

  CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();

  bool ready = false;
  mFinishedResourceAcquisition = false;

  if( mGeometry && mMaterial )
  {
    unsigned int completeCount = 0;
    unsigned int neverCount = 0;
    unsigned int frameBufferCount = 0;

    const Material::Samplers& samplers = mMaterial->GetSamplers();
    for( Material::Samplers::ConstIterator iter = samplers.Begin();
         iter != samplers.End(); ++iter )
    {
      const Sampler* sampler = static_cast<const Sampler*>(*iter);

      ResourceId textureId = sampler->GetTextureId( updateBufferIndex );
      switch( completeStatusManager.GetStatus( textureId ) )
      {
        case CompleteStatusManager::NOT_READY:
        {
          ready = false;
          BitmapMetadata metaData = resourceManager.GetBitmapMetadata( textureId );
          if( metaData.GetIsFramebuffer() )
          {
            frameBufferCount++;
          }
          FollowTracker( textureId ); // @todo MESH_REWORK Trackers per sampler rather than per actor?
        }
        break;

        case CompleteStatusManager::COMPLETE:
        {
          completeCount++;
        }
        break;

        case CompleteStatusManager::NEVER:
        {
          neverCount++;
        }
        break;
      }
    }

    // We are ready if all samplers are complete, or those that aren't are framebuffers
    // We are complete if all samplers are either complete or will never complete

    ready = ( completeCount + frameBufferCount >= samplers.Count() ) ;
    mFinishedResourceAcquisition = ( completeCount + neverCount >= samplers.Count() );
  }
  return ready;
}



} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
