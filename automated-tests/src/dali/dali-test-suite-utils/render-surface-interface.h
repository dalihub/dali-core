#pragma once

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
 */

#include <dali/integration-api/core-enumerations.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/uint-16-pair.h>

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

  virtual PositionSize                              GetPositionSize() const                                                                             = 0;
  virtual void                                      GetDpi(unsigned int& dpiHorizontal, unsigned int& dpiVertical)                                      = 0;
  virtual int                                       GetSurfaceOrientation() const                                                                       = 0;
  virtual int                                       GetScreenOrientation() const                                                                        = 0;
  virtual void                                      InitializeGraphics()                                                                                = 0;
  virtual void                                      CreateSurface()                                                                                     = 0;
  virtual void                                      DestroySurface()                                                                                    = 0;
  virtual bool                                      ReplaceGraphicsSurface()                                                                            = 0;
  virtual void                                      MoveResize(Dali::PositionSize positionSize)                                                         = 0;
  virtual void                                      Resize(Dali::Uint16Pair size)                                                                       = 0;
  virtual void                                      StartRender()                                                                                       = 0;
  virtual bool                                      PreRender(bool resizingSurface, const std::vector<Rect<int>>& damageRects, Rect<int>& clippingRect) = 0;
  virtual void                                      PostRender()                                                                                        = 0;
  virtual void                                      StopRender()                                                                                        = 0;
  virtual void                                      ReleaseLock()                                                                                       = 0;
  virtual void                                      SetThreadSynchronization(ThreadSynchronizationInterface& threadSynchronization)                     = 0;
  virtual Integration::RenderSurfaceInterface::Type GetSurfaceType()                                                                                    = 0;
  virtual void                                      MakeContextCurrent()                                                                                = 0;
  virtual Integration::DepthBufferAvailable         GetDepthBufferRequired()                                                                            = 0;
  virtual Integration::StencilBufferAvailable       GetStencilBufferRequired()                                                                          = 0;

protected:
  virtual ~RenderSurfaceInterface();
};
} // namespace Integration
} // namespace Dali
