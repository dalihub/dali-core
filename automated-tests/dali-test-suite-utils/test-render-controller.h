#ifndef __TEST_RENDER_CONTROLLER_H__
#define __TEST_RENDER_CONTROLLER_H__

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
#include <dali/integration-api/render-controller.h>

namespace Dali
{

class DALI_IMPORT_API TestRenderController : public Dali::Integration::RenderController
{
public:
  TestRenderController()
  {
    Initialize();
  }

  ~TestRenderController()
  {
  }

  virtual void RequestUpdate()
  {
    mRequestUpdateCalled = true;
  }

  virtual void RequestProcessEventsOnIdle()
  {
    mRequestProcessEventsOnIdle = true;
  }

  typedef enum
  {
    RequestUpdateFunc,
    RequestNotificationEventOnIdleFunc,
  } TestRenderControllerFuncEnum;

  bool WasCalled(TestRenderControllerFuncEnum func)
  {
    switch(func)
    {
      case RequestUpdateFunc: return mRequestUpdateCalled;
      case RequestNotificationEventOnIdleFunc: return mRequestProcessEventsOnIdle;
    }

    return false;
  }

  void Initialize()
  {
    mRequestUpdateCalled = false;
    mRequestProcessEventsOnIdle = false;
  }

private:
  bool mRequestUpdateCalled;
  bool mRequestProcessEventsOnIdle;
};

} // Dali

#endif
