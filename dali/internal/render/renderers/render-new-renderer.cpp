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

#include "render-new-renderer.h"
#include <dali/devel-api/common/hash.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/event/common/property-input-impl.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/shaders/program.h>


namespace Dali
{
namespace Internal
{
namespace Render
{

NewRenderer* NewRenderer::New( SceneGraph::RenderDataProvider* dataProvider,
                               SceneGraph::RenderGeometry* renderGeometry )
{
  return new NewRenderer( dataProvider, renderGeometry);
}


NewRenderer::NewRenderer( SceneGraph::RenderDataProvider* dataProvider,
                          SceneGraph::RenderGeometry* renderGeometry )
: Renderer(),
  mRenderDataProvider( dataProvider ),
  mRenderGeometry( renderGeometry ),
  mUpdateAttributesLocation( true )
{
}

NewRenderer::~NewRenderer()
{
}

void NewRenderer::SetRenderDataProvider( SceneGraph::RenderDataProvider* dataProvider )
{
  mRenderDataProvider = dataProvider;
  mUpdateAttributesLocation = true;
}

void NewRenderer::SetGeometry( SceneGraph::RenderGeometry* renderGeometry )
{
  mRenderGeometry = renderGeometry;
  mUpdateAttributesLocation = true;
}

// Note - this is currently called from UpdateThread, PrepareRenderInstructions,
// as an optimisation.
// @todo MESH_REWORK Should use Update thread objects only in PrepareRenderInstructions.
bool NewRenderer::RequiresDepthTest() const
{
  if( mRenderGeometry )
  {
    return mRenderGeometry->RequiresDepthTest();
  }

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

bool NewRenderer::IsOutsideClipSpace( Context& context, const Matrix& modelViewProjectionMatrix )
{
  // @todo MESH_REWORK Add clipping
  return false;
}

void NewRenderer::DoSetUniforms( Context& context, BufferIndex bufferIndex, SceneGraph::Shader* shader, Program* program, unsigned int programIndex )
{
  // Do nothing, we're going to set up the uniforms with our own code instead
}

void NewRenderer::DoSetCullFaceMode( Context& context, BufferIndex bufferIndex )
{
}

void NewRenderer::DoSetBlending( Context& context, BufferIndex bufferIndex, bool blend )
{
  context.SetBlend( blend );
  if( blend )
  {
    const SceneGraph::MaterialDataProvider& material = mRenderDataProvider->GetMaterial();

    context.SetCustomBlendColor( material.GetBlendColor( bufferIndex ) );

    // Set blend source & destination factors
    context.BlendFuncSeparate( material.GetBlendSrcFactorRgb( bufferIndex ),
                               material.GetBlendDestFactorRgb( bufferIndex ),
                               material.GetBlendSrcFactorAlpha( bufferIndex ),
                               material.GetBlendDestFactorAlpha( bufferIndex ) );

    // Set blend equations
    context.BlendEquationSeparate( material.GetBlendEquationRgb( bufferIndex ),
                                   material.GetBlendEquationAlpha( bufferIndex ) );
  }
}

void NewRenderer::DoRender( Context& context, SceneGraph::TextureCache& textureCache, const SceneGraph::NodeDataProvider& node, BufferIndex bufferIndex, Program& program, const Matrix& modelViewMatrix, const Matrix& viewMatrix )
{
  BindTextures( textureCache, program );

  SetUniforms( bufferIndex, node, program );

  if( mUpdateAttributesLocation || mRenderGeometry->AttributesChanged() )
  {
    mRenderGeometry->GetAttributeLocationFromProgram( mAttributesLocation, program, bufferIndex );
    mUpdateAttributesLocation = false;
  }

  mRenderGeometry->UploadAndDraw( context, bufferIndex, mAttributesLocation );
}

void NewRenderer::GlContextDestroyed()
{
  mRenderGeometry->GlContextDestroyed();
}

void NewRenderer::GlCleanup()
{
}

void NewRenderer::SetUniforms( BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, Program& program )
{
  // Check if the map has changed
  DALI_ASSERT_DEBUG( mRenderDataProvider && "No Uniform map data provider available" );

  const SceneGraph::UniformMapDataProvider& uniformMapDataProvider = mRenderDataProvider->GetUniformMap();

  if( uniformMapDataProvider.GetUniformMapChanged( bufferIndex ) ||
      node.GetUniformMapChanged(bufferIndex))
  {
    const SceneGraph::CollectedUniformMap& uniformMap = uniformMapDataProvider.GetUniformMap( bufferIndex );
    const SceneGraph::CollectedUniformMap& uniformMapNode = node.GetUniformMap( bufferIndex );

    unsigned int maxMaps = uniformMap.Count() + uniformMapNode.Count();
    mUniformIndexMap.Clear(); // Clear contents, but keep memory if we don't change size
    mTextureIndexMap.Clear();
    mUniformIndexMap.Resize( maxMaps );

    unsigned int mapIndex(0);
    for(; mapIndex < uniformMap.Count() ; ++mapIndex )
    {
      mUniformIndexMap[mapIndex].propertyValue = uniformMap[mapIndex]->propertyPtr;
      mUniformIndexMap[mapIndex].uniformIndex = program.RegisterUniform( uniformMap[mapIndex]->uniformName );
    }

    for( unsigned int nodeMapIndex = 0; nodeMapIndex < uniformMapNode.Count() ; ++nodeMapIndex )
    {
      unsigned int uniformIndex = program.RegisterUniform( uniformMapNode[nodeMapIndex]->uniformName );
      bool found(false);
      for( unsigned int i(0); i<uniformMap.Count(); ++i )
      {
        if( mUniformIndexMap[i].uniformIndex == uniformIndex )
        {
          mUniformIndexMap[i].propertyValue = uniformMapNode[nodeMapIndex]->propertyPtr;
          found = true;
          break;
        }
      }

      if( !found )
      {
        mUniformIndexMap[mapIndex].propertyValue = uniformMapNode[nodeMapIndex]->propertyPtr;
        mUniformIndexMap[mapIndex].uniformIndex = uniformIndex;
        ++mapIndex;
      }
    }

    mUniformIndexMap.Resize( mapIndex );
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
    Vector3 size = node.GetRenderSize( bufferIndex );
    program.SetSizeUniform3f( sizeLoc, size.x, size.y, size.z );
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
  SceneGraph::TextureCache& textureCache,
  Program& program )
{
  int textureUnit = 0;

  const std::vector<Render::Texture>& textures( mRenderDataProvider->GetTextures());
  for( size_t i(0); i<textures.size(); ++i )
  {
    ResourceId textureId = textures[i].GetTextureId();
    Internal::Texture* texture = textureCache.GetTexture( textureId );
    if( texture )
    {
      textureCache.BindTexture( texture, textureId, GL_TEXTURE_2D, (TextureUnit)textureUnit );

      // Set sampler uniform location for the texture
      unsigned int uniformIndex = GetTextureUniformIndex( program, textures[i].GetUniformName() );
      GLint uniformLocation = program.GetUniformLocation( uniformIndex );
      if( Program::UNIFORM_UNKNOWN != uniformLocation )
      {
        program.SetUniform1i( uniformLocation, textureUnit );
      }

      unsigned int samplerBitfield(0);
      const Render::Sampler* sampler( textures[i].GetSampler() );
      if( sampler )
      {
        samplerBitfield = ImageSampler::PackBitfield(
          static_cast< FilterMode::Type >(sampler->GetMinifyFilterMode()),
          static_cast< FilterMode::Type >(sampler->GetMagnifyFilterMode()),
          static_cast< WrapMode::Type >(sampler->GetUWrapMode()),
          static_cast< WrapMode::Type >(sampler->GetVWrapMode())
          );
      }
      else
      {
        samplerBitfield = ImageSampler::DEFAULT_BITFIELD;
      }

      texture->ApplySampler( (TextureUnit)textureUnit, samplerBitfield );

      ++textureUnit;
    }
  }
}

unsigned int NewRenderer::GetTextureUniformIndex( Program& program, const std::string& uniformName )
{
  unsigned int uniformIndex = 0;
  bool found = false;

  size_t uniformNameHash = Dali::CalculateHash( uniformName );
  for( unsigned int i=0; i< mTextureIndexMap.Count(); ++i )
  {
    if( mTextureIndexMap[i].uniformNameHash == uniformNameHash )
    {
      uniformIndex = mTextureIndexMap[i].uniformIndex;
      found = true;
    }
  }

  if( ! found )
  {
    uniformIndex = program.RegisterUniform( uniformName );
    TextureUniformIndexMap textureUniformIndexMap = {uniformNameHash,uniformIndex};
    mTextureIndexMap.PushBack( textureUniformIndexMap );
  }

  return uniformIndex;
}

} // SceneGraph
} // Internal
} // Dali
