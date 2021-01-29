#ifndef TEST_GL_ABSTRACTION_H
#define TEST_GL_ABSTRACTION_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdio>
#include <cstring>
#include <cstring> // for strcmp
#include <map>
#include <sstream>
#include <string>
#include <typeinfo>

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/public-api/dali-core.h>
#include <test-compare-types.h>
#include <test-trace-call-stack.h>

namespace Dali
{
class DALI_CORE_API TestGlAbstraction : public Dali::Integration::GlAbstraction
{
public:
  static const int MAX_ATTRIBUTE_CACHE_SIZE{64};

  TestGlAbstraction();
  ~TestGlAbstraction() override;
  void Initialize();

  void PreRender() override;
  void PostRender() override;

  bool IsSurfacelessContextSupported() const override;

  bool IsAdvancedBlendEquationSupported() override;

  bool IsBlendEquationSupported(DevelBlendEquation::Type blendEquation) override;

  std::string GetShaderVersionPrefix();

  std::string GetVertexShaderPrefix();

  std::string GetFragmentShaderPrefix();

  bool TextureRequiresConverting(const GLenum imageGlFormat, const GLenum textureGlFormat, const bool isSubImage) const override;

  /* OpenGL ES 2.0 */

  inline void ActiveTexture(GLenum textureUnit) override
  {
    mActiveTextureUnit = textureUnit - GL_TEXTURE0;
  }

  inline GLenum GetActiveTextureUnit() const
  {
    return mActiveTextureUnit + GL_TEXTURE0;
  }

  inline void AttachShader(GLuint program, GLuint shader) override
  {
    std::stringstream out;
    out << program << ", " << shader;

    TraceCallStack::NamedParams namedParams;
    namedParams["program"] << program;
    namedParams["shader"] << shader;
    mShaderTrace.PushCall("AttachShader", out.str(), namedParams);
  }

  inline void BindAttribLocation(GLuint program, GLuint index, const char* name) override
  {
  }

  inline void BindBuffer(GLenum target, GLuint buffer) override
  {
    std::ostringstream o;
    o << std::hex << target << ", " << buffer;
    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << target;
    namedParams["buffer"] << buffer;
    mBufferTrace.PushCall("BindBuffer", o.str(), namedParams);
  }

  inline void BindFramebuffer(GLenum target, GLuint framebuffer) override
  {
    //Add 010 bit;
    mFramebufferStatus |= 2;
  }

  inline void BindRenderbuffer(GLenum target, GLuint renderbuffer) override
  {
  }

  /**
   * This method can be used by test cases, to query the texture IDs that have been bound by BindTexture.
   * @return A vector containing the IDs that were bound.
   */
  inline const std::vector<GLuint>& GetBoundTextures() const
  {
    return mBoundTextures;
  }

  /**
   * Query the texture IDs that have been bound with BindTexture, with a specific active texture unit.
   * @param[in] activeTextureUnit The specific active texture unit.
   * @return A vector containing the IDs that were bound.
   */
  inline const std::vector<GLuint>& GetBoundTextures(GLuint activeTextureUnit) const
  {
    return mActiveTextures[activeTextureUnit - GL_TEXTURE0].mBoundTextures;
  }

  /**
   * This method can be used by test cases, to clear the record of texture IDs that have been bound by BindTexture.
   */
  inline void ClearBoundTextures()
  {
    mBoundTextures.clear();

    for(unsigned int i = 0; i < MIN_TEXTURE_UNIT_LIMIT; ++i)
    {
      mActiveTextures[i].mBoundTextures.clear();
    }
  }

  inline void BindTexture(GLenum target, GLuint texture) override
  {
    // Record the bound textures for future checks
    if(texture)
    {
      mBoundTextures.push_back(texture);

      if(mActiveTextureUnit < MIN_TEXTURE_UNIT_LIMIT)
      {
        mActiveTextures[mActiveTextureUnit].mBoundTextures.push_back(texture);
      }
    }

    std::stringstream out;
    out << target << ", " << texture;

    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << target;
    namedParams["texture"] << texture;

    mTextureTrace.PushCall("BindTexture", out.str(), namedParams);
  }

  inline void BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override
  {
    mLastBlendColor.r = red;
    mLastBlendColor.g = green;
    mLastBlendColor.b = blue;
    mLastBlendColor.a = alpha;
  }

  inline const Vector4& GetLastBlendColor() const
  {
    return mLastBlendColor;
  }

  inline void BlendEquation(GLenum mode) override
  {
    mLastBlendEquationRgb   = mode;
    mLastBlendEquationAlpha = mode;
  }

  inline void BlendEquationSeparate(GLenum modeRgb, GLenum modeAlpha) override
  {
    mLastBlendEquationRgb   = modeRgb;
    mLastBlendEquationAlpha = modeAlpha;
  }

  inline GLenum GetLastBlendEquationRgb() const
  {
    return mLastBlendEquationRgb;
  }

  inline GLenum GetLastBlendEquationAlpha() const
  {
    return mLastBlendEquationAlpha;
  }

  inline void BlendFunc(GLenum sfactor, GLenum dfactor) override
  {
    mLastBlendFuncSrcRgb   = sfactor;
    mLastBlendFuncDstRgb   = dfactor;
    mLastBlendFuncSrcAlpha = sfactor;
    mLastBlendFuncDstAlpha = dfactor;
  }

  inline void BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) override
  {
    mLastBlendFuncSrcRgb   = srcRGB;
    mLastBlendFuncDstRgb   = dstRGB;
    mLastBlendFuncSrcAlpha = srcAlpha;
    mLastBlendFuncDstAlpha = dstAlpha;
  }

  inline GLenum GetLastBlendFuncSrcRgb() const
  {
    return mLastBlendFuncSrcRgb;
  }

  inline GLenum GetLastBlendFuncDstRgb() const
  {
    return mLastBlendFuncDstRgb;
  }

  inline GLenum GetLastBlendFuncSrcAlpha() const
  {
    return mLastBlendFuncSrcAlpha;
  }

  inline GLenum GetLastBlendFuncDstAlpha() const
  {
    return mLastBlendFuncDstAlpha;
  }

  inline void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) override
  {
    std::ostringstream o;
    o << std::hex << target << ", " << size << ", " << data << ", " << usage;
    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["size"] << size;
    namedParams["usage"] << usage;

    mBufferTrace.PushCall("BufferData", o.str(), namedParams);

    mBufferDataCalls.push_back(size);
  }

  inline void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) override
  {
    std::ostringstream o;
    o << std::hex << target << ", " << offset << ", " << size << ", " << data;
    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["offset"] << offset;
    namedParams["size"] << size;
    mBufferTrace.PushCall("BufferSubData", o.str());

    mBufferSubDataCalls.push_back(size);
  }

  inline GLenum CheckFramebufferStatus(GLenum target) override
  {
    //If it has the three last bits set to 1 - 111, then the three minimum functions to create a
    //Framebuffer texture have been called
    if(mFramebufferStatus == 7)
    {
      return GL_FRAMEBUFFER_COMPLETE;
    }

    return mCheckFramebufferStatusResult;
  }

  inline GLuint CheckFramebufferColorAttachmentCount()
  {
    return mFramebufferColorAttachmentCount;
  }

  inline GLenum CheckFramebufferDepthAttachment()
  {
    return mFramebufferDepthAttached;
  }

  inline GLenum CheckFramebufferStencilAttachment()
  {
    return mFramebufferStencilAttached;
  }

  inline void Clear(GLbitfield mask) override
  {
    mClearCount++;
    mLastClearBitMask = mask;
  }

  inline void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override
  {
    mLastClearColor.r = red;
    mLastClearColor.g = green;
    mLastClearColor.b = blue;
    mLastClearColor.a = alpha;
  }

  inline const Vector4& GetLastClearColor() const
  {
    return mLastClearColor;
  }

  inline void ClearDepthf(GLclampf depth) override
  {
  }

  inline void ClearStencil(GLint s) override
  {
    std::stringstream out;
    out << s;

    TraceCallStack::NamedParams namedParams;
    namedParams["s"] << s;

    mStencilFunctionTrace.PushCall("ClearStencil", out.str(), namedParams);
  }

  inline void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) override
  {
    mColorMaskParams.red   = red;
    mColorMaskParams.green = green;
    mColorMaskParams.blue  = blue;
    mColorMaskParams.alpha = alpha;
  }

  inline void CompileShader(GLuint shader) override
  {
    std::stringstream out;
    out << shader;
    TraceCallStack::NamedParams namedParams;
    namedParams["shader"] << shader;

    mShaderTrace.PushCall("CompileShader", out.str(), namedParams);
  }

  inline void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) override
  {
    std::stringstream out;
    out << target << ", " << level << ", " << width << ", " << height;

    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["level"] << level;
    namedParams["internalformat"] << internalformat;
    namedParams["width"] << width;
    namedParams["height"] << height;
    namedParams["border"] << border;
    namedParams["size"] << imageSize;

    mTextureTrace.PushCall("CompressedTexImage2D", out.str(), namedParams);
  }

  inline void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) override
  {
    std::stringstream out;
    out << target << ", " << level << ", " << xoffset << ", " << yoffset << ", " << width << ", " << height;

    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["level"] << level;
    namedParams["xoffset"] << xoffset;
    namedParams["yoffset"] << yoffset;
    namedParams["width"] << width;
    namedParams["height"] << height;
    mTextureTrace.PushCall("CompressedTexSubImage2D", out.str(), namedParams);
  }

  inline void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) override
  {
  }

  inline void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) override
  {
  }

  inline GLuint CreateProgram(void) override
  {
    mShaderTrace.PushCall("CreateProgram", "");

    ++mLastProgramIdUsed;
    mUniforms[mLastProgramIdUsed] = UniformIDMap();
    return mLastProgramIdUsed;
  }

  inline GLuint CreateShader(GLenum type) override
  {
    std::stringstream out;
    out << type;

    TraceCallStack::NamedParams namedParams;
    namedParams["type"] << std::hex << type;
    mShaderTrace.PushCall("CreateShader", out.str(), namedParams);

    return ++mLastShaderIdUsed;
  }

  inline void CullFace(GLenum mode) override
  {
    std::stringstream out;
    out << mode;

    TraceCallStack::NamedParams namedParams;
    namedParams["program"] << mode;

    mCullFaceTrace.PushCall("CullFace", out.str(), namedParams);
  }

  inline void DeleteBuffers(GLsizei n, const GLuint* buffers) override
  {
  }

  inline void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers) override
  {
  }

  inline void DeleteProgram(GLuint program) override
  {
    std::stringstream out;
    out << program;

    TraceCallStack::NamedParams namedParams;
    namedParams["program"] << program;

    mShaderTrace.PushCall("DeleteProgram", out.str(), namedParams);
  }

  inline void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers) override
  {
  }

  inline void DeleteShader(GLuint shader) override
  {
    std::stringstream out;
    out << shader;

    TraceCallStack::NamedParams namedParams;
    namedParams["shader"] << shader;

    mShaderTrace.PushCall("DeleteShader", out.str(), namedParams);
  }

  inline void DeleteTextures(GLsizei n, const GLuint* textures) override
  {
    std::stringstream out;
    out << "n:" << n << " textures[";

    TraceCallStack::NamedParams namedParams;

    for(GLsizei i = 0; i < n; i++)
    {
      out << (i > 0 ? ", " : "") << textures[i];
      std::stringstream paramName;
      paramName << "texture[" << i << "]";
      namedParams[paramName.str()] << textures[i];
      mDeletedTextureIds.push_back(textures[i]);
      mNumGeneratedTextures--;
    }
    out << "]";

    mTextureTrace.PushCall("DeleteTextures", out.str(), namedParams);
  }

  inline bool CheckNoTexturesDeleted()
  {
    return mDeletedTextureIds.size() == 0;
  }

  inline bool CheckTextureDeleted(GLuint textureId)
  {
    bool found = false;

    for(std::vector<GLuint>::iterator iter = mDeletedTextureIds.begin(); iter != mDeletedTextureIds.end(); ++iter)
    {
      if(*iter == textureId)
      {
        found = true;
        break;
      }
    }
    return found;
  }

  inline void ClearDeletedTextures()
  {
    mDeletedTextureIds.clear();
  }

  inline void DepthFunc(GLenum func) override
  {
    std::stringstream out;
    out << func;

    TraceCallStack::NamedParams namedParams;
    namedParams["func"] << std::hex << func;

    mDepthFunctionTrace.PushCall("DepthFunc", out.str(), namedParams);
  }

  inline void DepthMask(GLboolean flag) override
  {
    mLastDepthMask = flag;
  }

  inline bool GetLastDepthMask() const
  {
    return mLastDepthMask;
  }

  inline void DepthRangef(GLclampf zNear, GLclampf zFar) override
  {
  }

  inline void DetachShader(GLuint program, GLuint shader) override
  {
    std::stringstream out;
    out << program << ", " << shader;
    TraceCallStack::NamedParams namedParams;
    namedParams["program"] << program;
    namedParams["shader"] << shader;
    mShaderTrace.PushCall("DetachShader", out.str(), namedParams);
  }

  inline void Disable(GLenum cap) override
  {
    std::stringstream out;
    out << cap;
    TraceCallStack::NamedParams namedParams;
    namedParams["cap"] << std::hex << cap;
    mEnableDisableTrace.PushCall("Disable", out.str(), namedParams);
  }

  inline void DisableVertexAttribArray(GLuint index) override
  {
    std::stringstream out;
    out << index;
    TraceCallStack::NamedParams namedParams;
    namedParams["index"] << index;
    mBufferTrace.PushCall("DisableVertexAttribArray", out.str(), namedParams);
    SetVertexAttribArray(index, false);
  }

  inline void DrawArrays(GLenum mode, GLint first, GLsizei count) override
  {
    std::stringstream out;
    out << mode << ", " << first << ", " << count;
    TraceCallStack::NamedParams namedParams;
    namedParams["mode"] << std::hex << mode;
    namedParams["first"] << first;
    namedParams["count"] << count;
    mDrawTrace.PushCall("DrawArrays", out.str(), namedParams);
  }

  inline void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) override
  {
    std::stringstream out;
    out << mode << ", " << count << ", " << type << ", indices";

    TraceCallStack::NamedParams namedParams;
    namedParams["mode"] << std::hex << mode;
    namedParams["count"] << count;
    namedParams["type"] << type;
    // Skip void pointers - are they of any use?
    mDrawTrace.PushCall("DrawElements", out.str(), namedParams);
  }

  inline void Enable(GLenum cap) override
  {
    std::stringstream out;
    out << cap;
    TraceCallStack::NamedParams namedParams;
    namedParams["cap"] << std::hex << cap;
    mEnableDisableTrace.PushCall("Enable", out.str(), namedParams);
  }

  inline void EnableVertexAttribArray(GLuint index) override
  {
    std::stringstream out;
    out << index;
    TraceCallStack::NamedParams namedParams;
    namedParams["index"] << index;
    mBufferTrace.PushCall("EnableVertexAttribArray", out.str(), namedParams);
    SetVertexAttribArray(index, true);
  }

  inline void Finish(void) override
  {
  }

  inline void Flush(void) override
  {
  }

  inline void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) override
  {
    if(attachment == GL_DEPTH_ATTACHMENT)
    {
      mFramebufferDepthAttached = true;
    }
    else if(attachment == GL_STENCIL_ATTACHMENT)
    {
      mFramebufferStencilAttached = true;
    }
  }

  inline void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) override
  {
    //Add 100 bit;
    mFramebufferStatus |= 4;

    //We check 4 attachment colors
    if((attachment >= GL_COLOR_ATTACHMENT0) && (attachment < GL_COLOR_ATTACHMENT0 + Dali::DevelFrameBuffer::MAX_COLOR_ATTACHMENTS))
    {
      uint8_t mask = 1 << (attachment - GL_COLOR_ATTACHMENT0);
      if((mFrameBufferColorStatus & mask) == 0)
      {
        mFrameBufferColorStatus |= mask;
        ++mFramebufferColorAttachmentCount;
      }
    }
  }

  inline void FrontFace(GLenum mode) override
  {
  }

  inline void GenBuffers(GLsizei n, GLuint* buffers) override
  {
    // avoids an assert in GpuBuffers
    *buffers = 1u;

    std::ostringstream o;
    o << n;
    TraceCallStack::NamedParams namedParams;
    namedParams["n"] << o.str();
    mBufferTrace.PushCall("GenBuffers", o.str(), namedParams);
  }

  inline void GenerateMipmap(GLenum target) override
  {
    std::stringstream out;
    out << target;
    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;

    mTextureTrace.PushCall("GenerateMipmap", out.str(), namedParams);
  }

  inline void GenFramebuffers(GLsizei n, GLuint* framebuffers) override
  {
    for(int i = 0; i < n; i++)
    {
      framebuffers[i] = i + 1;
    }

    //Add 001 bit, this function needs to be called the first one in the chain
    mFramebufferStatus = 1;
  }

  inline void GenRenderbuffers(GLsizei n, GLuint* renderbuffers) override
  {
    for(int i = 0; i < n; i++)
    {
      renderbuffers[i] = i + 1;
    }
  }

  /**
   * This method can be used by test cases, to manipulate the texture IDs generated by GenTextures.
   * @param[in] ids A vector containing the next IDs to be generated
   */
  inline void SetNextTextureIds(const std::vector<GLuint>& ids)
  {
    mNextTextureIds = ids;
  }

  inline const std::vector<GLuint>& GetNextTextureIds()
  {
    return mNextTextureIds;
  }

  inline void GenTextures(GLsizei count, GLuint* textures) override
  {
    for(int i = 0; i < count; ++i)
    {
      if(!mNextTextureIds.empty())
      {
        *(textures + i) = mNextTextureIds[0];
        mNextTextureIds.erase(mNextTextureIds.begin());
      }
      else
      {
        *(textures + i) = ++mLastAutoTextureIdUsed;
      }
      mNumGeneratedTextures++;
    }

    TraceCallStack::NamedParams namedParams;
    namedParams["count"] << count;

    std::stringstream out;
    for(int i = 0; i < count; i++)
    {
      out << textures[i];
      if(i < count - 1)
      {
        out << ", ";
      }
      std::ostringstream oss;
      oss << "indices[" << i << "]";
      namedParams[oss.str()] << textures[i];
    }

    mTextureTrace.PushCall("GenTextures", out.str(), namedParams);
  }

  inline GLuint GetLastGenTextureId()
  {
    return mLastAutoTextureIdUsed;
  }

  inline GLuint GetNumGeneratedTextures()
  {
    return mNumGeneratedTextures;
  }

  inline void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) override
  {
  }

  inline void GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) override
  {
    switch(index)
    {
      case 0:
        *length = snprintf(name, bufsize, "sTexture");
        *type   = GL_SAMPLER_2D;
        *size   = 1;
        break;
      case 1:
        *length = snprintf(name, bufsize, "sEffect");
        *type   = GL_SAMPLER_2D;
        *size   = 1;
        break;
      case 2:
        *length = snprintf(name, bufsize, "sGloss");
        *type   = GL_SAMPLER_2D;
        *size   = 1;
        break;
      default:
        break;
    }
  }

  inline void GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders) override
  {
  }

  inline int GetAttribLocation(GLuint program, const char* name) override
  {
    std::string check(name);
    auto        iter = std::find(mAttribLocs.begin(), mAttribLocs.end(), check);
    if(iter == mAttribLocs.end())
      return -1;
    return iter - mAttribLocs.begin();
  }

  inline void GetBooleanv(GLenum pname, GLboolean* params) override
  {
  }

  inline void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params) override
  {
  }

  inline GLenum GetError(void) override
  {
    return mGetErrorResult;
  }

  inline void GetFloatv(GLenum pname, GLfloat* params) override
  {
  }

  inline void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params) override
  {
  }

  inline void GetIntegerv(GLenum pname, GLint* params) override
  {
    switch(pname)
    {
      case GL_MAX_TEXTURE_SIZE:
        *params = 2048;
        break;
      case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
        *params = 8;
        break;
      case GL_NUM_PROGRAM_BINARY_FORMATS_OES:
        *params = mNumBinaryFormats;
        break;
      case GL_PROGRAM_BINARY_FORMATS_OES:
        *params = mBinaryFormats;
        break;
    }
  }

  inline void GetProgramiv(GLuint program, GLenum pname, GLint* params) override
  {
    switch(pname)
    {
      case GL_LINK_STATUS:
        *params = mLinkStatus;
        break;
      case GL_PROGRAM_BINARY_LENGTH_OES:
        *params = mProgramBinaryLength;
        break;
      case GL_ACTIVE_UNIFORMS:
        *params = mNumberOfActiveUniforms;
        break;
      case GL_ACTIVE_UNIFORM_MAX_LENGTH:
        *params = 100;
        break;
    }
  }

  inline void GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog) override
  {
  }

  inline void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params) override
  {
  }

  inline void GetShaderiv(GLuint shader, GLenum pname, GLint* params) override
  {
    switch(pname)
    {
      case GL_COMPILE_STATUS:
        *params = mCompileStatus;
        break;
    }
  }

  inline void GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog) override
  {
  }

  inline void GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) override
  {
  }

  inline const GLubyte* GetString(GLenum name) override
  {
    return mGetStringResult;
  }

  inline void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params) override
  {
  }

  inline void GetTexParameteriv(GLenum target, GLenum pname, GLint* params) override
  {
  }

  inline void GetUniformfv(GLuint program, GLint location, GLfloat* params) override
  {
  }

  inline void GetUniformiv(GLuint program, GLint location, GLint* params) override
  {
  }

  inline GLint GetUniformLocation(GLuint program, const char* name) override
  {
    ProgramUniformMap::iterator it = mUniforms.find(program);
    if(it == mUniforms.end())
    {
      // Not a valid program ID
      mGetErrorResult = GL_INVALID_OPERATION;
      return -1;
    }

    UniformIDMap&          uniformIDs = it->second;
    UniformIDMap::iterator it2        = uniformIDs.find(name);
    if(it2 == uniformIDs.end())
    {
      // Uniform not found, so add it...
      uniformIDs[name] = ++mLastUniformIdUsed;
      return mLastUniformIdUsed;
    }

    return it2->second;
  }

  inline void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params) override
  {
  }

  inline void GetVertexAttribiv(GLuint index, GLenum pname, GLint* params) override
  {
  }

  inline void GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer) override
  {
  }

  inline void Hint(GLenum target, GLenum mode) override
  {
  }

  inline GLboolean IsBuffer(GLuint buffer) override
  {
    return mIsBufferResult;
  }

  inline GLboolean IsEnabled(GLenum cap) override
  {
    return mIsEnabledResult;
  }

  inline GLboolean IsFramebuffer(GLuint framebuffer) override
  {
    return mIsFramebufferResult;
  }

  inline GLboolean IsProgram(GLuint program) override
  {
    return mIsProgramResult;
  }

  inline GLboolean IsRenderbuffer(GLuint renderbuffer) override
  {
    return mIsRenderbufferResult;
  }

  inline GLboolean IsShader(GLuint shader) override
  {
    return mIsShaderResult;
  }

  inline GLboolean IsTexture(GLuint texture) override
  {
    return mIsTextureResult;
  }

  inline void LineWidth(GLfloat width) override
  {
  }

  inline void LinkProgram(GLuint program) override
  {
    std::stringstream out;
    out << program;

    TraceCallStack::NamedParams namedParams;
    namedParams["program"] << program;
    mShaderTrace.PushCall("LinkProgram", out.str(), namedParams);

    mNumberOfActiveUniforms = 3;
    GetUniformLocation(program, "sTexture");
    GetUniformLocation(program, "sEffect");
    GetUniformLocation(program, "sGloss");
  }

  inline void PixelStorei(GLenum pname, GLint param) override
  {
  }

  inline void PolygonOffset(GLfloat factor, GLfloat units) override
  {
  }

  inline void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) override
  {
  }

  inline void ReleaseShaderCompiler(void) override
  {
  }

  inline void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) override
  {
  }

  inline void SampleCoverage(GLclampf value, GLboolean invert) override
  {
  }

  inline void Scissor(GLint x, GLint y, GLsizei width, GLsizei height) override
  {
    mScissorParams.x      = x;
    mScissorParams.y      = y;
    mScissorParams.width  = width;
    mScissorParams.height = height;

    std::stringstream out;
    out << x << ", " << y << ", " << width << ", " << height;
    TraceCallStack::NamedParams namedParams;
    namedParams["x"] << x;
    namedParams["y"] << y;
    namedParams["width"] << width;
    namedParams["height"] << height;
    mScissorTrace.PushCall("Scissor", out.str(), namedParams);
  }

  inline void ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length) override
  {
  }

  inline void ShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length) override
  {
    std::string stringBuilder;
    for(int i = 0; i < count; ++i)
    {
      stringBuilder += string[i];
    }
    mShaderSources[shader] = stringBuilder;
    mLastShaderCompiled    = shader;
  }

  inline void GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source) override
  {
    const std::string shaderSource       = mShaderSources[shader];
    const int         shaderSourceLength = static_cast<int>(shaderSource.length());
    if(shaderSourceLength < bufsize)
    {
      strncpy(source, shaderSource.c_str(), shaderSourceLength);
      *length = shaderSourceLength;
    }
    else
    {
      *length = bufsize - 1;
      strncpy(source, shaderSource.c_str(), *length);
      source[*length] = 0x0;
    }
  }

  inline std::string GetShaderSource(GLuint shader)
  {
    return mShaderSources[shader];
  }

  inline void StencilFunc(GLenum func, GLint ref, GLuint mask) override
  {
    std::stringstream out;
    out << func << ", " << ref << ", " << mask;

    TraceCallStack::NamedParams namedParams;
    namedParams["func"] << std::hex << func;
    namedParams["ref"] << ref;
    namedParams["mask"] << mask;

    mStencilFunctionTrace.PushCall("StencilFunc", out.str(), namedParams);
  }

  inline void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) override
  {
    std::stringstream out;
    out << face << ", " << func << ", " << ref << ", " << mask;

    TraceCallStack::NamedParams namedParams;
    namedParams["face"] << std::hex << face;
    namedParams["func"] << std::hex << func;
    namedParams["ref"] << ref;
    namedParams["mask"] << mask;

    mStencilFunctionTrace.PushCall("StencilFuncSeparate", out.str(), namedParams);
  }

  inline void StencilMask(GLuint mask) override
  {
    std::stringstream out;
    out << mask;

    TraceCallStack::NamedParams namedParams;
    namedParams["mask"] << mask;

    mStencilFunctionTrace.PushCall("StencilMask", out.str(), namedParams);
  }

  inline void StencilMaskSeparate(GLenum face, GLuint mask) override
  {
    std::stringstream out;
    out << face << ", " << mask;

    TraceCallStack::NamedParams namedParams;
    namedParams["face"] << std::hex << face;
    namedParams["mask"] << mask;

    mStencilFunctionTrace.PushCall("StencilMaskSeparate", out.str(), namedParams);
  }

  inline void StencilOp(GLenum fail, GLenum zfail, GLenum zpass) override
  {
    std::stringstream out;
    out << fail << ", " << zfail << ", " << zpass;

    TraceCallStack::NamedParams namedParams;
    namedParams["fail"] << std::hex << fail;
    namedParams["zfail"] << std::hex << zfail;
    namedParams["zpass"] << std::hex << zpass;

    mStencilFunctionTrace.PushCall("StencilOp", out.str(), namedParams);
  }

  inline void StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass) override
  {
    std::stringstream out;
    out << face << ", " << fail << ", " << zfail << "," << zpass;

    TraceCallStack::NamedParams namedParams;
    namedParams["face"] << std::hex << face;
    namedParams["fail"] << std::hex << fail;
    namedParams["zfail"] << std::hex << zfail;
    namedParams["zpass"] << std::hex << zpass;

    mStencilFunctionTrace.PushCall("StencilOpSeparate", out.str(), namedParams);
  }

  inline void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) override
  {
    std::stringstream out;
    out << target << ", " << level << ", " << width << ", " << height;

    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["level"] << level;
    namedParams["internalformat"] << internalformat;
    namedParams["width"] << width;
    namedParams["height"] << height;
    namedParams["border"] << border;
    namedParams["format"] << std::hex << format;
    namedParams["type"] << std::hex << type;

    mTextureTrace.PushCall("TexImage2D", out.str(), namedParams);
  }

  inline void TexParameterf(GLenum target, GLenum pname, GLfloat param) override
  {
    std::stringstream out;
    out << target << ", " << pname << ", " << param;

    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["pname"] << std::hex << pname;
    namedParams["param"] << param;

    mTexParameterTrace.PushCall("TexParameterf", out.str(), namedParams);
  }

  inline void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params) override
  {
    std::stringstream out;
    out << target << ", " << pname << ", " << params[0];

    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["pname"] << std::hex << pname;
    namedParams["params[0]"] << params[0];

    mTexParameterTrace.PushCall("TexParameterfv", out.str(), namedParams);
  }

  inline void TexParameteri(GLenum target, GLenum pname, GLint param) override
  {
    std::stringstream out;
    out << std::hex << target << ", " << pname << ", " << param;
    std::string params = out.str();

    out.str("");
    out << std::hex << target;
    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << out.str();
    out.str("");
    out << std::hex << pname;
    namedParams["pname"] << out.str();
    out.str("");
    out << std::hex << param;
    namedParams["param"] << out.str();
    mTexParameterTrace.PushCall("TexParameteri", params, namedParams);
  }

  inline void TexParameteriv(GLenum target, GLenum pname, const GLint* params) override
  {
    std::stringstream out;
    out << target << ", " << pname << ", " << params[0];
    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["pname"] << std::hex << pname;
    namedParams["params[0]"] << params[0];
    mTexParameterTrace.PushCall("TexParameteriv", out.str(), namedParams);
  }

  inline void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) override
  {
    std::stringstream out;
    out << target << ", " << level << ", " << xoffset << ", " << yoffset << ", " << width << ", " << height;

    TraceCallStack::NamedParams namedParams;
    namedParams["target"] << std::hex << target;
    namedParams["level"] << level;
    namedParams["xoffset"] << xoffset;
    namedParams["yoffset"] << yoffset;
    namedParams["width"] << width;
    namedParams["height"] << height;
    mTextureTrace.PushCall("TexSubImage2D", out.str(), namedParams);
  }

  inline void Uniform1f(GLint location, GLfloat value) override
  {
    std::string params = std::to_string(value);
    AddUniformCallToTraceStack(location, params);

    if(!mProgramUniforms1f.SetUniformValue(mCurrentProgram, location, value))
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform1fv(GLint location, GLsizei count, const GLfloat* v) override
  {
    std::string params;
    for(int i = 0; i < count; ++i)
    {
      params = params + std::to_string(v[i]) + ",";
    }

    AddUniformCallToTraceStack(location, params);

    for(int i = 0; i < count; ++i)
    {
      if(!mProgramUniforms1f.SetUniformValue(mCurrentProgram, location, v[i]))
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform1i(GLint location, GLint x) override
  {
    std::string params = std::to_string(x);

    AddUniformCallToTraceStack(location, params);

    if(!mProgramUniforms1i.SetUniformValue(mCurrentProgram, location, x))
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform1iv(GLint location, GLsizei count, const GLint* v) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << v[i];
    AddUniformCallToTraceStack(location, out.str());

    for(int i = 0; i < count; ++i)
    {
      if(!mProgramUniforms1i.SetUniformValue(mCurrentProgram,
                                             location,
                                             v[i]))
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform2f(GLint location, GLfloat x, GLfloat y) override
  {
    std::string params = std::to_string(x) + "," + std::to_string(y);
    AddUniformCallToTraceStack(location, params);

    if(!mProgramUniforms2f.SetUniformValue(mCurrentProgram,
                                           location,
                                           Vector2(x, y)))
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform2fv(GLint location, GLsizei count, const GLfloat* v) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << v[i];
    AddUniformCallToTraceStack(location, out.str());

    for(int i = 0; i < count; ++i)
    {
      if(!mProgramUniforms2f.SetUniformValue(mCurrentProgram,
                                             location,
                                             Vector2(v[2 * i], v[2 * i + 1])))
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform2i(GLint location, GLint x, GLint y) override
  {
    std::string params = std::to_string(x) + "," + std::to_string(y);
    AddUniformCallToTraceStack(location, params);
  }

  inline void Uniform2iv(GLint location, GLsizei count, const GLint* v) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << v[i];
    AddUniformCallToTraceStack(location, out.str());
  }

  inline void Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z) override
  {
    std::string params = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
    AddUniformCallToTraceStack(location, params);

    if(!mProgramUniforms3f.SetUniformValue(mCurrentProgram,
                                           location,
                                           Vector3(x, y, z)))
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform3fv(GLint location, GLsizei count, const GLfloat* v) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << v[i];
    AddUniformCallToTraceStack(location, out.str());

    for(int i = 0; i < count; ++i)
    {
      if(!mProgramUniforms3f.SetUniformValue(
           mCurrentProgram,
           location,
           Vector3(v[3 * i], v[3 * i + 1], v[3 * i + 2])))
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform3i(GLint location, GLint x, GLint y, GLint z) override
  {
    std::string params = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
    AddUniformCallToTraceStack(location, params);
  }

  inline void Uniform3iv(GLint location, GLsizei count, const GLint* v) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << v[i];
    AddUniformCallToTraceStack(location, out.str());
  }

  inline void Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) override
  {
    std::string params = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(w);
    AddUniformCallToTraceStack(location, params);

    if(!mProgramUniforms4f.SetUniformValue(mCurrentProgram,
                                           location,
                                           Vector4(x, y, z, w)))
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  inline void Uniform4fv(GLint location, GLsizei count, const GLfloat* v) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << v[i];
    AddUniformCallToTraceStack(location, out.str());

    for(int i = 0; i < count; ++i)
    {
      if(!mProgramUniforms4f.SetUniformValue(
           mCurrentProgram,
           location,
           Vector4(v[4 * i], v[4 * i + 1], v[4 * i + 2], v[4 * i + 3])))
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w) override
  {
    std::string params = std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(w);
    AddUniformCallToTraceStack(location, params);
  }

  inline void Uniform4iv(GLint location, GLsizei count, const GLint* v) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << v[i];
    AddUniformCallToTraceStack(location, out.str());
  }

  inline void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << value[i];
    AddUniformCallToTraceStack(location, out.str());
  }

  inline void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << value[i];
    AddUniformCallToTraceStack(location, out.str());

    for(int i = 0; i < count; ++i)
    {
      if(!mProgramUniformsMat3.SetUniformValue(
           mCurrentProgram,
           location,
           Matrix3(value[0], value[1], value[2], value[3], value[4], value[5], value[6], value[7], value[8])))
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::ostringstream out;
    for(GLsizei i = 0; i < count; ++i) out << (!i ? "" : ", ") << value[i];
    AddUniformCallToTraceStack(location, out.str());

    for(int i = 0; i < count; ++i)
    {
      if(!mProgramUniformsMat4.SetUniformValue(
           mCurrentProgram,
           location,
           Matrix(value)))
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  inline void UseProgram(GLuint program) override
  {
    mCurrentProgram = program;
  }

  inline void ValidateProgram(GLuint program) override
  {
  }

  inline void VertexAttrib1f(GLuint indx, GLfloat x) override
  {
  }

  inline void VertexAttrib1fv(GLuint indx, const GLfloat* values) override
  {
  }

  inline void VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y) override
  {
  }

  inline void VertexAttrib2fv(GLuint indx, const GLfloat* values) override
  {
  }

  inline void VertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z) override
  {
  }

  inline void VertexAttrib3fv(GLuint indx, const GLfloat* values) override
  {
  }

  inline void VertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w) override
  {
  }

  inline void VertexAttrib4fv(GLuint indx, const GLfloat* values) override
  {
  }

  inline void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr) override
  {
    TraceCallStack::NamedParams namedParams;
    namedParams["index"] << index;
    namedParams["size"] << size;
    namedParams["type"] << std::hex << type;
    namedParams["normalized"] << (normalized ? "T" : "F");
    namedParams["stride"] << stride;
    namedParams["offset"] << std::to_string(reinterpret_cast<unsigned long>(ptr));

    mBufferTrace.PushCall("VertexAttribPointer", namedParams.str(), namedParams);
  }

  inline void Viewport(GLint x, GLint y, GLsizei width, GLsizei height) override
  {
    std::string commaString(", ");
    std::string params(std::to_string(x) + commaString + std::to_string(y) + commaString + std::to_string(width) + commaString + std::to_string(height));

    mViewportTrace.PushCall("Viewport", params);
  }

  /* OpenGL ES 3.0 */

  inline void ReadBuffer(GLenum mode) override
  {
  }

  inline void DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices) override
  {
  }

  inline void TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels) override
  {
  }

  inline void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels) override
  {
  }

  inline void CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) override
  {
  }

  inline void CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data) override
  {
  }

  inline void CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data) override
  {
  }

  inline void GenQueries(GLsizei n, GLuint* ids) override
  {
  }

  inline void DeleteQueries(GLsizei n, const GLuint* ids) override
  {
  }

  inline GLboolean IsQuery(GLuint id) override
  {
    return false;
  }

  inline void BeginQuery(GLenum target, GLuint id) override
  {
  }

  inline void EndQuery(GLenum target) override
  {
  }

  inline void GetQueryiv(GLenum target, GLenum pname, GLint* params) override
  {
  }

  inline void GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params) override
  {
  }

  inline GLboolean UnmapBuffer(GLenum target) override
  {
    return false;
  }

  inline void GetBufferPointerv(GLenum target, GLenum pname, GLvoid** params) override
  {
  }

  inline void DrawBuffers(GLsizei n, const GLenum* bufs) override
  {
  }

  inline void UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
  }

  inline void UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
  }

  inline void UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
  }

  inline void UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
  }

  inline void UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
  }

  inline void UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
  }

  inline void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) override
  {
  }

  inline void RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) override
  {
  }

  inline void FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) override
  {
  }

  inline GLvoid* MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) override
  {
    return NULL;
  }

  inline void FlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length) override
  {
  }

  inline void BindVertexArray(GLuint array) override
  {
  }

  inline void DeleteVertexArrays(GLsizei n, const GLuint* arrays) override
  {
  }

  inline void GenVertexArrays(GLsizei n, GLuint* arrays) override
  {
  }

  inline GLboolean IsVertexArray(GLuint array) override
  {
    return false;
  }

  inline void GetIntegeri_v(GLenum target, GLuint index, GLint* data) override
  {
  }

  inline void BeginTransformFeedback(GLenum primitiveMode) override
  {
  }

  inline void EndTransformFeedback(void) override
  {
  }

  inline void BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) override
  {
  }

  inline void BindBufferBase(GLenum target, GLuint index, GLuint buffer) override
  {
  }

  inline void TransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode) override
  {
  }

  inline void GetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name) override
  {
  }

  inline void VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) override
  {
  }

  inline void GetVertexAttribIiv(GLuint index, GLenum pname, GLint* params) override
  {
  }

  inline void GetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params) override
  {
  }

  inline void VertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w) override
  {
  }

  inline void VertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) override
  {
  }

  inline void VertexAttribI4iv(GLuint index, const GLint* v) override
  {
  }

  inline void VertexAttribI4uiv(GLuint index, const GLuint* v) override
  {
  }

  inline void GetUniformuiv(GLuint program, GLint location, GLuint* params) override
  {
  }

  inline GLint GetFragDataLocation(GLuint program, const GLchar* name) override
  {
    return -1;
  }

  inline void Uniform1ui(GLint location, GLuint v0) override
  {
  }

  inline void Uniform2ui(GLint location, GLuint v0, GLuint v1) override
  {
  }

  inline void Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2) override
  {
  }

  inline void Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) override
  {
  }

  inline void Uniform1uiv(GLint location, GLsizei count, const GLuint* value) override
  {
  }

  inline void Uniform2uiv(GLint location, GLsizei count, const GLuint* value) override
  {
  }

  inline void Uniform3uiv(GLint location, GLsizei count, const GLuint* value) override
  {
  }

  inline void Uniform4uiv(GLint location, GLsizei count, const GLuint* value) override
  {
  }

  inline void ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value) override
  {
  }

  inline void ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value) override
  {
  }

  inline void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value) override
  {
  }

  inline void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) override
  {
  }

  inline const GLubyte* GetStringi(GLenum name, GLuint index) override
  {
    return NULL;
  }

  inline void CopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) override
  {
  }

  inline void GetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices) override
  {
  }

  inline void GetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params) override
  {
  }

  inline GLuint GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName) override
  {
    return GL_INVALID_INDEX;
  }

  inline void GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params) override
  {
  }

  inline void GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName) override
  {
  }

  inline void UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) override
  {
  }

  inline void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount) override
  {
  }

  inline void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount) override
  {
  }

  inline GLsync FenceSync(GLenum condition, GLbitfield flags) override
  {
    return NULL;
  }

  inline GLboolean IsSync(GLsync sync) override
  {
    return false;
  }

  inline void DeleteSync(GLsync sync) override
  {
  }

  inline GLenum ClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) override
  {
    return 0;
  }

  inline void WaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) override
  {
  }

  inline void GetInteger64v(GLenum pname, GLint64* params) override
  {
  }

  inline void GetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values) override
  {
  }

  inline void GetInteger64i_v(GLenum target, GLuint index, GLint64* data) override
  {
  }

  inline void GetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params) override
  {
  }

  inline void GenSamplers(GLsizei count, GLuint* samplers) override
  {
  }

  inline void DeleteSamplers(GLsizei count, const GLuint* samplers) override
  {
  }

  inline GLboolean IsSampler(GLuint sampler) override
  {
    return false;
  }

  inline void BindSampler(GLuint unit, GLuint sampler) override
  {
  }

  inline void SamplerParameteri(GLuint sampler, GLenum pname, GLint param) override
  {
  }

  inline void SamplerParameteriv(GLuint sampler, GLenum pname, const GLint* param) override
  {
  }

  inline void SamplerParameterf(GLuint sampler, GLenum pname, GLfloat param) override
  {
  }

  inline void SamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat* param) override
  {
  }

  inline void GetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params) override
  {
  }

  inline void GetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat* params) override
  {
  }

  inline void VertexAttribDivisor(GLuint index, GLuint divisor) override
  {
  }

  inline void BindTransformFeedback(GLenum target, GLuint id) override
  {
  }

  inline void DeleteTransformFeedbacks(GLsizei n, const GLuint* ids) override
  {
  }

  inline void GenTransformFeedbacks(GLsizei n, GLuint* ids) override
  {
  }

  inline GLboolean IsTransformFeedback(GLuint id) override
  {
    return false;
  }

  inline void PauseTransformFeedback(void) override
  {
  }

  inline void ResumeTransformFeedback(void) override
  {
  }

  inline void GetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary) override
  {
    mGetProgramBinaryCalled = true;
  }

  inline void ProgramBinary(GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length) override
  {
  }

  inline void ProgramParameteri(GLuint program, GLenum pname, GLint value) override
  {
  }

  inline void InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments) override
  {
  }

  inline void InvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height) override
  {
  }

  inline void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) override
  {
  }

  inline void TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) override
  {
  }

  inline void GetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params) override
  {
  }

  inline void BlendBarrier(void)
  {
  }

private:
  inline void AddUniformCallToTraceStack(GLint location, const std::string& value)
  {
    std::string name    = "<not found>";
    bool        matched = false;

    UniformIDMap& map = mUniforms[mCurrentProgram];
    for(UniformIDMap::iterator it = map.begin(); it != map.end(); ++it)
    {
      if(it->second == location)
      {
        name    = it->first;
        matched = true;
        break;
      }
    }

    if(matched)
    {
      mSetUniformTrace.PushCall(name, value);
    }
  }

public: // TEST FUNCTIONS
  inline void SetCompileStatus(GLuint value)
  {
    mCompileStatus = value;
  }
  inline void SetLinkStatus(GLuint value)
  {
    mLinkStatus = value;
  }
  inline void SetAttribLocations(std::vector<std::string> locs)
  {
    mAttribLocs = locs;
  }
  inline void SetGetErrorResult(GLenum result)
  {
    mGetErrorResult = result;
  }
  inline void SetGetStringResult(GLubyte* result)
  {
    mGetStringResult = result;
  }
  inline void SetIsBufferResult(GLboolean result)
  {
    mIsBufferResult = result;
  }
  inline void SetIsEnabledResult(GLboolean result)
  {
    mIsEnabledResult = result;
  }
  inline void SetIsFramebufferResult(GLboolean result)
  {
    mIsFramebufferResult = result;
  }
  inline void SetIsProgramResult(GLboolean result)
  {
    mIsProgramResult = result;
  }
  inline void SetIsRenderbufferResult(GLboolean result)
  {
    mIsRenderbufferResult = result;
  }
  inline void SetIsShaderResult(GLboolean result)
  {
    mIsShaderResult = result;
  }
  inline void SetIsTextureResult(GLboolean result)
  {
    mIsTextureResult = result;
  }
  inline void SetCheckFramebufferStatusResult(GLenum result)
  {
    mCheckFramebufferStatusResult = result;
  }
  inline void SetNumBinaryFormats(GLint numFormats)
  {
    mNumBinaryFormats = numFormats;
  }
  inline void SetBinaryFormats(GLint binaryFormats)
  {
    mBinaryFormats = binaryFormats;
  }
  inline void SetProgramBinaryLength(GLint length)
  {
    mProgramBinaryLength = length;
  }

  inline bool GetVertexAttribArrayState(GLuint index)
  {
    if(index >= MAX_ATTRIBUTE_CACHE_SIZE)
    {
      // out of range
      return false;
    }
    return mVertexAttribArrayState[index];
  }
  inline void ClearVertexAttribArrayChanged()
  {
    mVertexAttribArrayChanged = false;
  }
  inline bool GetVertexAttribArrayChanged()
  {
    return mVertexAttribArrayChanged;
  }

  //Methods for CullFace verification
  inline void EnableCullFaceCallTrace(bool enable)
  {
    mCullFaceTrace.Enable(enable);
  }
  inline void ResetCullFaceCallStack()
  {
    mCullFaceTrace.Reset();
  }
  inline TraceCallStack& GetCullFaceTrace()
  {
    return mCullFaceTrace;
  }

  //Methods for Enable/Disable call verification
  inline void EnableEnableDisableCallTrace(bool enable)
  {
    mEnableDisableTrace.Enable(enable);
  }
  inline void ResetEnableDisableCallStack()
  {
    mEnableDisableTrace.Reset();
  }
  inline TraceCallStack& GetEnableDisableTrace()
  {
    return mEnableDisableTrace;
  }

  //Methods for Shader verification
  inline void EnableShaderCallTrace(bool enable)
  {
    mShaderTrace.Enable(enable);
  }
  inline void ResetShaderCallStack()
  {
    mShaderTrace.Reset();
  }
  inline TraceCallStack& GetShaderTrace()
  {
    return mShaderTrace;
  }

  //Methods for Texture verification
  inline void EnableTextureCallTrace(bool enable)
  {
    mTextureTrace.Enable(enable);
  }
  inline void ResetTextureCallStack()
  {
    mTextureTrace.Reset();
  }
  inline TraceCallStack& GetTextureTrace()
  {
    return mTextureTrace;
  }

  //Methods for Texture verification
  inline void EnableTexParameterCallTrace(bool enable)
  {
    mTexParameterTrace.Enable(enable);
  }
  inline void ResetTexParameterCallStack()
  {
    mTexParameterTrace.Reset();
  }
  inline TraceCallStack& GetTexParameterTrace()
  {
    return mTexParameterTrace;
  }

  //Methods for Draw verification
  inline void EnableDrawCallTrace(bool enable)
  {
    mDrawTrace.Enable(enable);
  }
  inline void ResetDrawCallStack()
  {
    mDrawTrace.Reset();
  }
  inline TraceCallStack& GetDrawTrace()
  {
    return mDrawTrace;
  }

  //Methods for Depth function verification
  inline void EnableDepthFunctionCallTrace(bool enable)
  {
    mDepthFunctionTrace.Enable(enable);
  }
  inline void ResetDepthFunctionCallStack()
  {
    mDepthFunctionTrace.Reset();
  }
  inline TraceCallStack& GetDepthFunctionTrace()
  {
    return mDepthFunctionTrace;
  }

  //Methods for Stencil function verification
  inline void EnableStencilFunctionCallTrace(bool enable)
  {
    mStencilFunctionTrace.Enable(enable);
  }
  inline void ResetStencilFunctionCallStack()
  {
    mStencilFunctionTrace.Reset();
  }
  inline TraceCallStack& GetStencilFunctionTrace()
  {
    return mStencilFunctionTrace;
  }

  //Methods for Scissor verification
  inline void EnableScissorCallTrace(bool enable)
  {
    mScissorTrace.Enable(enable);
  }
  inline void ResetScissorCallStack()
  {
    mScissorTrace.Reset();
  }
  inline TraceCallStack& GetScissorTrace()
  {
    return mScissorTrace;
  }

  //Methods for Uniform function verification
  inline void EnableSetUniformCallTrace(bool enable)
  {
    mSetUniformTrace.Enable(enable);
  }
  inline void ResetSetUniformCallStack()
  {
    mSetUniformTrace.Reset();
  }
  inline TraceCallStack& GetSetUniformTrace()
  {
    return mSetUniformTrace;
  }

  //Methods for Viewport verification
  inline void EnableViewportCallTrace(bool enable)
  {
    mViewportTrace.Enable(enable);
  }
  inline void ResetViewportCallStack()
  {
    mViewportTrace.Reset();
  }
  inline TraceCallStack& GetViewportTrace()
  {
    return mViewportTrace;
  }
  inline TraceCallStack& GetBufferTrace()
  {
    return mBufferTrace;
  }

  template<typename T>
  inline bool GetUniformValue(const char* name, T& value) const
  {
    for(ProgramUniformMap::const_iterator program_it = mUniforms.begin();
        program_it != mUniforms.end();
        ++program_it)
    {
      const UniformIDMap& uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find(name);
      if(uniform_it != uniformIDs.end())
      {
        // found one matching uniform name, lets check the value...
        GLuint programId = program_it->first;
        GLint  uniformId = uniform_it->second;

        const ProgramUniformValue<T>& mProgramUniforms = GetProgramUniformsForType(value);
        return mProgramUniforms.GetUniformValue(programId, uniformId, value);
      }
    }
    return false;
  }

  template<typename T>
  inline bool CheckUniformValue(const char* name, const T& value) const
  {
    for(ProgramUniformMap::const_iterator program_it = mUniforms.begin();
        program_it != mUniforms.end();
        ++program_it)
    {
      const UniformIDMap& uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find(name);
      if(uniform_it != uniformIDs.end())
      {
        // found one matching uniform name, lets check the value...
        GLuint programId = program_it->first;
        GLint  uniformId = uniform_it->second;

        const ProgramUniformValue<T>& mProgramUniforms = GetProgramUniformsForType(value);
        if(mProgramUniforms.CheckUniformValue(programId, uniformId, value))
        {
          // the value matches
          return true;
        }
      }
    }

    fprintf(stderr, "Not found, printing possible values:\n");
    for(ProgramUniformMap::const_iterator program_it = mUniforms.begin();
        program_it != mUniforms.end();
        ++program_it)
    {
      const UniformIDMap& uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find(name);
      if(uniform_it != uniformIDs.end())
      {
        // found one matching uniform name, lets check the value...
        GLuint programId = program_it->first;
        GLint  uniformId = uniform_it->second;

        const ProgramUniformValue<T>& mProgramUniforms = GetProgramUniformsForType(value);
        T                             origValue;
        if(mProgramUniforms.GetUniformValue(programId, uniformId, origValue))
        {
          std::stringstream out;
          out << uniform_it->first << ": " << origValue;
          fprintf(stderr, "%s\n", out.str().c_str());
        }
      }
    }
    return false;
  }

  template<typename T>
  inline bool GetUniformValue(GLuint programId, GLuint uniformId, T& outValue) const
  {
    const ProgramUniformValue<T>& mProgramUniforms = GetProgramUniformsForType(outValue);
    return mProgramUniforms.GetUniformValue(programId, uniformId, outValue);
  }

  inline bool GetUniformIds(const char* name, GLuint& programId, GLuint& uniformId) const
  {
    for(ProgramUniformMap::const_iterator program_it = mUniforms.begin();
        program_it != mUniforms.end();
        ++program_it)
    {
      const UniformIDMap& uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find(name);
      if(uniform_it != uniformIDs.end())
      {
        programId = program_it->first;
        uniformId = uniform_it->second;
        return true;
      }
    }
    return false;
  }

  inline GLuint GetLastShaderCompiled() const
  {
    return mLastShaderCompiled;
  }

  inline GLuint GetLastProgramCreated() const
  {
    return mLastProgramIdUsed;
  }

  inline GLbitfield GetLastClearMask() const
  {
    return mLastClearBitMask;
  }

  enum AttribType
  {
    ATTRIB_UNKNOWN = -1,
    ATTRIB_POSITION,
    ATTRIB_NORMAL,
    ATTRIB_TEXCOORD,
    ATTRIB_COLOR,
    ATTRIB_BONE_WEIGHTS,
    ATTRIB_BONE_INDICES,
    ATTRIB_TYPE_LAST
  };

  struct ScissorParams
  {
    GLint   x;
    GLint   y;
    GLsizei width;
    GLsizei height;

    ScissorParams()
    : x(0),
      y(0),
      width(0),
      height(0)
    {
    }
  };

  // Methods to check scissor tests
  inline const ScissorParams& GetScissorParams() const
  {
    return mScissorParams;
  }

  struct ColorMaskParams
  {
    GLboolean red;
    GLboolean green;
    GLboolean blue;
    GLboolean alpha;

    ColorMaskParams()
    : red(true),
      green(true),
      blue(true),
      alpha(true)
    {
    }
  };

  inline bool GetProgramBinaryCalled() const
  {
    return mGetProgramBinaryCalled;
  }

  inline unsigned int GetClearCountCalled() const
  {
    return mClearCount;
  }

  inline const ColorMaskParams& GetColorMaskParams() const
  {
    return mColorMaskParams;
  }

  typedef std::vector<size_t>   BufferDataCalls;
  inline const BufferDataCalls& GetBufferDataCalls() const
  {
    return mBufferDataCalls;
  }
  inline void ResetBufferDataCalls()
  {
    mBufferDataCalls.clear();
  }

  typedef std::vector<size_t>      BufferSubDataCalls;
  inline const BufferSubDataCalls& GetBufferSubDataCalls() const
  {
    return mBufferSubDataCalls;
  }
  inline void ResetBufferSubDataCalls()
  {
    mBufferSubDataCalls.clear();
  }

private:
  GLuint                                mCurrentProgram;
  GLuint                                mCompileStatus;
  BufferDataCalls                       mBufferDataCalls;
  BufferSubDataCalls                    mBufferSubDataCalls;
  GLuint                                mLinkStatus;
  GLint                                 mNumberOfActiveUniforms;
  GLenum                                mGetErrorResult;
  GLubyte*                              mGetStringResult;
  GLboolean                             mIsBufferResult;
  GLboolean                             mIsEnabledResult;
  GLboolean                             mIsFramebufferResult;
  GLboolean                             mIsProgramResult;
  GLboolean                             mIsRenderbufferResult;
  GLboolean                             mIsShaderResult;
  GLboolean                             mIsTextureResult;
  GLenum                                mActiveTextureUnit;
  GLenum                                mCheckFramebufferStatusResult;
  GLint                                 mFramebufferStatus;
  GLenum                                mFramebufferDepthAttached;
  GLenum                                mFramebufferStencilAttached;
  GLuint                                mFramebufferColorAttachmentCount;
  GLuint                                mFrameBufferColorStatus;
  GLint                                 mNumBinaryFormats;
  GLint                                 mBinaryFormats;
  GLint                                 mProgramBinaryLength;
  bool                                  mVertexAttribArrayState[MAX_ATTRIBUTE_CACHE_SIZE];
  bool                                  mVertexAttribArrayChanged; // whether the vertex attrib array has been changed
  bool                                  mGetProgramBinaryCalled;
  typedef std::map<GLuint, std::string> ShaderSourceMap;
  ShaderSourceMap                       mShaderSources;
  std::vector<std::string>              mAttribLocs; // should be bound to shader
  GLuint                                mLastShaderCompiled;
  GLbitfield                            mLastClearBitMask;
  Vector4                               mLastClearColor;
  unsigned int                          mClearCount;

  Vector4 mLastBlendColor;
  GLenum  mLastBlendEquationRgb;
  GLenum  mLastBlendEquationAlpha;
  GLenum  mLastBlendFuncSrcRgb;
  GLenum  mLastBlendFuncDstRgb;
  GLenum  mLastBlendFuncSrcAlpha;
  GLenum  mLastBlendFuncDstAlpha;

  GLboolean mLastDepthMask;

  // Data for manipulating the IDs returned by GenTextures
  GLuint              mLastAutoTextureIdUsed;
  GLuint              mNumGeneratedTextures;
  std::vector<GLuint> mNextTextureIds;
  std::vector<GLuint> mDeletedTextureIds;
  std::vector<GLuint> mBoundTextures;

  struct ActiveTextureType
  {
    std::vector<GLuint> mBoundTextures;
  };

  ActiveTextureType mActiveTextures[MIN_TEXTURE_UNIT_LIMIT];

  TraceCallStack mBufferTrace;
  TraceCallStack mCullFaceTrace;
  TraceCallStack mEnableDisableTrace;
  TraceCallStack mShaderTrace;
  TraceCallStack mTextureTrace;
  TraceCallStack mTexParameterTrace;
  TraceCallStack mDrawTrace;
  TraceCallStack mDepthFunctionTrace;
  TraceCallStack mStencilFunctionTrace;
  TraceCallStack mScissorTrace;
  TraceCallStack mSetUniformTrace;
  TraceCallStack mViewportTrace;

  // Shaders & Uniforms
  GLuint                                 mLastShaderIdUsed;
  GLuint                                 mLastProgramIdUsed;
  GLuint                                 mLastUniformIdUsed;
  typedef std::map<std::string, GLint>   UniformIDMap;
  typedef std::map<GLuint, UniformIDMap> ProgramUniformMap;
  ProgramUniformMap                      mUniforms;

  template<typename T>
  struct ProgramUniformValue : public std::map<GLuint, std::map<GLint, T> >
  {
  public:
    typedef std::map<GLint, T>                UniformValueMap;
    typedef std::map<GLuint, UniformValueMap> Map;

    bool SetUniformValue(GLuint program, GLuint uniform, const T& value)
    {
      if(program == 0)
      {
        return false;
      }

      typename Map::iterator it = Map::find(program);
      if(it == Map::end())
      {
        // if its the first uniform for this program add it
        std::pair<typename Map::iterator, bool> result =
          Map::insert(typename Map::value_type(program, UniformValueMap()));
        it = result.first;
      }

      UniformValueMap& uniforms = it->second;
      uniforms[uniform]         = value;

      return true;
    }

    bool CheckUniformValue(GLuint program, GLuint uniform, const T& value) const
    {
      T uniformValue;
      if(GetUniformValue(program, uniform, uniformValue))
      {
        return CompareType<T>(value, uniformValue, Math::MACHINE_EPSILON_10);
      }

      return false;
    }

    bool GetUniformValue(GLuint program, GLuint uniform, T& value) const
    {
      if(program == 0)
      {
        return false;
      }

      typename Map::const_iterator it = Map::find(program);
      if(it == Map::end())
      {
        // Uniform values always initialised as 0
        value = GetZero();
        return true;
      }

      const UniformValueMap&                   uniforms = it->second;
      typename UniformValueMap::const_iterator it2      = uniforms.find(uniform);
      if(it2 == uniforms.end())
      {
        // Uniform values always initialised as 0
        value = GetZero();
        return true;
      }
      value = it2->second;

      return true;
    }

    T GetZero() const;
  };
  ProgramUniformValue<int>     mProgramUniforms1i;
  ProgramUniformValue<float>   mProgramUniforms1f;
  ProgramUniformValue<Vector2> mProgramUniforms2f;
  ProgramUniformValue<Vector3> mProgramUniforms3f;
  ProgramUniformValue<Vector4> mProgramUniforms4f;
  ProgramUniformValue<Matrix>  mProgramUniformsMat4;
  ProgramUniformValue<Matrix3> mProgramUniformsMat3;

  inline const ProgramUniformValue<int>& GetProgramUniformsForType(const int) const
  {
    return mProgramUniforms1i;
  }
  inline const ProgramUniformValue<float>& GetProgramUniformsForType(const float) const
  {
    return mProgramUniforms1f;
  }
  inline const ProgramUniformValue<Vector2>& GetProgramUniformsForType(const Vector2&) const
  {
    return mProgramUniforms2f;
  }
  inline const ProgramUniformValue<Vector3>& GetProgramUniformsForType(const Vector3&) const
  {
    return mProgramUniforms3f;
  }
  inline const ProgramUniformValue<Vector4>& GetProgramUniformsForType(const Vector4&) const
  {
    return mProgramUniforms4f;
  }
  inline const ProgramUniformValue<Matrix>& GetProgramUniformsForType(const Matrix&) const
  {
    return mProgramUniformsMat4;
  }
  inline const ProgramUniformValue<Matrix3>& GetProgramUniformsForType(const Matrix3&) const
  {
    return mProgramUniformsMat3;
  }
  inline void SetVertexAttribArray(GLuint index, bool state)
  {
    if(index >= MAX_ATTRIBUTE_CACHE_SIZE)
    {
      // out of range
      return;
    }
    mVertexAttribArrayState[index] = state;
    mVertexAttribArrayChanged      = true;
  }

  ScissorParams   mScissorParams;
  ColorMaskParams mColorMaskParams;
};

template<>
inline int TestGlAbstraction::ProgramUniformValue<int>::GetZero() const
{
  return 0;
}

template<>
inline float TestGlAbstraction::ProgramUniformValue<float>::GetZero() const
{
  return 0.0f;
}

template<>
inline Vector2 TestGlAbstraction::ProgramUniformValue<Vector2>::GetZero() const
{
  return Vector2::ZERO;
}

template<>
inline Vector3 TestGlAbstraction::ProgramUniformValue<Vector3>::GetZero() const
{
  return Vector3::ZERO;
}

template<>
inline Vector4 TestGlAbstraction::ProgramUniformValue<Vector4>::GetZero() const
{
  return Vector4::ZERO;
}

template<>
inline Matrix TestGlAbstraction::ProgramUniformValue<Matrix>::GetZero() const
{
  return Matrix();
}

template<>
inline Matrix3 TestGlAbstraction::ProgramUniformValue<Matrix3>::GetZero() const
{
  return Matrix3(Matrix());
}

} // namespace Dali

bool BlendEnabled(const Dali::TraceCallStack& callStack);
bool BlendDisabled(const Dali::TraceCallStack& callStack);

#endif // TEST_GL_ABSTRACTION_H
