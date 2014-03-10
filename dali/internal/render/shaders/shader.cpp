//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/render/shaders/shader.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/common/post-process-resource-dispatcher.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/uniform-meta.h>

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

namespace SceneGraph
{

namespace // unnamed namespace
{

// Convert Geometry type bitmask to an array index
unsigned int GetGeometryTypeIndex(GeometryType type)
{
  unsigned int index = Log<GEOMETRY_TYPE_IMAGE>::value;
  if ( type & GEOMETRY_TYPE_IMAGE )
  {
    index = Log<GEOMETRY_TYPE_IMAGE>::value;
  }
  else if ( type & GEOMETRY_TYPE_TEXT )
  {
    index = Log<GEOMETRY_TYPE_TEXT>::value;
  }
  else if ( type & GEOMETRY_TYPE_MESH )
  {
    index = Log<GEOMETRY_TYPE_MESH>::value;
  }
  else if ( type & GEOMETRY_TYPE_TEXTURED_MESH )
  {
    index = Log<GEOMETRY_TYPE_TEXTURED_MESH>::value;
  }
  return index;
}

} // unnamed namespace




Shader::Shader( Dali::ShaderEffect::GeometryHints& hints )
: mGeometryHints( hints ),
  mGridDensity( Dali::ShaderEffect::DEFAULT_GRID_DENSITY ),
  mTexture( NULL ),
  mRenderTextureId( 0 ),
  mUpdateTextureId( 0 ),
  mModelViewProjection( false ), // Don't initialize.
  mRenderQueue(NULL),
  mPostProcessDispatcher(NULL),
  mTextureCache(NULL),
  mFrametime(0.f)
{
  // Create enough size for all default types and sub-types
  mPrograms.resize(Log<GEOMETRY_TYPE_LAST>::value);
  for( unsigned int i = 0; i < Log<GEOMETRY_TYPE_LAST>::value; ++i)
  {
    mPrograms[ i ].Resize(SHADER_SUBTYPE_LAST);
  }
}

Shader::~Shader()
{
}

void Shader::Initialize( PostProcessResourceDispatcher& postProcessDispatcher, RenderQueue& renderQueue, TextureCache& textureCache )
{
  mPostProcessDispatcher = &postProcessDispatcher;
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
  // (Maintains thread safety on std::vector)

  typedef MessageValue1< Shader, UniformMeta* > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::InstallUniformMetaInRender, meta );
}

void Shader::ForwardGridDensity( BufferIndex updateBufferIndex, float density )
{
  typedef MessageValue1< Shader, float > DerivedType;

  // Reserve some memory inside the render queue
  unsigned int* slot = mRenderQueue->ReserveMessageSlot( updateBufferIndex, sizeof( DerivedType ) );

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new (slot) DerivedType( this, &Shader::SetGridDensity, density );
}

void Shader::ForwardHints( BufferIndex updateBufferIndex, int hint )
{
  typedef MessageValue1< Shader, int > DerivedType;

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

void Shader::SetProgram( GeometryType geometryType,
                         ShaderSubTypes subType,
                         Integration::ResourceId resourceId,
                         Integration::ShaderDataPtr shaderData,
                         Context* context )
{
  DALI_LOG_TRACE_METHOD_FMT(Debug::Filter::gShader, "%d %d\n", (int)geometryType, resourceId);

  bool precompiledBinary = shaderData->HasBinary();

  Program* program = Program::New( resourceId, shaderData.Get(), *context );

  ShaderSubTypes theSubType = subType;
  if( subType == SHADER_SUBTYPE_ALL )
  {
    theSubType = SHADER_DEFAULT;
  }

  if(geometryType != GEOMETRY_TYPE_TEXT && subType == SHADER_SUBTYPE_ALL)
  {
    mPrograms[GetGeometryTypeIndex(geometryType)].Resize(1);
    mPrograms[GetGeometryTypeIndex(geometryType)][theSubType] = program;
    mPrograms[GetGeometryTypeIndex(geometryType)].mUseDefaultForAllSubtypes = true;
  }
  else
  {
    mPrograms[GetGeometryTypeIndex(geometryType)][theSubType] = program;
    mPrograms[GetGeometryTypeIndex(geometryType)].mUseDefaultForAllSubtypes = false;
  }

  if( !precompiledBinary )
  {
    // The binary will have been compiled/linked during Program::New(), so save it
    if( shaderData->HasBinary() )
    {
      DALI_ASSERT_DEBUG( mPostProcessDispatcher != NULL );
      ResourcePostProcessRequest request( resourceId, ResourcePostProcessRequest::SAVE );
      mPostProcessDispatcher->DispatchPostProcessRequest( request );
    }
  }
}

bool Shader::AreSubtypesRequired(GeometryType geometryType)
{
  DALI_ASSERT_DEBUG(geometryType < GEOMETRY_TYPE_LAST);
  unsigned int programType = GetGeometryTypeIndex( geometryType );

  return ! mPrograms[ programType ].mUseDefaultForAllSubtypes;
}

void Shader::GlCleanup()
{
  mPrograms.clear();
}

Program& Shader::Apply( Context& context,
                        BufferIndex bufferIndex,
                        GeometryType type,
                        const Matrix& model,
                        const Matrix& view,
                        const Matrix& modelview,
                        const Matrix& projection,
                        const Vector4& color,
                        const ShaderSubTypes subType /*= SHADER_DEFAULT*/ )
{
  DALI_ASSERT_DEBUG(type < GEOMETRY_TYPE_LAST);
  DALI_DEBUG_OSTREAM(debugStream);

  if( mRenderTextureId && ( mTexture == NULL ) )
  {
    mTexture = mTextureCache->GetTexture( mRenderTextureId );

    DALI_ASSERT_DEBUG( mTexture != NULL );
  }

  unsigned int programType = GetGeometryTypeIndex( type );

  DALI_ASSERT_DEBUG(!mPrograms[ programType ].mUseDefaultForAllSubtypes || subType == SHADER_DEFAULT);
  DALI_ASSERT_DEBUG((unsigned int)subType < mPrograms[ programType ].Count());
  DALI_ASSERT_DEBUG(NULL != mPrograms[ programType ][ subType ]);

  Program& program = *(mPrograms[ programType ][ subType ]);
  program.Use();

  // Ignore missing uniforms - custom shaders and flat color shaders don't have SAMPLER

  // get color uniform
  const GLint colorLoc = program.GetUniformLocation( Program::UNIFORM_COLOR );
  if( Program::UNIFORM_UNKNOWN != colorLoc )
  {
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uColor", color );
    // set the uniform
    program.SetUniform4f( colorLoc, color.r, color.g, color.b, color.a );
  }

  GLint loc = Program::UNIFORM_UNKNOWN;

  if( mTexture )
  {
    // got effect texture, bind it to texture unit 1
    mTexture->Bind( GL_TEXTURE_2D, GL_TEXTURE1 );
    // get effect sampler uniform
    const GLint loc = program.GetUniformLocation( Program::UNIFORM_EFFECT_SAMPLER );
    if( Program::UNIFORM_UNKNOWN != loc )
    {
      DALI_PRINT_UNIFORM( debugStream, bufferIndex, "sEffect", 1 );
      // set the uniform
      program.SetUniform1i( loc, 1 );
    }
  }

  // Pass ModelView, projection, MVP, and normal matrices to the shader
  loc = program.GetUniformLocation(Program::UNIFORM_MODELVIEW_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uModelView", modelview );
    program.SetUniformMatrix4fv( loc, 1, modelview.AsFloat() );
  }

  loc = program.GetUniformLocation(Program::UNIFORM_PROJECTION_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uProjection", projection );
    program.SetUniformMatrix4fv( loc, 1, projection.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_MVP_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    Matrix::Multiply( mModelViewProjection, modelview, projection );
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uMvpMatrix", mModelViewProjection );
    program.SetUniformMatrix4fv( loc, 1, mModelViewProjection.AsFloat() );
  }

  loc = program.GetUniformLocation( Program::UNIFORM_NORMAL_MATRIX );
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    Matrix3 normalMatrix( modelview );
    normalMatrix.Invert();
    normalMatrix.Transpose();
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uNormalMatrix", normalMatrix );
    program.SetUniformMatrix3fv( loc, 1, normalMatrix.AsFloat() );
  }

  loc = program.GetUniformLocation(Program::UNIFORM_TIME_DELTA);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uTimeDelta", mFrametime );
    program.SetUniform1f( loc, mFrametime );
  }

  loc = program.GetUniformLocation(Program::UNIFORM_MODEL_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uModelMatrix", model );
    program.SetUniformMatrix4fv( loc, 1, model.AsFloat() );
  }

  loc = program.GetUniformLocation(Program::UNIFORM_VIEW_MATRIX);
  if( Program::UNIFORM_UNKNOWN != loc )
  {
    DALI_PRINT_UNIFORM( debugStream, bufferIndex, "uViewMatrix", view );
    program.SetUniformMatrix4fv( loc, 1, view.AsFloat() );
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
      if( 0 == metadata.cacheIndeces[ programType ][ subType ] )
      {
        // register cacheindex for this program
        metadata.cacheIndeces[ programType ][ subType ] = program.RegisterUniform( metadata.name );
      }
      loc = program.GetUniformLocation( metadata.cacheIndeces[ programType ][ subType ] );

      // if we find uniform with location
      if ( Program::UNIFORM_UNKNOWN != loc )
      {
        DALI_PRINT_CUSTOM_UNIFORM( debugStream, bufferIndex, metadata.name, property );

        // switch based on property type to use correct GL uniform setter
        switch ( property.GetType() )
        {
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
              case Dali::ShaderEffect::COORDINATE_TYPE_TEXTURE_POSITION :
              {
                if ( mTexture )
                {
                  UvRect textureArea;
                  mTexture->GetTextureCoordinates( textureArea );

                  //TODO: this only works for textures that are mapped as a axis aligned rectangle
                  float width = textureArea.u2 - textureArea.u0;
                  float height = textureArea.v2 - textureArea.v0;
                  value.x = textureArea.u0 + value.x * width;
                  value.y = textureArea.v0 + value.y * height;
                }
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

          default :
          {
            // Only float and Vector properties are passed as uniforms; other types are ignored.
            break;
          }
        }
      }
    }
  }

  DALI_PRINT_SHADER_UNIFORMS(debugStream);

  return program;
}

void Shader::SetFrameTime( float frametime )
{
  mFrametime = frametime;
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
