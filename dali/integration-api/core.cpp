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
#include <dali/integration-api/render-surface.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/debug.h>
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/internal/common/core-impl.h>


namespace Dali
{
namespace Integration
{

Core* Core::New( RenderController& renderController,
                 PlatformAbstraction& platformAbstraction,
                 Graphics::Controller& graphicsController,
                 GestureManager& gestureManager,
                 ResourcePolicy::DataRetention policy,
                 RenderToFrameBuffer renderToFboEnabled,
                 DepthBufferAvailable depthBufferAvailable,
                 StencilBufferAvailable stencilBufferAvailable )
{
  Core* instance = new Core;
  instance->mImpl = new Internal::Core( renderController,
                                        platformAbstraction,
                                        graphicsController,
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

void Core::Initialize()
{
  mImpl->Initialize();
}

void Core::SurfaceResized( Integration::RenderSurface* surface )
{
  mImpl->SurfaceResized(surface);
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
