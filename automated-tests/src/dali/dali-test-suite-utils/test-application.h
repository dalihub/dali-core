#ifndef __DALI_TEST_APPLICATION_H__
#define __DALI_TEST_APPLICATION_H__

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
#include <test-platform-abstraction.h>
#include "test-gesture-manager.h"
#include "test-render-controller.h"
#include "test-graphics-controller.h"
#include "test-render-surface.h"
#include <dali/integration-api/scene.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/resource-policies.h>
#include <dali/integration-api/trace.h>

namespace Dali
{

class DALI_CORE_API TestApplication : public ConnectionTracker
{
public:

  // Default values derived from H2 device.
  static const uint32_t DEFAULT_SURFACE_WIDTH = 480;
  static const uint32_t DEFAULT_SURFACE_HEIGHT = 800;

#ifdef _CPP11
  static constexpr uint32_t DEFAULT_HORIZONTAL_DPI = 220;
  static constexpr uint32_t DEFAULT_VERTICAL_DPI   = 217;
#else
  static const uint32_t DEFAULT_HORIZONTAL_DPI = 220;
  static const uint32_t DEFAULT_VERTICAL_DPI   = 217;
#endif

  static const uint32_t DEFAULT_RENDER_INTERVAL = 1;

  static const uint32_t RENDER_FRAME_INTERVAL = 16;

  TestApplication( uint32_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                   uint32_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                   uint32_t horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                   uint32_t verticalDpi   = DEFAULT_VERTICAL_DPI,
                   ResourcePolicy::DataRetention policy = ResourcePolicy::DALI_DISCARDS_ALL_DATA);

  void Initialize();
  virtual ~TestApplication();
  static void LogMessage( Dali::Integration::Log::DebugPriority level, std::string& message );
  static void LogContext( bool start, const char* tag );
  Dali::Integration::Core& GetCore();
  TestPlatformAbstraction& GetPlatform();
  TestRenderController& GetRenderController();
  TestGestureManager& GetGestureManager();
  void ProcessEvent(const Integration::Event& event);
  void SendNotification();
  bool Render( uint32_t intervalMilliseconds = DEFAULT_RENDER_INTERVAL, const char* location=NULL );
  uint32_t GetUpdateStatus();
  bool UpdateOnly( uint32_t intervalMilliseconds = DEFAULT_RENDER_INTERVAL );
  bool RenderOnly( );
  void ResetContext();
  bool GetRenderNeedsUpdate();
  uint32_t Wait( uint32_t durationToWait );
  static void EnableLogging( bool enabled )
  {
    mLoggingEnabled = enabled;
  }

  Integration::Scene GetScene() const
  {
    return mScene;
  }
private:
  void DoUpdate( uint32_t intervalMilliseconds, const char* location=NULL );

protected:
  TestPlatformAbstraction             mPlatformAbstraction;
  std::unique_ptr<Test::GraphicsController> mGraphicsController;
  TestRenderController                mRenderController;
  TestGestureManager                  mGestureManager;
  TestRenderSurface*        mRenderSurface;

  Integration::UpdateStatus mStatus;
  Integration::RenderStatus mRenderStatus;

  Integration::Core* mCore;
  Dali::Integration::Scene mScene;
  uint32_t mSurfaceWidth;
  uint32_t mSurfaceHeight;
  uint32_t mFrame;

  struct { uint32_t x; uint32_t y; } mDpi;
  uint32_t mLastVSyncTime;
  ResourcePolicy::DataRetention mDataRetentionPolicy;
  static bool mLoggingEnabled;
};

} // Dali

#endif
