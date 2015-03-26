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

#include "render-renderer.h"

#include <dali/internal/common/image-sampler.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/render/data-providers/geometry-data-provider.h>
#include <dali/internal/render/data-providers/material-data-provider.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/sampler-data-provider.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/shaders/program.h>


namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

NewRenderer* NewRenderer::New( NodeDataProvider& nodeDataProvider,
                               const UniformMapDataProvider& uniformMapDataProvider,
                               const GeometryDataProvider* geometryDataProvider,
                               const MaterialDataProvider* materialDataProvider)
{
  return new NewRenderer(nodeDataProvider, uniformMapDataProvider, geometryDataProvider, materialDataProvider);
}


NewRenderer::NewRenderer( NodeDataProvider& nodeDataProvider,
                          const UniformMapDataProvider& uniformMapDataProvider,
                          const GeometryDataProvider* geometryDataProvider,
                          const MaterialDataProvider* materialDataProvider)
: Renderer( nodeDataProvider ),
  mUniformMapDataProvider( uniformMapDataProvider ),
  //mShaderDataProvider( shaderDataProvider ), //@todo Add in after merge with parent class
  mMaterialDataProvider( materialDataProvider ),
  mGeometryDataProvider( geometryDataProvider )
{
}

NewRenderer::~NewRenderer()
{
}

// @todo MESH_REWORK Should we consider changing the providers, or should we instead
// create a new renderer when these change?
void NewRenderer::SetGeometryDataProvider( const GeometryDataProvider* geometryDataProvider )
{
  mGeometryDataProvider = geometryDataProvider;
  mRenderGeometry.GeometryUpdated();
}

void NewRenderer::SetMaterialDataProvider( const MaterialDataProvider* materialDataProvider )
{
  mMaterialDataProvider = materialDataProvider;
}


// Note - this is currently called from UpdateThread, PrepareRenderInstructions,
// as an optimisation.
// @todo MESH_REWORK Should use Update thread objects only in PrepareRenderInstructions.
bool NewRenderer::RequiresDepthTest() const
{
  return true;
}

bool NewRenderer::CheckResources()
{
  // Query material to check it has texture pointers & image has size
  // Query geometry to check it has vertex buffers

  // General point though - why would we have a render item in RenderThread with no ready
  // resources in UpdateThread?
  return true;
}

void NewRenderer::ResolveGeometryTypes( BufferIndex bufferIndex, GeometryType& outType, ShaderSubTypes& outSubType )
{
  // @todo MESH_REWORK Remove after merge

  // Do nothing
}

bool NewRenderer::IsOutsideClipSpace( const Matrix& modelMatrix, const Matrix& modelViewProjectionMatrix )
{
  // @todo MESH_REWORK Add clipping
  return false;
}

void NewRenderer::DoSetUniforms( Shader* shader, Context* context, Program* program, BufferIndex bufferIndex, unsigned int programIndex, ShaderSubTypes subType )
{
  // Do nothing, we're going to set up the uniforms with our own code instead
}


void NewRenderer::DoRender( BufferIndex bufferIndex, Program& program, const Matrix& modelViewMatrix, const Matrix& viewMatrix )
{
  BindTextures( bufferIndex, program, mMaterialDataProvider->GetSamplers() );

  SetUniforms( bufferIndex, program );

  mRenderGeometry.UploadAndDraw( mContext, program, bufferIndex, *mGeometryDataProvider );
}

void NewRenderer::GlContextDestroyed()
{
  mRenderGeometry.GlContextDestroyed();
}

void NewRenderer::GlCleanup()
{
}

void NewRenderer::SetUniforms( BufferIndex bufferIndex, Program& program )
{
  // Check if the map has changed
  if( mUniformMapDataProvider.GetUniformMapChanged( bufferIndex ) )
  {
    const CollectedUniformMap& uniformMap = mUniformMapDataProvider.GetUniformMap( bufferIndex );

    unsigned int numberOfMaps = uniformMap.Count();
    mUniformIndexMap.Clear(); // Clear contents, but keep memory if we don't change size
    mUniformIndexMap.Resize( numberOfMaps );

    // Remap uniform indexes to property value addresses

    for( unsigned int mapIndex = 0 ; mapIndex < numberOfMaps ; ++mapIndex )
    {
      mUniformIndexMap[mapIndex].propertyValue = uniformMap[mapIndex]->propertyPtr;
      mUniformIndexMap[mapIndex].uniformIndex = program.RegisterUniform( uniformMap[mapIndex]->uniformName );
    }
  }

  // Set uniforms in local map
  for( UniformIndexMappings::Iterator iter = mUniformIndexMap.Begin(),
         end = mUniformIndexMap.End() ;
       iter != end ;
       ++iter )
  {
    SetUniformFromProperty( bufferIndex, program, *iter );
  }

  // @todo MESH_REWORK On merge, copy code from renderer to setup standard matrices and color

  GLint sizeLoc = program.GetUniformLocation( Program::UNIFORM_SIZE );
  if( -1 != sizeLoc )
  {
    Vector3 size = mDataProvider.GetRenderSize( bufferIndex );
    program.SetUniform3f( sizeLoc, size.x, size.y, size.z );
  }
}

void NewRenderer::SetUniformFromProperty( BufferIndex bufferIndex, Program& program, UniformIndexMap& map )
{
  GLint location = program.GetUniformLocation(map.uniformIndex);
  if( Program::UNIFORM_UNKNOWN != location )
  {
    // switch based on property type to use correct GL uniform setter
    switch ( map.propertyValue->GetType() )
    {
      case Property::INTEGER:
      {
        program.SetUniform1i( location, map.propertyValue->GetInteger( bufferIndex ) );
        break;
      }
      case Property::FLOAT:
      {
        program.SetUniform1f( location, map.propertyValue->GetFloat( bufferIndex ) );
        break;
      }
      case Property::VECTOR2:
      {
        Vector2 value( map.propertyValue->GetVector2( bufferIndex ) );
        program.SetUniform2f( location, value.x, value.y );
        break;
      }

      case Property::VECTOR3:
      {
        Vector3 value( map.propertyValue->GetVector3( bufferIndex ) );
        program.SetUniform3f( location, value.x, value.y, value.z );
        break;
      }

      case Property::VECTOR4:
      {
        Vector4 value( map.propertyValue->GetVector4( bufferIndex ) );
        program.SetUniform4f( location, value.x, value.y, value.z, value.w );
        break;
      }

      case Property::ROTATION:
      {
        Quaternion value( map.propertyValue->GetQuaternion( bufferIndex ) );
        program.SetUniform4f( location, value.mVector.x, value.mVector.y, value.mVector.z, value.mVector.w );
        break;
      }

      case Property::MATRIX:
      {
        const Matrix& value = map.propertyValue->GetMatrix(bufferIndex);
        program.SetUniformMatrix4fv(location, 1, value.AsFloat() );
        break;
      }

      case Property::MATRIX3:
      {
        const Matrix3& value = map.propertyValue->GetMatrix3(bufferIndex);
        program.SetUniformMatrix3fv(location, 1, value.AsFloat() );
        break;
      }

      default:
      {
        // Other property types are ignored
        break;
      }
    }
  }
}

void NewRenderer::BindTextures(
  BufferIndex bufferIndex,
  Program& program,
  const MaterialDataProvider::Samplers& samplers )
{
  // @todo MESH_REWORK Write a cache of texture units to commonly used sampler textures
  unsigned int textureUnit = 0;

  for( MaterialDataProvider::Samplers::Iterator iter = samplers.Begin();
       iter != samplers.End();
       ++iter )
  {
    const SamplerDataProvider* sampler = *iter;
    ResourceId textureId = sampler->GetTextureId(bufferIndex);
    Texture* texture = mTextureCache->GetTexture( textureId );
    if( texture != NULL )
    {
      unsigned int textureUnitUniformIndex = GetTextureUnitUniformIndex( program, *sampler );
      TextureUnit theTextureUnit = static_cast<TextureUnit>(textureUnit);
      BindTexture( program, textureId, texture, theTextureUnit, textureUnitUniformIndex );
      ApplySampler( bufferIndex, texture, theTextureUnit, *sampler );
    }

    ++textureUnit;
  }
}

void NewRenderer::BindTexture(
  Program& program,
  ResourceId id,
  Texture* texture,
  TextureUnit textureUnit,
  unsigned int textureUnitUniformIndex )
{
  DALI_ASSERT_DEBUG( NULL != mTextureCache );

  if( texture != NULL )
  {
    mTextureCache->BindTexture( texture, id, GL_TEXTURE_2D, textureUnit );

    // Set sampler uniform location for the texture
    GLint textureUnitLoc = program.GetUniformLocation( textureUnitUniformIndex );
    if( Program::UNIFORM_UNKNOWN != textureUnitLoc )
    {
      program.SetUniform1i( textureUnitLoc, textureUnit );
    }
  }
}

void NewRenderer::ApplySampler(
  BufferIndex bufferIndex,
  Texture*    texture,
  TextureUnit textureUnit,
  const SamplerDataProvider& sampler )
{
  unsigned int samplerBitfield = ImageSampler::PackBitfield(
    static_cast< FilterMode::Type >(sampler.GetMinifyFilterMode(bufferIndex)),
    static_cast< FilterMode::Type >(sampler.GetMagnifyFilterMode(bufferIndex)) );

  texture->ApplySampler( textureUnit, samplerBitfield );

  // @todo MESH_REWORK add support for wrap modes
}

unsigned int NewRenderer::GetTextureUnitUniformIndex(
  Program& program,
  const SamplerDataProvider& sampler )
{
  // Find sampler in mSamplerNameCache
  // If it doesn't exist,
  //   get the index by calling program.RegisterUniform and store it
  // If it exists, it's index should be set.
  // @todo Cache should be reset on scene change

  unsigned int uniformIndex = 0;
  bool found = false;

  for( unsigned int i=0; i< mTextureUnitUniforms.Count(); ++i )
  {
    if( mTextureUnitUniforms[i].sampler == &sampler )
    {
      uniformIndex = mTextureUnitUniforms[i].index;
      found = true;
    }
  }

  if( ! found )
  {
    TextureUnitUniformIndex textureUnitUniformIndex;
    textureUnitUniformIndex.sampler = &sampler;
    textureUnitUniformIndex.index = program.RegisterUniform( sampler.GetUnitName() );
    mTextureUnitUniforms.PushBack( textureUnitUniformIndex );
    uniformIndex = textureUnitUniformIndex.index;
  }

  return uniformIndex;
}


} // SceneGraph
} // Internal
} // Dali
