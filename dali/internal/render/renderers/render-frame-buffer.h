#ifndef DALI_INTERNAL_RENDER_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_FRAME_BUFFER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/integration-api/gl-defines.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

class FrameBuffer
{
public:

  /**
   * Constructor
   */
  FrameBuffer() {};

  /**
   * Destructor
   */
  virtual ~FrameBuffer() {};

  /**
   * Creates a FrameBuffer object in the GPU.
   * @param[in] context The GL context
   */
  virtual void Initialize( Context& context ) = 0;

  /**
   * Deletes the framebuffer object from the GPU
   * @param[in] context The GL context
   */
  virtual void Destroy( Context& context ) = 0;

  /**
   * Called by RenderManager to inform the framebuffer that the context has been destroyed
   */
  virtual void GlContextDestroyed() = 0;

  /**
   * @brief Bind the framebuffer
   * @param[in] context The GL context
   */
  virtual void Bind( Context& context ) = 0;

  /**
   * @brief Get the width of the FrameBuffer
   * @return The width of the framebuffer
   */
  virtual uint32_t GetWidth() const = 0;

  /**
   * @brief Get the height of the FrameBuffer
   * @return The height of the framebuffer
   */
  virtual uint32_t GetHeight() const = 0;

  /**
   * @brief Check whether the FrameBuffer is backed by a render surface
   * @return True if the FrameBuffer is backed by a render surface
   */
  virtual bool IsSurfaceBacked() = 0;

private:

  /**
   * @brief Undefined copy constructor. FrameBuffer cannot be copied
   */
  FrameBuffer( const FrameBuffer& rhs ) = delete;

  /**
   * @brief Undefined assignment operator. FrameBuffer cannot be copied
   */
  FrameBuffer& operator=( const FrameBuffer& rhs ) = delete;

};

} // namespace Render

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_RENDER_FRAME_BUFFER_H
