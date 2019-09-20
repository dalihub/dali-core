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
#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/integration-api/gl-context-helper-abstraction.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/internal/common/core-impl.h>

namespace Dali
{

namespace Integration
{

Core* Core::New( RenderController& renderController,
                 PlatformAbstraction& platformAbstraction,
                 GlAbstraction& glAbstraction,
                 GlSyncAbstraction& glSyncAbstraction,
                 GlContextHelperAbstraction& glContextHelperAbstraction,
                 ResourcePolicy::DataRetention policy,
                 RenderToFrameBuffer renderToFboEnabled,
                 DepthBufferAvailable depthBufferAvailable,
                 StencilBufferAvailable stencilBufferAvailable )
{
  Core* instance = new Core;
  instance->mImpl = new Internal::Core( renderController,
                                        platformAbstraction,
                                        glAbstraction,
                                        glSyncAbstraction,
                                        glContextHelperAbstraction,
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

ContextNotifierInterface* Core::GetContextNotifier()
{
  return mImpl->GetContextNotifier();
}

void Core::ContextCreated()
{
  mImpl->ContextCreated();
}

void Core::ContextDestroyed()
{
  mImpl->ContextDestroyed();
}

void Core::RecoverFromContextLoss()
{
  mImpl->RecoverFromContextLoss();
}

void Core::SurfaceDeleted( Integration::RenderSurface* surface )
{
  mImpl->SurfaceDeleted(surface);
}

void Core::SceneCreated()
{
  mImpl->SceneCreated();
}

void Core::QueueEvent(const Event& event)
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

void Core::Update( float elapsedSeconds, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds, UpdateStatus& status, bool renderToFboEnabled, bool isRenderingToFbo )
{
  mImpl->Update( elapsedSeconds, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds, status, renderToFboEnabled, isRenderingToFbo );
}

void Core::Render( RenderStatus& status, bool forceClear, bool uploadOnly )
{
  mImpl->Render( status, forceClear, uploadOnly );
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
