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

#include "test-render-controller.h"

namespace Dali
{

TestRenderController::TestRenderController()
{
  Initialize();
}

TestRenderController::~TestRenderController()
{
}

void TestRenderController::RequestUpdate()
{
  mRequestUpdateCalled = true;
}

void TestRenderController::RequestProcessEventsOnIdle()
{
  mRequestProcessEventsOnIdleCalled = true;
}

bool TestRenderController::WasCalled(TestRenderControllerFuncEnum func)
{
  switch(func)
  {
    case RequestUpdateFunc: return mRequestUpdateCalled;
    case RequestProcessEventsOnIdleFunc: return mRequestProcessEventsOnIdleCalled;
  }

  return false;
}

void TestRenderController::Initialize()
{
  mRequestUpdateCalled = false;
  mRequestProcessEventsOnIdleCalled = false;
}


} // namespace dali
