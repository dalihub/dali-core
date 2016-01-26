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

// CLASS HEADER
#include "scene-graph-material.h"

// INTERNAL HEADERS
#include <dali/public-api/actors/blending.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/update/resources/texture-metadata.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/common/memory-pool-object-allocator.h>

namespace //Unnamed namespace
{
//Memory pool used to allocate new materials. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Material> gMaterialMemoryPool;
}

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Material* Material::New()
{
  return new ( gMaterialMemoryPool.AllocateRawThreadSafe() ) Material();
}

Material::Material()
: mShader( NULL ),
  mSamplers(),
  mTextureId(),
  mUniformName(),
  mConnectionObservers(),
  mResourcesReady( false ),
  mFinishedResourceAcquisition( false ),
  mMaterialChanged( true )
{
  // Observe own property-owner's uniform map
  AddUniformMapObserver( *this );
}

Material::~Material()
{
  mConnectionObservers.Destroy( *this );
}

void Material::operator delete( void* ptr )
{
  gMaterialMemoryPool.FreeThreadSafe( static_cast<Material*>( ptr ) );
}

void Material::Prepare( const ResourceManager& resourceManager )
{
  if( mMaterialChanged )
  {
    unsigned int opaqueCount = 0;
    unsigned int completeCount = 0;
    unsigned int failedCount = 0;
    unsigned int frameBufferCount = 0;
    const std::size_t textureCount( mTextureId.Count() );
    if( textureCount > 0 )
    {
      for( unsigned int i(0); i<textureCount; ++i )
      {
        const ResourceId textureId = mTextureId[ i ];
        TextureMetadata* metadata = NULL;
        // if there is metadata, resource is loaded
        if( resourceManager.GetTextureMetadata( textureId, metadata ) )
        {
          DALI_ASSERT_DEBUG( metadata );
          // metadata is valid pointer from now on
          if( metadata->IsFullyOpaque() )
          {
            ++opaqueCount;
          }

          if( metadata->IsFramebuffer() )
          {
            if( metadata->HasFrameBufferBeenRenderedTo() )
            {
              ++completeCount;
            }
            else
            {
              frameBufferCount++;
            }
          }
          else
          {
            // loaded so will complete this frame
            ++completeCount;
          }
          // no point checking failure as there is no metadata for failed loads
        }
        // if no metadata, loading can be failed
        else if( resourceManager.HasResourceLoadFailed( textureId ) )
        {
          ++failedCount;
        }
      }
    }

    //  whether the textures or the shader require the opacity to be translucent
    mIsTranslucent = ( opaqueCount != textureCount ) || ( mShader && mShader->GeometryHintEnabled( Dali::ShaderEffect::HINT_BLENDING ) );

    // ready for rendering when all textures are either successfully loaded or they are FBOs
    mResourcesReady = (completeCount + frameBufferCount >= textureCount);

    // material is complete if all resources are either loaded or failed or, if they are FBOs have been rendererd to
    mFinishedResourceAcquisition = ( completeCount + failedCount == textureCount );

    if( mFinishedResourceAcquisition )
    {
      // material is now considered not changed
      mMaterialChanged = false;
    }
  }
}

void Material::SetShader( Shader* shader )
{
  mMaterialChanged = true;
  mShader = shader;
  mShader->AddUniformMapObserver( *this );

  // Inform NewRenderer about this shader: (Will force a re-load of the
  // shader from the data providers)
  mConnectionObservers.ConnectionsChanged(*this);
}

Shader* Material::GetShader() const
{
  return mShader;
}

bool Material::IsTranslucent() const
{
  return mIsTranslucent;
}

void Material::AddTexture( const std::string& name, ResourceId id, Render::Sampler* sampler )
{
  mMaterialChanged = true;
  mTextureId.PushBack( id );
  mUniformName.push_back( name );
  mSamplers.PushBack( sampler );

  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::RemoveTexture( size_t index )
{
  mMaterialChanged = true;
  mTextureId.Erase( mTextureId.Begin()+index );
  mUniformName.erase( mUniformName.begin() + index );
  mSamplers.Erase( mSamplers.Begin()+index );
  mConnectionObservers.ConnectionsChanged( *this );
}

void Material::SetTextureImage( size_t index, ResourceId id )
{
  mMaterialChanged = true;
  mTextureId[index] = id;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::SetTextureSampler( size_t index, Render::Sampler* sampler)
{
  // sampler does not change material blending of readiness
  mSamplers[index] = sampler;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::SetTextureUniformName( size_t index, const std::string& uniformName)
{
  // Uniform name does not change material blending of readiness
  mUniformName[index] = uniformName;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::GetResourcesStatus( bool& resourcesReady, bool& finishedResourceAcquisition )
{
  resourcesReady = mResourcesReady;
  finishedResourceAcquisition = mFinishedResourceAcquisition;
}

void Material::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Material::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Material::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Material::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Material::UniformMappingsChanged( const UniformMap& mappings )
{
  // Our uniform map, or that of one of the watched children has changed.
  // Inform connected observers.
  mConnectionObservers.ConnectedUniformMapChanged();
}

void Material::ConnectionsChanged( PropertyOwner& owner )
{
  // this should happen in the case of shader properties changed
  mMaterialChanged = true;

  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::ConnectedUniformMapChanged( )
{
  mConnectionObservers.ConnectedUniformMapChanged();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
