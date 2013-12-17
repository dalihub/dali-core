#ifndef __TEST_GL_ABSTRACTION_H__
#define __TEST_GL_ABSTRACTION_H__

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

// EXTERNAL INCLUDES
#include <sstream>
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

#include <dali/integration-api/core.h>
#include <dali/integration-api/gl-abstraction.h>
#include <test-trace-call-stack.h>

namespace Dali
{

static const unsigned int MAX_ATTRIBUTE_CACHE_SIZE = 64;
static const char *mStdAttribs[MAX_ATTRIBUTE_CACHE_SIZE] =
{
    "aPosition",    // ATTRIB_POSITION
    "aNormal",      // ATTRIB_NORMAL
    "aTexCoord",    // ATTRIB_TEXCOORD
    "aColor",       // ATTRIB_COLOR
    "aBoneWeights", // ATTRIB_BONE_WEIGHTS
    "aBoneIndices"  // ATTRIB_BONE_INDICES
};

class DALI_IMPORT_API TestGlAbstraction: public Dali::Integration::GlAbstraction
{
public:
  TestGlAbstraction()
  {
    Initialize();
  }

  ~TestGlAbstraction() {}

  void Initialize()
  {
    mCurrentProgram = 0;
    mCompileStatus = GL_TRUE;
    mLinkStatus = GL_TRUE;

    mGetAttribLocationResult = 0;
    mGetErrorResult = 0;
    mGetStringResult = NULL;
    mIsBufferResult = 0;
    mIsEnabledResult = 0;
    mIsFramebufferResult = 0;
    mIsProgramResult = 0;
    mIsRenderbufferResult = 0;
    mIsShaderResult = 0;
    mIsTextureResult = 0;
    mVertexAttribArrayChanged = false;

    mCheckFramebufferStatusResult = 0;
    mNumBinaryFormats = 1;
    mBinaryFormats = 1;
    mProgramBinaryLength = 0;

    mLastAutoTextureIdUsed = 0;

    mLastShaderIdUsed = 0;
    mLastProgramIdUsed = 0;
    mLastUniformIdUsed = 0;
    mLastShaderCompiled = 0;

    mLastBlendEquationRgb   = 0;
    mLastBlendEquationAlpha = 0;
    mLastBlendFuncSrcRgb    = 0;
    mLastBlendFuncDstRgb    = 0;
    mLastBlendFuncSrcAlpha  = 0;
    mLastBlendFuncDstAlpha  = 0;

    mUniforms.clear();
    mProgramUniforms1i.clear();
    mProgramUniforms1f.clear();
    mProgramUniforms2f.clear();
    mProgramUniforms3f.clear();
    mProgramUniforms4f.clear();
  }

  /* OpenGL ES 2.0 */

  void ActiveTexture( GLenum textureUnit )
  {
    mActiveTextureUnit = textureUnit - GL_TEXTURE0;
  }

  GLenum GetActiveTextureUnit() const
  {
    return mActiveTextureUnit + GL_TEXTURE0;
  }

  void AttachShader( GLuint program, GLuint shader )
  {
    std::stringstream out;
    out << program << ", " << shader;
    mShaderTrace.PushCall("AttachShader", out.str());
  }

  void BindAttribLocation( GLuint program, GLuint index, const char* name )
  {
  }

  void BindBuffer( GLenum target, GLuint buffer )
  {
  }

  void BindFramebuffer( GLenum target, GLuint framebuffer )
  {
  }

  void BindRenderbuffer( GLenum target, GLuint renderbuffer )
  {
  }

  /**
   * This method can be used by test cases, to query the texture IDs that have been bound by BindTexture.
   * @return A vector containing the IDs that were bound.
   */
  const std::vector<GLuint>& GetBoundTextures() const
  {
    return mBoundTextures;
  }

  /**
   * Query the texture IDs that have been bound with BindTexture, with a specific active texture unit.
   * @param[in] activeTextureUnit The specific active texture unit.
   * @return A vector containing the IDs that were bound.
   */
  const std::vector<GLuint>& GetBoundTextures( GLuint activeTextureUnit ) const
  {
    return mActiveTextures[ activeTextureUnit - GL_TEXTURE0 ].mBoundTextures;
  }

  /**
   * This method can be used by test cases, to clear the record of texture IDs that have been bound by BindTexture.
   */
  void ClearBoundTextures()
  {
    mBoundTextures.clear();

    for( unsigned int i=0; i<MIN_TEXTURE_UNIT_LIMIT; ++i )
    {
      mActiveTextures[ i ].mBoundTextures.clear();
    }
  }

  void BindTexture( GLenum target, GLuint texture )
  {
    // Record the bound textures for future checks
    if( texture )
    {
      mBoundTextures.push_back( texture );

      if( mActiveTextureUnit < MIN_TEXTURE_UNIT_LIMIT )
      {
        mActiveTextures[ mActiveTextureUnit ].mBoundTextures.push_back( texture );
      }
    }
  }

  void BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
  {
    mLastBlendColor.r = red;
    mLastBlendColor.g = green;
    mLastBlendColor.b = blue;
    mLastBlendColor.a = alpha;
  }

  const Vector4& GetLastBlendColor() const
  {
    return mLastBlendColor;
  }

  void BlendEquation( GLenum mode )
  {
    mLastBlendEquationRgb   = mode;
    mLastBlendEquationAlpha = mode;
  }

  void BlendEquationSeparate( GLenum modeRgb, GLenum modeAlpha )
  {
    mLastBlendEquationRgb   = modeRgb;
    mLastBlendEquationAlpha = modeAlpha;
  }

  GLenum GetLastBlendEquationRgb() const
  {
    return mLastBlendEquationRgb;
  }

  GLenum GetLastBlendEquationAlpha() const
  {
    return mLastBlendEquationAlpha;
  }

  void BlendFunc(GLenum sfactor, GLenum dfactor)
  {
    mLastBlendFuncSrcRgb = sfactor;
    mLastBlendFuncDstRgb = dfactor;
    mLastBlendFuncSrcAlpha = sfactor;
    mLastBlendFuncDstAlpha = dfactor;
  }

  void BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
  {
    mLastBlendFuncSrcRgb = srcRGB;
    mLastBlendFuncDstRgb = dstRGB;
    mLastBlendFuncSrcAlpha = srcAlpha;
    mLastBlendFuncDstAlpha = dstAlpha;
  }

  GLenum GetLastBlendFuncSrcRgb() const
  {
    return mLastBlendFuncSrcRgb;
  }

  GLenum GetLastBlendFuncDstRgb() const
  {
    return mLastBlendFuncDstRgb;
  }

  GLenum GetLastBlendFuncSrcAlpha() const
  {
    return mLastBlendFuncSrcAlpha;
  }

  GLenum GetLastBlendFuncDstAlpha() const
  {
    return mLastBlendFuncDstAlpha;
  }

  void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
  {
  }

  void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
  {
  }

  GLenum CheckFramebufferStatus(GLenum target)
  {
    return mCheckFramebufferStatusResult;
  }

  void Clear(GLbitfield mask)
  {
  }

  void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
  {
  }

  void ClearDepthf(GLclampf depth)
  {
  }

  void ClearStencil(GLint s)
  {
  }

  void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
  {
  }

  void CompileShader(GLuint shader)
  {
    std::stringstream out;
    out << shader;
    mShaderTrace.PushCall("CompileShader", out.str());
  }

  void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
  {
  }

  void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
  {
  }

  void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
  {
  }

  void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  GLuint CreateProgram(void)
  {
    mShaderTrace.PushCall("CreateProgram", "");

    ++mLastProgramIdUsed;
    mUniforms[mLastProgramIdUsed] = UniformIDMap();
    return mLastProgramIdUsed;
  }

  GLuint CreateShader(GLenum type)
  {
    std::stringstream out;
    out << type;
    mShaderTrace.PushCall("CreateShader", out.str());

    return ++mLastShaderIdUsed;
  }

  void CullFace(GLenum mode)
  {
    std::stringstream out;
    out << mode;
    mCullFaceTrace.PushCall("CullFace", out.str());
  }

  void DeleteBuffers(GLsizei n, const GLuint* buffers)
  {
  }

  void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
  {
  }

  void DeleteProgram(GLuint program)
  {
    std::stringstream out;
    out << program;
    mShaderTrace.PushCall("DeleteProgram", out.str());
  }

  void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
  {
  }

  void DeleteShader(GLuint shader)
  {
    std::stringstream out;
    out << shader;
    mShaderTrace.PushCall("DeleteShader", out.str());
  }

  void DeleteTextures(GLsizei n, const GLuint* textures)
  {
    std::stringstream out;
    out << n << ", " << textures << " = [";

    for(GLsizei i=0; i<n; i++)
    {
      out << textures[i] << ", ";
      mDeletedTextureIds.push_back(textures[i]);
    }
    out << "]";
    mTextureTrace.PushCall("DeleteTextures", out.str());
  }

  bool CheckNoTexturesDeleted()
  {
    return mDeletedTextureIds.size() == 0;
  }

  bool CheckTextureDeleted( GLuint textureId )
  {
    bool found = false;

    for(std::vector<GLuint>::iterator iter=mDeletedTextureIds.begin(); iter != mDeletedTextureIds.end(); ++iter)
    {
      if(*iter == textureId)
      {
        found = true;
        break;
      }
    }
    return found;
  }

  void ClearDeletedTextures()
  {
    mDeletedTextureIds.clear();
  }

  void DepthFunc(GLenum func)
  {
  }

  void DepthMask(GLboolean flag)
  {
  }

  void DepthRangef(GLclampf zNear, GLclampf zFar)
  {
  }

  void DetachShader(GLuint program, GLuint shader)
  {
    std::stringstream out;
    out << program << ", " << shader;
    mShaderTrace.PushCall("DetachShader", out.str());
  }

  void Disable(GLenum cap)
  {
    std::stringstream out;
    out << cap;
    mCullFaceTrace.PushCall("Disable", out.str());
  }

  void DisableVertexAttribArray(GLuint index)
  {
    SetVertexAttribArray( index, false );
  }

  void DrawArrays(GLenum mode, GLint first, GLsizei count)
  {
    std::stringstream out;
    out << mode << ", " << first << ", " << count;
    mDrawTrace.PushCall("DrawArrays", out.str());
  }

  void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
  {
    std::stringstream out;
    out << mode << ", " << count << ", " << type << ", indices";
    mDrawTrace.PushCall("DrawElements", out.str());
  }

  void Enable(GLenum cap)
  {
    std::stringstream out;
    out << cap;
    mCullFaceTrace.PushCall("Enable", out.str());
  }

  void EnableVertexAttribArray(GLuint index)
  {
    SetVertexAttribArray( index, true);
  }

  void Finish(void)
  {
  }

  void Flush(void)
  {
  }

  void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
  {
  }

  void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
  {
  }

  void FrontFace(GLenum mode)
  {
  }

  void GenBuffers(GLsizei n, GLuint* buffers)
  {
    // avoids an assert in GpuBuffers
    *buffers = 1u;
  }

  void GenerateMipmap(GLenum target)
  {
  }

  void GenFramebuffers(GLsizei n, GLuint* framebuffers)
  {
  }

  void GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
  {
  }

  /**
   * This method can be used by test cases, to manipulate the texture IDs generated by GenTextures.
   * @param[in] ids A vector containing the next IDs to be generated
   */
  void SetNextTextureIds( const std::vector<GLuint>& ids )
  {
    mNextTextureIds = ids;
  }

  const std::vector<GLuint>& GetNextTextureIds()
  {
    return mNextTextureIds;
  }

  void GenTextures(GLsizei n, GLuint* textures)
  {
    for( int i=0; i<n; ++i )
    {
      if( !mNextTextureIds.empty() )
      {
        *(textures+i) = mNextTextureIds[0];
        mNextTextureIds.erase( mNextTextureIds.begin() );
      }
      else
      {
        *(textures+i) = ++mLastAutoTextureIdUsed;
      }
    }
  }

  void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
  {
  }

  void GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
  {
  }

  void GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
  {
  }

  int  GetAttribLocation(GLuint program, const char* name)
  {
    std::string attribName(name);

    for( unsigned int i = 0; i < ATTRIB_TYPE_LAST; ++i )
    {
      if( mStdAttribs[i] == attribName )
      {
        return i;
      }
    }

    // 0 is a valid location
    return 0;
  }

  void GetBooleanv(GLenum pname, GLboolean* params)
  {
  }

  void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
  {
  }

  GLenum GetError(void)
  {
    return mGetErrorResult;
  }

  void GetFloatv(GLenum pname, GLfloat* params)
  {
  }

  void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
  {
  }

  void GetIntegerv(GLenum pname, GLint* params)
  {
    switch( pname )
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

  void GetProgramiv(GLuint program, GLenum pname, GLint* params)
  {
    switch( pname ) {
      case GL_LINK_STATUS:
        *params = mLinkStatus;
        break;
      case GL_PROGRAM_BINARY_LENGTH_OES:
        *params = mProgramBinaryLength;
        break;
    }
  }

  void GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
  {
  }


  void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
  {
  }

  void GetShaderiv(GLuint shader, GLenum pname, GLint* params)
  {
    switch( pname ) {
      case GL_COMPILE_STATUS:
        *params = mCompileStatus;
        break;
    }
  }

  void GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
  {
  }

  void GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
  {
  }

  const GLubyte* GetString(GLenum name)
  {
    return mGetStringResult;
  }

  void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
  {
  }

  void GetTexParameteriv(GLenum target, GLenum pname, GLint* params)
  {
  }

  void GetUniformfv(GLuint program, GLint location, GLfloat* params)
  {
  }

  void GetUniformiv(GLuint program, GLint location, GLint* params)
  {
  }

  GLint GetUniformLocation(GLuint program, const char* name)
  {
    ProgramUniformMap::iterator it = mUniforms.find(program);
    if( it == mUniforms.end() )
    {
      // Not a valid program ID
      mGetErrorResult = GL_INVALID_OPERATION;
      return -1;
    }

    UniformIDMap& uniformIDs = it->second;
    UniformIDMap::iterator it2 = uniformIDs.find( name );
    if( it2 == uniformIDs.end() )
    {
      // Uniform not found, so add it...
      uniformIDs[name] = ++mLastUniformIdUsed;
      return mLastUniformIdUsed;
    }

    return it2->second;
  }

  void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
  {
  }

  void GetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
  {
  }

  void GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
  {
  }

  void Hint(GLenum target, GLenum mode)
  {
  }

  GLboolean IsBuffer(GLuint buffer)
  {
    return mIsBufferResult;
  }

  GLboolean IsEnabled(GLenum cap)
  {
    return mIsEnabledResult;
  }

  GLboolean IsFramebuffer(GLuint framebuffer)
  {
    return mIsFramebufferResult;
  }

  GLboolean IsProgram(GLuint program)
  {
    return mIsProgramResult;
  }

  GLboolean IsRenderbuffer(GLuint renderbuffer)
  {
    return mIsRenderbufferResult;
  }

  GLboolean IsShader(GLuint shader)
  {
    return mIsShaderResult;
  }

  GLboolean IsTexture(GLuint texture)
  {
    return mIsTextureResult;
  }

  void LineWidth(GLfloat width)
  {
  }

  void LinkProgram(GLuint program)
  {
    std::stringstream out;
    out << program;
    mShaderTrace.PushCall("LinkProgram", out.str());
  }

  void PixelStorei(GLenum pname, GLint param)
  {
  }

  void PolygonOffset(GLfloat factor, GLfloat units)
  {
  }

  void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)
  {
  }

  void ReleaseShaderCompiler(void)
  {
  }

  void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
  {
  }

  void SampleCoverage(GLclampf value, GLboolean invert)
  {
  }

  void Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  void ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length)
  {
  }

  void ShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
  {
    std::string stringBuilder;
    for(int i = 0; i < count; ++i)
    {
      stringBuilder += string[i];
    }
    mShaderSources[shader] = stringBuilder;
    mLastShaderCompiled = shader;
  }

  void GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
  {
    const std::string shaderSource = mShaderSources[shader];
    if( static_cast<int>(shaderSource.length()) < bufsize )
    {
      strcpy(source, shaderSource.c_str());
      *length = shaderSource.length();
    }
    else
    {
      *length = bufsize -1;
      strncpy(source, shaderSource.c_str(), *length);
      source[*length] = 0x0;
    }
  }

  void StencilFunc(GLenum func, GLint ref, GLuint mask)
  {
  }

  void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
  {
  }

  void StencilMask(GLuint mask)
  {
  }

  void StencilMaskSeparate(GLenum face, GLuint mask)
  {
  }

  void StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
  {
  }

  void StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
  {
  }

  void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
  {
    std::stringstream out;
    out << width << ", " << height;
    mTextureTrace.PushCall("TexImage2D", out.str());
  }

  void TexParameterf(GLenum target, GLenum pname, GLfloat param)
  {
  }

  void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
  {
  }

  void TexParameteri(GLenum target, GLenum pname, GLint param)
  {
  }

  void TexParameteriv(GLenum target, GLenum pname, const GLint* params)
  {
  }

  void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
  {
    std::stringstream out;
    out << xoffset << ", " << yoffset << ", " << width << ", " << height;
    mTextureTrace.PushCall("TexSubImage2D", out.str());
  }

  void Uniform1f(GLint location, GLfloat x)
  {
    if( ! mProgramUniforms1f.SetUniformValue( mCurrentProgram, location, x ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  void Uniform1fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms1f.SetUniformValue( mCurrentProgram, location, v[i] ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  void Uniform1i(GLint location, GLint x)
  {
    if( ! mProgramUniforms1i.SetUniformValue( mCurrentProgram, location, x ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  void Uniform1iv(GLint location, GLsizei count, const GLint* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms1i.SetUniformValue( mCurrentProgram,
                                                 location,
                                                 v[i] ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  void Uniform2f(GLint location, GLfloat x, GLfloat y)
  {
    if( ! mProgramUniforms2f.SetUniformValue( mCurrentProgram,
                                               location,
                                               Vector2( x, y ) ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  void Uniform2fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms2f.SetUniformValue( mCurrentProgram,
                                                 location,
                                                 Vector2( v[2*i], v[2*i+1] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  void Uniform2i(GLint location, GLint x, GLint y)
  {
  }

  void Uniform2iv(GLint location, GLsizei count, const GLint* v)
  {
  }

  void Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
  {
    if( ! mProgramUniforms3f.SetUniformValue( mCurrentProgram,
                                               location,
                                               Vector3( x, y, z ) ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  void Uniform3fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms3f.SetUniformValue(
          mCurrentProgram,
          location,
          Vector3( v[3*i], v[3*i+1], v[3*i+2] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  void Uniform3i(GLint location, GLint x, GLint y, GLint z)
  {
  }

  void Uniform3iv(GLint location, GLsizei count, const GLint* v)
  {
  }

  void Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
  {
    if( ! mProgramUniforms4f.SetUniformValue( mCurrentProgram,
                                              location,
                                              Vector4( x, y, z, w ) ) )
    {
      mGetErrorResult = GL_INVALID_OPERATION;
    }
  }

  void Uniform4fv(GLint location, GLsizei count, const GLfloat* v)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniforms4f.SetUniformValue(
          mCurrentProgram,
          location,
          Vector4( v[4*i], v[4*i+1], v[4*i+2], v[4*i+3] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  void Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
  {
  }

  void Uniform4iv(GLint location, GLsizei count, const GLint* v)
  {
  }

  void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniformsMat3.SetUniformValue(
            mCurrentProgram,
            location,
            Matrix3( value[0], value[1], value[2], value[3], value[4], value[5], value[6], value[7], value[8] ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
    for( int i = 0; i < count; ++i )
    {
      if( ! mProgramUniformsMat4.SetUniformValue(
          mCurrentProgram,
          location,
          Matrix( value ) ) )
      {
        mGetErrorResult = GL_INVALID_OPERATION;
        break;
      }
    }
  }

  void UseProgram(GLuint program)
  {
    mCurrentProgram = program;
  }

  void ValidateProgram(GLuint program)
  {
  }

  void VertexAttrib1f(GLuint indx, GLfloat x)
  {
  }

  void VertexAttrib1fv(GLuint indx, const GLfloat* values)
  {
  }

  void VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
  {
  }

  void VertexAttrib2fv(GLuint indx, const GLfloat* values)
  {
  }

  void VertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
  {
  }

  void VertexAttrib3fv(GLuint indx, const GLfloat* values)
  {
  }

  void VertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
  {
  }

  void VertexAttrib4fv(GLuint indx, const GLfloat* values)
  {
  }

  void VertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
  {
  }

  void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  /* OpenGL ES 3.0 */

  void ReadBuffer(GLenum mode)
  {
  }

  void DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices)
  {
  }

  void TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
  {
  }

  void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels)
  {
  }

  void CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  void CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data)
  {
  }

  void CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data)
  {
  }

  void GenQueries(GLsizei n, GLuint* ids)
  {
  }

  void DeleteQueries(GLsizei n, const GLuint* ids)
  {
  }

  GLboolean IsQuery(GLuint id)
  {
    return false;
  }

  void BeginQuery(GLenum target, GLuint id)
  {
  }

  void EndQuery(GLenum target)
  {
  }

  void GetQueryiv(GLenum target, GLenum pname, GLint* params)
  {
  }

  void GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params)
  {
  }

  GLboolean UnmapBuffer(GLenum target)
  {
    return false;
  }

  void GetBufferPointerv(GLenum target, GLenum pname, GLvoid** params)
  {
  }

  void DrawBuffers(GLsizei n, const GLenum* bufs)
  {
  }

  void UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  void UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  void UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  void UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  void UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  void UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
  {
  }

  void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
  {
  }

  void RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
  {
  }

  void FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
  {
  }

  GLvoid* MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
  {
    return NULL;
  }

  void FlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
  {
  }

  void BindVertexArray(GLuint array)
  {
  }

  void DeleteVertexArrays(GLsizei n, const GLuint* arrays)
  {
  }

  void GenVertexArrays(GLsizei n, GLuint* arrays)
  {
  }

  GLboolean IsVertexArray(GLuint array)
  {
    return false;
  }

  void GetIntegeri_v(GLenum target, GLuint index, GLint* data)
  {
  }

  void BeginTransformFeedback(GLenum primitiveMode)
  {
  }

  void EndTransformFeedback(void)
  {
  }

  void BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
  {
  }

  void BindBufferBase(GLenum target, GLuint index, GLuint buffer)
  {
  }

  void TransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode)
  {
  }

  void GetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name)
  {
  }

  void VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
  {
  }

  void GetVertexAttribIiv(GLuint index, GLenum pname, GLint* params)
  {
  }

  void GetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params)
  {
  }

  void VertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
  {
  }

  void VertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
  {
  }

  void VertexAttribI4iv(GLuint index, const GLint* v)
  {
  }

  void VertexAttribI4uiv(GLuint index, const GLuint* v)
  {
  }

  void GetUniformuiv(GLuint program, GLint location, GLuint* params)
  {
  }

  GLint GetFragDataLocation(GLuint program, const GLchar *name)
  {
    return -1;
  }

  void Uniform1ui(GLint location, GLuint v0)
  {
  }

  void Uniform2ui(GLint location, GLuint v0, GLuint v1)
  {
  }

  void Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
  {
  }

  void Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
  {
  }

  void Uniform1uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  void Uniform2uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  void Uniform3uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  void Uniform4uiv(GLint location, GLsizei count, const GLuint* value)
  {
  }

  void ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value)
  {
  }

  void ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value)
  {
  }

  void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value)
  {
  }

  void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
  {
  }

  const GLubyte* GetStringi(GLenum name, GLuint index)
  {
    return NULL;
  }

  void CopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
  {
  }

  void GetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices)
  {
  }

  void GetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params)
  {
  }

  GLuint GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName)
  {
    return GL_INVALID_INDEX;
  }

  void GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params)
  {
  }

  void GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName)
  {
  }

  void UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
  {
  }

  void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
  {
  }

  void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount)
  {
  }

  GLsync FenceSync(GLenum condition, GLbitfield flags)
  {
    return NULL;
  }

  GLboolean IsSync(GLsync sync)
  {
    return false;
  }

  void DeleteSync(GLsync sync)
  {
  }

  GLenum ClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
  {
    return 0;
  }

  void WaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
  {
  }

  void GetInteger64v(GLenum pname, GLint64* params)
  {
  }

  void GetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values)
  {
  }

  void GetInteger64i_v(GLenum target, GLuint index, GLint64* data)
  {
  }

  void GetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params)
  {
  }

  void GenSamplers(GLsizei count, GLuint* samplers)
  {
  }

  void DeleteSamplers(GLsizei count, const GLuint* samplers)
  {
  }

  GLboolean IsSampler(GLuint sampler)
  {
    return false;
  }

  void BindSampler(GLuint unit, GLuint sampler)
  {
  }

  void SamplerParameteri(GLuint sampler, GLenum pname, GLint param)
  {
  }

  void SamplerParameteriv(GLuint sampler, GLenum pname, const GLint* param)
  {
  }

  void SamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
  {
  }

  void SamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat* param)
  {
  }

  void GetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params)
  {
  }

  void GetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat* params)
  {
  }

  void VertexAttribDivisor(GLuint index, GLuint divisor)
  {
  }

  void BindTransformFeedback(GLenum target, GLuint id)
  {
  }

  void DeleteTransformFeedbacks(GLsizei n, const GLuint* ids)
  {
  }

  void GenTransformFeedbacks(GLsizei n, GLuint* ids)
  {
  }

  GLboolean IsTransformFeedback(GLuint id)
  {
    return false;
  }

  void PauseTransformFeedback(void)
  {
  }

  void ResumeTransformFeedback(void)
  {
  }

  void GetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary)
  {
  }

  void ProgramBinary(GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length)
  {
  }

  void ProgramParameteri(GLuint program, GLenum pname, GLint value)
  {
  }

  void InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments)
  {
  }

  void InvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height)
  {
  }

  void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
  {
  }

  void TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
  {
  }

  void GetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params)
  {
  }

public: // TEST FUNCTIONS
  void SetCompileStatus( GLuint value ) { mCompileStatus = value; }
  void SetLinkStatus( GLuint value ) { mLinkStatus = value; }
  void SetGetAttribLocationResult(  int result) { mGetAttribLocationResult = result; }
  void SetGetErrorResult(  GLenum result) { mGetErrorResult = result; }
  void SetGetStringResult(  GLubyte* result) { mGetStringResult = result; }
  void SetIsBufferResult(  GLboolean result) { mIsBufferResult = result; }
  void SetIsEnabledResult(  GLboolean result) { mIsEnabledResult = result; }
  void SetIsFramebufferResult(  GLboolean result) { mIsFramebufferResult = result; }
  void SetIsProgramResult(  GLboolean result) { mIsProgramResult = result; }
  void SetIsRenderbufferResult(  GLboolean result) { mIsRenderbufferResult = result; }
  void SetIsShaderResult(  GLboolean result) { mIsShaderResult = result; }
  void SetIsTextureResult(  GLboolean result) { mIsTextureResult = result; }
  void SetCheckFramebufferStatusResult(  GLenum result) { mCheckFramebufferStatusResult = result; }
  void SetNumBinaryFormats( GLint numFormats ) { mNumBinaryFormats = numFormats; }
  void SetBinaryFormats( GLint binaryFormats ) { mBinaryFormats = binaryFormats; }
  void SetProgramBinaryLength( GLint length ) { mProgramBinaryLength = length; }

  bool GetVertexAttribArrayState(GLuint index)
  {
    if( index >= MAX_ATTRIBUTE_CACHE_SIZE )
    {
      // out of range
      return false;
    }
    return mVertexAttribArrayState[ index ];
  }
  void ClearVertexAttribArrayChanged() {  mVertexAttribArrayChanged = false; }
  bool GetVertexAttribArrayChanged()  { return mVertexAttribArrayChanged; }

  //Methods for CullFace verification
  void EnableCullFaceCallTrace(bool enable) { mCullFaceTrace.Enable(enable); }
  void ResetCullFaceCallStack() { mCullFaceTrace.Reset(); }
  TraceCallStack& GetCullFaceTrace() { return mCullFaceTrace; }

  //Methods for Shader verification
  void EnableShaderCallTrace(bool enable) { mShaderTrace.Enable(enable); }
  void ResetShaderCallStack() { mShaderTrace.Reset(); }
  TraceCallStack& GetShaderTrace() { return mShaderTrace; }

  //Methods for Texture verification
  void EnableTextureCallTrace(bool enable) { mTextureTrace.Enable(enable); }
  void ResetTextureCallStack() { mTextureTrace.Reset(); }
  TraceCallStack& GetTextureTrace() { return mTextureTrace; }

  //Methods for Draw verification
  void EnableDrawCallTrace(bool enable) { mDrawTrace.Enable(enable); }
  void ResetDrawCallStack() { mDrawTrace.Reset(); }
  TraceCallStack& GetDrawTrace() { return mDrawTrace; }

  template <typename T>
  bool CheckUniformValue( const char* name, const T& value ) const
  {
    for( ProgramUniformMap::const_iterator program_it = mUniforms.begin();
          program_it != mUniforms.end();
          ++program_it )
    {
      const UniformIDMap &uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find( name );
      if( uniform_it != uniformIDs.end() )
      {
        // found one matching uniform name, lets check the value...
        GLuint programId = program_it->first;
        GLint uniformId = uniform_it->second;

        const ProgramUniformValue<T> &mProgramUniforms = GetProgramUniformsForType( value );
        if( mProgramUniforms.CheckUniformValue( programId, uniformId, value ) )
        {
          // the value matches
          return true;
        }
      }
    }

    tet_infoline( "Not found, printing possible values:" );
    for( ProgramUniformMap::const_iterator program_it = mUniforms.begin();
          program_it != mUniforms.end();
          ++program_it )
    {
      const UniformIDMap &uniformIDs = program_it->second;

      UniformIDMap::const_iterator uniform_it = uniformIDs.find( name );
      if( uniform_it != uniformIDs.end() )
      {
        // found one matching uniform name, lets check the value...
        GLuint programId = program_it->first;
        GLint uniformId = uniform_it->second;

        const ProgramUniformValue<T> &mProgramUniforms = GetProgramUniformsForType( value );
        T origValue;
        if ( mProgramUniforms.GetUniformValue(programId, uniformId, origValue) )
        {
          std::stringstream out;
          out << uniform_it->first << ": " << origValue;
          tet_printf( "%s", out.str().c_str() );
        }
      }
    }
    return false;
  }

  template <typename T>
  bool GetUniformValue( GLuint programId, GLuint uniformId, T& outValue) const
  {
    const ProgramUniformValue<T> &mProgramUniforms = GetProgramUniformsForType( outValue );
    return mProgramUniforms.GetUniformValue( programId, uniformId, outValue );
  }

  GLuint GetLastShaderCompiled() const
  {
    return mLastShaderCompiled;
  }

  GLuint GetLastProgramCreated() const
  {
    return mLastProgramIdUsed;
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


private:
  GLuint     mCurrentProgram;
  GLuint     mCompileStatus;
  GLuint     mLinkStatus;
  GLint      mGetAttribLocationResult;
  GLenum     mGetErrorResult;
  GLubyte*   mGetStringResult;
  GLboolean  mIsBufferResult;
  GLboolean  mIsEnabledResult;
  GLboolean  mIsFramebufferResult;
  GLboolean  mIsProgramResult;
  GLboolean  mIsRenderbufferResult;
  GLboolean  mIsShaderResult;
  GLboolean  mIsTextureResult;
  GLenum     mActiveTextureUnit;
  GLenum     mCheckFramebufferStatusResult;
  GLint      mNumBinaryFormats;
  GLint      mBinaryFormats;
  GLint      mProgramBinaryLength;
  bool       mVertexAttribArrayState[MAX_ATTRIBUTE_CACHE_SIZE];
  bool       mVertexAttribArrayChanged;                            // whether the vertex attrib array has been changed
  typedef std::map< GLuint, std::string> ShaderSourceMap;
  ShaderSourceMap mShaderSources;
  GLuint     mLastShaderCompiled;

  Vector4 mLastBlendColor;
  GLenum  mLastBlendEquationRgb;
  GLenum  mLastBlendEquationAlpha;
  GLenum  mLastBlendFuncSrcRgb;
  GLenum  mLastBlendFuncDstRgb;
  GLenum  mLastBlendFuncSrcAlpha;
  GLenum  mLastBlendFuncDstAlpha;

  // Data for manipulating the IDs returned by GenTextures
  GLuint mLastAutoTextureIdUsed;
  std::vector<GLuint> mNextTextureIds;
  std::vector<GLuint> mDeletedTextureIds;
  std::vector<GLuint> mBoundTextures;

  struct ActiveTextureType
  {
    std::vector<GLuint> mBoundTextures;
  };

  ActiveTextureType mActiveTextures[ MIN_TEXTURE_UNIT_LIMIT ];

  TraceCallStack mCullFaceTrace;
  TraceCallStack mShaderTrace;
  TraceCallStack mTextureTrace;
  TraceCallStack mDrawTrace;

  // Shaders & Uniforms
  GLuint mLastShaderIdUsed;
  GLuint mLastProgramIdUsed;
  GLuint mLastUniformIdUsed;
  typedef std::map< std::string, GLint > UniformIDMap;
  typedef std::map< GLuint, UniformIDMap > ProgramUniformMap;
  ProgramUniformMap mUniforms;

  template <typename T>
  struct ProgramUniformValue : public std::map< GLuint, std::map< GLint, T > >
  {
  public:
    typedef std::map< GLint, T > UniformValueMap;
    typedef std::map< GLuint, UniformValueMap > Map;

    bool SetUniformValue( GLuint program, GLuint uniform, const T& value )
    {
      if( program == 0 )
      {
        return false;
      }

      typename Map::iterator it = Map::find( program );
      if( it == Map::end() )
      {
        // if its the first uniform for this program add it
        std::pair< typename Map::iterator, bool > result =
            Map::insert( typename Map::value_type( program, UniformValueMap() ) );
        it = result.first;
      }

      UniformValueMap& uniforms = it->second;
      uniforms[uniform] = value;

      return true;
    }

    bool CheckUniformValue( GLuint program, GLuint uniform, const T& value ) const
    {
      T uniformValue;
      if ( GetUniformValue( program, uniform, uniformValue ) )
      {
        return value == uniformValue;
      }

      return false;
    }

    bool GetUniformValue( GLuint program, GLuint uniform, T& value ) const
    {
      if( program == 0 )
      {
        return false;
      }

      typename Map::const_iterator it = Map::find( program );
      if( it == Map::end() )
      {
        // Uniform values always initialised as 0
        value = GetZero();
        return true;
      }

      const UniformValueMap& uniforms = it->second;
      typename UniformValueMap::const_iterator it2 = uniforms.find( uniform );
      if( it2 == uniforms.end() )
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
  ProgramUniformValue<int> mProgramUniforms1i;
  ProgramUniformValue<float> mProgramUniforms1f;
  ProgramUniformValue<Vector2> mProgramUniforms2f;
  ProgramUniformValue<Vector3> mProgramUniforms3f;
  ProgramUniformValue<Vector4> mProgramUniforms4f;
  ProgramUniformValue<Matrix> mProgramUniformsMat4;
  ProgramUniformValue<Matrix3> mProgramUniformsMat3;

  const ProgramUniformValue<int>& GetProgramUniformsForType( const int ) const
  {
    return mProgramUniforms1i;
  }
  const ProgramUniformValue<float>& GetProgramUniformsForType( const float ) const
  {
    return mProgramUniforms1f;
  }
  const ProgramUniformValue<Vector2>& GetProgramUniformsForType( const Vector2& ) const
  {
    return mProgramUniforms2f;
  }
  const ProgramUniformValue<Vector3>& GetProgramUniformsForType( const Vector3& ) const
  {
    return mProgramUniforms3f;
  }
  const ProgramUniformValue<Vector4>& GetProgramUniformsForType( const Vector4& ) const
  {
    return mProgramUniforms4f;
  }
  const ProgramUniformValue<Matrix>& GetProgramUniformsForType( const Matrix& ) const
  {
    return mProgramUniformsMat4;
  }
  const ProgramUniformValue<Matrix3>& GetProgramUniformsForType( const Matrix3& ) const
  {
    return mProgramUniformsMat3;
  }
  void SetVertexAttribArray(GLuint index, bool state)
  {
    if( index >= MAX_ATTRIBUTE_CACHE_SIZE )
    {
      // out of range
      return;
    }
    mVertexAttribArrayState[ index ] = state;
    mVertexAttribArrayChanged = true;
  }

};

template <>
inline int TestGlAbstraction::ProgramUniformValue<int>::GetZero() const
{
  return 0;
}

template <>
inline float TestGlAbstraction::ProgramUniformValue<float>::GetZero() const
{
  return 0.0f;
}

template <>
inline Vector2 TestGlAbstraction::ProgramUniformValue<Vector2>::GetZero() const
{
  return Vector2::ZERO;
}

template <>
inline Vector3 TestGlAbstraction::ProgramUniformValue<Vector3>::GetZero() const
{
  return Vector3::ZERO;
}

template <>
inline Vector4 TestGlAbstraction::ProgramUniformValue<Vector4>::GetZero() const
{
  return Vector4::ZERO;
}

template <>
inline Matrix TestGlAbstraction::ProgramUniformValue<Matrix>::GetZero() const
{
  return Matrix();
}

template <>
inline Matrix3 TestGlAbstraction::ProgramUniformValue<Matrix3>::GetZero() const
{
  return Matrix3( Matrix() );
}

} // namespace Dali

extern bool BlendEnabled(const TraceCallStack& callStack);
extern bool BlendDisabled(const TraceCallStack& callStack);


#endif // header
