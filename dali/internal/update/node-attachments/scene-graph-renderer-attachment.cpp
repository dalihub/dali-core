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
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/geometry/scene-graph-geometry.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

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


namespace // unnamed namespace
{
const int REGENERATE_UNIFORM_MAP = 2;
const int COPY_UNIFORM_MAP       = 1;
}

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
  mRegenerateUniformMap(REGENERATE_UNIFORM_MAP),
  mResendDataProviders(false),
  mDepthIndex(0)
{
  mUniformMapChanged[0]=false;
  mUniformMapChanged[1]=false;

  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
}


RendererAttachment::~RendererAttachment()
{
  mMaterial->RemoveConnectionObserver(*this);
  mGeometry->RemoveConnectionObserver(*this);

  mMaterial=NULL;
  mGeometry=NULL;
}

void RendererAttachment::Initialize2( BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( mSceneController );
}

void RendererAttachment::OnDestroy2()
{
}

void RendererAttachment::ConnectedToSceneGraph()
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
  mParent->AddUniformMapObserver( *this );

  DALI_ASSERT_DEBUG( mParent != NULL );

  RenderDataProvider* dataProvider = new RenderDataProvider( *mGeometry,
                                                             *mMaterial,
                                                             *this,
                                                             *mMaterial->GetShader(),
                                                             mGeometry->GetIndexBuffer(),
                                                             mGeometry->GetVertexBuffers(),
                                                             mMaterial->GetSamplers() );

  mRenderer = NewRenderer::New( *mParent, dataProvider );
  mSceneController->GetRenderMessageDispatcher().AddRenderer( *mRenderer );
}

void RendererAttachment::DisconnectedFromSceneGraph()
{
  mRegenerateUniformMap = 0;
  mParent->RemoveUniformMapObserver( *this );

  DALI_ASSERT_DEBUG( mSceneController );
  mSceneController->GetRenderMessageDispatcher().RemoveRenderer( *mRenderer );
  mRenderer = NULL;
}

void RendererAttachment::SetMaterial( BufferIndex updateBufferIndex, const Material* material)
{
  DALI_ASSERT_DEBUG( material != NULL && "Material pointer is NULL" );

  mMaterial = const_cast<Material*>(material); // Need this to be non-const to add observer only.
  mMaterial->AddConnectionObserver( *this );
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  mResendDataProviders = true;
}

const Material& RendererAttachment::GetMaterial() const
{
  return *mMaterial;
}

void RendererAttachment::SetGeometry( BufferIndex updateBufferIndex, const Geometry* geometry)
{
  DALI_ASSERT_DEBUG( geometry != NULL && "Geometry pointer is NULL");

  mGeometry = const_cast<Geometry*>(geometry); // Need this to be non-const to add observer only
  mGeometry->AddConnectionObserver( *this ); // Observe geometry connections / uniform mapping changes
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  mResendDataProviders = true;
}

const Geometry& RendererAttachment::GetGeometry() const
{
  return *mGeometry;
}

int RendererAttachment::GetDepthIndex(BufferIndex bufferIndex) const
{
  return mDepthIndex[bufferIndex];
}

Renderer& RendererAttachment::GetRenderer()
{
  return *mRenderer;
}

const Renderer& RendererAttachment::GetRenderer() const
{
  return *mRenderer;
}

// Uniform maps are checked in the following priority order:
//   Renderer (this object)
//   Actor
//   Material
//   Samplers
//   Shader
//   Geometry
//   VertexBuffers
void RendererAttachment::DoPrepareRender( BufferIndex updateBufferIndex )
{
  if( mRegenerateUniformMap > 0)
  {
    if( mRegenerateUniformMap == REGENERATE_UNIFORM_MAP)
    {
      DALI_ASSERT_DEBUG( mGeometry != NULL && "No geometry available in DoPrepareRender()" );
      DALI_ASSERT_DEBUG( mMaterial != NULL && "No geometry available in DoPrepareRender()" );

      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      localMap.Resize(0);

      const UniformMap& rendererUniformMap = PropertyOwner::GetUniformMap();
      AddMappings( localMap, rendererUniformMap );

      const UniformMap& actorUniformMap = mParent->GetUniformMap();
      AddMappings( localMap, actorUniformMap );

      AddMappings( localMap, mMaterial->GetUniformMap() );
      const RenderDataProvider::Samplers& samplers = mMaterial->GetSamplers();
      for( RenderDataProvider::Samplers::ConstIterator iter = samplers.Begin(), end = samplers.End();
           iter != end ;
           ++iter )
      {
        const SceneGraph::Sampler* sampler = static_cast<const SceneGraph::Sampler*>( *iter );
        AddMappings( localMap, sampler->GetUniformMap() );
      }

      AddMappings( localMap, mMaterial->GetShader()->GetUniformMap() );

      AddMappings( localMap, mGeometry->GetUniformMap() );

      const GeometryDataProvider::VertexBuffers& vertexBuffers = mGeometry->GetVertexBuffers();
      for( GeometryDataProvider::VertexBuffers::ConstIterator iter = vertexBuffers.Begin(), end = vertexBuffers.End() ;
           iter != end ;
           ++iter )
      {
        AddMappings( localMap, (*iter)->GetUniformMap() );
      }

      mUniformMapChanged[updateBufferIndex] = true;
    }
    else if( mRegenerateUniformMap == COPY_UNIFORM_MAP )
    {
      // Copy old map into current map
      CollectedUniformMap& localMap = mCollectedUniformMap[ updateBufferIndex ];
      CollectedUniformMap& oldMap = mCollectedUniformMap[ 1-updateBufferIndex ];

      localMap.Resize( oldMap.Count() );

      unsigned int index=0;
      for( CollectedUniformMap::Iterator iter = oldMap.Begin(), end = oldMap.End() ; iter != end ; ++iter )
      {
        localMap[index] = *iter;
      }

      mUniformMapChanged[updateBufferIndex] = true;
    }

    mRegenerateUniformMap--;
  }

  if( mResendDataProviders )
  {
    RenderDataProvider* dataProvider = new RenderDataProvider( *mGeometry,
                                                               *mMaterial,
                                                               *this,
                                                               *mMaterial->GetShader(),
                                                               mGeometry->GetIndexBuffer(),
                                                               mGeometry->GetVertexBuffers(),
                                                               mMaterial->GetSamplers() );

    // Tell renderer about a new provider
    // @todo MESH_REWORK Should we instead create a new renderer when these change?

    typedef MessageValue1< NewRenderer, OwnerPointer<RenderDataProvider> > DerivedType;
    unsigned int* slot = mSceneController->GetRenderQueue().ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );
    new (slot) DerivedType( mRenderer, &NewRenderer::SetRenderDataProvider, dataProvider );

    mResendDataProviders = false;
  }
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

    const RenderDataProvider::Samplers& samplers = mMaterial->GetSamplers();
    for( RenderDataProvider::Samplers::ConstIterator iter = samplers.Begin();
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

bool RendererAttachment::DoPrepareResources(
  BufferIndex updateBufferIndex,
  ResourceManager& resourceManager )
{
  DALI_ASSERT_DEBUG( mSceneController );

  CompleteStatusManager& completeStatusManager = mSceneController->GetCompleteStatusManager();

  bool ready = false;
  mFinishedResourceAcquisition = false;

  // Can only be considered ready when all the scene graph objects are connected to the renderer
  if( ( mGeometry ) &&
      ( mGeometry->GetVertexBuffers().Count() > 0 ) &&
      ( mMaterial ) &&
      ( mMaterial->GetShader() != NULL ) )
  {
    unsigned int completeCount = 0;
    unsigned int neverCount = 0;
    unsigned int frameBufferCount = 0;

    const RenderDataProvider::Samplers& samplers = mMaterial->GetSamplers();
    for( RenderDataProvider::Samplers::ConstIterator iter = samplers.Begin();
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

void RendererAttachment::ConnectionsChanged( PropertyOwner& object )
{
  // One of our child objects has changed it's connections. Ensure the uniform
  // map gets regenerated during PrepareRender
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;

  // Ensure the child object pointers get re-sent to the renderer
  mResendDataProviders = true;
}

void RendererAttachment::ConnectedUniformMapChanged()
{
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

void RendererAttachment::UniformMappingsChanged( const UniformMap& mappings )
{
  // The mappings are either from PropertyOwner base class, or the Actor
  mRegenerateUniformMap = REGENERATE_UNIFORM_MAP;
}

bool RendererAttachment::GetUniformMapChanged( BufferIndex bufferIndex ) const
{
  return mUniformMapChanged[bufferIndex];
}

const CollectedUniformMap& RendererAttachment::GetUniformMap( BufferIndex bufferIndex ) const
{
  return mCollectedUniformMap[ bufferIndex ];
}

void RendererAttachment::AddMappings( CollectedUniformMap& localMap, const UniformMap& uniformMap )
{
  // Iterate thru uniformMap.
  //   Any maps that aren't in localMap should be added in a single step
  CollectedUniformMap newUniformMappings;

  for( unsigned int i=0, count=uniformMap.Count(); i<count; ++i )
  {
    UniformPropertyMapping::Hash nameHash = uniformMap[i].uniformNameHash;
    bool found = false;

    for( CollectedUniformMap::Iterator iter = localMap.Begin() ; iter != localMap.End() ; ++iter )
    {
      const UniformPropertyMapping* map = (*iter);
      if( map->uniformNameHash == nameHash )
      {
        if( map->uniformName == uniformMap[i].uniformName )
        {
          found = true;
          break;
        }
      }
    }
    if( !found )
    {
      // it's a new mapping. Add raw ptr to temporary list
      newUniformMappings.PushBack( &uniformMap[i] );
    }
  }

  if( newUniformMappings.Count() > 0 )
  {
    localMap.Reserve( localMap.Count() + newUniformMappings.Count() );

    for( CollectedUniformMap::Iterator iter = newUniformMappings.Begin(),
           end = newUniformMappings.End() ;
         iter != end ;
         ++iter )
    {
      const UniformPropertyMapping* map = (*iter);
      localMap.PushBack( map );
    }
    //@todo MESH_REWORK Use memcpy to copy ptrs from one array to the other
  }
}



} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
