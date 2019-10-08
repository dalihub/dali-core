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

// EXTERNAL INCLUDES
#include <atomic>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
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

  /**
   * @brief Sets the frame buffer size.
   * @param[in] width The width size
   * @param[in] height The height size
   */
  void SetSize( uint32_t width, uint32_t height );

  /**
   * @brief Sets the background color.
   * @param[in] color The new background color
   */
  void SetBackgroundColor( const Vector4& color );

  /**
   * @copydoc Dali::Internal::FrameBuffer::MarkSurfaceAsInvalid()
   */
  void MarkSurfaceAsInvalid() { mIsSurfaceInvalid = true; };

  /**
   * @brief Gets whether the render surface in this frame buffer is valid or not
   * @note The render surface becomes invalid when it is deleted in the event thread
   * @return Whether the render surface is valid or not
   */
  bool IsSurfaceValid() const;

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
   * @brief Makes the graphics context current
   */
  void MakeContextCurrent();

  /**
   * @brief Gets the background color of the surface.
   * @return The background color
   */
  Vector4 GetBackgroundColor();

private:

  Integration::RenderSurface* mSurface;   ///< The render surface
  Context*                    mContext;   ///< The context holding the GL state of rendering for the surface backed frame buffer

  uint32_t                    mWidth;
  uint32_t                    mHeight;
  Vector4                     mBackgroundColor;
  bool                        mSizeChanged;
  std::atomic<bool>           mIsSurfaceInvalid; ///< This is set only from the event thread and read only from the render thread
};

// Messages for FrameBuffer
inline void SetFrameBufferSizeMessage( SceneGraph::UpdateManager& updateManager, SurfaceFrameBuffer* surfaceFrameBuffer, uint32_t width, uint32_t height )
{
  typedef MessageValue2< SurfaceFrameBuffer, uint32_t, uint32_t  > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = updateManager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( surfaceFrameBuffer, &SurfaceFrameBuffer::SetSize, width, height );
}

inline void SetFrameBufferBackgroundColorMessage( SceneGraph::UpdateManager& updateManager, SurfaceFrameBuffer* surfaceFrameBuffer, const Vector4& color )
{
  typedef MessageValue1< SurfaceFrameBuffer, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = updateManager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( surfaceFrameBuffer, &SurfaceFrameBuffer::SetBackgroundColor, color );
}

} // namespace Render

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_RENDER_SURFACE_FRAME_BUFFER_H
