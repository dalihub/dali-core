#ifndef __DALI_INTERNAL_FRAME_BUFFER_TEXTURE_H__
#define __DALI_INTERNAL_FRAME_BUFFER_TEXTURE_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/images/frame-buffer-image.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

/**
 * Texture class.
 * Used as a frame buffer for RenderTask
 */
class FrameBufferTexture : public Texture
{
public:

  /**
   * Creates a new texture object
   * @param[in] width       The width (pixels)
   * @param[in] height      The height (pixels)
   * @param[in] context The GL context
   */
  FrameBufferTexture(unsigned int width, unsigned int height, Context& context);

  /**
   * Creates a new texture object
   * @param[in] width       The width (pixels)
   * @param[in] height      The height (pixels)
   * @param[in] pixelFormat The pixel format
   * @param[in] context The GL context
   */
  FrameBufferTexture(unsigned int width, unsigned int height, Pixel::Format pixelFormat, Context& context);

  /**
   * Creates a new texture object
   * @param[in] width         The width (pixels)
   * @param[in] height        The height (pixels)
   * @param[in] pixelFormat   The pixel format
   * @param[in] bufferFormat  The buffer formats to be attached in the FBO
   * @param[in] context The GL context
   */
  FrameBufferTexture(unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat, Context& context);

  /**
   * Destructor.
   */
  virtual ~FrameBufferTexture();

  /**
   * @copydoc Texture::IsFullyOpaque
   */
  virtual bool IsFullyOpaque() const;

  /**
   * @copydoc Texture::HasAlphaChannel
   */
  virtual bool HasAlphaChannel() const;

  /**
   * @copydoc Texture::Init
   */
  virtual bool Init();

  /**
   * Prepare FBO for rendering.
   * @return true if the FBO is prepared and its status is GL_FRAMEBUFFER_COMPLETE, otherwise false.
   */
  virtual bool Prepare();

protected:
  unsigned int mFrameBufferName;
  unsigned int mRenderBufferName;
  unsigned int mStencilBufferName;
  Pixel::Format mPixelFormat;
  RenderBuffer::Format mBufferFormat;

  /**
   * @copydoc Texture::CreateGlTexture
   */
  virtual bool CreateGlTexture();

  /**
   * @copydoc Texture::GlCleanup
   */
  virtual void GlCleanup();

}; // class FrameBufferTexture

}  //namespace Internal

} //namespace Dali
#endif //__DALI_INTERNAL_FRAME_BUFFER_TEXTURE_H__

