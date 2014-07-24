/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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


TestApplication::TestApplication( size_t surfaceWidth,
                                  size_t surfaceHeight,
                                  float  horizontalDpi,
                                  float  verticalDpi)
: mCore( NULL ),
  mSurfaceWidth( surfaceWidth ),
  mSurfaceHeight( surfaceHeight ),
  mFrame( 0u ),
  mDpi( horizontalDpi, verticalDpi )
{
  Initialize();
}

TestApplication::TestApplication( bool   initialize,
                                  size_t surfaceWidth,
                                  size_t surfaceHeight,
                                  float  horizontalDpi,
                                  float  verticalDpi )
: mCore( NULL ),
  mSurfaceWidth( surfaceWidth ),
  mSurfaceHeight( surfaceHeight ),
  mFrame( 0u ),
  mDpi( horizontalDpi, verticalDpi )
{
  if ( initialize )
  {
    Initialize();
  }
}

void TestApplication::Initialize()
{
  mCore = Dali::Integration::Core::New(
    mRenderController,
    mPlatformAbstraction,
    mGlAbstraction,
    mGlSyncAbstraction,
    mGestureManager );

  mCore->ContextCreated();
  mCore->SurfaceResized( mSurfaceWidth, mSurfaceHeight );
  mCore->SetDpi( mDpi.x, mDpi.y );

  Dali::Integration::Log::LogFunction logFunction(&TestApplication::LogMessage);
  Dali::Integration::Log::InstallLogFunction(logFunction);
}

TestApplication::~TestApplication()
{
  Dali::Integration::Log::UninstallLogFunction();
  delete mCore;
}

void TestApplication::LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message)
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

TestGestureManager& TestApplication::GetGestureManager()
{
  return mGestureManager;
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

void TestApplication::SetSurfaceWidth( unsigned int width, unsigned height )
{
  mSurfaceWidth = width;
  mSurfaceHeight = height;

  mCore->SurfaceResized( mSurfaceWidth, mSurfaceHeight );
}

bool TestApplication::Render( unsigned int intervalMilliseconds  )
{
  // Update Time values
  mPlatformAbstraction.IncrementGetTimeResult( intervalMilliseconds );
  unsigned int seconds(0u), microseconds(0u);
  mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  mCore->VSync( mFrame, seconds, microseconds );
  mCore->Update( mStatus );
  mCore->Render( mRenderStatus );

  mFrame++;

  return mStatus.KeepUpdating() || mRenderStatus.NeedsUpdate();
}

unsigned int TestApplication::GetUpdateStatus()
{
  return mStatus.KeepUpdating();
}

bool TestApplication::UpdateOnly( unsigned int intervalMilliseconds  )
{
  // Update Time values
  mPlatformAbstraction.IncrementGetTimeResult( intervalMilliseconds );
  unsigned int seconds(0u), microseconds(0u);
  mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

  mCore->VSync( mFrame, seconds, microseconds );
  mCore->Update( mStatus );

  return mStatus.KeepUpdating();
}

bool TestApplication::RenderOnly( )
{
  // Update Time values
  mCore->Render( mRenderStatus );

  mFrame++;

  return mRenderStatus.NeedsUpdate();
}

void TestApplication::ResetContext()
{
  mCore->ContextToBeDestroyed();
  mCore->ContextCreated();
}


} // Namespace dali
