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
#include <dali/internal/render/shaders/program.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/shader-data.h>

namespace Dali
{

namespace Internal
{

// LOCAL STUFF
namespace
{

const char* gStdAttribs[ Program::ATTRIB_TYPE_LAST ] =
{
  "aPosition",    // ATTRIB_POSITION
  "aNormal",      // ATTRIB_NORMAL
  "aTexCoord",    // ATTRIB_TEXCOORD
  "aColor",       // ATTRIB_COLOR
  "aBoneWeights", // ATTRIB_BONE_WEIGHTS
  "aBoneIndices"  // ATTRIB_BONE_INDICES
};

const char* gStdUniforms[ Program::UNIFORM_TYPE_LAST ] =
{
  "uMvpMatrix",           // UNIFORM_MVP_MATRIX
  "uModelView",           // UNIFORM_MODELVIEW_MATRIX
  "uProjection",          // UNIFORM_PROJECTION_MATRIX
  "uModelMatrix",         // UNIFORM_MODEL_MATRIX,
  "uViewMatrix",          // UNIFORM_VIEW_MATRIX,
  "uNormalMatrix",        // UNIFORM_NORMAL_MATRIX
  "uColor",               // UNIFORM_COLOR
  "uCustomTextureCoords", // UNIFORM_CUSTOM_TEXTURE_COORDS
  "sTexture",             // UNIFORM_SAMPLER
  "sTextureRect",         // UNIFORM_SAMPLER_RECT
  "sEffect",              // UNIFORM_EFFECT_SAMPLER
  "sEffectRect",          // UNIFORM_EFFECT_SAMPLER_RECT
  "uTimeDelta",           // UNIFORM_TIME_DELTA
  "sOpacityTexture",      // UNIFORM_SAMPLER_OPACITY
  "sNormalMapTexture",    // UNIFORM_SAMPLER_NORMAL_MAP
  "uTextColor",           // UNIFORM_TEXT_COLOR
  "uSmoothing",           // UNIFORM_SMOOTHING
  "uOutline",             // UNIFORM_OUTLINE
  "uOutlineColor",        // UNIFORM_OUTLINE_COLOR
  "uGlow",                // UNIFORM_GLOW
  "uGlowColor",           // UNIFORM_GLOW_COLOR
  "uShadow",              // UNIFORM_SHADOW
  "uShadowColor",         // UNIFORM_SHADOW_COLOR
  "uShadowSmoothing",     // UNIFORM_SHADOW_SMOOTHING
  "uGradientColor",       // UNIFORM_GRADIENT_COLOR
  "uGradientLine",        // UNIFORM_GRADIENT_LINE
  "uInvTextSize"          // UNIFORM_INVERSE_TEXT_SIZE
};

}  // <unnamed> namespace

// IMPLEMENTATION

Program* Program::New( const Integration::ResourceId& resourceId, Integration::ShaderData* shaderData, Context& context )
{
  size_t shaderHash = shaderData->GetHashValue();
  Program* program = context.GetCachedProgram( shaderHash );

  if( NULL == program )
  {
    // program not found so create it
    program = new Program( shaderData, context );

    program->Load();

    // tell context to cache it
    context.CacheProgram( shaderHash, program );
  }

  return program;
}

void Program::Use()
{
  if ( !mLinked &&
       mContext.IsGlContextCreated() )
  {
    Load();
  }

  if ( mLinked )
  {
    if ( this != mContext.GetCurrentProgram() )
    {
      LOG_GL( "UseProgram(%d)\n", mProgramId );
      CHECK_GL( mContext, mGlAbstraction.UseProgram(mProgramId) );
      INCREASE_COUNTER(PerformanceMonitor::SHADER_STATE_CHANGES);

      mContext.SetCurrentProgram( this );
    }
  }
}

bool Program::IsUsed()
{
  return ( this == mContext.GetCurrentProgram() );
}

GLint Program::GetAttribLocation( AttribType type )
{
  DALI_ASSERT_DEBUG(type != ATTRIB_UNKNOWN);

  if( mAttribLocations[ type ] == ATTRIB_UNKNOWN )
  {
    LOG_GL( "GetAttribLocation(program=%d,%s) = %d\n", mProgramId, gStdAttribs[type], mAttribLocations[type] );
    GLint loc = CHECK_GL( mContext, mGlAbstraction.GetAttribLocation( mProgramId, gStdAttribs[type] ) );
    mAttribLocations[ type ] = loc;
  }

  return mAttribLocations[type];
}

unsigned int Program::RegisterUniform( const std::string& name )
{
  unsigned int index = 0;
  // find the value from cache
  for( ;index < mUniformLocations.size(); ++index )
  {
    if( mUniformLocations[ index ].first == name )
    {
      // name found so return index
      return index;
    }
  }
  // if we get here, index is one past end so push back the new name
  mUniformLocations.push_back( std::make_pair( name, UNIFORM_NOT_QUERIED ) );
  return index;
}

GLint Program::GetUniformLocation( unsigned int uniformIndex )
{
  // debug check that index is within name cache
  DALI_ASSERT_DEBUG( mUniformLocations.size() > uniformIndex );

  // check if we have already queried the location of the uniform
  GLint location = mUniformLocations[ uniformIndex ].second;

  if( location == UNIFORM_NOT_QUERIED )
  {
    LOG_GL( "GetUniformLocation(program=%d,%s) = %d\n", mProgramId, mUniformLocations[ uniformIndex ].first.c_str(), mUniformLocations[ uniformIndex ].second );
    location = CHECK_GL( mContext, mGlAbstraction.GetUniformLocation( mProgramId, mUniformLocations[ uniformIndex ].first.c_str() ) );

    mUniformLocations[ uniformIndex ].second = location;
  }

  return location;
}

void Program::SetUniform1i( GLint location, GLint value0 )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }

  // check if uniform location fits the cache
  if( location >= MAX_UNIFORM_CACHE_SIZE )
  {
    // not cached, make the gl call through context
    LOG_GL( "Uniform1i(%d,%d)\n", location, value0 );
    CHECK_GL( mContext, mGlAbstraction.Uniform1i( location, value0 ) );
  }
  else
  {
    // check if the value is different from what's already been set
    if( value0 != mUniformCacheInt[ location ] )
    {
      // make the gl call through context
      LOG_GL( "Uniform1i(%d,%d)\n", location, value0 );
      CHECK_GL( mContext, mGlAbstraction.Uniform1i( location, value0 ) );
      // update cache
      mUniformCacheInt[ location ] = value0;
    }
  }
}

void Program::SetUniform4i( GLint location, GLint value0, GLint value1, GLint value2, GLint value3 )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }

  // Not caching these as based on current analysis this is not called that often by our shaders
  LOG_GL( "Uniform4i(%d,%d,%d,%d,%d)\n", location, value0, value1, value2, value3 );
  CHECK_GL( mContext, mGlAbstraction.Uniform4i( location, value0, value1, value2, value3 ) );
}

void Program::SetUniform1f( GLint location, GLfloat value0 )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }

  // check if uniform location fits the cache
  if( location >= MAX_UNIFORM_CACHE_SIZE )
  {
    // not cached, make the gl call through context
    LOG_GL( "Uniform1f(%d,%f)\n", location, value0 );
    CHECK_GL( mContext, mGlAbstraction.Uniform1f( location, value0 ) );
  }
  else
  {
    // check if the same value has already been set, reset if it is different
    if( ( fabsf(value0 - mUniformCacheFloat[ location ]) >= Math::MACHINE_EPSILON_1 ) )
    {
      // make the gl call through context
      LOG_GL( "Uniform1f(%d,%f)\n", location, value0 );
      CHECK_GL( mContext, mGlAbstraction.Uniform1f( location, value0 ) );

      // update cache
      mUniformCacheFloat[ location ] = value0;
    }
  }
}

void Program::SetUniform2f( GLint location, GLfloat value0, GLfloat value1 )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }

  // Not caching these as based on current analysis this is not called that often by our shaders
  LOG_GL( "Uniform2f(%d,%f,%f)\n", location, value0, value1 );
  CHECK_GL( mContext, mGlAbstraction.Uniform2f( location, value0, value1 ) );
}

void Program::SetUniform3f( GLint location, GLfloat value0, GLfloat value1, GLfloat value2 )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }

  // Not caching these as based on current analysis this is not called that often by our shaders
  LOG_GL( "Uniform3f(%d,%f,%f,%f)\n", location, value0, value1, value2 );
  CHECK_GL( mContext, mGlAbstraction.Uniform3f( location, value0, value1, value2 ) );
}

void Program::SetUniform4f( GLint location, GLfloat value0, GLfloat value1, GLfloat value2, GLfloat value3 )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }

  // check if uniform location fits the cache
  if( location >= MAX_UNIFORM_CACHE_SIZE )
  {
    // not cached, make the gl call through context
    LOG_GL( "Uniform4f(%d,%f,%f,%f,%f)\n", location, value0, value1, value2, value3 );
    CHECK_GL( mContext, mGlAbstraction.Uniform4f( location, value0, value1, value2, value3 ) );
  }
  else
  {
    // check if the same value has already been set, reset if any component is different
    // checking index 3 first because we're often animating alpha (rgba)
    if( ( fabsf(value3 - mUniformCacheFloat4[ location ][ 3 ]) >= Math::MACHINE_EPSILON_1 )||
        ( fabsf(value0 - mUniformCacheFloat4[ location ][ 0 ]) >= Math::MACHINE_EPSILON_1 )||
        ( fabsf(value1 - mUniformCacheFloat4[ location ][ 1 ]) >= Math::MACHINE_EPSILON_1 )||
        ( fabsf(value2 - mUniformCacheFloat4[ location ][ 2 ]) >= Math::MACHINE_EPSILON_1 ) )
    {
      // make the gl call through context
      LOG_GL( "Uniform4f(%d,%f,%f,%f,%f)\n", location, value0, value1, value2, value3 );
      CHECK_GL( mContext, mGlAbstraction.Uniform4f( location, value0, value1, value2, value3 ) );
      // update cache
      mUniformCacheFloat4[ location ][ 0 ] = value0;
      mUniformCacheFloat4[ location ][ 1 ] = value1;
      mUniformCacheFloat4[ location ][ 2 ] = value2;
      mUniformCacheFloat4[ location ][ 3 ] = value3;
    }
  }
}

void Program::SetUniformMatrix4fv( GLint location, GLsizei count, const GLfloat* value )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }


  // Not caching these calls. Based on current analysis this is called very often
  // but with different values (we're using this for MVP matrices)
  // NOTE! we never want GPU to transpose
  LOG_GL( "UniformMatrix4fv(%d,%d,GL_FALSE,%x)\n", location, count, value );
  CHECK_GL( mContext, mGlAbstraction.UniformMatrix4fv( location, count, GL_FALSE, value ) );
}

void Program::SetUniformMatrix3fv( GLint location, GLsizei count, const GLfloat* value )
{
  DALI_ASSERT_DEBUG( IsUsed() ); // should not call this if this program is not used

  if( UNIFORM_UNKNOWN == location )
  {
    // From http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml : Notes
    // If location is equal to UNIFORM_UNKNOWN, the data passed in will be silently ignored and the
    // specified uniform variable will not be changed.following opengl silently do nothing
    return;
  }


  // Not caching these calls. Based on current analysis this is called very often
  // but with different values (we're using this for MVP matrices)
  // NOTE! we never want GPU to transpose
  LOG_GL( "UniformMatrix3fv(%d,%d,GL_FALSE,%x)\n", location, count, value );
  CHECK_GL( mContext, mGlAbstraction.UniformMatrix3fv( location, count, GL_FALSE, value ) );
}

void Program::GlContextCreated()
{
  if (!mLinked)
  {
    Load();
  }
}

void Program::GlContextToBeDestroyed()
{
  Unload();

  mVertexShaderId = 0;
  mFragmentShaderId = 0;
  mProgramId = 0;
  mContext.SetCurrentProgram( NULL );

  ResetAttribsUniforms();
}

Program::Program(Integration::ShaderData* shaderData, Context& context )
: mContext( context ),
  mGlAbstraction( context.GetAbstraction() ),
  mLinked( false ),
  mVertexShaderId( 0 ),
  mFragmentShaderId( 0 ),
  mProgramId( 0 ),
  mProgramData(shaderData)
{
  // reserve space for standard uniforms
  mUniformLocations.reserve( UNIFORM_TYPE_LAST );
  // reset built in uniform names in cache
  for( int i = 0; i < UNIFORM_TYPE_LAST; ++i )
  {
    RegisterUniform( gStdUniforms[ i ] );
  }
  // reset values
  ResetAttribsUniforms();

  mContext.AddObserver( *this );
}

Program::~Program()
{
  mContext.RemoveObserver( *this );

  Unload(); // Resets gCurrentProgram
}

void Program::Load()
{
  DALI_ASSERT_ALWAYS( NULL != mProgramData && "Program data is not initialized" );

  Unload();

  LOG_GL( "CreateProgram()\n" );
  mProgramId = CHECK_GL( mContext, mGlAbstraction.CreateProgram() );

  GLint linked = GL_FALSE;

  // ShaderData contains compiled bytecode?
  if( 0 != mContext.CachedNumberOfProgramBinaryFormats() && mProgramData->HasBinary() )
  {
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Program::Load() - Using Compiled Shader, Size = %d\n", mProgramData->buffer.size());

    CHECK_GL( mContext, mGlAbstraction.ProgramBinary(mProgramId, mContext.CachedProgramBinaryFormat(), mProgramData->buffer.data(), mProgramData->buffer.size()) );

    CHECK_GL( mContext, mGlAbstraction.ValidateProgram(mProgramId) );

    GLint success;
    CHECK_GL( mContext, mGlAbstraction.GetProgramiv( mProgramId, GL_VALIDATE_STATUS, &success ) );

    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ValidateProgram Status = %d\n", success);

    CHECK_GL( mContext, mGlAbstraction.GetProgramiv( mProgramId, GL_LINK_STATUS, &linked ) );

    if( GL_FALSE == linked )
    {
      DALI_LOG_ERROR("Failed to load program binary \n");

      char* szLog = NULL;
      GLint nLength;
      CHECK_GL( mContext, mGlAbstraction.GetProgramiv( mProgramId, GL_INFO_LOG_LENGTH, &nLength) );
      if(nLength > 0)
      {
        szLog = new char[ nLength ];
        CHECK_GL( mContext, mGlAbstraction.GetProgramInfoLog( mProgramId, nLength, &nLength, szLog ) );
        DALI_LOG_ERROR( "Program Link Error: %s\n", szLog );

        delete [] szLog;
      }
    }
    else
    {
      mLinked = true;
    }
  }

  // Fall back to compiling and linking the vertex and fragment sources
  if( GL_FALSE == linked )
  {
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Program::Load() - Runtime compilation\n");
    if( CompileShader( GL_VERTEX_SHADER, mVertexShaderId, mProgramData->vertexShader.c_str() ) )
    {
      if( CompileShader( GL_FRAGMENT_SHADER, mFragmentShaderId, mProgramData->fragmentShader.c_str() ) )
      {
        Link();

        if( mLinked )
        {
          GLint  binaryLength = 0;
          GLenum binaryFormat;

          CHECK_GL( mContext, mGlAbstraction.GetProgramiv(mProgramId, GL_PROGRAM_BINARY_LENGTH_OES, &binaryLength) );
          DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Program::Load() - GL_PROGRAM_BINARY_LENGTH_OES: %d\n", binaryLength);

          // Allocate space for the bytecode in ShaderData
          mProgramData->AllocateBuffer(binaryLength);
          // Copy the bytecode to ShaderData
          CHECK_GL( mContext, mGlAbstraction.GetProgramBinary(mProgramId, binaryLength, NULL, &binaryFormat, mProgramData->buffer.data()) );
        }
      }
    }
  }

  // No longer needed
  FreeShaders();

}

void Program::Unload()
{
  FreeShaders();

  if( this == mContext.GetCurrentProgram() )
  {
    CHECK_GL( mContext, mGlAbstraction.UseProgram(0) );

    mContext.SetCurrentProgram( NULL );
  }

  if (mProgramId)
  {
    LOG_GL( "DeleteProgram(%d)\n", mProgramId );
    CHECK_GL( mContext, mGlAbstraction.DeleteProgram( mProgramId ) );
    mProgramId = 0;
  }

  mLinked = false;

}

bool Program::CompileShader( GLenum shaderType, GLuint& shaderId, const char* src )
{
  if (!shaderId)
  {
    LOG_GL( "CreateShader(%d)\n", shaderType );
    shaderId = CHECK_GL( mContext, mGlAbstraction.CreateShader( shaderType ) );
    LOG_GL( "AttachShader(%d,%d)\n", mProgramId, shaderId );
    CHECK_GL( mContext, mGlAbstraction.AttachShader( mProgramId, shaderId ) );
  }

  LOG_GL( "ShaderSource(%d)\n", shaderId );
  CHECK_GL( mContext, mGlAbstraction.ShaderSource(shaderId, 1, &src, NULL ) );

  LOG_GL( "CompileShader(%d)\n", shaderId );
  CHECK_GL( mContext, mGlAbstraction.CompileShader( shaderId ) );

  GLint compiled;
  LOG_GL( "GetShaderiv(%d)\n", shaderId );
  CHECK_GL( mContext, mGlAbstraction.GetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled ) );

  if (compiled == GL_FALSE)
  {
    DALI_LOG_ERROR("Failed to compiler shader \n%s\n", src);
    std::vector< char > szLog;
    GLint nLength;
    mGlAbstraction.GetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &nLength);
    if(nLength > 0)
    {
      szLog.reserve( nLength );
      mGlAbstraction.GetShaderInfoLog( shaderId, nLength, &nLength, &szLog[ 0 ] );
      DALI_LOG_ERROR( "Shader Compiler Error: %s\n", &szLog[ 0 ] );
    }

    throw DaliException( "Shader compilation failure", &szLog[ 0 ] );
  }

  return compiled != 0;
}

void Program::Link()
{
  LOG_GL( "LinkProgram(%d)\n", mProgramId );
  CHECK_GL( mContext, mGlAbstraction.LinkProgram( mProgramId ) );

  GLint linked;
  LOG_GL( "GetProgramiv(%d)\n", mProgramId );
  CHECK_GL( mContext, mGlAbstraction.GetProgramiv( mProgramId, GL_LINK_STATUS, &linked ) );

  if (linked == GL_FALSE)
  {
    DALI_LOG_ERROR("Shader failed to link \n");

    char* szLog = NULL;
    GLint nLength;
    mGlAbstraction.GetProgramiv( mProgramId, GL_INFO_LOG_LENGTH, &nLength);
    if(nLength > 0)
    {
      szLog = new char[ nLength ];
      mGlAbstraction.GetProgramInfoLog( mProgramId, nLength, &nLength, szLog );
      DALI_LOG_ERROR( "Shader Link Error: %s\n", szLog );
      delete [] szLog;
    }

    DALI_ASSERT_DEBUG(0);
  }

  mLinked = linked != GL_FALSE;
}

void Program::FreeShaders()
{
  if (mVertexShaderId)
  {
    LOG_GL( "DeleteShader(%d)\n", mVertexShaderId );
    CHECK_GL( mContext, mGlAbstraction.DetachShader( mProgramId, mVertexShaderId ) );
    CHECK_GL( mContext, mGlAbstraction.DeleteShader( mVertexShaderId ) );
    mVertexShaderId = 0;
  }

  if (mFragmentShaderId)
  {
    LOG_GL( "DeleteShader(%d)\n", mFragmentShaderId );
    CHECK_GL( mContext, mGlAbstraction.DetachShader( mProgramId, mFragmentShaderId ) );
    CHECK_GL( mContext, mGlAbstraction.DeleteShader( mFragmentShaderId ) );
    mFragmentShaderId = 0;
  }
}

void Program::ResetAttribsUniforms()
{
  // reset attribute locations
  for( unsigned i = 0; i < ATTRIB_TYPE_LAST; ++i )
  {
    mAttribLocations[ i ] = ATTRIB_UNKNOWN;
  }

  // reset all gl uniform locations
  for( unsigned int i = 0; i < mUniformLocations.size(); ++i )
  {
    // reset gl program locations and names
    mUniformLocations[ i ].second = UNIFORM_NOT_QUERIED;
  }

  // reset uniform cache
  for( int i = 0; i < MAX_UNIFORM_CACHE_SIZE; ++i )
  {
    // GL initializes uniforms to 0
    mUniformCacheInt[ i ] = 0;
    mUniformCacheFloat[ i ] = 0.0f;
    mUniformCacheFloat4[ i ][ 0 ] = 0.0f;
    mUniformCacheFloat4[ i ][ 1 ] = 0.0f;
    mUniformCacheFloat4[ i ][ 2 ] = 0.0f;
    mUniformCacheFloat4[ i ][ 3 ] = 0.0f;
  }
}

} // namespace Internal

} // namespace Dali
