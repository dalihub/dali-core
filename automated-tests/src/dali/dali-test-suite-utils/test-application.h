#ifndef DALI_TEST_APPLICATION_H
#define DALI_TEST_APPLICATION_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/resource-policies.h>
#include <dali/integration-api/scene-pre-render-status.h>
#include <dali/integration-api/scene.h>
#include <dali/integration-api/trace.h>

#include <dali/public-api/common/dali-common.h>
#include <test-platform-abstraction.h>

#include "test-graphics-controller.h"
#include "test-render-controller.h"
#include "test-render-surface.h"

namespace Dali
{
class DALI_CORE_API TestApplication : public ConnectionTracker
{
public:
  // Default values derived from H2 device.
  static const uint32_t DEFAULT_SURFACE_WIDTH  = 480;
  static const uint32_t DEFAULT_SURFACE_HEIGHT = 800;

  static const Rect<int> DEFAULT_SURFACE_RECT;

  static constexpr uint32_t DEFAULT_HORIZONTAL_DPI = 220;
  static constexpr uint32_t DEFAULT_VERTICAL_DPI   = 217;

  static const uint32_t DEFAULT_RENDER_INTERVAL = 1;

  static const uint32_t RENDER_FRAME_INTERVAL = 16;

  TestApplication(uint32_t surfaceWidth        = DEFAULT_SURFACE_WIDTH,
                  uint32_t surfaceHeight       = DEFAULT_SURFACE_HEIGHT,
                  uint32_t horizontalDpi       = DEFAULT_HORIZONTAL_DPI,
                  uint32_t verticalDpi         = DEFAULT_VERTICAL_DPI,
                  bool     initialize          = true,
                  bool     enablePartialUpdate = false);

  void Initialize();
  void CreateCore();
  void CreateScene();
  void InitializeCore();
  ~TestApplication() override;
  static void              LogMessage(Dali::Integration::Log::DebugPriority level, std::string& message);
  static void              LogContext(bool start, const char* tag, const char* message);
  Dali::Integration::Core& GetCore();
  TestPlatformAbstraction& GetPlatform();
  TestRenderController&    GetRenderController();
  TestGraphicsController&  GetGraphicsController();

  TestGlAbstraction&              GetGlAbstraction();
  TestGraphicsSyncImplementation& GetGraphicsSyncImpl();

  void        ProcessEvent(const Integration::Event& event);
  void        SendNotification();
  bool        Render(uint32_t intervalMilliseconds = DEFAULT_RENDER_INTERVAL, const char* location = NULL, bool uploadOnly = false);
  bool        PreRenderWithPartialUpdate(uint32_t intervalMilliseconds, const char* location, std::vector<Rect<int>>& damagedRects);
  bool        RenderWithPartialUpdate(std::vector<Rect<int>>& damagedRects, Rect<int>& clippingRect);
  bool        RenderWithPartialUpdate(uint32_t intervalMilliseconds, const char* location = NULL);
  uint32_t    GetUpdateStatus();
  bool        UpdateOnly(uint32_t intervalMilliseconds = DEFAULT_RENDER_INTERVAL);
  bool        RenderOnly();
  void        ResetContext();
  bool        GetRenderNeedsUpdate();
  bool        GetRenderNeedsPostRender();
  uint32_t    Wait(uint32_t durationToWait);
  static void EnableLogging(bool enabled)
  {
    mLoggingEnabled = enabled;
  }

  Integration::Scene GetScene() const
  {
    return mScene;
  }

  void AddScene(Integration::Scene scene);
  void RemoveScene(Integration::Scene scene);

private:
  void DoUpdate(uint32_t intervalMilliseconds, const char* location = NULL, bool uploadOnly = false);

protected:
  TestPlatformAbstraction mPlatformAbstraction;
  TestRenderController    mRenderController;
  TestGraphicsController  mGraphicsController;
  TestRenderSurface*      mRenderSurface;

  Integration::UpdateStatus         mStatus;
  Integration::RenderStatus         mRenderStatus;
  Integration::ScenePreRenderStatus mScenePreRenderStatus;

  Integration::Core*              mCore;
  Dali::Integration::Scene        mScene;
  std::vector<Integration::Scene> mScenes;

  uint32_t mSurfaceWidth;
  uint32_t mSurfaceHeight;
  uint32_t mFrame;

  Graphics::UniquePtr<Graphics::RenderTarget> mRenderTarget;

  struct
  {
    uint32_t x;
    uint32_t y;
  } mDpi;
  uint32_t    mLastVSyncTime;
  bool        mPartialUpdateEnabled;
  static bool mLoggingEnabled;
};

} // namespace Dali

#endif // DALI_TEST_APPLICATION_H
