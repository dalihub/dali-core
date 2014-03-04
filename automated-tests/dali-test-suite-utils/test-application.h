#ifndef __DALI_TEST_APPLICATION_H__
#define __DALI_TEST_APPLICATION_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include "test-platform-abstraction.h"
#include "test-gesture-manager.h"
#include "test-gl-sync-abstraction.h"
#include "test-gl-abstraction.h"
#include "test-render-controller.h"
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

class DALI_IMPORT_API TestApplication : public ConnectionTracker
{
public:

  // Default values derived from H2 device.
  static const unsigned int DEFAULT_SURFACE_WIDTH = 480;
  static const unsigned int DEFAULT_SURFACE_HEIGHT = 800;

#ifdef _CPP11
  static constexpr float DEFAULT_HORIZONTAL_DPI = 220.0f;
  static constexpr float DEFAULT_VERTICAL_DPI   = 217.0f;
#else
  static const float DEFAULT_HORIZONTAL_DPI     = 220.0f;
  static const float DEFAULT_VERTICAL_DPI       = 217.0f;
#endif

  static const unsigned int DEFAULT_RENDER_INTERVAL = 1;

  TestApplication( size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                   size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                   float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                   float  verticalDpi   = DEFAULT_VERTICAL_DPI )
  : mCore( NULL ),
    mSurfaceWidth( surfaceWidth ),
    mSurfaceHeight( surfaceHeight ),
    mFrame( 0u ),
    mDpi( horizontalDpi, verticalDpi )
  {
    Initialize();
  }

  TestApplication( bool   initialize,
                   size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                   size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                   float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                   float  verticalDpi   = DEFAULT_VERTICAL_DPI )
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

  void Initialize()
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
    unsigned int logOpts = Dali::Integration::Log::ParseLogOptions("");
    Dali::Integration::Log::InstallLogFunction(logFunction, logOpts);
  }

  virtual ~TestApplication()
  {
    Dali::Integration::Log::UninstallLogFunction();
    delete mCore;
  }

  static void LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message)
  {
    switch(level)
    {
      case Dali::Integration::Log::DebugInfo:
        tet_printf("INFO: %s", message.c_str());
        break;
      case Dali::Integration::Log::DebugWarning:
        tet_printf("WARN: %s", message.c_str());
        break;
      case Dali::Integration::Log::DebugError:
        tet_printf("ERROR: %s", message.c_str());
        break;
      case Dali::Integration::Log::DebugResources:
        tet_printf("INFO: %s", message.c_str());
        break;
      default:
        tet_printf("DEFAULT: %s", message.c_str());
        break;
    }
  }

  Dali::Integration::Core& GetCore()
  {
    return *mCore;
  }

  TestPlatformAbstraction& GetPlatform()
  {
    return mPlatformAbstraction;
  }

  TestRenderController& GetRenderController()
  {
    return mRenderController;
  }

  TestGlAbstraction& GetGlAbstraction()
  {
    return mGlAbstraction;
  }

  TestGlSyncAbstraction& GetGlSyncAbstraction()
  {
    return mGlSyncAbstraction;
  }

  TestGestureManager& GetGestureManager()
  {
    return mGestureManager;
  }

  void ProcessEvent(const Integration::Event& event)
  {
    mCore->QueueEvent(event);
    mCore->ProcessEvents();
  }

  void SendNotification()
  {
    mCore->ProcessEvents();
  }

  void SetSurfaceWidth( unsigned int width, unsigned height )
  {
    mSurfaceWidth = width;
    mSurfaceHeight = height;

    mCore->SurfaceResized( mSurfaceWidth, mSurfaceHeight );
  }

  bool Render( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL )
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

  unsigned int GetUpdateStatus()
  {
    return mStatus.KeepUpdating();
  }

  bool UpdateOnly( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL )
  {
    // Update Time values
    mPlatformAbstraction.IncrementGetTimeResult( intervalMilliseconds );
    unsigned int seconds(0u), microseconds(0u);
    mPlatformAbstraction.GetTimeMicroseconds( seconds, microseconds );

    mCore->VSync( mFrame, seconds, microseconds );
    mCore->Update( mStatus );

    return mStatus.KeepUpdating();
  }

  bool RenderOnly( )
  {
    // Update Time values
    mCore->Render( mRenderStatus );

    mFrame++;

    return mRenderStatus.NeedsUpdate();
  }

  void ResetContext()
  {
    mCore->ContextToBeDestroyed();
    mCore->ContextCreated();
  }

protected:
  TestPlatformAbstraction   mPlatformAbstraction;
  TestRenderController      mRenderController;
  TestGlAbstraction         mGlAbstraction;
  TestGlSyncAbstraction     mGlSyncAbstraction;
  TestGestureManager        mGestureManager;

  Integration::UpdateStatus mStatus;
  Integration::RenderStatus mRenderStatus;

  Integration::Core* mCore;

  unsigned int mSurfaceWidth;
  unsigned int mSurfaceHeight;
  unsigned int mFrame;

  Vector2 mDpi;
};

} // Dali

#endif
