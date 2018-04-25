/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/graphics/graphics.h>
#include <dali/internal/common/core-impl.h>

namespace Dali
{

namespace Integration
{

Core* Core::New( RenderController& renderController,
                 PlatformAbstraction& platformAbstraction,
                 Graphics::Graphics& graphics,
                 GestureManager& gestureManager,
                 ResourcePolicy::DataRetention policy,
                 bool renderToFboEnabled )
{
  Core* instance = new Core;
  instance->mImpl = new Internal::Core( renderController,
                                        platformAbstraction,
                                        graphics,
                                        gestureManager,
                                        policy,
                                        renderToFboEnabled );

  return instance;
}

Core::~Core()
{
  delete mImpl;
}

void Core::SurfaceResized( unsigned int width, unsigned int height )
{
  mImpl->SurfaceResized(width, height);
}

void Core::SetTopMargin( unsigned int margin )
{
  mImpl->SetTopMargin(margin);
}

void Core::SetDpi( unsigned int dpiHorizontal, unsigned int dpiVertical )
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

unsigned int Core::GetMaximumUpdateCount() const
{
  return mImpl->GetMaximumUpdateCount();
}

void Core::Update(
  float elapsedSeconds,
  unsigned int lastVSyncTimeMilliseconds,
  unsigned int nextVSyncTimeMilliseconds,
  UpdateStatus& status,
  bool renderToFboEnabled,
  bool isRenderingToFbo )
{
  mImpl->Update( elapsedSeconds, lastVSyncTimeMilliseconds, nextVSyncTimeMilliseconds, status,
                 renderToFboEnabled, isRenderingToFbo );
}

void Core::Render( RenderStatus& status )
{
  DALI_LOG_ERROR("Core::Render() called in error");
}

SystemOverlay& Core::GetSystemOverlay()
{
  return mImpl->GetSystemOverlay();
}

void Core::SetViewMode( ViewMode viewMode )
{
  mImpl->SetViewMode( viewMode );
}

ViewMode Core::GetViewMode() const
{
  return mImpl->GetViewMode();
}

void Core::SetStereoBase( float stereoBase )
{
  mImpl->SetStereoBase( stereoBase );
}

float Core::GetStereoBase() const
{
  return mImpl->GetStereoBase();
}

Core::Core()
: mImpl( NULL )
{
}

} // namespace Integration

} // namespace Dali
