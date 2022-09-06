/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include "test-gl-abstraction.h"
#include "test-trace-call-stack.h"

static const bool TRACE{
  false};

namespace Dali
{
TestGlAbstraction::TestGlAbstraction()
: mBufferTrace(TRACE, std::string("gl")),
  mCullFaceTrace(TRACE, "gl"),
  mEnableDisableTrace(TRACE, "gl"),
  mShaderTrace(TRACE, "gl"),
  mTextureTrace(TRACE, std::string("gl")),
  mTexParameterTrace(TRACE, "gl"),
  mDrawTrace(TRACE, "gl"),
  mDepthFunctionTrace(TRACE, "gl"),
  mStencilFunctionTrace(TRACE, "gl"),
  mScissorTrace(TRACE, "gl"),
  mSetUniformTrace(TRACE, "Uniform "),
  mViewportTrace(TRACE, "gl")
{
  Initialize();
}

TestGlAbstraction::~TestGlAbstraction()
{
}

void TestGlAbstraction::Initialize()
{
  mCurrentProgram                         = 0;
  mCompileStatus                          = GL_TRUE;
  mLinkStatus                             = GL_TRUE;
  mGetErrorResult                         = 0;
  mGetStringResult                        = NULL;
  mIsBufferResult                         = 0;
  mIsEnabledResult                        = 0;
  mIsFramebufferResult                    = 0;
  mIsProgramResult                        = 0;
  mIsRenderbufferResult                   = 0;
  mIsShaderResult                         = 0;
  mIsTextureResult                        = 0;
  mActiveTextureUnit                      = 0;
  mCheckFramebufferStatusResult           = 0;
  mFramebufferStatus                      = 0;
  mFramebufferDepthAttached               = 0;
  mFramebufferStencilAttached             = 0;
  mFramebufferDepthStencilAttached        = 0;
  mFramebufferColorAttachmentCount        = 0;
  mFrameBufferColorStatus                 = 0;
  mFramebufferDepthAttachmentCount        = 0;
  mFramebufferStencilAttachmentCount      = 0;
  mFramebufferDepthStencilAttachmentCount = 0;
  mNumBinaryFormats                       = 0;
  mBinaryFormats                          = 0;
  mProgramBinaryLength                    = 0;

  mVertexAttribArrayChanged = false;
  mGetProgramBinaryCalled   = false;

  mLastShaderCompiled = 0;
  mLastClearBitMask   = 0;
  mLastClearColor     = Color::TRANSPARENT;
  mClearCount         = 0;

  mLastBlendEquationRgb   = 0;
  mLastBlendEquationAlpha = 0;
  mLastBlendFuncSrcRgb    = 0;
  mLastBlendFuncDstRgb    = 0;
  mLastBlendFuncSrcAlpha  = 0;
  mLastBlendFuncDstAlpha  = 0;
  mLastAutoTextureIdUsed  = 0;
  mNumGeneratedTextures   = 0;
  mLastShaderIdUsed       = 0;
  mLastProgramIdUsed      = 0;
  mLastUniformIdUsed      = 0;
  mLastDepthMask          = false;

  mUniforms.clear();
  mProgramUniforms1i.clear();
  mProgramUniforms1f.clear();
  mProgramUniforms2f.clear();
  mProgramUniforms3f.clear();
  mProgramUniforms4f.clear();

  mAttribLocs.clear();
  mAttribLocs.push_back("aPosition");
  mAttribLocs.push_back("aTexCoord");
  mCullFaceTrace.Reset();
  mDepthFunctionTrace.Reset();
  mEnableDisableTrace.Reset();
  mShaderTrace.Reset();
  mStencilFunctionTrace.Reset();
  mScissorTrace.Reset();
  mTextureTrace.Reset();
  mTexParameterTrace.Reset();
  mDrawTrace.Reset();

  for(unsigned int i = 0; i < MAX_ATTRIBUTE_CACHE_SIZE; ++i)
  {
    mVertexAttribArrayState[i] = false;
  }

  mActiveUniforms = std::vector<ActiveUniform>{
    {"uRendererColor", GL_FLOAT, 1},
    {"uCustom", GL_FLOAT_VEC3, 1},
    {"uCustom3", GL_FLOAT_VEC3, 1},
    {"uFadeColor", GL_FLOAT_VEC4, 1},
    {"uUniform1", GL_FLOAT_VEC4, 1},
    {"uUniform2", GL_FLOAT_VEC4, 1},
    {"uUniform3", GL_FLOAT_VEC4, 1},
    {"uFadeProgress", GL_FLOAT, 1},
    {"uANormalMatrix", GL_FLOAT_MAT3, 1},
    {"sEffect", GL_SAMPLER_2D, 1},
    {"sTexture", GL_SAMPLER_2D, 1},
    {"sTextureRect", GL_SAMPLER_2D, 1},
    {"sGloss", GL_SAMPLER_2D, 1},
    {"uColor", GL_FLOAT_VEC4, 1},
    {"uActorColor", GL_FLOAT_VEC4, 1},
    {"uModelMatrix", GL_FLOAT_MAT4, 1},
    {"uModelView", GL_FLOAT_MAT4, 1},
    {"uMvpMatrix", GL_FLOAT_MAT4, 1},
    {"uNormalMatrix", GL_FLOAT_MAT4, 1},
    {"uProjection", GL_FLOAT_MAT4, 1},
    {"uSize", GL_FLOAT_VEC3, 1},
    {"uViewMatrix", GL_FLOAT_MAT4, 1},
    {"uLightCameraProjectionMatrix", GL_FLOAT_MAT4, 1},
    {"uLightCameraViewMatrix", GL_FLOAT_MAT4, 1}};

  // WARNING: IF YOU CHANGE THIS LIST, ALSO CHANGE UNIFORMS IN test-graphics-reflection.cpp
}

void TestGlAbstraction::PreRender()
{
}

void TestGlAbstraction::PostRender()
{
}

bool TestGlAbstraction::IsSurfacelessContextSupported() const
{
  return true;
}

bool TestGlAbstraction::IsAdvancedBlendEquationSupported()
{
  return true;
}

bool TestGlAbstraction::IsBlendEquationSupported(DevelBlendEquation::Type blendEquation)
{
  return true;
}

std::string TestGlAbstraction::GetShaderVersionPrefix()
{
  return std::string("");
}

std::string TestGlAbstraction::GetVertexShaderPrefix()
{
  return std::string("");
}

std::string TestGlAbstraction::GetFragmentShaderPrefix()
{
  return std::string("");
}

bool TestGlAbstraction::TextureRequiresConverting(const GLenum imageGlFormat, const GLenum textureGlFormat, const bool isSubImage) const
{
  return ((imageGlFormat == GL_RGB) && (textureGlFormat == GL_RGBA));
}

} // namespace Dali

bool BlendEnabled(const Dali::TraceCallStack& callStack)
{
  std::stringstream out;
  out << GL_BLEND;
  bool blendEnabled = callStack.FindMethodAndParams("Enable", out.str());
  return blendEnabled;
}

bool BlendDisabled(const Dali::TraceCallStack& callStack)
{
  std::stringstream out;
  out << GL_BLEND;
  bool blendEnabled = callStack.FindMethodAndParams("Disable", out.str());
  return blendEnabled;
}
