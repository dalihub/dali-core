#ifndef TEST_REENDER_SURFACE_H
#define TEST_REENDER_SURFACE_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/render-surface.h>

namespace Dali
{

/**
 * Concrete implementation of the RenderSurface class.
 */
class DALI_CORE_API TestRenderSurface : public Dali::Integration::RenderSurface
{
public:

  /**
   * @copydoc Dali::Integration::RenderSurface::RenderSurface
   */
  TestRenderSurface( Dali::PositionSize positionSize );

  /**
   * @copydoc Dali::Integration::RenderSurface::~RenderSurface
   */
  virtual ~TestRenderSurface();

  /**
   * @copydoc Dali::Integration::RenderSurface::GetPositionSize
   */
  virtual Dali::PositionSize GetPositionSize() const;

  /**
   * @copydoc Dali::Integration::RenderSurface::GetDpi
   */
  virtual void GetDpi( unsigned int& dpiHorizontal, unsigned int& dpiVertical );

  /**
   * @copydoc Dali::Integration::RenderSurface::InitializeGraphics
   */
  virtual void InitializeGraphics();

  /**
   * @copydoc Dali::Integration::RenderSurface::CreateSurface
   */
  virtual void CreateSurface();

  /**
   * @copydoc Dali::Integration::RenderSurface::DestroySurface
   */
  virtual void DestroySurface();

  /**
   * @copydoc Dali::Integration::RenderSurface::ReplaceGraphicsSurface
   */
  virtual bool ReplaceGraphicsSurface();

  /**
   * @copydoc Dali::Integration::RenderSurface::MoveResize
   */
  virtual void MoveResize( Dali::PositionSize positionSize );

  /**
   * @copydoc Dali::Integration::RenderSurface::StartRender
   */
  virtual void StartRender();

  /**
   * @copydoc Dali::Integration::RenderSurface::PreRender
   */
  virtual bool PreRender( bool resizingSurface );

  /**
   * @copydoc Dali::Integration::RenderSurface::PostRender
   */
  virtual void PostRender( bool renderToFbo, bool replacingSurface, bool resizingSurface );

  /**
   * @copydoc Dali::Integration::RenderSurface::StopRender
   */
  virtual void StopRender();

  /**
   * @copydoc Dali::Integration::RenderSurface::ReleaseLock
   */
  virtual void ReleaseLock();

  /**
   * @copydoc Dali::Integration::RenderSurface::GetSurfaceType
   */
  virtual Dali::Integration::RenderSurface::Type GetSurfaceType();

  /**
   * @copydoc Dali::Integration::RenderSurface::MakeContextCurrent
   */
  virtual void MakeContextCurrent();

  /**
   * @copydoc Dali::Integration::RenderSurface::GetDepthBufferRequired
   */
  virtual Integration::DepthBufferAvailable GetDepthBufferRequired();

  /**
   * @copydoc Dali::Integration::RenderSurface::GetStencilBufferRequired
   */
  virtual Integration::StencilBufferAvailable GetStencilBufferRequired();

  /**
   * @copydoc Dali::Integration::RenderSurface::SetBackgroundColor
   */
  virtual void SetBackgroundColor( Vector4 color );

  /**
   * @copydoc Dali::Integration::RenderSurface::GetBackgroundColor
   */
  virtual Vector4 GetBackgroundColor();

private:

  /**
   * @brief Undefined copy constructor. RenderSurface cannot be copied
   */
  TestRenderSurface( const TestRenderSurface& rhs );

  /**
   * @brief Undefined assignment operator. RenderSurface cannot be copied
   */
  TestRenderSurface& operator=( const TestRenderSurface& rhs );

private:

  Dali::PositionSize mPositionSize;
  Vector4 mBackgroundColor;                                     ///< The background color of the surface
};

} // Dali

#endif
