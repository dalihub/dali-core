#ifndef TEST_GRAPHICS_CONTROLLER_H
#define TEST_GRAPHICS_CONTROLLER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

#include <dali/graphics-api/graphics-controller.h>
#include "test-gl-abstraction.h"
#include "test-gl-context-helper-abstraction.h"
#include "test-gl-sync-abstraction.h"

namespace Dali
{
class TestGraphicsController : public Dali::Graphics::Controller
{
public:
  TestGraphicsController()          = default;
  virtual ~TestGraphicsController() = default;

  void Initialize()
  {
    mGlAbstraction.Initialize();
  }

  Integration::GlAbstraction& GetGlAbstraction() override
  {
    return mGlAbstraction;
  }

  Integration::GlSyncAbstraction& GetGlSyncAbstraction() override
  {
    return mGlSyncAbstraction;
  }

  Integration::GlContextHelperAbstraction& GetGlContextHelperAbstraction() override
  {
    return mGlContextHelperAbstraction;
  }

private:
  TestGlAbstraction              mGlAbstraction;
  TestGlSyncAbstraction          mGlSyncAbstraction;
  TestGlContextHelperAbstraction mGlContextHelperAbstraction;
};

} // namespace Dali

#endif //TEST_GRAPHICS_CONTROLLER_H
