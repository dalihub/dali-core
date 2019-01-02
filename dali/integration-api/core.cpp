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

// CLASS HEADER
#include <dali/integration-api/core.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/graphics/graphics-interface.h>
#include <dali/internal/common/core-impl.h>

using Dali::Integration::Graphics::GraphicsInterface;

namespace Dali
{
namespace Integration
{

Core* Core::New( RenderController& renderController,
                 PlatformAbstraction& platformAbstraction,
                 GraphicsInterface& graphics,
                 GestureManager& gestureManager,
                 ResourcePolicy::DataRetention policy,
                 RenderToFrameBuffer renderToFboEnabled,
                 DepthBufferAvailable depthBufferAvailable,
                 StencilBufferAvailable stencilBufferAvailable )
{
  Core* instance = new Core;
  instance->mImpl = new Internal::Core( renderController,
                                        platformAbstraction,
                                        graphics,
                                        gestureManager,
                                        policy,
                                        renderToFboEnabled,
                                        depthBufferAvailable,
                                        stencilBufferAvailable );

  return instance;
}

Core::~Core()
{
  delete mImpl;
}

void Core::SurfaceResized(uint32_t width, uint32_t height)
{
  mImpl->SurfaceResized(width, height);
}

void Core::SetTopMargin( uint32_t margin )
{
  mImpl->SetTopMargin(margin);
}

void Core::SetDpi( uint32_t dpiHorizontal, uint32_t dpiVertical)
{
  mImpl->SetDpi(dpiHorizontal, dpiVertical);
}

void Core::SceneCreated()
{
  mImpl->SceneCreated();
}

void Core::QueueEvent( const Event& event )
{
  mImpl->QueueEvent(event);
}

void Core::ProcessEvents()
{
  mImpl->ProcessEvents();
}

uint32_t Core::GetMaximumUpdateCount() const
{
  return mImpl->GetMaximumUpdateCount();
}

void Core::Update(
  float elapsedSeconds,
  uint32_t lastVSyncTimeMilliseconds,
  uint32_t nextVSyncTimeMilliseconds,
  UpdateStatus& status,
  bool renderToFboEnabled,
  bool isRenderingToFbo )
{
  mImpl->Update( elapsedSeconds, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds, status,
                 renderToFboEnabled, isRenderingToFbo );
}

void Core::Render( RenderStatus& status, bool forceClear )
{
  DALI_LOG_ERROR("Core::Render() called in error\n");
}

void Core::GraphicsShutdown()
{
  mImpl->GraphicsShutdown();
}

SystemOverlay& Core::GetSystemOverlay()
{
  return mImpl->GetSystemOverlay();
}

void Core::RegisterProcessor( Processor& processor )
{
  mImpl->RegisterProcessor( processor );
}

void Core::UnregisterProcessor( Processor& processor )
{
  mImpl->UnregisterProcessor( processor );
}

Core::Core()
: mImpl( NULL )
{
}

} // namespace Integration

} // namespace Dali
