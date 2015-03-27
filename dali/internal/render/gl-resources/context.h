#ifndef __DALI_INTERNAL_CONTEXT_H__
#define __DALI_INTERNAL_CONTEXT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector4.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/internal/render/gl-resources/frame-buffer-state-cache.h>
#include <dali/internal/render/gl-resources/gl-call-debug.h>

namespace Dali
{

namespace Internal
{

/**
 * Context records the current GL state, and provides access to the OpenGL ES 2.0 API.
 * Context avoids duplicate GL calls, if the same setting etc. is requested repeatedly.
 */
class Context
{
public:

  /**
   * FrameBuffer Clear mode
   */
  enum ClearMode
  {
    FORCE_CLEAR,        ///< always perform the glClear regardless of current state
    CHECK_CACHED_VALUES ///< check the Frame buffers cached state to see if a clear is required
  };

  /**
   * Size of the VertexAttributeArray enables
   * GLES specification states that there's minimum of 8
   */
  static const unsigned int MAX_ATTRIBUTE_CACHE_SIZE = 8;

  static const unsigned int MAX_TEXTURE_UNITS = 8; // for GLES 2.0 8 is guaranteed, which is more than DALi uses anyways

  /**
   * Creates the Dali Context object.
   * This method does not create an OpenGL context i.e. that is done from outside dali-core.
   * @pre Context has not been created.
   * @exception Context already created.
   * @param glAbstraction the gl abstraction.
   */
  Context( Integration::GlAbstraction& glAbstraction );

  /**
   * Destructor
   */
  ~Context();

  /**
   * Called when the GL context has been created.
   */
  void GlContextCreated();

  /**
   * Called when the GL context has been destroyed.
   */
  void GlContextDestroyed();

  /**
   * Query whether the OpenGL context has been created.
   * @return True if the OpenGL context has been created.
   */
  bool IsGlContextCreated() { return mGlContextCreated; }

  /**
   * @return the GLAbstraction
   */
  Integration::GlAbstraction& GetAbstraction() { return mGlAbstraction; }

#ifdef DEBUG_ENABLED

  /**
   * Debug helper which prints the currently cached GL state.
   */
  void PrintCurrentState();

#endif

  /**
   * Helper to convert GL error code to string
   * @param errorCode to convert
   * @return C string
   */
  const char* ErrorToString( GLenum errorCode );

  /**
   * Helper to print GL string to debug log
   */
  void PrintGlString(const char* stringName, GLenum stringId)
  {
    DALI_LOG_INFO(Debug::Filter::gRender, Debug::General, "GL %s = %s\n", stringName, (const char *)GetString( stringId ) );
  }

  /****************************************************************************************
   * The following methods are forwarded to Dali::Integration::GlAbstraction.
   * In some cases the GL state is recorded, to avoid duplicate calls with the same state.
   * All Shader, Program, Uniform and Attribute related calls are not here, Program class
   * handles them and optimizes any program related state changes
   ****************************************************************************************/

  /**
   * Wrapper for OpenGL ES 2.0 glActiveTexture()
   */
  void ActiveTexture( TextureUnit textureUnit )
  {
    if ( textureUnit != mActiveTextureUnit )
    {
      mActiveTextureUnit = textureUnit;
      LOG_GL("ActiveTexture %x\n", textureUnit);
      CHECK_GL( mGlAbstraction, mGlAbstraction.ActiveTexture(TextureUnitAsGLenum(textureUnit)) );
    }
  }

  /**
   * Wrapper for OpenGL ES 3.0 glBeginQuery()
   */
  void BeginQuery(GLenum target, GLuint id)
  {
    LOG_GL("BeginQuery %d %d\n", target, id);
    CHECK_GL( mGlAbstraction, mGlAbstraction.BeginQuery(target, id) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glBeginTransformFeedback()
   */
  void BeginTransformFeedback(GLenum primitiveMode)
  {
    LOG_GL("BeginTransformFeedback %x\n", primitiveMode);
    CHECK_GL( mGlAbstraction, mGlAbstraction.BeginTransformFeedback(primitiveMode) );
  }

  /**
   * The wrapper for OpenGL ES 2.0 glBindBuffer() has been replaced by BindArrayBuffer & BindElementArrayBuffer & BindTransformFeedbackBuffer.
   */

  /**
   * Wrapper for OpenGL ES 2.0 glBindBuffer(GL_ARRAY_BUFFER, ...)
   */
  void BindArrayBuffer(GLuint buffer)
  {
    // Avoid unecessary calls to BindBuffer
    if (mBoundArrayBufferId != buffer)
    {
      mBoundArrayBufferId = buffer;

      LOG_GL("BindBuffer GL_ARRAY_BUFFER %d\n", buffer);
      CHECK_GL( mGlAbstraction, mGlAbstraction.BindBuffer(GL_ARRAY_BUFFER, buffer) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ...)
   */
  void BindElementArrayBuffer(GLuint buffer)
  {
    // Avoid unecessary calls to BindBuffer
    if (mBoundElementArrayBufferId!= buffer)
    {
      mBoundElementArrayBufferId = buffer;

      LOG_GL("BindBuffer GL_ELEMENT_ARRAY_BUFFER %d\n", buffer);
      CHECK_GL( mGlAbstraction, mGlAbstraction.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer) );
    }
  }

  /**
   * Wrapper for OpenGL ES 3.0 glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, ...)
   */
  void BindTransformFeedbackBuffer(GLuint buffer)
  {
    // Avoid unecessary calls to BindBuffer
    if (mBoundTransformFeedbackBufferId != buffer)
    {
      mBoundTransformFeedbackBufferId = buffer;

      LOG_GL("BindBuffer GL_TRANSFORM_FEEDBACK_BUFFER %d\n", buffer);
      CHECK_GL( mGlAbstraction, mGlAbstraction.BindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER , buffer) );
    }
  }

  /**
   * Wrapper for OpenGL ES 3.0 glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, ...)
   */
  void BindTransformFeedbackBufferBase(GLuint index, GLuint buffer)
  {
    // Avoid unecessary calls to BindBufferBase
    if (mBoundTransformFeedbackBufferId != buffer)
    {
      mBoundTransformFeedbackBufferId = buffer;

      LOG_GL("BindBufferBase GL_TRANSFORM_FEEDBACK_BUFFER %d %d\n", index, buffer);
      CHECK_GL( mGlAbstraction, mGlAbstraction.BindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBindFramebuffer()
   */
  void BindFramebuffer(GLenum target, GLuint framebuffer)
  {
    mFrameBufferStateCache.SetCurrentFrameBuffer( framebuffer );

    LOG_GL("BindFramebuffer %d %d\n", target, framebuffer);
    CHECK_GL( mGlAbstraction, mGlAbstraction.BindFramebuffer(target, framebuffer) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBindRenderbuffer()
   */
  void BindRenderbuffer(GLenum target, GLuint renderbuffer)
  {
    LOG_GL("BindRenderbuffer %d %d\n", target, renderbuffer);
    CHECK_GL( mGlAbstraction, mGlAbstraction.BindRenderbuffer(target, renderbuffer) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glBindTransformFeedback()
   */
  void BindTransformFeedback(GLenum target, GLuint id)
  {
    LOG_GL("BindTransformFeedback %d %d\n", target, id);
    CHECK_GL( mGlAbstraction, mGlAbstraction.BindTransformFeedback(target, id) );
  }

  /**
   * Helper to bind texture for rendering. If given texture is
   * already bound in the given textureunit, this method does nothing.
   * Otherwise changes the active texture unit and binds the texture.
   * Note! after this call active texture unit may not necessarily be the one
   * passed in as argument so you cannot change texture unit state!!
   * @param textureunit to bind to
   * @param texture to bind
   */
  void BindTextureForUnit( TextureUnit textureunit, GLuint texture )
  {
    if( mBound2dTextureId[ textureunit ] != texture )
    {
      ActiveTexture( textureunit );
      Bind2dTexture( texture );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBindTexture(GL_TEXTURE_2D)
   */
  void Bind2dTexture( GLuint texture )
  {
    if (mBound2dTextureId[ mActiveTextureUnit ] != texture)
    {
      mBound2dTextureId[ mActiveTextureUnit ] = texture;

      LOG_GL("BindTexture GL_TEXTURE_2D %d\n", texture);
      CHECK_GL( mGlAbstraction, mGlAbstraction.BindTexture(GL_TEXTURE_2D, texture) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBlendColor()
   */
  void SetDefaultBlendColor()
  {
    if( !mUsingDefaultBlendColor )
    {
      LOG_GL( "BlendColor %f %f %f %f\n", 0.0f, 0.0f, 0.0f, 0.0f );
      CHECK_GL( mGlAbstraction, mGlAbstraction.BlendColor( 0.0f, 0.0f, 0.0f, 0.0f ) );
      mUsingDefaultBlendColor = true;
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBlendColor()
   */
  void SetCustomBlendColor( const Vector4& color )
  {
    LOG_GL( "BlendColor %f %f %f %f\n", color.r, color.g, color.b, color.a );
    CHECK_GL( mGlAbstraction, mGlAbstraction.BlendColor(color.r, color.g, color.b, color.a) );
    mUsingDefaultBlendColor = false;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBlendEquation()
   */
  void BlendEquation(GLenum mode)
  {
    // use BlendEquationSeparate to set the rgb and alpha modes the same
    BlendEquationSeparate( mode, mode );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBlendEquationSeparate()
   */
  void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
  {
    if( ( modeRGB != mBlendEquationSeparateModeRGB ) ||
        ( modeAlpha != mBlendEquationSeparateModeAlpha ) )
    {
      mBlendEquationSeparateModeRGB = modeRGB;
      mBlendEquationSeparateModeAlpha = modeAlpha;
      LOG_GL("BlendEquationSeparate %d %d\n", modeRGB, modeAlpha);
      CHECK_GL( mGlAbstraction, mGlAbstraction.BlendEquationSeparate(modeRGB, modeAlpha) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBlendFunc()
   */
  void BlendFunc(GLenum sfactor, GLenum dfactor)
  {
    // reuse the BlendFuncSeparate as thats what the DDK does anyways
    BlendFuncSeparate( sfactor, dfactor, sfactor, dfactor );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBlendFuncSeparate()
   */
  void BlendFuncSeparate( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )
  {
    if( ( mBlendFuncSeparateSrcRGB != srcRGB )||( mBlendFuncSeparateDstRGB != dstRGB )||
        ( mBlendFuncSeparateSrcAlpha != srcAlpha )||( mBlendFuncSeparateDstAlpha != dstAlpha ) )
    {
      mBlendFuncSeparateSrcRGB = srcRGB;
      mBlendFuncSeparateDstRGB = dstRGB;
      mBlendFuncSeparateSrcAlpha = srcAlpha;
      mBlendFuncSeparateDstAlpha = dstAlpha;

      LOG_GL( "BlendFuncSeparate %d %d %d %d\n", srcRGB, dstRGB, srcAlpha, dstAlpha );
      CHECK_GL( mGlAbstraction, mGlAbstraction.BlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha ) );
    }
  }

  /**
   * Wrapper for OpenGL ES 3.0 glBlitFramebuffer()
   */
  void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
  {
    LOG_GL( "BlitFramebuffer %d %d %d %d %d %d %d %d %x %d\n", srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter );
    CHECK_GL( mGlAbstraction, mGlAbstraction.BlitFramebuffer( srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter ) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBufferData()
   */
  void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
  {
    LOG_GL("BufferData %d %d %p %d\n", target, size, data, usage);
    CHECK_GL( mGlAbstraction, mGlAbstraction.BufferData(target, size, data, usage) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glBufferSubData()
   */
  void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
  {
    LOG_GL("BufferSubData %d %d %d %p\n", target, offset, size, data);
    CHECK_GL( mGlAbstraction, mGlAbstraction.BufferSubData(target, offset, size, data) );
  }

  /**
   * Wrapper for OpenGL ES 2.0  glCheckFramebufferStatus()
   */
  GLenum CheckFramebufferStatus(GLenum target)
  {
    LOG_GL("CheckFramebufferStatus %d\n", target);
    GLenum value = CHECK_GL( mGlAbstraction, mGlAbstraction.CheckFramebufferStatus(target) );
    return value;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glClear()
   */
  void Clear(GLbitfield mask, ClearMode mode )
  {
    bool forceClear = (mode == FORCE_CLEAR );
    mask = mFrameBufferStateCache.GetClearMask( mask, forceClear , mScissorTestEnabled );

    if( mask > 0 )
    {
      LOG_GL("Clear %d\n", mask);
      CHECK_GL( mGlAbstraction, mGlAbstraction.Clear( mask ) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glClearColor()
   */
  void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
  {
    Vector4 newCol(red,green,blue,alpha);

    if (!mClearColorSet || mClearColor !=newCol )
    {
      LOG_GL("ClearColor %f %f %f %f\n", red, green, blue, alpha);
      CHECK_GL( mGlAbstraction, mGlAbstraction.ClearColor(red, green, blue, alpha) );

      mClearColorSet = true;
      mClearColor = newCol;
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glClearDepthf()
   */
  void ClearDepthf(GLclampf depth)
  {
    LOG_GL("ClearDepthf %f\n", depth);
    CHECK_GL( mGlAbstraction, mGlAbstraction.ClearDepthf(depth) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glClearStencil()
   */
  void ClearStencil(GLint s)
  {
    LOG_GL("ClearStencil %d\n", s);
    CHECK_GL( mGlAbstraction, mGlAbstraction.ClearStencil(s) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glColorMask()
   * @note This has been optimized to a single boolean value (masking individual channels is not required)
   */
  void ColorMask( bool flag )
  {
    // only change state if needed
    if( flag != mColorMask )
    {
      mColorMask = flag;
      LOG_GL("ColorMask %s %s %s %s\n", flag ? "True" : "False", flag ? "True" : "False", flag ? "True" : "False", flag ? "True" : "False");
      CHECK_GL( mGlAbstraction, mGlAbstraction.ColorMask(flag, flag, flag, flag) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glCompressedTexImage2D()
   */
  void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                            GLint border, GLsizei imageSize, const void* data)
  {
    LOG_GL("CompressedTexImage2D %d %d %x %d %d %d %d %p\n", target, level, internalformat, width, height, border, imageSize, data);
    CHECK_GL( mGlAbstraction, mGlAbstraction.CompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glCompressedTexImage3D()
   */
  void CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
                            GLint border, GLsizei imageSize, const void* data)
  {
    LOG_GL("CompressedTexImage3D %d %d %x %d %d %d %d %d %p\n", target, level, internalformat, width, height, depth, border, imageSize, data);
    CHECK_GL( mGlAbstraction, mGlAbstraction.CompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glCompressedTexSubImage2D()
   */
  void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                               GLenum format, GLsizei imageSize, const void* data)
  {
    LOG_GL("CompressedTexSubImage2D %x %d %d %d %d %d %x %d %p\n", target, level, xoffset, yoffset, width, height, format, imageSize, data);
    CHECK_GL( mGlAbstraction, mGlAbstraction.CompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glCompressedTexSubImage3D()
   */
  void CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                               GLsizei width, GLsizei height, GLsizei depth,
                               GLenum format, GLsizei imageSize, const void* data)
  {
    LOG_GL("CompressedTexSubImage3D %x %d %d %d %d %d %d %d %x %d %p\n", target, level, xoffset, yoffset, xoffset, width, height, depth, format, imageSize, data);
    CHECK_GL( mGlAbstraction, mGlAbstraction.CompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glCopyTexImage2D()
   */
  void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
  {
    LOG_GL("CopyTexImage2D %x %d %x %d %d %d %d %d\n", target, level, internalformat, x, y, width, height, border);
    CHECK_GL( mGlAbstraction, mGlAbstraction.CopyTexImage2D(target, level, internalformat, x, y, width, height, border) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glCopyTexSubImage2D()
   */
  void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
  {
    LOG_GL("CopyTexSubImage2D %x %d %d %d %d %d %d %d\n", target, level, xoffset, yoffset, x, y, width, height);
    CHECK_GL( mGlAbstraction, mGlAbstraction.CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glCopyTexSubImage3D()
   */
  void CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
  {
    LOG_GL("CopyTexSubImage3D %x %d %d %d %d %d %d %d %d\n", target, level, xoffset, yoffset, zoffset, x, y, width, height);
    CHECK_GL( mGlAbstraction, mGlAbstraction.CopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glCullFace()
   * enables GL_CULL_FACE if in any of the face culling modes
   * otherwise disables GL_CULL_FACE
   */
  void CullFace(CullFaceMode mode)
  {
    // Avoid unnecessary calls to gl
    if(mCullFaceMode != mode)
    {
      mCullFaceMode = mode;
      switch(mode)
      {
        case CullNone:
        {
          LOG_GL("Disable GL_CULL_FACE\n");
          CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_CULL_FACE) );
          break;
        }

        case CullFront:
        {
          LOG_GL("Enable GL_CULL_FACE\n");
          CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_CULL_FACE) );
          LOG_GL("Enable GL_FRONT\n");
          CHECK_GL( mGlAbstraction, mGlAbstraction.CullFace(GL_FRONT) );
          break;
        }

        case CullBack:
        {
          LOG_GL("Enable GL_CULL_FACE\n");
          CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_CULL_FACE) );
          LOG_GL("Enable GL_BACK\n");
          CHECK_GL( mGlAbstraction, mGlAbstraction.CullFace(GL_BACK) );
          break;
        }

        case CullFrontAndBack:
        {
          LOG_GL("Enable GL_CULL_FACE\n");
          CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_CULL_FACE) );
          LOG_GL("Enable GL_FRONT_AND_BACK\n");
          CHECK_GL( mGlAbstraction, mGlAbstraction.CullFace(GL_FRONT_AND_BACK) );
          break;
        }

        default:
          break;
      }
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDeleteBuffers()
   */
  void DeleteBuffers(GLsizei n, const GLuint* buffers)
  {
    // @todo: this is to prevent mesh destructor from doing GL calls when DALi core is being deleted
    // can be taken out once render manages either knows about meshes or gpubuffers and can tell them directly that context is lost
    if( this->IsGlContextCreated() )
    {
      LOG_GL("DeleteBuffers %d %p\n", n, buffers);
      CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteBuffers(n, buffers) );
    }
    // reset the cached buffer id's
    // fixes problem where some drivers will a generate a buffer with the
    // same id, as the last deleted buffer id.
    mBoundArrayBufferId = 0;
    mBoundElementArrayBufferId = 0;
    mBoundTransformFeedbackBufferId = 0;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDeleteFramebuffers()
   */
  void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
  {
    mFrameBufferStateCache.FrameBuffersDeleted( n, framebuffers );

    LOG_GL("DeleteFramebuffers %d %p\n", n, framebuffers);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteFramebuffers(n, framebuffers) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glDeleteQueries()
   */
  void DeleteQueries(GLsizei n, GLuint* ids)
  {
    LOG_GL("DeleteQueries %d %p\n", n, ids);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteQueries(n, ids) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDeleteRenderbuffers()
   */
  void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
  {
    LOG_GL("DeleteRenderbuffers %d %p\n", n, renderbuffers);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteRenderbuffers(n, renderbuffers) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDeleteTextures()
   */
  void DeleteTextures(GLsizei n, const GLuint* textures)
  {
    LOG_GL("DeleteTextures %d %p\n", n, textures);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteTextures(n, textures) );

    // reset the cached texture id's incase the driver re-uses them
    // when creating new textures
    for( unsigned int i=0; i < MAX_TEXTURE_UNITS; ++i )
    {
       mBound2dTextureId[ i ] = 0;
    }
  }

  /**
   * Wrapper for OpenGL ES 3.0 glDeleteTransformFeedbacks()
   */
  void DeleteTransformFeedbacks(GLsizei n, GLuint* ids)
  {
    LOG_GL("DeleteTransformFeedbacks %d %p\n", n, ids);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DeleteTransformFeedbacks(n, ids) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDepthFunc()
   */
  void DepthFunc(GLenum func)
  {
    LOG_GL("DepthFunc %x\n", func);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DepthFunc(func) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDepthMask()
   */
  void DepthMask(GLboolean flag)
  {
    // only change state if needed
    if( flag != mDepthMaskEnabled )
    {
      mDepthMaskEnabled = flag;
      LOG_GL("DepthMask %s\n", flag ? "True" : "False");
      CHECK_GL( mGlAbstraction, mGlAbstraction.DepthMask( mDepthMaskEnabled ) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDepthRangef()
   */
  void DepthRangef(GLclampf zNear, GLclampf zFar)
  {
    LOG_GL("DepthRangef %f %f\n", zNear, zFar);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DepthRangef(zNear, zFar) );
  }

  /**
   * The wrapper for OpenGL ES 2.0 glDisable() has been replaced by SetBlend, SetCullFace, SetDepthTest,
   * SetDither, SetPolygonOffsetFill, SetSampleAlphaToCoverage, SetSampleCoverage, SetScissorTest & SetStencilTest.
   */

  /**
   * Wrapper for OpenGL ES 2.0 glDrawArrays()
   */
  void DrawArrays(GLenum mode, GLint first, GLsizei count)
  {
    mFrameBufferStateCache.DrawOperation( mColorMask, DepthBufferWriteEnabled(), StencilBufferWriteEnabled() );
    FlushVertexAttributeLocations();

    LOG_GL("DrawArrays %x %d %d\n", mode, first, count);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DrawArrays(mode, first, count) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glDrawArraysInstanced()
   */
  void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
  {
    mFrameBufferStateCache.DrawOperation( mColorMask, DepthBufferWriteEnabled(), StencilBufferWriteEnabled() );
    FlushVertexAttributeLocations();

    LOG_GL("DrawArraysInstanced %x %d %d %d\n", mode, first, count, instanceCount);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DrawArraysInstanced(mode, first, count,instanceCount) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glDrawBuffers()
   */
  void DrawBuffers(GLsizei n, const GLenum* bufs)
  {
    mFrameBufferStateCache.DrawOperation( mColorMask, DepthBufferWriteEnabled(), StencilBufferWriteEnabled() );
    LOG_GL("DrawBuffers %d %p\n", n, bufs);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DrawBuffers(n, bufs) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glDrawElements()
   */
  void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
  {
    mFrameBufferStateCache.DrawOperation( mColorMask, DepthBufferWriteEnabled(), StencilBufferWriteEnabled() );

    FlushVertexAttributeLocations();

    LOG_GL("DrawElements %x %d %d %p\n", mode, count, type, indices);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DrawElements(mode, count, type, indices) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glDrawElementsInstanced()
   */
  void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instanceCount)
  {
    mFrameBufferStateCache.DrawOperation( mColorMask, DepthBufferWriteEnabled(), StencilBufferWriteEnabled() );

    FlushVertexAttributeLocations();

    LOG_GL("DrawElementsInstanced %x %d %d %p %d\n", mode, count, type, indices, instanceCount);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DrawElementsInstanced(mode, count, type, indices, instanceCount) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glDrawRangeElements()
   */
  void DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices)
  {
    mFrameBufferStateCache.DrawOperation( mColorMask, DepthBufferWriteEnabled(), StencilBufferWriteEnabled() );
    FlushVertexAttributeLocations();

    LOG_GL("DrawRangeElements %x %u %u %d %d %p\n", mode, start, end, count, type, indices);
    CHECK_GL( mGlAbstraction, mGlAbstraction.DrawRangeElements(mode, start, end, count, type, indices) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glGenQuerieS()
   */
  void GenQueries(GLsizei n, GLuint* ids)
  {
    LOG_GL("GenQueries %d %p\n", n, ids);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GenQueries(n, ids) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glGenTransformFeedbacks()
   */
  void GenTransformFeedbacks(GLsizei n, GLuint* ids)
  {
    LOG_GL("GenTransformFeedbacks %d %p\n", n, ids);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GenTransformFeedbacks(n, ids) );
  }

  /**
   * @return the current buffer bound for a given target
   */
  GLuint GetCurrentBoundArrayBuffer(GLenum target)
  {
    GLuint result(0);
    switch(target)
    {
      case GL_ARRAY_BUFFER:
      {
        result = mBoundArrayBufferId;
        break;
      }
      case GL_ELEMENT_ARRAY_BUFFER:
      {
        result = mBoundElementArrayBufferId;
        break;
      }
      case GL_TRANSFORM_FEEDBACK_BUFFER:
      {
        result = mBoundTransformFeedbackBufferId;
        break;
      }
      default:
      {
        DALI_ASSERT_DEBUG(0 && "target buffer type not supported");
      }
    }
    return result;
  }

  void EnableVertexAttributeArray( GLuint location )
  {
    SetVertexAttributeLocation( location, true);
  }

  void DisableVertexAttributeArray( GLuint location )
  {
    SetVertexAttributeLocation( location, false);
  }

  /**
   * Wrapper for OpenGL ES 3.0 glVertexAttribDivisor()
   */
  void VertexAttribDivisor ( GLuint index, GLuint divisor )
  {
    LOG_GL("VertexAttribDivisor(%d, %d)\n", index, divisor );
    CHECK_GL( mGlAbstraction, mGlAbstraction.VertexAttribDivisor( index, divisor ) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glVertexAttribPointer()
   */
  void VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr )
  {
    LOG_GL("VertexAttribPointer(%d, %d, %d, %d, %d, %x)\n", index, size, type, normalized, stride, ptr );
    CHECK_GL( mGlAbstraction, mGlAbstraction.VertexAttribPointer( index, size, type, normalized, stride, ptr ) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glInvalidateFramebuffer()
   */
  void InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
  {
    LOG_GL("InvalidateFramebuffer\n");
    CHECK_GL( mGlAbstraction, mGlAbstraction.InvalidateFramebuffer(target, numAttachments, attachments) );
  }

  /**
   * The wrapper for OpenGL ES 2.0 glEnable() has been replaced by SetBlend, SetCullFace, SetDepthTest,
   * SetDither, SetPolygonOffsetFill, SetSampleAlphaToCoverage, SetSampleCoverage, SetScissorTest & SetStencilTest.
   */

  /**
   * This method replaces glEnable(GL_BLEND) and glDisable(GL_BLEND).
   * @param[in] enable True if GL_BLEND should be enabled.
   */
  void SetBlend(bool enable)
  {
    // Avoid unecessary calls to glEnable/glDisable
    if (enable != mBlendEnabled)
    {
      mBlendEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_BLEND\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_BLEND) );
      }
      else
      {
        LOG_GL("Disable GL_BLEND\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_BLEND) );
      }
    }
  }

  /**
   * This method replaces glEnable(GL_DEPTH_TEST) and glDisable(GL_DEPTH_TEST).
   * Note GL_DEPTH_TEST means enable the depth buffer for writing and or testing.
   * glDepthMask is used to enable / disable writing to depth buffer.
   * glDepthFunc us used to control if testing is enabled and how it is performed ( default GL_LESS)
   *
   * @param[in] enable True if GL_DEPTH_TEST should be enabled.
   */
  void EnableDepthBuffer( bool enable )
  {
    // Avoid unecessary calls to glEnable/glDisable
    if( enable != mDepthBufferEnabled )
    {
      mDepthBufferEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_DEPTH_TEST\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_DEPTH_TEST) );
      }
      else
      {
        LOG_GL("Disable GL_DEPTH_TEST\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_DEPTH_TEST) );
      }
    }
  }

  /**
   * This method replaces glEnable(GL_DITHER) and glDisable(GL_DITHER).
   * @param[in] enable True if GL_DITHER should be enabled.
   */
  void SetDither(bool enable)
  {
    // Avoid unecessary calls to glEnable/glDisable
    if (enable != mDitherEnabled)
    {
      mDitherEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_DITHER\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_DITHER) );
      }
      else
      {
        LOG_GL("Disable GL_DITHER\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_DITHER) );
      }
    }
  }

  /**
   * This method replaces glEnable(GL_POLYGON_OFFSET_FILL) and glDisable(GL_POLYGON_OFFSET_FILL).
   * @param[in] enable True if GL_POLYGON_OFFSET_FILL should be enabled.
   */
  void SetPolygonOffsetFill(bool enable)
  {
    // Avoid unecessary calls to glEnable/glDisable
    if (enable != mPolygonOffsetFillEnabled)
    {
      mPolygonOffsetFillEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_POLYGON_OFFSET_FILL\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_POLYGON_OFFSET_FILL) );
      }
      else
      {
        LOG_GL("Disable GL_POLYGON_OFFSET_FILL\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_POLYGON_OFFSET_FILL) );
      }
    }
  }

  /**
   * This method replaces glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE) and glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE).
   * @param[in] enable True if GL_SAMPLE_ALPHA_TO_COVERAGE should be enabled.
   */
  void SetSampleAlphaToCoverage(bool enable)
  {
    // Avoid unecessary calls to glEnable/glDisable
    if (enable != mSampleAlphaToCoverageEnabled)
    {
      mSampleAlphaToCoverageEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_SAMPLE_ALPHA_TO_COVERAGE\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_SAMPLE_ALPHA_TO_COVERAGE) );
      }
      else
      {
        LOG_GL("Disable GL_SAMPLE_ALPHA_TO_COVERAGE\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_SAMPLE_ALPHA_TO_COVERAGE) );
      }
    }
  }

  /**
   * This method replaces glEnable(GL_SAMPLE_COVERAGE) and glDisable(GL_SAMPLE_COVERAGE).
   * @param[in] enable True if GL_SAMPLE_COVERAGE should be enabled.
   */
  void SetSampleCoverage(bool enable)
  {
    // Avoid unecessary calls to glEnable/glDisable
    if (enable != mSampleCoverageEnabled)
    {
      mSampleCoverageEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_SAMPLE_COVERAGE\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_SAMPLE_COVERAGE) );
      }
      else
      {
        LOG_GL("Disable GL_SAMPLE_COVERAGE\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_SAMPLE_COVERAGE) );
      }
    }
  }

  /**
   * This method replaces glEnable(GL_SCISSOR_TEST) and glDisable(GL_SCISSOR_TEST).
   * @param[in] enable True if GL_SCISSOR_TEST should be enabled.
   */
  void SetScissorTest(bool enable)
  {
    // Avoid unecessary calls to glEnable/glDisable
    if (enable != mScissorTestEnabled)
    {
      mScissorTestEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_SCISSOR_TEST\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_SCISSOR_TEST) );
      }
      else
      {
        LOG_GL("Disable GL_SCISSOR_TEST\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_SCISSOR_TEST) );
      }
    }
  }

  /**
   * This method replaces glEnable(GL_STENCIL_TEST) and glDisable(GL_STENCIL_TEST).
   * Note GL_STENCIL_TEST means enable the stencil buffer for writing and or testing.
   * glStencilMask is used to control how bits are written to the stencil buffer.
   * glStencilFunc is used to control if testing is enabled and how it is performed ( default GL_ALWAYS )
   * @param[in] enable True if GL_STENCIL_TEST should be enabled.
   */
  void EnableStencilBuffer(bool enable)
  {
    // Avoid unecessary calls to glEnable/glDisable
    if( enable != mStencilBufferEnabled )
    {
      mStencilBufferEnabled = enable;

      if (enable)
      {
        LOG_GL("Enable GL_STENCIL_TEST\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Enable(GL_STENCIL_TEST) );
      }
      else
      {
        LOG_GL("Disable GL_STENCIL_TEST\n");
        CHECK_GL( mGlAbstraction, mGlAbstraction.Disable(GL_STENCIL_TEST) );
      }
    }
  }

  /**
   * Wrapper for OpenGL ES 3.0 glEndQuery()
   */
  void EndQuery(GLenum target)
  {
    LOG_GL("EndQuery %d\n", target);
    CHECK_GL( mGlAbstraction, mGlAbstraction.EndQuery(target) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glEndTransformFeedback()
   */
  void EndTransformFeedback()
  {
    LOG_GL("EndTransformFeedback\n");
    CHECK_GL( mGlAbstraction, mGlAbstraction.EndTransformFeedback() );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glFinish()
   */
  void Finish(void)
  {
    LOG_GL("Finish\n");
    CHECK_GL( mGlAbstraction, mGlAbstraction.Finish() );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glFlush()
   */
  void Flush(void)
  {
    LOG_GL("Flush\n");
    CHECK_GL( mGlAbstraction, mGlAbstraction.Flush() );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glFramebufferRenderbuffer()
   */
  void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
  {
    LOG_GL("FramebufferRenderbuffer %x %x %x %d\n", target, attachment, renderbuffertarget, renderbuffer);
    CHECK_GL( mGlAbstraction, mGlAbstraction.FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glFramebufferTexture2D()
   */
  void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
  {
    LOG_GL("FramebufferTexture2D %x %x %x %d %d\n", target, attachment, textarget, texture, level);
    CHECK_GL( mGlAbstraction, mGlAbstraction.FramebufferTexture2D(target, attachment, textarget, texture, level) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glFramebufferTextureLayer()
   */
  void FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
  {
    LOG_GL("FramebufferTextureLayer %x %x %d %d %d\n", target, attachment, texture, level, layer);
    CHECK_GL( mGlAbstraction, mGlAbstraction.FramebufferTextureLayer(target, attachment, texture, level, layer) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glFrontFace()
   */
  void FrontFace(GLenum mode)
  {
    LOG_GL("FrontFace %x\n", mode);
    CHECK_GL( mGlAbstraction, mGlAbstraction.FrontFace(mode) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGenBuffers()
   */
  void GenBuffers(GLsizei n, GLuint* buffers)
  {
    LOG_GL("GenBuffers %d\n", n, buffers);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GenBuffers(n, buffers) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGenerateMipmap()
   */
  void GenerateMipmap(GLenum target)
  {
    LOG_GL("GenerateMipmap %x\n", target);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GenerateMipmap(target) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGenFramebuffers()
   */
  void GenFramebuffers(GLsizei n, GLuint* framebuffers)
  {
    LOG_GL("GenFramebuffers %d %p\n", n, framebuffers);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GenFramebuffers(n, framebuffers) );

    mFrameBufferStateCache.FrameBuffersCreated( n, framebuffers );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGenRenderbuffers()
   */
  void GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
  {
    LOG_GL("GenRenderbuffers %d %p\n", n, renderbuffers);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GenRenderbuffers(n, renderbuffers) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGenTextures()
   */
  void GenTextures(GLsizei n, GLuint* textures)
  {
    LOG_GL("GenTextures %d %p\n", n, textures);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GenTextures(n, textures) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetBooleanv()
   */
  void GetBooleanv(GLenum pname, GLboolean* params)
  {
    LOG_GL("GetBooleanv %x\n", pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetBooleanv(pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetBufferParameteriv()
   */
  void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
  {
    LOG_GL("GetBufferParameteriv %x %x %p\n", target, pname, params);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetBufferParameteriv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glGetBufferPointer()
   */
  void GetBufferPointerv(GLenum target, GLenum pname, GLvoid** params)
  {
    LOG_GL("GetBufferPointerv %x %x %p\n", target, pname, params);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetBufferPointerv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetError()
   */
  GLenum GetError(void)
  {
    // Not worth logging here
    return mGlAbstraction.GetError();
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetFloatv()
   */
  void GetFloatv(GLenum pname, GLfloat* params)
  {
    LOG_GL("GetFloatv %x\n", pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetFloatv(pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetFramebufferAttachmentParameteriv()
   */
  void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
  {
    LOG_GL("GetFramebufferAttachmentParameteriv %x %x %x\n", target, attachment, pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetFramebufferAttachmentParameteriv(target, attachment, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetIntegerv()
   */
  void GetIntegerv(GLenum pname, GLint* params)
  {
    LOG_GL("GetIntegerv %x\n", pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetIntegerv(pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glGetQueryiv()
   */
  void GetQueryiv(GLenum target, GLenum pname, GLint* params)
  {
    LOG_GL("GetQueryiv %x %x\n", target, pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetQueryiv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glGetQueryObjectuiv()
   */
  void GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params)
  {
    LOG_GL("GetQueryObjectuiv %u %x %p\n", id, pname, params);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetQueryObjectuiv(id, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetRenderbufferParameteriv()
   */
  void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
  {
    LOG_GL("GetRenderbufferParameteriv %x %x\n", target, pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetRenderbufferParameteriv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetString()
   */
  const GLubyte* GetString(GLenum name)
  {
    LOG_GL("GetString %x\n", name);
    const GLubyte* str = CHECK_GL( mGlAbstraction, mGlAbstraction.GetString(name) );
    return str;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetTexParameterfv()
   */
  void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
  {
    LOG_GL("GetTexParameterfv %x %x\n", target, pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetTexParameterfv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glGetTexParameteriv()
   */
  void GetTexParameteriv(GLenum target, GLenum pname, GLint* params)
  {
    LOG_GL("GetTexParameteriv %x %x\n", target, pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetTexParameteriv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glHint()
   */
  void Hint(GLenum target, GLenum mode)
  {
    LOG_GL("Hint %x %x\n", target, mode);
    CHECK_GL( mGlAbstraction, mGlAbstraction.Hint(target, mode) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glIsBuffer()
   */
  GLboolean IsBuffer(GLuint buffer)
  {
    LOG_GL("IsBuffer %d\n", buffer);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.IsBuffer(buffer) );
    return val;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glIsEnabled()
   */
  GLboolean IsEnabled(GLenum cap)
  {
    LOG_GL("IsEnabled %x\n", cap);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.IsEnabled(cap) );
    return val;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glIsFramebuffer()
   */
  GLboolean IsFramebuffer(GLuint framebuffer)
  {
    LOG_GL("IsFramebuffer %d\n", framebuffer);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.IsFramebuffer(framebuffer) );
    return val;
  }

  /**
   * Wrapper for OpenGL ES 3.0 glIsQuery()
   */
  GLboolean IsQuery(GLuint id)
  {
    LOG_GL("IsQuery %u\n", id);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.IsQuery(id) );
    return val;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glIsRenderbuffer()
   */
  GLboolean IsRenderbuffer(GLuint renderbuffer)
  {
    LOG_GL("IsRenderbuffer %d\n", renderbuffer);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.IsRenderbuffer(renderbuffer) );
    return val;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glIsTexture()
   */
  GLboolean IsTexture(GLuint texture)
  {
    LOG_GL("IsTexture %d\n", texture);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.IsTexture(texture) );
    return val;
  }

  /**
   * Wrapper for OpenGL ES 3.0 glIsTransformFeedback()
   */
  GLboolean IsTransformFeedback(GLuint id)
  {
    LOG_GL("IsTransformFeedback %u\n", id);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.IsTransformFeedback(id) );
    return val;
  }

  /**
   * Wrapper for OpenGL ES 2.0 glLineWidth()
   */
  void LineWidth(GLfloat width)
  {
    LOG_GL("LineWidth %f\n", width);
    CHECK_GL( mGlAbstraction, mGlAbstraction.LineWidth(width) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glPauseTransformFeedback()
   */
  void PauseTransformFeedback()
  {
    LOG_GL("PauseTransformFeedback\n");
    CHECK_GL( mGlAbstraction, mGlAbstraction.PauseTransformFeedback() );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glPixelStorei()
   */
  void PixelStorei(GLenum pname, GLint param)
  {
    LOG_GL("PixelStorei %x %d\n", pname, param);
    CHECK_GL( mGlAbstraction, mGlAbstraction.PixelStorei(pname, param) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glPolygonOffset()
   */
  void PolygonOffset(GLfloat factor, GLfloat units)
  {
    LOG_GL("PolygonOffset %f %f\n", factor, units);
    CHECK_GL( mGlAbstraction, mGlAbstraction.PolygonOffset(factor, units) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glReadPixels()
   */
  void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)
  {
    LOG_GL("ReadPixels %d %d %d %d %x %x\n", x, y, width, height, format, type);
    CHECK_GL( mGlAbstraction, mGlAbstraction.ReadPixels(x, y, width, height, format, type, pixels) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glRenderbufferStorage()
   */
  void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
  {
    LOG_GL("RenderbufferStorage %x %x %d %d\n", target, internalformat, width, height);
    CHECK_GL( mGlAbstraction, mGlAbstraction.RenderbufferStorage(target, internalformat, width, height) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glRenderbufferStorageMultisample()
   */
  void RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
  {
    LOG_GL("RenderbufferStorageMultisample %x %u %x %d %d\n", target, samples, internalformat, width, height);
    CHECK_GL( mGlAbstraction, mGlAbstraction.RenderbufferStorageMultisample(target, samples, internalformat, width, height) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glResumeTransformFeedback()
   */
  void ResumeTransformFeedback()
  {
    LOG_GL("ResumeTransformFeedback\n");
    CHECK_GL( mGlAbstraction, mGlAbstraction.ResumeTransformFeedback() );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glSampleCoverage()
   */
  void SampleCoverage(GLclampf value, GLboolean invert)
  {
    LOG_GL("SampleCoverage %f %s\n", value, invert ? "True" : "False");
    CHECK_GL( mGlAbstraction, mGlAbstraction.SampleCoverage(value, invert) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glScissor()
   */
  void Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
  {
    LOG_GL("Scissor %d %d %d %d\n", x, y, width, height);
    CHECK_GL( mGlAbstraction, mGlAbstraction.Scissor(x, y, width, height) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glStencilFunc()
   */
  void StencilFunc(GLenum func, GLint ref, GLuint mask)
  {


    LOG_GL("StencilFunc %x %d %d\n", func, ref, mask);
    CHECK_GL( mGlAbstraction, mGlAbstraction.StencilFunc(func, ref, mask) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glStencilFuncSeparate()
   */
  void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
  {
    LOG_GL("StencilFuncSeparate %x %x %d %d\n", face, func, ref, mask);
    CHECK_GL( mGlAbstraction, mGlAbstraction.StencilFuncSeparate(face, func, ref, mask) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glStencilMask()
   */
  void StencilMask(GLuint mask)
  {
    if( mask != mStencilMask )
    {
      mStencilMask = mask;

      LOG_GL("StencilMask %d\n", mask);
      CHECK_GL( mGlAbstraction, mGlAbstraction.StencilMask(mask) );
    }
  }

  /**
   * Wrapper for OpenGL ES 2.0 glStencilMaskSeparate()
   */
  void StencilMaskSeparate(GLenum face, GLuint mask)
  {
    LOG_GL("StencilMaskSeparate %x %d\n", face, mask);
    CHECK_GL( mGlAbstraction, mGlAbstraction.StencilMaskSeparate(face, mask) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glStencilOp()
   */
  void StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
  {
    LOG_GL("StencilOp %x %x %x\n", fail, zfail, zpass);
    CHECK_GL( mGlAbstraction, mGlAbstraction.StencilOp(fail, zfail, zpass) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glStencilOpSeparate()
   */
  void StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
  {
    LOG_GL("StencilOpSeparate %x %x %x %x\n", face, fail, zfail, zpass);
    CHECK_GL( mGlAbstraction, mGlAbstraction.StencilOpSeparate(face, fail, zfail, zpass) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glTexImage2D()
   */
  void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                  GLint border, GLenum format, GLenum type, const void* pixels)
  {
    LOG_GL("TexImage2D %x %d %d %dx%d %d %x %x %p\n", target, level, internalformat, width, height, border, format, type, pixels);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexImage2D(target, level, internalformat, width, height, border, format, type, pixels) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glTexImage3D()
   */
  void TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth,
                  GLint border, GLenum format, GLenum type, const void* pixels)
  {
    LOG_GL("TexImage3D %x %d %d %dx%dx%d %d %x %x %p\n", target, level, internalformat, width, height, depth, border, format, type, pixels);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glTexParameterf()
   */
  void TexParameterf(GLenum target, GLenum pname, GLfloat param)
  {
    LOG_GL("TexParameterf %x %x %f\n", target, pname, param);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexParameterf(target, pname, param) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glTexParameterfv()
   */
  void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
  {
    LOG_GL("TexParameterfv %x %x\n", target, pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexParameterfv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glTexParameteri()
   */
  void TexParameteri(GLenum target, GLenum pname, GLint param)
  {
    LOG_GL("TexParameteri %x %x %d\n", target, pname, param);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexParameteri(target, pname, param) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glTexParameteriv()
   */
  void TexParameteriv(GLenum target, GLenum pname, const GLint* params)
  {
    LOG_GL("TexParameteriv %x %x\n", target, pname);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexParameteriv(target, pname, params) );
  }

  /**
   * Wrapper for OpenGL ES 2.0 glTexSubImage2D()
   */
  void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                     GLenum format, GLenum type, const void* pixels)
  {
    LOG_GL("TexSubImage2D %x %d %d %d %d %d %x %x %p\n", target, level, xoffset, yoffset, width, height, format, type, pixels);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glTexSubImage3D()
   */
  void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                     GLsizei width, GLsizei height, GLsizei depth,
                     GLenum format, GLenum type, const void* pixels)
  {
    LOG_GL("TexSubImage3D %x %d %d %d %d %d %d %d %x %x %p\n", target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    CHECK_GL( mGlAbstraction, mGlAbstraction.TexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels) );
  }

  /**
   * Wrapper for OpenGL ES 3.0 glUnmapBubffer()
   */
  GLboolean UnmapBuffer(GLenum target)
  {
    LOG_GL("UnmapBuffer %x \n", target);
    GLboolean val = CHECK_GL( mGlAbstraction, mGlAbstraction.UnmapBuffer(target) );
    return val;
  }
  /**
   * Wrapper for OpenGL ES 2.0 glViewport()
   */
  void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
  {
    // check if its same as already set
    Rect<int> newViewport( x, y, width, height );
    if( mViewPort != newViewport )
    {
      // set new one
      LOG_GL("Viewport %d %d %d %d\n", x, y, width, height);
      CHECK_GL( mGlAbstraction, mGlAbstraction.Viewport(x, y, width, height) );
      mViewPort = newViewport; // remember new one
    }
  }

  /**
   * Get the implementation defined MAX_TEXTURE_SIZE. This values is cached when the context is created
   * @return The implementation defined MAX_TEXTURE_SIZE
   */
  GLint CachedMaxTextureSize() const
  {
    return mMaxTextureSize;
  }

  /**
   * Get the current viewport.
   * @return Viewport rectangle.
   */
  const Rect< int >& GetViewport();

  /**
   * Set the frame count of render thread
   */
  inline void SetFrameCount(unsigned int frameCount)
  {
    mFrameCount = frameCount;
  }

  /**
   * Get the frame count
   */
  inline unsigned int GetFrameCount()
  {
    return mFrameCount;
  }

  /**
   * Increment the count of culled renderers
   */
  inline void IncrementCulledCount()
  {
    mCulledCount++;
  }

  /**
   * Clear the count of culled renderers
   */
  inline void ClearCulledCount()
  {
    mCulledCount = 0;
  }

  /**
   * Get the count of culled renderers in this frame
   */
  inline unsigned int GetCulledCount()
  {
    return mCulledCount;
  }

  /**
   * Increment the count of culled renderers
   */
  inline void IncrementRendererCount()
  {
    mRendererCount++;
  }

  /**
   * Clear the count of image renderers
   */
  inline void ClearRendererCount()
  {
    mRendererCount = 0;
  }

  /**
   * Get the count of image renderers in this frame
   */
  inline unsigned int GetRendererCount()
  {
    return mRendererCount;
  }

private: // Implementation

  /**
   * @return true if next draw operation will write to depth buffer
   */
  bool DepthBufferWriteEnabled() const
  {
    return mDepthBufferEnabled && mDepthMaskEnabled;
  }

  /**
   * @return true if next draw operation will write to stencil buffer
   */
  bool StencilBufferWriteEnabled() const
  {
    return mStencilBufferEnabled && ( mStencilMask > 0 );
  }

  /**
   * Flushes vertex attribute location changes to the driver
   */
  void FlushVertexAttributeLocations();

  /**
   * Reset the cached internal vertex attribute state
   */
  void ResetVertexAttributeState();

  /**
   * Either enables or disables a vertex attribute location in the cache
   * The cahnges won't take affect until FlushVertexAttributeLocations is called
   * @param location attribute location
   * @param state attribute state
   */
  void SetVertexAttributeLocation(unsigned int location, bool state);

  /**
   * Sets the initial GL state.
   */
  void ResetGlState();

private: // Data

  Integration::GlAbstraction& mGlAbstraction;

  bool mGlContextCreated; ///< True if the OpenGL context has been created

  // glEnable/glDisable states
  bool mColorMask;
  GLuint mStencilMask;
  bool mBlendEnabled;
  bool mDepthBufferEnabled;
  bool mDepthMaskEnabled;
  bool mDitherEnabled;
  bool mPolygonOffsetFillEnabled;
  bool mSampleAlphaToCoverageEnabled;
  bool mSampleCoverageEnabled;
  bool mScissorTestEnabled;
  bool mStencilBufferEnabled;
  bool mClearColorSet;

  // glBindBuffer() state
  GLuint mBoundArrayBufferId;        ///< The ID passed to glBindBuffer(GL_ARRAY_BUFFER)
  GLuint mBoundElementArrayBufferId; ///< The ID passed to glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)
  GLuint mBoundTransformFeedbackBufferId; ///< The ID passed to glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER)

  // glBindTexture() state
  TextureUnit mActiveTextureUnit;
  GLuint mBound2dTextureId[ MAX_TEXTURE_UNITS ];  ///< The ID passed to glBindTexture(GL_TEXTURE_2D)

  // glBlendColor() state
  bool mUsingDefaultBlendColor;

  // glBlendFuncSeparate() state
  GLenum mBlendFuncSeparateSrcRGB;   ///< The srcRGB parameter passed to glBlendFuncSeparate()
  GLenum mBlendFuncSeparateDstRGB;   ///< The dstRGB parameter passed to glBlendFuncSeparate()
  GLenum mBlendFuncSeparateSrcAlpha; ///< The srcAlpha parameter passed to glBlendFuncSeparate()
  GLenum mBlendFuncSeparateDstAlpha; ///< The dstAlpha parameter passed to glBlendFuncSeparate()

  // glBlendEquationSeparate state
  GLenum mBlendEquationSeparateModeRGB;    ///< Controls RGB blend mode
  GLenum mBlendEquationSeparateModeAlpha;  ///< Controls Alpha blend mode

  GLint mMaxTextureSize;      ///< return value from GetIntegerv(GL_MAX_TEXTURE_SIZE)
  Vector4 mClearColor;        ///< clear color

  // Face culling mode
  CullFaceMode mCullFaceMode;

  // cached viewport size
  Rect< int > mViewPort;

  // Vertex Attribute Buffer enable caching
  bool mVertexAttributeCachedState[ MAX_ATTRIBUTE_CACHE_SIZE ];    ///< Value cache for Enable Vertex Attribute
  bool mVertexAttributeCurrentState[ MAX_ATTRIBUTE_CACHE_SIZE ];   ///< Current state on the driver for Enable Vertex Attribute

  unsigned int mFrameCount;       ///< Number of render frames
  unsigned int mCulledCount;      ///< Number of culled renderers per frame
  unsigned int mRendererCount;    ///< Number of image renderers per frame
  FrameBufferStateCache mFrameBufferStateCache;   ///< frame buffer state cache
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CONTEXT_H__
