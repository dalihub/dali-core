#ifndef __DALI_TEST_APPLICATION_H__
#define __DALI_TEST_APPLICATION_H__

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

// INTERNAL INCLUDES
#include <test-platform-abstraction.h>
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
  static const float DEFAULT_HORIZONTAL_DPI = 220.0f;
  static const float DEFAULT_VERTICAL_DPI   = 217.0f;
#endif

  static const unsigned int DEFAULT_RENDER_INTERVAL = 1;

  TestApplication( size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                   size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                   float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                   float  verticalDpi   = DEFAULT_VERTICAL_DPI );

  TestApplication( bool   initialize,
                   size_t surfaceWidth  = DEFAULT_SURFACE_WIDTH,
                   size_t surfaceHeight = DEFAULT_SURFACE_HEIGHT,
                   float  horizontalDpi = DEFAULT_HORIZONTAL_DPI,
                   float  verticalDpi   = DEFAULT_VERTICAL_DPI );

  void Initialize();
  virtual ~TestApplication();
  static void LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message);
  Dali::Integration::Core& GetCore();
  TestPlatformAbstraction& GetPlatform();
  TestRenderController& GetRenderController();
  TestGlAbstraction& GetGlAbstraction();
  TestGlSyncAbstraction& GetGlSyncAbstraction();
  TestGestureManager& GetGestureManager();
  void ProcessEvent(const Integration::Event& event);
  void SendNotification();
  void SetSurfaceWidth( unsigned int width, unsigned height );
  bool Render( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL );
  unsigned int GetUpdateStatus();
  bool UpdateOnly( unsigned int intervalMilliseconds = DEFAULT_RENDER_INTERVAL );
  bool RenderOnly( );
  void ResetContext();

private:
  void DoUpdate( unsigned int intervalMilliseconds );

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
  unsigned int mLastVSyncTime;
};

} // Dali

#endif
