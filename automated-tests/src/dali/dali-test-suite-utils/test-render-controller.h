#ifndef TEST_RENDER_CONTROLLER_H
#define TEST_RENDER_CONTROLLER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/render-controller.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
class DALI_CORE_API TestRenderController : public Dali::Integration::RenderController
{
public:
  TestRenderController();
  ~TestRenderController() override;

  void RequestUpdate() override;
  void RequestProcessEventsOnIdle() override;

  typedef enum
  {
    RequestUpdateFunc,
    RequestProcessEventsOnIdleFunc,
  } TestRenderControllerFuncEnum;

  bool WasCalled(TestRenderControllerFuncEnum func);
  void Initialize();

private:
  bool mRequestUpdateCalled;
  bool mRequestProcessEventsOnIdleCalled;
};

} // namespace Dali

#endif // TEST_RENDER_CONTROLLER_H
