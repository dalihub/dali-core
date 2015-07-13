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
#include <dali/internal/render/shaders/scene-graph-shader.h>

// INTERNAL INCLUDES
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/uniform-meta.h>
#include <dali/internal/common/image-sampler.h>

// See render-debug.h
#ifdef DALI_PRINT_RENDER_INFO

#include <sstream>
#define DALI_DEBUG_OSTREAM(streamName) std::stringstream streamName;

#define DALI_PRINT_UNIFORM(streamName,bufferIndex,name,value) \
  { \
    streamName << " " << name << ": " << value; \
  }

#define DALI_PRINT_CUSTOM_UNIFORM(streamName,bufferIndex,name,property) \
  { \
    streamName << " " << name << ": "; \
    property.DebugPrint( streamName, bufferIndex ); \
  }

#define DALI_PRINT_SHADER_UNIFORMS(streamName) \
  { \
    std::string debugString( streamName.str() ); \
    DALI_LOG_RENDER_INFO( "           %s\n", debugString.c_str() ); \
  }

#else // DALI_PRINT_RENDER_INFO

#define DALI_DEBUG_OSTREAM(streamName)
#define DALI_PRINT_UNIFORM(streamName,bufferIndex,name,value)
#define DALI_PRINT_CUSTOM_UNIFORM(streamName,bufferIndex,name,property)
#define DALI_PRINT_SHADER_UNIFORMS(streamName)

#endif // DALI_PRINT_RENDER_INFO

namespace Dali
{

namespace Internal
{

template <> struct ParameterType< Dali::ShaderEffect::GeometryHints> : public BasicType< Dali::ShaderEffect::GeometryHints > {};
template <> struct ParameterType< Dali::ShaderEffect::UniformCoordinateType > : public BasicType< Dali::ShaderEffect::UniformCoordinateType > {};

namespace SceneGraph
{

Shader::Shader( Dali::ShaderEffect::GeometryHints& hints )
: mGeometryHints( hints ),
  mGridDensity( Dali::ShaderEffect::DEFAULT_GRID_DENSITY ),
  mTexture( NULL ),
  mRenderTextureId( 0 ),
  mUpdateTextureId( 0 ),
  mProgram( NULL ),
  mRenderQueue( NULL ),
  mTextureCache( NULL )
{
}

Shader::~Shader()
{
}

void Shader::Initialize( RenderQueue& renderQueue, TextureCache& textureCache )
{
  mRenderQueue = &renderQueue;
  mTextureCache = &textureCache;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following methods are called during UpdateManager::Update()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Shader::ForwardTextureId( BufferIndex updateBufferIndex, ResourceId textureId )
{
  mUpdateTextureId = textureId;

  typedef MessageValue1< Shader, Integration::ResourceId > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetTextureId, textureId );
}

Integration::ResourceId Shader::GetEffectTextureResourceId()
{
  return mUpdateTextureId;
}

void Shader::ForwardUniformMeta( BufferIndex updateBufferIndex, UniformMeta* meta )
{
  // Defer setting uniform metadata until the next Render

  typedef MessageValue1< Shader, UniformMeta* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::InstallUniformMetaInRender, meta );
}

void Shader::ForwardCoordinateType( BufferIndex updateBufferIndex, unsigned int index, Dali::ShaderEffect::UniformCoordinateType type )
{
  // Defer setting uniform coordinate type until the next Render
  typedef MessageValue2< Shader, unsigned int, Dali::ShaderEffect::UniformCoordinateType > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetCoordinateTypeInRender, index, type );
}

void Shader::ForwardGridDensity( BufferIndex updateBufferIndex, float density )
{
  typedef MessageValue1< Shader, float > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetGridDensity, density );
}

void Shader::ForwardHints( BufferIndex updateBufferIndex, Dali::ShaderEffect::GeometryHints hint )
{
  typedef MessageValue1< Shader, Dali::ShaderEffect::GeometryHints > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetGeometryHints, hint );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following methods are called during RenderManager::Render()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Shader::SetTextureId( Integration::ResourceId textureId )
{
  if ( mRenderTextureId != textureId )
  {
    mRenderTextureId = textureId;
    mTexture = NULL;
  }
}

Integration::ResourceId Shader::GetTextureIdToRender()
{
  return mRenderTextureId;
}

void Shader::SetGridDensity( float density )
{
  mGridDensity = density;
}

float Shader::GetGridDensity()
{
  return mGridDensity;
}

void Shader::InstallUniformMetaInRender( UniformMeta* meta )
{
  mUniformMetadata.PushBack( meta );
}

void Shader::SetCoordinateTypeInRender( unsigned int index, Dali::ShaderEffect::UniformCoordinateType type )
{
  DALI_ASSERT_DEBUG( index < mUniformMetadata.Count() );
  mUniformMetadata[ index ]->SetCoordinateType( type );
}

void Shader::SetProgram( Integration::ShaderDataPtr shaderData,
                         ProgramCache* programCache,
                         bool modifiesGeometry )
{
  DALI_LOG_TRACE_METHOD_FMT( Debug::Filter::gShader, "%d\n", shaderData->GetHashValue() );

  mProgram = Program::New( *programCache, shaderData, modifiesGeometry );
  // The program cache owns the Program object so we don't need to worry about this raw allocation here.
}

Program* Shader::GetProgram()
{
  return mProgram;
}

void Shader::SetUniforms( Context& context,
                          Program& program,
                          BufferIndex bufferIndex )
{
  if( mRenderTextureId && ( mTexture == NULL ) )
  {
    mTexture = mTextureCache->GetTexture( mRenderTextureId );

    DALI_ASSERT_DEBUG( mTexture != NULL );
  }

  GLint loc = Program::UNIFORM_UNKNOWN;

  if( mTexture )
  {
    // if effect sampler uniform used by the program ?
    const GLint loc = program.GetUniformLocation( Program::UNIFORM_EFFECT_SAMPLER );
    if( Program::UNIFORM_UNKNOWN != loc )
    {
      // got effect texture, bind it to texture unit 1
      mTextureCache->BindTexture( mTexture, mRenderTextureId, GL_TEXTURE_2D, TEXTURE_UNIT_SHADER);

      // Apply the default sampling options for now
      mTexture->ApplySampler( TEXTURE_UNIT_SHADER, ImageSampler::PackBitfield( FilterMode::DEFAULT, FilterMode::DEFAULT ) );

      DALI_PRINT_UNIFORM( debugStream, bufferIndex, "sEffect", TEXTURE_UNIT_SHADER );
      // set the uniform
      program.SetUniform1i( loc, TEXTURE_UNIT_SHADER );
    }
  }

  // We should have one UniformMeta per uniform property
  for ( unsigned int i = 0u; i < mUniformMetadata.Count(); ++i )
  {
    UniformMeta& metadata = *mUniformMetadata[i];
    const PropertyBase& property = metadata.property;

    // send the updated uniform to the program
    if ( metadata.name.length() > 0 )
    {
      // 0 means program has not got a cache index for this uniform
      if( 0 == metadata.cacheIndex )
      {
        // register cacheindex for this program
        metadata.cacheIndex = program.RegisterUniform( metadata.name );
      }
      loc = program.GetUniformLocation( metadata.cacheIndex );

      // if we find uniform with location
      if ( Program::UNIFORM_UNKNOWN != loc )
      {
        DALI_PRINT_CUSTOM_UNIFORM( debugStream, bufferIndex, metadata.name, property );

        // switch based on property type to use correct GL uniform setter
        switch ( property.GetType() )
        {
          case Property::BOOLEAN :
          {
            program.SetUniform1i( loc, property.GetBoolean( bufferIndex ) );
            break;
          }
          case Property::INTEGER :
          {
            program.SetUniform1i( loc, property.GetInteger( bufferIndex ) );
            break;
          }
          case Property::FLOAT :
          {
            program.SetUniform1f( loc, property.GetFloat( bufferIndex ) );
            break;
          }
          case Property::VECTOR2 :
          {
            Vector2 value( property.GetVector2( bufferIndex ) );

            switch ( metadata.coordinateType )
            {
              case Dali::ShaderEffect::COORDINATE_TYPE_VIEWPORT_POSITION :
              {
                /**
                 * Convert coordinates from viewport to GL view space
                 *
                 * Viewport coordinate
                 * (0,0)
                 *      +-----+
                 *      |     |
                 *      |     |
                 *      +-----+
                 *             (width,height)
                 *
                 * GL view space coordinates
                 * (width/2,-height/2)
                 *      +-----+
                 *      |     |
                 *      |     |
                 *      +-----+
                 *             (-width/2,height/2)
                 **/
                const Rect< int >& viewport = context.GetViewport();
                value.x = viewport.width * 0.5f - value.x;
                value.y = value.y - viewport.height * 0.5f;

                break;
              }
              case Dali::ShaderEffect::COORDINATE_TYPE_VIEWPORT_DIRECTION :
              {
                // Check diagram in COORDINATE_TYPE_VIEWPORT_POSITION
                value.x *= -1.0f;
                break;
              }
              case Dali::ShaderEffect::COORDINATE_TYPE_DEFAULT :
              {
                // nothing to do in this case
                break;
              }
              // no default so compiler will warn if a case is not handled
            }

            program.SetUniform2f( loc, value.x, value.y );
            break;
          }

          case Property::VECTOR3 :
          {
            Vector3 value( property.GetVector3( bufferIndex ) );
            if( Dali::ShaderEffect::COORDINATE_TYPE_VIEWPORT_DIRECTION == metadata.coordinateType)
            {
              value.y *= -1.0f;
            }

            program.SetUniform3f( loc, value.x, value.y, value.z );
            break;
          }

          case Property::VECTOR4 :
          {
            Vector4 value( property.GetVector4( bufferIndex ) );
            if( Dali::ShaderEffect::COORDINATE_TYPE_VIEWPORT_DIRECTION == metadata.coordinateType)
            {
              value.y *= -1.0f;
            }

            program.SetUniform4f( loc, value.x, value.y, value.z, value.w );
            break;
          }

          case Property::MATRIX:
          {
            const Matrix& value = property.GetMatrix(bufferIndex);
            program.SetUniformMatrix4fv(loc, 1, value.AsFloat() );
            break;
          }

          case Property::MATRIX3:
          {
            const Matrix3& value = property.GetMatrix3(bufferIndex);
            program.SetUniformMatrix3fv(loc, 1, value.AsFloat() );
            break;
          }

          case Property::NONE:
          case Property::ROTATION:
          case Property::STRING:
          case Property::RECTANGLE:
          case Property::MAP:
          case Property::ARRAY:
          {
            DALI_LOG_ERROR( "Invalid property type for a uniform" );
            break;
          }
        }
      }
    }
  }

  DALI_PRINT_SHADER_UNIFORMS(debugStream);
}


// Messages

void SetTextureIdMessage( EventThreadServices& eventThreadServices, const Shader& shader, Integration::ResourceId textureId )
{
  typedef MessageDoubleBuffered1< Shader, Integration::ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardTextureId, textureId );
}

void SetGridDensityMessage( EventThreadServices& eventThreadServices, const Shader& shader, float density )
{
  typedef MessageDoubleBuffered1< Shader, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardGridDensity, density );
}

void SetHintsMessage( EventThreadServices& eventThreadServices, const Shader& shader, Dali::ShaderEffect::GeometryHints hint )
{
  typedef MessageDoubleBuffered1< Shader, Dali::ShaderEffect::GeometryHints > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardHints, hint );
}

void InstallUniformMetaMessage( EventThreadServices& eventThreadServices, const Shader& shader, UniformMeta& meta )
{
  typedef MessageDoubleBuffered1< Shader, UniformMeta* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardUniformMeta, &meta );
}

void SetCoordinateTypeMessage( EventThreadServices& eventThreadServices, const Shader& shader, unsigned int index, Dali::ShaderEffect::UniformCoordinateType type )
{
  typedef MessageDoubleBuffered2< Shader, unsigned int, Dali::ShaderEffect::UniformCoordinateType > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::ForwardCoordinateType, index, type );
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
