#ifndef __DALI_INTEGRATION_GL_ABSTRACTION_H__
#define __DALI_INTEGRATION_GL_ABSTRACTION_H__

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

#include <stdint.h>

/*
 * This file is based on gl3.h, the following licence is included for conformance.
 */
/*
** Copyright (c) 2007-2012 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/


/* OpenGL ES 3.0 */
struct __GLsync;

namespace Dali
{

/**
 * These types are equivalent to those in the GLES2 API.
 * Dali objects should only access GL indirectly, through the Context API.
 */

/* OpenGL ES 2.0 */

typedef void             GLvoid;
typedef char             GLchar;
typedef unsigned int     GLenum;
typedef unsigned char    GLboolean;
typedef unsigned int     GLbitfield;
typedef int8_t           GLbyte;
typedef short            GLshort;
typedef int              GLint;
typedef int              GLsizei;
typedef uint8_t          GLubyte;
typedef unsigned short   GLushort;
typedef unsigned int     GLuint;
typedef float            GLfloat;
typedef float            GLclampf;
typedef int              GLfixed;
typedef signed long int  GLintptr;
typedef signed long int  GLsizeiptr;

/* OpenGL ES 3.0 */

typedef unsigned short     GLhalf;
typedef int64_t            GLint64;
typedef uint64_t           GLuint64;
typedef __GLsync* GLsync;

namespace Integration
{

/**
 * GlAbstraction is an abstract interface, used to access OpenGL services.
 * A concrete implementation must be created for each platform, and provided when creating the
 * Dali::Integration::Core object.
 */
class GlAbstraction
{
protected:

  /**
   * Virtual protected destructor, no deletion through this interface
   */
  virtual ~GlAbstraction() {}

public:
  /**
   * Invoked by Render thread before Core::Render.
   */
  virtual void PreRender() = 0;

  /**
   * Invoked by Render thread after Core::Render
   * Inform the gl implementation that the rendering in this frame has finished,
   * and how much time was spent.
   * @param[in] timeDelta Time in microseconds since last PostRender call
   */
  virtual void PostRender( unsigned int timeDelta ) = 0;

  /**
   * The number of texture units an implementation supports is implementation dependent, but must be at least 8.
   */
  static const unsigned int MIN_TEXTURE_UNIT_LIMIT = 8;

  /* OpenGL ES 2.0 */

  virtual void         ActiveTexture (GLenum texture) = 0;
  virtual void         AttachShader (GLuint program, GLuint shader) = 0;
  virtual void         BindAttribLocation (GLuint program, GLuint index, const char* name) = 0;
  virtual void         BindBuffer (GLenum target, GLuint buffer) = 0;
  virtual void         BindFramebuffer (GLenum target, GLuint framebuffer) = 0;
  virtual void         BindRenderbuffer (GLenum target, GLuint renderbuffer) = 0;
  virtual void         BindTexture (GLenum target, GLuint texture) = 0;
  virtual void         BlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;
  virtual void         BlendEquation ( GLenum mode ) = 0;
  virtual void         BlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha) = 0;
  virtual void         BlendFunc (GLenum sfactor, GLenum dfactor) = 0;
  virtual void         BlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) = 0;
  virtual void         BufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage) = 0;
  virtual void         BufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void* data) = 0;
  virtual GLenum       CheckFramebufferStatus (GLenum target) = 0;
  virtual void         Clear (GLbitfield mask) = 0;
  virtual void         ClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;
  virtual void         ClearDepthf (GLclampf depth) = 0;
  virtual void         ClearStencil (GLint s) = 0;
  virtual void         ColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = 0;
  virtual void         CompileShader (GLuint shader) = 0;
  virtual void         CompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) = 0;
  virtual void         CompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) = 0;
  virtual void         CopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) = 0;
  virtual void         CopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual GLuint       CreateProgram (void) = 0;
  virtual GLuint       CreateShader (GLenum type) = 0;
  virtual void         CullFace (GLenum mode) = 0;
  virtual void         DeleteBuffers (GLsizei n, const GLuint* buffers) = 0;
  virtual void         DeleteFramebuffers (GLsizei n, const GLuint* framebuffers) = 0;
  virtual void         DeleteProgram (GLuint program) = 0;
  virtual void         DeleteRenderbuffers (GLsizei n, const GLuint* renderbuffers) = 0;
  virtual void         DeleteShader (GLuint shader) = 0;
  virtual void         DeleteTextures (GLsizei n, const GLuint* textures) = 0;
  virtual void         DepthFunc (GLenum func) = 0;
  virtual void         DepthMask (GLboolean flag) = 0;
  virtual void         DepthRangef (GLclampf zNear, GLclampf zFar) = 0;
  virtual void         DetachShader (GLuint program, GLuint shader) = 0;
  virtual void         Disable (GLenum cap) = 0;
  virtual void         DisableVertexAttribArray (GLuint index) = 0;
  virtual void         DrawArrays (GLenum mode, GLint first, GLsizei count) = 0;
  virtual void         DrawElements (GLenum mode, GLsizei count, GLenum type, const void* indices) = 0;
  virtual void         Enable (GLenum cap) = 0;
  virtual void         EnableVertexAttribArray (GLuint index) = 0;
  virtual void         Finish (void) = 0;
  virtual void         Flush (void) = 0;
  virtual void         FramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = 0;
  virtual void         FramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = 0;
  virtual void         FrontFace (GLenum mode) = 0;
  virtual void         GenBuffers (GLsizei n, GLuint* buffers) = 0;
  virtual void         GenerateMipmap (GLenum target) = 0;
  virtual void         GenFramebuffers (GLsizei n, GLuint* framebuffers) = 0;
  virtual void         GenRenderbuffers (GLsizei n, GLuint* renderbuffers) = 0;
  virtual void         GenTextures (GLsizei n, GLuint* textures) = 0;
  virtual void         GetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) = 0;
  virtual void         GetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) = 0;
  virtual void         GetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders) = 0;
  virtual GLint        GetAttribLocation (GLuint program, const char* name) = 0;
  virtual void         GetBooleanv (GLenum pname, GLboolean* params) = 0;
  virtual void         GetBufferParameteriv (GLenum target, GLenum pname, GLint* params) = 0;
  virtual GLenum       GetError (void) = 0;
  virtual void         GetFloatv (GLenum pname, GLfloat* params) = 0;
  virtual void         GetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params) = 0;
  virtual void         GetIntegerv (GLenum pname, GLint* params) = 0;
  virtual void         GetProgramiv (GLuint program, GLenum pname, GLint* params) = 0;
  virtual void         GetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog) = 0;
  virtual void         GetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params) = 0;
  virtual void         GetShaderiv (GLuint shader, GLenum pname, GLint* params) = 0;
  virtual void         GetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog) = 0;
  virtual void         GetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) = 0;
  virtual void         GetShaderSource (GLuint shader, GLsizei bufsize, GLsizei* length, char* source) = 0;
  virtual const GLubyte* GetString (GLenum name) = 0;
  virtual void         GetTexParameterfv (GLenum target, GLenum pname, GLfloat* params) = 0;
  virtual void         GetTexParameteriv (GLenum target, GLenum pname, GLint* params) = 0;
  virtual void         GetUniformfv (GLuint program, GLint location, GLfloat* params) = 0;
  virtual void         GetUniformiv (GLuint program, GLint location, GLint* params) = 0;
  virtual GLint        GetUniformLocation (GLuint program, const char* name) = 0;
  virtual void         GetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params) = 0;
  virtual void         GetVertexAttribiv (GLuint index, GLenum pname, GLint* params) = 0;
  virtual void         GetVertexAttribPointerv (GLuint index, GLenum pname, void** pointer) = 0;
  virtual void         Hint (GLenum target, GLenum mode) = 0;
  virtual GLboolean    IsBuffer (GLuint buffer) = 0;
  virtual GLboolean    IsEnabled (GLenum cap) = 0;
  virtual GLboolean    IsFramebuffer (GLuint framebuffer) = 0;
  virtual GLboolean    IsProgram (GLuint program) = 0;
  virtual GLboolean    IsRenderbuffer (GLuint renderbuffer) = 0;
  virtual GLboolean    IsShader (GLuint shader) = 0;
  virtual GLboolean    IsTexture (GLuint texture) = 0;
  virtual void         LineWidth (GLfloat width) = 0;
  virtual void         LinkProgram (GLuint program) = 0;
  virtual void         PixelStorei (GLenum pname, GLint param) = 0;
  virtual void         PolygonOffset (GLfloat factor, GLfloat units) = 0;
  virtual void         ReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) = 0;
  virtual void         ReleaseShaderCompiler (void) = 0;
  virtual void         RenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = 0;
  virtual void         SampleCoverage (GLclampf value, GLboolean invert) = 0;
  virtual void         Scissor (GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void         ShaderBinary (GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length) = 0;
  virtual void         ShaderSource (GLuint shader, GLsizei count, const char** string, const GLint* length) = 0;
  virtual void         StencilFunc (GLenum func, GLint ref, GLuint mask) = 0;
  virtual void         StencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask) = 0;
  virtual void         StencilMask (GLuint mask) = 0;
  virtual void         StencilMaskSeparate (GLenum face, GLuint mask) = 0;
  virtual void         StencilOp (GLenum fail, GLenum zfail, GLenum zpass) = 0;
  virtual void         StencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass) = 0;
  virtual void         TexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) = 0;
  virtual void         TexParameterf (GLenum target, GLenum pname, GLfloat param) = 0;
  virtual void         TexParameterfv (GLenum target, GLenum pname, const GLfloat* params) = 0;
  virtual void         TexParameteri (GLenum target, GLenum pname, GLint param) = 0;
  virtual void         TexParameteriv (GLenum target, GLenum pname, const GLint* params) = 0;
  virtual void         TexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) = 0;
  virtual void         Uniform1f (GLint location, GLfloat x) = 0;
  virtual void         Uniform1fv (GLint location, GLsizei count, const GLfloat* v) = 0;
  virtual void         Uniform1i (GLint location, GLint x) = 0;
  virtual void         Uniform1iv (GLint location, GLsizei count, const GLint* v) = 0;
  virtual void         Uniform2f (GLint location, GLfloat x, GLfloat y) = 0;
  virtual void         Uniform2fv (GLint location, GLsizei count, const GLfloat* v) = 0;
  virtual void         Uniform2i (GLint location, GLint x, GLint y) = 0;
  virtual void         Uniform2iv (GLint location, GLsizei count, const GLint* v) = 0;
  virtual void         Uniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z) = 0;
  virtual void         Uniform3fv (GLint location, GLsizei count, const GLfloat* v) = 0;
  virtual void         Uniform3i (GLint location, GLint x, GLint y, GLint z) = 0;
  virtual void         Uniform3iv (GLint location, GLsizei count, const GLint* v) = 0;
  virtual void         Uniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) = 0;
  virtual void         Uniform4fv (GLint location, GLsizei count, const GLfloat* v) = 0;
  virtual void         Uniform4i (GLint location, GLint x, GLint y, GLint z, GLint w) = 0;
  virtual void         Uniform4iv (GLint location, GLsizei count, const GLint* v) = 0;
  virtual void         UniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void         UniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void         UniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void         UseProgram (GLuint program) = 0;
  virtual void         ValidateProgram (GLuint program) = 0;
  virtual void         VertexAttrib1f (GLuint indx, GLfloat x) = 0;
  virtual void         VertexAttrib1fv (GLuint indx, const GLfloat* values) = 0;
  virtual void         VertexAttrib2f (GLuint indx, GLfloat x, GLfloat y) = 0;
  virtual void         VertexAttrib2fv (GLuint indx, const GLfloat* values) = 0;
  virtual void         VertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z) = 0;
  virtual void         VertexAttrib3fv (GLuint indx, const GLfloat* values) = 0;
  virtual void         VertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w) = 0;
  virtual void         VertexAttrib4fv (GLuint indx, const GLfloat* values) = 0;
  virtual void         VertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr) = 0;
  virtual void         Viewport (GLint x, GLint y, GLsizei width, GLsizei height) = 0;

  /* OpenGL ES 3.0 */

  virtual void           ReadBuffer (GLenum mode) = 0;
  virtual void           DrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices) = 0;
  virtual void           TexImage3D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels) = 0;
  virtual void           TexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels) = 0;
  virtual void           CopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void           CompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data) = 0;
  virtual void           CompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data) = 0;
  virtual void           GenQueries (GLsizei n, GLuint* ids) = 0;
  virtual void           DeleteQueries (GLsizei n, const GLuint* ids) = 0;
  virtual GLboolean      IsQuery (GLuint id) = 0;
  virtual void           BeginQuery (GLenum target, GLuint id) = 0;
  virtual void           EndQuery (GLenum target) = 0;
  virtual void           GetQueryiv (GLenum target, GLenum pname, GLint* params) = 0;
  virtual void           GetQueryObjectuiv (GLuint id, GLenum pname, GLuint* params) = 0;
  virtual GLboolean      UnmapBuffer (GLenum target) = 0;
  virtual void           GetBufferPointerv (GLenum target, GLenum pname, GLvoid** params) = 0;
  virtual void           DrawBuffers (GLsizei n, const GLenum* bufs) = 0;
  virtual void           UniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void           UniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void           UniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void           UniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void           UniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void           UniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = 0;
  virtual void           BlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) = 0;
  virtual void           RenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) = 0;
  virtual void           FramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) = 0;
  virtual GLvoid*        MapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) = 0;
  virtual void           FlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length) = 0;
  virtual void           BindVertexArray (GLuint array) = 0;
  virtual void           DeleteVertexArrays (GLsizei n, const GLuint* arrays) = 0;
  virtual void           GenVertexArrays (GLsizei n, GLuint* arrays) = 0;
  virtual GLboolean      IsVertexArray (GLuint array) = 0;
  virtual void           GetIntegeri_v (GLenum target, GLuint index, GLint* data) = 0;
  virtual void           BeginTransformFeedback (GLenum primitiveMode) = 0;
  virtual void           EndTransformFeedback (void) = 0;
  virtual void           BindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) = 0;
  virtual void           BindBufferBase (GLenum target, GLuint index, GLuint buffer) = 0;
  virtual void           TransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode) = 0;
  virtual void           GetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name) = 0;
  virtual void           VertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) = 0;
  virtual void           GetVertexAttribIiv (GLuint index, GLenum pname, GLint* params) = 0;
  virtual void           GetVertexAttribIuiv (GLuint index, GLenum pname, GLuint* params) = 0;
  virtual void           VertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w) = 0;
  virtual void           VertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) = 0;
  virtual void           VertexAttribI4iv (GLuint index, const GLint* v) = 0;
  virtual void           VertexAttribI4uiv (GLuint index, const GLuint* v) = 0;
  virtual void           GetUniformuiv (GLuint program, GLint location, GLuint* params) = 0;
  virtual GLint          GetFragDataLocation (GLuint program, const GLchar *name) = 0;
  virtual void           Uniform1ui (GLint location, GLuint v0) = 0;
  virtual void           Uniform2ui (GLint location, GLuint v0, GLuint v1) = 0;
  virtual void           Uniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2) = 0;
  virtual void           Uniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) = 0;
  virtual void           Uniform1uiv (GLint location, GLsizei count, const GLuint* value) = 0;
  virtual void           Uniform2uiv (GLint location, GLsizei count, const GLuint* value) = 0;
  virtual void           Uniform3uiv (GLint location, GLsizei count, const GLuint* value) = 0;
  virtual void           Uniform4uiv (GLint location, GLsizei count, const GLuint* value) = 0;
  virtual void           ClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint* value) = 0;
  virtual void           ClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint* value) = 0;
  virtual void           ClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat* value) = 0;
  virtual void           ClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) = 0;
  virtual const GLubyte* GetStringi (GLenum name, GLuint index) = 0;
  virtual void           CopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) = 0;
  virtual void           GetUniformIndices (GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices) = 0;
  virtual void           GetActiveUniformsiv (GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params) = 0;
  virtual GLuint         GetUniformBlockIndex (GLuint program, const GLchar* uniformBlockName) = 0;
  virtual void           GetActiveUniformBlockiv (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params) = 0;
  virtual void           GetActiveUniformBlockName (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName) = 0;
  virtual void           UniformBlockBinding (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) = 0;
  virtual void           DrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei instanceCount) = 0;
  virtual void           DrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount) = 0;
  virtual GLsync         FenceSync (GLenum condition, GLbitfield flags) = 0;
  virtual GLboolean      IsSync (GLsync sync) = 0;
  virtual void           DeleteSync (GLsync sync) = 0;
  virtual GLenum         ClientWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout) = 0;
  virtual void           WaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout) = 0;
  virtual void           GetInteger64v (GLenum pname, GLint64* params) = 0;
  virtual void           GetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values) = 0;
  virtual void           GetInteger64i_v (GLenum target, GLuint index, GLint64* data) = 0;
  virtual void           GetBufferParameteri64v (GLenum target, GLenum pname, GLint64* params) = 0;
  virtual void           GenSamplers (GLsizei count, GLuint* samplers) = 0;
  virtual void           DeleteSamplers (GLsizei count, const GLuint* samplers) = 0;
  virtual GLboolean      IsSampler (GLuint sampler) = 0;
  virtual void           BindSampler (GLuint unit, GLuint sampler) = 0;
  virtual void           SamplerParameteri (GLuint sampler, GLenum pname, GLint param) = 0;
  virtual void           SamplerParameteriv (GLuint sampler, GLenum pname, const GLint* param) = 0;
  virtual void           SamplerParameterf (GLuint sampler, GLenum pname, GLfloat param) = 0;
  virtual void           SamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat* param) = 0;
  virtual void           GetSamplerParameteriv (GLuint sampler, GLenum pname, GLint* params) = 0;
  virtual void           GetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat* params) = 0;
  virtual void           VertexAttribDivisor (GLuint index, GLuint divisor) = 0;
  virtual void           BindTransformFeedback (GLenum target, GLuint id) = 0;
  virtual void           DeleteTransformFeedbacks (GLsizei n, const GLuint* ids) = 0;
  virtual void           GenTransformFeedbacks (GLsizei n, GLuint* ids) = 0;
  virtual GLboolean      IsTransformFeedback (GLuint id) = 0;
  virtual void           PauseTransformFeedback (void) = 0;
  virtual void           ResumeTransformFeedback (void) = 0;
  virtual void           GetProgramBinary (GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary) = 0;
  virtual void           ProgramBinary (GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length) = 0;
  virtual void           ProgramParameteri (GLuint program, GLenum pname, GLint value) = 0;
  virtual void           InvalidateFramebuffer (GLenum target, GLsizei numAttachments, const GLenum* attachments) = 0;
  virtual void           InvalidateSubFramebuffer (GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void           TexStorage2D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) = 0;
  virtual void           TexStorage3D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) = 0;
  virtual void           GetInternalformativ (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params) = 0;

};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_GL_ABSTRACTION_H__
