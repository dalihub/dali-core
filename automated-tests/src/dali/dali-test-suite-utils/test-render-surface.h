#ifndef TEST_RENDER_SURFACE_H
#define TEST_RENDER_SURFACE_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/core.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/rect.h>
#include <vector>

namespace Dali
{
using PositionSize = Dali::Rect<int>;
class ThreadSynchronizationInterface;

namespace Integration
{
class RenderSurfaceInterface
{
public:
  enum Type
  {
    WINDOW_RENDER_SURFACE,
    PIXMAP_RENDER_SURFACE,
    NATIVE_RENDER_SURFACE
  };
  RenderSurfaceInterface();

  virtual Dali::PositionSize                              GetPositionSize() const                                                                             = 0;
  virtual void                                            GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical)                                      = 0;
  virtual int                                             GetSurfaceOrientation() const                                                                       = 0;
  virtual int                                             GetScreenOrientation() const                                                                        = 0;
  virtual void                                            InitializeGraphics()                                                                                = 0;
  virtual void                                            CreateSurface()                                                                                     = 0;
  virtual void                                            DestroySurface()                                                                                    = 0;
  virtual bool                                            ReplaceGraphicsSurface()                                                                            = 0;
  virtual void                                            MoveResize(Dali::PositionSize positionSize)                                                         = 0;
  virtual void                                            StartRender()                                                                                       = 0;
  virtual bool                                            PreRender(bool resizingSurface, const std::vector<Rect<int>>& damageRects, Rect<int>& clippingRect) = 0;
  virtual void                                            PostRender()                                                                                        = 0;
  virtual void                                            StopRender()                                                                                        = 0;
  virtual void                                            ReleaseLock()                                                                                       = 0;
  virtual void                                            SetThreadSynchronization(ThreadSynchronizationInterface& threadSynchronization)                     = 0;
  virtual Dali::Integration::RenderSurfaceInterface::Type GetSurfaceType()                                                                                    = 0;
  virtual void                                            MakeContextCurrent()                                                                                = 0;
  virtual Integration::DepthBufferAvailable               GetDepthBufferRequired()                                                                            = 0;
  virtual Integration::StencilBufferAvailable             GetStencilBufferRequired()                                                                          = 0;

protected:
  virtual ~RenderSurfaceInterface();
};
} // namespace Integration

/**
 * Concrete implementation of the RenderSurface class.
 */
class TestRenderSurface : public Dali::Integration::RenderSurfaceInterface
{
public:
  /**
   * @copydoc Dali::Integration::RenderSurface::RenderSurface
   */
  TestRenderSurface(Dali::PositionSize positionSize);

  /**
   * @copydoc Dali::Integration::RenderSurface::~RenderSurface
   */
  ~TestRenderSurface() override = default;

  /**
   * @copydoc Dali::Integration::RenderSurface::GetPositionSize
   */
  Dali::PositionSize GetPositionSize() const override;

  /**
   * @copydoc Dali::Integration::RenderSurface::GetDpi
   */
  void GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical) override;

  /**
   * @copydoc Dali::Integration::RenderSurface::GetSurfaceOrientation
   */
  int GetSurfaceOrientation() const override;

  /**
   * @copydoc Dali::Integration::RenderSurface::GetScreenOrientation
   */
  int GetScreenOrientation() const override;

  /**
   * @copydoc Dali::Integration::RenderSurface::InitializeGraphics
   */
  void InitializeGraphics() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::CreateSurface
   */
  void CreateSurface() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::DestroySurface
   */
  void DestroySurface() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::ReplaceGraphicsSurface
   */
  bool ReplaceGraphicsSurface() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::MoveResize
   */
  void MoveResize(Dali::PositionSize positionSize) override;

  /**
   * @copydoc Dali::Integration::RenderSurface::StartRender
   */
  void StartRender() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::PreRender
   */
  bool PreRender(bool resizingSurface, const std::vector<Rect<int>>& damageRects, Rect<int>& clippingRect) override;

  /**
   * @copydoc Dali::Integration::RenderSurface::PostRender
   */
  void PostRender() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::StopRender
   */
  void StopRender() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::ReleaseLock
   */
  void ReleaseLock() override;

  void SetThreadSynchronization(ThreadSynchronizationInterface& threadSynchronization) override;
  /**
   * @copydoc Dali::Integration::RenderSurface::GetSurfaceType
   */
  Dali::Integration::RenderSurfaceInterface::Type GetSurfaceType() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::MakeContextCurrent
   */
  void MakeContextCurrent() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::GetDepthBufferRequired
   */
  Integration::DepthBufferAvailable GetDepthBufferRequired() override;

  /**
   * @copydoc Dali::Integration::RenderSurface::GetStencilBufferRequired
   */
  Integration::StencilBufferAvailable GetStencilBufferRequired() override;

private:
  /**
   * @brief Undefined copy constructor. RenderSurface cannot be copied
   */
  TestRenderSurface(const TestRenderSurface& rhs);

  /**
   * @brief Undefined assignment operator. RenderSurface cannot be copied
   */
  TestRenderSurface& operator=(const TestRenderSurface& rhs);

private:
  Dali::PositionSize mPositionSize;
  Vector4            mBackgroundColor; ///< The background color of the surface
};

} // namespace Dali

#endif // TEST_RENDER_SURFACE_H
