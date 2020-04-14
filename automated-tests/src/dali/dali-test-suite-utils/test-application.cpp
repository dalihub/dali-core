/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include "test-application.h"

namespace Dali
{

bool TestApplication::mLoggingEnabled = true;

TestApplication::TestApplication( uint32_t surfaceWidth,
                                  uint32_t surfaceHeight,
                                  uint32_t  horizontalDpi,
                                  uint32_t  verticalDpi,
                                  bool initialize,
                                  bool enablePartialUpdate )
: mCore( NULL ),
  mSurfaceWidth( surfaceWidth ),
  mSurfaceHeight( surfaceHeight ),
  mFrame( 0u ),
  mDpi{ horizontalDpi, verticalDpi },
  mLastVSyncTime(0u),
  mPartialUpdateEnabled(enablePartialUpdate)
{
  if( initialize )
  {
    Initialize();
  }
}

void TestApplication::Initialize()
{
  CreateCore();
  CreateScene();
  InitializeCore();
}

void TestApplication::CreateCore()
{
  // We always need the first update!
  mStatus.keepUpdating = Integration::KeepUpdating::STAGE_KEEP_RENDERING;

  mCore = Dali::Integration::Core::New( mRenderController,
                                        mPlatformAbstraction,
                                        mGlAbstraction,
                                        mGlSyncAbstraction,
                                        mGlContextHelperAbstraction,
                                        Integration::RenderToFrameBuffer::FALSE,
                                        Integration::DepthBufferAvailable::TRUE,
                                        Integration::StencilBufferAvailable::TRUE,
                                        mPartialUpdateEnabled ? Integration::PartialUpdateAvailable::TRUE : Integration::PartialUpdateAvailable::FALSE );

  mCore->ContextCreated();

  Dali::Integration::Log::LogFunction logFunction(&TestApplication::LogMessage);
  Dali::Integration::Log::InstallLogFunction(logFunction);

  Dali::Integration::Trace::LogContextFunction logContextFunction(&TestApplication::LogContext);
  Dali::Integration::Trace::InstallLogContextFunction( logContextFunction );

  Dali::Integration::Trace::LogContext( true, "Test" );
}

void TestApplication::CreateScene()
{
  mScene = Dali::Integration::Scene::New( Size( static_cast<float>( mSurfaceWidth ), static_cast<float>( mSurfaceHeight ) ) );
  mScene.SetDpi( Vector2( static_cast<float>( mDpi.x ), static_cast<float>( mDpi.y ) ) );
}

void TestApplication::InitializeCore()
{
  mCore->SceneCreated();
  mCore->Initialize();
}

TestApplication::~TestApplication()
{
  Dali::Integration::Log::UninstallLogFunction();
  delete mCore;
}

void TestApplication::LogContext( bool start, const char* tag )
{
  if( start )
  {
    fprintf(stderr, "INFO: Trace Start: %s\n", tag);
  }
  else
  {
    fprintf(stderr, "INFO: Trace End: %s\n", tag);
  }
}

void TestApplication::LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message)
{
  if( mLoggingEnabled )
  {
    switch(level)
    {
      case Dali::Integration::Log::DebugInfo:
        fprintf(stderr, "INFO: %s", message.c_str());
        break;
      case Dali::Integration::Log::DebugWarning:
        fprintf(stderr, "WARN: %s", message.c_str());
        break;
      case Dali::Integration::Log::DebugError:
        fprintf(stderr, "ERROR: %s", message.c_str());
        break;
      default:
        fprintf(stderr, "DEFAULT: %s", message.c_str());
        break;
    }
  }
}

Dali::Integration::Core& TestApplication::GetCore()
{
  return *mCore;
}

TestPlatformAbstraction& TestApplication::GetPlatform()
{
  return mPlatformAbstraction;
}

TestRenderController& TestApplication::GetRenderController()
{
  return mRenderController;
}

TestGlAbstraction& TestApplication::GetGlAbstraction()
{
  return mGlAbstraction;
}

TestGlSyncAbstraction& TestApplication::GetGlSyncAbstraction()
{
  return mGlSyncAbstraction;
}

TestGlContextHelperAbstraction& TestApplication::GetGlContextHelperAbstraction()
{
  return mGlContextHelperAbstraction;
}

void TestApplication::ProcessEvent(const Integration::Event& event)
{
  mCore->QueueEvent(event);
  mCore->ProcessEvents();
}

void TestApplication::SendNotification()
{
  mCore->ProcessEvents();
}

void TestApplication::DoUpdate( uint32_t intervalMilliseconds, const char* location )
{
  if( GetUpdateStatus() == 0 &&
      mRenderStatus.NeedsUpdate() == false &&
      ! GetRenderController().WasCalled(TestRenderController::RequestUpdateFunc) )
  {
    fprintf(stderr, "WARNING - Update not required :%s\n", location==NULL?"NULL":location);
  }

  uint32_t nextVSyncTime = mLastVSyncTime + intervalMilliseconds;
  float elapsedSeconds = static_cast<float>( intervalMilliseconds ) * 0.001f;

  mCore->Update( elapsedSeconds, mLastVSyncTime, nextVSyncTime, mStatus, false, false );

  GetRenderController().Initialize();

  mLastVSyncTime = nextVSyncTime;
}

bool TestApplication::Render( uint32_t intervalMilliseconds, const char* location )
{
  DoUpdate( intervalMilliseconds, location );

  // Reset the status
  mRenderStatus.SetNeedsUpdate( false );
  mRenderStatus.SetNeedsPostRender( false );

  mCore->PreRender( mRenderStatus, false /*do not force clear*/, false /*do not skip rendering*/ );
  mCore->RenderScene( mRenderStatus, mScene, true /*render the off-screen buffers*/ );
  mCore->RenderScene( mRenderStatus, mScene, false /*render the surface*/ );
  mCore->PostRender( false /*do not skip rendering*/ );

  mFrame++;

  return mStatus.KeepUpdating() || mRenderStatus.NeedsUpdate();
}

bool TestApplication::PreRenderWithPartialUpdate(uint32_t intervalMilliseconds, const char* location, std::vector<Rect<int>>& damagedRects)
{
  DoUpdate(intervalMilliseconds, location);

  mCore->PreRender(mRenderStatus, false /*do not force clear*/, false /*do not skip rendering*/ );
  mCore->PreRender(mScene, damagedRects);

  return mStatus.KeepUpdating() || mRenderStatus.NeedsUpdate();
}

bool TestApplication::RenderWithPartialUpdate(std::vector<Rect<int>>& damagedRects, Rect<int>& clippingRect)
{
  mCore->RenderScene(mRenderStatus, mScene, true /*render the off-screen buffers*/, clippingRect);
  mCore->RenderScene(mRenderStatus, mScene, false /*render the surface*/, clippingRect);
  mCore->PostRender(false /*do not skip rendering*/);

  mFrame++;

  return mStatus.KeepUpdating() || mRenderStatus.NeedsUpdate();
}

uint32_t TestApplication::GetUpdateStatus()
{
  return mStatus.KeepUpdating();
}

bool TestApplication::UpdateOnly( uint32_t intervalMilliseconds  )
{
  DoUpdate( intervalMilliseconds );
  return mStatus.KeepUpdating();
}

bool TestApplication::GetRenderNeedsUpdate()
{
  return mRenderStatus.NeedsUpdate();
}

bool TestApplication::GetRenderNeedsPostRender()
{
  return mRenderStatus.NeedsPostRender();
}

bool TestApplication::RenderOnly( )
{
  // Update Time values
  mCore->PreRender( mRenderStatus, false /*do not force clear*/, false /*do not skip rendering*/ );
  mCore->RenderScene( mRenderStatus, mScene, true /*render the off-screen buffers*/ );
  mCore->RenderScene( mRenderStatus, mScene, false /*render the surface*/ );
  mCore->PostRender( false /*do not skip rendering*/ );

  mFrame++;

  return mRenderStatus.NeedsUpdate();
}

void TestApplication::ResetContext()
{
  mCore->ContextDestroyed();
  mGlAbstraction.Initialize();
  mCore->ContextCreated();
}

uint32_t TestApplication::Wait( uint32_t durationToWait )
{
  int time = 0;

  for(uint32_t i = 0; i <= ( durationToWait / RENDER_FRAME_INTERVAL); i++)
  {
    SendNotification();
    Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }
  return time;
}

} // Namespace dali
