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
 *
 */

// CLASS HEADER
#include <dali/internal/render/renderers/render-renderer.h>


// INTERNAL INCLUDES
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/data-providers/node-data-provider.h>
#include <dali/internal/render/data-providers/uniform-name-cache.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/public-api/actors/blending.h>

namespace Dali
{

namespace Internal
{

namespace
{

static Matrix gModelViewProjectionMatrix( false ); ///< a shared matrix to calculate the MVP matrix, dont want to store it in object to reduce storage overhead
static Matrix3 gNormalMatrix; ///< a shared matrix to calculate normal matrix, dont want to store it in object to reduce storage overhead

/**
 * Helper to set view and projection matrices once per program
 * @param program to set the matrices to
 * @param modelMatrix to set
 * @param viewMatrix to set
 * @param projectionMatrix to set
 * @param modelViewMatrix to set
 * @param modelViewProjectionMatrix to set
 */
inline void SetMatrices( Program& program,
                         const Matrix& modelMatrix,
                         const Matrix& viewMatrix,
                         const Matrix& projectionMatrix,
                         const Matrix& modelViewMatrix )
{
  GLint loc = program.GetUniformLocation( Program::UNIFORM_MODEL_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelMatrix.AsFloat() );
  }
  loc = program.GetUniformLocation( Program::UNIFORM_VIEW_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    if( program.GetViewMatrix() != &viewMatrix )
    {
      program.SetViewMatrix( &viewMatrix );
      program.SetUniformMatrix4fv( loc, 1, viewMatrix.AsFloat() );
    }
  }
  // set projection matrix if program has not yet received it this frame or if it is dirty
  loc = program.GetUniformLocation( Program::UNIFORM_PROJECTION_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    if( program.GetProjectionMatrix() != &projectionMatrix )
    {
      program.SetProjectionMatrix( &projectionMatrix );
      program.SetUniformMatrix4fv( loc, 1, projectionMatrix.AsFloat() );
    }
  }
  loc = program.GetUniformLocation(Program::UNIFORM_MODELVIEW_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    program.SetUniformMatrix4fv( loc, 1, modelViewMatrix.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_MVP_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    Matrix::Multiply( gModelViewProjectionMatrix, modelViewMatrix, projectionMatrix );
    program.SetUniformMatrix4fv( loc, 1, gModelViewProjectionMatrix.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_NORMAL_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    gNormalMatrix = modelViewMatrix;
    gNormalMatrix.Invert();
    gNormalMatrix.Transpose();
    program.SetUniformMatrix3fv( loc, 1, gNormalMatrix.AsFloat() );
  }
}

}

namespace Render
{

Renderer* Renderer::New( SceneGraph::RenderDataProvider* dataProvider,
                         SceneGraph::RenderGeometry* renderGeometry,
                         unsigned int blendingBitmask,
                         const Vector4* blendColor,
                         Dali::Renderer::FaceCullingMode faceCullingMode,
                         bool preMultipliedAlphaEnabled )
{
  return new Renderer( dataProvider, renderGeometry, blendingBitmask, blendColor, faceCullingMode, preMultipliedAlphaEnabled );
}

Renderer::Renderer( SceneGraph::RenderDataProvider* dataProvider,
                    SceneGraph::RenderGeometry* renderGeometry,
                    unsigned int blendingBitmask,
                    const Vector4* blendColor,
                    Dali::Renderer::FaceCullingMode faceCullingMode,
                    bool preMultipliedAlphaEnabled)
: mRenderDataProvider( dataProvider ),
  mContext(NULL),
  mTextureCache( NULL ),
  mUniformNameCache( NULL ),
  mRenderGeometry( renderGeometry ),
  mUniformIndexMap(),
  mAttributesLocation(),
  mBlendingOptions(),
  mFaceCullingMode( faceCullingMode  ),
  mSamplerBitfield( ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) ),
  mUpdateAttributesLocation( true ),
  mPremultipledAlphaEnabled( preMultipliedAlphaEnabled )
{
  if(  blendingBitmask != 0u )
  {
    mBlendingOptions.SetBitmask( blendingBitmask );
  }

  if( blendColor )
  {
    mBlendingOptions.SetBlendColor( *blendColor );
  }
}

void Renderer::Initialize( Context& context, SceneGraph::TextureCache& textureCache, Render::UniformNameCache& uniformNameCache )
{
  mContext = &context;
  mTextureCache = &textureCache;
  mUniformNameCache = &uniformNameCache;
}

Renderer::~Renderer()
{
}

void Renderer::SetRenderDataProvider( SceneGraph::RenderDataProvider* dataProvider )
{
  mRenderDataProvider = dataProvider;
  mUpdateAttributesLocation = true;
}

void Renderer::SetGeometry( SceneGraph::RenderGeometry* renderGeometry )
{
  mRenderGeometry = renderGeometry;
  mUpdateAttributesLocation = true;
}

// Note - this is currently called from UpdateThread, PrepareRenderInstructions,
// as an optimisation.
// @todo MESH_REWORK Should use Update thread objects only in PrepareRenderInstructions.
bool Renderer::RequiresDepthTest() const
{
  return mRenderGeometry->RequiresDepthTest();
}

void Renderer::SetBlending( Context& context, bool blend )
{
  context.SetBlend( blend );
  if( blend )
  {
    // Blend color is optional and rarely used
    const Vector4* blendColor = mBlendingOptions.GetBlendColor();
    if( blendColor )
    {
      context.SetCustomBlendColor( *blendColor );
    }
    else
    {
      context.SetDefaultBlendColor();
    }

    // Set blend source & destination factors
    context.BlendFuncSeparate( mBlendingOptions.GetBlendSrcFactorRgb(),
                               mBlendingOptions.GetBlendDestFactorRgb(),
                               mBlendingOptions.GetBlendSrcFactorAlpha(),
                               mBlendingOptions.GetBlendDestFactorAlpha() );

    // Set blend equations
    context.BlendEquationSeparate( mBlendingOptions.GetBlendEquationRgb(),
                                   mBlendingOptions.GetBlendEquationAlpha() );
  }
}

void Renderer::GlContextDestroyed()
{
  mRenderGeometry->GlContextDestroyed();
}

void Renderer::GlCleanup()
{
}

void Renderer::SetUniforms( BufferIndex bufferIndex, const SceneGraph::NodeDataProvider& node, const Vector3& size, Program& program )
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

  GLint sizeLoc = program.GetUniformLocation( Program::UNIFORM_SIZE );
  if( -1 != sizeLoc )
  {
    program.SetSizeUniform3f( sizeLoc, size.x, size.y, size.z );
  }
}

void Renderer::SetUniformFromProperty( BufferIndex bufferIndex, Program& program, UniformIndexMap& map )
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

bool Renderer::BindTextures( SceneGraph::TextureCache& textureCache, Program& program )
{
  int textureUnit = 0;
  bool result = true;

  std::vector<Render::Texture>& textures( mRenderDataProvider->GetTextures() );
  for( size_t i(0); result && i<textures.size(); ++i )
  {
    ResourceId textureId = textures[i].GetTextureId();
    Internal::Texture* texture = textureCache.GetTexture( textureId );
    if( texture )
    {
      result = textureCache.BindTexture( texture, textureId, GL_TEXTURE_2D, (TextureUnit)textureUnit );

      if( result )
      {
        GLint uniformLocation;

        bool result = program.GetSamplerUniformLocation( i, uniformLocation );
        if( result && Program::UNIFORM_UNKNOWN != uniformLocation )
        {
          program.SetUniform1i( uniformLocation, textureUnit );

          unsigned int samplerBitfield(0);
          Render::Texture& textureMapping = textures[i];
          const Render::Sampler* sampler( textureMapping.GetSampler() );
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
  }
  return result;
}

void Renderer::SetFaceCullingMode( Dali::Renderer::FaceCullingMode mode )
{
  mFaceCullingMode =  mode;
}

void Renderer::SetBlendingBitMask( unsigned int bitmask )
{
  mBlendingOptions.SetBitmask( bitmask );
}

void Renderer::SetBlendColor( const Vector4* color )
{
  mBlendingOptions.SetBlendColor( *color );
}

void Renderer::EnablePreMultipliedAlpha( bool enable )
{
  mPremultipledAlphaEnabled = enable;
}

void Renderer::SetSampler( unsigned int samplerBitfield )
{
  mSamplerBitfield = samplerBitfield;
}

void Renderer::Render( Context& context,
                       SceneGraph::TextureCache& textureCache,
                       BufferIndex bufferIndex,
                       const SceneGraph::NodeDataProvider& node,
                       SceneGraph::Shader& defaultShader,
                       const Matrix& modelViewMatrix,
                       const Matrix& viewMatrix,
                       const Matrix& projectionMatrix,
                       const Vector3& size,
                       bool blend )
{
  // Get the program to use:
  Program* program = mRenderDataProvider->GetShader().GetProgram();
  if( !program )
  {
    // if program is NULL it means this is a custom shader with non matching geometry type so we need to use default shaders program
    program = defaultShader.GetProgram();
    DALI_ASSERT_DEBUG( program && "Default shader should always have a program available." );
    if( !program )
    {
      DALI_LOG_ERROR( "Failed to get program for shader at address %p.", (void*)&mRenderDataProvider->GetShader() );
      return;
    }
  }

  //Set cull face  mode
  context.CullFace( mFaceCullingMode );

  //Set blending mode
  SetBlending( context, blend );

  // Take the program into use so we can send uniforms to it
  program->Use();

  if( DALI_LIKELY( BindTextures( textureCache, *program ) ) )
  {
    // Only set up and draw if we have textures and they are all valid

    // set projection and view matrix if program has not yet received them yet this frame
    SetMatrices( *program, node.GetModelMatrix( bufferIndex ), viewMatrix, projectionMatrix, modelViewMatrix );

    // set color uniform
    GLint loc = program->GetUniformLocation( Program::UNIFORM_COLOR );
    if( Program::UNIFORM_UNKNOWN != loc )
    {
      const Vector4& color = node.GetRenderColor( bufferIndex );
      if( mPremultipledAlphaEnabled )
      {
        program->SetUniform4f( loc, color.r*color.a, color.g*color.a, color.b*color.a, color.a );
      }
      else
      {
        program->SetUniform4f( loc, color.r, color.g, color.b, color.a );
      }
    }

  SetUniforms( bufferIndex, node, size, *program );

    if( mUpdateAttributesLocation || mRenderGeometry->AttributesChanged() )
    {
      mRenderGeometry->GetAttributeLocationFromProgram( mAttributesLocation, *program, bufferIndex );
      mUpdateAttributesLocation = false;
    }

    mRenderGeometry->UploadAndDraw( context, bufferIndex, mAttributesLocation );
  }
}

void Renderer::SetSortAttributes( BufferIndex bufferIndex, SceneGraph::RendererWithSortAttributes& sortAttributes ) const
{
  sortAttributes.shader = &( mRenderDataProvider->GetShader() );
  const std::vector<Render::Texture>& textures( mRenderDataProvider->GetTextures() );
  if( !textures.empty() )
  {
    sortAttributes.textureResourceId = textures[0].GetTextureId();
  }
  else
  {
    sortAttributes.textureResourceId = Integration::InvalidResourceId;
  }

  sortAttributes.geometry = mRenderGeometry;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
