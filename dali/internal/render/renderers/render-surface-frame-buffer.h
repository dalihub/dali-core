#ifndef DALI_INTERNAL_RENDER_SURFACE_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_SURFACE_FRAME_BUFFER_H

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
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/integration-api/render-surface.h>

namespace Dali
{

namespace Internal
{

class Context;

namespace Render
{

class SurfaceFrameBuffer : public FrameBuffer
{
public:

  /**
   * Constructor
   * @param[in] surface The render surface
   */
  SurfaceFrameBuffer( Integration::RenderSurface* surface );

  /**
   * Destructor
   */
  virtual ~SurfaceFrameBuffer();

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::Initialize()
   */
  void Initialize( Context& context ) override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::Destroy()
   */
  void Destroy( Context& context ) override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::GlContextDestroyed()
   */
  void GlContextDestroyed() override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::Bind()
   */
  void Bind( Context& context ) override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::GetWidth()
   */
  uint32_t GetWidth() const override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::GetHeight()
   */
  uint32_t GetHeight() const override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::IsSurfaceBacked()
   */
  bool IsSurfaceBacked() override { return true; };

public:

  /**
   * Called after this frame buffer is rendered in the render manager
   */
  void PostRender();

  /**
   * Gets the context holding the GL state of rendering for the surface
   * @return the context
   */
  Context* GetContext();

  /**
   * @brief Gets whether the depth buffer is required
   * @return TRUE if the depth buffer is required
   */
  Integration::DepthBufferAvailable GetDepthBufferRequired();

  /**
   * @brief Gets whether the stencil buffer is required
   * @return TRUE if the stencil buffer is required
   */
  Integration::StencilBufferAvailable GetStencilBufferRequired();

  /**
   * @brief Gets the background color of the surface.
   * @return The background color
   */
  Vector4 GetBackgroundColor();

private:

  Integration::RenderSurface* mSurface;   ///< The render surface
  Context*                    mContext;   ///< The context holding the GL state of rendering for the surface backed frame buffer
};


} // namespace Render

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_RENDER_SURFACE_FRAME_BUFFER_H
