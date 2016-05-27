#ifndef DALI_INTERNAL_RENDER_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_FRAME_BUFFER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */


// INTERNAL INCLUDES
#include <dali/devel-api/rendering/frame-buffer.h>
#include <dali/integration-api/resource-declarations.h>

#include <dali/internal/render/gl-resources/context.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/render/renderers/render-sampler.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class NewTexture;

class FrameBuffer
{
public:

  typedef Dali::FrameBuffer::Format Format;

  /**
   * Constructor
   * @param[in] width The width of the FrameBuffer
   * @param[in] height The height of the FrameBuffer
   * @param[in] format The format of the FrameBuffer
   */
  FrameBuffer( unsigned int width, unsigned int height, Format format );

  /**
   * Destructor
   */
  ~FrameBuffer();

  /**
   * Creates a FrameBuffer object in the GPU.
   * @param[in] context The GL context
   */
  void Initialize(Context& context);

  /**
   * Deletes the framebuffer object from the GPU
   * @param[in] context The GL context
   */
  void Destroy( Context& context );

  /**
   * @brief Attach a texture for color rendering
   * param[in] context The GL context
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   */
  void AttachColorTexture( Context& context, Render::NewTexture* texture, unsigned int mipmapLevel, unsigned int layer );

  /**
   * @brief Bind the framebuffer
   * @param[in] context The GL context
   */
  void Bind( Context& context );

  /**
   * @brief Get the width of the FrameBuffer
   * @return The width of the framebuffer
   */
  unsigned int GetWidth() const;

  /**
   * @brief Get the height of the FrameBuffer
   * @return The height of the framebuffer
   */
  unsigned int GetHeight() const;

private:

  GLuint mId;
  GLuint mDepthBuffer;
  GLuint mStencilBuffer;
  unsigned int mWidth;
  unsigned int mHeight;
};


} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_FRAME_BUFFER_H
