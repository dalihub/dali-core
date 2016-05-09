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
#include <cstring>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/compile-time-assert.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/devel-api/rendering/texture-set.h>

namespace Dali
{

namespace Internal
{

namespace // unnamed namespace
{

DALI_COMPILE_TIME_ASSERT( TEXTURE_UNIT_LAST <= Context::MAX_TEXTURE_UNITS );

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

} // unnamed namespace

#ifdef DEBUG_ENABLED
Debug::Filter* gContextLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_CONTEXT_STATE");
#endif

Context::Context(Integration::GlAbstraction& glAbstraction)
: mGlAbstraction(glAbstraction),
  mGlContextCreated(false),
  mColorMask(true),
  mStencilMask(0xFF),
  mBlendEnabled(false),
  mDepthBufferEnabled(false),
  mDepthMaskEnabled(false),
  mDitherEnabled(true), // This the only GL capability which defaults to true
  mPolygonOffsetFillEnabled(false),
  mSampleAlphaToCoverageEnabled(false),
  mSampleCoverageEnabled(false),
  mScissorTestEnabled(false),
  mStencilBufferEnabled(false),
  mClearColorSet(false),
  mUsingDefaultBlendColor(true),
  mBoundArrayBufferId(0),
  mBoundElementArrayBufferId(0),
  mBoundTransformFeedbackBufferId(0),
  mActiveTextureUnit( TEXTURE_UNIT_LAST ),
  mBlendColor(Color::TRANSPARENT),
  mBlendFuncSeparateSrcRGB(GL_ONE),
  mBlendFuncSeparateDstRGB(GL_ZERO),
  mBlendFuncSeparateSrcAlpha(GL_ONE),
  mBlendFuncSeparateDstAlpha(GL_ZERO),
  mBlendEquationSeparateModeRGB( GL_FUNC_ADD ),
  mBlendEquationSeparateModeAlpha( GL_FUNC_ADD ),
  mDepthFunction( GL_LESS ),
  mMaxTextureSize(0),
  mClearColor(Color::WHITE),    // initial color, never used until it's been set by the user
  mCullFaceMode( FaceCullingMode::NONE ),
  mViewPort( 0, 0, 0, 0 )
{
}

Context::~Context()
{
}

void Context::GlContextCreated()
{
  DALI_LOG_INFO(gContextLogFilter, Debug::Verbose, "Context::GlContextCreated()\n");

  DALI_ASSERT_DEBUG(!mGlContextCreated);

  mGlContextCreated = true;

  // Set the initial GL state, and check it.
  InitializeGlState();

#ifdef DEBUG_ENABLED
  PrintCurrentState();
#endif
}

void Context::GlContextDestroyed()
{
  DALI_LOG_INFO(gContextLogFilter, Debug::Verbose, "Context::GlContextDestroyed()\n");
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
        CHECK_GL( mGlAbstraction, mGlAbstraction.EnableVertexAttribArray( i ) );
      }
      else
      {
        LOG_GL("DisableVertexAttribArray %d\n", i);
        CHECK_GL( mGlAbstraction, mGlAbstraction.DisableVertexAttribArray( i ) );
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
       CHECK_GL( mGlAbstraction, mGlAbstraction.EnableVertexAttribArray( location ) );
    }
    else
    {
      LOG_GL("DisableVertexAttribArray %d\n", location);
      CHECK_GL( mGlAbstraction, mGlAbstraction.DisableVertexAttribArray( location ) );
    }
  }
  else
  {
    // set the cached state, it will be set at the next draw call
    // if it's different from the current driver state
    mVertexAttributeCachedState[ location ] = state;
  }
}

void Context::InitializeGlState()
{
  DALI_LOG_INFO(gContextLogFilter, Debug::Verbose, "Context::InitializeGlState()\n");
  DALI_ASSERT_DEBUG(mGlContextCreated);

  mClearColorSet = false;
  mColorMask = true;
  mStencilMask = 0xFF;
  mBlendEnabled = false;
  mDepthBufferEnabled = false;
  mDepthMaskEnabled = false;
  mPolygonOffsetFillEnabled = false;
  mSampleAlphaToCoverageEnabled = false;
  mSampleCoverageEnabled = false;
  mScissorTestEnabled = false;
  mStencilBufferEnabled = false;
  mDitherEnabled = false; // This and GL_MULTISAMPLE are the only GL capability which defaults to true
  mGlAbstraction.Disable(GL_DITHER);

  mBoundArrayBufferId = 0;
  mBoundElementArrayBufferId = 0;
  mBoundTransformFeedbackBufferId = 0;
  mActiveTextureUnit = TEXTURE_UNIT_IMAGE;

  mUsingDefaultBlendColor = true; //Default blend color is (0,0,0,0)

  mBlendFuncSeparateSrcRGB = GL_ONE;
  mBlendFuncSeparateDstRGB = GL_ZERO;
  mBlendFuncSeparateSrcAlpha = GL_ONE;
  mBlendFuncSeparateDstAlpha = GL_ZERO;

  // initial state is GL_FUNC_ADD for both RGB and Alpha blend modes
  mBlendEquationSeparateModeRGB = GL_FUNC_ADD;
  mBlendEquationSeparateModeAlpha = GL_FUNC_ADD;

  mCullFaceMode = FaceCullingMode::NONE; //By default cullface is disabled, front face is set to CCW and cull face is set to back

  // get maximum texture size
  mGlAbstraction.GetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize);

  // reset viewport, this will be set to something useful when rendering
  mViewPort.x = mViewPort.y = mViewPort.width = mViewPort.height = 0;

  //Initialze vertex attribute cache
  memset( &mVertexAttributeCachedState, 0, sizeof(mVertexAttributeCachedState) );
  memset( &mVertexAttributeCurrentState, 0, sizeof(mVertexAttributeCurrentState) );

  //Initialize bound 2d texture cache
  memset( &mBound2dTextureId, 0, sizeof(mBound2dTextureId) );

  mFrameBufferStateCache.Reset();
}

#ifdef DEBUG_ENABLED

void Context::PrintCurrentState()
{
  const char* cullFaceModes[] = { "CullNone", "CullFront", "CullBack", "CullFrontAndBack" };
  DALI_LOG_INFO( gContextLogFilter, Debug::General,
                "\n----------------- Context State BEGIN -----------------\n"
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
                cullFaceModes[ mCullFaceMode ],
                mDepthBufferEnabled ? "Enabled" : "Disabled",
                mDepthMaskEnabled ? "Enabled" : "Disabled",
                mDitherEnabled ? "Enabled" : "Disabled",
                mPolygonOffsetFillEnabled ? "Enabled" : "Disabled",
                mSampleAlphaToCoverageEnabled ? "Enabled" : "Disabled",
                mSampleCoverageEnabled ? "Enabled" : "Disabled",
                mScissorTestEnabled ? "Enabled" : "Disabled",
                mStencilBufferEnabled ? "Enabled" : "Disabled");
}

#endif // DALI_CONTEXT_LOGGING

} // namespace Internal

} // namespace Dali
