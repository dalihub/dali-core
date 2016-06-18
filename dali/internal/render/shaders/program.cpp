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
#include <dali/internal/render/shaders/program.h>

// EXTERNAL INCLUDES
#include <iomanip>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/constants.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/shader-data.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/shaders/program-cache.h>
#include <dali/internal/render/gl-resources/gl-call-debug.h>

namespace
{
void LogWithLineNumbers( const char * source )
{
  unsigned int lineNumber = 0u;
  const char *prev = source;
  const char *ptr = prev;

  while( true )
  {
    if(lineNumber > 200u)
    {
      break;
    }
    // seek the next end of line or end of text
    while( *ptr!='\n' && *ptr != '\0' )
    {
      ++ptr;
    }

    std::string line( prev, ptr-prev );
    Dali::Integration::Log::LogMessage(Dali::Integration::Log::DebugError, "%4d %s\n", lineNumber, line.c_str());

    if( *ptr == '\0' )
    {
      break;
    }
    prev = ++ptr;
    ++lineNumber;
  }
}

} //namespace

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
  "aTexCoord",    // ATTRIB_TEXCOORD
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
  "sTexture",             // UNIFORM_SAMPLER
  "sTextureRect",         // UNIFORM_SAMPLER_RECT
  "sEffect",              // UNIFORM_EFFECT_SAMPLER
  "uSize"                 // UNIFORM_SIZE
};

}  // <unnamed> namespace

// IMPLEMENTATION

Program* Program::New( ProgramCache& cache, Internal::ShaderDataPtr shaderData, bool modifiesGeometry )
{
  size_t shaderHash = shaderData->GetHashValue();
  Program* program = cache.GetProgram( shaderHash );

  if( NULL == program )
  {
    // program not found so create it
    program = new Program( cache, shaderData, modifiesGeometry );

    // we want to lazy load programs so dont do a Load yet, it gets done in Use()
    cache.AddProgram( shaderHash, program );
  }

  return program;
}

void Program::Use()
{
  if ( !mLinked )
  {
    Load();
  }

  if ( mLinked )
  {
    if ( this != mCache.GetCurrentProgram() )
    {
      LOG_GL( "UseProgram(%d)\n", mProgramId );
      CHECK_GL( mGlAbstraction, mGlAbstraction.UseProgram(mProgramId) );

      mCache.SetCurrentProgram( this );
    }
  }
}

bool Program::IsUsed()
{
  return ( this == mCache.GetCurrentProgram() );
}

GLint Program::GetAttribLocation( AttribType type )
{
  DALI_ASSERT_DEBUG(type != ATTRIB_UNKNOWN);

  return GetCustomAttributeLocation( type );
}

unsigned int Program::RegisterCustomAttribute( const std::string& name )
{
  unsigned int index = 0;
  // find the value from cache
  for( ;index < mAttributeLocations.size(); ++index )
  {
    if( mAttributeLocations[ index ].first == name )
    {
      // name found so return index
      return index;
    }
  }
  // if we get here, index is one past end so push back the new name
  mAttributeLocations.push_back( std::make_pair( name, ATTRIB_UNKNOWN ) );
  return index;
}

GLint Program::GetCustomAttributeLocation( unsigned int attributeIndex )
{
  // debug check that index is within name cache
  DALI_ASSERT_DEBUG( mAttributeLocations.size() > attributeIndex );

  // check if we have already queried the location of the attribute
  GLint location = mAttributeLocations[ attributeIndex ].second;

  if( location == ATTRIB_UNKNOWN )
  {
    location = CHECK_GL( mGlAbstraction, mGlAbstraction.GetAttribLocation( mProgramId, mAttributeLocations[ attributeIndex ].first.c_str() ) );

    mAttributeLocations[ attributeIndex ].second = location;
    LOG_GL( "GetAttributeLocation(program=%d,%s) = %d\n", mProgramId, mAttributeLocations[ attributeIndex ].first.c_str(), mAttributeLocations[ attributeIndex ].second );
  }

  return location;
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
    location = CHECK_GL( mGlAbstraction, mGlAbstraction.GetUniformLocation( mProgramId, mUniformLocations[ uniformIndex ].first.c_str() ) );

    mUniformLocations[ uniformIndex ].second = location;
    LOG_GL( "GetUniformLocation(program=%d,%s) = %d\n", mProgramId, mUniformLocations[ uniformIndex ].first.c_str(), mUniformLocations[ uniformIndex ].second );
  }

  return location;
}

namespace
{
/**
 * This struct is used to record the position of a uniform declaration
 * within the fragment shader source code.
 */
struct LocationPosition
{
  GLint uniformLocation; ///< The location of the uniform (used as an identifier)
  int characterPosition; ///< the position of the uniform declaration
  LocationPosition( GLint uniformLocation, int characterPosition )
  : uniformLocation(uniformLocation), characterPosition(characterPosition)
  {
  }
};

bool sortByPosition( LocationPosition a, LocationPosition b )
{
  return a.characterPosition < b.characterPosition;
}
}

void Program::GetActiveSamplerUniforms()
{
  GLint numberOfActiveUniforms = -1;
  GLint uniformMaxNameLength=-1;

  mGlAbstraction.GetProgramiv( mProgramId, GL_ACTIVE_UNIFORMS, &numberOfActiveUniforms );
  mGlAbstraction.GetProgramiv( mProgramId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxNameLength );

  std::vector<std::string> samplerNames;
  char name[uniformMaxNameLength+1]; // Allow for null terminator
  std::vector< LocationPosition >  samplerUniformLocations;

  {
    int nameLength = -1;
    int number = -1;
    GLenum type = GL_ZERO;

    for( int i=0; i<numberOfActiveUniforms; ++i )
    {
      mGlAbstraction.GetActiveUniform( mProgramId, (GLuint)i, uniformMaxNameLength,
                                       &nameLength, &number, &type, name );

      if( type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE ) /// Is there a native sampler type?
      {
        GLuint location = mGlAbstraction.GetUniformLocation( mProgramId, name );
        samplerNames.push_back(name);
        samplerUniformLocations.push_back(LocationPosition(location, 0u));
      }
    }
  }

  if( samplerUniformLocations.size() > 1 )
  {
    // Now, re-order according to declaration order in the fragment source.
    std::string fragShader( mProgramData->GetFragmentShader() );
    for( unsigned int i=0; i<samplerUniformLocations.size(); ++i )
    {
      // Better to write own search algorithm that searches for all of
      // the sampler names simultaneously, ensuring only one iteration
      // over fragShader.
      size_t characterPosition = fragShader.find( samplerNames[i] );
      samplerUniformLocations[i].characterPosition = characterPosition;
    }
    std::sort( samplerUniformLocations.begin(), samplerUniformLocations.end(), sortByPosition);
  }

  for( unsigned int i=0; i<samplerUniformLocations.size(); ++i )
  {
    mSamplerUniformLocations.push_back( samplerUniformLocations[i].uniformLocation );
  }
}

bool Program::GetSamplerUniformLocation( unsigned int index, GLint& location  )
{
  bool result = false;
  if( index < mSamplerUniformLocations.size() )
  {
    location = mSamplerUniformLocations[index];
    result = true;
  }
  return result;
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
    // not cached, make the gl call
    LOG_GL( "Uniform1i(%d,%d)\n", location, value0 );
    CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform1i( location, value0 ) );
  }
  else
  {
    // check if the value is different from what's already been set
    if( value0 != mUniformCacheInt[ location ] )
    {
      // make the gl call
      LOG_GL( "Uniform1i(%d,%d)\n", location, value0 );
      CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform1i( location, value0 ) );
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
  CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform4i( location, value0, value1, value2, value3 ) );
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
    // not cached, make the gl call
    LOG_GL( "Uniform1f(%d,%f)\n", location, value0 );
    CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform1f( location, value0 ) );
  }
  else
  {
    // check if the same value has already been set, reset if it is different
    if( ( fabsf(value0 - mUniformCacheFloat[ location ]) >= Math::MACHINE_EPSILON_1 ) )
    {
      // make the gl call
      LOG_GL( "Uniform1f(%d,%f)\n", location, value0 );
      CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform1f( location, value0 ) );

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

  // check if uniform location fits the cache
  if( location >= MAX_UNIFORM_CACHE_SIZE )
  {
    // not cached, make the gl call
    LOG_GL( "Uniform2f(%d,%f,%f)\n", location, value0, value1 );
    CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform2f( location, value0, value1 ) );
  }
  else
  {
    // check if the same value has already been set, reset if it is different
    if( ( fabsf(value0 - mUniformCacheFloat2[ location ][ 0 ]) >= Math::MACHINE_EPSILON_1 )||
        ( fabsf(value1 - mUniformCacheFloat2[ location ][ 1 ]) >= Math::MACHINE_EPSILON_1 ) )
    {
      // make the gl call
      LOG_GL( "Uniform2f(%d,%f,%f)\n", location, value0, value1 );
      CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform2f( location, value0, value1 ) );

      // update cache
      mUniformCacheFloat2[ location ][ 0 ] = value0;
      mUniformCacheFloat2[ location ][ 1 ] = value1;
    }
  }
}

void Program::SetSizeUniform3f( GLint location, GLfloat value0, GLfloat value1, GLfloat value2 )
{
  if( ( fabsf(value0 - mSizeUniformCache.x) >= Math::MACHINE_EPSILON_1 )||
      ( fabsf(value1 - mSizeUniformCache.y) >= Math::MACHINE_EPSILON_1 )||
      ( fabsf(value2 - mSizeUniformCache.z) >= Math::MACHINE_EPSILON_1 ) )
  {
    mSizeUniformCache.x = value0;
    mSizeUniformCache.y = value1;
    mSizeUniformCache.z = value2;
    SetUniform3f( location, value0, value1, value2 );
  }
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
  CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform3f( location, value0, value1, value2 ) );
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
    // not cached, make the gl call
    LOG_GL( "Uniform4f(%d,%f,%f,%f,%f)\n", location, value0, value1, value2, value3 );
    CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform4f( location, value0, value1, value2, value3 ) );
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
      // make the gl call
      LOG_GL( "Uniform4f(%d,%f,%f,%f,%f)\n", location, value0, value1, value2, value3 );
      CHECK_GL( mGlAbstraction, mGlAbstraction.Uniform4f( location, value0, value1, value2, value3 ) );
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
  // NOTE! we never want driver or GPU to transpose
  LOG_GL( "UniformMatrix4fv(%d,%d,GL_FALSE,%x)\n", location, count, value );
  CHECK_GL( mGlAbstraction, mGlAbstraction.UniformMatrix4fv( location, count, GL_FALSE, value ) );
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
  // NOTE! we never want driver or GPU to transpose
  LOG_GL( "UniformMatrix3fv(%d,%d,GL_FALSE,%x)\n", location, count, value );
  CHECK_GL( mGlAbstraction, mGlAbstraction.UniformMatrix3fv( location, count, GL_FALSE, value ) );
}

void Program::GlContextCreated()
{
}

void Program::GlContextDestroyed()
{
  mLinked = false;
  mVertexShaderId = 0;
  mFragmentShaderId = 0;
  mProgramId = 0;

  ResetAttribsUniformCache();
}

bool Program::ModifiesGeometry()
{
  return mModifiesGeometry;
}

Program::Program( ProgramCache& cache, Internal::ShaderDataPtr shaderData, bool modifiesGeometry )
: mCache( cache ),
  mGlAbstraction( mCache.GetGlAbstraction() ),
  mProjectionMatrix( NULL ),
  mViewMatrix( NULL ),
  mLinked( false ),
  mVertexShaderId( 0 ),
  mFragmentShaderId( 0 ),
  mProgramId( 0 ),
  mProgramData(shaderData),
  mModifiesGeometry( modifiesGeometry )
{
  // reserve space for standard attributes
  mAttributeLocations.reserve( ATTRIB_TYPE_LAST );
  for( int i=0; i<ATTRIB_TYPE_LAST; ++i )
  {
    RegisterCustomAttribute( gStdAttribs[i] );
  }

  // reserve space for standard uniforms
  mUniformLocations.reserve( UNIFORM_TYPE_LAST );
  // reset built in uniform names in cache
  for( int i = 0; i < UNIFORM_TYPE_LAST; ++i )
  {
    RegisterUniform( gStdUniforms[ i ] );
  }

  // reset values
  ResetAttribsUniformCache();
}

Program::~Program()
{
  Unload();
}

void Program::Load()
{
  DALI_ASSERT_ALWAYS( NULL != mProgramData.Get() && "Program data is not initialized" );
  DALI_ASSERT_DEBUG( mProgramId == 0 && "mProgramId != 0, so about to leak a GL resource by overwriting it." );

  LOG_GL( "CreateProgram()\n" );
  mProgramId = CHECK_GL( mGlAbstraction, mGlAbstraction.CreateProgram() );

  GLint linked = GL_FALSE;

  const bool binariesSupported = mCache.IsBinarySupported();

  // if shader binaries are supported and ShaderData contains compiled bytecode?
  if( binariesSupported && mProgramData->HasBinary() )
  {
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Program::Load() - Using Compiled Shader, Size = %d\n", mProgramData->GetBufferSize());

    CHECK_GL( mGlAbstraction, mGlAbstraction.ProgramBinary(mProgramId, mCache.ProgramBinaryFormat(), mProgramData->GetBufferData(), mProgramData->GetBufferSize()) );

    CHECK_GL( mGlAbstraction, mGlAbstraction.ValidateProgram(mProgramId) );

    GLint success;
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetProgramiv( mProgramId, GL_VALIDATE_STATUS, &success ) );

    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ValidateProgram Status = %d\n", success);

    CHECK_GL( mGlAbstraction, mGlAbstraction.GetProgramiv( mProgramId, GL_LINK_STATUS, &linked ) );

    if( GL_FALSE == linked )
    {
      DALI_LOG_ERROR("Failed to load program binary \n");

      GLint nLength;
      CHECK_GL( mGlAbstraction, mGlAbstraction.GetProgramiv( mProgramId, GL_INFO_LOG_LENGTH, &nLength) );
      if(nLength > 0)
      {
        Dali::Vector< char > szLog;
        szLog.Reserve( nLength ); // Don't call Resize as we don't want to initialise the data, just reserve a buffer
        CHECK_GL( mGlAbstraction, mGlAbstraction.GetProgramInfoLog( mProgramId, nLength, &nLength, szLog.Begin() ) );
        DALI_LOG_ERROR( "Program Link Error: %s\n", szLog.Begin() );
      }
    }
    else
    {
      mLinked = true;
      DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "Reused binary.\n" );
    }
  }

  // Fall back to compiling and linking the vertex and fragment sources
  if( GL_FALSE == linked )
  {
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Program::Load() - Runtime compilation\n");
    if( CompileShader( GL_VERTEX_SHADER, mVertexShaderId, mProgramData->GetVertexShader() ) )
    {
      if( CompileShader( GL_FRAGMENT_SHADER, mFragmentShaderId, mProgramData->GetFragmentShader() ) )
      {
        Link();

        if( binariesSupported && mLinked )
        {
          GLint  binaryLength = 0;
          GLenum binaryFormat;
          DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "Compiled and linked.\n\nVS:\n%s\nFS:\n%s\n", mProgramData->GetVertexShader(), mProgramData->GetFragmentShader() );

          CHECK_GL( mGlAbstraction, mGlAbstraction.GetProgramiv(mProgramId, GL_PROGRAM_BINARY_LENGTH_OES, &binaryLength) );
          DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Program::Load() - GL_PROGRAM_BINARY_LENGTH_OES: %d\n", binaryLength);
          if( binaryLength > 0 )
          {
            // Allocate space for the bytecode in ShaderData
            mProgramData->AllocateBuffer(binaryLength);
            // Copy the bytecode to ShaderData
            CHECK_GL( mGlAbstraction, mGlAbstraction.GetProgramBinary(mProgramId, binaryLength, NULL, &binaryFormat, mProgramData->GetBufferData()) );
            mCache.StoreBinary( mProgramData );
            DALI_LOG_INFO( Debug::Filter::gShader, Debug::General, "Saved binary.\n" );
          }
        }
      }
    }
  }

  GetActiveSamplerUniforms();

  // No longer needed
  FreeShaders();
}

void Program::Unload()
{
  FreeShaders();

  if( this == mCache.GetCurrentProgram() )
  {
    CHECK_GL( mGlAbstraction, mGlAbstraction.UseProgram(0) );

    mCache.SetCurrentProgram( NULL );
  }

  if (mProgramId)
  {
    LOG_GL( "DeleteProgram(%d)\n", mProgramId );
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteProgram( mProgramId ) );
    mProgramId = 0;
  }

  mLinked = false;

}

bool Program::CompileShader( GLenum shaderType, GLuint& shaderId, const char* src )
{
  if (!shaderId)
  {
    LOG_GL( "CreateShader(%d)\n", shaderType );
    shaderId = CHECK_GL( mGlAbstraction, mGlAbstraction.CreateShader( shaderType ) );
    LOG_GL( "AttachShader(%d,%d)\n", mProgramId, shaderId );
    CHECK_GL( mGlAbstraction, mGlAbstraction.AttachShader( mProgramId, shaderId ) );
  }

  LOG_GL( "ShaderSource(%d)\n", shaderId );
  CHECK_GL( mGlAbstraction, mGlAbstraction.ShaderSource(shaderId, 1, &src, NULL ) );

  LOG_GL( "CompileShader(%d)\n", shaderId );
  CHECK_GL( mGlAbstraction, mGlAbstraction.CompileShader( shaderId ) );

  GLint compiled;
  LOG_GL( "GetShaderiv(%d)\n", shaderId );
  CHECK_GL( mGlAbstraction, mGlAbstraction.GetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled ) );

  if (compiled == GL_FALSE)
  {
    DALI_LOG_ERROR("Failed to compile shader\n");
    LogWithLineNumbers(src);

    GLint nLength;
    mGlAbstraction.GetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &nLength);
    if(nLength > 0)
    {
      Dali::Vector< char > szLog;
      szLog.Reserve( nLength ); // Don't call Resize as we don't want to initialise the data, just reserve a buffer
      mGlAbstraction.GetShaderInfoLog( shaderId, nLength, &nLength, szLog.Begin() );
      DALI_LOG_ERROR( "Shader Compiler Error: %s\n", szLog.Begin() );
    }

    DALI_ASSERT_ALWAYS( 0 && "Shader compilation failure" );
  }

  return compiled != 0;
}

void Program::Link()
{
  LOG_GL( "LinkProgram(%d)\n", mProgramId );
  CHECK_GL( mGlAbstraction, mGlAbstraction.LinkProgram( mProgramId ) );

  GLint linked;
  LOG_GL( "GetProgramiv(%d)\n", mProgramId );
  CHECK_GL( mGlAbstraction, mGlAbstraction.GetProgramiv( mProgramId, GL_LINK_STATUS, &linked ) );

  if (linked == GL_FALSE)
  {
    DALI_LOG_ERROR("Shader failed to link \n");

    GLint nLength;
    mGlAbstraction.GetProgramiv( mProgramId, GL_INFO_LOG_LENGTH, &nLength);
    if(nLength > 0)
    {
      Dali::Vector< char > szLog;
      szLog.Reserve( nLength ); // Don't call Resize as we don't want to initialise the data, just reserve a buffer
      mGlAbstraction.GetProgramInfoLog( mProgramId, nLength, &nLength, szLog.Begin() );
      DALI_LOG_ERROR( "Shader Link Error: %s\n", szLog.Begin() );
    }

    DALI_ASSERT_ALWAYS( 0 && "Shader linking failure" );
  }

  mLinked = linked != GL_FALSE;
}

void Program::FreeShaders()
{
  if (mVertexShaderId)
  {
    LOG_GL( "DeleteShader(%d)\n", mVertexShaderId );
    CHECK_GL( mGlAbstraction, mGlAbstraction.DetachShader( mProgramId, mVertexShaderId ) );
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteShader( mVertexShaderId ) );
    mVertexShaderId = 0;
  }

  if (mFragmentShaderId)
  {
    LOG_GL( "DeleteShader(%d)\n", mFragmentShaderId );
    CHECK_GL( mGlAbstraction, mGlAbstraction.DetachShader( mProgramId, mFragmentShaderId ) );
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteShader( mFragmentShaderId ) );
    mFragmentShaderId = 0;
  }
}

void Program::ResetAttribsUniformCache()
{
  // reset attribute locations
  for( unsigned i = 0; i < mAttributeLocations.size() ; ++i )
  {
    mAttributeLocations[ i ].second = ATTRIB_UNKNOWN;
  }

  // reset all gl uniform locations
  for( unsigned int i = 0; i < mUniformLocations.size(); ++i )
  {
    // reset gl program locations and names
    mUniformLocations[ i ].second = UNIFORM_NOT_QUERIED;
  }

  mSamplerUniformLocations.clear();

  // reset uniform caches
  mSizeUniformCache.x = mSizeUniformCache.y = mSizeUniformCache.z = 0.f;

  for( int i = 0; i < MAX_UNIFORM_CACHE_SIZE; ++i )
  {
    // GL initializes uniforms to 0
    mUniformCacheInt[ i ] = 0;
    mUniformCacheFloat[ i ] = 0.0f;
    mUniformCacheFloat2[ i ][ 0 ] = 0.0f;
    mUniformCacheFloat2[ i ][ 1 ] = 0.0f;
    mUniformCacheFloat4[ i ][ 0 ] = 0.0f;
    mUniformCacheFloat4[ i ][ 1 ] = 0.0f;
    mUniformCacheFloat4[ i ][ 2 ] = 0.0f;
    mUniformCacheFloat4[ i ][ 3 ] = 0.0f;
  }
}

} // namespace Internal

} // namespace Dali
