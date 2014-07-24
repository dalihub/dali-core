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
#include <dali/internal/render/gl-resources/context.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <limits>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/integration-api/debug.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

/**
 * GL error strings
 */
struct errorStrings
{
  const GLenum errorCode;
  const char* errorString;
};
errorStrings errors[] =
{
   { GL_NO_ERROR,           "GL_NO_ERROR" },
   { GL_INVALID_ENUM,       "GL_INVALID_ENUM" },
   { GL_INVALID_VALUE,      "GL_INVALID_VALUE" },
   { GL_INVALID_OPERATION,  "GL_INVALID_OPERATION" },
   { GL_OUT_OF_MEMORY,      "GL_OUT_OF_MEMORY" }
};

/*
 * Called by std::for_each from ~Context
 */
void deletePrograms(std::pair< std::size_t, Program* > hashProgram)
{
  DALI_ASSERT_DEBUG( hashProgram.second );
  delete hashProgram.second;
}

const unsigned int UNINITIALIZED_TEXTURE_UNIT = std::numeric_limits<unsigned int>::max();// GL_MAX_TEXTURE_UNITS can't be used because it's depreciated in gles2

} // unnamed namespace

#ifdef DEBUG_ENABLED
Debug::Filter* Context::gGlLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_CONTEXT");
#endif

Context::Context(Integration::GlAbstraction& glAbstraction)
: mGlAbstraction(glAbstraction),
  mGlContextCreated(false),
  mColorMask(true),
  mStencilMask(0xFF),
  mBlendEnabled(false),
  mDepthTestEnabled(false),
  mDepthMaskEnabled(false),
  mDitherEnabled(true), // This the only GL capability which defaults to true
  mPolygonOffsetFillEnabled(false),
  mSampleAlphaToCoverageEnabled(false),
  mSampleCoverageEnabled(false),
  mScissorTestEnabled(false),
  mStencilTestEnabled(false),
  mClearColorSet(false),
  mBoundArrayBufferId(0),
  mBoundElementArrayBufferId(0),
  mBoundTransformFeedbackBufferId(0),
  mActiveTextureUnit( UNINITIALIZED_TEXTURE_UNIT ),
  mUsingDefaultBlendColor(true),
  mBlendFuncSeparateSrcRGB(GL_ONE),
  mBlendFuncSeparateDstRGB(GL_ZERO),
  mBlendFuncSeparateSrcAlpha(GL_ONE),
  mBlendFuncSeparateDstAlpha(GL_ZERO),
  mBlendEquationSeparateModeRGB( GL_FUNC_ADD ),
  mBlendEquationSeparateModeAlpha( GL_FUNC_ADD ),
  mMaxTextureSize(0),
  mMaxTextureUnits(0),
  mClearColor(Color::WHITE),    // initial color, never used until it's been set by the user
  mCullFaceMode(CullNone),
  mViewPort( 0, 0, 0, 0 ),
  mCurrentProgram( NULL )
{
}

Context::~Context()
{
  // release the cached programs
  std::for_each(mProgramCache.begin(), mProgramCache.end(), deletePrograms);
  mProgramCache.clear();
}

void Context::GlContextCreated()
{
  DALI_ASSERT_DEBUG(!mGlContextCreated);

  mGlContextCreated = true;

  // Set the initial GL state, and check it.
  ResetGlState();

  const ProgramContainer::iterator endp = mProgramCache.end();
  for ( ProgramContainer::iterator itp = mProgramCache.begin(); itp != endp; ++itp )
  {
    (*itp).second->GlContextCreated();
  }
}

void Context::GlContextDestroyed()
{
  const ProgramContainer::iterator endp = mProgramCache.end();
  for ( ProgramContainer::iterator itp = mProgramCache.begin(); itp != endp; ++itp )
  {
    (*itp).second->GlContextDestroyed();
  }

  mGlContextCreated = false;
}

const char* Context::ErrorToString( GLenum errorCode )
{
  for( unsigned int i = 0; i < sizeof(errors) / sizeof(errors[0]); ++i)
  {
    if (errorCode == errors[i].errorCode)
    {
      return errors[i].errorString;
    }
  }
  return "Unknown Open GLES error";
}

void Context::ResetProgramMatrices()
{
  const ProgramContainer::iterator endp = mProgramCache.end();
  for ( ProgramContainer::iterator itp = mProgramCache.begin(); itp != endp; ++itp )
  {
    (*itp).second->SetProjectionMatrix( NULL );
    (*itp).second->SetViewMatrix( NULL );
  }
}

Program* Context::GetCachedProgram( std::size_t hash ) const
{
  std::map< std::size_t, Program* >::const_iterator iter = mProgramCache.find(hash);

  if (iter != mProgramCache.end())
  {
     return iter->second;
  }
  return NULL;
}

void Context::CacheProgram( std::size_t hash, Program* pointer )
{
  mProgramCache[ hash ] = pointer;
}

const Rect< int >& Context::GetViewport()
{
  return mViewPort;
}

void Context::FlushVertexAttributeLocations()
{
  for( unsigned int i = 0; i < MAX_ATTRIBUTE_CACHE_SIZE; ++i )
  {
    // see if our cached state is different to the actual state
    if (mVertexAttributeCurrentState[ i ] != mVertexAttributeCachedState[ i ] )
    {
      // it's different so make the change to the driver
      // and update the cached state
      mVertexAttributeCurrentState[ i ] = mVertexAttributeCachedState[ i ];

      if (mVertexAttributeCurrentState[ i ] )
      {
        LOG_GL("EnableVertexAttribArray %d\n", i);
        CHECK_GL( *this, mGlAbstraction.EnableVertexAttribArray( i ) );
      }
      else
      {
        LOG_GL("DisableVertexAttribArray %d\n", i);
        CHECK_GL( *this, mGlAbstraction.DisableVertexAttribArray( i ) );
      }
    }
  }

}

void Context::SetVertexAttributeLocation(unsigned int location, bool state)
{

  if( location >= MAX_ATTRIBUTE_CACHE_SIZE )
  {
    // not cached, make the gl call through context
    if ( state )
    {
       LOG_GL("EnableVertexAttribArray %d\n", location);
       CHECK_GL( *this, mGlAbstraction.EnableVertexAttribArray( location ) );
    }
    else
    {
      LOG_GL("DisableVertexAttribArray %d\n", location);
      CHECK_GL( *this, mGlAbstraction.DisableVertexAttribArray( location ) );
    }
  }
  else
  {
    // set the cached state, it will be set at the next draw call
    // if it's different from the current driver state
    mVertexAttributeCachedState[ location ] = state;
  }
}

void Context::ResetVertexAttributeState()
{
  // reset attribute cache
  for( unsigned int i=0; i < MAX_ATTRIBUTE_CACHE_SIZE; ++i )
  {
    mVertexAttributeCachedState[ i ] = false;
    mVertexAttributeCurrentState[ i ] = false;

    LOG_GL("DisableVertexAttribArray %d\n", i);
    CHECK_GL( *this, mGlAbstraction.DisableVertexAttribArray( i ) );
  }
}

void Context::ResetGlState()
{
  DALI_ASSERT_DEBUG(mGlContextCreated);

  mClearColorSet = false;
  // Render manager will call clear in next render

  // Reset internal state and Synchronize it with real OpenGL context.
  // This may seem like overkill, but the GL context is not owned by dali-core,
  // and no assumptions should be made about the initial state.
  mColorMask = true;
  mGlAbstraction.ColorMask( true, true, true, true );

  mStencilMask = 0xFF;
  mGlAbstraction.StencilMask( 0xFF );

  mBlendEnabled = false;
  mGlAbstraction.Disable(GL_BLEND);

  mDepthTestEnabled = false;
  mGlAbstraction.Disable(GL_DEPTH_TEST);

  mDepthMaskEnabled = false;
  mGlAbstraction.DepthMask(GL_FALSE);

  mDitherEnabled = false; // This the only GL capability which defaults to true
  mGlAbstraction.Disable(GL_DITHER);

  mPolygonOffsetFillEnabled = false;
  mGlAbstraction.Disable(GL_POLYGON_OFFSET_FILL);

  mSampleAlphaToCoverageEnabled = false;
  mGlAbstraction.Disable(GL_SAMPLE_ALPHA_TO_COVERAGE);

  mSampleCoverageEnabled = false;
  mGlAbstraction.Disable(GL_SAMPLE_COVERAGE);

  mScissorTestEnabled = false;
  mGlAbstraction.Disable(GL_SCISSOR_TEST);

  mStencilTestEnabled = false;
  mGlAbstraction.Disable(GL_STENCIL_TEST);

  mBoundArrayBufferId = 0;
  LOG_GL("BindBuffer GL_ARRAY_BUFFER 0\n");
  mGlAbstraction.BindBuffer(GL_ARRAY_BUFFER, mBoundArrayBufferId);

  mBoundElementArrayBufferId = 0;
  LOG_GL("BindBuffer GL_ELEMENT_ARRAY_BUFFER 0\n");
  mGlAbstraction.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBoundElementArrayBufferId);

#ifndef EMSCRIPTEN // not in WebGL
  mBoundTransformFeedbackBufferId = 0;
  LOG_GL("BindBuffer GL_TRANSFORM_FEEDBACK_BUFFER 0\n");
  mGlAbstraction.BindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, mBoundTransformFeedbackBufferId);
#endif

  mActiveTextureUnit = UNINITIALIZED_TEXTURE_UNIT;

  mUsingDefaultBlendColor = true;
  mGlAbstraction.BlendColor( 0.0f, 0.0f, 0.0f, 0.0f );

  mBlendFuncSeparateSrcRGB = GL_ONE;
  mBlendFuncSeparateDstRGB = GL_ZERO;
  mBlendFuncSeparateSrcAlpha = GL_ONE;
  mBlendFuncSeparateDstAlpha = GL_ZERO;
  mGlAbstraction.BlendFuncSeparate( mBlendFuncSeparateSrcRGB, mBlendFuncSeparateDstRGB,
                                    mBlendFuncSeparateSrcAlpha, mBlendFuncSeparateDstAlpha );

  // initial state is GL_FUNC_ADD for both RGB and Alpha blend modes
  mBlendEquationSeparateModeRGB = GL_FUNC_ADD;
  mBlendEquationSeparateModeAlpha = GL_FUNC_ADD;
  mGlAbstraction.BlendEquationSeparate( mBlendEquationSeparateModeRGB, mBlendEquationSeparateModeAlpha);

  mCullFaceMode = CullNone;
  mGlAbstraction.Disable(GL_CULL_FACE);
  mGlAbstraction.FrontFace(GL_CCW);
  mGlAbstraction.CullFace(GL_BACK);

  // get max texture units
  mGlAbstraction.GetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mMaxTextureUnits);
  DALI_ASSERT_DEBUG(mMaxTextureUnits > 7);  // according to GLES 2.0 specification
  mBound2dTextureId.reserve(mMaxTextureUnits);
  // rebind texture units
  for( int i=0; i < mMaxTextureUnits; ++i )
  {
    mBound2dTextureId[ i ] = 0;
    // set active texture
    mGlAbstraction.ActiveTexture( GL_TEXTURE0 + i );
    // bind the previous texture
    mGlAbstraction.BindTexture(GL_TEXTURE_2D, mBound2dTextureId[ i ] );
  }

  // get maximum texture size
  mGlAbstraction.GetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize);

  GLint numProgramBinaryFormats;
  mGlAbstraction.GetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS_OES, &numProgramBinaryFormats);
  if( GL_NO_ERROR == mGlAbstraction.GetError() && 0 != numProgramBinaryFormats )
  {
    mProgramBinaryFormats.resize(numProgramBinaryFormats);
    mGlAbstraction.GetIntegerv(GL_PROGRAM_BINARY_FORMATS_OES, &mProgramBinaryFormats[0]);
  }

  // reset viewport, this will be set to something useful when rendering
  mViewPort.x = mViewPort.y = mViewPort.width = mViewPort.height = 0;

  ResetVertexAttributeState();
}

#ifdef DALI_CONTEXT_LOGGING

void Context::PrintCurrentState()
{
  DALI_LOG_INFO(SceneGraph::Context::gGlLogFilter, Debug::General,
                "----------------- Context State BEGIN -----------------\n"
                "Blend = %s\n"
                "Cull Face = %s\n"
                "Depth Test = %s\n"
                "Depth Mask = %s\n"
                "Dither = %s\n"
                "Polygon Offset Fill = %s\n"
                "Sample Alpha To Coverage = %s\n"
                "Sample Coverage = %s\n"
                "Scissor Test = %s\n"
                "Stencil Test = %s\n"
                "----------------- Context State END -----------------\n",
                mBlendEnabled ? "Enabled" : "Disabled",
                mDepthTestEnabled ? "Enabled" : "Disabled",
                mDepthMaskEnabled ? "Enabled" : "Disabled",
                mDitherEnabled ? "Enabled" : "Disabled",
                mPolygonOffsetFillEnabled ? "Enabled" : "Disabled",
                mSampleAlphaToCoverageEnabled ? "Enabled" : "Disabled",
                mSampleCoverageEnabled ? "Enabled" : "Disabled",
                mScissorTestEnabled ? "Enabled" : "Disabled",
                mStencilTestEnabled ? "Enabled" : "Disabled");
}

#endif // DALI_CONTEXT_LOGGING

} // namespace Internal

} // namespace Dali
